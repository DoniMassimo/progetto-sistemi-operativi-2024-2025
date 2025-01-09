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
#include "ticket_dispenser.h"

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
  int nof_notifc = 2;
  MesType notifc_filter[] = {DAY_ENDED, TICKET_REQ};
  void* notifc = NULL;
  GetNotfParam get_notf_param = {0};
  get_notf_param.notifc_filter = notifc_filter;
  get_notf_param.nof_notifc = nof_notifc;
  get_notf_param.msg_id = MSG_NOTIFY_DISPENSER_ID;
  get_notf_param.sem_id = SEM_NOTIFY_DISPENSER_ID;
  get_notf_param.sem_count = 0;
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
    else if (TICKET_REQ == notification)
    {
      TicketReq* ticket_req = (TicketReq*)notifc;
      handle_ticket_req(ticket_req);
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
