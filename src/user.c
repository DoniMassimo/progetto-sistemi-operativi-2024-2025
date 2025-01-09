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

int id;
int P_SERV;

void send_notific_clock(int req_times[], Service* serv_req, int nof_req)
{
  size_t notific_size = sizeof(ClockReq) + (sizeof(Service) + sizeof(int)) * (size_t)nof_req;
  ClockReq* notification = (ClockReq*)malloc(notific_size);
  if (NULL == notification) { FUNC_PERROR(); }
  size_t times_size = sizeof(int) * (size_t)nof_req;
  size_t serv_size = sizeof(Service) * (size_t)nof_req;
  for (int i = 0; i < nof_req; i++)
  {
    log_trace("user %d invia richiesta -> serv: %d time: %d", id, serv_req[i], req_times[i]);
  }
  if (nof_req > 0)
  {
    memcpy(notification->data, req_times, times_size);
    memcpy(notification->data + times_size, serv_req, serv_size);
  }
  notification->mtype = CLOCK_REQ;
  if (nof_req > 0)
  {
    notification->msg_id = MSG_NOTIFY_USER_IDS[id];
    notification->sem_count = id;
  }
  notification->times_size = times_size;
  notification->serv_req_size = serv_size;
  if (-1 == msgsnd(MSG_NOTIFY_CLOCK_ID, notification, notific_size - sizeof(long), 0))
  {
    FUNC_PERROR();
  }
}

void send_ticket_request(Service serv)
{
  ComStruct ticket_req = {0};
  ticket_req.mtype = TICKET_REQ;
  ticket_req.content.msg_id = MSG_NOTIFY_USER_IDS[id];
  ticket_req.content.sem_count = id;
  ticket_req.content.info = (int)serv;
  msgsnd(MSG_NOTIFY_DISPENSER_ID, &ticket_req, sizeof(Content), 0);
  release_sem(SEM_NOTIFY_DISPENSER_ID, 0);
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
