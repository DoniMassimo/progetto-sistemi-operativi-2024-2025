#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include "seats.h"
#include "sem_utils.h"
#include "ipc_config.h"
#include "config.h"
#include "macros.h"
#include "utils.h"

char REL_DIR[MAX_PATH_LEN];
int SEM_START_ID;
int SEM_SEATS_ID;
int SEM_SHM_SEATS_INFO_ID;
int SHM_SEATS_INFO_ID;
int SHM_SEATS_INDEX_ID;

void init_sem()
{
  key_t sem_key = ftok(".", 65);
  SEM_START_ID = semget(sem_key, 1, 0666 | IPC_CREAT);
  if (SEM_START_ID < 0) { FUNC_PERROR(); }
  key_t sem_seats_key = ftok(".", 68);
  SEM_SEATS_ID = semget(sem_seats_key, SERV_NUM, 0666 | IPC_CREAT);
  if (SEM_SEATS_ID < 0) { FUNC_PERROR(); }
  key_t sem_shme_seats_key = ftok(".", 99);
  SEM_SHM_SEATS_INFO_ID = semget(sem_shme_seats_key, 1, 0666 | IPC_CREAT);
  if (SEM_SHM_SEATS_INFO_ID < 0) { FUNC_PERROR(); }
  init_sem_one(SEM_SHM_SEATS_INFO_ID, 0);
}

void init_shm()
{
  key_t shm_sinfo_key = ftok(".", 93);
  if (-1 == shm_sinfo_key) { FUNC_PERROR(); }
  size_t sinfo_size = sizeof(SeatInfo) * NOF_WORKER_SEATS;
  SHM_SEATS_INFO_ID = shmget(shm_sinfo_key, sinfo_size, 0666 | IPC_CREAT);
  if (-1 == SHM_SEATS_INFO_ID) { FUNC_PERROR(); }
  SeatInfo* shm_sinfo_ptr = (SeatInfo*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
  if ((SeatInfo*)-1 == (SeatInfo*)shm_sinfo_ptr) { FUNC_PERROR(); }
  memset(shm_sinfo_ptr, 0, sinfo_size);
  if (-1 == shmdt(shm_sinfo_ptr)) { FUNC_PERROR(); }
  key_t shm_sindex_key = ftok(".", 72);
  if (-1 == shm_sindex_key) { FUNC_PERROR(); }
  size_t sindex_size = sizeof(int) * (SERV_NUM - 1);
  SHM_SEATS_INDEX_ID = shmget(shm_sindex_key, sindex_size, 0666 | IPC_CREAT);
  if (-1 == SHM_SEATS_INDEX_ID) { FUNC_PERROR(); }
  int* shm_sindex_ptr = (int*)shmat(SHM_SEATS_INDEX_ID, NULL, 0);
  if ((int*)-1 == (int*)shm_sindex_ptr) { FUNC_PERROR(); }
  memset(shm_sindex_ptr, 0, sindex_size);
  if (-1 == shmdt(shm_sindex_ptr)) { FUNC_PERROR(); }
}

void config_semid()
{
  key_t sem_key = ftok(".", 65);
  SEM_START_ID = semget(sem_key, 1, 0666);
  if (SEM_START_ID < 0) { FUNC_PERROR(); }
  key_t sem_seats_key = ftok(".", 68);
  SEM_SEATS_ID = semget(sem_seats_key, SERV_NUM, 0666);
  if (SEM_SEATS_ID < 0) { FUNC_PERROR(); }
  key_t sem_shme_seats_key = ftok(".", 99);
  SEM_SHM_SEATS_INFO_ID = semget(sem_shme_seats_key, 1, 0666);
  if (SEM_SHM_SEATS_INFO_ID < 0) { FUNC_PERROR(); }
}

void config_shmid()
{
  key_t shm_sinfo_key = ftok(".", 93);
  if (-1 == shm_sinfo_key) { FUNC_PERROR(); }
  size_t sinfo_size = sizeof(SeatInfo) * NOF_WORKER_SEATS;
  SHM_SEATS_INFO_ID = shmget(shm_sinfo_key, sinfo_size, 0666);
  if (-1 == SHM_SEATS_INFO_ID) { FUNC_PERROR(); }
  key_t shm_sindex_key = ftok(".", 72);
  if (-1 == shm_sindex_key) { FUNC_PERROR(); }
  size_t sindex_size = sizeof(int) * (SERV_NUM - 1);
  SHM_SEATS_INDEX_ID = shmget(shm_sindex_key, sindex_size, 0666);
  if (-1 == SHM_SEATS_INDEX_ID) { FUNC_PERROR(); }
}

void ipc_config_init_manager()
{
  init_sem();
  init_shm();
}

void ipc_config_init_worker()
{
  config_semid();
  config_shmid();
}
