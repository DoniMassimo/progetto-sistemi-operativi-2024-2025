#ifndef NOTIDICATION_H
#define NOTIDICATION_H

#include "struct.h"

size_t get_notifc_size(MesType mes_type);
MesType get_notifications(MesType notifc_filter[], int nof_notifc, int msg_id, int sem_id, int sem_count,
              void** notifc_mes);

#endif
