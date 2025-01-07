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
#define MINUTES_IN_DAY 480

ServiceDuration service_duration[SERV_NUM] = {{SEND_PICK_PARC, 15},  {SEND_LET_REG, 12},
                                              {WDRAWALS_DEPOSIT, 9}, {PAY_POST_BULL, 12},
                                              {PURCH_FIN_PROD, 30},  {PURCH_WATCH_BRAC, 30}};

int get_serv_duration(Service* serv, int serv_num)
{
  int sum = 0;
  for (int i = 0; i < serv_num; i++) { sum += service_duration[serv[i]].duration; }
  return sum;
}

// int find_best_time(int* calendar, int requested_time, int serv_duration, Service serv)
int find_best_time(int requested_time, Service* serv, int serv_num)
{
  int best_time = -1;
  int best_time_sum = INT_MAX;

  lock_reader(SEMRW_CALENDAR_ID); //decremento il contatore dei lettori

  int* calendar = (int*)shmat(SHM_CALENDAR_ID, NULL, 0); //leggo la shm
  if ((void*)-1 == (void*)calendar) { FUNC_PERROR(); }

  int serv_duration = get_serv_duration(serv, serv_num); //calcolo la durata del servizio

  for (int i = -60; i <= 60; i++)
  {
    int current_minute = requested_time + i;
    if (current_minute < 0 || current_minute >= MINUTES_IN_DAY) { continue; }
    else if (get_serv_duration + current_minute >= MINUTES_IN_DAY) { continue; }
    else
    {
      int sum = 0;
      for (int j = 0; j < get_serv_duration; j++) { sum += serv[current_minute + j]; }
      if (sum < best_time_sum)
      {
        best_time_sum = sum;
        best_time = current_minute;
      }
    }
  }
  if (-1 == shmdt(calendar)) { FUNC_PERROR(); } //rilascio la shm
  release_reader(SEMRW_CALENDAR_ID); //incremento il contatore dei lettori

  if (-1 == lock_sem(SEMRW_CALENDAR_ID.sem_writer_id, 0)) { FUNC_PERROR(); }

  calendar = (int*)shmat(SHM_CALENDAR_ID, NULL, 0); //mi attacco alla shm
  if ((void*)-1 == (void*)calendar) { FUNC_PERROR(); }

  for (int i = 0; i < serv_duration; i++) 
  { 
    calendar[best_time + i]++; // segnalo la prenotazione
     
  }

  if(-1 == shmdt(calendar)) {FUNC_PERROR();} //rilascio la shm
  if(-1 == release_sem(SEMRW_CALENDAR_ID.sem_writer_id, 0)) {FUNC_PERROR();} //rilascio il semaforo
}
