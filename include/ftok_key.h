#ifndef FTOK_KEY_H
#define FTOK_KEY_H

typedef struct
{
  int sem_mutex_key;
  int sem_reader_count_key;
  int sem_writer_key;
} SemRP_Key;

typedef struct
{
  int sem_mutex_key;
  int sem_ar_count_key;
  int sem_wr_count_key;
  int sem_aw_count_key;
  int sem_ww_count_key;
  int sem_ok_read_key;
  int sem_ok_write_key;
} SemWP_Key;

extern key_t SEM_START_KEY;
extern key_t SEM_SEATS_KEY;
extern key_t SEM_SHM_SEATS_INFO_KEY;
extern key_t SEM_DAY_STARTED_KEY;
extern key_t SEM_DAY_END_KEY;
extern key_t SEM_PROC_READY_KEY;
extern key_t SEM_NOTIFY_WORKER_KEY;
extern key_t SEM_NOTIFY_DISPENSER_KEY;
extern key_t SEM_NOTIFY_USER_KEY;
extern key_t SEM_NOTIFY_CLOCK_KEY;
extern key_t SEM_ADD_USERS_KEY;
extern key_t SEM_CLOCK_ADD_USERS_KEY;
extern key_t SEM_PROC_CAN_DIE_KEY;
extern key_t SEM_ALL_PROC_STOP_KEY;
extern SemRP_Key SEMRP_CALENDAR_STRUCT_KEY;
extern SemRP_Key SEMRP_MIN_COUNT_STRUCT_KEY;
extern SemWP_Key SEMWP_SEATS_INFO_STRUCT_KEY;

extern key_t SHM_WORKERS_PID_KEY;
extern key_t SHM_SEATS_INDEX_KEY;
extern key_t SHM_SEATS_INFO_KEY;
extern key_t SHM_TICKET_DISPENSER_PID_KEY;
extern key_t SHM_CALENDAR_KEY;
extern key_t SHM_MIN_COUNT_KEY;

extern key_t* MSG_NOTIFY_WORKER_KEYS;
extern key_t MSG_NOTIFY_DISPENSER_KEY;
extern key_t* MSG_NOTIFY_USER_KEYS;
extern key_t MSG_NOTIFY_CLOCK_KEY;
extern key_t MSG_STATS_DATA_KEY;
extern key_t MSG_STATS_METADATA_KEY;

void ftok_key_init(void);

#endif
