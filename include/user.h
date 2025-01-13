#ifndef USER_H
#define USER_H

#include "config.h"
#include "notification.h"

extern int id;
extern int P_SERV;
extern int rem_serv_req[SERV_NUM];

void setup_user_stats(void);
void user_clear_msg_queue(void);
void send_user_stats(void);
void add_completed_serv(Service serv);
void set_failed_serv(Service serv,int fail_serv_num);
void add_waiting_time(Service serv, int time);
void send_notific_clock(int req_times[], Service* serv_req, int nof_req);
void send_ticket_request(Service serv);
void send_serv_request(TicketResp* ticket_resp);
void calc_times_from_serv(int all_req_times[], Service serv_req[], int req_time, int nof_req);
void setup_clock_notifc(void);
void user_set_notf_param(GetNotfParam* get_notf_param, void** notifc);

#endif
