#include <stdio.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <limits.h>
#include "config.h"
#include "calendar.h"
#include "ftok_key.h"
#include "macros.h"
#include "sem.h"
#include "msg.h"
#include "shm.h"
#include "sem_utils.h"
#include "struct.h"
#include "notification.h"
#include "clock.h"

void setup(void)
{
  config_load();
  ftok_key_init();
  msg_config();
  sem_config();
  shm_config();
  if (-1 == release_sem_val(SEM_DAY_END_ID, 0, START_SEM_COUNT)) { FUNC_PERROR(); }
  log_trace("clock rilascia day ENDEDEDE %d", START_SEM_COUNT);
}

void start(void)
{
  if (-1 == lock_sem(SEM_CLOCK_ADD_USERS_ID, 0)) { FUNC_PERROR(); }
  int sem_add_users_val = get_sem_value(SEM_CLOCK_ADD_USERS_ID, 0);
  if (1 == sem_add_users_val)
  {
    if (-1 == lock_sem(SEM_CLOCK_ADD_USERS_ID, 0)) { FUNC_PERROR(); }
    add_new_users();
    if (-1 == release_sem_val(SEM_DAY_END_ID, 0, N_NEW_USERS)) { FUNC_PERROR(); }
  }
  if (timer_data != NULL)
  {
    free(timer_data);
    timer_data = NULL;
    timer_data_size = 0;
    timer_send_index = 0;
    timer_recv_index = 0;
  }
  setup_user_notific();
  setup_worker_pause();
  struct msqid_ds buf;
  if (msgctl(MSG_NOTIFY_CLOCK_ID, IPC_STAT, &buf) == -1) { FUNC_PERROR(); }
  if (buf.msg_qnum != 0) { MSG_ERROR("Zero mess are expected"); }
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEM_START_ID, 0)) { FUNC_PERROR(); }
  release_sem(SEM_DAY_STARTED_ID, 0);
}

void core(void)
{
  int* min_count = (int*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
  if ((int*)-1 == (int*)min_count) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEMRP_MIN_COUNT_ID.sem_writer_id, 0)) { FUNC_PERROR(); }
  *min_count = 0;
  if (-1 == release_sem(SEMRP_MIN_COUNT_ID.sem_writer_id, 0)) { FUNC_PERROR(); }
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = (long int)N_NANO_SECS;
  while (*min_count < (60 * 8))
  {
    get_new_timer(*min_count);
    if (-1 == lock_sem(SEMRP_MIN_COUNT_ID.sem_writer_id, 0)) { FUNC_PERROR(); }
    (*min_count)++;
    if (-1 == release_sem(SEMRP_MIN_COUNT_ID.sem_writer_id, 0)) { FUNC_PERROR(); }
    if (nanosleep(&req, NULL) == -1) { FUNC_PERROR(); }
    send_user_notific(*min_count);
    send_worker_pause(*min_count);
    send_timer_notifc(*min_count);
  }
  send_timer_notifc(INT_MAX);
  clear_calendar();
  send_msg_day_ended();
  req.tv_nsec = (long int)N_NANO_SECS * 5;
  if (nanosleep(&req, NULL) == -1) { FUNC_PERROR(); }
  get_new_timer(*min_count);
  send_timer_notifc(INT_MAX);
  if (-1 == shmdt(min_count)) { FUNC_PERROR(); }
  if (-1 == release_sem_val(SEM_DAY_END_ID, 0, START_SEM_COUNT)) { FUNC_PERROR(); }
}

int main(void)
{
  setup();
  int day_count = 0;
  while (1)
  {
    if (day_count >= SIM_DURATION) { break; }
    start();
    core();
    day_count++;
  }
  lock_sem(SEM_PROC_CAN_DIE_ID, 0);
  return 0;
}
