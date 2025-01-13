#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include "macros.h"
#include "struct.h"
#include "msg.h"
#include "sem.h"
#include "shm.h"
#include "ticket_dispenser.h"
#include "config.h"
#include "calendar.h"
#include "notification.h"
#include "seats.h"

void handle_ticket_req(TicketReq* ticket_req)
{
  Service asked_serv = (Service)ticket_req->serv;
  SeatInfo seat_info = {0};
  int service_available = seats_get_less_worker(asked_serv, &seat_info);
  TicketResp ticket_resp = {0};
  ticket_resp.mtype = TICKET_RESP;
  ticket_resp.worker_sem_count = seat_info.sem_notify_worker_count;
  ticket_resp.worker_msg_id = seat_info.msg_notify_worker_id;
  ticket_resp.serv = asked_serv;
  if (1 == service_available) { ticket_resp.status = 1; }
  else { ticket_resp.status = 0; }
  if (-1 == msgsnd(ticket_req->user_msg_id, &ticket_resp, get_notifc_size(TICKET_RESP), 0))
  {
    FUNC_PERROR();
  }
  if (-1 == release_sem(SEM_NOTIFY_USER_ID, ticket_req->user_sem_count)) { FUNC_PERROR(); }
}

void dispenser_set_notf_param(GetNotfParam* get_notf_param, void** notifc)
{
  MesType notifc_filter[] = {DAY_ENDED, TICKET_REQ};
  get_notf_param->nof_notifc = 2;
  size_t notifc_filter_size = sizeof(MesType) * 2;
  get_notf_param->notifc_filter = (MesType*)malloc(notifc_filter_size);
  if (NULL == get_notf_param->notifc_filter) { FUNC_PERROR(); }
  memcpy(get_notf_param->notifc_filter, notifc_filter, notifc_filter_size);
  get_notf_param->msg_id = MSG_NOTIFY_DISPENSER_ID;
  get_notf_param->sem_id = SEM_NOTIFY_DISPENSER_ID;
  get_notf_param->sem_count = 0;
  get_notf_param->can_skip = 0;
  get_notf_param->nowait = 0;
  get_notf_param->notifc_mes = notifc;
}

void clear_dispenser_msg_queue(void)
{
  void* notifc = NULL;
  GetNotfParam get_notf_param = {0};
  dispenser_set_notf_param(&get_notf_param, &notifc);
  get_notf_param.nowait = 1;
  get_notf_param.can_skip = 1;
  int count_rem = 0;
  MesType notification = get_notifications(&get_notf_param);
  while (notification != NO_MES)
  {
    count_rem++;
    notification = get_notifications(&get_notf_param);
  }
  struct msqid_ds buf;
  if (msgctl(MSG_NOTIFY_DISPENSER_ID, IPC_STAT, &buf) == -1) { FUNC_PERROR(); }
  if (buf.msg_qnum > 0)
  {
    MSG_ERROR("ticket disp -> Unexpected msg");
  }
  log_trace("ticket -> clear_msg: %d", count_rem);
  if (-1 == init_sem_zero(SEM_NOTIFY_DISPENSER_ID, 0)) { FUNC_PERROR(); }
}
