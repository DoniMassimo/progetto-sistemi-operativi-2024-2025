#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#define MAX_PATH_LEN 256
#define MAX_EXE_LEN 64

extern int NOF_WORKERS;
extern int NOF_USERS;
extern int NOF_WORKER_SEATS;
extern int START_SEM_COUNT;
extern size_t N_NANO_SECS;

typedef enum
{
  SEND_PICK_PARC,   // Invio e ritiro pacchi
  SEND_LET_REG,     // Invio e lettere e raccomandate
  WDRAWALS_DEPOSIT, // Prelievi e versamenti Bancoposta
  PAY_POST_BULL,    // Pagamento bollettini postali
  PURCH_FIN_PROD,   // Acquisto prodotti finanziari
  PURCH_WATCH_BRAC, // Acquisto orologi e braccialetti
  SERV_NUM          // numero di servizi erogati
} Service;

extern void config_load(void);

#endif
