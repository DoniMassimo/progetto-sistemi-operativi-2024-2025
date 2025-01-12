#ifndef SEM_H
#define SEM_H

#include "sem_utils.h"

extern int SEM_START_ID;
extern int SEM_SEATS_ID;
extern int SEM_DAY_STARTED_ID;
extern int SEM_DAY_END_ID;
extern int SEM_SHM_SEATS_INFO_ID;
extern int SEM_PROC_READY_ID;
extern int SEM_NOTIFY_WORKER_ID;
extern int SEM_NOTIFY_DISPENSER_ID;
extern int SEM_NOTIFY_USER_ID;
extern int SEM_NOTIFY_CLOCK_ID;
extern int SEM_ADD_USERS_ID;
extern int SEM_CLOCK_ADD_USERS_ID;
extern int SEM_PROC_CAN_DIE_ID;
extern SemRP_Id SEMRP_CALENDAR_ID;
extern SemWP_Id SEMWP_SEATS_INFO_ID;

void sem_init(void);
void sem_config(void);
void SEM_ADD_USERS_config(void);
void sem_deallocate(void);

#endif
