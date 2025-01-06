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

ServiceDuration service_duration[SERV_NUM] = {{SEND_PICK_PARC, 15},  {SEND_LET_REG, 12},
                                              {WDRAWALS_DEPOSIT, 9}, {PAY_POST_BULL, 12},
                                              {PURCH_FIN_PROD, 30},  {PURCH_WATCH_BRAC, 30}};

//int find_best_time(int* calendar, int requested_time, int serv_duration, Service serv)
int find_best_time(int requested_time, Service* serv, int serv_num)
{
  int best_time = -1;
  int best_time_sum = INT_MAX;
  for (int i = -MINUTES_IN_HOUR; i <= MINUTES_IN_HOUR; i++)
  {
    int current_minute = requested_time + i;
    if (current_minute < 0 || current_minute >= MINUTES_IN_DAY) { continue; }
    else if (serv_duration + current_minute >= MINUTES_IN_DAY) { continue; }
    else
    {
      int sum = 0;
      for (int j = 0; j < serv_duration; j++) { sum += calendar[current_minute + j]; }
      if (sum < best_time_sum)
      {
        best_time_sum = sum;
        best_time = current_minute;
      }
    }
  }
  return best_time;
}
