#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>
#include "seats.h"
#include "clock.h"
#include "utils.h"
#include "macros.h"
#include "config.h"
#include "ipc_config.h"
#include "sem_utils.h"

void init_workers()
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

void init_users()
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

void init_clock()
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

void init_processes()
{
  init_workers();
  init_clock();
  // init_users();
}

int main(int argc, char* argv[])
{
  if (1 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  config_load();
  ipc_config_init();
  int assigned_serv_seats[SERV_NUM];
  utils_assign_count_array(assigned_serv_seats, SERV_NUM, NOF_WORKER_SEATS);
  seats_init_resources(assigned_serv_seats);
  init_processes();
  int ope_res = set_sem_val(SEM_START_ID, 0, START_SEM_COUNT);
  wait(NULL);
  if (ope_res < 0) { FUNC_PERROR(); }
  return 0;
}
