#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "sig_utils.h"
#include "macros.h"

void block_user_signal(SigMasks* sig_mask)
{
  sigemptyset(&sig_mask->set);
  sigaddset(&sig_mask->set, SIGUSR1);
  if (-1 == sigprocmask(SIG_BLOCK, &sig_mask->set, &sig_mask->oldset)) { FUNC_PERROR(); }
}

void unlock_user_signal(SigMasks* sig_mask)
{
  if (-1 == sigprocmask(SIG_SETMASK, &sig_mask->oldset, NULL)) { FUNC_PERROR(); }
}
