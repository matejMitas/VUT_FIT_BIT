/**
 * Projekt IPK-proj1
 * Hlavickovy soubor server
 * @brief Hlavickovy soubor server
 * @Author Matej Mitas, xmitas02
 * @file ipk-server.h
 */

#ifndef IPK_SERVER_H
#define IPK_SERVER_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
// sitova cast
// sockety
#include <sys/socket.h>
// pro internetove adresy
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
// nacti protokol
#include "ipk-error.h"
#include "ipk-protocol.h"
#include "ipk-parse.h"
// pro handler kodu
#include <signal.h>

#define BUFFER_LEN 1050

// ukonceni programu
int end = 0;

#endif
