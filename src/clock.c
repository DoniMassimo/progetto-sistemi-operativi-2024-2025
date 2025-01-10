#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include "macros.h"
#include "notification.h"
#include "struct.h"
#include "log.h"
#include "msg.h"
#include "sem.h"
#include "shm.h"
#include "config.h"
#include "calendar.h"
#include "clock.h"

UserNotific* user_notific = NULL;
size_t user_notf_size = 0;
size_t user_notf_index = 0;

void add_user_notific(ClockReq* clock_com, size_t* curr_nof_user_notfc)
{
  size_t data_count = clock_com->times_size / sizeof(int);
  int* ptr_times = (int*)clock_com->data;
  Service* ptr_serv_req = (Service*)(clock_com->data + clock_com->times_size);
  for (size_t i = 0; i < data_count; i++)
  {
    if (*curr_nof_user_notfc >= user_notf_size)
    {
      user_notf_size = user_notf_size * 2;
      user_notific = realloc(user_notific, sizeof(UserNotific) * user_notf_size);
      if (NULL == user_notific) { FUNC_PERROR(); }
    }
    user_notific[*curr_nof_user_notfc].time = ptr_times[i];
    user_notific[*curr_nof_user_notfc].serv = ptr_serv_req[i];
    user_notific[*curr_nof_user_notfc].sem_count = clock_com->sem_count;
    user_notific[*curr_nof_user_notfc].msg_id = clock_com->msg_id;
    (*curr_nof_user_notfc)++;
  }
}

int compare(const void* a, const void* b)
{
  UserNotific* req1 = (UserNotific*)a;
  UserNotific* req2 = (UserNotific*)b;
  return req1->time - req2->time;
}

void setup_user_notific(void)
{
  if (NULL != user_notific || 0 != user_notf_size)
  {
    free(user_notific);
    user_notific = NULL;
    user_notf_size = 0;
    user_notf_index = 0;
  }
  user_notific = (UserNotific*)malloc(sizeof(UserNotific) * 2);
  if (NULL == user_notific) { FUNC_PERROR(); }
  user_notf_size = 2;
  size_t max_content_size = sizeof(ClockReq) + (sizeof(Service) + sizeof(int)) * (size_t)N_REQUESTS;
  ClockReq* clock_com = (ClockReq*)malloc(max_content_size);
  if (NULL == clock_com) { FUNC_PERROR(); }
  size_t curr_nof_user_notfc = 0;
  int user_set = 0;
  while (user_set < NOF_USERS)
  {
    if (-1 == msgrcv(MSG_NOTIFY_CLOCK_ID, clock_com, max_content_size - sizeof(long), CLOCK_REQ, 0))
    {
      FUNC_PERROR();
    }
    add_user_notific(clock_com, &curr_nof_user_notfc);
    user_set++;
  }
  struct msqid_ds msq_stat;
  if (-1 == msgctl(MSG_NOTIFY_CLOCK_ID, IPC_STAT, &msq_stat)) { FUNC_PERROR(); }
  if (msq_stat.msg_qnum != 0) { MSG_ERROR("zero messages are expected") }
  user_notf_size = curr_nof_user_notfc;
  qsort(user_notific, (size_t)user_notf_size, sizeof(UserNotific), compare);
}

void send_msg_day_ended(void)
{
  DayEnded com_struct;
  size_t msg_size = get_notifc_size(DAY_ENDED);
  com_struct.mtype = DAY_ENDED;
  for (int i = 0; i < NOF_USERS; i++)
  {
    if (-1 == msgsnd(MSG_NOTIFY_USER_IDS[i], &com_struct, sizeof(msg_size), 0)) { FUNC_PERROR(); }
  }
  if (-1 == release_all_sem(SEM_NOTIFY_USER_ID, NOF_USERS)) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    if (-1 == msgsnd(MSG_NOTIFY_WORKER_IDS[i], &com_struct, sizeof(msg_size), 0)) { FUNC_PERROR(); }
  }
  if (NOF_WORKERS > 0)
  {
    if (-1 == release_all_sem(SEM_NOTIFY_WORKER_ID, NOF_WORKERS)) { FUNC_PERROR(); }
  }
  if (-1 == msgsnd(MSG_NOTIFY_DISPENSER_ID, &com_struct, sizeof(msg_size), 0)) { FUNC_PERROR(); }
  if (-1 == release_sem(SEM_NOTIFY_DISPENSER_ID, 0)) { FUNC_PERROR(); }
}

void send_user_notific(int curr_min)
{
  while (user_notific[user_notf_index].time <= curr_min)
  {
    if (user_notf_index >= user_notf_size) { break; }
    int time = user_notific[user_notf_index].time;
    Service serv = user_notific[user_notf_index].serv;
    int msg_id = user_notific[user_notf_index].msg_id;
    int sem_count = user_notific[user_notf_index].sem_count;
    ClockNotifc clock_notifc = {0};
    clock_notifc.mtype = CLOCK_NOTIFC;
    clock_notifc.serv = (int)serv;
    log_trace("clock clock_notifc -> user: %d time: %d serv: %d msg_id: %d", sem_count, time, serv,
              msg_id);
    msgsnd(msg_id, &clock_notifc, get_notifc_size(CLOCK_NOTIFC), 0);
    release_sem(SEM_NOTIFY_USER_ID, sem_count);
    user_notf_index++;
  }
}
