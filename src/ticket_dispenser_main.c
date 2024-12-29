#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "seats.h"
#include "utils.h"
#include "macros.h"
#include "config.h"
#include "sem_utils.h"
#include "ftok_key.h"
#include "shm.h"
#include "sem.h"
#include "msg.h"

typedef struct
{
  int status;
  int com_mqueue_id;
  int notify_worker_sem_id;
} Ticket;

typedef struct
{
  long mtype;
  Ticket ticket;
} DispenserMsg;

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
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_MSG_PERROR("t rel"); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_MSG_PERROR("t start"); }
}

void core(void)
{
  while (1)
  {

  if (-1 == lock_sem(SEM_NOTIFY_DISPENSER_ID, 0)) { FUNC_PERROR(); }
    // msget
    SeatInfo* shm_sinfo_ptr = (SeatInfo*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
    if ((SeatInfo*)-1 == (SeatInfo*)shm_sinfo_ptr) { FUNC_PERROR(); }
    // controllo se ce il servizio
    if (-1 == shmdt(shm_sinfo_ptr)) { FUNC_PERROR(); }
    // msgsnd
  }
}

int main(int argc, char* argv[])
{
  setup();
  start();
  printf("ticket disp\n");
  fflush(stdout);
}
