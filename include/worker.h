#ifndef WORKER_H
#define WORKER_H

extern Service assigned_service;
extern int seat_index;
extern int id;
extern int nof_pause_rem;
extern int sem_timer_id;

void set_pause_time(void);
void send_service_resp(ServiceReq* service_req);
void provide_service(ServiceReq* service_req);
int check_day_ended(void);
int prov_serv_paused_worker(SeatFreeCom* seat_free_com);
void comunicate_free_seat(void);
void set_notf_param(GetNotfParam* get_notf_param, void** notifc);
void take_pause(void);
int try_take_paused_seats(void* notifc);

#endif
