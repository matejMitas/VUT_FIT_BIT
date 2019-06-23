/**
 * Projekt ISA
 * Ukladani DNS zaznamu
 * @brief Ukladani DNS zaznamu
 * @Author Matej Mitas, xmitas02
 * @file isa-db.c
 */

#include "isa-db.h"

// globaly pro praci s databazi
isa_db_Wrapper db_data = { NULL, NULL, 0};
isa_db_Item *curr = NULL;
char *host = NULL;

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */
void isa_change_host(char *received_host) {
	// odstranime singleton od prechoziho pruchodu
	if (host) {
		free(host);
	}
	host = received_host;
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */
int isa_init_item() {
	// nova polozka
	curr = (isa_db_Item *) malloc(sizeof(isa_db_Item));
	if (curr == NULL) {
		return ISA_PRG_ERR;
	}
	// vynulujeme dalsi polozku, zatim zadnou nemame
	curr->type = A;
	curr->name = NULL;
	curr->source = NULL;
	curr->payload = NULL;
	curr->count = 0;

	curr->next = NULL;

	return ISA_PRG_OK;
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 */
void isa_add_type(enum dns_data_types type) {
	if (type) {
		curr->type = type;
	}
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 */
void isa_add_source(char *source) {
	if (source) {
		curr->source = source;
	}
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 */
void isa_add_name(char *name) {
	if (name) {
		curr->name = name;
	}
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 */
void isa_add_payload(char *payload) {
	if (payload) {
		curr->payload = payload;
	}
}

int isa_compare_items(isa_db_Item *a, isa_db_Item *b) {
	// nejlevnejsi je kontrola typu
	if (a->type != b->type) {
		return -1;
	}
	if (strcmp(a->name, b->name) != 0) {
		return -1;
	} 
	if (strcmp(a->payload, b->payload) != 0) {
		return -1;
	}
	// neni potreba, nasli jsme duplikat
	// free(b->name);
	// free(b->source);
	// free(b->payload);
	// free(b);
	// pricteme pocet polozek
	a->count += 1;
	return 0;
}

int isa_check_item_exists(isa_db_Item *item) {
	if (db_data.start != NULL && db_data.end != NULL && db_data.size > 0) {
		if (db_data.size == 1) {
			return isa_compare_items(db_data.start, curr);
		} else {
			isa_db_Item *ptr = db_data.start;
			while (ptr != NULL) {
				if (isa_compare_items(db_data.start, curr) == 0) {
					return 0;
				}
				ptr = ptr->next;
			}
		}
	}
	return -1;
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */
void isa_add_item() {
	// zkopirujeme hosta pro kazdou polozku
	if (host) {
		curr->source = (char *) malloc(sizeof(char) * strlen(host));
		if (curr->source == NULL) {
			return;
		}
		if (curr->source) {
			strcpy(curr->source, host);
		}
	}
	curr->count = 1;
	// pokud je prazdna databeze
	if (db_data.start == NULL && db_data.end == NULL && db_data.size == 0) {
		db_data.start = curr; 
		db_data.end = curr;
	} else if (db_data.size == 1) {
		// vyhledat v databazi
		db_data.start->next = curr;
		db_data.end = curr;
	}
	// mame polozky
	else {
		isa_db_Item *ptr = db_data.end;
		ptr->next = curr;
		db_data.end = curr; 	
	}

	db_data.size += 1;
	// konec davame vzdy
	db_data.end = curr;
	// reset prvku
	curr = NULL;
}
/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */
char * isa_print_type(int type) {
	switch (type) {
		case A:
			return "A";
		case AAAA:
			return "AAAA";
		case CNAME:
			return "CNAME";
		case PTR:
			return "PTR";
		case TXT:
			return "TXT";
		case MX:
			return "MX";
		case SOA:
			return "SOA";
		case NS:
			return "NS";
	}
	return NULL;
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */
// komunikace mezi jmennym serverem a nami
int isa_send_to_server(char *server, char *buffer, long buffer_len) {
	// vytvorime socket, na kterem se budeme ptat
	int send_sock;
    if ((send_sock = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
        isa_error_msg("Nepodarilo se vytvorit socket");
    	return ISA_PRG_ERR;
    }

    // vytvorime adresu z parametru
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(514); // port pro DNS
    server_addr.sin_addr.s_addr = inet_addr(server);
    // delka adresy
    socklen_t server_addd_len = (socklen_t) sizeof(server_addr);

    // posleme DNS Query
    if (sendto(send_sock, buffer, buffer_len, 0, (struct sockaddr *) &server_addr, server_addd_len) < 0) {
        isa_error_msg("Nepodarilo se vytvorit socket");
    	return ISA_PRG_ERR;
    }
    // zavreme socket
    close(send_sock);
    // v poradku
    return ISA_PRG_OK;
}

char * isa_timestamp() {
	char *timestamp = (char *) malloc (sizeof(char) * 30);
	if (timestamp == NULL) {
		return NULL;
	}
	// structy pro casove udaje
	struct timeval millis;
	time_t ltime;
	struct tm *tm;
	// nacteme data, i pro mikrosec
	gettimeofday(&millis, NULL);
	ltime = time(NULL);
	tm = localtime(&ltime);
	// vytiskene timestapm do bufferu
	sprintf(timestamp,"%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", 
		tm->tm_year+1900, tm->tm_mon, 
	    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
	    millis.tv_usec / 1000);
	//
	return timestamp;
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */
void isa_print_item(isa_db_Item *item, char *server) {
	char buffer[1024] = {0, };
	char *timestamp = isa_timestamp();

	int chars = sprintf(buffer, "<134>1 %s %s dns-export - - - %s %s %s %d", 
		timestamp, item->source, item->name, isa_print_type(item->type), item->payload, item->count);

	isa_send_to_server(server, buffer, chars);
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */
void isa_print_db(char *server) {
	if (db_data.start != NULL && db_data.end != NULL && db_data.size > 0) {
		if (db_data.size == 1) {
			isa_print_item(db_data.start, server);
		} else {
			isa_db_Item *ptr = db_data.start;
			while (ptr != NULL) {
				isa_print_item(ptr, server);
				ptr = ptr->next;
			}
		}
	}
}
