/**
 * Projekt IPK-proj1
 * Hlavickovy soubor protokol
 * @brief Hlavickovy soubor protokol
 * @Author Matej Mitas, xmitas02
 * @file ipk-protocol.h
 */

#ifndef IPK_PROTOKOL_H
#define IPK_PROTOKOL_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#define BUFFER_LEN 1050

typedef struct ipk18_protocol {
	// typ operace, kterou chceme provadet
	uint8_t type;
	// opravdova delka dat
	uint16_t data_len;	
	// hodnota, kterou chceme poslat
	char data[];
} ipk18_protocol;


int ipk18_protocol_ser_int16(char *buffer, uint16_t value, int i);
uint16_t ipk18_protocol_deser_int16(char msb, char lsb);
int ipk18_protocol_sep(char *buffer, int type, int i);
char * ipk18_protocol_decode(char *buffer);
void ipk18_protocol_encode(char *buffer, uint8_t type, uint16_t data_len, char *data);

#endif
