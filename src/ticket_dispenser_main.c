#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "seats.h"
#include "log.h"
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
  int msg_id;
  int sem_count;
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
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
}

MesType get_notifications(ComStruct* com_struct)
{
  if (-1 == lock_sem(SEM_NOTIFY_DISPENSER_ID, 0)) { FUNC_PERROR(); }
  if (-1 == msgrcv(MSG_NOTIFY_DISPENSER_ID, com_struct, sizeof(Content), DAY_ENDED, IPC_NOWAIT))
  {
    if (ENOMSG != errno) { FUNC_PERROR(); }
  }
  else { return DAY_ENDED; }
  if (-1 == msgrcv(MSG_NOTIFY_DISPENSER_ID, com_struct, sizeof(Content), TICKET_REQ, IPC_NOWAIT))
  {
    if (ENOMSG != errno) { FUNC_PERROR(); }
  }
  else { return TICKET_REQ; }
  MSG_ERROR("Expect to find message\n");
}

void core(void)
{
  ComStruct com_struct = {0};
  while (1)
  {
    MesType notification = get_notifications(&com_struct);
    if (DAY_ENDED == notification)
    {
      log_info("day ended");
      return;
    }
    else if (TICKET_REQ == notification)
    {
      Service asked_serv = (Service)com_struct.content.ticket_cont.info;
      SeatInfo seat_info = {0};
      int service_available = seats_get_less_worker(asked_serv, &seat_info);
      ComStruct resp_struct = {0};
      resp_struct.mtype = TICKET_RESP;
      if (service_available) { resp_struct.content.ticket_cont.info = 1; }
      else { resp_struct.content.ticket_cont.info = 0; }
      if (-1 == msgsnd(com_struct.content.ticket_cont.msg_id, &resp_struct, sizeof(Content), 0))
      {
        FUNC_PERROR();
      }
      if (-1 == release_sem(SEM_NOTIFY_USER_ID, com_struct.content.ticket_cont.sem_count))
      {
        FUNC_PERROR();
      }
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
