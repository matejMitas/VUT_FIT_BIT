/**
 * Projekt IPK-proj1
 * Implementacni soubor protokol
 * @brief Implementacni soubor protokol
 * @Author Matej Mitas, xmitas02
 * @file ipk-protocol.c
 */

#include "ipk-protocol.h"
#include "ipk-error.h"

// tyhle vsechny funkce budou vracet pozici v bufferu
// poresit to s pointerama
int ipk18_protocol_ser_int16(char *buffer, uint16_t value, int i) {
    buffer[i] = value >> 8;
    buffer[i+1] = value;
    // vratime posunuty index
    return i+1;
}

int ipk18_protocol_sep(char *buffer, int type, int i) {
    buffer[i+1] = type;
    return i+2;
}

uint16_t ipk18_protocol_deser_int16(char msb, char lsb) {
    uint16_t value = 0;
    // preneseni hodnot
    value |= msb;
    value |= lsb;
    return value;
}


char * ipk18_protocol_decode(char *buffer) {
    // index pro posouvani bufferu
    int i_buffer = 19;
    // kontrolni string
    char check_str[21];
    // zkopirujeme kontrolni string
    for (int i = 0; i <= i_buffer; i++)
        check_str[i] = buffer[i];
    // musime pridat koncovou nulu
    check_str[20] = 0;
    // zkontrolujeme, ze nam jde spravny protokol
    if (strcmp(check_str, "ipk18-proj1_xmitas02") != 0) {
        ipk18_error_msg("Nesedi kontrolni retezec v hlavicce");
        return NULL;
    }

    // zkontrolujeme spravnou separaci hlavicky dat
    if (buffer[20] != 31 || buffer[22] != 31 || buffer[25] != 30) {
        ipk18_error_msg("Nesedi oddelovaci, je mozne, ze nekdo upravil zpravu");
        return NULL;
    }

    // precteme si typ zpravy
    uint8_t type = buffer[21];

    if (!(type > 0 && type < 7)) {
        ipk18_error_msg("Neexistujici typ zpravy");
        return NULL;
    }

    // delka dat
    uint16_t data_len = ipk18_protocol_deser_int16(buffer[23], buffer[24]);
    
    // pridej data
    char *data = (char *) malloc(sizeof(char) * data_len + 1); 
    if (data == NULL)
        return NULL;

    for (int i = 0; i < data_len; i++)
        data[i] = buffer[i+26];

    // ukoncime to nulou
    data[26+data_len] = 0;
    
    // vratime data
    return data;
}


void ipk18_protocol_encode(char *buffer, uint8_t type, uint16_t data_len, char *data) {
    // funkce postupne zapisuji do bufferu,
    // takze je nutne mi poradek v zapisu
    // 20 je delka kontrolniho stringu
    int i_buffer = 19;
    
    // kontrolni string
    char *header = "ipk18-proj1_xmitas02";
    for (int i = 0; i <= i_buffer; i++)
        buffer[i] = header[i];
    // dame zarazku
    i_buffer = ipk18_protocol_sep(buffer, 31, i_buffer);

    // stav zpravy
    buffer[i_buffer] = type;
    i_buffer = ipk18_protocol_sep(buffer, 31, i_buffer);

    // delka dat
    i_buffer = ipk18_protocol_ser_int16(buffer, data_len, i_buffer);
    i_buffer = ipk18_protocol_sep(buffer, 30, i_buffer);

    // zkopirujeme data
    // zde jiz musime pocitat s tim, ze pracujeme s posunutym indexem
    for (int i = 0; i < data_len; i++) {
        buffer[i_buffer + i] = data[i];
    }
}
