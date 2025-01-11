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

void init_workers(void)
{
  if (NOF_WORKERS < 1) { return; }
  pid_t* workers_pid = shmat(SHM_WORKERS_PID_ID, NULL, 0);
  if ((pid_t*)-1 == (pid_t*)workers_pid) { FUNC_PERROR(); }
  int assigned_worker[] = {2, 0, 0, 0, 0, 0};
  // int assigned_worker[SERV_NUM];
  // utils_assign_count_array(assigned_worker, SERV_NUM, NOF_WORKERS);
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
      else { workers_pid[worker_count++] = pid; }
    }
  }
  if (-1 == shmdt(workers_pid)) { FUNC_PERROR(); }
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
}

void init_ticket_dispenser(void)
{
  pid_t* ticket_disp_pid = shmat(SHM_TICKET_DISPENSER_PID_ID, NULL, 0);
  if ((pid_t*)-1 == (pid_t*)ticket_disp_pid) { FUNC_PERROR(); }
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
  else { *ticket_disp_pid = pid; }
  if (-1 == shmdt(ticket_disp_pid)) { FUNC_PERROR(); }
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
  ftok_key_init();
  sem_init();
  shm_init();
  msg_init();
  // int assigned_serv_seats[SERV_NUM];
  int assigned_serv_seats[] = {1, 0, 0, 0, 0, 0};
  // utils_assign_count_array(assigned_serv_seats, SERV_NUM, NOF_WORKER_SEATS);
  seats_init_resources(assigned_serv_seats);
  init_processes();
}

void start(void)
{
  if (-1 == lock_sem_val(SEM_PROC_READY_ID, 0, START_SEM_COUNT)) { FUNC_PERROR(); }
  if (-1 == set_sem_val(SEM_START_ID, 0, START_SEM_COUNT)) { FUNC_PERROR(); }
}

void core(void)
{
  if (-1 == lock_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
}

int main(int argc, char* argv[])
{
  if (1 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  setup();
  while (1)
  {
    log_trace("\n");
    start();
    core();
  }
  return 0;
}
