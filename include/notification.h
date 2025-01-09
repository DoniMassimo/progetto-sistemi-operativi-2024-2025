#ifndef NOTIDICATION_H
#define NOTIDICATION_H

#include "struct.h"

typedef struct 
{
  MesType* notifc_filter;
  int nof_notifc;
  int msg_id;
  int sem_id;
  int sem_count;
  void** notifc_mes;
} GetNotfParam;

size_t get_notifc_size(MesType mes_type);
MesType get_notifications(GetNotfParam* get_notf_param);

#endif
