#include <sys/ipc.h>
#include <sys/sem.h>
#include "ftok_key.h"
#include "sem_utils.h"
#include "config.h"
#include "macros.h"
#include "sem.h"

int SEM_START_ID = -1;
int SEM_DAY_STARTED_ID = -1;
int SEM_DAY_END_ID = -1;
int SEM_SEATS_ID = -1;
int SEM_SHM_SEATS_INFO_ID = -1;
int SEM_PROC_READY_ID = -1;
int* SEM_NOTIFY_WORKER_IDS = NULL;

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

void SEM_NOTIFY_WORKER_IDS_init(void)
{
  SEM_NOTIFY_WORKER_IDS = (int*)malloc(sizeof(int) * (size_t)NOF_WORKERS);
  if (NULL == SEM_NOTIFY_WORKER_IDS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    SEM_NOTIFY_WORKER_IDS[i] = semget(SEM_NOTIFY_WORKER_KEYS[i], 1, 0666 | IPC_CREAT);
    if (-1 == SEM_NOTIFY_WORKER_IDS[i]) { FUNC_PERROR(); }
    init_sem_zero(SEM_NOTIFY_WORKER_IDS[i], 0);
  }
}

void SEM_NOTIFY_WORKER_IDS_config(void)
{
  SEM_NOTIFY_WORKER_IDS = (int*)malloc(sizeof(int) * (size_t)NOF_WORKERS);
  if (NULL == SEM_NOTIFY_WORKER_IDS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    SEM_NOTIFY_WORKER_IDS[i] = semget(SEM_NOTIFY_WORKER_KEYS[i], 1, 0666);
    if (-1 == SEM_NOTIFY_WORKER_IDS[i]) { FUNC_PERROR(); }
  }
}

void sem_init(void)
{
  SEM_START_ID_init();
  SET_SEATS_ID_init();
  SEM_SHM_SEATS_INFO_init();
  SEM_DAY_STARTED_init();
  SEM_DAY_END_init();
  SEM_PROC_READY_ID_init();
  SEM_NOTIFY_WORKER_IDS_init();
}

void sem_config(void)
{
  SEM_START_ID_config();
  SET_SEATS_ID_config();
  SEM_SHM_SEATS_INFO_config();
  SEM_DAY_STARTED_config();
  SEM_DAY_END_config();
  SEM_PROC_READY_ID_config();
  SEM_NOTIFY_WORKER_IDS_config();
}
