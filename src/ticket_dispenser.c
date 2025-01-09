#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include "macros.h"
#include "struct.h"
#include "msg.h"
#include "sem.h"
#include "shm.h"
#include "ticket_dispenser.h"
#include "config.h"
#include "calendar.h"
#include "notification.h"
#include "seats.h"

void handle_ticket_req(TicketReq* ticket_req)
{
  Service asked_serv = (Service)ticket_req->serv;
  log_trace("ticke disp richiesta ticket -> serv: %d, user: %d", asked_serv, ticket_req->sem_count);
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
