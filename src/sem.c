#include <sys/ipc.h>
#include <sys/sem.h>
#include "ftok_key.h"
#include "sem_utils.h"
#include "config.h"
#include "macros.h"
#include "sem.h"
#include "sem_utils.h"

int SEM_START_ID = -1;
int SEM_DAY_STARTED_ID = -1;
int SEM_DAY_END_ID = -1;
int SEM_SEATS_ID = -1;
int SEM_SHM_SEATS_INFO_ID = -1;
int SEM_PROC_READY_ID = -1;
int SEM_NOTIFY_WORKER_ID = -1;
int SEM_NOTIFY_DISPENSER_ID = -1;
int SEM_NOTIFY_USER_ID = -1;
int SEM_ADD_USERS_ID = -1;
int SEM_CLOCK_ADD_USERS_ID = -1;
int SEM_PROC_CAN_DIE_ID = -1;
SemRP_Id SEMRP_CALENDAR_ID = {0};
SemRP_Id SEMRP_MIN_COUNT_ID = {0};
SemWP_Id SEMWP_SEATS_INFO_ID = {0};

void SEM_START_ID_init(void)
{
  SEM_START_ID = semget(SEM_START_KEY, 1, 0666 | IPC_CREAT);
  if (SEM_START_ID < 0) { FUNC_PERROR(); }
}

void SEM_START_ID_config(void)
{
  SEM_START_ID = semget(SEM_START_KEY, 1, 0666);
  if (SEM_START_ID < 0) { FUNC_PERROR(); }
}

void SET_SEATS_ID_init(void)
{
  SEM_SEATS_ID = semget(SEM_SEATS_KEY, SERV_NUM, 0666 | IPC_CREAT);
  if (SEM_SEATS_ID < 0) { FUNC_PERROR(); }
}

void SET_SEATS_ID_config(void)
{
  SEM_SEATS_ID = semget(SEM_SEATS_KEY, SERV_NUM, 0666);
  if (SEM_SEATS_ID < 0) { FUNC_PERROR(); }
}

void SEM_SHM_SEATS_INFO_init(void)
{
  SEM_SHM_SEATS_INFO_ID = semget(SEM_SHM_SEATS_INFO_KEY, 1, 0666 | IPC_CREAT);
  if (SEM_SHM_SEATS_INFO_ID < 0) { FUNC_PERROR(); }
  init_sem_one(SEM_SHM_SEATS_INFO_ID, 0);
}

void SEM_SHM_SEATS_INFO_config(void)
{
  SEM_SHM_SEATS_INFO_ID = semget(SEM_SHM_SEATS_INFO_KEY, 1, 0666);
  if (SEM_SHM_SEATS_INFO_ID < 0) { FUNC_PERROR(); }
}

void SEM_DAY_STARTED_init(void)
{
  SEM_DAY_STARTED_ID = semget(SEM_DAY_STARTED_KEY, 1, 0666 | IPC_CREAT);
  if (SEM_DAY_STARTED_ID < 0) { FUNC_PERROR(); }
  init_sem_zero(SEM_DAY_STARTED_ID, 0);
}

void SEM_DAY_STARTED_config(void)
{
  SEM_DAY_STARTED_ID = semget(SEM_DAY_STARTED_KEY, 1, 0666);
  if (SEM_DAY_STARTED_ID < 0) { FUNC_PERROR(); }
}

void SEM_DAY_END_init(void)
{
  SEM_DAY_END_ID = semget(SEM_DAY_END_KEY, 1, 0666 | IPC_CREAT);
  if (SEM_DAY_END_ID < 0) { FUNC_PERROR(); }
  init_sem_zero(SEM_DAY_END_ID, 0);
}

void SEM_DAY_END_config(void)
{
  SEM_DAY_END_ID = semget(SEM_DAY_END_KEY, 1, 0666);
  if (SEM_DAY_END_ID < 0) { FUNC_PERROR(); }
}

void SEM_PROC_READY_ID_init(void)
{
  SEM_PROC_READY_ID = semget(SEM_PROC_READY_KEY, 1, 0666 | IPC_CREAT);
  if (SEM_PROC_READY_ID < 0) { FUNC_PERROR(); }
  if (-1 == init_sem_zero(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
}

void SEM_PROC_READY_ID_config(void)
{
  SEM_PROC_READY_ID = semget(SEM_PROC_READY_KEY, 1, 0666);
  if (SEM_PROC_READY_ID < 0) { FUNC_PERROR(); }
}

void SEM_NOTIFY_WORKER_ID_init(void)
{
  if (NOF_WORKERS < 1) { return; }
  SEM_NOTIFY_WORKER_ID = semget(SEM_NOTIFY_WORKER_KEY, NOF_WORKERS, 0666 | IPC_CREAT);
  if (-1 == SEM_NOTIFY_WORKER_ID) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++) { init_sem_zero(SEM_NOTIFY_WORKER_ID, i); }
}

void SEM_NOTIFY_WORKER_ID_config(void)
{
  if (NOF_WORKERS < 1) { return; }
  SEM_NOTIFY_WORKER_ID = semget(SEM_NOTIFY_WORKER_KEY, NOF_WORKERS, 0666);
  if (-1 == SEM_NOTIFY_WORKER_ID) { FUNC_PERROR(); }
}

void SEM_NOTIFY_DISPENSER_ID_init(void)
{
  SEM_NOTIFY_DISPENSER_ID = semget(SEM_NOTIFY_DISPENSER_KEY, 1, 0666 | IPC_CREAT);
  if (SEM_NOTIFY_DISPENSER_ID < 0) { FUNC_PERROR(); }
  if (-1 == init_sem_zero(SEM_NOTIFY_DISPENSER_ID, 0)) { FUNC_PERROR(); }
}

void SEM_NOTIFY_DISPENSER_ID_config(void)
{
  SEM_NOTIFY_DISPENSER_ID = semget(SEM_NOTIFY_DISPENSER_KEY, 1, 0666);
  if (SEM_NOTIFY_DISPENSER_ID < 0) { FUNC_PERROR(); }
}

void SEM_NOTIFY_USER_ID_init(void)
{
  SEM_NOTIFY_USER_ID = semget(SEM_NOTIFY_USER_KEY, NOF_USERS + N_NEW_USERS, 0666 | IPC_CREAT);
  if (SEM_NOTIFY_USER_ID < 0) { FUNC_PERROR(); }
  if (-1 == init_all_sem_zero(SEM_NOTIFY_USER_ID, NOF_USERS + N_NEW_USERS)) { FUNC_PERROR(); }
}

void SEM_NOTIFY_USER_ID_config(void)
{
  SEM_NOTIFY_USER_ID = semget(SEM_NOTIFY_USER_KEY, NOF_USERS + N_NEW_USERS, 0666);
  if (SEM_NOTIFY_USER_ID < 0) { FUNC_PERROR(); }
}

void SEM_ADD_USERS_init(void)
{
  SEM_ADD_USERS_ID = semget(SEM_ADD_USERS_KEY, 1, 0666 | IPC_CREAT);
  if (SEM_ADD_USERS_ID < 0) { FUNC_PERROR(); }
  init_sem_zero(SEM_ADD_USERS_ID, 0);
}

void SEM_ADD_USERS_config(void)
{
  SEM_ADD_USERS_ID = semget(SEM_ADD_USERS_KEY, 1, 0666);
  if (SEM_ADD_USERS_ID < 0) { FUNC_PERROR(); }
}

void SEM_CLOCK_ADD_USERS_init(void)
{
  SEM_CLOCK_ADD_USERS_ID = semget(SEM_CLOCK_ADD_USERS_KEY, 1, 0666 | IPC_CREAT);
  if (SEM_CLOCK_ADD_USERS_ID < 0) { FUNC_PERROR(); }
  init_sem_zero(SEM_CLOCK_ADD_USERS_ID, 0);
}

void SEM_CLOCK_ADD_USERS_config(void)
{
  SEM_CLOCK_ADD_USERS_ID = semget(SEM_CLOCK_ADD_USERS_KEY, 1, 0666);
  if (SEM_CLOCK_ADD_USERS_ID < 0) { FUNC_PERROR(); }
}

void SEM_PROC_CAN_DIE_ID_init(void)
{
  SEM_PROC_CAN_DIE_ID = semget(SEM_PROC_CAN_DIE_KEY, 1, 0666 | IPC_CREAT);
  if (SEM_PROC_CAN_DIE_ID < 0) { FUNC_PERROR(); }
  init_sem_zero(SEM_PROC_CAN_DIE_ID, 0);
}

void SEM_PROC_CAN_DIE_ID_config(void)
{
  SEM_PROC_CAN_DIE_ID = semget(SEM_PROC_CAN_DIE_KEY, 1, 0666);
  if (SEM_PROC_CAN_DIE_ID < 0) { FUNC_PERROR(); }
}

void SEMRP_CALENDAR_ID_init(void)
{
  SEMRP_CALENDAR_ID.sem_mutex_id =
      semget(SEMRP_CALENDAR_STRUCT_KEY.sem_mutex_key, 1, 0666 | IPC_CREAT);
  init_sem_one(SEMRP_CALENDAR_ID.sem_mutex_id, 0);
  if (-1 == SEMRP_CALENDAR_ID.sem_mutex_id) { FUNC_PERROR(); }
  SEMRP_CALENDAR_ID.sem_reader_count_id =
      semget(SEMRP_CALENDAR_STRUCT_KEY.sem_reader_count_key, 1, 0666 | IPC_CREAT);
  init_sem_zero(SEMRP_CALENDAR_ID.sem_reader_count_id, 0);
  if (-1 == SEMRP_CALENDAR_ID.sem_reader_count_id) { FUNC_PERROR(); }
  SEMRP_CALENDAR_ID.sem_writer_id =
      semget(SEMRP_CALENDAR_STRUCT_KEY.sem_writer_key, 1, 0666 | IPC_CREAT);
  if (-1 == SEMRP_CALENDAR_ID.sem_writer_id) { FUNC_PERROR(); }
  init_sem_one(SEMRP_CALENDAR_ID.sem_writer_id, 0);
}

void SEMRP_CALENDAR_ID_config(void)
{
  SEMRP_CALENDAR_ID.sem_mutex_id = semget(SEMRP_CALENDAR_STRUCT_KEY.sem_mutex_key, 1, 0666);
  if (-1 == SEMRP_CALENDAR_ID.sem_mutex_id) { FUNC_PERROR(); }
  SEMRP_CALENDAR_ID.sem_reader_count_id =
      semget(SEMRP_CALENDAR_STRUCT_KEY.sem_reader_count_key, 1, 0666);
  if (-1 == SEMRP_CALENDAR_ID.sem_reader_count_id) { FUNC_PERROR(); }
  SEMRP_CALENDAR_ID.sem_writer_id = semget(SEMRP_CALENDAR_STRUCT_KEY.sem_writer_key, 1, 0666);
  if (-1 == SEMRP_CALENDAR_ID.sem_writer_id) { FUNC_PERROR(); }
}

void SEMRP_MIN_COUNT_ID_init(void)
{
  SEMRP_MIN_COUNT_ID.sem_mutex_id =
      semget(SEMRP_MIN_COUNT_STRUCT_KEY.sem_mutex_key, 1, 0666 | IPC_CREAT);
  init_sem_one(SEMRP_MIN_COUNT_ID.sem_mutex_id, 0);
  if (-1 == SEMRP_MIN_COUNT_ID.sem_mutex_id) { FUNC_PERROR(); }
  SEMRP_MIN_COUNT_ID.sem_reader_count_id =
      semget(SEMRP_MIN_COUNT_STRUCT_KEY.sem_reader_count_key, 1, 0666 | IPC_CREAT);
  init_sem_zero(SEMRP_MIN_COUNT_ID.sem_reader_count_id, 0);
  if (-1 == SEMRP_MIN_COUNT_ID.sem_reader_count_id) { FUNC_PERROR(); }
  SEMRP_MIN_COUNT_ID.sem_writer_id =
      semget(SEMRP_MIN_COUNT_STRUCT_KEY.sem_writer_key, 1, 0666 | IPC_CREAT);
  if (-1 == SEMRP_MIN_COUNT_ID.sem_writer_id) { FUNC_PERROR(); }
  init_sem_one(SEMRP_MIN_COUNT_ID.sem_writer_id, 0);
}

void SEMRP_MIN_COUNT_ID_config(void)
{
  SEMRP_MIN_COUNT_ID.sem_mutex_id = semget(SEMRP_MIN_COUNT_STRUCT_KEY.sem_mutex_key, 1, 0666);
  if (-1 == SEMRP_MIN_COUNT_ID.sem_mutex_id) { FUNC_PERROR(); }
  SEMRP_MIN_COUNT_ID.sem_reader_count_id =
      semget(SEMRP_MIN_COUNT_STRUCT_KEY.sem_reader_count_key, 1, 0666);
  if (-1 == SEMRP_MIN_COUNT_ID.sem_reader_count_id) { FUNC_PERROR(); }
  SEMRP_MIN_COUNT_ID.sem_writer_id = semget(SEMRP_MIN_COUNT_STRUCT_KEY.sem_writer_key, 1, 0666);
  if (-1 == SEMRP_MIN_COUNT_ID.sem_writer_id) { FUNC_PERROR(); }
}

void SEMWP_SEATS_INFO_ID_init(void)
{
  SEMWP_SEATS_INFO_ID.sem_mutex_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_mutex_key, 1, 0666 | IPC_CREAT);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_mutex_id) { FUNC_PERROR(); }
  init_sem_one(SEMWP_SEATS_INFO_ID.sem_mutex_id, 0);

  SEMWP_SEATS_INFO_ID.sem_ar_count_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_ar_count_key, 1, 0666 | IPC_CREAT);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_ar_count_id) { FUNC_PERROR(); }
  init_sem_zero(SEMWP_SEATS_INFO_ID.sem_ar_count_id, 0);

  SEMWP_SEATS_INFO_ID.sem_wr_count_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_wr_count_key, 1, 0666 | IPC_CREAT);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_wr_count_id) { FUNC_PERROR(); }
  init_sem_zero(SEMWP_SEATS_INFO_ID.sem_wr_count_id, 0);

  SEMWP_SEATS_INFO_ID.sem_aw_count_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_aw_count_key, 1, 0666 | IPC_CREAT);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_aw_count_id) { FUNC_PERROR(); }
  init_sem_zero(SEMWP_SEATS_INFO_ID.sem_aw_count_id, 0);

  SEMWP_SEATS_INFO_ID.sem_ww_count_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_ww_count_key, 1, 0666 | IPC_CREAT);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_ww_count_id) { FUNC_PERROR(); }
  init_sem_zero(SEMWP_SEATS_INFO_ID.sem_ww_count_id, 0);

  SEMWP_SEATS_INFO_ID.sem_ok_read_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_ok_read_key, 1, 0666 | IPC_CREAT);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_ok_read_id) { FUNC_PERROR(); }
  init_sem_zero(SEMWP_SEATS_INFO_ID.sem_ok_read_id, 0);

  SEMWP_SEATS_INFO_ID.sem_ok_write_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_ok_write_key, 1, 0666 | IPC_CREAT);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_ok_write_id) { FUNC_PERROR(); }
  init_sem_zero(SEMWP_SEATS_INFO_ID.sem_ok_write_id, 0);
}

void SEMWP_SEATS_INFO_ID_config(void)
{
  SEMWP_SEATS_INFO_ID.sem_mutex_id = semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_mutex_key, 1, 0666);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_mutex_id) { FUNC_PERROR(); }

  SEMWP_SEATS_INFO_ID.sem_ar_count_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_ar_count_key, 1, 0666);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_ar_count_id) { FUNC_PERROR(); }

  SEMWP_SEATS_INFO_ID.sem_wr_count_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_wr_count_key, 1, 0666);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_wr_count_id) { FUNC_PERROR(); }

  SEMWP_SEATS_INFO_ID.sem_aw_count_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_aw_count_key, 1, 0666);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_aw_count_id) { FUNC_PERROR(); }

  SEMWP_SEATS_INFO_ID.sem_ww_count_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_ww_count_key, 1, 0666);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_ww_count_id) { FUNC_PERROR(); }

  SEMWP_SEATS_INFO_ID.sem_ok_read_id = semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_ok_read_key, 1, 0666);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_ok_read_id) { FUNC_PERROR(); }

  SEMWP_SEATS_INFO_ID.sem_ok_write_id =
      semget(SEMWP_SEATS_INFO_STRUCT_KEY.sem_ok_write_key, 1, 0666);
  if (-1 == SEMWP_SEATS_INFO_ID.sem_ok_write_id) { FUNC_PERROR(); }
}

void sem_init(void)
{
  SEM_START_ID_init();
  SET_SEATS_ID_init();
  SEM_SHM_SEATS_INFO_init();
  SEM_DAY_STARTED_init();
  SEM_DAY_END_init();
  SEM_PROC_READY_ID_init();
  SEM_NOTIFY_WORKER_ID_init();
  SEM_NOTIFY_DISPENSER_ID_init();
  SEM_NOTIFY_USER_ID_init();
  SEM_ADD_USERS_init();
  SEM_CLOCK_ADD_USERS_init();
  SEM_PROC_CAN_DIE_ID_init();
  SEMRP_CALENDAR_ID_init();
  SEMRP_MIN_COUNT_ID_init();
  SEMWP_SEATS_INFO_ID_init();
}

void sem_config(void)
{
  SEM_START_ID_config();
  SET_SEATS_ID_config();
  SEM_SHM_SEATS_INFO_config();
  SEM_DAY_STARTED_config();
  SEM_DAY_END_config();
  SEM_PROC_READY_ID_config();
  SEM_NOTIFY_WORKER_ID_config();
  SEM_NOTIFY_DISPENSER_ID_config();
  SEM_NOTIFY_USER_ID_config();
  SEM_ADD_USERS_config();
  SEM_CLOCK_ADD_USERS_config();
  SEM_PROC_CAN_DIE_ID_init();
  SEMRP_CALENDAR_ID_config();
  SEMRP_MIN_COUNT_ID_init();
  SEMWP_SEATS_INFO_ID_config();
}

void sem_deallocate(void)
{
  if (-1 == semctl(SEM_START_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_SEATS_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_SHM_SEATS_INFO_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_DAY_STARTED_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_DAY_END_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_PROC_READY_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_NOTIFY_WORKER_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_NOTIFY_DISPENSER_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_NOTIFY_USER_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_ADD_USERS_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_CLOCK_ADD_USERS_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEM_PROC_CAN_DIE_ID, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMRP_CALENDAR_ID.sem_mutex_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMRP_CALENDAR_ID.sem_reader_count_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMRP_CALENDAR_ID.sem_writer_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMRP_MIN_COUNT_ID.sem_mutex_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMRP_MIN_COUNT_ID.sem_reader_count_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMRP_MIN_COUNT_ID.sem_writer_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMWP_SEATS_INFO_ID.sem_mutex_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMWP_SEATS_INFO_ID.sem_ar_count_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMWP_SEATS_INFO_ID.sem_wr_count_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMWP_SEATS_INFO_ID.sem_aw_count_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMWP_SEATS_INFO_ID.sem_ww_count_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMWP_SEATS_INFO_ID.sem_ok_read_id, 0, IPC_RMID)) { FUNC_PERROR(); }
  if (-1 == semctl(SEMWP_SEATS_INFO_ID.sem_ok_write_id, 0, IPC_RMID)) { FUNC_PERROR(); }
}
