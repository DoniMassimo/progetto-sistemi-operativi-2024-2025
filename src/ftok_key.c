#include <sys/ipc.h>
#include <sys/types.h>
#include "ftok_key.h"
#include "macros.h"
#include "config.h"

key_t key_count = 1;
key_t* key_array = NULL;
size_t size_key = 0;
size_t nof_key = 0;
char* paths[] = {".", "/tmp", "/etc/hostname", "/var"};
int path_index = 0;
int nof_paths = 4;

key_t SEM_START_KEY = -1;
key_t SEM_SEATS_KEY = -1;
key_t SEM_SHM_SEATS_INFO_KEY = -1;
key_t SEM_DAY_STARTED_KEY = -1;
key_t SEM_DAY_END_KEY = -1;
key_t SEM_PROC_READY_KEY = -1;
key_t SEM_NOTIFY_WORKER_KEY = -1;
key_t SEM_NOTIFY_DISPENSER_KEY = -1;
key_t SEM_NOTIFY_USER_KEY = -1;
key_t SEM_NOTIFY_CLOCK_KEY = -1;
key_t SEM_ADD_USERS_KEY = -1;
key_t SEM_CLOCK_ADD_USERS_KEY = -1;
key_t SEM_PROC_CAN_DIE_KEY = -1;
key_t SEM_ALL_PROC_STOP_KEY = -1;
SemRP_Key SEMRP_CALENDAR_STRUCT_KEY = {0};
SemRP_Key SEMRP_MIN_COUNT_STRUCT_KEY = {0};
SemWP_Key SEMWP_SEATS_INFO_STRUCT_KEY = {0};

key_t SHM_SEATS_INDEX_KEY = -1;
key_t SHM_SEATS_INFO_KEY = -1;
key_t SHM_WORKERS_PID_KEY = -1;
key_t SHM_TICKET_DISPENSER_PID_KEY = -1;
key_t SHM_CALENDAR_KEY = -1;
key_t SHM_MIN_COUNT_KEY = -1;

key_t* MSG_NOTIFY_WORKER_KEYS = NULL;
key_t MSG_NOTIFY_DISPENSER_KEY = -1;
key_t* MSG_NOTIFY_USER_KEYS = NULL;
key_t MSG_NOTIFY_CLOCK_KEY = -1;
key_t MSG_STATS_DATA_KEY = -1;
key_t MSG_STATS_METADATA_KEY = -1;

int check_exist_key(key_t new_key, int* insert_pos)
{
  int left = 0;
  int right = (int)nof_key - 1;
  while (left <= right)
  {
    int center = left + (right - left) / 2;
    if (key_array[center] == new_key) { return center; }
    if (key_array[center] < new_key) { left = center + 1; }
    else { right = center - 1; }
  }
  *insert_pos = left;
  return -1;
}

key_t generate_key(void)
{
  if (NULL == key_array || 0 == size_key)
  {
    size_key = 16;
    key_array = (key_t*)calloc(size_key, sizeof(key_t));
    if (NULL == key_array) { FUNC_PERROR(); }
  }
  else if (nof_key >= size_key)
  {
    size_key *= 2;
    key_array = (key_t*)realloc(key_array, sizeof(key_t) * size_key);
    if (NULL == key_array) { FUNC_PERROR(); }
  }
  int insert_pos = -1;
  key_t new_key;
  while (1)
  {
    if (key_count >= 256)
    {
      path_index++;
      if (path_index >= 4) { MSG_ERROR("Paths to generate keys are no longer available"); }
      key_count = 1;
    }
    new_key = ftok(paths[path_index], key_count++);
    if (-1 == new_key) { FUNC_PERROR(); }
    int key_exist = check_exist_key(new_key, &insert_pos);
    if (-1 == key_exist) { break; }
  }
  if (-1 == insert_pos) { MSG_ERROR("Invalid insert_pos value"); }
  size_t move_size = (nof_key - (size_t)insert_pos) * sizeof(key_t);
  if (move_size > 0) { memmove(key_array + insert_pos + 1, key_array + insert_pos, move_size); }
  key_array[insert_pos] = new_key;
  nof_key++;
  return new_key;
}

void sem_key_init(void)
{
  SEM_START_KEY = generate_key();
  if (-1 == SEM_START_KEY) { FUNC_PERROR(); }
  SEM_SEATS_KEY = generate_key();
  if (-1 == SEM_SEATS_KEY) { FUNC_PERROR(); }
  SEM_SHM_SEATS_INFO_KEY = generate_key();
  if (-1 == SEM_SHM_SEATS_INFO_KEY) { FUNC_PERROR(); }
  SEM_DAY_STARTED_KEY = generate_key();
  if (-1 == SEM_DAY_STARTED_KEY) { FUNC_PERROR(); }
  SEM_PROC_READY_KEY = generate_key();
  if (-1 == SEM_PROC_READY_KEY) { FUNC_PERROR(); }
  SEM_NOTIFY_WORKER_KEY = generate_key();
  if (-1 == SEM_NOTIFY_WORKER_KEY) { FUNC_PERROR(); }
  SEM_DAY_END_KEY = generate_key();
  if (-1 == SEM_DAY_END_KEY) { FUNC_PERROR(); }
  SEM_NOTIFY_DISPENSER_KEY = generate_key();
  if (-1 == SEM_NOTIFY_DISPENSER_KEY) { FUNC_PERROR(); }
  SEM_NOTIFY_USER_KEY = generate_key();
  if (-1 == SEM_NOTIFY_USER_KEY) { FUNC_PERROR(); }
  SEM_NOTIFY_CLOCK_KEY = generate_key();
  if (-1 == SEM_NOTIFY_CLOCK_KEY) { FUNC_PERROR(); }
  SEM_ADD_USERS_KEY = generate_key();
  if (-1 == SEM_ADD_USERS_KEY) { FUNC_PERROR(); }
  SEM_CLOCK_ADD_USERS_KEY = generate_key();
  if (-1 == SEM_CLOCK_ADD_USERS_KEY) { FUNC_PERROR(); }
  SEM_PROC_CAN_DIE_KEY = generate_key();
  if (-1 == SEM_PROC_CAN_DIE_KEY) { FUNC_PERROR(); }
  SEM_ALL_PROC_STOP_KEY = generate_key();
  if (-1 == SEM_ALL_PROC_STOP_KEY) { FUNC_PERROR(); }
}

void semrp_key_init(void)
{
  SEMRP_CALENDAR_STRUCT_KEY.sem_mutex_key = generate_key();
  if (-1 == SEMRP_CALENDAR_STRUCT_KEY.sem_mutex_key) { FUNC_PERROR(); }
  SEMRP_CALENDAR_STRUCT_KEY.sem_reader_count_key = generate_key();
  if (-1 == SEMRP_CALENDAR_STRUCT_KEY.sem_reader_count_key) { FUNC_PERROR(); }
  SEMRP_CALENDAR_STRUCT_KEY.sem_writer_key = generate_key();
  if (-1 == SEMRP_CALENDAR_STRUCT_KEY.sem_writer_key) { FUNC_PERROR(); }

  SEMRP_MIN_COUNT_STRUCT_KEY.sem_mutex_key = generate_key();
  if (-1 == SEMRP_MIN_COUNT_STRUCT_KEY.sem_mutex_key) { FUNC_PERROR(); }
  SEMRP_MIN_COUNT_STRUCT_KEY.sem_reader_count_key = generate_key();
  if (-1 == SEMRP_MIN_COUNT_STRUCT_KEY.sem_reader_count_key) { FUNC_PERROR(); }
  SEMRP_MIN_COUNT_STRUCT_KEY.sem_writer_key = generate_key();
  if (-1 == SEMRP_MIN_COUNT_STRUCT_KEY.sem_writer_key) { FUNC_PERROR(); }
}

void semwp_key_init(void)
{
  SEMWP_SEATS_INFO_STRUCT_KEY.sem_mutex_key = generate_key();
  if (-1 == SEMWP_SEATS_INFO_STRUCT_KEY.sem_mutex_key) { FUNC_PERROR(); }
  SEMWP_SEATS_INFO_STRUCT_KEY.sem_ar_count_key = generate_key();
  if (-1 == SEMWP_SEATS_INFO_STRUCT_KEY.sem_ar_count_key) { FUNC_PERROR(); }
  SEMWP_SEATS_INFO_STRUCT_KEY.sem_wr_count_key = generate_key();
  if (-1 == SEMWP_SEATS_INFO_STRUCT_KEY.sem_wr_count_key) { FUNC_PERROR(); }
  SEMWP_SEATS_INFO_STRUCT_KEY.sem_aw_count_key = generate_key();
  if (-1 == SEMWP_SEATS_INFO_STRUCT_KEY.sem_aw_count_key) { FUNC_PERROR(); }
  SEMWP_SEATS_INFO_STRUCT_KEY.sem_ww_count_key = generate_key();
  if (-1 == SEMWP_SEATS_INFO_STRUCT_KEY.sem_ww_count_key) { FUNC_PERROR(); }
  SEMWP_SEATS_INFO_STRUCT_KEY.sem_ok_read_key = generate_key();
  if (-1 == SEMWP_SEATS_INFO_STRUCT_KEY.sem_ok_read_key) { FUNC_PERROR(); }
  SEMWP_SEATS_INFO_STRUCT_KEY.sem_ok_write_key = generate_key();
  if (-1 == SEMWP_SEATS_INFO_STRUCT_KEY.sem_ok_write_key) { FUNC_PERROR(); }
}

void shm_key_init(void)
{
  SHM_SEATS_INDEX_KEY = generate_key();
  if (-1 == SHM_SEATS_INDEX_KEY) { FUNC_PERROR(); }
  SHM_SEATS_INFO_KEY = generate_key();
  if (-1 == SHM_SEATS_INFO_KEY) { FUNC_PERROR(); }
  SHM_WORKERS_PID_KEY = generate_key();
  if (-1 == SHM_WORKERS_PID_KEY) { FUNC_PERROR(); }
  SHM_TICKET_DISPENSER_PID_KEY = generate_key();
  if (-1 == SHM_TICKET_DISPENSER_PID_KEY) { FUNC_PERROR(); }
  SHM_CALENDAR_KEY = generate_key();
  if (-1 == SHM_CALENDAR_KEY) { FUNC_PERROR(); }
}

void msg_key_init(void)
{
  MSG_NOTIFY_WORKER_KEYS = (key_t*)malloc(sizeof(key_t) * (size_t)NOF_WORKERS);
  if (NULL == MSG_NOTIFY_WORKER_KEYS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    MSG_NOTIFY_WORKER_KEYS[i] = generate_key();
    if (-1 == MSG_NOTIFY_WORKER_KEYS[i]) { FUNC_PERROR(); }
  }
  size_t num_user = (size_t)(NOF_USERS + N_NEW_USERS);
  MSG_NOTIFY_USER_KEYS = (key_t*)malloc(sizeof(key_t) * num_user);
  if (NULL == MSG_NOTIFY_USER_KEYS) { FUNC_PERROR(); }
  for (size_t i = 0; i < num_user; i++)
  {
    MSG_NOTIFY_USER_KEYS[i] = generate_key();
    if (-1 == MSG_NOTIFY_USER_KEYS[i]) { FUNC_PERROR(); }
  }
  MSG_NOTIFY_DISPENSER_KEY = generate_key();
  if (-1 == MSG_NOTIFY_DISPENSER_KEY) { FUNC_PERROR(); }
  MSG_NOTIFY_CLOCK_KEY = generate_key();
  if (-1 == MSG_NOTIFY_CLOCK_KEY) { FUNC_PERROR(); }
  MSG_STATS_DATA_KEY = generate_key();
  if (-1 == MSG_STATS_DATA_KEY) { FUNC_PERROR(); }
  MSG_STATS_METADATA_KEY = generate_key();
  if (-1 == MSG_STATS_METADATA_KEY) { FUNC_PERROR(); }
}

void ftok_key_init(void)
{
  sem_key_init();
  semrp_key_init();
  semwp_key_init();
  shm_key_init();
  msg_key_init();
}
