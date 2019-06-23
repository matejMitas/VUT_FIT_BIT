// main.c
// Řešení IOS-DU2, 27.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Hlavní soubor pro vytváření procesů

#ifndef main_h
#define main_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include "params.h"

// define pro semafory
#define SEM_WRITE_MUTEX "/xmitas02-load-mutex"
#define SEM_BARRIER "/xmitas02-barrier"
#define SEM_BARRIER_MUTEX "/xmitas02-barrier-mutex"
#define SEM_MUTEX "/xmitas02-mutex"
#define SEM_TUTOR_LEAVE "/xmitas02-tutor"
#define SEM_INMATE_ENTER "/xmitas02-inmate"

// define pro stavy
#define STARTED "started"
#define ENTER "enter"
#define WAIT "waiting"
#define LEAVE "leaving"
#define TLEAVE "trying to leave"
#define FINISHED "finished"

// exit codes
#define PARAM_ERROR 1
#define ALLOC_ERROR 2



#endif /* main_h */
