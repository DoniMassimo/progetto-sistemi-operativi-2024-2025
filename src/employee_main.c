#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ftok_key.h"
#include "ipc_config.h"
#include "macros.h"
#include "seats.h"
#include "sem.h"
#include "sem_utils.h"
#include "shm.h"
#include "utils.h"

int assigned_service;

void setup(char arg_1[])
{
  char* endptr;
  assigned_service = (int)strtol(arg_1, &endptr, 10);
  if (*endptr != '\0') { FUNC_MSG_ERROR("Cant convert argv[1] to int."); }
  config_load();
  ftok_key_init();
  sem_config();
  shm_config();
}

void start(void)
{
  int sem_op_res = lock_sem(SEM_START_ID, 0);
  if (-1 == sem_op_res) { FUNC_MSG_PERROR("sem empl"); }
}

void core(void)
{
  int sem_op_res = seats_try_take_seat(assigned_service);
  if (-1 == sem_op_res) { FUNC_MSG_PERROR("sem empl"); }
  printf("servizio trovato %d\n", assigned_service);
  fflush(stdout);
}

int main(int argc, char* argv[])
{
  if (2 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  setup(argv[1]);
  start();
  core();
  return 0;
}
