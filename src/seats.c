#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
#include "config.h"
#include "macros.h"
#include "seats.h"
#include "sem.h"
#include "sem_utils.h"
#include "shm.h"
#include "msg.h"
#include "utils.h"
#include "sig_utils.h"

void seats_init_resources(int* assigned_serv_seats)
{
  int* shm_sindex_ptr = (int*)shmat(SHM_SEATS_INDEX_ID, NULL, 0);
  if ((int*)-1 == (int*)shm_sindex_ptr) { FUNC_PERROR(); }
  SeatInfo* shm_sinfo_ptr = (SeatInfo*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
  if ((void*)-1 == (void*)shm_sinfo_ptr) { FUNC_PERROR(); }
  int seats_count = 0;
  for (int i = 0; i < SERV_NUM; i++)
  {
    int seats_num = assigned_serv_seats[i];
    seats_count = seats_count + seats_num;
    if (i < SERV_NUM - 1) { shm_sindex_ptr[i] = seats_count; }
    Service ser = (Service)i;
    int ope_res = set_sem_val(SEM_SEATS_ID, ser, seats_num);
    if (-1 == ope_res) { FUNC_PERROR(); }
  }
  if (-1 == shmdt(shm_sindex_ptr)) { FUNC_PERROR(); }
  if (-1 == shmdt(shm_sinfo_ptr)) { FUNC_PERROR(); }
}

void get_bounds_serv(int* bounds, Service serv)
{
  int* shm_sindex_ptr = (int*)shmat(SHM_SEATS_INDEX_ID, NULL, 0);
  if ((void*)-1 == (void*)shm_sindex_ptr) { FUNC_PERROR(); }
  bounds[0] = (0 == serv) ? 0 : shm_sindex_ptr[serv - 1];
  bounds[1] = (serv == SERV_NUM - 1) ? NOF_WORKER_SEATS : shm_sindex_ptr[serv];
  if (-1 == shmdt(shm_sindex_ptr)) { FUNC_PERROR(); }
}

int seats_try_take_seat(Service serv, int worker_id)
{
  int semop_seats_res = lock_sem_nowait(SEM_SEATS_ID, serv);
  if (-1 == semop_seats_res) { FUNC_PERROR(); }
  else if (-2 == semop_seats_res) { return -2; }
  SeatInfo* shm_sinfo_ptr = (SeatInfo*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
  if ((void*)-1 == (void*)shm_sinfo_ptr) { FUNC_PERROR(); }
  int sinfo_serv_bounds[2];
  get_bounds_serv(sinfo_serv_bounds, serv);
  if (-1 == lock_sem(SEM_SHM_SEATS_INFO_ID, 0)) { FUNC_PERROR(); }
  int seat_index = -1;
  for (int i = sinfo_serv_bounds[0]; i < sinfo_serv_bounds[1]; i++)
  {
    if (0 == shm_sinfo_ptr[i].seats_taken)
    {
      seat_index = i;
      shm_sinfo_ptr[i].seats_taken = 1;
      shm_sinfo_ptr[i].msg_notify_worker_id = MSG_NOTIFY_WORKER_IDS[worker_id];
      shm_sinfo_ptr[i].sem_notify_worker_count = worker_id;
      break;
    }
  }
  if (-1 == release_sem(SEM_SHM_SEATS_INFO_ID, 0)) { FUNC_PERROR(); }
  if (-1 == seat_index) { FUNC_MSG_ERROR("Expecting to find free seats."); }
  if (-1 == shmdt(shm_sinfo_ptr)) { FUNC_PERROR(); }
  return 0;
}

void seats_release_seat(Service serv, int seat_index)
{
  SeatInfo* shm_sinfo_ptr = (SeatInfo*)shmat(SHM_SEATS_INFO_ID, NULL, 0);
  if ((void*)-1 == (void*)shm_sinfo_ptr) { FUNC_PERROR(); }
  shm_sinfo_ptr[seat_index].seats_taken = 0;
  if (-1 == shmdt(shm_sinfo_ptr)) { FUNC_PERROR(); }
  if (-1 == release_sem(SEM_SEATS_ID, serv)) { FUNC_PERROR(); }
}
