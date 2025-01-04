#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "seats.h"
#include "utils.h"
#include "macros.h"
#include "config.h"
#include "sem_utils.h"
#include "ftok_key.h"
#include "shm.h"
#include "sem.h"
#include "msg.h"
#include "struct.h"

int id;

void setup(char arg_1[])
{
  char* endptr;
  id = (int)strtol(arg_1, &endptr, 10);
  if (*endptr != '\0') { FUNC_MSG_ERROR("Cant convert argv[1] to int."); }
  config_load();
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
}

void start(void)
{
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
}

void core(void)
{
  ComStruct com_struct = {0};
  printf("user id -> %d\n", id);
  while (1)
  {
    if (-1 == lock_sem(SEM_NOTIFY_USER_ID, id)) { FUNC_PERROR(); }
    if (-1 == msgrcv(MSG_NOTIFY_USER_IDS[id], &com_struct, sizeof(Content), DAY_ENDED, IPC_NOWAIT))
    {
      FUNC_PERROR();
    }
    else
    {
      printf("user %d -> finisco giornata\n", id);
      return;
    }
  }
}

int main(int argc, char* argv[])
{
  if (2 != argc) { MSG_ERROR("agrc error"); }
  setup(argv[1]);
  while (1)
  {
    start();
    core();
  }
  return 0;
}
