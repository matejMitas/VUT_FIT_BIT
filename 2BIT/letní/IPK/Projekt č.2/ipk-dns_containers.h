/**
 * Projekt IPK-proj2
 * Hlavickovy pro popis jednotlivych structu
 * @brief Hlavickovy pro popis jednotlivych structu
 * @Author Matej Mitas, xmitas02
 * @file ipk-dns_containers.h
 */

#ifndef IPK_DNS_COINTAINERS_H
#define IPK_DNS_COINTAINERS_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

// navratove kody
#define PRG_OK 0
#define PRG_ERR 1
#define ARG_ERR 2

// DNS muze mit maximalne 512B
#define DNS_SIZE 512

// Prepinace pro dotaz
#define DNS_HEADER_OPT_RECR 0x100
#define DNS_HEADER_OPT_NRECR 0x000

// Skoky
#define DNS_RESP_HAS_JUMP 192

// Adresy
#define DNS_IPV4_SIZE 4
#define DNS_IPV6_SIZE 16

// delka IPV4 adresy v binarni forme 
#define DNS_BINARY_IPV4 30
#define DNS_BINARY_IPV6 74
#define DNS_BINARY_NIBBLE_IPV6 64
#define DNS_BINARY_OFFSET 4

/**
 * DNS hlavicka
 */
typedef struct ipk18_dns_Header {
	// id transakce (16b)
	uint16_t id;
	// priznakova cast
	// horni cast 2B
	uint16_t opt;
	// datova cast (16b)
	uint16_t qdcount; // kolik vet ma dotaz
	uint16_t ancount; // kolik vet ma odpoved 
	uint16_t nscount; // sekce pro odkazy na autoritativni servery
	uint16_t arcount; // doplnujici informace

} ipk18_dns_Header;

/**
 * DNS hlavicka, pro cteni parametru
 */
typedef struct ipk18_dns_Header_opt {
	uint8_t qr;
	uint8_t opcode;
	uint8_t aa;
	uint8_t tc;
	uint8_t rd;
	uint8_t ra;
	uint8_t z;
	uint8_t rcode;
} ipk18_dns_Header_opt;

/**
 * DNS dotaz
 */
typedef struct ipk18_dns_Query {
	// typ pro dotaz
	uint16_t qtype; // A a tak dale
	uint16_t qclass;
} ipk18_dns_Query;

/**
 * DNS odpoved
 */

// odstraneni zarovnani na 16B, kdyz my potrebujeme
// pouze 12B, bordel v programu
#pragma pack(push, 1)

typedef struct ipk18_dns_Resp {
	uint8_t has_jump;
	uint8_t where_jump;
	// typ pro odpoved
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
	// rdata zde nejsou, jsou samostatne alokovane
} ipk18_dns_Resp;

#pragma pack(pop)

#endif
