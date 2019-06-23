/**
 * Projekt IPK-proj2
 * Hlavickovy soubor pro prevod adres na spravny tvary pro DNS
 * @brief Hlavickovy soubor pro prevod adres na spravny tvary pro DNS
 * @Author Matej Mitas, xmitas02
 * @file ipk-name.h
 */

#ifndef IPK2_NAME_H
#define IPK2_NAME_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include <sys/socket.h>  
#include <netinet/in.h>
#include <arpa/inet.h> 

#include "ipk-dns_containers.h"

#include "ipk-error.h"

char * ipk18_dns_Name_bytes(char *name);
char * ipk18_dns_Name_ptr(char *name);

#endif
