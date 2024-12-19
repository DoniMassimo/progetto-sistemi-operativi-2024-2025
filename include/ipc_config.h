#ifndef IPC_CONFIG_H
#define IPC_CONFIG_H

#include "config.h"

#define MAX_PATH_LEN 256
#define MAX_EXE_LEN 64

extern char REL_DIR[];
extern int SEM_START_ID;
extern int SEM_SEATS_ID;
extern int SEM_SHM_SEATS_INFO_ID;
extern int SHM_SEATS_INFO_ID;
extern int SHM_SEATS_INDEX_ID;

extern void ipc_config_init_worker();
extern void ipc_config_init_manager();

#endif
