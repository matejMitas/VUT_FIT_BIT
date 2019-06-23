/**
 * Projekt IPK-proj1
 * Hlavickovy soubor pro parser /etc/passwd
 * @brief Hlavickovy soubor pro parser /etc/passwd
 * @Author Matej Mitas, xmitas02
 * @file ipk-parse.h
 */

#ifndef IPK_PARSE_H
#define IPK_PARSE_H 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_LINE 1024

typedef struct ipk18_str_index {
	int start;
	int end;
} ipk18_str_index;

ipk18_str_index * ipk18_server_str_pos(char *str, int start, int end);
int ipk18_server_prefix(char *str1, char *str2);
char * ipk18_server_parse_line(char *line, char *login, char flag);
bool ipk18_server_parse(int client_sock, char f, char *login);

#endif
