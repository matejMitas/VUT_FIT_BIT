/**
 * Projekt IPK-proj2
 * Prevod adres na spravny tvary pro DNS
 * @brief Prevod adres na spravny tvary pro DNS
 * @Author Matej Mitas, xmitas02
 * @file ipk-name.c
 */

#include "ipk-name.h"

// prevedeme adresu adresu do formatu pro query
char * ipk18_dns_Name_bytes(char *name) {
	// 
	int old_len = strlen(name)+1;
	int new_len = old_len+1;

	// alokace noveho stringu
	// + 1, jelikoz si musme pomatovat prvni tecku a +1 protoze
	// koncova nula
	char *bytes_name = (char *) calloc(new_len, sizeof(char));
	if (bytes_name == NULL)
		return NULL;

	bytes_name[0] = ' ';

	unsigned chars = 0;
	unsigned offset = 1;

	for (int i = 0; i < old_len; i++) {
		char c = name[i];
		// ta trojka je hotfix pro fit.vutbr.cz
		if (c == '.' || c == '\0' || c == '\3') {
			bytes_name[i-chars] = chars;
			chars = 0;
		} else {
			bytes_name[i+offset] = name[i];
			chars++;
		}
	}

	return bytes_name;
}

// prevedeme adresu zpet do citelneho formatu
char * ipk18_dns_Name_ptr(char *name) {

	// docasne adresy pro overeni platnosti
	// zde ulozi pouze docasne prime cisla z adresy
    char ipv4_buffer[DNS_IPV4_SIZE];
    char ipv6_buffer[DNS_IPV6_SIZE];

	// podivame se jestli je string validni a urcime typ adresy
	if (inet_pton(AF_INET, name, &ipv4_buffer)) {
		// zde je potreba oseknout na spravnou delku
		// pouze 1B
		uint8_t p0 = ipv4_buffer[0];
		uint8_t p1 = ipv4_buffer[1];
		uint8_t p2 = ipv4_buffer[2];
		uint8_t p3 = ipv4_buffer[3];

		// hotove ip adresy, 16B + 13B pro .in_addr.arpa + 1B
		char *temp_ipv4 = (char *) malloc(sizeof(char) * DNS_BINARY_IPV4);
		if (temp_ipv4 == NULL)
			return NULL;

		// vytiskeneme do stringu
		snprintf(temp_ipv4, DNS_BINARY_IPV4, "%d.%d.%d.%d.in-addr.arpa", p3, p2, p1, p0);
		// prevedeme na bity
		char *ipv4 = ipk18_dns_Name_bytes(temp_ipv4);
		free(temp_ipv4);

		return ipv4;

	} else if (inet_pton(AF_INET6, name, &ipv6_buffer)) {
		// hotove ip adresy, 32B + 32B (tecky) + 8B pro ip6.arpa + 1B
		char *temp_ipv6 = (char *) malloc(sizeof(char) * DNS_BINARY_IPV6);
		if (temp_ipv6 == NULL)
			return NULL;
		// buffer s jiz otocenymi pulbajty (4b)
		char ipv6_values[32] = {0, };

		// prectu hodnoty, rozdelim na pulbajty a ulozim naopak
		for (int i = 0; i < 32; i = i+2) {
			ipv6_values[31 - i] = (uint8_t) ipv6_buffer[i/2] >> 4;
			ipv6_values[31 - (i+1)] = (uint8_t) ipv6_buffer[i/2] & 0xf;
		}

		// potrebujeme nejak namapovat hodnoty z binar do ASCII
		char ascii_buffer[2] = {0, };
		// ulozim obracene hodnoty do spravneho bufferu
		for (int i = 0; i < 64; i = i+2) {
			// vytiskeneme do stringu
			snprintf(ascii_buffer, 2, "%x", ipv6_values[i/2]);
			// ulozime
			temp_ipv6[i+1] = ascii_buffer[0];
			temp_ipv6[i] = 1;
		}

		// pridej koncovku
		temp_ipv6[DNS_BINARY_IPV6-10] = 3;
		temp_ipv6[DNS_BINARY_IPV6-9] = 'i';
		temp_ipv6[DNS_BINARY_IPV6-8] = 'p';
		temp_ipv6[DNS_BINARY_IPV6-7] = '6';

		temp_ipv6[DNS_BINARY_IPV6-6] = 4;
		temp_ipv6[DNS_BINARY_IPV6-5] = 'a';
		temp_ipv6[DNS_BINARY_IPV6-4] = 'r';
		temp_ipv6[DNS_BINARY_IPV6-3] = 'p';
		temp_ipv6[DNS_BINARY_IPV6-2] = 'a';

		// pridej koncovny znak
		temp_ipv6[DNS_BINARY_IPV6-1] = '\0';


		// neni jiz treba prevadet prenos, byl proveden automaticky
		return temp_ipv6;

	} else {
		ipk18_error_msg("U parametru -t PTR nebyla dodane validni IP adresa");
		return NULL;
	}	
}
