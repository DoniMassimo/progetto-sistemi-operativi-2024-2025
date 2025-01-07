#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "seats.h"
#include "calendar.h"
#include "log.h"
#include "utils.h"
#include "macros.h"
#include "config.h"
#include "sem_utils.h"
#include "ftok_key.h"
#include "shm.h"
#include "sem.h"
#include "msg.h"
#include "struct.h"

int id;
int P_SERV;

void setup(char arg_1[])
{
  char* endptr;
  id = (int)strtol(arg_1, &endptr, 10);
  if (*endptr != '\0') { MSG_ERROR("Cant convert argv[1] to int."); }
  config_load();
  P_SERV = P_SERV_MIN + rand() % (P_SERV_MAX - P_SERV_MIN + 1);
  ftok_key_init();
  sem_config();
  shm_config();
  msg_config();
}

void send_notific_clock(int req_times[], Service* serv_req, int nof_req)
{
  log_trace("user -> nof_req: %d", nof_req);
  size_t notific_size = sizeof(ClockCom) + (sizeof(Service) + sizeof(int)) * (size_t)nof_req;
  ClockCom* notification = (ClockCom*)malloc(notific_size);
  if (NULL == notification) { FUNC_PERROR(); }
  size_t times_size = sizeof(int) * (size_t)nof_req;
  size_t serv_size = sizeof(Service) * (size_t)nof_req;
  for (int i = 0; i < nof_req; i++)
  {
    memcpy(notification->data, req_times, times_size);
    memcpy(notification->data + times_size, serv_req, serv_size);
  }
  notification->mtype = CLOCK_REQ;
  if (nof_req > 0)
  {
    notification->msg_id = MSG_NOTIFY_USER_IDS[id];
    notification->sem_count = id;
  }
  notification->times_size = times_size;
  notification->serv_req_size = serv_size;
  if (-1 == msgsnd(MSG_NOTIFY_CLOCK_ID, notification, notific_size - sizeof(long), 0)) { FUNC_PERROR(); }
}

void calc_times_from_serv(int all_req_times[], Service serv_req[], int req_time, int nof_req)
{
  for (int i = 0; i < nof_req; i++)
  {
    if (req_time >= MINUTES_IN_DAY) { req_time = MINUTES_IN_DAY - 1; }
    all_req_times[i] = req_time;
    req_time = req_time + get_serv_duration(&serv_req[i], 1);
  }
}

void setup_request(void)
{
  if ((rand() % 100) + 1 > P_SERV)
  {
    send_notific_clock(NULL, NULL, 0);
    return;
  }
  int nof_req = (rand() % N_REQUESTS) + 1;
  Service serv_req[nof_req];
  int all_req_times[nof_req];
  for (int i = 0; i < nof_req; i++) { serv_req[i] = (Service)(rand() % SERV_NUM); }
  int req_time = (int)(rand() % (8 * 60));
  int opt_time = find_best_time(req_time, serv_req, nof_req);
  calc_times_from_serv(all_req_times, serv_req, opt_time, nof_req);
  send_notific_clock(all_req_times, serv_req, nof_req);
}

void start(void)
{
  setup_request();
  if (-1 == release_sem(SEM_PROC_READY_ID, 0)) { FUNC_PERROR(); }
  int sem_res = lock_sem(SEM_START_ID, 0);
  if (-1 == sem_res) { FUNC_PERROR(); }
}

MesType get_notifications(ComStruct* com_struct)
{
  if (-1 == lock_sem(SEM_NOTIFY_USER_ID, id)) { FUNC_PERROR(); }
  if (-1 == msgrcv(MSG_NOTIFY_USER_IDS[id], com_struct, sizeof(Content), DAY_ENDED, IPC_NOWAIT))
  {
    if (ENOMSG != errno) { FUNC_PERROR(); }
  }
  else { return DAY_ENDED; }
  if (-1 == msgrcv(MSG_NOTIFY_USER_IDS[id], com_struct, sizeof(Content), TICKET_RESP, IPC_NOWAIT))
  {
    if (ENOMSG != errno) { FUNC_PERROR(); }
  }
  else { return TICKET_RESP; }
  MSG_ERROR("Expect to find message\n");
}

void core(void)
{
  ComStruct com_struct = {0};
  while (1)
  {
    MesType notification = get_notifications(&com_struct);
    if (DAY_ENDED == notification)
    {
      log_info("user %d ends day", id);
      return;
    }
    else if (TICKET_RESP == notification)
    {
      int ticket_outcome = com_struct.content.info;
      log_info("ticket request outcome: %d", ticket_outcome);
    }
  }
}

int main(int argc, char* argv[])
{
  srand((unsigned int)(time(NULL) + getpid()));
  if (2 != argc) { MSG_ERROR("agrc error"); }
  setup(argv[1]);
  while (1)
  {
    start();
    core();
  }
  return 0;
}
