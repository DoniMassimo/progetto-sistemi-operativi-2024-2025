#ifndef STATS_HANDLER_H
#define STATS_HANDLER_H

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
void get_stats(void);
// salva stats in un csv
void save_stats(void);
void print_stats(void);

#endif
