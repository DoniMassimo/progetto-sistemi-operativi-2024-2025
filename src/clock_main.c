#include <stdio.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include "config.h"
#include "calendar.h"
#include "ftok_key.h"
#include "macros.h"
#include "sem.h"
#include "msg.h"
#include "shm.h"
#include "sem_utils.h"
#include "struct.h"
#include "notification.h"
#include "clock.h"

void setup(void)
{
  config_load();
  ftok_key_init();
  msg_config();
  sem_config();
  shm_config();
}

void start(void)
{
  if (-1 == release_sem_val(SEM_DAY_END_ID, 0, START_SEM_COUNT - 1)) { FUNC_PERROR(); }
  setup_user_notific();
  setup_worker_pause();
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
  init_sem_one(SEM_DAY_STARTED_ID, 0);
}

void core(void)
{
  int min_count = 0;
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = (long int)N_NANO_SECS;
  while (min_count < (60 * 8))
  {
    min_count++;
    if (nanosleep(&req, NULL) == -1) { FUNC_PERROR(); }
    send_user_notific(min_count);
    send_worker_pause(min_count);
  }
  log_trace("clock end -> time: %d", min_count);
  clear_calendar();
  send_msg_day_ended();
  if (-1 == release_sem_val(SEM_DAY_END_ID, 0, START_SEM_COUNT - 1)) { FUNC_PERROR(); }
}

int main(void)
{
  setup();
  while (1)
  {
    start();
    core();
  }
  return 0;
}
