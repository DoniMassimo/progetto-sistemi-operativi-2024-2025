#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sem_utils.h"

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

int release_sem(int semid, int sem_num)
{
  struct sembuf sops;
  sops.sem_num = (short unsigned int)sem_num;
  sops.sem_op = 1;
  sops.sem_flg = 0;
  return semop(semid, &sops, 1);
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
