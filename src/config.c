#include <stdio.h>
#include "config.h"

int NOF_WORKERS;
int NOF_USERS;
int NOF_WORKER_SEATS;
int START_SEM_COUNT;
size_t N_NANO_SECS;

void config_load()
{
  NOF_WORKERS = 12;
  NOF_USERS = 10;
  NOF_WORKER_SEATS = 10;
  START_SEM_COUNT = NOF_WORKERS + NOF_USERS;
  N_NANO_SECS = 1000000;
}
