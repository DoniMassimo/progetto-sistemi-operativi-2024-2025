#include <stdlib.h>
#include <sys/msg.h>
#include "config.h"
#include "log.h"
#include "stats_handler.h"
#include "msg.h"
#include "struct.h"
#include "macros.h"


//ServStats** calendar_stats = NULL;
//int curr_day = 0;
//
//void init_stats(void)
//{
//  calendar_stats = (ServStats**)malloc(SIM_DURATION * sizeof(ServStats*));
//  if (NULL == calendar_stats) { FUNC_PERROR(); }
//  for (int i = 0; i < SIM_DURATION; i++)
//  {
//    calendar_stats[i] = (ServStats*)malloc(SERV_NUM * sizeof(ServStats));
//    for (int j = 0; j < SERV_NUM; j++)
//    {
//      if (NULL == calendar_stats[i]) { FUNC_PERROR(); }
//      calendar_stats[i][j].nof_served_user = 0;
//      calendar_stats[i][j].nof_delivered_serv = 0;
//      calendar_stats[i][j].nof_failedserv = 0;
//      calendar_stats[i][j].nof_wait_time = 0;
//      calendar_stats[i][j].wait_time = NULL;
//      calendar_stats[i][j].nof_deliv_time = 0;
//      calendar_stats[i][j].deliv_time = NULL;
//      calendar_stats[i][j].nof_active_worker = 0;
//      calendar_stats[i][j].nof_pause = 0;
//      calendar_stats[i][j].worker_seat_frac = 0;
//    }
//  }
//}
//
//void get_stats(int nof_msg)
//{
//  for (int i = 0; i < nof_msg; i++)
//  {
//    StatsSize stats_size;
//    UserStats user_stats;
//    if (-1 == msgrcv(MSG_STATS_METADATA_ID, &stats_size, sizeof(StatsSize), 0, 0))
//    {
//      FUNC_PERROR();
//    }
//
//    log_info("Received stats size");
//    long filter_mtype = stats_size.mtype;
//
//    if (filter_mtype == 0)
//    {
//      UserStats* user_stats = (UserStats*)malloc(sizeof(UserStats) + stats_size.ser_data_size);
//      if (NULL == user_stats) { FUNC_PERROR(); }
//      if (-1 == msgrcv(MSG_STATS_DATA_ID, user_stats,
//                       sizeof(UserStats) + stats_size.ser_data_size - sizeof(long), filter_mtype,
//                       0))
//      {
//        FUNC_PERROR();
//        free(user_stats);
//      }
//      log_trace("Received user stats");
//
//      calendar_stats[curr_day][user_stats->serv].nof_served_user++;
//      calendar_stats[curr_day][user_stats->serv].nof_delivered_serv += user_stats->completed_serv;
//      calendar_stats[curr_day][user_stats->serv].nof_failedserv += user_stats->failed_serv;
//      calendar_stats[curr_day][user_stats->serv].nof_wait_time += user_stats->nof_waiting_times;
//      calendar_stats[curr_day][user_stats->serv].nof_deliv_time += user_stats->nof_delivery_times;
//
//      calendar_stats[curr_day][user_stats->serv].wait_time =
//          (int*)realloc(calendar_stats[curr_day][user_stats->serv].wait_time,
//                        calendar_stats[curr_day][user_stats->serv].nof_wait_time * sizeof(int));
//      calendar_stats[curr_day][user_stats->serv].deliv_time =
//          (int*)realloc(calendar_stats[curr_day][user_stats->serv].deliv_time,
//                        calendar_stats[curr_day][user_stats->serv].nof_deliv_time * sizeof(int));
//      if (NULL == calendar_stats[curr_day][user_stats->serv].wait_time ||
//          NULL == calendar_stats[curr_day][user_stats->serv].deliv_time)
//      {
//        FUNC_PERROR();
//      }
//
//      for (int k = 0; k < user_stats->nof_waiting_times; k++)
//      {
//        calendar_stats[curr_day][user_stats->serv].wait_time[k] = user_stats->ser_data[k];
//      }
//      for (int k = 0; k < user_stats->nof_delivery_times; k++)
//      {
//        calendar_stats[curr_day][user_stats->serv].deliv_time[k] =
//            user_stats->ser_data[k + user_stats->nof_waiting_times];
//      }
//
//      free(user_stats);
//    }
//    else if (filter_mtype == 1)
//    {
//      WorkerStats* worker_stats = (WorkerStats*)malloc(sizeof(WorkerStats) - sizeof(long));
//      if (NULL == worker_stats) { FUNC_PERROR(); }
//      if (-1 == msgrcv(MSG_STATS_DATA_ID, worker_stats, sizeof(WorkerStats) - sizeof(long),
//                       filter_mtype, 0))
//      {
//        FUNC_PERROR();
//      }
//      log_info("Received worker stats");
//
//      calendar_stats[curr_day][worker_stats->serv].nof_active_worker += worker_stats->active;
//      calendar_stats[curr_day][worker_stats->serv].nof_pause += worker_stats->pause;
//
//      free(worker_stats);
//    }
//  }
//}
//
//void save_stats(void)
//{
//  FILE* stats_file = fopen("stats.csv", "w");
//  if (NULL == stats_file) { FUNC_PERROR(); }
//  fprintf(stats_file,
//          "Day,Service,NofServedUser,NofDeliveredServ,NofFailedServ,NofWaitTime,NofDelivTime,"
//          "NofActiveWorker,NofPause,WorkerSeatFrac\n");
//  for (int i = 0; i < SIM_DURATION; i++)
//  {
//    for (int j = 0; j < SERV_NUM; j++)
//    {
//      fprintf(stats_file, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%f\n", i, j,
//              calendar_stats[i][j].nof_served_user, calendar_stats[i][j].nof_delivered_serv,
//              calendar_stats[i][j].nof_failedserv, calendar_stats[i][j].nof_wait_time,
//              calendar_stats[i][j].nof_deliv_time, calendar_stats[i][j].nof_active_worker,
//              calendar_stats[i][j].nof_pause, calendar_stats[i][j].worker_seat_frac);
//    }
//  }
//  fclose(stats_file);
//}
//
//void print_stats(void)
//{
//  for (int i = 0; i < SIM_DURATION; i++)
//  {
//    for (int j = 0; j < SERV_NUM; j++)
//    {
//      log_info("Day %d, Service %d:", i, j);
//      log_info("  Number of Served Users: %d", calendar_stats[i][j].nof_served_user);
//      log_info("  Number of Delivered Services: %d", calendar_stats[i][j].nof_delivered_serv);
//      log_info("  Number of Failed Services: %d", calendar_stats[i][j].nof_failedserv);
//      log_info("  Number of Waiting Times: %d", calendar_stats[i][j].nof_wait_time);
//      log_info("  Number of Delivery Times: %d", calendar_stats[i][j].nof_deliv_time);
//      log_info("  Number of Active Workers: %d", calendar_stats[i][j].nof_active_worker);
//      log_info("  Number of Pauses: %d", calendar_stats[i][j].nof_pause);
//      log_info("  Worker Seat Fraction: %f", calendar_stats[i][j].worker_seat_frac);
//    }
//  }
//}
