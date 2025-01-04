#ifndef STRUCT_H
#define STRUCT_H

typedef enum
{
  DAY_ENDED = 1,
  SERVICE_REQ,
  TICKET_REQ,
} MesType;

typedef struct
{
  int msg_response_id;
  int type;
} Content;

typedef struct
{
  long mtype;
  Content content;
} ComStruct;

#endif
