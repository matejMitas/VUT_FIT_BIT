/**
 * Projekt IPK-proj1
 * Implementacni soubor pro chyby
 * @brief Implementacni soubor pro chyby
 * @Author Matej Mitas, xmitas02
 * @file ipk-error.c
 */

#include "ipk-error.h"

int ipk18_error_msg(char *msg) {
	fprintf(stderr, ">%s\n", msg);
	return 1;
}
