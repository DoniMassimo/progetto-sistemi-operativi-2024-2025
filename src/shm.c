#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "seats.h"
#include "shm.h"

int SHM_SEATS_INFO_ID;
int SHM_SEATS_INDEX_ID;

void SHM_SEATS_INFO_init(void)
{
  size_t sinfo_size = sizeof(SeatInfo) * (size_t)NOF_WORKER_SEATS;
  SHM_SEATS_INFO_ID = shmget(SHM_SEATS_INFO_KEY, sinfo_size, 0666 | IPC_CREAT);
  if (-1 == SHM_SEATS_INFO_ID) { FUNC_PERROR(); }
  SeatInfo* shm_sinfo_ptr = (SeatInfo*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
  if ((SeatInfo*)-1 == (SeatInfo*)shm_sinfo_ptr) { FUNC_PERROR(); }
  memset(shm_sinfo_ptr, 0, sinfo_size);
  if (-1 == shmdt(shm_sinfo_ptr)) { FUNC_PERROR(); }
}

void SHM_SEATS_INFO_config(void)
{
  size_t sinfo_size = sizeof(SeatInfo) * (size_t)NOF_WORKER_SEATS;
  SHM_SEATS_INFO_ID = shmget(SHM_SEATS_INFO_KEY, sinfo_size, 0666);
  if (-1 == SHM_SEATS_INFO_ID) { FUNC_PERROR(); }
}

void SHM_SEATS_INDEX_init(void)
{
  size_t sindex_size = sizeof(int) * (SERV_NUM - 1);
  SHM_SEATS_INDEX_ID = shmget(SHM_SEATS_INDEX_KEY, sindex_size, 0666 | IPC_CREAT);
  if (-1 == SHM_SEATS_INDEX_ID) { FUNC_PERROR(); }
  int* shm_sindex_ptr = (int*)shmat(SHM_SEATS_INDEX_ID, NULL, 0);
  if ((int*)-1 == (int*)shm_sindex_ptr) { FUNC_PERROR(); }
  memset(shm_sindex_ptr, 0, sindex_size);
  if (-1 == shmdt(shm_sindex_ptr)) { FUNC_PERROR(); }
}

void SHM_SEATS_INDEX_config(void)
{
  size_t sindex_size = sizeof(int) * (SERV_NUM - 1);
  SHM_SEATS_INDEX_ID = shmget(SHM_SEATS_INDEX_KEY, sindex_size, 0666);
  if (-1 == SHM_SEATS_INDEX_ID) { FUNC_PERROR(); }
}

void shm_init(void)
{
  SHM_SEATS_INFO_init();
  SHM_SEATS_INDEX_init();
}

void shm_config(void)
{
  SHM_SEATS_INFO_config();
  SHM_SEATS_INDEX_config();
}
