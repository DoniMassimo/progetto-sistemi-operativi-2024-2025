#ifndef MSG_H
#define MSG_H

extern int* MSG_NOTIFY_WORKER_IDS;
extern int MSG_NOTIFY_DISPENSER_ID;
extern int* MSG_NOTIFY_USER_IDS;
extern int MSG_NOTIFY_CLOCK_ID;
extern int MSG_STATS_DATA_ID;
extern int MSG_STATS_METADATA_ID ;

void msg_init(void);
void msg_config(void);

#endif
