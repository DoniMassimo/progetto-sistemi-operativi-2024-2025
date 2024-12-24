#include <stdio.h>
#include <time.h>
#include <sys/shm.h>
#include <signal.h>
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "sem.h"
#include "shm.h"
#include "sem_utils.h"

void send_signal_day_end()
{
  pid_t* shm_wpid_ptr = shmat(SHM_WORKERS_PID_ID, NULL, 0);
  if ((pid_t*)-1 == (pid_t*)shm_wpid_ptr) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    pid_t proc_pid = shm_wpid_ptr[i];
    union sigval value;
    value.sival_int = DAY_ENDED;
    if (sigqueue(proc_pid, SIGUSR1, value) == -1) { FUNC_PERROR(); }
  }
  if (-1 == shmdt(shm_wpid_ptr)) { FUNC_PERROR(); }
}

void setup(void)
{
  config_load();
  ftok_key_init();
  sem_config();
  shm_config();
}

void start(void)
{
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
  init_sem_one(SEM_DAY_STARTED_ID, 0);
}

void core(void)
{
  printf("giorno iniziato\n");
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
  send_signal_day_end();
  release_sem(SEM_DAY_END_ID, 0);
  printf("giorno finito\n");
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
