#include "config.h"

int NOF_WORKERS;
int NOF_USERS;
int NOF_WORKER_SEATS;
int START_SEM_COUNT;

void config_load()
{
  NOF_WORKERS = 1;
  NOF_USERS = 10;
  NOF_WORKER_SEATS = 1;
  START_SEM_COUNT = NOF_WORKERS + NOF_USERS;
}

