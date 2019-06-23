/**
 * Projekt IPK-proj2
 * Hlavni soubor implementace domenoveho vyhledavace
 * @brief Hlavni soubor implementace domenoveho vyhledavace
 * @Author Matej Mitas, xmitas02
 * @file ipk-lookup.c
 */

#include "ipk-lookup.h"

// Zhodnotime odpoved
void ipk18_dns_Resp_parse_ntohs(ipk18_dns_Resp *resp) {
	if (resp) {
		resp->type = ntohs(resp->type);
		resp->class = ntohs(resp->class);
		resp->ttl = ntohs(resp->ttl);
		resp->rdlength = ntohs(resp->rdlength);
	}
}


// Vytiskneme IPv4 adresu
int ipk18_dns_Resp_print_ipv4(char *buffer, int index) {
	printf("%d.%d.%d.%d", (uint8_t) buffer[index],
						  (uint8_t) buffer[index+1],
						  (uint8_t) buffer[index+2],
						  (uint8_t) buffer[index+3]);
	// posun o 4B
	return index+4;
}

// Vytiskneme IPv6 adresu
int ipk18_dns_Resp_print_ipv6(char *buffer, int index) {
	bool is_already_null = false;

	for (int i = index+1; i < index+16; i = i+2) {
		// spojeni znaku
		uint16_t hextet = (((buffer[i-1] & 0xff) << 8) | (buffer[i] & 0xff));
		// vytisknou nebo nevytisknou :
		if (hextet == 0 && !is_already_null) {
			printf(":");
			is_already_null = true;
		} else if (hextet > 0) {
			// vytisknuti nuly na konci
			if (i == index+15) {
				printf("%x", hextet);
			} else {
				printf("%x:", hextet);
			}
		}
	}
	// posun o 16B
	return index+16;
}

// Vytiskneme string z odpovedi
void ipk18_dns_Resp_string(char *buffer, int index) {
	// zkontrolujeme skok na zacatku
	if ((uint8_t) buffer[index] == DNS_RESP_HAS_JUMP) {
		// proved skok, nacti hodnotu a rekuretne volej
		//printf("⎆ Mame skok\n");
		ipk18_dns_Resp_string(buffer, (uint8_t) buffer[index+1]);
	} else {
		//printf("⎆ Normalme cti\n");

		int number_to_read = buffer[index];
		int str_pos = index;
		// posuneme index
		str_pos++;
		
		// pokud jsme nenarazili na konec
		while (number_to_read > 0) {
			for (int i = 0; i < number_to_read; i++) {
				// precti x-znaku
				printf("%c", buffer[str_pos]);
				str_pos++;
			} 
			// najdi dalsi cislo
			number_to_read = buffer[str_pos];
			// uz jsme na konci, nemusime tisknout '.'
			if (number_to_read != 0) {
				// vytiskni tecku
				printf(".");
			}
			// kdyz bychom jeste nahodou meli skok ve skoku
			if ((uint8_t) number_to_read == DNS_RESP_HAS_JUMP) {
				ipk18_dns_Resp_string(buffer, str_pos);
			}
			str_pos++;
		}
	}
}

// komunikace mezi jmennym serverem a nami
int ipk18_dns_Com(ipk18_client_Args *args, char *buffer, long buffer_len) {
	// vytvorime socket, na kterem se budeme ptat
	int send_sock;
    if ((send_sock = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
        return ipk18_error_msg("Nepodarilo se vytvorit socket");
    }

  	// nastaveni zpozdeni
    struct timeval tm;
    // hodnoty
    tm.tv_sec = args->timeout;
    tm.tv_usec = 0;
    // pro socket
    int timeout_code = setsockopt (send_sock, SOL_SOCKET, SO_RCVTIMEO, &tm,
                sizeof(tm));

    // zkontrolovat spravnost
    if (timeout_code < 0)
    	return ipk18_error_msg("Nepodarilo nastavit timeout");

    // vytvorime adresu z parametru
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(53); // port pro DNS
    server_addr.sin_addr.s_addr = inet_addr(args->server);
    // delka adresy
    socklen_t server_addd_len = (socklen_t) sizeof(server_addr);

    // posleme DNS Query
    if (sendto(send_sock, buffer, buffer_len, 0, (struct sockaddr *) &server_addr, server_addd_len) < 0) {
        return ipk18_error_msg("Nepodarilo se vytvorit socket");
    }
    // vymazeme cely buffer
    memset(buffer, 0, DNS_SIZE);
    // naplnime buffer
    if (recvfrom(send_sock, (char *) buffer, DNS_SIZE, 0, (struct sockaddr *) &server_addr, &server_addd_len) < 0) {
        return ipk18_error_msg("Ze serveru neprisla zadna data");
    }
    // zavreme socket
    close(send_sock);
    // v poradku
    return PRG_OK;
}

// vytiskni standartizovany separator
void ipk18_dns_Resp_sep() {
	printf("\t");
}




// hlavni funkce pro beh programu
int ipk18_dns_lookup(ipk18_client_Args *args) {
	// navratovy kod
	int ret_code = PRG_OK;
	// zjistime, jestli se nahodou nejedna o iterativni
	if (args->iterative) {
		return ipk18_error_msg("Funkce iterativniho prohledavani bohuzel neni dostupna");
	}
	// do hlavicky
	uint16_t hash_id = 43951;
	// hlavni buffer, ktery budeme posilat
	char buffer[DNS_SIZE] = {0, };

	// nastaveni adresy
	char *temp_qname = NULL;

	if (args->type == PTR) {
		temp_qname = ipk18_dns_Name_ptr(args->name);
	} else {
		// prevedeme jmeno na Query tvar
		temp_qname = ipk18_dns_Name_bytes(args->name);
	}
	
	// problem s argumentem
	if (temp_qname == NULL)
		return ARG_ERR;

	// po jednotlive indexovani v bufferu
    long buffer_header = 0;
    long buffer_qname = sizeof(ipk18_dns_Header);
    long buffer_query = buffer_qname + strlen(temp_qname) + 1; 
    long buffer_len = buffer_query + sizeof(ipk18_dns_Query);

    // vytvorime DNS Query
    // priradime misto v bufferu hlavicce
    // vyplnime 12B
    ipk18_dns_Header *header = (ipk18_dns_Header *) &buffer[buffer_header];

    // nastavime id
    header->id = htons(hash_id);
	// priznakova cast
	header->opt = htons(DNS_HEADER_OPT_RECR);

	// datova cast (16b)
	header->qdcount = htons(1);
	header->ancount = htons(0); // ptame se, datova cast obsahuje 1 polozku
	header->nscount = htons(0); 
	header->arcount = htons(0); 

	// naplnime jmeno serveru
	for (int i = buffer_qname; i < buffer_query; i++) {
		// vyplnime, zde je nutny offset pro temp_qname
		// jelikoz jsme zde jiz na 12 indexu bufferu (hlavicka)
		buffer[i] = temp_qname[i-buffer_qname];
	}

	// uvolnime pretvorene jmeno, jiz neni treba, je zkopirovane
	// do naseho bufferu
	free(temp_qname);

	// pro matchovani typu requestu k cislum
	unsigned char ipk18_dns_Query_type[] = {1, 28, 2, 12, 5};

	// odkaz na typ otazky, vyplnime 21B
	ipk18_dns_Query *query = (ipk18_dns_Query *) &buffer[buffer_query];
	// naplnime polozky
	query->qtype = htons(ipk18_dns_Query_type[args->type]);
	query->qclass = htons(1);



	// sitove komunikujeme
	ret_code = ipk18_dns_Com(args, buffer, buffer_len);



    // zkontrolujeme hlavicku
    header = (ipk18_dns_Header *) &buffer;

    // zkontrolujeme, jestli mame spravnou hlavicku
    if (!(hash_id == ntohs(header->id))) {
    	return ipk18_error_msg("Ze serveru prisla odpoved se spatnym ID");
    }

    // vytvor nove prepinace
	ipk18_dns_Header_opt * header_opt = ipk18_dns_Header_init_opt();
	// precti nove prepinace
	ipk18_dns_Header_read_opt(header, header_opt);
	// vyhodnot nove prepinace
	if (ipk18_dns_Header_asses_opt(header_opt) != PRG_OK) {
		free(header_opt);
		return PRG_ERR;
	}
	// smaz prepinace
	ipk18_dns_Header_del_opt(header_opt);


	// precteme si pocet odpovedi
	int ancount = ntohs(header->ancount);
	int data_cnt = buffer_len;

	// postupne zpracujeme jednotlive odpovedi
	// POCET ODPOVEDI
	for (int i = 0; i < ancount; i++) {
		//printf("⎆ Puchod: %d\n", i+1);
		// obecny wrapper pro odpoved
		ipk18_dns_Resp *resp = (ipk18_dns_Resp *) &buffer[data_cnt];
		// normalizace dat
		ipk18_dns_Resp_parse_ntohs(resp);
		// podivame se na odpoved (precteme si jmeno)
		ipk18_dns_Resp_string(buffer, data_cnt);
		// zde vytiskeneme tab, funkce to neresi
		ipk18_dns_Resp_sep();
		// posuneme obecny citac o delku bufferu
		data_cnt = data_cnt + sizeof(resp) + 4;

		// precteme delku dat
		int data_len = resp->rdlength;

		// poresime tridu
		if (resp->class == 1) {
			printf("IN");
			ipk18_dns_Resp_sep();
		} else {
			return ipk18_error_msg("Ze serveru prisla odpoved v jine tride nez IN");
		}

		// urcime typ requestu
		switch (resp->type) {
			case 1:
				printf("A");
				ipk18_dns_Resp_sep();
				ipk18_dns_Resp_print_ipv4(buffer, data_cnt);
				break;
			case 2:
				printf("NS");
				ipk18_dns_Resp_sep();
				// podivame se na odpoved (precteme si jmeno)
				ipk18_dns_Resp_string(buffer, data_cnt);
			case 5:
				printf("CNAME");
				ipk18_dns_Resp_sep();
				// podivame se na odpoved (precteme si jmeno)
				ipk18_dns_Resp_string(buffer, data_cnt);
				break;
			case 12:
				printf("PTR");
				ipk18_dns_Resp_sep();
				// podivame se na odpoved (precteme si jmeno)
				ipk18_dns_Resp_string(buffer, data_cnt);
				break;
			case 28:
				printf("AAAA");
				ipk18_dns_Resp_sep();
				ipk18_dns_Resp_print_ipv6(buffer, data_cnt);
				break;
		}

		// preskoc index na dalsi polozku
		// (samozrejme za jeji existence)
		data_cnt = data_cnt + data_len;
		printf("\n");
	}
	
	return ret_code;
}


int main(int argc, char *argv[]) {
	// nacteme parametry prikazove radky
	ipk18_client_Args *args = ipk18_read_args(argc, argv);
	// pokud nejsou validni argumenty
	if (args == NULL)
		return ARG_ERR;
	// status programu
	int prg_stat = PRG_OK;
	// vytiskni napovedu
	if (args->help)
		ipk18_usage_args();
	else
		prg_stat = ipk18_dns_lookup(args);
	// uvolni 
	free(args);
	// 
	return prg_stat;
}
