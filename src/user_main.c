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
  int nof_notifc = 3;
  MesType notifc_filter[] = {DAY_ENDED, CLOCK_NOTIFC, TICKET_RESP, SERVICE_RESP};
  void* notifc = NULL;
  GetNotfParam get_notf_param = {0};
  get_notf_param.notifc_filter = notifc_filter;
  get_notf_param.nof_notifc = nof_notifc;
  get_notf_param.msg_id = MSG_NOTIFY_USER_IDS[id];
  get_notf_param.sem_id = SEM_NOTIFY_USER_ID;
  get_notf_param.sem_count = id;
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
    else if (TICKET_RESP == notification)
    {
      TicketResp* ticket_resp = (TicketResp*)notifc;
      log_trace("user %d risposta ticket -> esito: %d", id, ticket_resp->status);
      if (1 == ticket_resp->status) {}
    }
    else if (SERVICE_RESP == notification) {}
    else if (CLOCK_NOTIFC == notification)
    {
      ClockNotifc* clock_req = (ClockNotifc*)notifc;
      log_trace("user %d riceve notifica -> serv: %d", id, clock_req->serv);
      send_ticket_request((Service)clock_req->serv);
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
