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
#include "notification.h"
#include "user.h"

void setup(char arg_1[])
{
  char* endptr;
  id = (int)strtol(arg_1, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[1] to int."); }
  config_load();
  log_set_level(log_level);
  P_SERV = P_SERV_MIN + rand() % (P_SERV_MAX - P_SERV_MIN + 1);
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
}

void start(void)
{
  setup_user_stats();
  if (-1 == lock_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
  if (-1 == wait_zero_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
  user_clear_msg_queue();
  setup_clock_notifc();
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
}

void core(void)
{
  void* notifc = NULL;
  GetNotfParam get_notf_param = {0};
  user_set_notf_param(&get_notf_param, &notifc);
  int pending_req = 0;
  Service requested_serv = -1;
  int serv_req_time;
  while (1)
  {
    if (notifc != NULL) { free(notifc); }
    MesType notification = get_notifications(&get_notf_param);
    if (DAY_ENDED == notification)
    {
      free(notifc);
      break;
    }
    else if (TICKET_RESP == notification)
    {
      TicketResp* ticket_resp = (TicketResp*)notifc;
      log_trace("user: %d R ticket_resp -> serv: %d status: %d", id, ticket_resp->serv,
                ticket_resp->status);
      if (1 == ticket_resp->status)
      {
        log_trace("user: %d S service_req -> serv: %d worker: %d", id, ticket_resp->serv,
                  ticket_resp->worker_sem_count);
        send_serv_request(ticket_resp);
        int* min_count = (int*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
        if ((int*)-1 == (int*)min_count) { FUNC_PERROR(); }
        lock_reader_RP(SEMRP_MIN_COUNT_ID);
        serv_req_time = *min_count;
        release_reader_RP(SEMRP_MIN_COUNT_ID);
        if (-1 == shmdt(min_count)) { FUNC_PERROR(); }
        int new_filter[] = {DAY_ENDED, SERVICE_RESP};
        memcpy(get_notf_param.notifc_filter, new_filter, sizeof(MesType) * 2);
      }
    }
    else if (SERVICE_RESP == notification)
    {
      int* min_count = (int*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
      if ((int*)-1 == (int*)min_count) { FUNC_PERROR(); }
      lock_reader_RP(SEMRP_MIN_COUNT_ID);
      serv_req_time = serv_req_time - *min_count;
      release_reader_RP(SEMRP_MIN_COUNT_ID);
      if (-1 == shmdt(min_count)) { FUNC_PERROR(); }
      log_trace("user: %d R service_resp-> risposta servizio", id);
      ServiceResp* service_resp = (ServiceResp*)notifc;
      if (0 == service_resp->data) { MSG_ERROR("Expecting 1"); }
      get_notf_param.can_skip = 0;
      get_notf_param.nof_notifc = 4;
      MesType new_filter[] = {DAY_ENDED, CLOCK_NOTIFC, TICKET_RESP, SERVICE_RESP};
      memcpy(get_notf_param.notifc_filter, new_filter, sizeof(MesType) * 4);
      if (pending_req > 0)
      {
        if (-1 == release_sem_val(SEM_NOTIFY_USER_ID, id, pending_req)) { FUNC_PERROR(); }
      }
      if (-1 == requested_serv)
      {
        log_fatal("user: %d -> Unexpected serv resp", id);
        MSG_ERROR("Unexpected serv resp");
      }
      add_completed_serv(requested_serv);
      add_waiting_time(requested_serv, serv_req_time);
      rem_serv_req[requested_serv]--;
      pending_req = 0;
    }
    else if (CLOCK_NOTIFC == notification)
    {
      ClockNotifc* clock_notifc = (ClockNotifc*)notifc;
      log_trace("user: %d R clock_notifc -> serv: %d", id, clock_notifc->serv);
      requested_serv = (Service)clock_notifc->serv;
      send_ticket_request(requested_serv);
      get_notf_param.can_skip = 1;
      get_notf_param.nof_notifc = 2;
      int new_filter[] = {DAY_ENDED, TICKET_RESP};
      memcpy(get_notf_param.notifc_filter, new_filter, sizeof(MesType) * 2);
    }
    else if (NO_MES == notification)
    {
      if (1 == get_notf_param.can_skip) { pending_req++; }
      else { MSG_ERROR("Expect to find message"); }
    }
  }
}

int main(int argc, char* argv[])
{
  int day_count = 0;
  srand((unsigned int)(time(NULL) + getpid()));
  if (argc < 2 || argc > 3) { MSG_ERROR("agrc error"); }
  if (3 == argc)
  {
    char* endptr;
    day_count = (int)strtol(argv[2], &endptr, 10);
    if (*endptr != '\0') { MSG_ERROR("Cant convert argv[2] to int."); }
  }
  setup(argv[1]);
  while (1)
  {
    if (day_count >= SIM_DURATION) { break; }
    start();
    core();
    send_user_stats();
    day_count++;
  }
  lock_sem(SEM_PROC_CAN_DIE_ID, 0);
  return 0;
}
