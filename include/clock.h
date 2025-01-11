#ifndef CLOCK_H
#define CLOCK_H

#include "struct.h"

typedef struct
{
  int time;
  Service serv;
  int msg_id;
  int sem_count;
} UserNotific;

extern UserNotific* user_notific;
extern size_t user_notf_size;
extern size_t user_notf_index;

extern ClockReqPause* worker_pause;
extern size_t worker_pause_size;
extern size_t worker_pause_index;

void add_user_notific(ClockReq* clock_com, size_t* curr_nof_user_notfc);
int compare(const void* a, const void* b);
void setup_user_notific(void);
void setup_worker_pause(void);
void send_msg_day_ended(void);
void send_user_notific(int curr_min);
void send_worker_pause(int curr_min);

#endif
