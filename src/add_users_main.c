#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ftok_key.h"
#include "sem_utils.h"
#include "sem.h"
#include "log.h"

int main(void)
{
  ftok_key_init();
  SEM_ADD_USERS_config();
  log_trace("AGGIUNTI UTENTI");
  release_sem_val(SEM_ADD_USERS_ID, 0, 1);
}
