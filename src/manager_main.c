#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "log.h"
#include "seats.h"
#include "utils.h"
#include "macros.h"
#include "config.h"
#include "sem_utils.h"
#include "ftok_key.h"
#include "shm.h"
#include "sem.h"
#include "msg.h"
#include "stats_handler.h"

pid_t* all_proc_pid = NULL;
size_t nof_proc = 0;
int user_added = 0;

void init_workers(void)
{
  if (NOF_WORKERS < 1) { return; }
  int assigned_worker[SERV_NUM];
  utils_assign_count_array(assigned_worker, SERV_NUM, NOF_WORKERS);
  for (int i = 0; i < SERV_NUM; i++)
  {
    log_trace("manager -> %d worker for serv: %d", assigned_worker[i], i);
  }
  int worker_count = 0;
  for (int i = 0; i < SERV_NUM; i++)
  {
    for (int j = 0; j < assigned_worker[i]; j++)
    {
      pid_t pid = fork();
      if (-1 == pid) { FUNC_PERROR(); }
      else if (0 == pid)
      {
        char i_str[20];
        sprintf(i_str, "%d", i);
        char id[12];
        sprintf(id, "%d", worker_count);
        char dir[MAX_PATH_LEN + MAX_EXE_LEN];
        strcpy(dir, REL_DIR);
        strcat(dir, "worker_main");
        char* args[] = {dir, i_str, id, NULL};
        if (execv(args[0], args) == -1) { FUNC_PERROR(); }
      }
      else
      {
        worker_count++;
        all_proc_pid[nof_proc++] = pid;
      }
    }
  }
}

void init_new_users(void)
{
  for (int i = NOF_USERS; i < NOF_USERS + N_NEW_USERS; i++)
  {
    pid_t pid = fork();
    if (-1 == pid) { FUNC_PERROR(); }
    else if (0 == pid)
    {
      char id[12];
      sprintf(id, "%d", i);
      char dir[MAX_PATH_LEN + MAX_EXE_LEN];
      strcpy(dir, REL_DIR);
      strcat(dir, "user_main");
      char* args[] = {dir, id, NULL};
      if (execv(args[0], args) == -1) { FUNC_PERROR(); }
    }
    else { all_proc_pid[nof_proc++] = pid; }
  }
  add_new_users();
}

void init_users(void)
{
  for (int i = 0; i < NOF_USERS; i++)
  {
    pid_t pid = fork();
    if (-1 == pid) { FUNC_PERROR(); }
    else if (0 == pid)
    {
      char id[12];
      sprintf(id, "%d", i);
      char dir[MAX_PATH_LEN + MAX_EXE_LEN];
      strcpy(dir, REL_DIR);
      strcat(dir, "user_main");
      char* args[] = {dir, id, NULL};
      if (execv(args[0], args) == -1) { FUNC_PERROR(); }
    }
    else { all_proc_pid[nof_proc++] = pid; }
  }
}

void init_clock(void)
{
  pid_t pid = fork();
  if (-1 == pid) { FUNC_PERROR(); }
  else if (0 == pid)
  {
    char dir[MAX_PATH_LEN + MAX_EXE_LEN];
    strcpy(dir, REL_DIR);
    strcat(dir, "clock_main");
    char* args[] = {dir, NULL};
    if (execv(args[0], args) == -1) { FUNC_PERROR(); }
  }
  else { all_proc_pid[nof_proc++] = pid; }
}

void init_ticket_dispenser(void)
{
  pid_t pid = fork();
  if (-1 == pid) { FUNC_PERROR(); }
  else if (0 == pid)
  {
    char dir[MAX_PATH_LEN + MAX_EXE_LEN];
    strcpy(dir, REL_DIR);
    strcat(dir, "ticket_dispenser_main");
    char* args[] = {dir, NULL};
    if (execv(args[0], args) == -1) { FUNC_PERROR(); }
  }
  else { all_proc_pid[nof_proc++] = pid; }
}

void init_processes(void)
{
  init_workers();
  init_clock();
  init_users();
  init_ticket_dispenser();
}

void setup(void)
{
  config_load();
  size_t all_proc_num = (size_t)(START_SEM_COUNT + N_NEW_USERS);
  all_proc_pid = (pid_t*)malloc(sizeof(pid_t) * all_proc_num);
  if (NULL == all_proc_pid) { FUNC_PERROR(); }
  ftok_key_init();
  sem_init();
  shm_init();
  msg_init();
  int assigned_serv_seats[SERV_NUM];
  utils_assign_count_array(assigned_serv_seats, SERV_NUM, NOF_WORKER_SEATS);
  seats_init_resources(assigned_serv_seats);
  init_processes();
  init_stats();
}

void start(void)
{
  if (-1 == lock_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
  if (-1 == wait_zero_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
  int semop_add_user = lock_sem_nowait(SEM_ADD_USERS_ID, 0);
  if (-1 == semop_add_user) { FUNC_PERROR(); }
  else if (-2 == semop_add_user) { release_sem_val(SEM_CLOCK_ADD_USERS_ID, 0, 1); }
  else
  {
    log_trace("manager -> %d users added", N_NEW_USERS);
    user_added = 1;
    init_new_users();
    release_sem_val(SEM_CLOCK_ADD_USERS_ID, 0, 2);
  }
  if (-1 == lock_sem_val(SEM_PROC_READY_ID, 0, START_SEM_COUNT)) { FUNC_PERROR(); }
  if (-1 == set_sem_val(SEM_START_ID, 0, START_SEM_COUNT)) { FUNC_PERROR(); }
}

void core(void)
{
}

int main(int argc, char* argv[])
{
  if (1 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  log_trace("%s", REL_DIR);
  setup();
  int day_count = 0;
  while (1)
  {
    if (day_count >= SIM_DURATION) { break; }
    log_trace("\n");
    start();
    core();
    get_stats(NOF_USERS * SERV_NUM + NOF_WORKERS, day_count);
    day_count++;
    print_stats(day_count);
  }
  release_sem_val(SEM_PROC_CAN_DIE_ID, 0, START_SEM_COUNT);
  for (size_t i = 0; i < nof_proc; i++)
  {
    int status;
    pid_t child_pid = waitpid(-1, &status, 0);
    if (-1 == child_pid) { FUNC_PERROR(); }
  }
  free(all_proc_pid);
  sem_deallocate();
  shm_deallocate();
  msg_deallocate(user_added);
  return 0;
}
