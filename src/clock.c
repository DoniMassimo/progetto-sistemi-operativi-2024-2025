#include <stdio.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "sem.h"
#include "msg.h"
#include "shm.h"
#include "sem_utils.h"
#include "struct.h"

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
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
  printf("clock -> iniziato\n");
  fflush(stdout);
  init_sem_one(SEM_DAY_STARTED_ID, 0);
}

void send_msg_day_ended(void)
{
  ComStruct com_struct;
  com_struct.mtype = DAY_ENDED;
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    if (-1 == msgsnd(MSG_NOTIFY_WORKER_IDS[i], &com_struct, sizeof(Content), 0)) { FUNC_PERROR(); }
  }
  release_all_sem(SEM_NOTIFY_WORKER_ID, NOF_WORKERS);
  if (-1 == msgsnd(MSG_NOTIFY_DISPENSER_ID, &com_struct, sizeof(Content), 0)) { FUNC_PERROR(); }
  release_sem(SEM_NOTIFY_DISPENSER_ID, 0);
}

void core(void)
{
  fflush(stdout);
  int min_count = 0;
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = (long int)N_NANO_SECS;
  while (min_count < (60 * 8))
  {
    min_count++;
    if (nanosleep(&req, NULL) == -1) { FUNC_MSG_PERROR("clock"); }
  }
  release_sem(SEM_DAY_END_ID, 0);
  send_msg_day_ended();
  printf("clock -> giorno finito\n");
  fflush(stdout);
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
