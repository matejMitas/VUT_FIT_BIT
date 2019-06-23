/**
 * Projekt IPK-proj1
 * Hlavickovy soubor pro klienta
 * @brief Hlavickovy soubor pro klienta
 * @Author Matej Mitas, xmitas02
 * @file ipk-client.h
 */

#ifndef IPK_CLIENT_H
#define IPK_CLIENT_H 

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
// 
#include <sys/socket.h>
// pro internetove adresy
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
// kontrola rozsahu 
#include <stdint.h>
// nacti protokol
#include "ipk-protocol.h"
#include "ipk-error.h"

#define LOGIN_LEN   32
#define IP_LEN      16

#endif
