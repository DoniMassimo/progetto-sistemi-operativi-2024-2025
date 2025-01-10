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

Service assigned_service;
int id;

void setup(char arg_1[], char arg_2[])
{
  char* endptr;
  assigned_service = (Service)strtol(arg_1, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[1] to int."); }
  id = (int)strtol(arg_2, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[2] to int."); }
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
  log_trace("worker %d service_req -> duration: %d, serv: %d, user: %d", id, serv_dur,
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
  else { return 1; }
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
    provide_service(&service_req);
  }
  return 0;
}

void comunicate_free_seat(void)
{
  for (int i = 0; i < NOF_WORKER; i++)
  {
    if (i == id) { continue; }
    SeatFreeCom seat_free_com = {0};
    size_t sfc_size = get_notifc_size(SEAT_FREE);
    seat_free_com.mtype = SEAT_FREE;
    seat_free_com.worker_msg_id = MSG_NOTIFY_WORKER_IDS[id];
    msgsnd(MSG_NOTIFY_WORKER_IDS[i], &seat_free_com, sfc_size, 0);
  }
  release_all_sem_excl(SEM_NOTIFY_WORKER_ID, NOF_WORKER, id);
}

void core(void)
{
  int* seat_index;
  int take_seat_res = seats_try_take_seat(assigned_service, id, seat_index);
  int nof_notifc = 2;
  MesType notifc_filter[] = {DAY_ENDED, SERVICE_REQ, SEAT_FREE};
  void* notifc = NULL;
  GetNotfParam get_notf_param = {0};
  get_notf_param.notifc_filter = notifc_filter;
  get_notf_param.nof_notifc = nof_notifc;
  get_notf_param.msg_id = MSG_NOTIFY_WORKER_IDS[id];
  get_notf_param.sem_id = SEM_NOTIFY_WORKER_ID;
  get_notf_param.sem_count = id;
  get_notf_param.can_skip = 0;
  get_notf_param.notifc_mes = &notifc;
  while (1)
  {
    if (notifc != NULL) { free(notifc); }
    MesType notification = get_notifications(&get_notf_param);
    if (DAY_ENDED == notification)
    {
      free(notifc);
      return;
    }
    else if (SERVICE_REQ == notification)
    {
      ServiceReq* service_req = (ServiceReq*)notifc;
      provide_service(service_req);
    }
    else if (SEAT_FREE == notification)
    {
      if (-2 == take_seat_res)
      {
        take_seat_res = seats_try_take_seat(assigned_service, id, seat_index);
        if (0 == take_seat_res)
        {
          SeatFreeCom* seat_free_com = (SeatFreeCom*)notifc;
          if (-1 == prov_serv_paused_worker(seat_free_com))
          {
            // day ended;
            return;
          }
        }
      }
    }
    else if (PAUSE_REQ == notification)
    {
      if (take_seat_res != -2 && *seat_index != -1)
      {
        seats_release_seat(assigned_service, seat_index);
        comunicate_free_seat();
      }
    }
  }
}

int main(int argc, char* argv[])
{
  srand((unsigned int)(time(NULL) + getpid()));
  if (3 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  setup(argv[1], argv[2]);
  while (1)
  {
    start();
    core();
  }
  return 0;
}
