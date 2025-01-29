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

typedef struct
{
  int serv_user_count_tot;         // (1) il numero di utenti serviti totali nella simulazione
  float avg_serv_user_count;       // (2) il numero di utenti serviti in media al giorno
  int serv_deliv_count_tot;        // (3) il numero di servizi erogati totali nella simulazione
  int serv_fail_count_tot;         // (4) il numero di servizi non erogati totali nella simulazione
  float avg_serv_deliv_count;      // (5) il numero di servizi erogati in media al giorno
  float avg_serv_fail_count;       // (6) il numero di servizi non erogati in media al giorno
  float avg_user_wait;             // (7) il tempo medio di attesa degli utenti nella simulazione
  float avg_user_wait_today;       // (8) il tempo medio di attesa degli utenti nella giornata
  float avg_serv_deliv_time;       // (9) il tempo medio di erogazione dei servizi nella simulazione
  float avg_serv_deliv_time_today; // (10) il tempo medio di erogazione dei servizi nella giornata
  int active_worker_count_daily;   // (11) il numero di operatori attivi durante la giornata;
  int active_worker_count_tot;     // (12) il numero di operatori attivi durante la simulazione;
  float avg_pause_count;           // (13) il numero medio di pause effettuate nella giornata
  int pause_count_tot;             // (14) il totale di pause effettuate durante la simulazione
  float worker_seat_frac; // (15) il rapporto fra operatori disponibili e sportelli esistenti, per
                          // ogni sportello per ogni giornata.
} GeneralStats;

void init_stats(void);
int get_stats(int nof_msg, int curr_day);
void print_stats(int curr_day);
void print_save_stats(int curr_day, int* assigned_serv_seats, int* assigned_worker);

#endif
