#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include "config.h"
#include "macros.h"

int NOF_WORKERS;
int NOF_USERS;
int NOF_WORKER_SEATS;
int START_SEM_COUNT;
char REL_DIR[MAX_PATH_LEN];

key_t START_SEM_ID;
key_t SEM_SEATS_ID;

void config_load()
{
  NOF_WORKERS = 10;
  NOF_USERS = 10;
  NOF_WORKER_SEATS = 10;
  START_SEM_COUNT = NOF_WORKERS + NOF_USERS;
}

void config_ipcid()
{
  key_t sem_key = ftok(".", 65);
  START_SEM_ID = semget(sem_key, 1, 0666 | IPC_CREAT);
  if (START_SEM_ID < 0) { FUNC_PERROR(); }
  key_t sem_seats_key = ftok(".", 68);
  SEM_SEATS_ID = semget(sem_seats_key, SERV_NUM, 0666 | IPC_CREAT);
  if (SEM_SEATS_ID < 0) { FUNC_PERROR(); }
}

void config_init()
{
  config_load();
  config_ipcid();
}
