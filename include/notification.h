#ifndef NOTIDICATION_H
#define NOTIDICATION_H

#include "struct.h"

typedef struct NotifcNode NotifcNode;
struct NotifcNode
{
  void* notifc;
  MesType notifc_type;
  struct NotifcNode* next;
  struct NotifcNode* prev;
};

typedef struct
{
  NotifcNode* start;
  NotifcNode* end;
  int nof_node;
} NotifcLinked;

typedef struct
{
  MesType* notifc_filter;
  int nof_notifc;
  int msg_id;
  int sem_id;
  int sem_count;
  int can_skip;
  void** notifc_mes;
} GetNotfParam;

size_t get_notifc_size(MesType mes_type);
MesType get_notifications(GetNotfParam* get_notf_param);
void push_notifc_node(NotifcLinked* notifc_queue, void* notifc, size_t notifc_size,
                      MesType notifc_type);

void* pop_start_notifc_node(NotifcLinked* notifc_queue, MesType* notifc_type);
void* pop_end_notifc_node(NotifcLinked* notifc_queue, MesType* notifc_type);

#endif
