#ifndef SHM_H
#define SHM_H

extern int SHM_SEATS_INFO_ID;
extern int SHM_SEATS_INDEX_ID;
extern int SHM_WORKERS_PID_ID;
extern int SHM_TICKET_DISPENSER_PID_ID;
extern int SHM_CALENDAR_ID;
extern int SHM_MIN_COUNT_ID;

void shm_init(void);
void shm_config(void);
void shm_deallocate(void);

#endif
