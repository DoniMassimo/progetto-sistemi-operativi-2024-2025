#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include "macros.h"
#include "sem_utils.h"
#include "struct.h"
#include "notification.h"

size_t get_notifc_size(MesType mes_type)
{
  if (DAY_ENDED == mes_type) { return sizeof(DayEnded) - sizeof(long); }
  if (TICKET_REQ == mes_type) { return sizeof(TicketReq) - sizeof(long); }
  if (TICKET_RESP == mes_type) { return sizeof(TicketResp) - sizeof(long); }
  if (CLOCK_REQ == mes_type)
  {
    size_t calc_size = sizeof(ClockReq);
    calc_size = calc_size + (sizeof(Service) + sizeof(int)) * (size_t)N_REQUESTS;
    calc_size = calc_size - sizeof(long);
    return calc_size;
  }
  if (CLOCK_NOTIFC == mes_type) { return sizeof(ClockNotifc) - sizeof(long); }
  if (SERVICE_REQ == mes_type) { return sizeof(ServiceReq) - sizeof(long); }
  if (SERVICE_RESP == mes_type) { return sizeof(ServiceResp) - sizeof(long); }
  MSG_ERROR("Not valid MesType");
}

MesType get_notifications(GetNotfParam* get_notf_param)
{
  MesType* notifc_filter = get_notf_param->notifc_filter;
  int nof_notifc = get_notf_param->nof_notifc;
  int msg_id = get_notf_param->msg_id;
  int sem_id = get_notf_param->sem_id;
  int sem_count = get_notf_param->sem_count;
  int can_skip = get_notf_param->can_skip;
  void** notifc_mes = get_notf_param->notifc_mes;
  size_t notific_size = get_notifc_size(notifc_filter[0]);
  *notifc_mes = (void*)malloc(notific_size);
  if (NULL == *notifc_mes) { FUNC_PERROR(); }
  if (-1 == lock_sem(sem_id, sem_count)) { FUNC_PERROR(); }
  for (int i = 0; i < nof_notifc; i++)
  {
    if (i != 0)
    {
      notific_size = get_notifc_size(notifc_filter[i]);
      *notifc_mes = (void*)realloc(*notifc_mes, notific_size);
      if (NULL == *notifc_mes) { FUNC_PERROR(); }
    }
    if (-1 == msgrcv(msg_id, *notifc_mes, notific_size, notifc_filter[i], IPC_NOWAIT))
    {
      if (ENOMSG != errno) { FUNC_PERROR(); }
    }
    else { return notifc_filter[i]; }
  }
  if (0 == can_skip) { MSG_ERROR("Notification expeceted"); }
  return NO_MES;
}

void push_notifc_node(NotifcLinked* notifc_queue, void* notifc, size_t notifc_size,
                      MesType notifc_type)
{
  NotifcNode* new_node = (NotifcNode*)malloc(sizeof(NotifcNode));
  if (NULL == new_node) { FUNC_PERROR(); }
  new_node->next = NULL;
  new_node->prev = NULL;
  new_node->notifc = (void*)malloc(notifc_size);
  if (NULL == new_node->notifc) { FUNC_PERROR(); }
  memcpy(new_node->notifc, notifc, notifc_size);
  new_node->notifc_type = notifc_type;
  if (0 == notifc_queue->nof_node)
  {
    notifc_queue->start = new_node;
    notifc_queue->end = new_node;
  }
  else
  {
    notifc_queue->start->prev = new_node;
    notifc_queue->start = new_node;
  }
  notifc_queue->nof_node++;
}

void* pop_start_notifc_node(NotifcLinked* notifc_queue, size_t notifc_size, MesType* notifc_type)
{
  if (0 == notifc_queue->nof_node) { MSG_ERROR("No element in queue"); }
  NotifcNode* start_node = notifc_queue->start;
  void* ret_notifc = notifc_queue->start->notifc;
  *notifc_type = start_node->notifc_type;
  if (notifc_queue->nof_node > 1)
  {
    notifc_queue->start = start_node->next;
    start_node->prev = NULL;
  }
  else if (1 == notifc_queue->nof_node)
  {
    notifc_queue->start = NULL;
    notifc_queue->end = NULL;
  }
  free(start_node);
  notifc_queue->nof_node--;
  return ret_notifc;
}

void* pop_end_notifc_node(NotifcLinked* notifc_queue, size_t notifc_size, MesType* notifc_type)
{
  if (0 == notifc_queue->nof_node) { MSG_ERROR("No element in queue"); }
  NotifcNode* end_node = notifc_queue->end;
  void* ret_notifc = end_node->notifc;
  *notifc_type = end_node->notifc_type;
  if (notifc_queue->nof_node > 1)
  {
    notifc_queue->end = end_node->prev;
    end_node->next = NULL;
  }
  else if (1 == notifc_queue->nof_node)
  {
    notifc_queue->start = NULL;
    notifc_queue->end = NULL;
  }
  free(end_node);
  notifc_queue->nof_node--;
  return ret_notifc;
}
