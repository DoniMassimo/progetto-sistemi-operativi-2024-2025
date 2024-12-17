#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sem_utils.h"
#include "macros.h"
#include "config.h"

int main(int argc, char* argv[])
{
  config_init();
  if (2 != argc) { MSG_ERROR("agrc error"); }
  char* endptr;
  int assigned_service = (int)strtol(argv[1], &endptr, 10);
  if (*endptr != '\0') { FUNC_MSG_ERROR("Cant convert argv[1] to int."); }
  printf("em inizia %d\n", assigned_service);
  fflush(stdout);
  int sem_op_res = lock_sem(START_SEM_ID, 0);
  if (-1 == sem_op_res) { FUNC_MSG_PERROR("sem empl"); }
  sem_op_res = lock_sem(SEM_SEATS_ID, assigned_service);
  if (-1 == sem_op_res) { FUNC_MSG_PERROR("sem empl"); }
  printf("servizio trovato %d\n", assigned_service);
  fflush(stdout);
  return 0;
}
