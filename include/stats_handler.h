#ifndef STATS_HANDLER_H
#define STATS_HANDLER_H

int total_served_users;
int total_delivered_services;
int total_failed_services;
int total_wait_time;
int total_deliv_time;
int total_active_workers;
int total_pauses;
int total_services;
int total_days;
int avg_wait_time_per_day;
int avg_deliv_time_per_day;
float avg_worker_seat_frac;

typedef struct
{
  int nof_served_user;
  int nof_delivered_serv;
  int nof_failedserv;
  int nof_wait_time;
  int* wait_time;
  int nof_deliv_time;
  int* deliv_time;
  int nof_active_worker;
  int nof_pause;
  float worker_seat_frac;
} ServStats;

// prende le statistiche da una coda di messaggi
void get_stats(int nof_msg);
// salva stats in un csv
void save_stats(void);
void print_stats(void);

#endif
