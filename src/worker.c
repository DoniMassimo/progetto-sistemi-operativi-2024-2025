#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>
#include "notification.h"
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "log.h"
#include "seats.h"
#include "sem.h"
#include "sem_utils.h"
#include "shm.h"
#include "utils.h"
#include "msg.h"
#include "struct.h"
#include "calendar.h"
#include "worker.h"

Service assigned_service;
int seat_index;
int id;
int nof_pause_rem;

void set_pause_time(void)
{
  ClockReqPause clock_req_pause = {0};
  clock_req_pause.mtype = CLOCK_REQ_PAUSE;
  clock_req_pause.worker_msg_id = MSG_NOTIFY_WORKER_IDS[id];
  clock_req_pause.worker_sem_count = id;
  size_t crp_size = get_notifc_size(CLOCK_REQ_PAUSE);
  if (nof_pause_rem > 0)
  {
    int random_num = rand() % 100;
    if (random_num < 80)
    {
      int random_time = rand() % ((60 * 8) - 120);
      random_time = random_time + 60;
      clock_req_pause.time = random_time;
    }
  }
  else { clock_req_pause.time = -1; }
  if (-1 == msgsnd(MSG_NOTIFY_CLOCK_ID, &clock_req_pause, crp_size, 0)) { FUNC_PERROR(); }
  log_trace("worker %d S clock_req_pause -> time: %d", id, clock_req_pause.time);
}

void send_service_resp(ServiceReq* service_req)
{
  ServiceResp service_resp = {0};
  service_resp.mtype = SERVICE_RESP;
  service_resp.data = 1;
  size_t notifc_size = get_notifc_size(SERVICE_RESP);
  msgsnd(service_req->user_msg_id, &service_resp, notifc_size, 0);
  release_sem(service_req->user_sem_id, service_req->user_sem_count);
}

void provide_service(ServiceReq* service_req)
{
  if (service_req->serv != assigned_service) { MSG_ERROR("Service request not deliverable"); }
  int serv_dur = get_serv_duration(&service_req->serv, 1);
  int min_dur = serv_dur - (serv_dur / 2);
  int max_dur = serv_dur + (serv_dur / 2);
  serv_dur = min_dur + rand() % (max_dur - min_dur + 1);
  log_trace("worker: %d service_req -> duration: %d, serv: %d, user: %d", id, serv_dur,
            service_req->serv, service_req->user_sem_count);
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = (long int)N_NANO_SECS * serv_dur;
  if (nanosleep(&req, NULL) == -1) { FUNC_PERROR(); }
  send_service_resp(service_req);
}

int check_day_ended(void)
{
  DayEnded day_ended = {0};
  size_t day_ended_size = get_notifc_size(DAY_ENDED);
  if (-1 == msgrcv(MSG_NOTIFY_WORKER_IDS[id], &day_ended, day_ended_size, DAY_ENDED, IPC_NOWAIT))
  {
    if (ENOMSG != errno) { FUNC_PERROR(); }
    else if (ENOMSG == errno) { return 0; }
  }
  return 1;
}

int prov_serv_paused_worker(SeatFreeCom* seat_free_com)
{
  while (1)
  {
    if (1 == check_day_ended()) { return -1; }
    size_t service_req_size = get_notifc_size(SERVICE_REQ);
    ServiceReq service_req = {0};
    if (-1 == msgrcv(seat_free_com->worker_msg_id, &service_req, service_req_size, SERVICE_REQ,
                     IPC_NOWAIT))
    {
      if (ENOMSG != errno) { FUNC_PERROR(); }
      else if (ENOMSG == errno) { break; }
    }
    log_trace("worker: %d prov_serv_paused -> user: %d", id, service_req.user_sem_count);
    provide_service(&service_req);
  }
  return 0;
}

void comunicate_free_seat(void)
{
  SeatFreeCom seat_free_com = {0};
  size_t sfc_size = get_notifc_size(SEAT_FREE);
  seat_free_com.mtype = SEAT_FREE;
  seat_free_com.worker_msg_id = MSG_NOTIFY_WORKER_IDS[id];
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    if (i == id) { continue; }
    if (-1 == msgsnd(MSG_NOTIFY_WORKER_IDS[i], &seat_free_com, sfc_size, 0)) { FUNC_PERROR(); }
    log_trace("worker: %d comun_free_seat -> worker: %d, msgid: %d", id, i,
              MSG_NOTIFY_WORKER_IDS[i]);
  }
  if (-1 == release_all_sem_excl(SEM_NOTIFY_WORKER_ID, NOF_WORKERS, id)) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    int val = get_sem_value(SEM_NOTIFY_WORKER_ID, id);
    log_trace("worker: %d -> worker: %d sem_val: %d", id, i, val);
  }
}

void set_notf_param(GetNotfParam* get_notf_param, void** notifc)
{
  MesType notifc_filter[] = {DAY_ENDED, PAUSE_NOTIFC, SEAT_FREE, SERVICE_REQ};
  get_notf_param->nof_notifc = 4;
  size_t notifc_filter_size = sizeof(MesType) * 4;
  get_notf_param->notifc_filter = (MesType*)malloc(notifc_filter_size);
  if (NULL == get_notf_param->notifc_filter) { FUNC_PERROR(); }
  memcpy(get_notf_param->notifc_filter, notifc_filter, notifc_filter_size);
  get_notf_param->msg_id = MSG_NOTIFY_WORKER_IDS[id];
  get_notf_param->sem_id = SEM_NOTIFY_WORKER_ID;
  get_notf_param->sem_count = id;
  get_notf_param->can_skip = 0;
  get_notf_param->notifc_mes = notifc;
}

void take_pause(void)
{
  nof_pause_rem--;
  struct msqid_ds buf;
  if (msgctl(MSG_NOTIFY_WORKER_IDS[id], IPC_STAT, &buf) == -1) { FUNC_PERROR(); }
  log_trace("worker %d R pause_notifc -> msgid: %d count: %d", id, MSG_NOTIFY_WORKER_IDS[id],
            buf.msg_qnum);
  seats_release_seat(assigned_service, seat_index);
  comunicate_free_seat();
}

int try_take_paused_seats(void* notifc)
{
  int take_seat_res = seats_try_take_seat(assigned_service, id, &seat_index);
  if (0 == take_seat_res)
  {
    SeatFreeCom* seat_free_com = (SeatFreeCom*)notifc;
    log_trace("worker %d R seat_free_com -> paused worker msgid: %d", id,
              seat_free_com->worker_msg_id);
    if (-1 == prov_serv_paused_worker(seat_free_com))
    {
      seats_release_seat(assigned_service, seat_index);
      // day ended;
      return -1;
    }
  }
  return 0;
}
