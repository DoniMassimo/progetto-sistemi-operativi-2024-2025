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

ServiceDuration service_duration_max[SERV_NUM] = {{SEND_PICK_PARC, 15},  {SEND_LET_REG, 12},
                                                  {WDRAWALS_DEPOSIT, 9}, {PAY_POST_BULL, 12},
                                                  {PURCH_FIN_PROD, 30},  {PURCH_WATCH_BRAC, 30}};

ServiceDuration service_duration[SERV_NUM] = {{SEND_PICK_PARC, 10},  {SEND_LET_REG, 8},
                                              {WDRAWALS_DEPOSIT, 6}, {PAY_POST_BULL, 8},
                                              {PURCH_FIN_PROD, 20},  {PURCH_WATCH_BRAC, 20}};

int get_serv_duration(Service* serv, int serv_num)
{
  int sum = 0;
  for (int i = 0; i < serv_num; i++) { sum += service_duration[serv[i]].duration; }
  return sum;
}

int get_serv_duration_max(Service* serv, int serv_num)
{
  int sum = 0;
  for (int i = 0; i < serv_num; i++) { sum += service_duration_max[serv[i]].duration; }
  return sum;
}

int find_best_time(int requested_time, Service* serv, int serv_num)
{
  int best_time = -1;
  int best_time_sum = INT_MAX;
  int closest_time = INT_MAX;
  int emergency_time = -1; //il tempo di emergenza è il tempo più vicino al tempo richiesto
  int emergency_time_sum = INT_MAX; //tiene traccia del minor numero di servizi in un intervallo di 2 ore
  int emergency_time_diff = INT_MAX; //tiene traccia della differenza tra il tempo richiesto e il tempo di emergenza
  lock_reader(SEMRW_CALENDAR_ID);
  int* calendar = (int*)shmat(SHM_CALENDAR_ID, NULL, 0);
  if ((void*)-1 == (void*)calendar) { FUNC_PERROR(); }
  int serv_duration = get_serv_duration(serv, serv_num);
  for (int i = -60; i <= 60; i++)
  {
    int current_minute = requested_time + i;
    if (current_minute >= MINUTES_IN_DAY || serv_duration + current_minute >= MINUTES_IN_DAY) 
    {
      int time_diff = abs(requested_time - current_minute);
      if(time_diff < emergency_time_diff)
      {
        emergency_time_diff = time_diff;
        emergency_time = current_minute;
      }
      continue; 
    }
    else if(current_minute < 0)
    {
      continue;
    }
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
      if (sum < emergency_time_sum || (sum == emergency_time_sum && time_diff < emergency_time_diff))
      {
        emergency_time_sum = sum;
        emergency_time = current_minute;
        emergency_time_diff = time_diff;
      }
    }
  }

    // Se non è stato trovato un tempo ideale, usa il emrgency_time
  if (best_time == -1) 
  {
    best_time = emergency_time;
  }

  // Assicurati che il best_time sia all'interno dei limiti della giornata lavorativa
  if (best_time < 0) 
  {
    best_time = 0;
  } else if (best_time >= MINUTES_IN_DAY) 
  {
    best_time = MINUTES_IN_DAY - serv_duration;
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
