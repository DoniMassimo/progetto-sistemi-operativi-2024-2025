#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "ftok_key.h"
#include "config.h"
#include "macros.h"
#include "msg.h"

int* MSG_SEATS_QUEUE_ID = NULL;

void MSG_SEATS_QUEUE_ID_init(void)
{
  MSG_SEATS_QUEUE_ID = (int*)malloc(sizeof(int) * (size_t)NOF_WORKER_SEATS);
  if (NULL == NOF_WORKER_SEATS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKER_SEATS; i++)
  {
    MSG_SEATS_QUEUE_ID[i] = msgget(MSG_SEATS_QUEUE_KEY[i], 0666 | IPC_CREAT);
    if (-1 == MSG_SEATS_QUEUE_ID[i]) { FUNC_PERROR(); }
  }
}

void MSG_SEATS_QUEUE_ID_config(void)
{
  MSG_SEATS_QUEUE_ID = (int*)malloc(sizeof(int) * (size_t)NOF_WORKER_SEATS);
  if (NULL == NOF_WORKER_SEATS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKER_SEATS; i++)
  {
    MSG_SEATS_QUEUE_ID[i] = msgget(MSG_SEATS_QUEUE_KEY[i], 0666);
    if (-1 == MSG_SEATS_QUEUE_ID[i]) { FUNC_PERROR(); }
  }
}

void msg_init(void)
{
  MSG_SEATS_QUEUE_ID_init();
}

void msg_config(void)
{
  MSG_SEATS_QUEUE_ID_config();
}
