#ifndef STRUCT_H
#define STRUCT_H

#include "config.h"

typedef enum
{
  DAY_ENDED = 1,
  SERVICE_REQ,
  TICKET_REQ,
  TICKET_RESP,
  CLOCK_REQ,
  CLOCK_NOTIFC,
  NO_MES,
} MesType;

typedef struct
{
  int msg_id;
  int sem_count;
  int info;
} Content;

typedef struct
{
  long mtype;
  Content content;
} ComStruct;

typedef struct
{
  long mtype;
} DayEnded;

typedef struct
{
  long mtype;
  int msg_id;
  int sem_count;
  size_t times_size;
  size_t serv_req_size;
  char data[];
} ClockReq;

typedef struct
{
  long mtype;
  Service serv;
} ClockNotifc;

typedef struct
{
  long mtype;
  int msg_id;
  int sem_count;
  Service serv;
} TicketReq;

typedef struct
{
  long mtype;
  int worker_msg_id;
  int worker_sem_count;
  Service serv;
  int status;
} TicketResp;

#endif
