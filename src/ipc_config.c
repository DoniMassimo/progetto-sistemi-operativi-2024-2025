#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include "sem_utils.h"
#include "ipc_config.h"
#include "config.h"
#include "macros.h"
#include "utils.h"

char REL_DIR[MAX_PATH_LEN];
int START_SEM_ID;
int SEM_SEATS_ID;
int SHM_NOF_SEATS_ID;
int SEM_SHM_NOF_SEATS_ID;
int SHM_SEATS_INFO_ID[SERV_NUM];
int SEM_SHM_SEATS_INFO_ID;

void ipc_config_sem()
{
  key_t sem_key = ftok(".", 65);
  START_SEM_ID = semget(sem_key, 1, 0666 | IPC_CREAT);
  if (START_SEM_ID < 0) { FUNC_PERROR(); }
  key_t sem_seats_key = ftok(".", 68);
  SEM_SEATS_ID = semget(sem_seats_key, SERV_NUM, 0666 | IPC_CREAT);
  if (SEM_SEATS_ID < 0) { FUNC_PERROR(); }
  key_t sem_shme_seats_key = ftok(".", 99);
  SEM_SHM_SEATS_INFO_ID = semget(sem_shme_seats_key, 1, 0666 | IPC_CREAT);
  if (SEM_SHM_SEATS_INFO_ID < 0) { FUNC_PERROR(); }
  init_sem_one(SEM_SHM_SEATS_INFO_ID, 0);
}

void ipc_config_shmem_seats_info(size_t mem_size, size_t struct_num, Service ser_type)
{
  mem_size = mem_size * struct_num;
  if (mem_size > 0)
  {
    char str[MAX_PATH_LEN + MAX_EXE_LEN];
    utils_join_str(str, REL_DIR, "employee_main");
    key_t shmem_sinfo_key = ftok(str, (int)(10 + ser_type));
    SHM_SEATS_INFO_ID[ser_type] = shmget(shmem_sinfo_key, mem_size, 0666 | IPC_CREAT);
    if (SHM_SEATS_INFO_ID[ser_type] < 0) { FUNC_PERROR(); }
    void* shm_sinfo_ptr = (void*)shmat(SHM_SEATS_INFO_ID[ser_type], NULL, 0);
    if ((void*)-1 == (void*)shm_sinfo_ptr) { FUNC_PERROR(); }
    memset(shm_sinfo_ptr, 0, mem_size);
    if (-1 == shmdt(shm_sinfo_ptr)) { FUNC_PERROR(); }
  }
  int* shm_nof_seats_ptr = (int*)shmat(SHM_NOF_SEATS_ID, NULL, 0);
  if ((int*)-1 == (int*)shm_nof_seats_ptr) { FUNC_PERROR(); }
  shm_nof_seats_ptr[ser_type] = (int)struct_num;
  if (-1 == shmdt(shm_nof_seats_ptr)) { FUNC_PERROR(); }
}

int get_nof_seats_for_serv_a(Service ser_type)
{
  int* shm_nof_seats_ptr = (int*)shmat(SHM_NOF_SEATS_ID, NULL, 0);
  if ((void*)-1 == (void*)shm_nof_seats_ptr) { FUNC_PERROR(); }
  int nof_seats = shm_nof_seats_ptr[ser_type];
  if (-1 == shmdt(shm_nof_seats_ptr)) { FUNC_PERROR(); }
  return nof_seats;
}

void ipc_config_shm()
{
  size_t shmem_nof_seats_size = sizeof(int) * SERV_NUM;
  key_t shmem_nof_seats_key = ftok(".", 72);
  if (-1 == shmem_nof_seats_key) { FUNC_PERROR(); }
  SHM_NOF_SEATS_ID = shmget(shmem_nof_seats_key, shmem_nof_seats_size, 0666 | IPC_CREAT);
  if (-1 == SHM_NOF_SEATS_ID) { FUNC_PERROR(); }
}

void ipc_config_shm_worker()
{
  for (Service ser_type = 0; ser_type < SERV_NUM; ser_type++)
  {
    if (0 == get_nof_seats_for_serv_a(ser_type)) { continue; }
    char str[MAX_PATH_LEN + MAX_EXE_LEN];
    utils_join_str(str, REL_DIR, "employee_main");
    key_t shmem_sinfo_key = ftok(str, (int)(10 + ser_type));
    SHM_SEATS_INFO_ID[ser_type] = shmget(shmem_sinfo_key, 0, 0666);
    if (SHM_SEATS_INFO_ID[ser_type] < 0) { FUNC_PERROR(); }
  }
}

void ipc_init_shm()
{
  size_t shmem_nof_seats_size = sizeof(int) * SERV_NUM;
  int* shm_nof_seats_ptr = (int*)shmat(SHM_NOF_SEATS_ID, NULL, 0);
  if ((int*)-1 == shm_nof_seats_ptr) { FUNC_PERROR(); }
  memset(shm_nof_seats_ptr, 0, shmem_nof_seats_size);
  if (-1 == shmdt(shm_nof_seats_ptr)) { FUNC_PERROR(); }
}




void ipc_config_init_manager()
{
  ipc_config_sem();
  ipc_config_shm();
  ipc_init_shm();
}

void ipc_config_init_worker()
{
  ipc_config_sem();
  ipc_config_shm();
  ipc_config_shm_worker();
}
