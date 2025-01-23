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
  log_set_level(log_level);
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
}

void start(void)
{
  if (-1 == lock_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
  if (-1 == wait_zero_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
  clear_dispenser_msg_queue();
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
  int sem_expl = get_sem_value(SEM_STOP_SIM_EXPLODE_ID, 0);
  if (-1 == sem_expl) { FUNC_PERROR(); }
  else if (1 == sem_expl) { exit(0); }
}

void core(void)
{
  void* notifc = NULL;
  GetNotfParam get_notf_param = {0};
  dispenser_set_notf_param(&get_notf_param, &notifc);
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
  int day_count = 0;
  while (1)
  {
    if (day_count >= SIM_DURATION) { break; }
    start();
    core();
    day_count++;
  }
  lock_sem(SEM_PROC_CAN_DIE_ID, 0);
}
