#ifndef TICKET_DISPENSER_H
#define TICKET_DISPENSER_H

#include "notification.h"

void dispenser_set_notf_param(GetNotfParam* get_notf_param, void** notifc);
void clear_dispenser_msg_queue(void);
void handle_ticket_req(TicketReq* ticket_req);

#endif
