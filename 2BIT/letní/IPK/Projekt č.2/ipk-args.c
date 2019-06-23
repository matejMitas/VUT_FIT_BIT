/**
 * Projekt IPK-proj2
 * Cteni argumentu cmd
 * @brief Cteni argumentu cmd
 * @Author Matej Mitas, xmitas02
 * @file ipk-args.c
 */

#include "ipk-args.h"

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 */
void ipk18_usage_args() {
    fprintf(stderr, "\n\
./ipk-lookup -s server [-T timeout] [-t type] [-i] name\n\n\
    h (help) - volitelny parametr, pri jeho zadani se vypise napoveda a program se ukonci;\n\
    -s (server) - povinny parametr, DNS server (IPv4 adresa), na ktery se budou odesilat dotazy;\n\
    -T (timeout) - volitelny parametr, timeout (v sekundach) pro dotaz, vychozi hodnota 5 sekund;\n\
    -t (type) - volitelny parametr, typ dotazovaneho záznamu: A (vychozi), AAAA, NS, PTR, CNAME;\n\
    -i (iterative) - volitelny parametr, vynuceni iterativniho zpusobu rezoluce, viz dale\n\
    -name (jmeno) prekladane domenove jmeno, v pripade parametru -t PTR program na vstupu naopak ocekava IPv4 nebo IPv6 adresu.\
    ");
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */

ipk18_client_Args * ipk18_read_args(int argc, char *argv[]) {
    int option = 0;
    bool help_set = false;

    // zkontroluj, jestli vubec ma smysl volat getopt
    if (argc < 2 || argc > 9) {
        ipk18_error_msg("Nesedi pocet argumentu");
        return NULL;
    }

    // vytvoreni struktury pro argumenty
    ipk18_client_Args *args = (ipk18_client_Args *) malloc(sizeof(ipk18_client_Args));
    // prosty malloc
    if (args == NULL)
        return NULL;

    // nastav zacatek
    args->help 		= false;
    args->server 	= NULL;
    args->timeout 	= 5;
    args->iterative = false;
    args->name 		= NULL;
    args->type 		= A;

    long int cvt;
    char *err = NULL;

    while ((option = getopt(argc, argv,"his:T:t:")) != -1 && !help_set) {
        switch (option) {
        	case 'h':
     			args->help = true;
     			// vyskocime z cyklu, neni treba nic dalsiho kontrolovat
     			help_set = !help_set;
     			// primo vratime, dal nema cenu nic resit
     			if (argc != 2) {
     				ipk18_error_msg("prepinac -h nelze kombinovat s ostatnimi parametry");
     				free(args);
     				return NULL;
     			} else {
     				return args;
     			}
     			break;
     		case 's':
     			// vytvorime misto pro nazev serveru
     			args->server = (char *) malloc(sizeof(char) * strlen(optarg));
                //
                if (args->server == NULL) {
                    free(args);
                    return NULL;
                }
                strcpy(args->server, optarg);
     			break;
     		case 'T':
     			// prevest na int
     			cvt = strtol(optarg, &err, 10);
     			// pretypujeme na int
     			if (strlen(err) > 0) {
     				ipk18_error_msg(" -T nedostal spravnou volbu");
     				if (args->server != NULL)
     					free(args->server);
     				free(args);
     				return NULL;
     			} else {
     				args->timeout = cvt;
     			}
     			break;
     		case 't':
     			if (strcmp(optarg, "A") == 0) {
     				args->type = A;
     			}
     			else if (strcmp(optarg, "AAAA") == 0) {
     				args->type = AAAA;
     			}
     			else if (strcmp(optarg, "NS") == 0) {
     				args->type = NS;
     			}
     			else if (strcmp(optarg, "PTR") == 0) {
     				args->type = PTR;
     			}	
     			else if (strcmp(optarg, "CNAME") == 0) {
     				args->type = CNAME;
     			}
     			else {
        			ipk18_error_msg(" -t nedostal spravnou volbu");
     				if (args->server != NULL)
     					free(args->server);
     				free(args);
     				return NULL;
     			}
     			break;
     		case 'i':
     			args->iterative = true;
     			break;
     		default:
     			ipk18_error_msg("Neznamy prepinac");
     			if (args->server != NULL)
     				free(args->server);
     			free(args);
     			return NULL;
        }
    }
    // nacteme posledni jmeno
    bool set = false;
   	for (int i = optind; i < argc; i++) {
   		if (!set) {
   			// vytvorime misto pro nazev serveru
     		args->name = (char *) malloc(sizeof(char) * strlen(argv[i]));
     		strcpy(args->name, argv[i]);
     		set = !set;
   		} else {
   			ipk18_error_msg("Nezname textove parametry");
     		if (args->server != NULL)
     			free(args->server);
     		free(args);
     		return NULL;
   		}
   	}
   	// zkontrolujeme validitu kombinace argumentu
   	if (args->server == NULL) {
   		ipk18_error_msg("Prepinac -s je povinny");
   		if (args->name != NULL)
     		free(args->server);
   		free(args);
     	return NULL;
   	} else if (args->name == NULL) {
   		ipk18_error_msg("je treba zadat jmeno serveru");
   		if (args->server != NULL)
     		free(args->server);
     	free(args);
     	return NULL;
   	} else {
   		return args;
   	}
}
