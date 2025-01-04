#include <stdio.h>
#include "config.h"
#include "config.h"

char REL_DIR[MAX_PATH_LEN];

int NOF_WORKERS;
int NOF_USERS;
int NOF_WORKER_SEATS;
int START_SEM_COUNT;
size_t N_NANO_SECS;
int P_SERV_MIN;
int P_SERV_MAX;

void config_load(void)
{
  NOF_WORKERS = 1;
  NOF_USERS = 2;
  NOF_WORKER_SEATS = 1;
  START_SEM_COUNT = NOF_WORKERS + NOF_USERS + 2;
  N_NANO_SECS = 10000;
  P_SERV_MIN = 90;
  P_SERV_MAX = 100;
}

