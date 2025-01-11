#include <stdlib.h>
#include "config.h"
#include "stats_handler.h"

ServStats** calendar_stats = NULL;
int curr_day = 0;

void get_stats(int nof_msg)
{
  for (int i = 0; i < nof_msg; i++)
  {
    // msgrcv
    // msgrcv
  }
  // error se ci sono ancora dati
}

void save_stats(void) {}
