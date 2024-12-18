#include <sys/msg.h>
#include <sys/shm.h>
#include "utils.h"
#include "macros.h"
#include "config.h"
#include "ipc_config.h"
#include "sem_utils.h"
#include "seats.h"

void seats_init_resources()
{
  int ope_res = init_sem_zero(START_SEM_ID, 0);
  if (-1 == ope_res) { FUNC_PERROR(); }
  int assigned_serv_seats[SERV_NUM];
  utils_assign_count_array(assigned_serv_seats, SERV_NUM, NOF_WORKER_SEATS);
  for (int i = 0; i < SERV_NUM; i++)
  {
    int seats_num = assigned_serv_seats[i];
    Service ser = (Service)i;
    int ope_res = set_sem_val(SEM_SEATS_ID, ser, seats_num);
    if (-1 == ope_res) { FUNC_PERROR(); }
    ipc_config_shmem_seats_info(sizeof(SeatsInfo), (size_t)seats_num, ser);
  }
}

int get_nof_seats_for_serv(Service ser_type)
{
  int* shm_nof_seats_ptr = (int*)shmat(SHM_NOF_SEATS_ID, NULL, 0);
  if ((void*)-1 == (void*)shm_nof_seats_ptr) { FUNC_PERROR(); }
  int nof_seats = shm_nof_seats_ptr[ser_type];
  if (-1 == shmdt(shm_nof_seats_ptr)) { FUNC_PERROR(); }
  return nof_seats;
}

int seats_try_take_seat(Service ser_type)
{
  if (-1 == lock_sem(SEM_SEATS_ID, 0)) { FUNC_PERROR(); }
  SeatsInfo* shm_sinfo_ptr = (SeatsInfo*)shmat(SHM_SEATS_INFO_ID[ser_type], NULL, 0);
  if ((void*)-1 == (void*)shm_sinfo_ptr) { FUNC_PERROR(); }
  int nof_seats_serv = get_nof_seats_for_serv(ser_type);
  if (-1 == lock_sem(SEM_SHM_SEATS_INFO_ID, 0)) { FUNC_PERROR(); }
  int msgid = -2;
  for (int i = 0; i < nof_seats_serv; i++)
  {
    SeatsInfo* seats_info = &shm_sinfo_ptr[i];
    if (0 == seats_info->seats_taken)
    {
      seats_info->seats_taken = 1;
      char str[MAX_PATH_LEN + MAX_EXE_LEN];
      utils_join_str(str, REL_DIR, "employee_main");
      key_t msgkey = ftok(str, 65);
      if (-1 == msgkey) { FUNC_PERROR(); }
      msgid = msgget(msgkey, 0666 | IPC_CREAT);
      if (-1 == msgid) { FUNC_PERROR(); }
      seats_info->empoyee_mqueue_id = msgid;
      break;
    }
  }
  if (-1 == release_sem(SEM_SHM_SEATS_INFO_ID, 0)) { FUNC_PERROR(); }
  if (-2 == msgid) { FUNC_MSG_ERROR("Expecting to find free seats."); }
  if (-1 == shmdt(shm_sinfo_ptr)) { FUNC_PERROR(); }
  return msgid;
}
