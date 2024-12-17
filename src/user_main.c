#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sem_utils.h"
#include "macros.h"
#include "config.h"

int main(int argc, char* argv[])
{
  config_init();
  if (1 != argc) { MSG_ERROR("agrc error"); }
  key_t start_sem_key = ftok(".", 65);
  //printf("users inizia\n");
  fflush(stdout);
  int sem_op_res = lock_sem(START_SEM_ID, 0);
  if (-1 == sem_op_res) { FUNC_MSG_PERROR("sem empl"); }
  //printf("users fine\n");
  fflush(stdout);
  return 0;
}
