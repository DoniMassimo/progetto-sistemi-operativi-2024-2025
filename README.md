# Progetto Sistemi Operativi 2024-2025

## Descrizione del Progetto

Questo progetto implementa un sistema di gestione di un ufficio postale simulato. Include la gestione di code per vari servizi, la gestione dei posti a sedere per i lavoratori, e la gestione delle notifiche per gli utenti e i lavoratori.

## Struttura del Progetto

- **src/**: Contiene i file sorgente del progetto.
  - `ticket_dispenser_main.c`: Gestisce la distribuzione dei biglietti per i servizi.
  - `worker_main.c`: Gestisce le operazioni dei lavoratori.
  - `user_main.c`: Gestisce le richieste degli utenti.
  - `clock_main.c`: Gestisce il tempo e le notifiche temporali.
  - `utils.c`: Contiene funzioni di utilità.
  - `config.c`: Carica la configurazione del sistema.
  - `calendar.c`: Gestisce il calendario delle prenotazioni.
  - `unity.c`: Framework di test per il progetto.
- **include/**: Contiene i file header del progetto.
  - `seats.h`: Definisce le strutture e le funzioni per la gestione dei posti a sedere.
  - `config.h`: Definisce le configurazioni del sistema.
  - `ftok_key.h`: Definisce le chiavi per la comunicazione interprocesso.
  - `shm.h`: Definisce le funzioni per la gestione della memoria condivisa.
  - `unity.h`: Framework di test Unity.
  - `unity_internals.h`: Internals del framework di test Unity.
- **Makefile**: File per la compilazione del progetto.

## Compilazione

Per compilare il progetto, eseguire il comando:

```sh
make
```

## Esecuzione

Per eseguire il progetto, utilizzare il seguente comando:

```sh
./manager_main
```

Questo comando avvierà tutti i componenti necessari del progetto.

## Configurazione

La configurazione del sistema è definita nel file `config.c`. È possibile modificare i parametri di configurazione come il numero di lavoratori, utenti, e la durata dei servizi.

## Test

Il progetto utilizza il framework di test Unity. Per eseguire i test, utilizzare il comando:

```sh
make test
```

## Autori

- Davide Gioetto
- Massimo Doni
