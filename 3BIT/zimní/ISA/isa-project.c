/**
 * Projekt ISA
 * Hlavni program
 * @brief Hlavni program
 * @Author Matej Mitas, xmitas02
 * @file isa_project.c
 */

#include "isa-project.h"

// global z duvodu pristupu signalu
char *hostname;

/**
 * @brief Prelozi jmeno serveru
 * @param host jmeno serveru
 * @return prelozene jmeno souboru
 */

char * isa_find_hostname(char *host) {
	struct hostent *ret = gethostbyname(host);

    if (ret == NULL) {
       	isa_error_msg("Nepodarilo se najit syslog server");
    	return NULL;
    } 

    char *str = inet_ntoa(*(struct in_addr*)(ret->h_addr_list[0]));
    return str;
}

/**
 * @brief Nacteme nazev rozhrani z prikazove radky a zkontrolujeme
 * @param args Argumenty prikazove radky
 * @return Stav programu
 */

int isa_check_interface(isa_cmd_Args *args) {
	char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_if_t *dev_list;
    /* get the devices list */
	if (pcap_findalldevs(&dev_list, error_buffer) == -1) {
	    isa_error_msg("Nepovedlo se najit vsechny rozhrani");
	    return ISA_PRG_ERR;
	}

	pcap_if_t *dev;
	//
	for (dev = dev_list; dev != NULL; dev = dev->next) {
	    pcap_addr_t *dev_addr; 
	    //
	    for (dev_addr = dev->addresses; dev_addr != NULL; dev_addr = dev_addr->next) {
	        if (dev_addr->addr->sa_family == AF_INET && dev_addr->addr && dev_addr->netmask) {
	            if (strcmp(dev->name, args->interface) == 0) {
	            	return ISA_PRG_OK;
	            }
	        }
	    }
	}
	return ISA_PRG_ERR;
}


/**
 * @brief Hlavni smycka pro cteni paketu
 * @param args Argumenty prikazove radky
 * @param live_capture prepinac pro mod cteni
 * @return Stav programu
 */

int isa_get_packets(isa_cmd_Args *args, bool live_capture) {
	char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    // otevreme soubory nebo rozhrani
	if (live_capture) {
		// zjistit jestli zarizeni existuje
		if (!isa_check_interface(args)) {
			handle = pcap_open_live(args->interface, 1024, 0, args->timeout * 1000, error_buffer);
		} else {
			isa_error_msg("Nepovedlo se najit zvolene rozhrani");
	    	return ISA_PRG_ERR;
		}
		
	} else {
		handle = pcap_open_offline(args->parse, error_buffer);
		if (handle == NULL) {
			isa_error_msg("Nepodarilo se otevrit soubor .pcap");
			return ISA_PRG_ERR;
		}
	}

	// filtry
	struct bpf_program filter;
    char *filter_exp = ISA_FILTER;

	if (pcap_compile(handle, &filter, filter_exp, 0, 0) == -1) {
        isa_error_msg("Spatny filtr");
        return ISA_PRG_ERR;
    }
    if (pcap_setfilter(handle, &filter) == -1) {
        isa_error_msg("Nepodarilo se nastavit filtr");
        return ISA_PRG_ERR;
    }

    // posilani paketu
    while (ISA_READ_PACKETS) {
    	if (pcap_dispatch(handle, 0, isa_parse_packet, NULL)) {
    		isa_print_db(hostname);
    		// printf(">Posli na syslog\n");
    	} else {
    		if (!live_capture) {
    			//printf(">Posli na syslog\n");
    			isa_print_db(hostname);
    			break;
    		}
    	}
    }
    // zavreme
    pcap_close(handle);
    return ISA_PRG_OK;
}

void isa_signal_handler(int signum) {
    if (signum == SIGUSR1) {
       	isa_print_db(hostname);
        signal(SIGUSR1, isa_signal_handler);
    }
}

/**
 * @brief Pomocna funkce pro urceni modu programu
 * @param args Argumenty prikazove radky
 * @return Mod
 */

bool isa_sniffing(isa_cmd_Args *args) {
	return args->parse ? false : true;
}

int main(int argc, char *argv[]) {
    isa_cmd_Args *args = isa_args_parse(argc, argv);
    if (args == NULL)
        return ISA_ARG_ERR;

    char *source_hostname = args->hostname;
    // precteme si hostname ze zadaneho vstupu
    hostname = isa_find_hostname(source_hostname);
    if (args->hostname == NULL)
    	return ISA_ARG_ERR;
    // vycistime
    free(source_hostname);
    //
    return isa_get_packets(args, isa_sniffing(args));
}
