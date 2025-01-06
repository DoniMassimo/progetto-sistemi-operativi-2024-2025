#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "seats.h"
#include "shm.h"

int SHM_SEATS_INFO_ID = -1;
int SHM_SEATS_INDEX_ID = -1;
int SHM_WORKERS_PID_ID = -1;
int SHM_TICKET_DISPENSER_PID_ID = -1;
int SHM_CALENDAR_ID = -1;

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

void SHM_TICKET_DISPENSER_PID_init(void)
{
  size_t ticket_disp_pid_size = sizeof(pid_t);
  SHM_TICKET_DISPENSER_PID_ID =
      shmget(SHM_TICKET_DISPENSER_PID_KEY, ticket_disp_pid_size, 0666 | IPC_CREAT);
  if (-1 == SHM_TICKET_DISPENSER_PID_ID) { FUNC_PERROR(); }
}

void SHM_TICKET_DISPENSER_PID_config(void)
{
  size_t ticket_disp_pid_size = sizeof(pid_t);
  SHM_TICKET_DISPENSER_PID_ID = shmget(SHM_TICKET_DISPENSER_PID_KEY, ticket_disp_pid_size, 0666);
  if (-1 == SHM_TICKET_DISPENSER_PID_ID) { FUNC_PERROR(); }
}

void SHM_CALENDAR_ID_init(void)
{
  size_t cal_size = sizeof(int) * 60 * 8;
  SHM_CALENDAR_ID = shmget(SHM_CALENDAR_KEY, cal_size, 0666 | IPC_CREAT);
  if (-1 == SHM_CALENDAR_ID) { FUNC_PERROR(); }
  int* shm_cal_ptr = (int*)shmat(SHM_CALENDAR_ID, NULL, 0);
  if ((int*)-1 == (int*)shm_cal_ptr) { FUNC_PERROR(); }
  memset(shm_cal_ptr, 0, cal_size);
  if (-1 == shmdt(shm_cal_ptr)) { FUNC_PERROR(); }
}

void SHM_CALENDAR_ID_config(void)
{
  size_t cal_size = sizeof(int) * 60 * 8;
  SHM_CALENDAR_ID = shmget(SHM_CALENDAR_KEY, cal_size, 0666);
  if (-1 == SHM_CALENDAR_ID) { FUNC_PERROR(); }
}

void shm_init(void)
{
  SHM_SEATS_INFO_init();
  SHM_SEATS_INDEX_init();
  SHM_WORKERS_PID_init();
  SHM_TICKET_DISPENSER_PID_init();
  SHM_CALENDAR_ID_init();
}

void shm_config(void)
{
  SHM_SEATS_INFO_config();
  SHM_SEATS_INDEX_config();
  SHM_WORKERS_PID_config();
  SHM_TICKET_DISPENSER_PID_config();
  SHM_CALENDAR_ID_config();
}

void shm_deallocate(void)
{
  if (-1 == shmctl(SHM_SEATS_INFO_ID, IPC_RMID, NULL)) { FUNC_PERROR(); }
  if (-1 == shmctl(SHM_SEATS_INDEX_ID, IPC_RMID, NULL)) { FUNC_PERROR(); }
  if (-1 == shmctl(SHM_WORKERS_PID_ID, IPC_RMID, NULL)) { FUNC_PERROR(); }
  if (-1 == shmctl(SHM_TICKET_DISPENSER_PID_ID, IPC_RMID, NULL)) { FUNC_PERROR(); }
}
