/**
 * Projekt ISA
 * Cteni argumentu cmd
 * @brief Cteni argumentu cmd
 * @Author Matej Mitas, xmitas02
 * @file isa-args.c
 */

#include "isa-args.h"

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 */
void isa_args_help() {
    fprintf(stderr, "\
./dns-export [-r file.pcap] [-i interface] [-s syslog-server] [-t seconds]\n\
-r : zpracuje dany pcap soubor\n\
-i : naslouchá na danem sitovem rozhrani a zpracovava DNS provoz\n\
-s : hostname/ipv4/ipv6 adresa syslog serveru\n\
-t : doba vypoctu statistik, vychozi hodnota 60s\n\
");
}

void isa_args_purge(isa_cmd_Args *args) {
	if (args->parse != NULL)
     	free(args->parse);
    if (args->hostname != NULL)
     	free(args->hostname);
    if (args->interface != NULL)
     	free(args->interface);
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */
isa_cmd_Args * isa_args_parse(int argc, char *argv[]) {
    int option = 0;
    // zkontroluj, jestli vubec ma smysl volat getopt
    if (argc < 2 || argc > 9) {
        isa_error_msg("Nesedi pocet argumentu");
        isa_args_help();
        return NULL;
    }

    // vytvoreni struktury pro argumenty
    isa_cmd_Args *args = (isa_cmd_Args *) malloc(sizeof(isa_cmd_Args));
    // prosty malloc
    if (args == NULL)
        return NULL;

    // nastaveni prepinacu
    args->parse 				= NULL;
    args->interface 			= NULL;
    args->hostname 				= NULL;
    args->timeout 				= 60;
    
    // pro prevod timeoutu
    long int cvt;
    char *err = NULL;
    // parsovani argumentu
    while ((option = getopt(argc, argv,"r:i:s:t:")) != -1) {
        switch (option) {
        	case 'r':
        		// vytvorime misto pro nazev serveru
     			args->parse = (char *) malloc(sizeof(char) * strlen(optarg) + 1);
                //
                if (args->parse == NULL) {
                    free(args);
                    return NULL;
                }
                strcpy(args->parse, optarg);
        		break;
        	case 'i':
        	    args->interface = (char *) malloc(sizeof(char) * strlen(optarg) + 1);
                //
                if (args->interface == NULL) {
                	free(args->parse);
                    free(args);
                    return NULL;
                }
                strcpy(args->interface, optarg);
        		break;
        	case 's':
        	    args->hostname = (char *) malloc(sizeof(char) * strlen(optarg) + 1);
                //
                if (args->hostname == NULL) {
                	free(args->interface);
                	free(args->parse);
                    free(args);
                    return NULL;
                }
                strcpy(args->hostname, optarg);
        		break;
        	case 't':
        		// prevest na int
     			cvt = strtol(optarg, &err, 10);
     			// pretypujeme na int
     			if (strlen(err) > 0 || cvt <= 0) {
     				isa_error_msg(" -t prijima pouze cela cisla od nuly vyse");
					isa_args_purge(args);
     				free(args);
     				return NULL;
     			} else {
     				args->timeout = cvt;
     			}
        		break;
     		default:
     			isa_error_msg("Neznamy prepinac");
     			isa_args_purge(args);
     			free(args);
     			return NULL;
        }
    }
    // moznosti kombinace flagu
    if (args->parse != NULL && args->hostname == NULL) {
        isa_error_msg("Nelze parsovat bez serveru");
        return NULL;
    } else if (args->parse != NULL && args->hostname != NULL && argc != 5) {
        isa_error_msg("Prilis mnoho argumentu");
        return NULL;
    } else if (args->parse == NULL && args->interface != NULL && args->hostname == NULL) {
        isa_error_msg("Pri specifikaci rozhrani je nutno zadat i jmeno syslog serveru");
        return NULL;
    } else if (args->parse == NULL && args->interface == NULL && args->hostname != NULL) {
        isa_error_msg("Pri specifikaci syslog serveru je nutno zadat i rozhrani");
        return NULL;
    } else {
        // nastavime mod programu
        if (args->parse != NULL)
            args->mode = PARSE;
        else if (args->interface != NULL)
            args->mode = SURVEIL;
    	return args;
    }
}

