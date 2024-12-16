#ifndef CONFIG_H
#define CONFIG_H

#define MAX_PATH_LEN 256
#define MAX_EXE_LEN 64

extern int NOF_WORKERS;
extern int WORKER_SEETS;
extern int NOF_USERS;
extern int NOF_WORKER_SEATS;
extern int START_SEM_COUNT;
extern char REL_DIR[];
extern int START_SEM_ID;
extern int SEM_SEATS_ID;

typedef enum
{
  SEND_PICK_PARC,   // Invio e ritiro pacchi
  SEND_LET_REG,     // Invio e lettere e raccomandate
  WDRAWALS_DEPOSIT, // Prelievi e versamenti Bancoposta
  PAY_POST_BULL,    // Pagamento bollettini postali
  PURCH_FIN_PROD,   // Acquisto prodotti finanziari
  PURCH_WATCH_BRAC, // Acquisto orologi e braccialetti
  SERV_NUM          // numero di servizi erogati
} Services;

extern void config_load();
extern void config_init();
#endif
