/**
 * Projekt ISA
 * Soubor pro hlavni program
 * @brief Soubor pro hlavni program
 * @Author Matej Mitas, xmitas02
 * @file isa_parser.c
 */

#include "isa-parser.h"

// Zhodnotime odpoved
void isa_dns_resp_parse_ntohs(isa_dns_Resp_generic *resp) {
	if (resp) {
		resp->type = ntohs(resp->type);
		resp->class = ntohs(resp->class);
		resp->ttl = ntohs(resp->ttl);
		resp->rdlength = ntohs(resp->rdlength);
	}
}

void isa_dns_header_parse_ntohs(isa_dns_Header * header) {
	if (header) {
		header->id = ntohs(header->id);
		header->opt = ntohs(header->opt);
		//
		header->qdcount = ntohs(header->qdcount);
 		header->ancount = ntohs(header->ancount);
		header->nscount = ntohs(header->nscount);
		header->arcount = ntohs(header->arcount);
	}
}

// Vytiskneme IPv4 adresu
char * isa_print_ipv4(const u_char *buffer, int index) {
	char *ipv4_addr = (char *) malloc(sizeof(char) * IPV4_SIZE);
	if (ipv4_addr == NULL) {
		return NULL;
	}
	sprintf(ipv4_addr,
			"%d.%d.%d.%d", buffer[index],
						   buffer[index+1],
						   buffer[index+2],
						   buffer[index+3]);
	ipv4_addr[IPV4_SIZE-1] = '\0';
	return ipv4_addr;
}

// Vytiskneme IPv6 adresu
char * isa_print_ipv6(const u_char *buffer, int index) {
	char *ipv6_addr = (char *) malloc(sizeof(char) * INET6_ADDRSTRLEN);
	char *ipv6_temp = (char *) malloc(sizeof(char) * IPV6_TEMP_SIZE);
	if (ipv6_addr == NULL || ipv6_temp == NULL) {
		return NULL;
	}
	// naplnime temp
	for (int i = index, j = 0; i < index + IPV6_TEMP_SIZE; i++, j++) {
		ipv6_temp[j] = buffer[i];
	}
	// prevod
	inet_ntop(AF_INET6, ipv6_temp, ipv6_addr, INET6_ADDRSTRLEN);
	// docasna pro prevod, dale pracujeme s ipv6_addr
	free(ipv6_temp);
	return ipv6_addr;
}

void isa_debug_print_dns_header(isa_dns_Header * header) {
	printf(RED);
	printf("\n>>>>>>>>>>>>>>>> Header <<<<<<<<<<<<<<<<<\n");
	printf(RESET);

	printf("Id: 		%x\n", header->id);
	printf("Opt: 		%x\n", header->opt);
	//
	printf("Questions: 	%x\n", header->qdcount);
	printf("Answers: 	%x\n", header->ancount);
	printf("Authority: 	%x\n", header->nscount);
	printf("Additional: 	%x\n", header->arcount);
}


void isa_inc_offset(isa_Packet_header *packet, int amount) {
	if (packet) {
		packet->payload += amount;
	}
}

void isa_set_offset(isa_Packet_header *packet, int amount) {
	if (packet) {
		packet->payload = amount;
	}
}

int isa_offset(isa_Packet_header *packet) {
	return packet ? packet->payload : -1; 
}

void isa_read_data_packet(isa_Packet_header *packet, const u_char *data) {
	for (int i = packet->payload; i < packet->len; i++) {
		printf("%02x ", data[i]);
	}
}

/**
 * @brief TODO: doplnit komentar
 * @param packet ziskane informace o paketu
 * @param possible_protocol mozny protokol na overeni
 * @param data ukazatel na data
 * @return spravnost operace
 */
int isa_check_packet_valid(isa_dns_Header *header) {
	if (header->opt >> 15 == 1 && header->ancount > 0) {
		return ISA_PACKET_OK;
	} else {
		return ISA_PACKET_SKIP;
	}
}

/**
 * @brief TODO: doplnit komentar
 * @param packet ziskane informace o paketu
 * @param possible_protocol mozny protokol na overeni
 * @param data ukazatel na data
 * @return spravnost operace
 */

uint16_t isa_dns_jump(const u_char *data, int start) {
	if ((data[start] >> 6) == ISA_POSSIBLE_JUMP) {
		uint16_t val = (data[start] & 0x3f << 8) | (data[start + 1] & 0xff);
		return val;
	} else {
		return ISA_NO_JUMP;
	}
}

char * isa_print_string(const u_char *data, int start, int dns_start) {
	// vytvoreni prostoru pro jmeno
	char *ret = (char *) malloc(sizeof(char) * DNS_STR_LEN);
	if (ret == NULL) {
		return NULL;
	}
	int ret_pos = 0;

	// skos na zacatku?
	int has_jump_start = isa_dns_jump(data, start);
	int number_to_read;
	int str_pos;

	if (has_jump_start) {
		number_to_read = data[has_jump_start + dns_start];
		str_pos = has_jump_start + dns_start;
	} else {
		number_to_read = data[start];
		str_pos = start;
	}
	// pausalni offset
	str_pos++;
	// printf("number_to_read pred: %d\n", number_to_read);
	while (true) {
		while (number_to_read > 0) {
			// printf("%c", data[str_pos]);
			ret[ret_pos] = data[str_pos];
			//
			str_pos++;
			ret_pos++;
			number_to_read--;
		}
		// skok v textu
		if (isa_dns_jump(data, str_pos)) {
			str_pos = data[str_pos + 1] + dns_start;
			// printf("%02x\n", data[str_pos]);
			// break;
		}
	
		number_to_read = data[str_pos];
		str_pos++;

		if (number_to_read == 0) {
			ret[ret_pos] = '\0';
			break;
		} else {
			// printf(".");
			ret[ret_pos] = '.';
			ret_pos++;
		}
	}
	return ret;
}

char * isa_txt_resp(isa_Packet_header *packet, const u_char *data) {
	int txt_data_len = data[isa_offset(packet)];
	int new_start = isa_offset(packet) + 1;
	// zkopirujeme TXT
	char *txt = (char *) malloc(sizeof(char) * DNS_STR_LEN);
	if (txt == NULL) {
		return NULL;
	}
	// zkopirujeme
	int i, j = 0;
	for (i = new_start; i < new_start + txt_data_len; i++) {
		txt[j] = data[i];
		j++;
	}
	txt[j] = '\0';
	return txt;
}



/**
 * @brief TODO: doplnit komentar
 * @param packet ziskane informace o paketu
 * @param possible_protocol mozny protokol na overeni
 * @param data ukazatel na data
 * @return spravnost operace
 */

void isa_dns_skip_question(isa_Packet_header *packet, const u_char *data) {
	for (int i = isa_offset(packet); i < packet->len; i++) {
    	// hledame konec jmena otazky, najdeme konec stringu
    	// pripocteme 2x2B typ a tridu, a jeden pro offset
		if (data[i] == 0x0) {
			isa_set_offset(packet, i + sizeof(isa_dns_Query) + 1);
			break;
		}
	}
}

void isa_dns_answers(isa_Packet_header *packet, int ancount, const u_char *data) {
	// printf(YEL);
	// printf("\n>>>>>>>>>>>>>>> Responses <<<<<<<<<<<<<<<<");
	// printf(RESET);	

	for (int i = 0; i < ancount; i++) {
		// printf(GRN);
	 //    printf("\n--------------- Response\n\n");
	 //    printf(RESET);

	    //vytvorime struct do databaze
	    if (isa_init_item() == ISA_PRG_ERR) {
	    	return;
	    }
		// vytiskneme nazev
	    isa_add_name(isa_print_string(data, isa_offset(packet), packet->dns_start));
	    // prvni dva bajty (skok a misto skoku)
	    isa_inc_offset(packet, 2);
	    // nacteme obecnou strukturu, k ostatnim se jeste vratime
	   	isa_dns_Resp_generic *resp = (isa_dns_Resp_generic *) &data[isa_offset(packet)];
	   	isa_dns_resp_parse_ntohs(resp);
	   	// TODO: do riti, nepocitam ofset tohoto packetu
	   	isa_inc_offset(packet, sizeof(isa_dns_Resp_generic));
	   	//printf("Trida: %d\n", resp->type);
	    isa_add_type(resp->type);

	    //printf("%d\n", resp->type);

	    // zpracujeme typ odpovedi
	   	switch (resp->type) {
			case A:
				isa_add_payload(isa_print_ipv4(data, isa_offset(packet)));
				break;
			case AAAA:
				//isa_add_payload(isa_print_ipv4(data, isa_offset(packet)));
				isa_add_payload(isa_print_ipv6(data, isa_offset(packet)));
				break;
			case CNAME:
			case PTR:			
				isa_add_payload(isa_print_string(data, isa_offset(packet), packet->dns_start));
				break;
			case TXT:
			case SPF:
				isa_add_payload(isa_txt_resp(packet, data));
				break;
			case MX:
				isa_add_payload(isa_print_string(data, isa_offset(packet) + DNS_MX_OFFSET, packet->dns_start));
				break;
			case NS:
			case SOA:
				isa_add_payload(isa_print_string(data, isa_offset(packet), packet->dns_start));
				break;
			// TODO: DNSSEC
			default:
				isa_error_msg("Sorry, this packet can't be parsed");
		}

		// preskocime odpoved
	   	isa_inc_offset(packet, resp->rdlength);
	   	// pridame do DB
	    isa_add_item();
	}
}


/**
 * @brief Zpracuje IP vrstvu paketu
 * @param packet ziskane informace o paketu
 * @param possible_protocol mozny protokol na overeni
 * @param data ukazatel na data
 * @return spravnost operace
 */
int isa_protocol_header(isa_Packet_header *packet, bool ipv6, uint8_t possible_protocol, const u_char *data) {
	if (possible_protocol == UDP) {
		packet->protocol = UDP;
		// UDP ma fixni, 8B hlavicku
		isa_inc_offset(packet, UDP_HEADER);
	} 
	// else if (possible_protocol == TCP) {
	// 	packet->protocol = TCP;
	// 	// u TCP ještě musí zjistit, jak je dlouhá
	// 	isa_inc_offset(packet, (data[isa_offset(packet) + 12] >> 4) * 4);
	//  	// isa_read_data_packet(packet, data);
	//  	// printf("\n\n\n");
	//  	return ISA_PACKET_SKIP;
	// } 
	else {
		return ISA_PACKET_SKIP;
	}
	return ISA_PACKET_OK;
}

/**
 * @brief Zpracuje datagram vrstvu
 * @param packet ziskane informace o paketu
 * @param header hlavicka datagramu
 * @param data ukazatel na data
 * @return spravnost operace
 */
int isa_process_header(isa_Packet_header *packet, const struct pcap_pkthdr *header, const u_char *data) {
	// nasazeni hlavicky
    struct eth_header *hdr 	= (struct eth_header *) data;
    // nas obal pro packet
    packet->payload 	= 0;
    packet->dns_start 	= 0;
    packet->len 		= header->len;
    packet->ether 		= ntohs(hdr->ether_type);

    // posuneme citac o hlavicku
    isa_inc_offset(packet, ETHER_HEADER);
    // po zjisteni typu 
    if (packet->ether == ETYPEIP) {
    	packet->ip = IPV4;	
    	// typ protokolu
    	uint8_t possible_protocol = data[isa_offset(packet) + IPV4_HDR_PROTOCOL];
    	// zdrojova adresa
    	isa_change_host(isa_print_ipv4(data, isa_offset(packet) + IPV4_SRC_ADDR_OFFSET));
    	// delka hlavicky
    	uint8_t ihl = data[packet->payload] & IHL_MASK;
    	isa_inc_offset(packet, (ihl * HEADER_WORD) / 8);
    	// kontrola a posunuti dat
    	if (isa_protocol_header(packet, false, possible_protocol, data) == ISA_PACKET_SKIP) {
    		return ISA_PACKET_SKIP;
    	}
    } else if (packet->ether == ETYPEIP6) {
    	packet->ip = IPV6;
    	// zjistime delku, a potom uz jenom odecteme od konce
    	uint16_t data_len = (data[isa_offset(packet) + IPV6_DATA_LEN] << 8) | (data[packet->payload + 5] & 0xff);
    	uint16_t next_hdr = data[isa_offset(packet) + IPV6_NEXT_HDR];
    	// zdrojova adresa
    	isa_change_host(isa_print_ipv6(data, isa_offset(packet) + IPV6_SRC_ADDR_OFFSET));
    	// preskocime IPv6 hlavicku
    	isa_set_offset(packet, packet->len - data_len);
    	// kontrola a posunuti dat
    	if (isa_protocol_header(packet, true, next_hdr, data) == ISA_PACKET_SKIP) {
    		return ISA_PACKET_SKIP;
    	}
    } else {
    	return ISA_PACKET_SKIP;
    }
    return ISA_PACKET_OK;
}

/**
 * @brief Zpracuje DNS zaznam
 * @param packet ziskane informace o paketu
 * @param data ukazatel na data
 * @return spravnost operace
 */
int isa_process_payload(isa_Packet_header *packet, const u_char *data) {

    isa_dns_Header *header = (isa_dns_Header *) &data[packet->payload];
    // zacatek hlavicky
    packet->dns_start = isa_offset(packet);
    // posuneme o hlavicku DNS az ke queries
    isa_inc_offset(packet, ISA_DNS_HEADER_LEN);
    // logicky otocime poradi bajtu
    isa_dns_header_parse_ntohs(header);
    // zkontrolujeme, jestli vubec paket obsahuje to co ma
    if (isa_check_packet_valid(header) == ISA_PACKET_OK) {
    	//isa_debug_print_dns_header(header);
    	// preskocime otazku
    	isa_dns_skip_question(packet, data);
    	isa_dns_answers(packet, header->ancount, data);

		return ISA_PACKET_OK;
    } else {
    	return ISA_PACKET_SKIP;
    }
}

void isa_parse_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *data) {
    //vytvorime hlavickou paketu a naplnime ji
    isa_Packet_header *packet = (isa_Packet_header *) malloc(sizeof(isa_Packet_header));
    if (packet == NULL) {
    	return;
    }
    // zpracujeme packet 
    if (isa_process_header(packet, header, data) != ISA_PACKET_SKIP) {
    	if (isa_process_payload(packet, data) == ISA_PACKET_SKIP) {
    		isa_error_msg("Tento paket nelze parsovat");
    	}
    } else {
    	// TODO: na server
    	isa_error_msg("Tento paket nelze parsovat");
    }
    //
    free(packet);
}
