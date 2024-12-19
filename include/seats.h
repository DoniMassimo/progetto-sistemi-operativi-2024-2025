#ifndef SEATS_H
#define SEATS_H

#include "config.h"

void seats_init_resources();
int seats_try_take_seat(Service ser_type);

typedef struct
{
  int is_busy;
  int empoyee_mqueue_id;
  int seats_taken;
} SeatInfo;

#endif
