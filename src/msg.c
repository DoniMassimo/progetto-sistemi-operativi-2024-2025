#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "ftok_key.h"
#include "config.h"
#include "macros.h"
#include "msg.h"

int* MSG_NOTIFY_WORKER_IDS = NULL;
int MSG_NOTIFY_DISPENSER_ID = -1;

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

void msg_init(void)
{
  MSG_NOTIFY_WORKER_IDS_init();
  MSG_NOTIFY_DISPENSER_ID_init();
}

void msg_config(void)
{
  MSG_NOTIFY_WORKER_IDS_config();
  MSG_NOTIFY_DISPENSER_ID_config();
}
