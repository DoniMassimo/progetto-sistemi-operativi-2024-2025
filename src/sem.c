#include <sys/ipc.h>
#include <sys/sem.h>
#include "ftok_key.h"
#include "sem_utils.h"
#include "config.h"
#include "macros.h"
#include "sem.h"

int SEM_START_ID = -1;
int SEM_DAY_STARTED_ID = -1;
int SEM_SEATS_ID = -1;
int SEM_SHM_SEATS_INFO_ID = -1;

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

void sem_init(void)
{
  SEM_START_ID_init();
  SET_SEATS_ID_init();
  SEM_SHM_SEATS_INFO_init();
  SEM_DAY_STARTED_init();
}

void sem_config(void)
{
  SEM_START_ID_config();
  SET_SEATS_ID_config();
  SEM_SHM_SEATS_INFO_config();
  SEM_DAY_STARTED_config();
}
