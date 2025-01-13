#include <string.h>
#include <sys/sem.h>
#include <errno.h>
#include "sem_utils.h"
#include "macros.h"

union semun
{
  int val;
  struct semid_ds* buf;
  unsigned short* array;
  struct seminfo* __buf;
};

int init_sem_one(int semid, int sem_num)
{
  union semun arg;
  arg.val = 1;
  return semctl(semid, sem_num, SETVAL, arg);
}

int init_sem_zero(int semid, int sem_num)
{
  union semun arg;
  arg.val = 0;
  return semctl(semid, sem_num, SETVAL, arg);
}

int set_sem_val(int semid, int sem_num, int val)
{
  union semun arg;
  arg.val = val;
  return semctl(semid, sem_num, SETVAL, arg);
}

int lock_sem(int semid, int sem_num)
{
  struct sembuf sops;
  sops.sem_num = (short unsigned int)sem_num;
  sops.sem_op = -1;
  sops.sem_flg = 0;
  return semop(semid, &sops, 1);
}

int lock_sem_nowait(int semid, int sem_num)
{
  struct sembuf sops;
  sops.sem_num = (short unsigned int)sem_num;
  sops.sem_op = -1;
  sops.sem_flg = IPC_NOWAIT;
  int semop_res = semop(semid, &sops, 1);
  if (-1 == semop_res && EAGAIN != errno) { return -1; }
  else if (-1 == semop_res && EAGAIN == errno) { return -2; }
  else { return 0; }
}

int lock_sem_nowait_val(int semid, int sem_num, int val)
{
  if (val <= 0) { MSG_ERROR("val cant be negative.\n"); }
  if (val > 32767) { MSG_ERROR("val is too big.\n"); }
  struct sembuf sops;
  sops.sem_num = (short unsigned int)sem_num;
  sops.sem_op = (short int)-val;
  sops.sem_flg = IPC_NOWAIT;
  int semop_res = semop(semid, &sops, 1);
  if (-1 == semop_res && EAGAIN != errno) { return -1; }
  else if (-1 == semop_res && EAGAIN == errno) { return -2; }
  else { return 0; }
}

int lock_sem_val(int semid, int sem_num, int val)
{
  if (val <= 0) { MSG_ERROR("val cant be negative.\n"); }
  if (val > 32767) { MSG_ERROR("val is too big.\n"); }
  struct sembuf sops;
  sops.sem_num = (short unsigned int)sem_num;
  sops.sem_op = (short int)-val;
  sops.sem_flg = 0;
  return semop(semid, &sops, 1);
}

int wait_zero_sem(int semid, int sem_num)
{
  struct sembuf sops;
  sops.sem_num = (short unsigned int)sem_num;
  sops.sem_op = 0;
  sops.sem_flg = 0;
  return semop(semid, &sops, 1);
}

int release_sem(int semid, int sem_num)
{
  struct sembuf sops;
  sops.sem_num = (short unsigned int)sem_num;
  sops.sem_op = 1;
  sops.sem_flg = 0;
  return semop(semid, &sops, 1);
}

int release_sem_val(int semid, int sem_num, int val)
{
  struct sembuf sops;
  sops.sem_num = (short unsigned int)sem_num;
  sops.sem_op = (short int)val;
  sops.sem_flg = 0;
  return semop(semid, &sops, 1);
}

int release_all_sem(int semid, int sem_count)
{
  struct sembuf sops[sem_count];
  for (short unsigned int i = 0; i < sem_count; i++)
  {
    sops[i].sem_num = i;
    sops[i].sem_op = 1;
    sops[i].sem_flg = 0;
  }
  return semop(semid, sops, (short unsigned int)sem_count);
}

int release_all_sem_excl(int semid, int sem_count, int excluded)
{
  struct sembuf sops[sem_count - 1];
  int curr_index = 0;
  for (short unsigned int i = 0; i < sem_count; i++)
  {
    if (i == excluded) { continue; }
    sops[curr_index].sem_num = i;
    sops[curr_index].sem_op = 1;
    sops[curr_index].sem_flg = 0;
    curr_index++;
  }
  return semop(semid, sops, (short unsigned int)sem_count - 1);
}

int init_all_sem_one(int semid, int sem_count)
{
  union semun arg;
  unsigned short values[sem_count];
  memset(values, 1, sizeof(unsigned short) * (unsigned short)sem_count);
  arg.array = values;
  return semctl(semid, 0, SETALL, arg);
}

int init_all_sem_zero(int semid, int sem_count)
{
  union semun arg;
  unsigned short values[sem_count];
  memset(values, 0, sizeof(unsigned short) * (unsigned short)sem_count);
  arg.array = values;
  return semctl(semid, 0, SETALL, arg);
}

int lock_all_sem(int semid, int sem_count)
{
  struct sembuf sops[sem_count];
  for (short unsigned int i = 0; i < sem_count; i++)
  {
    sops[i].sem_num = i;
    sops[i].sem_op = -1;
    sops[i].sem_flg = 0;
  }
  return semop(semid, sops, (short unsigned int)sem_count);
}

int get_sem_value(int semid, int sem_count)
{
  union semun arg;
  arg.val = 0;
  return semctl(semid, sem_count, GETVAL, arg);
}

void lock_reader_RP(SemRP_Id sem_rp)
{
  if (-1 == lock_sem(sem_rp.sem_mutex_id, 0)) { FUNC_PERROR(); }
  if (-1 == release_sem(sem_rp.sem_reader_count_id, 0)) { FUNC_PERROR(); }
  int num_reader = get_sem_value(sem_rp.sem_reader_count_id, 0);
  if (-1 == num_reader) { FUNC_PERROR(); }
  else if (1 == num_reader)
  {
    if (-1 == lock_sem(sem_rp.sem_writer_id, 0)) { FUNC_PERROR(); }
  }
  if (-1 == release_sem(sem_rp.sem_mutex_id, 0)) { FUNC_PERROR(); }
}

void release_reader_RP(SemRP_Id sem_rp)
{
  if (-1 == lock_sem(sem_rp.sem_mutex_id, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(sem_rp.sem_reader_count_id, 0)) { FUNC_PERROR(); }
  int num_reader = get_sem_value(sem_rp.sem_reader_count_id, 0);
  if (-1 == num_reader) { FUNC_PERROR(); }
  else if (0 == num_reader)
  {
    if (-1 == release_sem(sem_rp.sem_writer_id, 0)) { FUNC_PERROR(); }
  }
  if (-1 == release_sem(sem_rp.sem_mutex_id, 0)) { FUNC_PERROR(); }
}

void lock_reader_WP(SemWP_Id sem_wp)
{
  if (-1 == lock_sem(sem_wp.sem_mutex_id, 0)) { FUNC_PERROR(); }
  int active_writer = get_sem_value(sem_wp.sem_aw_count_id, 0);
  int waiting_writer = get_sem_value(sem_wp.sem_ww_count_id, 0);
  if (-1 == active_writer || -1 == waiting_writer) { FUNC_PERROR(); }
  if (active_writer + waiting_writer > 0) { release_sem(sem_wp.sem_wr_count_id, 0); }
  else
  {
    if (-1 == release_sem(sem_wp.sem_ok_read_id, 0)) { FUNC_PERROR(); }
    if (-1 == release_sem(sem_wp.sem_ar_count_id, 0)) { FUNC_PERROR(); }
  }
  if (-1 == release_sem(sem_wp.sem_mutex_id, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(sem_wp.sem_ok_read_id, 0)) { FUNC_PERROR(); }
}

void release_reader_WP(SemWP_Id sem_wp)
{
  if (-1 == lock_sem(sem_wp.sem_mutex_id, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(sem_wp.sem_ar_count_id, 0)) { FUNC_PERROR(); }
  int active_reader = get_sem_value(sem_wp.sem_ar_count_id, 0);
  int waiting_writer = get_sem_value(sem_wp.sem_ww_count_id, 0);
  if (-1 == active_reader || -1 == waiting_writer) { FUNC_PERROR(); }
  if (0 == active_reader && waiting_writer > 0)
  {
    if (-1 == release_sem(sem_wp.sem_ok_write_id, 0)) { FUNC_PERROR(); }
    if (-1 == release_sem(sem_wp.sem_aw_count_id, 0)) { FUNC_PERROR(); }
    if (-1 == lock_sem(sem_wp.sem_ww_count_id, 0)) { FUNC_PERROR(); }
  }
  if (-1 == release_sem(sem_wp.sem_mutex_id, 0)) { FUNC_PERROR(); }
}

void lock_writer_WP(SemWP_Id sem_wp)
{
  if (-1 == lock_sem(sem_wp.sem_mutex_id, 0)) { FUNC_PERROR(); }
  int active_writer = get_sem_value(sem_wp.sem_aw_count_id, 0);
  int active_reader = get_sem_value(sem_wp.sem_ar_count_id, 0);
  if (-1 == active_reader || -1 == active_writer) { FUNC_PERROR(); }
  if (active_reader + active_writer > 0)
  {
    if (-1 == release_sem(sem_wp.sem_ww_count_id, 0)) { FUNC_PERROR(); }
  }
  else
  {
    if (-1 == release_sem(sem_wp.sem_ok_write_id, 0)) { FUNC_PERROR(); }
    if (-1 == release_sem(sem_wp.sem_aw_count_id, 0)) { FUNC_PERROR(); }
  }
  if (-1 == release_sem(sem_wp.sem_mutex_id, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(sem_wp.sem_ok_write_id, 0)) { FUNC_PERROR(); }
}

void release_writer_WP(SemWP_Id sem_wp)
{
  if (-1 == lock_sem(sem_wp.sem_mutex_id, 0)) { FUNC_PERROR(); }
  if (-1 == lock_sem(sem_wp.sem_aw_count_id, 0)) { FUNC_PERROR(); }
  int waiting_writer = get_sem_value(sem_wp.sem_ww_count_id, 0);
  int waiting_reader = get_sem_value(sem_wp.sem_wr_count_id, 0);
  if (-1 == waiting_writer) { FUNC_PERROR(); }
  if (waiting_writer > 0)
  {
    if (-1 == release_sem(sem_wp.sem_ok_write_id, 0)) { FUNC_PERROR(); }
    if (-1 == release_sem(sem_wp.sem_aw_count_id, 0)) { FUNC_PERROR(); }
    if (-1 == lock_sem(sem_wp.sem_ww_count_id, 0)) { FUNC_PERROR(); }
  }
  else if (waiting_reader > 0)
  {
    if (-1 == release_sem(sem_wp.sem_ok_read_id, 0)) { FUNC_PERROR(); }
    if (-1 == release_sem(sem_wp.sem_ar_count_id, 0)) { FUNC_PERROR(); }
    if (-1 == lock_sem(sem_wp.sem_wr_count_id, 0)) { FUNC_PERROR(); }
  }
  if (-1 == release_sem(sem_wp.sem_mutex_id, 0)) { FUNC_PERROR(); }
}
