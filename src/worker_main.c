#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "seats.h"
#include "sem.h"
#include "sem_utils.h"
#include "shm.h"
#include "utils.h"
#include "msg.h"
#include "struct.h"

int assigned_service;
int id;

void setup(char arg_1[], char arg_2[])
{
  char* endptr;
  assigned_service = (int)strtol(arg_1, &endptr, 10);
  if (*endptr != '\0') { FUNC_MSG_ERROR("Cant convert argv[1] to int."); }
  id = (int)strtol(arg_2, &endptr, 10);
  if (*endptr != '\0') { FUNC_MSG_ERROR("Cant convert argv[2] to int."); }
  printf("mio id: %d\n", id);
  config_load();
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
}

void start(void)
{
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_MSG_PERROR("sem empl"); }
}

void core(void)
{
  int outcome = seats_try_take_seat(assigned_service, id);
  ComStruct com_struct = {0};
  int recived_msg = -1;
  while (1)
  {
    if (-1 == lock_sem(SEM_NOTIFY_WORKER_ID, id)) { FUNC_PERROR(); }
    if (-1 ==
        msgrcv(MSG_NOTIFY_WORKER_IDS[id], &com_struct, sizeof(Content), DAY_ENDED, IPC_NOWAIT))
    {

      if (ENOMSG != errno) { FUNC_PERROR(); }
    }
    else
    {
      printf("worker %d -> finisco giornata\n", id);
      return;
    }
  }
}

int main(int argc, char* argv[])
{
  if (3 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  setup(argv[1], argv[2]);
  while (1)
  {
    start();
    core();
  }
  return 0;
}
