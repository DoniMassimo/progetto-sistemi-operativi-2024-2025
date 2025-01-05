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

void req_test()
{
  ComStruct test = {0};
  test.mtype = TICKET_REQ;
  test.content.ticket_cont.info = PAY_POST_BULL;
  test.content.ticket_cont.msg_response_id = MSG_NOTIFY_USER_IDS[id];
  test.content.ticket_cont.sem_response_count = id;
  if (-1 == msgsnd(MSG_NOTIFY_DISPENSER_ID, &test, sizeof(Content), 0)) { FUNC_PERROR(); }
  release_sem(SEM_NOTIFY_DISPENSER_ID, 0);
}

void start(void)
{
  printf("user %d -> start\n", id);
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
  req_test();
}

MesType get_notifications(ComStruct* com_struct)
{
  if (-1 == lock_sem(SEM_NOTIFY_USER_ID, id)) { FUNC_PERROR(); }
  if (-1 == msgrcv(MSG_NOTIFY_USER_IDS[id], com_struct, sizeof(Content), DAY_ENDED, IPC_NOWAIT))
  {
    if (ENOMSG != errno) { FUNC_PERROR(); }
  }
  else { return DAY_ENDED; }
  if (-1 == msgrcv(MSG_NOTIFY_USER_IDS[id], com_struct, sizeof(Content), TICKET_RESP, IPC_NOWAIT))
  {
    if (ENOMSG != errno) { FUNC_PERROR(); }
  }
  else { return TICKET_RESP; }
  FUNC_MSG_ERROR("Expect to find message\n");
}

void core(void)
{
  ComStruct com_struct = {0};
  while (1)
  {
    MesType notification = get_notifications(&com_struct);
    if (DAY_ENDED == notification)
    {
      printf("user %d -> finisco giornata\n", id);
      return;
    }
    else if (TICKET_RESP == notification)
    {
      printf("ticket resp -> %d\n", com_struct.content.ticket_cont.info);
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
