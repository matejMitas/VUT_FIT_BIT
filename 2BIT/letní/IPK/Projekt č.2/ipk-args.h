/**
 * Projekt IPK-proj2
 * Rozhrani cteni argumentu cmd
 * @brief Rozhrani cteni argumentu cmd
 * @Author Matej Mitas, xmitas02
 * @file ipk-args.h
 */

#ifndef IPK2_ARGS_H
#define IPK2_ARGS_H 

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "ipk-error.h"

#define LOGIN_LEN   32
#define IP_LEN      16

/**
 * Enum pro nastaveni modu vraceneho obsahu
 */
enum ipk18_type {
    A,
    AAAA,
    NS,
    PTR,
    CNAME
};

/**
 * Struktura pro parametry
 */
typedef struct ipk18_client_Args {
	bool help;			// prepinac pro napovedu
    char *server; 		// pro pingovani
    long int timeout; 		// vyprseni udalosti
    bool iterative; 	// iteracni vyhledavani
    char *name; 		// jmeno serveru
    // co vlastne mame vracet
    enum ipk18_type type; 
} ipk18_client_Args;

ipk18_client_Args * ipk18_read_args(int argc, char *argv[]);
void ipk18_usage_args();

#endif
