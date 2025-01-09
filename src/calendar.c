#include <stdio.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <limits.h>
#include "calendar.h"
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "sem.h"
#include "msg.h"
#include "shm.h"
#include "sem_utils.h"
#include "struct.h"
#include "sem_utils.h"
#define MAX_LATE_REQUESTS INT_MAX

ServiceDuration service_duration[SERV_NUM] = {{SEND_PICK_PARC, 15},  {SEND_LET_REG, 12},
                                              {WDRAWALS_DEPOSIT, 9}, {PAY_POST_BULL, 12},
                                              {PURCH_FIN_PROD, 30},  {PURCH_WATCH_BRAC, 30}};

int get_serv_duration(Service* serv, int serv_num)
{
  int sum = 0;
  for (int i = 0; i < serv_num; i++) { sum += service_duration[serv[i]].duration; }
  return sum;
}

void handle_late_request(int requested_time, int serv_duration, int* calendar, Service* serv,
                         int serv_num)
{
  int last_minute = MINUTES_IN_DAY - 1;
  calendar[last_minute]++;

  static LateRequest late_requests[MAX_LATE_REQUESTS];
  static int late_requests_count = 0;

  if (late_requests_count < NOF_USERS)
  {
    late_requests[late_requests_count].requested_time = requested_time;
    late_requests[late_requests_count].duration = serv_duration;
    late_requests[late_requests_count].serv_num = serv_num;
    for (int i = 0; i < serv_num; i++) { late_requests[late_requests_count].serv[i] = serv[i]; }
    late_requests_count++;
  }
  else { log_error("Too many late requests."); }
  
}

int find_best_time(int requested_time, Service* serv, int serv_num)
{
  int best_time = -1;
  int best_time_sum = INT_MAX;
  int closest_time = INT_MAX;
  lock_reader(SEMRW_CALENDAR_ID);
  int* calendar = (int*)shmat(SHM_CALENDAR_ID, NULL, 0);
  if ((void*)-1 == (void*)calendar) { FUNC_PERROR(); }
  int serv_duration = get_serv_duration(serv, serv_num);
  for (int i = -60; i <= 60; i++)
  {
    int current_minute = requested_time + i;
    if (current_minute < 0 || current_minute >= MINUTES_IN_DAY) { handle_late_request(requested_time, serv_duration, calendar, serv, serv_num); }
    else if (serv_duration + current_minute >= MINUTES_IN_DAY) { handle_late_request(requested_time, serv_duration, calendar, serv, serv_num); }
    else
    {
      int sum = 0;
      for (int j = 0; j < serv_duration; j++) { sum += (int)calendar[current_minute + j]; }
      int time_diff = abs(requested_time - current_minute);
      if (sum < best_time_sum || (sum == best_time_sum && time_diff < closest_time))
      {
        best_time_sum = sum;
        best_time = current_minute;
        closest_time = time_diff;
      }
    }
  }
  if (-1 == shmdt(calendar)) { FUNC_PERROR(); }
  release_reader(SEMRW_CALENDAR_ID);
  if (-1 == lock_sem(SEMRW_CALENDAR_ID.sem_writer_id, 0)) { FUNC_PERROR(); }
  calendar = (int*)shmat(SHM_CALENDAR_ID, NULL, 0);
  if ((void*)-1 == (void*)calendar) { FUNC_PERROR(); }
  for (int i = 0; i < serv_duration; i++) { calendar[best_time + i]++; }
  if (-1 == shmdt(calendar)) { FUNC_PERROR(); }
  if (-1 == release_sem(SEMRW_CALENDAR_ID.sem_writer_id, 0)) { FUNC_PERROR(); }
  return best_time;
}

void clear_calendar(void)
{
  int* calendar = (int*)shmat(SHM_CALENDAR_ID, NULL, 0);
  if ((void*)-1 == (void*)calendar) { FUNC_PERROR(); }
  if (-1 == lock_sem(SEMRW_CALENDAR_ID.sem_writer_id, 0)) { FUNC_PERROR(); }
  memset(calendar, 0, MINUTES_IN_DAY * sizeof(int));
  if (-1 == release_sem(SEMRW_CALENDAR_ID.sem_writer_id, 0)) { FUNC_PERROR(); }
  if (-1 == shmdt(calendar)) { FUNC_PERROR(); }
}
