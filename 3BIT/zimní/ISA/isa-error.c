/**
 * Projekt ISA
 * Implementacni soubor pro chyby
 * @brief Implementacni soubor pro chyby
 * @Author Matej Mitas, xmitas02
 * @file isa-error.c
 */

#include "isa-error.h"

int isa_error_msg(char *msg) {
	fprintf(stderr, ">%s\n", msg);
	return ISA_PRG_ERR;
}
