#include <sys/types.h> 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "seats.h"
#include "utils.h"
#include "macros.h"
#include "config.h"
#include "ipc_config.h"
#include "sem_utils.h"
#include "ftok_key.h"
#include "shm.h"
#include "sem.h"

void init_workers(void)
{
  int assigned_worker[SERV_NUM];
  utils_assign_count_array(assigned_worker, SERV_NUM, NOF_WORKERS);
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
        char dir[MAX_PATH_LEN + MAX_EXE_LEN];
        strcpy(dir, REL_DIR);
        strcat(dir, "employee_main");
        char* args[] = {dir, i_str, NULL};
        if (execv(args[0], args) == -1) { FUNC_PERROR(); }
      }
    }
  }
}

void init_users(void)
{
  for (int i = 0; i < NOF_USERS; i++)
  {
    pid_t pid = fork();
    if (-1 == pid) { FUNC_PERROR(); }
    else if (0 == pid)
    {
      char dir[MAX_PATH_LEN + MAX_EXE_LEN];
      strcpy(dir, REL_DIR);
      strcat(dir, "user_main");
      char* args[] = {dir, NULL};
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
    strcat(dir, "clock");
    char* args[] = {dir, NULL};
    if (execv(args[0], args) == -1) { FUNC_PERROR(); }
  }
}

void init_processes(void)
{
  init_workers();
  // init_clock();
  // init_users();
}

int main(int argc, char* argv[])
{
  if (1 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  config_load();
  ftok_key_init();
  sem_init();
  shm_init();
  int assigned_serv_seats[SERV_NUM];
  utils_assign_count_array(assigned_serv_seats, SERV_NUM, NOF_WORKER_SEATS);
  seats_init_resources(assigned_serv_seats);
  init_processes();
  int ope_res = set_sem_val(SEM_START_ID, 0, START_SEM_COUNT);
  if (ope_res < 0) { FUNC_PERROR(); }
  return 0;
}
