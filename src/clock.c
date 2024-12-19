#include <stdio.h>
#include <time.h>
#include "ipc_config.h"
#include "config.h"
#include "clock.h"
#include "macros.h"

int main()
{
  config_load();
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = (long int)N_NANO_SECS;
  int min_count = 0;
  while (min_count < (60 * 8))
  {
    min_count++;
    if (nanosleep(&req, NULL) == -1) { FUNC_MSG_PERROR("clock"); }
  }
  return 0;
}
