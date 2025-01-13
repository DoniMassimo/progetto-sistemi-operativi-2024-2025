#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>
#include "notification.h"
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "log.h"
#include "seats.h"
#include "sem.h"
#include "sem_utils.h"
#include "shm.h"
#include "utils.h"
#include "msg.h"
#include "struct.h"
#include "calendar.h"
#include "worker.h"

void setup(char arg_1[], char arg_2[])
{
  char* endptr;
  assigned_service = (Service)strtol(arg_1, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[1] to int."); }
  id = (int)strtol(arg_2, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[2] to int."); }
  config_load();
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
  key_t key = ftok(".", 333 + id);
  sem_timer_id = semget(key, 1, IPC_CREAT | 0666);
  nof_pause_rem = NOF_PAUSE;
}

void start(void)
{
  setup_worker_stats();
  if (-1 == lock_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
  set_pause_time();
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
}

void core(void)
{
  int take_seat_res = seats_try_take_seat(assigned_service, id, &seat_index);
  int exclude_pause = 0;
  if (-2 == take_seat_res)
  {
    log_trace("worker %d -> does not find seat", id);
    exclude_pause = 1;
  }
  else { set_active_state(); }
  GetNotfParam get_notf_param = {0};
  void* notifc = NULL;
  set_notf_param(&get_notf_param, &notifc);
  while (1)
  {
    if (notifc != NULL) { free(notifc); }
    MesType notification = get_notifications(&get_notf_param);
    if (DAY_ENDED == notification)
    {
      if (0 == take_seat_res) { seats_release_seat(assigned_service, seat_index); }
      return;
    }
    else if (SERVICE_REQ == notification)
    {
      ServiceReq* service_req = (ServiceReq*)notifc;
      provide_service(service_req);
    }
    else if (SEAT_FREE == notification)
    {
      log_trace("worker %d R seat_free_com -> seat status: %d", id, take_seat_res);
      if (-2 == take_seat_res)
      {
        if (-1 == try_take_paused_seats(notifc)) { break; }
      }
    }
    else if (PAUSE_NOTIFC == notification)
    {
      if (take_seat_res != -2 && 0 == exclude_pause)
      {
        set_pause_state();
        take_pause();
        break;
      }
    }
  }
  free(notifc);
}

int main(int argc, char* argv[])
{
  srand((unsigned int)(time(NULL) + getpid()));
  if (3 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  setup(argv[1], argv[2]);
  int day_count = 0;
  while (1)
  {
    if (day_count >= SIM_DURATION) { break; }
    start();
    core();
    send_worker_stats();
    day_count++;
  }
  lock_sem(SEM_PROC_CAN_DIE_ID, 0);
  if (-1 == semctl(sem_timer_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  return 0;
}
