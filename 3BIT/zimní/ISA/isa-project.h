/**
 * Projekt ISA
 * Hlavickovy soubor pro hlavni program
 * @brief Hlavickovy soubor pro hlavni program
 * @Author Matej Mitas, xmitas02
 * @file isa_project.h
 */

#ifndef ISA_PROJECT_H
#define ISA_PROJECT_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <time.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/if_ether.h>
#include <signal.h>

#define u_int __u_int
#define u_char __u_char
#define u_short __u_short

#include <pcap.h>

// nás oblíbený syslog
// #include "isa-pcap_parser.h"
#include "isa-structs.h"
#include "isa-defines.h"
#include "isa-args.h"
#include "isa-parser.h"
#include "isa-db.h"

// def pro
typedef struct pcap_if pcap_if;

#endif
