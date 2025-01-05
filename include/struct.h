#ifndef STRUCT_H
#define STRUCT_H

typedef enum
{
  DAY_ENDED = 1,
  SERVICE_REQ,
  TICKET_REQ,
  TICKET_RESP,
} MesType;

typedef struct
{
  int msg_id;
  int sem_count;
  int info;
} TicketCont;

typedef union
{
  TicketCont ticket_cont;
} Content;

typedef struct
{
  long mtype;
  Content content;
} ComStruct;

#endif
