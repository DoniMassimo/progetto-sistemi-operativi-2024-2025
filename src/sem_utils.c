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

int lock_sem_val(int semid, int sem_num, int val)
{
  if (val <= 0) { MSG_ERROR("val cant be negative.\n"); }
  struct sembuf sops;
  sops.sem_num = (short unsigned int)sem_num;
  sops.sem_op = -val;
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
