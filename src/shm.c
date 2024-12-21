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
int SHM_WORKERS_PID_ID;

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

void SHM_WORKERS_PID_init(void)
{
  size_t workers_pid_size = sizeof(pid_t) * (size_t)NOF_WORKERS;
  SHM_WORKERS_PID_ID = shmget(SHM_WORKERS_PID_KEY, workers_pid_size, 0666 | IPC_CREAT);
  if (-1 == SHM_WORKERS_PID_ID) { FUNC_PERROR(); }
  pid_t* workers_pid_ptr = shmat(SHM_WORKERS_PID_ID, NULL, 0);
  if ((pid_t*)-1 == (pid_t*)workers_pid_ptr) { FUNC_PERROR(); }
  memset(workers_pid_ptr, 0, workers_pid_size);
  if (-1 == shmdt(workers_pid_ptr)) { FUNC_PERROR(); }
}

void SHM_WORKERS_PID_config(void)
{
  size_t workers_pid_size = sizeof(pid_t) * (size_t)NOF_WORKERS;
  SHM_WORKERS_PID_ID = shmget(SHM_WORKERS_PID_KEY, workers_pid_size, 0666);
  if (-1 == SHM_WORKERS_PID_ID) { FUNC_PERROR(); }
}

void shm_init(void)
{
  SHM_SEATS_INFO_init();
  SHM_SEATS_INDEX_init();
  SHM_WORKERS_PID_init();
}

void shm_config(void)
{
  SHM_SEATS_INFO_config();
  SHM_SEATS_INDEX_config();
  SHM_WORKERS_PID_config();
}
