/**
 * Projekt ISA
 * Vsechny structy programu
 * @brief Vsechny structy programu
 * @Author Matej Mitas, xmitas02
 * @file isa-structs.h
 */

#ifndef ISA_STRUCTS_H
#define ISA_STRUCTS_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

enum dns_data_types {
	A 		= 1,
	AAAA 	= 28,
	CNAME	= 5,
	MX		= 15,
	NS 		= 2,
	SOA		= 6,
	TXT		= 16,
	SPF		= 99,
	PTR 	= 12,
	// DNSSSEC
	RRSIG 	= 46,
	DNSKEY  = 48,
	DS 		= 43,
	NSEC	= 47
};

typedef struct isa_db_Item {
	// typ odpovedi
	enum dns_data_types type;
	// pro porovnani
	char *source;
	char *name;
	char *payload;
	// pocet vyskytu
	uint32_t count;
	// linearni list
	struct isa_db_Item * next;
} isa_db_Item;

typedef struct isa_db_Wrapper {
	isa_db_Item * start;
	isa_db_Item * end;
	uint32_t size;
} isa_db_Wrapper;

/**
 * Pro parser
 */
typedef struct isa_Packet_header {
	uint8_t ip;
	uint8_t protocol;
	uint8_t payload;
	uint16_t ether;
	uint16_t len;
	uint16_t dns_start;
} isa_Packet_header;


/**
 * DNS hlavicka
 */
typedef struct isa_dns_Header {
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

} isa_dns_Header;

/**
 * DNS dotaz
 */
typedef struct isa_dns_Query {
	// typ pro dotaz
	uint16_t qtype; // A a tak dale
	uint16_t qclass;
} isa_dns_Query;

/**
 * DNS odpoved
 */
// odstraneni zarovnani na 16B, kdyz my potrebujeme
// pouze 12B, bordel v programu
#pragma pack(push, 1)

typedef struct isa_dns_Resp {
	uint8_t has_jump;
	uint8_t where_jump;
	// typ pro odpoved
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
	// rdata zde nejsou, jsou samostatne alokovane
} isa_dns_Resp;

#pragma pack(pop)


// odstraneni zarovnani na 16B, kdyz my potrebujeme
// pouze 12B, bordel v programu
#pragma pack(push, 1)

typedef struct isa_dns_Resp_generic {
	// typ pro odpoved
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
	// rdata zde nejsou, jsou samostatne alokovane
} isa_dns_Resp_generic;

#pragma pack(pop)

#endif
