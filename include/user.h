#ifndef USER_H
#define USER_H

#include "config.h"

extern int id;
extern int P_SERV;

void send_notific_clock(int req_times[], Service* serv_req, int nof_req);
void send_ticket_request(Service serv);
void calc_times_from_serv(int all_req_times[], Service serv_req[], int req_time, int nof_req);
void setup_clock_notifc(void);

#endif
