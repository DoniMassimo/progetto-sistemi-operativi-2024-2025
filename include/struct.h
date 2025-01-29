#ifndef STRUCT_H
#define STRUCT_H

#include "config.h"

typedef enum
{
  DAY_ENDED = 1,
  SERVICE_REQ,
  SERVICE_RESP,
  TICKET_REQ,
  TICKET_RESP,
  CLOCK_REQ,
  CLOCK_NOTIFC,
  SEAT_FREE,
  PAUSE_NOTIFC,
  CLOCK_REQ_PAUSE,
  TIMER_REQ,
  NO_MES,
} MesType;

typedef struct
{
  long mtype;
  int data;
} DayEnded;

typedef struct
{
  long mtype;
  int worker_msg_id;
} SeatFreeCom;

typedef struct
{
  long mtype;
  int user_msg_id;
  int user_sem_id;
  int user_sem_count;
  Service serv;
} ServiceReq;

typedef struct
{
  long mtype;
  int data;
} PauseNotifc;

typedef struct
{
  long mtype;
  int time;
  int worker_msg_id;
  int worker_sem_count;
} ClockReqPause;

typedef struct
{
  long mtype;
  int data;
  int time;
} ServiceResp;

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
  int user_msg_id;
  int user_sem_count;
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

typedef struct
{
  long mtype;
  int time;
  int sem_id;
  int sem_count;
  int info;
} TimerReq;

typedef struct
{
  long mtype;
  Service serv;
  int user_id;
  int completed_serv;
  int failed_serv;
  size_t nof_waiting_times;
  int ser_data[];
} UserStats;

typedef struct
{
  long mtype;
  Service serv;
  int active;
  int pause;
  size_t nof_delivery_times;
  int ser_data[];
} WorkerStats;

typedef struct
{
  long mtype;
  int type; //0 = UserStats
  size_t ser_data_size;
} StatsSize;

typedef struct
{
  long mtype;
  char data[];
} TestStruct;

#endif
