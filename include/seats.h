#ifndef SEATS_H
#define SEATS_H

#include "config.h"

typedef struct
{
  int is_busy;
  int seats_taken;
  int msg_notify_worker_id;
  int sem_notify_worker_count;
} SeatInfo;

void seats_init_resources(int* assigned_serv_seats);
int seats_try_take_seat(Service ser_type, int worker_id);
void seats_release_seat(Service serv, int seat_index);
void get_bounds_serv(int* bounds, Service serv);

#endif
