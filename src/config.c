#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "macros.h"

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
int SIM_DURATION;
int N_NEW_USERS;

void config_load(void)
{
  FILE* config_file = fopen("config.txt", "r");
  if (NULL == config_file) { FUNC_PERROR(); }
  char line[256];
  while (fgets(line, sizeof(line), config_file))
  {
    char* key = strtok(line, "=");
    char* value = strtok(NULL, "=");
    if (0 == strcmp(key, "NOF_WORKERS")) { NOF_WORKERS = atoi(value); }
    else if (0 == strcmp(key, "NOF_USERS")) { NOF_USERS = atoi(value); }
    else if (0 == strcmp(key, "NOF_WORKER_SEATS")) { NOF_WORKER_SEATS = atoi(value); }
    else if (0 == strcmp(key, "N_NANO_SECS")) { N_NANO_SECS = (size_t)atoi(value); }
    else if (0 == strcmp(key, "P_SERV_MIN")) { P_SERV_MIN = atoi(value); }
    else if (0 == strcmp(key, "P_SERV_MAX")) { P_SERV_MAX = atoi(value); }
    else if (0 == strcmp(key, "N_REQUESTS")) { N_REQUESTS = atoi(value); }
    else if (0 == strcmp(key, "NOF_PAUSE")) { NOF_PAUSE = atoi(value); }
    else if (0 == strcmp(key, "SIM_DURATION")) { SIM_DURATION = atoi(value); }
    else if (0 == strcmp(key, "N_NEW_USERS")) { N_NEW_USERS = atoi(value); }
    else { MSG_ERROR("Error in config file"); }
  }
  START_SEM_COUNT = NOF_WORKERS + NOF_USERS + 2;
  fclose(config_file);
}

void add_new_users(void)
{
  NOF_USERS = NOF_USERS + N_NEW_USERS;
  START_SEM_COUNT = START_SEM_COUNT + N_NEW_USERS;
}
