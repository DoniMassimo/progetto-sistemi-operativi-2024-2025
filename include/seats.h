#ifndef SEATS_H
#define SEATS_H

#include "config.h"

typedef struct
{
  int is_busy;
  int nof_user_wainting;
  int seats_taken;
  int com_mqueue_id;
  int notify_worker_sem_id;
} SeatInfo;

void seats_init_resources(int* assigned_serv_seats);
int seats_try_take_seat(Service ser_type, Signal* recived_signal, SeatInfo* seat_info);
void seats_release_seat(Service serv, int seat_index);
void get_bounds_serv(int* bounds, Service serv);

#endif
