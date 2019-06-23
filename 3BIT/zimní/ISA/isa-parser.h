/**
 * Projekt ISA
 * Hlavickovy soubor pro parser
 * @brief Hlavickovy soubor pro parser
 * @Author Matej Mitas, xmitas02
 * @file isa_project.h
 */

#ifndef ISA_PARSER_H
#define ISA_PARSER_H 

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

#define u_int __u_int
#define u_char __u_char
#define u_short __u_short

#define ETYPEIP 0x800
#define ETYPEIP6 0x86dd


struct eth_header {
	u_char 	ether_dhost [6];
	u_char 	ether_shost [6];
	u_short ether_type;
};


#include <pcap.h>

// nás oblíbený syslog
// #include "isa-pcap_parser.h"
#include "isa-structs.h"
#include "isa-defines.h"
#include "isa-db.h"

void isa_parse_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

#endif
