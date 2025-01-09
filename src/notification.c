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
  if (DAY_ENDED == mes_type) { return sizeof(ComStruct) - sizeof(long); }
  if (TICKET_REQ == mes_type) { return sizeof(TicketReq) - sizeof(long); }
  if (TICKET_RESP == mes_type) { return sizeof(TicketResp) - sizeof(long); }
  if (CLOCK_REQ == mes_type)
  {
    return sizeof(ClockReq) + (sizeof(Service) + sizeof(int)) * (size_t)N_REQUESTS - sizeof(long);
  }
  if (CLOCK_NOTIFC == mes_type) { return sizeof(ClockNotifc) - sizeof(long); }
  MSG_ERROR("Not valid MesType");
}

MesType get_notifications(GetNotfParam* get_notf_param)
{
  MesType* notifc_filter = get_notf_param->notifc_filter;
  int nof_notifc = get_notf_param->nof_notifc;
  int msg_id = get_notf_param->msg_id;
  int sem_id = get_notf_param->sem_id;
  int sem_count = get_notf_param->sem_count;
  void **notifc_mes = get_notf_param->notifc_mes;
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
  MSG_ERROR("Notification expeceted");
  return NO_MES;
}
