/**
 * Projekt ISA
 * Vsechny define programu
 * @brief Vsechny define programu
 * @Author Matej Mitas, xmitas02
 * @file isa-defines.h
 */

#ifndef ISA_DEFINES_H
#define ISA_DEFINES_H 

/**
 * Navratove kody pro beh programu
 */
#define ISA_PRG_OK 0
#define ISA_PRG_ERR 1
#define ISA_ARG_ERR 2
#define ISA_FILTER "src port 53"
#define ISA_READ_PACKETS 1

/**
 * Barevne kody pro terminal
 */
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

/**
 * Pro nacitani packet z .pcap nebo ze site
 */
#define ISA_PACKETS_UNLIMITED 0
#define ISA_PACKETS_SINGLE 1
#define ISA_PACKET_OK 0
#define ISA_PACKET_SKIP 1
#define ISA_NO_JUMP 0
#define ISA_POSSIBLE_JUMP 0x3

/**
 * Zjednoduseni referenci
 */
#define IPV4 4 
#define IPV6 6

/**
 * Obecne pro datagram
 */
#define HEADER_WORD 32
#define ETHER_HEADER 14
#define UDP 0x11
#define TCP 0x6

/**
 * IPv4
 */
#define IHL_MASK 0xF
#define IHL_BASIC 0x5
#define IPV4_HDR_PROTOCOL 0x9
#define IPV4_SRC_ADDR_OFFSET 0xC

/**
 * IPv6
 */
#define IPV6_NEXT_HDR 0x6
#define IPV6_DATA_LEN 0x4
#define IPV6_SRC_ADDR_OFFSET 0x8

/**
 * Pro protokoly
 */
#define UDP_HEADER 0x8
#define TCP_HEADER_OFFSET 0xC


// migrace z parseru
#define ISA_DNS_HEADER_LEN 12

// Prepinace pro dotaz
#define DNS_HEADER_OPT_RECR 0x100
#define DNS_HEADER_OPT_NRECR 0x000

#define DNS_STR_LEN 868
#define DNS_MX_OFFSET 0x2
#define IPV6_TEMP_SIZE 16
#define IPV4_SIZE 16



#endif
