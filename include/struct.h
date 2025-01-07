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
  CLOCK_RESP,
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
  int msg_id;
  int sem_count;
  size_t times_size;
  size_t serv_req_size;
  char data[];
} ClockCom;

#endif
