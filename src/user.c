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
UserStats* user_stats[SERV_NUM];
size_t wait_time_index[SERV_NUM];
long curr_stats_id = 1;
int rem_serv_req[SERV_NUM];

void setup_user_stats(void)
{
  for (int i = 0; i < SERV_NUM; i++)
  {
    user_stats[i] = (UserStats*)realloc(user_stats[i], sizeof(UserStats));
    if (NULL == user_stats[i]) { FUNC_PERROR(); }
    user_stats[i]->serv = (Service)i;
    user_stats[i]->completed_serv = 0;
    user_stats[i]->failed_serv = 0;
    wait_time_index[i] = 0;
  }
}

void user_clear_msg_queue(void)
{
  void* notifc = NULL;
  GetNotfParam get_notf_param = {0};
  user_set_notf_param(&get_notf_param, &notifc);
  get_notf_param.nowait = 1;
  get_notf_param.can_skip = 1;
  int count_rem = 0;
  MesType notification = get_notifications(&get_notf_param);
  while (notification != NO_MES)
  {
    count_rem++;
    notification = get_notifications(&get_notf_param);
  }
  struct msqid_ds buf;
  if (msgctl(MSG_NOTIFY_USER_IDS[id], IPC_STAT, &buf) == -1) { FUNC_PERROR(); }
  if (buf.msg_qnum > 0)
  {
    MesType notification = get_notifications(&get_notf_param);
    log_fatal("user: %d -> Unexpected msg", id);
    MSG_ERROR("Unexpected msg");
  }
  log_trace("user: %d -> clear_msg: %d", id, count_rem);
  if (-1 == init_sem_zero(SEM_NOTIFY_USER_ID, id)) { FUNC_PERROR(); }
}

void add_completed_serv(Service serv)
{
  user_stats[serv]->completed_serv++;
}

void add_waiting_time(Service serv, int time)
{
  if (wait_time_index[serv] >= user_stats[serv]->nof_waiting_times)
  {
    if (0 == user_stats[serv]->nof_waiting_times) { user_stats[serv]->nof_waiting_times = 2; }
    else { user_stats[serv]->nof_waiting_times *= 2; }
    size_t new_size = sizeof(int) * user_stats[serv]->nof_waiting_times + sizeof(UserStats);
    user_stats[serv] = (UserStats*)realloc(user_stats[serv], new_size);
    if (NULL == user_stats[serv]) { FUNC_PERROR(); }
  }
  user_stats[serv]->ser_data[wait_time_index[serv]] = time;
  wait_time_index[serv]++;
}

void send_user_stats(void)
{
  for (int i = 0; i < SERV_NUM; i++)
  {
    StatsSize stats_size = {0};
    long curr_mtype = 1 + (id * SERV_NUM) + i;
    stats_size.mtype = curr_mtype;
    stats_size.type = 0;
    stats_size.ser_data_size = sizeof(int) * wait_time_index[i];
    size_t msg_size = sizeof(StatsSize) - sizeof(long);
    msgsnd(MSG_STATS_METADATA_ID, &stats_size, msg_size, 0);
    user_stats[i]->nof_waiting_times = wait_time_index[i];
    user_stats[i]->mtype = curr_mtype;
    user_stats[i]->failed_serv = rem_serv_req[i];
    msg_size = sizeof(UserStats) + sizeof(int) * wait_time_index[i] - sizeof(long);
    msgsnd(MSG_STATS_DATA_ID, user_stats[i], msg_size, 0);
    log_trace("user: %d S send_stats -> serv: %d mtype: %d completed: %d fail: %d count: %d", id, i,
              stats_size.mtype, user_stats[i]->completed_serv, user_stats[i]->failed_serv,
              wait_time_index[i]);
  }
}

void send_notific_clock(int req_times[], Service* serv_req, int nof_req)
{
  size_t clock_req_size = get_notifc_size(CLOCK_REQ);
  ClockReq* clock_req = (ClockReq*)malloc(clock_req_size + sizeof(long));
  if (NULL == clock_req) { FUNC_PERROR(); }
  size_t times_size = sizeof(int) * (size_t)nof_req;
  size_t serv_size = sizeof(Service) * (size_t)nof_req;
  for (int i = 0; i < nof_req; i++)
  {
    log_trace("user: %d S clock_req -> serv: %d time: %d", id, serv_req[i], req_times[i]);
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
  if (-1 == msgsnd(MSG_NOTIFY_CLOCK_ID, clock_req, clock_req_size, 0)) { FUNC_PERROR(); }
}

void user_set_notf_param(GetNotfParam* get_notf_param, void** notifc)
{
  MesType notifc_filter[] = {DAY_ENDED, CLOCK_NOTIFC, TICKET_RESP, SERVICE_RESP};
  get_notf_param->nof_notifc = 4;
  size_t notifc_filter_size = sizeof(MesType) * 4;
  get_notf_param->notifc_filter = (MesType*)malloc(notifc_filter_size);
  if (NULL == get_notf_param->notifc_filter) { FUNC_PERROR(); }
  memcpy(get_notf_param->notifc_filter, notifc_filter, notifc_filter_size);
  get_notf_param->msg_id = MSG_NOTIFY_USER_IDS[id];
  get_notf_param->sem_id = SEM_NOTIFY_USER_ID;
  get_notf_param->sem_count = id;
  get_notf_param->can_skip = 0;
  get_notf_param->nowait = 0;
  get_notf_param->notifc_mes = notifc;
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
    log_trace("user: %d -> S send NULL clock notifc", id);
    send_notific_clock(NULL, NULL, 0);
    return;
  }
  memset(rem_serv_req, 0, sizeof(rem_serv_req));
  int nof_req = (rand() % N_REQUESTS) + 1;
  Service serv_req[nof_req];
  int all_req_times[nof_req];
  for (int i = 0; i < nof_req; i++)
  {
    serv_req[i] = (Service)(rand() % SERV_NUM);
    rem_serv_req[serv_req[i]]++;
  }
  int req_time = (int)(rand() % (8 * 60));
  int opt_time = find_best_time(req_time, serv_req, nof_req);
  calc_times_from_serv(all_req_times, serv_req, opt_time, nof_req);
  send_notific_clock(all_req_times, serv_req, nof_req);
}
