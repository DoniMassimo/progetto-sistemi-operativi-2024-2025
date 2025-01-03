#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <errno.h>
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "seats.h"
#include "sem.h"
#include "sem_utils.h"
#include "shm.h"
#include "utils.h"
#include "msg.h"

int assigned_service;
Signal recived_signal = NOSIGNAL;

void handle_sigusr1(int signo, siginfo_t* info, void* context)
{
  recived_signal = info->si_value.sival_int;
  printf("worker -> sengale ricevuto\n");
  fflush(stdout);
}

void signal_setup(void)
{
  struct sigaction sa;
  sa.sa_sigaction = handle_sigusr1;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGUSR1, &sa, NULL) == -1) { FUNC_PERROR(); }
}

void setup(char arg_1[])
{
  char* endptr;
  assigned_service = (int)strtol(arg_1, &endptr, 10);
  if (*endptr != '\0') { FUNC_MSG_ERROR("Cant convert argv[1] to int."); }
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
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_MSG_PERROR("sem empl"); }
}

void core(void)
{
  SeatInfo seat_info = {0};
  int seat_index = seats_try_take_seat(assigned_service, &recived_signal, &seat_info);
  if (DAY_ENDED == recived_signal)
  {
    recived_signal = NOSIGNAL;
    printf("worker -> interrotto mentre cercavo posto\n");
    fflush(stdout);
    if (seat_index >= 0) { seats_release_seat(assigned_service, seat_index); }
    return;
  }
  if (-1 == lock_sem(seat_info.notify_worker_sem_id, 0) && errno != EINTR) { FUNC_PERROR(); }
  if (DAY_ENDED == recived_signal)
  {
    recived_signal = NOSIGNAL;
    printf("worker -> interrotto mentre aspettavo user\n");
    fflush(stdout);
    seats_release_seat(assigned_service, seat_index);
    return;
  }
  // handle user request
}

int main(int argc, char* argv[])
{
  if (2 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  setup(argv[1]);
  while (1)
  {
    start();
    core();
  }
  return 0;
}
