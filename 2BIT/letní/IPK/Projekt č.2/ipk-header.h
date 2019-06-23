/**
 * Projekt IPK-proj2
 * Rozhrani pro hlavicku DNS dotazu
 * @brief Rozhrani pro hlavicku DNS dotazu
 * @Author Matej Mitas, xmitas02
 * @file ipk-header.h
 */

#ifndef IPK2_HEADER_H
#define IPK2_HEADER_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
// sitarina
#include <sys/socket.h>  
#include <netinet/in.h>
#include <arpa/inet.h> 

#include <math.h>

#include "ipk-dns_containers.h"
#include "ipk-error.h"

ipk18_dns_Header_opt * ipk18_dns_Header_init_opt();
void ipk18_dns_Header_read_opt(ipk18_dns_Header *header, ipk18_dns_Header_opt *opt);
int ipk18_dns_Header_asses_opt(ipk18_dns_Header_opt *opt);
void ipk18_dns_Header_del_opt(ipk18_dns_Header_opt *opt);

#endif
