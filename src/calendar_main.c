#include <stdio.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <limits.h>
#include "config.h"
#include "ftok_key.h"
#include "macros.h"
#include "sem.h"
#include "msg.h"
#include "shm.h"
#include "sem_utils.h"
#include "struct.h"
#include "sem_utils.h"
#define MINUTES_IN_DAY 1440
#define MINUTES_IN_HOUR 60

int calendar[MINUTES_IN_DAY] = {0};

ServiceDuration service_duration[SERV_NUM] = {
  {SEND_PICK_PARC, 15},
  {SEND_LET_REG, 12},
  {WDRAWALS_DEPOSIT, 9},
  {PAY_POST_BULL, 12},
  {PURCH_FIN_PROD, 30},
  {PURCH_WATCH_BRAC, 30}
};

void setup(void)
{
  config_load();
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
}

void start(void)
{
  if(-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); } //SEM FOR SAYING THAT PROC IS READY
  if(-1 == lock_sem(SEM_START_ID,0)) {FUNC_PERROR();} //SEM FOR STARTING
}

int find_best_time(int* calendar, int requested_time, int serv_duration, Service serv)
{
  int best_time = -1;
  int best_time_sum = INT_MAX;

  for(int i = -MINUTES_IN_HOUR; i <= MINUTES_IN_HOUR; i++)
  {
    int current_minute = requested_time + i;
    if(current_minute < 0 || current_minute >= MINUTES_IN_DAY)
    {
      continue;//salta i minuti fuori dalla giornata e eventualmente lo prenota per la giornata dopo (ancora da implementare)
    }
    else if(serv_duration + current_minute >= MINUTES_IN_DAY)
    {
      continue;//salta i minuti che non permettono di completare il servizio entro la fine della giornata
    } 
    else
    {
      int sum = 0;
      for(int j = 0; j < serv_duration; j++)
      {
        sum += calendar[current_minute + j];
      }
      if(sum < best_time_sum)
      {
        best_time_sum = sum;
        best_time = current_minute;
      }
    }

  } 
  return best_time;

}
void core(void)
{
  int request_minute = 720; //servizio richiesto a 12:00
  Service requested_service = rand() % SERV_NUM;//randomizza la richiesta del servizio
  printf("Servizio richiesto: %d\n", requested_service);
  int serv_duration = service_duration[requested_service].duration;//durata del servizio richiesto
  int best_time = find_best_time(calendar, request_minute, serv_duration, requested_service);//trova il miglior orario per il servizio richiesto
  if(-1 != best_time)
  printf("Il miglior orario per il servizio richiesto è alle %d\n", best_time);
  else
  printf("Non è stato possibile trovare un orario per il servizio richiesto\n");
}

int main(void)
{
  setup();
  while(1)
  {
    start();
    core();
  }
  return 0;
}
