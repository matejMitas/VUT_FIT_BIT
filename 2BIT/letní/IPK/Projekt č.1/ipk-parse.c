/**
 * Projekt IPK-proj1
 * Implementace pro parser /etc/passwd
 * @brief Implementace pro parser /etc/passwd
 * @Author Matej Mitas, xmitas02
 * @file ipk-parse.c
 */

#include "ipk-parse.h"
#include "ipk-protocol.h"


ipk18_str_index * ipk18_server_str_pos(char *str, int start, int end) {

	ipk18_str_index *indexes = (ipk18_str_index *) malloc(sizeof(ipk18_str_index));

	if (indexes == NULL) {
		return NULL;
	}

	int pos = 0;
	int count = 0;

	char* c = str;
	while (*c) {
		if (*c == ':') {
			if (count == start)
				indexes->start = pos;
			else if (count == end)
				indexes->end = pos;
			count++;
		}
		pos++;
		c++;
	} 

	return indexes;
}

int ipk18_server_prefix(char *str1, char *str2) {

	long str1_len = strlen(str1);
	long str2_len = strlen(str2);
	// nastaveni delky
	int len = (str1_len > str2_len) ? str2_len : str1_len;
	// optimalizace
	if (str2_len == str1_len && str1_len == 1) {
		return (str1 == str2) ? 0 : 1;
	} else {
		for (int i = 0; i < len; i++) {
			if (*str1 != *str2)
				return 1;
			str1++;
			str2++;
		}
	}
	return 0;
}

char * ipk18_server_parse_line(char *line, char *login, char flag) {
	// vytvorime string na porovnani
	int len = strcspn(line, ":");
	char *name = (char *) malloc(sizeof(char) * len + 1);

	if (name == NULL) {
		return NULL;
	}

	strncpy(name, line, len);
	// ukoncujici znak retezce
	name[len]='\0';

	// zde urcime, o jakou funkcionalitu se jedna 
	if (flag == 'n' || flag == 'f') {
		// tato cast je spolecna, urcime shodu
		// mame shodu, muzeme pracovat
		if (strcmp(name, login) == 0) {
			// pole indexu
			ipk18_str_index *i = NULL;
			// zde se rozhodneme, co chceme za moznost
			if (flag == 'n')
				i = ipk18_server_str_pos(line, 3, 4);
			else if (flag == 'f')
				i = ipk18_server_str_pos(line, 4, 5);

			// samotna logika kopirovani
			int ret_len = i->end - i->start;
			// char ret_str
			char *ret_value = (char *) malloc(sizeof(char) * ret_len);

			if (ret_value == NULL) {
				free(name);
				return NULL;
			}
			// najedeme si pointerem na spravny zacatek
			char *ret_value_beg = line + i->start + 1;
			// pouze zkopirujeme
			strncpy(ret_value, ret_value_beg, ret_len-1);
			// vratime hodnotu
			return ret_value;
		} else {
			// nemame, jdeme dal
			return NULL;
		}
		// uvolnime jmeno
		free(name);
	
	// hledame substring ve jmene
	} else if (flag == 'l') {
		if (!ipk18_server_prefix(name, login))
			return name;
	}

	return NULL;
}

/**
 * @brief Vytisne na stderr
 * @param Chybova hlaska
 * @return Chybovy stav
 */
bool ipk18_server_parse(int client_sock, char f, char *login) {
	// vstupni soubor
	FILE *open = fopen("/etc/passwd", "r");
	// file descriptor
	if (open == NULL) {
		return NULL;
	}
	// na nacitani radku
	char line[MAX_LINE];
	int i_line = 0;
	int c;
	bool ret = false;
	// pouziti komentare
	bool comment_flag = false;
	// osekani stringu z obou stran
	bool trim = true;
	// temp buffer pro vypisovani radku
	char buffer[BUFFER_LEN];

	while ((c = fgetc(open)) != EOF) {
		// preskocime na konec radku
		if (c != '\n' && i_line < (MAX_LINE - 1)) {
			// poreseni radku s komentarem
			if (c == '#')
				comment_flag = true;
			// necheme nic odebirat ze zacatku
			if (!isspace(c))
				trim = false;
			// nechceme nic trimovat ani komentovat
			if (!trim && !comment_flag) {
				line[i_line] = c;
				i_line++;
			}
		} else {
			// ukoncovani znak
			line[i_line] = '\0';
			// nezajima nas prazdny radek
			if (i_line) {
				char *value = ipk18_server_parse_line(line, login, f);
				
				if (value) {
					// tady to dostane socket
					//printf("%s\n", value);

					ipk18_protocol_encode(buffer, 4, strlen(value) + 1, value);
					write(client_sock, buffer, BUFFER_LEN);
					//
					ret = true;
				}
				// zde zavolat hodnotu
			}
			// reset citace
			i_line = 0;
			comment_flag = false;
			trim = true;
			// vyprazdenni bufferu
		}
	}
	
	// zavri soubor
	fclose(open);
	return ret;
}
