#ifndef FTOK_KEY_H
#define FTOK_KEY_H

extern key_t SEM_START_KEY;
extern key_t SEM_SEATS_KEY;
extern key_t SEM_SHM_SEATS_INFO_KEY;
extern key_t SEM_DAY_STARTED_KEY;

extern key_t SHM_SEATS_INDEX_KEY;
extern key_t SHM_SEATS_INFO_KEY;

// each “seats” has a message queue that connects “user” with “worker”
extern key_t* MSG_SEATS_QUEUE_KEY;

void ftok_key_init(void);

#endif
