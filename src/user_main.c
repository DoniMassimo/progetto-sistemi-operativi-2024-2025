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
  P_SERV = P_SERV_MIN + rand() % (P_SERV_MAX - P_SERV_MIN + 1);
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
}

void start(void)
{
  setup_clock_notifc();
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
}

void core(void)
{
  void* notifc = NULL;
  MesType notifc_filter[] = {DAY_ENDED, CLOCK_NOTIFC, TICKET_RESP, SERVICE_RESP};
  GetNotfParam get_notf_param = {0};
  get_notf_param.notifc_filter = notifc_filter;
  get_notf_param.nof_notifc = 4;
  get_notf_param.msg_id = MSG_NOTIFY_USER_IDS[id];
  get_notf_param.sem_id = SEM_NOTIFY_USER_ID;
  get_notf_param.sem_count = id;
  get_notf_param.can_skip = 0;
  get_notf_param.notifc_mes = &notifc;
  int pending_req = 0;
  while (1)
  {
    if (notifc != NULL) { free(notifc); }
    MesType notification = get_notifications(&get_notf_param);
    if (DAY_ENDED == notification)
    {
      log_trace("user %d -> day_ended", id);
      free(notifc);
      return;
    }
    else if (TICKET_RESP == notification)
    {
      TicketResp* ticket_resp = (TicketResp*)notifc;
      log_trace("user %d ticket_resp -> status: %d", id, ticket_resp->status);
      if (1 == ticket_resp->status) { send_serv_request(ticket_resp); }
      int new_filter[] = {DAY_ENDED, SERVICE_RESP};
      memcpy(get_notf_param.notifc_filter, new_filter, sizeof(MesType) * 2);
    }
    else if (SERVICE_RESP == notification)
    {
      log_trace("user %d -> risposta servizio", id);
      ServiceResp* service_resp = (ServiceResp*)notifc;
      if (0 == service_resp->data) { MSG_ERROR("Expecting 1"); }
      get_notf_param.can_skip = 0;
      get_notf_param.nof_notifc = 4;
      int new_filter[] = {DAY_ENDED, CLOCK_NOTIFC, TICKET_RESP, SERVICE_RESP};
      memcpy(get_notf_param.notifc_filter, new_filter, sizeof(MesType) * 2);
      release_sem_val(SEM_NOTIFY_USER_ID, id, pending_req);
      pending_req = 0;
    }
    else if (CLOCK_NOTIFC == notification)
    {
      ClockNotifc* clock_notifc = (ClockNotifc*)notifc;
      log_trace("user %d clock_notifc -> serv: %d", id, clock_notifc->serv);
      send_ticket_request((Service)clock_notifc->serv);
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
  srand((unsigned int)(time(NULL) + getpid()));
  if (2 != argc) { MSG_ERROR("agrc error"); }
  setup(argv[1]);
  while (1)
  {
    start();
    core();
  }
  return 0;
}
