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
key_t* SEM_NOTIFY_WORKER_KEYS = NULL;
key_t SEM_NOTIFY_DISPENSER_KEY = -1;

key_t SHM_SEATS_INDEX_KEY = -1;
key_t SHM_SEATS_INFO_KEY = -1;
key_t SHM_WORKERS_PID_KEY = -1;
key_t SHM_TICKET_DISPENSER_PID_KEY = -1;

key_t* MSG_SEATS_QUEUE_KEYS = NULL;
key_t MSG_TICKET_DISPENSER_KEY = -1;

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
  SEM_NOTIFY_WORKER_KEYS = (key_t*)malloc(sizeof(key_t) * (size_t)NOF_WORKERS);
  if (NULL == SEM_NOTIFY_WORKER_KEYS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    SEM_NOTIFY_WORKER_KEYS[i] = ftok(".", key_count++);
    if (-1 == SEM_NOTIFY_WORKER_KEYS[i]) { FUNC_PERROR(); }
  }
  SEM_DAY_END_KEY = ftok(".", key_count++);
  if (-1 == SEM_DAY_END_KEY) { FUNC_PERROR(); }
  SEM_NOTIFY_DISPENSER_KEY = ftok(".", key_count++);
  if (-1 == SEM_NOTIFY_DISPENSER_KEY) { FUNC_PERROR(); }
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
}

void msg_key_init(void)
{
  MSG_SEATS_QUEUE_KEYS = (key_t*)malloc(sizeof(key_t) * (size_t)NOF_WORKER_SEATS);
  if (NULL == MSG_SEATS_QUEUE_KEYS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKER_SEATS; i++)
  {
    MSG_SEATS_QUEUE_KEYS[i] = ftok(".", key_count++);
    if (-1 == MSG_SEATS_QUEUE_KEYS[i]) { FUNC_PERROR(); }
  }
  MSG_TICKET_DISPENSER_KEY = ftok(".", key_count++);
  if (-1 == MSG_TICKET_DISPENSER_KEY) { FUNC_PERROR(); }
}

void ftok_key_init(void)
{
  sem_key_init();
  shm_key_init();
  msg_key_init();
}
