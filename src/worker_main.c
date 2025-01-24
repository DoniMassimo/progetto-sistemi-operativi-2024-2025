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

void setup(char arg_1[], char arg_2[], char arg_3[], char arg_4[])
{
  char* endptr;
  assigned_service = (Service)strtol(arg_1, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[1] to int."); }
  id = (int)strtol(arg_2, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[2] to int."); }
  serv_bounds[0] = (int)strtol(arg_3, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[3] to int."); }
  serv_bounds[1] = (int)strtol(arg_4, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[4] to int."); }
  config_load();
  log_set_level(log_level);
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
  key_t key = ftok("/", 333 + id);
  sem_timer_id = semget(key, 1, IPC_CREAT | 0666);
  nof_pause_rem = NOF_PAUSE;
  if (id % 10 == 0) { log_info("Workers initialization..."); }
}

void start(void)
{
  setup_worker_stats();
  if (-1 == lock_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
  if (-1 == wait_zero_sem(SEM_DAY_END_ID, 0)) { FUNC_PERROR(); }
  worker_clear_msg_queue();
  set_pause_time();
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
  int sem_expl = get_sem_value(SEM_STOP_SIM_EXPLODE_ID, 0);
  if (-1 == sem_expl) { FUNC_PERROR(); }
  else if (1 == sem_expl)
  {
    if (-1 == semctl(sem_timer_id, 0, IPC_RMID)) { FUNC_PERROR(); }
    exit(0);
  }
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
  else
  {
    log_trace("worker %d -> find_seat_serv: %d", id, assigned_service);
    set_active_state();
  }
  GetNotfParam get_notf_param = {0};
  void* notifc = NULL;
  worker_set_notf_param(&get_notf_param, &notifc);
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
        int seat_outcome = try_take_paused_seats(notifc);
        if (-1 == seat_outcome) { break; }
        if (0 == seat_outcome) { take_seat_res = 0; }
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
  if (5 != argc) { MSG_ERROR("agrc error"); }
  utils_get_relative_path(argv[0], REL_DIR);
  setup(argv[1], argv[2], argv[3], argv[4]);
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
