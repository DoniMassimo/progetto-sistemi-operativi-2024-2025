#ifndef SEM_UTILS_H
#define SEM_UTILS_H

typedef struct
{
  int sem_mutex_id;
  int sem_reader_count_id;
  int sem_writer_id;
} SemRW_Id;

int init_sem_one(int semid, int sem_num);
int init_sem_zero(int semid, int sem_num);
int lock_sem(int semid, int sem_num);
int lock_sem_nowait(int semid, int sem_num);
int lock_sem_val(int semid, int sem_num, int val);
int release_sem(int semid, int sem_num);
int release_all_sem(int semid, int sem_count);
int init_all_sem_one(int semid, int sem_count);
int init_all_sem_zero(int semid, int sem_count);
int lock_all_sem(int semid, int sem_count);
int set_sem_val(int semid, int sem_num, int val);
int get_sem_value(int semid, int sem_count);
void lock_reader(SemRW_Id sem_rw);
void release_reader(SemRW_Id sem_rw);

#endif
