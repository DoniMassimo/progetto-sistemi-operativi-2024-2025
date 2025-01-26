#include <stdlib.h>
#include <sys/msg.h>
#include "config.h"
#include "log.h"
#include "stats_handler.h"
#include "msg.h"
#include "struct.h"
#include "macros.h"

int total_served_users;
int total_delivered_services;
int total_failed_services;
int total_wait_time;
int avg_wait_time_general;
int total_deliv_time;
int avg_deliv_time_general;
int total_active_workers;
int total_pauses;
int avg_total_pause;
int total_services;
int total_days;
int avg_wait_time_per_day;
int avg_deliv_time_per_day;
float avg_worker_seat_frac;

const char* service_names[SERV_NUM] = {"Invio e ritiro pacchi",
                                       "Invio e lettere e raccomandate",
                                       "Prelievi e versamenti Bancoposta",
                                       "Pagamento bollettini postali",
                                       "Acquisto prodotti finanziari",
                                       "Acquisto orologi e braccialetti"};

ServStats** calendar_stats = NULL;

void init_stats(void)
{
  calendar_stats = (ServStats**)malloc((size_t)SIM_DURATION * sizeof(ServStats*));
  if (NULL == calendar_stats) { FUNC_PERROR(); }
  for (int i = 0; i < SIM_DURATION; i++)
  {
    calendar_stats[i] = (ServStats*)calloc(SERV_NUM, sizeof(ServStats));
  }
}

int get_stats(int nof_msg, int curr_day)
{
  int bound_deliv_time = 0;
  int count_failed_serv = 0;
  for (int i = 0; i < nof_msg; i++)
  {
    StatsSize stats_size;
    if (-1 == msgrcv(MSG_STATS_METADATA_ID, &stats_size, sizeof(StatsSize), 0, 0))
    {
      FUNC_PERROR();
    }
    long filter_mtype = stats_size.mtype;
    int msg_type = stats_size.type;
    if (msg_type == 0) // UserStats
    {
      UserStats* user_stats = (UserStats*)malloc(sizeof(UserStats) + stats_size.ser_data_size);
      if (NULL == user_stats) { FUNC_PERROR(); }
      if (-1 == msgrcv(MSG_STATS_DATA_ID, user_stats,
                       sizeof(UserStats) + stats_size.ser_data_size - sizeof(long), filter_mtype,
                       0))
      {
        FUNC_PERROR();
        free(user_stats);
      }
      if (user_stats->completed_serv > 0)
      {
        calendar_stats[curr_day][user_stats->serv].nof_served_user++;
      }
      calendar_stats[curr_day][user_stats->serv].nof_delivered_serv = user_stats->completed_serv;
      calendar_stats[curr_day][user_stats->serv].nof_failedserv = user_stats->failed_serv;
      count_failed_serv += user_stats->failed_serv;
      int new_nof_wait_time = calendar_stats[curr_day][user_stats->serv].nof_wait_time +
                              (int)user_stats->nof_waiting_times;
      if (new_nof_wait_time > 0)
      {
        calendar_stats[curr_day][user_stats->serv].wait_time =
            (int*)realloc(calendar_stats[curr_day][user_stats->serv].wait_time,
                          (size_t)new_nof_wait_time * sizeof(int));
        if (NULL == calendar_stats[curr_day][user_stats->serv].wait_time) { FUNC_PERROR(); }
        for (size_t k = 0; k < user_stats->nof_waiting_times; k++)
        {
          calendar_stats[curr_day][user_stats->serv]
              .wait_time[calendar_stats[curr_day][user_stats->serv].nof_wait_time + (int)k] =
              user_stats->ser_data[k];
        }
        calendar_stats[curr_day][user_stats->serv].nof_wait_time = new_nof_wait_time;
      }
      bound_deliv_time = (int)user_stats->nof_waiting_times;
      free(user_stats);
    }
    else if (msg_type == 1) // WorkerStats
    {
      WorkerStats* worker_stats =
          (WorkerStats*)malloc(sizeof(WorkerStats) + stats_size.ser_data_size);
      if (NULL == worker_stats) { FUNC_PERROR(); }
      if (-1 == msgrcv(MSG_STATS_DATA_ID, worker_stats,
                       sizeof(WorkerStats) - sizeof(long) + stats_size.ser_data_size, filter_mtype,
                       0))
      {
        FUNC_PERROR();
        free(worker_stats);
      }
      calendar_stats[curr_day][worker_stats->serv].nof_active_worker += worker_stats->active;
      calendar_stats[curr_day][worker_stats->serv].nof_pause += worker_stats->pause;
      int new_nof_deliv_time = calendar_stats[curr_day][worker_stats->serv].nof_deliv_time +
                               (int)worker_stats->nof_delivery_times;
      if (new_nof_deliv_time > 0)
      {
        calendar_stats[curr_day][worker_stats->serv].deliv_time =
            (int*)realloc(calendar_stats[curr_day][worker_stats->serv].deliv_time,
                          (size_t)new_nof_deliv_time * sizeof(int));
        if (NULL == calendar_stats[curr_day][worker_stats->serv].deliv_time) { FUNC_PERROR(); }
        for (size_t k = 0; k < worker_stats->nof_delivery_times; k++)
        {
          calendar_stats[curr_day][worker_stats->serv]
              .deliv_time[calendar_stats[curr_day][worker_stats->serv].nof_deliv_time + (int)k] =
              worker_stats->ser_data[(int)k];
        }
        calendar_stats[curr_day][worker_stats->serv].nof_deliv_time = new_nof_deliv_time;
      }
      avg_worker_seat_frac = calendar_stats[curr_day][worker_stats->serv].worker_seat_frac =
          (float)calendar_stats[curr_day][worker_stats->serv].nof_active_worker / SERV_NUM;
      free(worker_stats);
    }
  }
  return count_failed_serv;
}

// void save_stats(void)
// {
//   FILE* stats_file = fopen("stats.csv", "w");
//   if (NULL == stats_file) { FUNC_PERROR(); }
//
//   fprintf(stats_file, "Day,Service,Field,Value\n");
//
//   for (int i = 0; i < SIM_DURATION; i++)
//   {
//     for (int j = 0; j < SERV_NUM; j++)
//     {
//       fprintf(stats_file, "%d,%s,1,%d\n", i + 1, service_names[j],
//               calendar_stats[i][j].nof_served_user);
//       fprintf(stats_file, "%d,%s,2,%f\n", i + 1, service_names[j],
//               (float)calendar_stats[i][j].nof_served_user / (float)(i + 1));
//       fprintf(stats_file, "%d,%s,3,%d\n", i + 1, service_names[j],
//               calendar_stats[i][j].nof_delivered_serv);
//       fprintf(stats_file, "%d,%s,4,%d\n", i + 1, service_names[j],
//               calendar_stats[i][j].nof_failedserv);
//       fprintf(stats_file, "%d,%s,5,%f\n", i + 1, service_names[j],
//               (float)calendar_stats[i][j].nof_delivered_serv / (float)(i + 1));
//       fprintf(stats_file, "%d,%s,6,%f\n", i + 1, service_names[j],
//               (float)calendar_stats[i][j].nof_failedserv / (float)(i + 1));
//       fprintf(stats_file, "%d,%s,8,%f\n", i + 1, service_names[j], avg_wait_time_per_day);
//       fprintf(stats_file, "%d,%s,10,%f\n", i + 1, service_names[j], avg_deliv_time_per_day);
//       fprintf(stats_file, "%d,%s,11,%d\n", i + 1, service_names[j],
//               calendar_stats[i][j].nof_active_worker);
//       fprintf(stats_file, "%d,%s,13,%d\n", i + 1, service_names[j],
//       calendar_stats[i][j].nof_pause); fprintf(stats_file, "%d,%s,14,%f\n", i + 1,
//       service_names[j],
//               calendar_stats[i][j].worker_seat_frac);
//     }
//   }
//
//   // Statistiche generali
//   fprintf(stats_file, "Total,generale,1,%d\n", total_served_users);
//   fprintf(stats_file, "Total,generale,2,%f\n", (float)total_served_users / (float)total_days);
//   fprintf(stats_file, "Total,generale,3,%d\n", total_delivered_services);
//   fprintf(stats_file, "Total,generale,4,%d\n", total_failed_services);
//   fprintf(stats_file, "Total,generale,5,%f\n", (float)total_delivered_services /
//   (float)total_days); fprintf(stats_file, "Total,generale,6,%f\n", (float)total_failed_services /
//   (float)total_days); fprintf(stats_file, "Total,generale,7,%f\n", avg_wait_time_general);
//   fprintf(stats_file, "Total,generale,9,%f\n", avg_deliv_time_general);
//   fprintf(stats_file, "Total,generale,12,%d\n", total_active_workers);
//   fprintf(stats_file, "Total,generale,13,%d\n", total_pauses);
//
//   fclose(stats_file);
// }

void calc_serv_stats(GeneralStats* general_stats, Service serv, int curr_day,
                     int* nof_avg_user_wait, int* nof_avg_serv_deliv_time)
{
  for (int i = 0; i < curr_day; i++)
  {
    general_stats->serv_user_count_tot += calendar_stats[i][serv].nof_served_user;
    general_stats->serv_deliv_count_tot += calendar_stats[i][serv].nof_delivered_serv;
    general_stats->serv_fail_count_tot += calendar_stats[i][serv].nof_failedserv;
    general_stats->active_worker_count_tot += calendar_stats[i][serv].nof_active_worker;
    general_stats->pause_count_tot += calendar_stats[i][serv].nof_pause;
    if (i == (curr_day - 1))
    {
      general_stats->active_worker_count_daily = calendar_stats[i][serv].nof_active_worker;
    }
    for (int j = 0; j < calendar_stats[i][serv].nof_wait_time; j++)
    {
      *nof_avg_user_wait += calendar_stats[i][serv].nof_wait_time;
      general_stats->avg_user_wait += (float)calendar_stats[i][serv].wait_time[j];
      if (i == (curr_day - 1))
      {
        general_stats->avg_user_wait_today += (float)calendar_stats[i][serv].wait_time[j];
      }
    }
    for (int j = 0; j < calendar_stats[i][serv].nof_deliv_time; j++)
    {
      *nof_avg_serv_deliv_time = calendar_stats[i][serv].nof_deliv_time;
      general_stats->avg_serv_deliv_time += (float)calendar_stats[i][serv].deliv_time[j];
      if (i == (curr_day - 1))
      {
        general_stats->avg_serv_deliv_time_today += (float)calendar_stats[i][serv].deliv_time[j];
      }
    }
  }
}

GeneralStats* calc_stats(Service* serv, int nof_serv, int curr_day)
{
  GeneralStats* general_stats = (GeneralStats*)calloc(1, sizeof(GeneralStats));
  if (NULL == general_stats) { FUNC_PERROR(); }
  int nof_avg_user_wait = 0;
  int nof_avg_serv_deliv_time = 0;
  int cum_nof_wait_time = 0;
  int cum_nof_deliv_time = 0;
  for (int i = 0; i < nof_serv; i++)
  {
    int temp_nof_avg_uw = 0;
    int temp_nof_avg_sdt = 0;
    calc_serv_stats(general_stats, serv[i], curr_day, &temp_nof_avg_uw, &temp_nof_avg_sdt);
    nof_avg_user_wait += temp_nof_avg_uw;
    nof_avg_serv_deliv_time += temp_nof_avg_sdt;
    cum_nof_wait_time += calendar_stats[curr_day][serv[i]].nof_wait_time;
    cum_nof_deliv_time += calendar_stats[curr_day][serv[i]].nof_deliv_time;
  }
  general_stats->avg_serv_user_count =
      (float)general_stats->serv_user_count_tot / (float)(curr_day + 1);
  general_stats->avg_serv_deliv_count =
      (float)general_stats->serv_deliv_count_tot / (float)(curr_day + 1);
  general_stats->avg_serv_fail_count =
      (float)general_stats->serv_fail_count_tot / (float)(curr_day + 1);
  if (0 != nof_avg_user_wait) { general_stats->avg_user_wait /= (float)nof_avg_user_wait; }
  if (0 != cum_nof_wait_time) { general_stats->avg_user_wait_today /= (float)cum_nof_wait_time; }
  if (0 != nof_avg_serv_deliv_time)
  {
    general_stats->avg_serv_deliv_time /= (float)nof_avg_serv_deliv_time;
  }
  if (0 != cum_nof_deliv_time)
  {
    general_stats->avg_serv_deliv_time_today /= (float)cum_nof_deliv_time;
  }
  general_stats->avg_pause_count = (float)general_stats->pause_count_tot / (float)(curr_day + 1);
  return general_stats;
}

void print_struct_stats(GeneralStats* stats)
{
  log_info("Numero di utenti serviti totali nella simulazione: %d", stats->serv_user_count_tot);
  log_info("Numero di utenti serviti in media al giorno: %f", stats->avg_serv_user_count);
  log_info("Numero di servizi erogati totali nella simulazione: %d", stats->serv_deliv_count_tot);
  log_info("Numero di servizi non erogati totali nella simulazione: %d",
           stats->serv_fail_count_tot);
  log_info("Numero di servizi erogati in media al giorno: %f", stats->avg_serv_deliv_count);
  log_info("Numero di servizi non erogati in media al giorno: %f", stats->avg_serv_fail_count);
  log_info("Tempo medio di attesa degli utenti nella simulazione: %f", stats->avg_user_wait);
  log_info("Tempo medio di attesa degli utenti nella giornata: %f", stats->avg_user_wait_today);
  log_info("Tempo medio di erogazione dei servizi nella simulazione: %f",
           stats->avg_serv_deliv_time);
  log_info("Tempo medio di erogazione dei servizi nella giornata: %f",
           stats->avg_serv_deliv_time_today);
}

void print_stats(int curr_day)
{
  for (int i = 0; i < SERV_NUM; i++)
  {
    GeneralStats* stats = calc_stats((Service*)&i, 1, curr_day);
    log_info("Giorno %d, Servizio %s: ", curr_day + 1, service_names[i]);
    print_struct_stats(stats);
    printf("\n\n");
    fflush(stdout);
    free(stats);
  }
  Service servs[] = {
      SEND_PICK_PARC, SEND_LET_REG,   WDRAWALS_DEPOSIT,
      PAY_POST_BULL,  PURCH_FIN_PROD, PURCH_WATCH_BRAC,
  };
  GeneralStats* gen_stats = calc_stats(servs, SERV_NUM, curr_day);
  log_info("Giorno %d, Statistiche generali: ", curr_day + 1);
  print_struct_stats(gen_stats);
  log_info("Numero di operatori attivi durante la giornata: %d",
           gen_stats->active_worker_count_daily);
  log_info("Numero di operatori attivi durante la simulazione: %d",
           gen_stats->active_worker_count_tot);
  log_info("Numero medio di pause effettuate nella giornata: %f", gen_stats->avg_pause_count);
  log_info("Totale di pause effettuate durante la simulazione: %d", gen_stats->pause_count_tot);
  printf("\n\n");
  fflush(stdout);
  free(gen_stats);
}
