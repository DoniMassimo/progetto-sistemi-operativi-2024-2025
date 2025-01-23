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
    calendar_stats[i] = (ServStats*)malloc(SERV_NUM * sizeof(ServStats));
    for (int j = 0; j < SERV_NUM; j++)
    {
      if (NULL == calendar_stats[i]) { FUNC_PERROR(); }
      calendar_stats[i][j].nof_served_user = 0;
      calendar_stats[i][j].nof_delivered_serv = 0;
      calendar_stats[i][j].nof_failedserv = 0;
      calendar_stats[i][j].nof_wait_time = 0;
      calendar_stats[i][j].wait_time = NULL;
      calendar_stats[i][j].nof_deliv_time = 0;
      calendar_stats[i][j].deliv_time = NULL;
      calendar_stats[i][j].nof_active_worker = 0;
      calendar_stats[i][j].nof_pause = 0;
      calendar_stats[i][j].worker_seat_frac = 0;
    }
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
      calendar_stats[curr_day][user_stats->serv].nof_delivered_serv += user_stats->completed_serv;
      calendar_stats[curr_day][user_stats->serv].nof_failedserv += user_stats->failed_serv;
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
              worker_stats->ser_data[(int)k + bound_deliv_time];
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

void calc_stats(void)
{
  total_served_users = 0;
  total_delivered_services = 0;
  total_failed_services = 0;
  total_wait_time = 0;
  avg_wait_time_general = 0;
  total_deliv_time = 0;
  total_active_workers = 0;
  total_pauses = 0;
  avg_total_pause = 0;
  avg_wait_time_per_day = 0;
  avg_deliv_time_per_day = 0;
  total_days = SIM_DURATION;
  total_services = total_delivered_services + total_failed_services;
  avg_worker_seat_frac = 0;

  for (int i = 0; i < SIM_DURATION; i++)
  {
    for (int j = 0; j < SERV_NUM; j++)
    {
      total_served_users += calendar_stats[i][j].nof_served_user;
      total_delivered_services += calendar_stats[i][j].nof_delivered_serv;
      total_failed_services += calendar_stats[i][j].nof_failedserv;
      total_wait_time += calendar_stats[i][j].nof_wait_time;
      total_deliv_time += calendar_stats[i][j].nof_deliv_time;
      total_active_workers += calendar_stats[i][j].nof_active_worker;
      total_pauses += calendar_stats[i][j].nof_pause;
      avg_worker_seat_frac += calendar_stats[i][j].worker_seat_frac;

      if (calendar_stats[i][j].nof_wait_time > 0)
      {
        for (int k = 0; k < calendar_stats[i][j].nof_wait_time; k++)
        {
          avg_wait_time_per_day += calendar_stats[i][j].wait_time[k];
        }
        avg_wait_time_per_day /= calendar_stats[i][j].nof_wait_time;
        avg_wait_time_general += avg_wait_time_per_day;
      }
      if (calendar_stats[i][j].nof_deliv_time > 0)
      {
        for (int k = 0; k < calendar_stats[i][j].nof_deliv_time; k++)
        {
          avg_deliv_time_per_day += calendar_stats[i][j].deliv_time[k];
        }
        avg_deliv_time_per_day /= calendar_stats[i][j].nof_deliv_time;
        avg_deliv_time_general += avg_deliv_time_per_day;
      }
    }
  }
  total_services = total_delivered_services + total_failed_services;
  avg_wait_time_general /= total_days;
  avg_deliv_time_general /= total_days;
  avg_total_pause = total_pauses / total_days;
}

void save_stats(void)
{
  FILE* stats_file = fopen("stats.csv", "w");
  if (NULL == stats_file) { FUNC_PERROR(); }

  fprintf(stats_file,
          "Day,Service,NofServedUser,NofDeliveredServ,NofFailedServ,NofWaitTime,NofDelivTime,"
          "NofActiveWorker,NofPause,WorkerSeatFrac\n");

  for (int i = 0; i < SIM_DURATION; i++)
  {
    for (int j = 0; j < SERV_NUM; j++)
    {
      fprintf(stats_file, "%d,%s,%d,%d,%d,%d,%d,%d,%d,%f\n", i, service_names[j],
              calendar_stats[i][j].nof_served_user, calendar_stats[i][j].nof_delivered_serv,
              calendar_stats[i][j].nof_failedserv, calendar_stats[i][j].nof_wait_time,
              calendar_stats[i][j].nof_deliv_time, calendar_stats[i][j].nof_active_worker,
              calendar_stats[i][j].nof_pause, calendar_stats[i][j].worker_seat_frac);
    }
  }

  fprintf(stats_file, "\n");
  fprintf(stats_file, "Total, ,%d,%d,%d,%d,%d,%d,%d,%d\n", total_served_users,
          total_delivered_services, total_failed_services, total_wait_time, total_deliv_time,
          total_active_workers, total_pauses, total_services);
  fprintf(stats_file, "Average per day, ,%f,%f,%f,%f,%f,%f,%f,%f\n",
          (float)total_served_users / (float)total_days,
          (float)total_delivered_services / (float)total_days,
          (float)total_failed_services / (float)total_days,
          (float)total_wait_time / (float)total_served_users,
          (float)total_deliv_time / (float)total_services,
          (float)total_active_workers / (float)total_days, (float)total_pauses / (float)total_days,
          (float)total_services / (float)total_days);

  fclose(stats_file);
}

void print_daily_stats(int curr_day)
{
  calc_stats();
  for (int j = 0; j < SERV_NUM; j++)
  {
    puts("\n");
    log_info("Day %d, Service %s: ", curr_day + 1, service_names[j]);
    log_info("Numero di utenti serviti totali nella simulazione: %d",
             calendar_stats[curr_day][j].nof_served_user);
    log_info("Numero di utenti serviti in media al giorno: %f",
             (float)calendar_stats[curr_day][j].nof_served_user / (float)(curr_day + 1));
    log_info("Numero di servizi erogati totali nella simulazione: %d",
             calendar_stats[curr_day][j].nof_delivered_serv);
    log_info("Numero di servizi non erogati totali nella simulazione: %d",
             calendar_stats[curr_day][j].nof_failedserv);
    log_info("Numero di servizi erogati in media al giorno: %f",
             (float)calendar_stats[curr_day][j].nof_delivered_serv / (float)(curr_day + 1));
    log_info("Numero di servizi non erogati in media al giorno: %f",
             (float)calendar_stats[curr_day][j].nof_failedserv / (float)(curr_day + 1));
    log_info("Tempo medio di attesa degli utenti nella simulazione: %f", avg_wait_time_per_day);
    log_info("Numero di operatori attivi durante la giornata: %d", calendar_stats[curr_day][j].nof_active_worker);
    log_info("Average Delivery Time: %f", avg_deliv_time_per_day);
    log_info("Number of Pauses: %d", calendar_stats[curr_day][j].nof_pause);
  }
}

void print_general_stats(int curr_day)
{
  calc_stats();
  puts("\n");
  log_info("Day %d, General stats:", curr_day + 1);
  log_info("Total Number of Served Users: %d", total_served_users);
  log_info("Average Served Users per day: %f", (float)total_served_users / (float)(curr_day + 1));
  log_info("Total Number of Delivered Services: %d", total_delivered_services);
  log_info("Total Number of Failed Services: %d", total_failed_services);
  log_info("Average Delivered Services per day: %f",
           (float)total_delivered_services / (float)(curr_day + 1));
  log_info("Average Failed Services per day: %f",
           (float)total_failed_services / (float)(curr_day + 1));
  log_info("Average Waiting Time: %f", avg_wait_time_general);
  log_info("Total number of active workers in the simulation: %d", total_active_workers);
  log_info("Total number of pauses in the simulation: %d", total_pauses);
  log_info("Average number of pauses in the simulation: %f", avg_total_pause);
  log_info("Average worker seat fraction: %f", (float)avg_worker_seat_frac / (float)total_days);
}
