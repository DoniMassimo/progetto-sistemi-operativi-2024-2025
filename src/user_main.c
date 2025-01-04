#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "seats.h"
#include "utils.h"
#include "macros.h"
#include "config.h"
#include "sem_utils.h"
#include "ftok_key.h"
#include "shm.h"
#include "sem.h"
#include "msg.h"

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
  printf("user -> core user\n");
  fflush(stdout);
}

int main(int argc, char* argv[])
{
  setup();
  while (1)
  {
    start();
    core();
  }
  return 0;
}
