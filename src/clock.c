#include <stdio.h>
#include <time.h>
#include "config.h"
#include "macros.h"
#include "sem.h"
#include "sem_utils.h"

int min_count = 0;

int main(void)
{
  config_load();
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
  init_sem_one(SEM_DAY_STARTED_ID, 0);
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = (long int)N_NANO_SECS;
  while (min_count < (60 * 8))
  {
    min_count++;
    if (nanosleep(&req, NULL) == -1) { FUNC_MSG_PERROR("clock"); }
  }
  return 0;
}
