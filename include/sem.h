#ifndef SEM_H
#define SEM_H

extern int SEM_START_ID;
extern int SEM_SEATS_ID;
extern int SEM_DAY_STARTED_ID;
extern int SEM_DAY_END_ID;
extern int SEM_SHM_SEATS_INFO_ID;
extern int SEM_PROC_READY_ID;
extern int* SEM_NOTIFY_WORKER_IDS;
extern int SEM_NOTIFY_DISPENSER_ID;

void sem_init(void);
void sem_config(void);

#endif
