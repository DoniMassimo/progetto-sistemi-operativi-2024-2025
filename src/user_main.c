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

Signal recived_signal = NOSIGNAL;

void handle_sigusr1(int signo, siginfo_t* info, void* context)
{
  recived_signal = info->si_value.sival_int;
}

void signal_setup(void)
{
  struct sigaction sa;
  sa.sa_sigaction = handle_sigusr1;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGUSR1, &sa, NULL) == -1) { FUNC_PERROR(); }
}

void setup(void)
{
  config_load();
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
  signal_setup();
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
