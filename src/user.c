#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include "macros.h"
#include "struct.h"
#include "msg.h"
#include "sem.h"
#include "shm.h"
#include "user.h"
#include "config.h"
#include "calendar.h"
#include "notification.h"

int id;
int P_SERV;

void send_notific_clock(int req_times[], Service* serv_req, int nof_req)
{
  size_t clock_req_size = get_notifc_size(CLOCK_REQ);
  ClockReq* clock_req = (ClockReq*)malloc(clock_req_size + sizeof(long));
  if (NULL == clock_req) { FUNC_PERROR(); }
  size_t times_size = sizeof(int) * (size_t)nof_req;
  size_t serv_size = sizeof(Service) * (size_t)nof_req;
  for (int i = 0; i < nof_req; i++)
  {
    log_trace("user %d S clock_req -> serv: %d time: %d", id, serv_req[i], req_times[i]);
  }
  if (nof_req > 0)
  {
    memcpy(clock_req->data, req_times, times_size);
    memcpy(clock_req->data + times_size, serv_req, serv_size);
  }
  clock_req->mtype = CLOCK_REQ;
  if (nof_req > 0)
  {
    clock_req->msg_id = MSG_NOTIFY_USER_IDS[id];
    clock_req->sem_count = id;
  }
  clock_req->times_size = times_size;
  clock_req->serv_req_size = serv_size;
  if (-1 == msgsnd(MSG_NOTIFY_CLOCK_ID, clock_req, clock_req_size, 0))
  {
    FUNC_PERROR();
  }
}

void send_ticket_request(Service serv)
{
  TicketReq ticket_req = {0};
  size_t msg_size = get_notifc_size(TICKET_REQ);
  ticket_req.mtype = TICKET_REQ;
  ticket_req.user_msg_id = MSG_NOTIFY_USER_IDS[id];
  ticket_req.user_sem_count = id;
  ticket_req.serv = serv;
  msgsnd(MSG_NOTIFY_DISPENSER_ID, &ticket_req, msg_size, 0);
  release_sem(SEM_NOTIFY_DISPENSER_ID, 0);
}

void send_serv_request(TicketResp* ticket_resp)
{
  ServiceReq service_req = {0};
  service_req.mtype = SERVICE_REQ;
  service_req.serv = ticket_resp->serv;
  service_req.user_msg_id = MSG_NOTIFY_USER_IDS[id];
  service_req.user_sem_id = SEM_NOTIFY_USER_ID;
  service_req.user_sem_count = id;
  size_t notifc_size = get_notifc_size(SERVICE_REQ);
  msgsnd(ticket_resp->worker_msg_id, &service_req, notifc_size, 0);
  if (-1 == release_sem(SEM_NOTIFY_WORKER_ID, ticket_resp->worker_sem_count)) { FUNC_PERROR(); }
}

void calc_times_from_serv(int all_req_times[], Service serv_req[], int req_time, int nof_req)
{
  for (int i = 0; i < nof_req; i++)
  {
    if (req_time >= MINUTES_IN_DAY) { req_time = MINUTES_IN_DAY - 1; }
    all_req_times[i] = req_time;
    req_time = req_time + get_serv_duration(&serv_req[i], 1);
  }
}

void setup_clock_notifc(void)
{
  if ((rand() % 100) + 1 > P_SERV)
  {
    send_notific_clock(NULL, NULL, 0);
    return;
  }
  int nof_req = (rand() % N_REQUESTS) + 1;
  Service serv_req[nof_req];
  int all_req_times[nof_req];
  for (int i = 0; i < nof_req; i++) { serv_req[i] = (Service)(rand() % SERV_NUM); }
  int req_time = (int)(rand() % (8 * 60));
  int opt_time = find_best_time(req_time, serv_req, nof_req);
  calc_times_from_serv(all_req_times, serv_req, opt_time, nof_req);
  send_notific_clock(all_req_times, serv_req, nof_req);
}

