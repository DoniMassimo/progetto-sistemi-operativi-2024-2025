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

typedef struct
{
  int status;
  int msg_notify_worker_id;
  int sem_notify_worker_count;
} Ticket;

typedef struct
{
  long mtype;
  Ticket ticket;
} DispenserMsg;

void setup(void)
{
  config_load();
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
}

void start(void)
{
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_MSG_PERROR("t rel"); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_MSG_PERROR("t start"); }
}

void core(void)
{
  ComStruct com_struct = {0};
  while (1)
  {
    if (-1 == lock_sem(SEM_NOTIFY_DISPENSER_ID, 0)) { FUNC_PERROR(); }
    if (-1 == msgrcv(MSG_NOTIFY_DISPENSER_ID, &com_struct, sizeof(Content), DAY_ENDED, IPC_NOWAIT))
    {
      if (ENOMSG != errno) { FUNC_PERROR(); }
    }
    else
    {
      printf("ticket -> finisco giornata\n");
      return;
    }
    if (-1 == msgrcv(MSG_NOTIFY_DISPENSER_ID, &com_struct, sizeof(Content), TICKET_REQ, IPC_NOWAIT))
    {
      if (ENOMSG != errno) { FUNC_PERROR(); }
    }
    else
    {
      // eroga ticker
    }
  }
}

int main(int argc, char* argv[])
{
  setup();
  while (1)
  {
    start();
    core();
  }
}
