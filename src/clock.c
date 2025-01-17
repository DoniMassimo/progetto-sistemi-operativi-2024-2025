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

ClockReqPause* worker_pause = NULL;
size_t worker_pause_size = 0;
size_t worker_pause_index = 0;

TimerReq* timer_data = NULL;
size_t timer_data_size = 0;
size_t timer_send_index = 0;
size_t timer_recv_index = 0;

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

int compare_timer_req(const void* a, const void* b)
{
  TimerReq* req1 = (TimerReq*)a;
  TimerReq* req2 = (TimerReq*)b;
  return req1->time - req2->time;
}

int compare_user_notfc(const void* a, const void* b)
{
  UserNotific* req1 = (UserNotific*)a;
  UserNotific* req2 = (UserNotific*)b;
  return req1->time - req2->time;
}

int compare_worker_pause(const void* a, const void* b)
{
  ClockReqPause* req1 = (ClockReqPause*)a;
  ClockReqPause* req2 = (ClockReqPause*)b;
  return req1->time - req2->time;
}

void setup_worker_pause(void)
{
  if (NULL != worker_pause || 0 != worker_pause_size)
  {
    free(worker_pause);
    worker_pause = NULL;
    worker_pause_size = 0;
    worker_pause_index = 0;
  }
  worker_pause = (ClockReqPause*)malloc(sizeof(ClockReqPause) * 2);
  if (NULL == worker_pause) { FUNC_PERROR(); }
  worker_pause_size = 2;
  int worker_set = 0;
  ClockReqPause clock_req_pause = {0};
  size_t crp_size = get_notifc_size(CLOCK_REQ_PAUSE);
  size_t curr_nof_req_pause = 0;
  while (worker_set < NOF_WORKERS)
  {
    if (curr_nof_req_pause >= worker_pause_size)
    {
      worker_pause_size = worker_pause_size * 2;
      worker_pause =
          (ClockReqPause*)realloc(worker_pause, sizeof(ClockReqPause) * worker_pause_size);
      if (NULL == worker_pause) { FUNC_PERROR(); }
    }
    if (-1 == msgrcv(MSG_NOTIFY_CLOCK_ID, &clock_req_pause, crp_size, CLOCK_REQ_PAUSE, 0))
    {
      FUNC_PERROR();
    }
    if (clock_req_pause.time != -1)
    {
      memcpy(worker_pause + curr_nof_req_pause, &clock_req_pause, crp_size + sizeof(long));
      curr_nof_req_pause++;
    }
    worker_set++;
  }
  worker_pause_size = curr_nof_req_pause;
  qsort(worker_pause, worker_pause_size, sizeof(ClockReqPause), compare_worker_pause);
}

void get_new_timer(int curr_time)
{
  while (1)
  {
    if (timer_recv_index >= timer_data_size)
    {
      if (0 == timer_data_size) { timer_data_size = 2; }
      if (timer_data_size > 0) { timer_data_size *= 2; }
      timer_data = (TimerReq*)realloc(timer_data, sizeof(TimerReq) * timer_data_size);
      if (NULL == timer_data) { FUNC_PERROR(); }
    }
    TimerReq timer_req = {0};
    size_t msg_size = get_notifc_size(TIMER_REQ);
    if (-1 == msgrcv(MSG_NOTIFY_CLOCK_ID, &timer_req, msg_size, TIMER_REQ, IPC_NOWAIT))
    {
      if (errno == ENOMSG) { break; }
      else { FUNC_PERROR(); }
    }
    timer_req.time += curr_time;
    timer_data[timer_recv_index] = timer_req;
    timer_recv_index++;
    log_trace("clock R timer_req -> time: %d sem_count: %d", timer_req.time, timer_req.info);
  }
  qsort(timer_data + timer_send_index, (size_t)(timer_recv_index - timer_send_index),
        sizeof(TimerReq), compare_timer_req);
}

void send_timer_notifc(int curr_min)
{
  if (timer_send_index >= timer_recv_index) { return; }
  while (timer_data[timer_send_index].time <= curr_min)
  {
    if (timer_send_index >= timer_recv_index) { break; }
    int sem_count = timer_data[timer_send_index].sem_count;
    int sem_id = timer_data[timer_send_index].sem_id;
    log_trace("clock S timer_notifc -> time: %d sem_count: %d", curr_min, sem_count);
    release_sem(sem_id, sem_count);
    timer_send_index++;
  }
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
  user_notf_size = curr_nof_user_notfc;
  qsort(user_notific, (size_t)user_notf_size, sizeof(UserNotific), compare_user_notfc);
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
  if (user_notf_index >= user_notf_size) { return; }
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
    log_trace("clock S clock_notifc -> user: %d time: %d serv: %d", sem_count, time, serv);
    msgsnd(msg_id, &clock_notifc, get_notifc_size(CLOCK_NOTIFC), 0);
    release_sem(SEM_NOTIFY_USER_ID, sem_count);
    user_notf_index++;
  }
}

void send_worker_pause(int curr_min)
{
  if (worker_pause_index >= worker_pause_size) { return; }
  while (worker_pause[worker_pause_index].time <= curr_min)
  {
    if (worker_pause_index >= worker_pause_size) { break; }
    int time = worker_pause[worker_pause_index].time;
    int msg_id = worker_pause[worker_pause_index].worker_msg_id;
    int sem_count = worker_pause[worker_pause_index].worker_sem_count;
    PauseNotifc pause_notifc = {0};
    pause_notifc.mtype = PAUSE_NOTIFC;
    log_trace("clock S pause_notifc -> worker: %d time: %d", sem_count, time);
    msgsnd(msg_id, &pause_notifc, get_notifc_size(PAUSE_NOTIFC), 0);
    release_sem(SEM_NOTIFY_WORKER_ID, sem_count);
    worker_pause_index++;
  }
}
