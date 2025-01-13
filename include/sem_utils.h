#ifndef SEM_UTILS_H
#define SEM_UTILS_H

typedef struct
{
  int sem_mutex_id;
  int sem_reader_count_id;
  int sem_writer_id;
} SemRP_Id;

typedef struct
{
  int sem_mutex_id;
  int sem_ar_count_id;
  int sem_wr_count_id;
  int sem_aw_count_id;
  int sem_ww_count_id;
  int sem_ok_read_id;
  int sem_ok_write_id;
} SemWP_Id;

int init_sem_one(int semid, int sem_num);
int init_sem_zero(int semid, int sem_num);
int lock_sem(int semid, int sem_num);
int lock_sem_nowait(int semid, int sem_num);
int lock_sem_val(int semid, int sem_num, int val);
int wait_zero_sem(int semid, int sem_num);
int release_sem(int semid, int sem_num);
int release_sem_val(int semid, int sem_num, int val);
int release_all_sem(int semid, int sem_count);
int release_all_sem_excl(int semid, int sem_count, int excluded);
int init_all_sem_one(int semid, int sem_count);
int init_all_sem_zero(int semid, int sem_count);
int lock_all_sem(int semid, int sem_count);
int set_sem_val(int semid, int sem_num, int val);
int get_sem_value(int semid, int sem_count);
void lock_reader_RP(SemRP_Id sem_rw);
void release_reader_RP(SemRP_Id sem_rw);
//https://www.cs.kent.edu/~farrell/osf03/oldnotes/L15.pdf
void lock_reader_WP(SemWP_Id sem_wp);
void release_reader_WP(SemWP_Id sem_wp);
void lock_writer_WP(SemWP_Id sem_wp);
void release_writer_WP(SemWP_Id sem_wp);

#endif
