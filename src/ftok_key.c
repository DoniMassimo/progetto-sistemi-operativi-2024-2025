#include <sys/ipc.h>
#include <sys/types.h>
#include "ftok_key.h"
#include "macros.h"

key_t key_count = 1;

key_t SEM_START_KEY = -1;
key_t SEM_SEATS_KEY = -1;
key_t SEM_SHM_SEATS_INFO_KEY = -1;
key_t SEM_DAY_STARTED_KEY = -1;

key_t SHM_SEATS_INDEX_KEY = -1;
key_t SHM_SEATS_INFO_KEY = -1;

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
}

void shm_key_init(void)
{
  SHM_SEATS_INDEX_KEY = ftok(".", key_count++);
  if (-1 == SHM_SEATS_INDEX_KEY) { FUNC_PERROR(); }
  SHM_SEATS_INFO_KEY = ftok(".", key_count++);
  if (-1 == SHM_SEATS_INFO_KEY) { FUNC_PERROR(); }
}

void ftok_key_init(void)
{
  sem_key_init();
  shm_key_init();
}
