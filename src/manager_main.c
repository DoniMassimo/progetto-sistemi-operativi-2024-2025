#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>
#include "utils.h"
#include "macros.h"
#include "config.h"
#include "sem_utils.h"

static int start_sem_id;

void assign_count_array(int* count_ar, int ar_size, int count)
{
  int min_seats = count / ar_size;
  int seats_left = count - (min_seats * ar_size);
  for (int i = 0; i < ar_size; i++)
  {
    count_ar[i] = (int)min_seats;
    if (seats_left-- > 0) { count_ar[i]++; }
  }
}

void init_resources()
{
  int ope_res = init_sem_zero(START_SEM_ID, 0);
  if (start_sem_id < 0) { FUNC_PERROR(); }
  int assigned_seats[SERV_NUM];
  assign_count_array(assigned_seats, SERV_NUM, NOF_WORKER_SEATS);
  for (int i = 0; i < SERV_NUM; i++)
  {
    int ope_res = set_sem_val(SEM_SEATS_ID, i, assigned_seats[i]);
    if (ope_res < 0) { FUNC_PERROR(); }
  }
}

void init_workers()
{
  int assigned_worker[SERV_NUM];
  assign_count_array(assigned_worker, SERV_NUM, NOF_WORKER_SEATS);
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

void init_processes()
{
  init_workers();
  init_users();
}

int main(int argc, char* argv[])
{
  config_init();
  if (1 != argc) { MSG_ERROR("agrc error"); }
  get_relative_path(argv[0], REL_DIR);
  init_resources();
  init_processes();
  int ope_res = set_sem_val(START_SEM_ID, 0, START_SEM_COUNT);
  if (ope_res < 0) { FUNC_PERROR(); }
  return 0;
}
