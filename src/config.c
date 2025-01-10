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
int N_REQUESTS;
int NOF_PAUSE;

void config_load(void)
{
  NOF_WORKERS = 3;
  NOF_USERS = 2;
  NOF_WORKER_SEATS = 3;
  START_SEM_COUNT = NOF_WORKERS + NOF_USERS + 2;
  N_NANO_SECS = 5000000;
  P_SERV_MIN = 100;
  P_SERV_MAX = 100;
  N_REQUESTS = 2;
  NOF_PAUSE = 2;
}
