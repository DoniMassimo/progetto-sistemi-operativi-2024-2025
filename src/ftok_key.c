#include <sys/ipc.h>
#include <sys/types.h>
#include "ftok_key.h"
#include "macros.h"
#include "config.h"

key_t key_count = 1;

key_t SEM_START_KEY = -1;
key_t SEM_SEATS_KEY = -1;
key_t SEM_SHM_SEATS_INFO_KEY = -1;
key_t SEM_DAY_STARTED_KEY = -1;
key_t SEM_DAY_END_KEY = -1;
key_t SEM_PROC_READY_KEY = -1;
key_t SEM_NOTIFY_WORKER_KEY = -1;
key_t SEM_NOTIFY_DISPENSER_KEY = -1;
key_t SEM_NOTIFY_USER_KEY = -1;
key_t SEM_NOTIFY_CLOCK_KEY = -1;
SemRW_Key SEMRW_CALENDAR_STRUCT_KEY = {0};

key_t SHM_SEATS_INDEX_KEY = -1;
key_t SHM_SEATS_INFO_KEY = -1;
key_t SHM_WORKERS_PID_KEY = -1;
key_t SHM_TICKET_DISPENSER_PID_KEY = -1;
key_t SHM_CALENDAR_KEY = -1;

key_t* MSG_NOTIFY_WORKER_KEYS = NULL;
key_t MSG_NOTIFY_DISPENSER_KEY = -1;
key_t* MSG_NOTIFY_USER_KEYS = NULL;
key_t MSG_NOTIFY_CLOCK_KEY = -1;

void sem_key_init(void)
{
  SEM_START_KEY = ftok(".", key_count++);
  if (-1 == SEM_START_KEY) { FUNC_PERROR(); }
  SEM_SEATS_KEY = ftok(".", key_count++);
  if (-1 == SEM_SEATS_KEY) { FUNC_PERROR(); }
  SEM_SHM_SEATS_INFO_KEY = ftok(".", key_count++);
  if (-1 == SEM_SHM_SEATS_INFO_KEY) { FUNC_PERROR(); }
  SEM_DAY_STARTED_KEY = ftok(".", key_count++);
  if (-1 == SEM_DAY_STARTED_KEY) { FUNC_PERROR(); }
  SEM_PROC_READY_KEY = ftok(".", key_count++);
  if (-1 == SEM_PROC_READY_KEY) { FUNC_PERROR(); }
  SEM_NOTIFY_WORKER_KEY = ftok(".", key_count++);
  if (-1 == SEM_NOTIFY_WORKER_KEY) { FUNC_PERROR(); }
  SEM_DAY_END_KEY = ftok(".", key_count++);
  if (-1 == SEM_DAY_END_KEY) { FUNC_PERROR(); }
  SEM_NOTIFY_DISPENSER_KEY = ftok(".", key_count++);
  if (-1 == SEM_NOTIFY_DISPENSER_KEY) { FUNC_PERROR(); }
  SEM_NOTIFY_USER_KEY = ftok(".", key_count++);
  if (-1 == SEM_NOTIFY_USER_KEY) { FUNC_PERROR(); }
  SEM_NOTIFY_CLOCK_KEY = ftok(".", key_count++);
  if (-1 == SEM_NOTIFY_CLOCK_KEY) { FUNC_PERROR(); }
}

void semrw_key_init(void)
{
  SEMRW_CALENDAR_STRUCT_KEY.sem_mutex_key = ftok(".", key_count++);
  if (-1 == SEMRW_CALENDAR_STRUCT_KEY.sem_mutex_key) { FUNC_PERROR(); }
  SEMRW_CALENDAR_STRUCT_KEY.sem_reader_count_key = ftok(".", key_count++);
  if (-1 == SEMRW_CALENDAR_STRUCT_KEY.sem_reader_count_key) { FUNC_PERROR(); }
  SEMRW_CALENDAR_STRUCT_KEY.sem_writer_key = ftok(".", key_count++);
  if (-1 == SEMRW_CALENDAR_STRUCT_KEY.sem_writer_key) { FUNC_PERROR(); }
}

void shm_key_init(void)
{
  SHM_SEATS_INDEX_KEY = ftok(".", key_count++);
  if (-1 == SHM_SEATS_INDEX_KEY) { FUNC_PERROR(); }
  SHM_SEATS_INFO_KEY = ftok(".", key_count++);
  if (-1 == SHM_SEATS_INFO_KEY) { FUNC_PERROR(); }
  SHM_WORKERS_PID_KEY = ftok(".", key_count++);
  if (-1 == SHM_WORKERS_PID_KEY) { FUNC_PERROR(); }
  SHM_TICKET_DISPENSER_PID_KEY = ftok(".", key_count++);
  if (-1 == SHM_TICKET_DISPENSER_PID_KEY) { FUNC_PERROR(); }
  SHM_CALENDAR_KEY = ftok(".", key_count++);
  if (-1 == SHM_CALENDAR_KEY) { FUNC_PERROR(); }
}

void msg_key_init(void)
{
  MSG_NOTIFY_WORKER_KEYS = (key_t*)malloc(sizeof(key_t) * (size_t)NOF_WORKERS);
  if (NULL == MSG_NOTIFY_WORKER_KEYS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    MSG_NOTIFY_WORKER_KEYS[i] = ftok(".", key_count++);
    if (-1 == MSG_NOTIFY_WORKER_KEYS[i]) { FUNC_PERROR(); }
  }
  MSG_NOTIFY_USER_KEYS = (key_t*)malloc(sizeof(key_t) * (size_t)NOF_USERS);
  if (NULL == MSG_NOTIFY_USER_KEYS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_USERS; i++)
  {
    MSG_NOTIFY_USER_KEYS[i] = ftok(".", key_count++);
    if (-1 == MSG_NOTIFY_USER_KEYS[i]) { FUNC_PERROR(); }
  }
  MSG_NOTIFY_DISPENSER_KEY = ftok(".", key_count++);
  if (-1 == MSG_NOTIFY_DISPENSER_KEY) { FUNC_PERROR(); }
  MSG_NOTIFY_CLOCK_KEY = ftok(".", key_count++);
  if (-1 == MSG_NOTIFY_CLOCK_KEY) { FUNC_PERROR(); }
}

void ftok_key_init(void)
{
  sem_key_init();
  semrw_key_init();
  shm_key_init();
  msg_key_init();
}
