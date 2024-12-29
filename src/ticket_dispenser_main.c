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
#define int_max 2147483647
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
    // msgrcv
    DispenserMsg msg;
    if(msgrcv(MSG_TICKET_DISPENSER_ID, &msg, sizeof(Ticket), 0, 0) == -1) { FUNC_PERROR(); }
    printf("message received: %s\n, msg.ticket.status");

    SeatInfo* shm_sinfo_ptr = (SeatInfo*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
    if ((SeatInfo*)-1 == (SeatInfo*)shm_sinfo_ptr) { FUNC_PERROR(); }
    // controllo se ce il servizio
    int bounds[2];
    get_bounds_serv(bounds, msg.ticket.status);

    int service_available = 0;
    int seat_index = -1;
    int min_nof_user_waiting = int_max;
    for(int i = bounds[0]; i < bounds[1]; i++)
    {
      if(shm_sinfo_ptr[i].nof_user_wainting < min_nof_user_waiting)
      {
        min_nof_user_waiting = shm_sinfo_ptr[i].nof_user_wainting;
        seat_index = i;
        service_available = 1;
      }
    }
    if (-1 == shmdt(shm_sinfo_ptr)) { FUNC_PERROR(); }
    
    if(service_available)
    {
      if(-1 == msgsnd(msg.ticket.com_mqueue_id, &msg, sizeof(Ticket), 0)) { FUNC_PERROR(); }
      printf("message sent: %s\n, msg.ticket.status");
    }
    else
    {
      printf("service not available: %s\n, msg.ticket.status");
    }
  }
}

int main(int argc, char* argv[])
{
  setup();
  start();
  printf("ticket disp\n");
  fflush(stdout);
  core();
  return 0;
}
