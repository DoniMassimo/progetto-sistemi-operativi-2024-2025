#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "ftok_key.h"
#include "config.h"
#include "macros.h"
#include "msg.h"

int* MSG_SEATS_QUEUE_IDS = NULL;
int MSG_TICKET_DISPENSER_ID = -1;

void MSG_SEATS_QUEUE_IDS_init(void)
{
  MSG_SEATS_QUEUE_IDS = (int*)malloc(sizeof(int) * (size_t)NOF_WORKER_SEATS);
  if (NULL == MSG_SEATS_QUEUE_IDS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKER_SEATS; i++)
  {
    MSG_SEATS_QUEUE_IDS[i] = msgget(MSG_SEATS_QUEUE_KEYS[i], 0666 | IPC_CREAT);
    if (-1 == MSG_SEATS_QUEUE_IDS[i]) { FUNC_PERROR(); }
  }
}

void MSG_SEATS_QUEUE_IDS_config(void)
{
  MSG_SEATS_QUEUE_IDS = (int*)malloc(sizeof(int) * (size_t)NOF_WORKER_SEATS);
  if (NULL == MSG_SEATS_QUEUE_IDS) { FUNC_PERROR(); }
  for (int i = 0; i < NOF_WORKER_SEATS; i++)
  {
    MSG_SEATS_QUEUE_IDS[i] = msgget(MSG_SEATS_QUEUE_KEYS[i], 0666);
    if (-1 == MSG_SEATS_QUEUE_IDS[i]) { FUNC_PERROR(); }
  }
}

void MSG_TICKET_DISPENSER_ID_init(void)
{
  MSG_TICKET_DISPENSER_ID = msgget(MSG_TICKET_DISPENSER_KEY, 0666 | IPC_CREAT);
  if (-1 == MSG_TICKET_DISPENSER_ID) { FUNC_PERROR(); }
}

void MSG_TICKET_DISPENSER_ID_config(void)
{
  MSG_TICKET_DISPENSER_ID = msgget(MSG_TICKET_DISPENSER_KEY, 0666);
  if (-1 == MSG_TICKET_DISPENSER_ID) { FUNC_PERROR(); }
}

void msg_init(void)
{
  MSG_SEATS_QUEUE_IDS_init();
  MSG_TICKET_DISPENSER_ID_init();
}

void msg_config(void)
{
  MSG_SEATS_QUEUE_IDS_config();
  MSG_TICKET_DISPENSER_ID_config();
}
