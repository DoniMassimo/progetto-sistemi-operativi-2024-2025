#ifndef STATS_HANDLER_H
#define STATS_HANDLER_H

extern int total_served_users;
extern int total_delivered_services;
extern int total_failed_services;
extern int total_wait_time;
extern int total_deliv_time;
extern int total_active_workers;
extern int total_pauses;
extern int total_services;
extern int total_days;

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

void init_stats(void);
// prende le statistiche da una coda di messaggi
void get_stats(int nof_msg, int curr_day);
// salva stats in un csv
void save_stats(void);
void print_stats(int curr_day);

#endif
