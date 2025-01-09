#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "seats.h"
#include "notification.h"
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

void core(void)
{
  int nof_notifc = 3;
  MesType notifc_filter[] = {DAY_ENDED, TICKET_REQ};
  void* notifc = NULL;
  while (1)
  {
    if (notifc != NULL) { free(notifc); }
    MesType notification = get_notifications(notifc_filter, nof_notifc, MSG_NOTIFY_DISPENSER_ID,
                                 SEM_NOTIFY_DISPENSER_ID, 0, &notifc);
    if (DAY_ENDED == notification)
    {
      free(notifc);
      return;
    }
    else if (TICKET_REQ == notification)
    {
      TicketReq* ticket_req = (TicketReq*)notifc;
      Service asked_serv = (Service)ticket_req->serv;
      log_trace("ticke disp richiesta ticket -> serv: %d, user: %d", asked_serv,
                ticket_req->sem_count);
      SeatInfo seat_info = {0};
      int service_available = seats_get_less_worker(asked_serv, &seat_info);
      TicketResp ticket_resp = {0};
      ticket_resp.mtype = TICKET_RESP;
      ticket_resp.worker_sem_count = seat_info.sem_notify_worker_count;
      ticket_resp.worker_msg_id = seat_info.msg_notify_worker_id;
      ticket_resp.serv = asked_serv;
      if (1 == service_available) { ticket_resp.status = 1; }
      else { ticket_resp.status = 0; }
      if (-1 == msgsnd(ticket_req->msg_id, &ticket_resp, get_notifc_size(TICKET_RESP), 0))
      {
        FUNC_PERROR();
      }
      if (-1 == release_sem(SEM_NOTIFY_USER_ID, ticket_req->sem_count)) { FUNC_PERROR(); }
    }
  }
}

int main(void)
{
  setup();
  while (1)
  {
    start();
    core();
  }
}
