#ifndef SEM_UTILS_H
#define SEM_UTILS_H

int init_sem_one(int semid, int sem_num);
int init_sem_zero(int semid, int sem_num);
int lock_sem(int semid, int sem_num);
int release_sem(int semid, int sem_num);
int init_all_sem_one(int semid, int sem_count);
int init_all_sem_zero(int semid, int sem_count);
int lock_all_sem(int semid, int sem_count);
int set_sem_val(int semid, int sem_num, int val);

#endif
