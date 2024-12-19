#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "utils.h"
#include "seats.h"
#include "sem_utils.h"
#include "macros.h"
#include "config.h"
#include "ipc_config.h"

int main(int argc, char* argv[])
{
  if (2 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  config_load();
  ipc_config_init_worker();
  char* endptr;
  int assigned_service = (int)strtol(argv[1], &endptr, 10);
  if (*endptr != '\0') { FUNC_MSG_ERROR("Cant convert argv[1] to int."); }
  int sem_op_res = lock_sem(SEM_START_ID, 0);
  if (-1 == sem_op_res) { FUNC_MSG_PERROR("sem empl"); }
  printf("em inizia %d\n", assigned_service);
  fflush(stdout);
  sem_op_res = seats_try_take_seat(assigned_service);
  if (-1 == sem_op_res) { FUNC_MSG_PERROR("sem empl"); }
  printf("servizio trovato %d\n", assigned_service);
  fflush(stdout);
  return 0;
}
