#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "ftok_key.h"
#include "config.h"
#include "macros.h"
#include "msg.h"

int* MSG_NOTIFY_WORKER_IDS = NULL;
int MSG_NOTIFY_DISPENSER_ID = -1;
int* MSG_NOTIFY_USER_IDS = NULL;
int MSG_NOTIFY_CLOCK_ID = -1;
int MSG_STATS_DATA_ID = -1;
int MSG_STATS_METADATA_ID = -1;

void MSG_NOTIFY_WORKER_IDS_init(void)
{
  MSG_NOTIFY_WORKER_IDS = (int*)malloc(sizeof(int) * (size_t)NOF_WORKERS);
  if (NULL == MSG_NOTIFY_WORKER_IDS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    MSG_NOTIFY_WORKER_IDS[i] = msgget(MSG_NOTIFY_WORKER_KEYS[i], 0666 | IPC_CREAT);
    if (-1 == MSG_NOTIFY_WORKER_IDS[i]) { FUNC_PERROR(); }
  }
}

void MSG_NOTIFY_WORKER_IDS_config(void)
{
  MSG_NOTIFY_WORKER_IDS = (int*)malloc(sizeof(int) * (size_t)NOF_WORKERS);
  if (NULL == MSG_NOTIFY_WORKER_IDS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    MSG_NOTIFY_WORKER_IDS[i] = msgget(MSG_NOTIFY_WORKER_KEYS[i], 0666);
    if (-1 == MSG_NOTIFY_WORKER_IDS[i]) { FUNC_PERROR(); }
  }
}

void MSG_NOTIFY_DISPENSER_ID_init(void)
{
  MSG_NOTIFY_DISPENSER_ID = msgget(MSG_NOTIFY_DISPENSER_KEY, 0666 | IPC_CREAT);
  if (-1 == MSG_NOTIFY_DISPENSER_ID) { FUNC_PERROR(); }
}

void MSG_NOTIFY_DISPENSER_ID_config(void)
{
  MSG_NOTIFY_DISPENSER_ID = msgget(MSG_NOTIFY_DISPENSER_KEY, 0666);
  if (-1 == MSG_NOTIFY_DISPENSER_ID) { FUNC_PERROR(); }
}

void MSG_NOTIFY_USER_IDS_init(void)
{
  size_t num_user = (size_t)(NOF_USERS + N_NEW_USERS);
  MSG_NOTIFY_USER_IDS = (int*)malloc(sizeof(int) * num_user);
  if (NULL == MSG_NOTIFY_USER_IDS) { FUNC_PERROR(); }
  for (size_t i = 0; i < num_user; i++)
  {
    MSG_NOTIFY_USER_IDS[i] = msgget(MSG_NOTIFY_USER_KEYS[i], 0666 | IPC_CREAT);
    if (-1 == MSG_NOTIFY_USER_IDS[i]) { FUNC_PERROR(); }
  }
}

void MSG_NOTIFY_USER_IDS_config(void)
{
  size_t num_user = (size_t)(NOF_USERS + N_NEW_USERS);
  MSG_NOTIFY_USER_IDS = (int*)malloc(sizeof(int) * num_user);
  if (NULL == MSG_NOTIFY_USER_IDS) { FUNC_PERROR(); }
  for (size_t i = 0; i < num_user; i++)
  {
    MSG_NOTIFY_USER_IDS[i] = msgget(MSG_NOTIFY_USER_KEYS[i], 0666);
    if (-1 == MSG_NOTIFY_USER_IDS[i]) { FUNC_PERROR(); }
  }
}

void MSG_NOTIFY_CLOCK_ID_init(void)
{
  MSG_NOTIFY_CLOCK_ID = msgget(MSG_NOTIFY_CLOCK_KEY, 0666 | IPC_CREAT);
  if (-1 == MSG_NOTIFY_CLOCK_ID) { FUNC_PERROR(); }
}

void MSG_NOTIFY_CLOCK_ID_config(void)
{
  MSG_NOTIFY_CLOCK_ID = msgget(MSG_NOTIFY_CLOCK_KEY, 0666);
  if (-1 == MSG_NOTIFY_CLOCK_ID) { FUNC_PERROR(); }
}

void MSG_STATS_DATA_ID_init(void)
{
  MSG_STATS_DATA_ID = msgget(MSG_STATS_DATA_KEY, 0666 | IPC_CREAT);
  if (-1 == MSG_STATS_DATA_ID) { FUNC_PERROR(); }
}

void MSG_STATS_DATA_ID_config(void)
{
  MSG_STATS_DATA_ID = msgget(MSG_STATS_DATA_KEY, 0666);
  if (-1 == MSG_STATS_DATA_ID) { FUNC_PERROR(); }
}

void MSG_STATS_METADATA_ID_init(void)
{
  MSG_STATS_METADATA_ID = msgget(MSG_STATS_METADATA_KEY, 0666 | IPC_CREAT);
  if (-1 == MSG_STATS_METADATA_ID) { FUNC_PERROR(); }
}

void MSG_STATS_METADATA_ID_config(void)
{
  MSG_STATS_METADATA_ID = msgget(MSG_STATS_METADATA_KEY, 0666);
  if (-1 == MSG_STATS_METADATA_ID) { FUNC_PERROR(); }
}

void msg_init(void)
{
  MSG_NOTIFY_WORKER_IDS_init();
  MSG_NOTIFY_DISPENSER_ID_init();
  MSG_NOTIFY_USER_IDS_init();
  MSG_NOTIFY_CLOCK_ID_init();
  MSG_STATS_DATA_ID_init();
  MSG_STATS_METADATA_ID_init();
}

void msg_config(void)
{
  MSG_NOTIFY_WORKER_IDS_config();
  MSG_NOTIFY_DISPENSER_ID_config();
  MSG_NOTIFY_USER_IDS_config();
  MSG_NOTIFY_CLOCK_ID_config();
  MSG_STATS_DATA_ID_config();
  MSG_STATS_METADATA_ID_config();
}

void msg_deallocate(void)
{
  for (int i = 0; i < NOF_WORKERS; i++)
  {
    if (-1 == msgctl(MSG_NOTIFY_WORKER_IDS[i], IPC_RMID, NULL)) { FUNC_PERROR(); }
  }
  free(MSG_NOTIFY_WORKER_IDS);
  if (-1 == msgctl(MSG_NOTIFY_DISPENSER_ID, IPC_RMID, NULL)) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_USERS; i++)
  {
    if (-1 == msgctl(MSG_NOTIFY_USER_IDS[i], IPC_RMID, NULL)) { FUNC_PERROR(); }
  }
  free(MSG_NOTIFY_USER_IDS);
}
