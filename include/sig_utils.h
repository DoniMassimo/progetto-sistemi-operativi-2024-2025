#ifndef SIG_UTILS_H
#define SIG_UTILS_H

typedef struct
{
  sigset_t set;
  sigset_t oldset;
} SigMasks;

void block_user_signal(SigMasks*);
void unlock_user_signal(SigMasks*);

#endif
