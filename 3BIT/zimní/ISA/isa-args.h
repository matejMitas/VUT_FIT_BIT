/**
 * Projekt ISA
 * Rozhrani cteni argumentu cmd
 * @brief Rozhrani cteni argumentu cmd
 * @Author Matej Mitas, xmitas02
 * @file isa-args.h
 */

#ifndef ISA_ARGS_H
#define ISA_ARGS_H 

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "isa-error.h"

/**
 * Enum pro nastaveni modu vraceneho obsahu
 */
enum isa_cmd_Args_type {
    PARSE,
    SURVEIL
};

/**
 * Struktura pro parametry
 */
typedef struct isa_cmd_Args {
	char *parse;					// chceme číst ze souboru
    long int timeout;           // jak dlouho chceme naslouchat na síťovém zařízení
    char *interface;		    // rozhraní, na kterém chceme naslouchat
    char *hostname;				// logovaci server
    //
    enum isa_cmd_Args_type mode;
} isa_cmd_Args;

void isa_args_help();
isa_cmd_Args * isa_args_parse(int argc, char *argv[]);

#endif
