#ifndef IPC_CONFIG_H
#define IPC_CONFIG_H

#include "config.h"

#define MAX_PATH_LEN 256
#define MAX_EXE_LEN 64

extern int START_SEM_COUNT;
extern char REL_DIR[];
extern int START_SEM_ID;
extern int SEM_SEATS_ID;
extern int SEM_SHM_NOF_SEATS_ID;
extern int SHM_NOF_SEATS_ID;
extern int SHM_SEATS_INFO_ID[];
extern int SEM_SHM_SEATS_INFO_ID;

extern void ipc_config_init_worker();
extern void ipc_config_init_manager();
extern void ipc_config_shmem_seats_info(size_t mem_size, size_t struct_num, Service ser_type);

#endif
