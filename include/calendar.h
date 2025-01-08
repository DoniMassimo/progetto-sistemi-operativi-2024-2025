#ifndef CALENDAR_H
#define CALENDAR_H

#include "config.h"

#define MINUTES_IN_DAY 480

int get_serv_duration(Service* serv, int serv_num);
int find_best_time(int requested_time, Service* serv, int serv_num);
void clear_calendar(void);

#endif
