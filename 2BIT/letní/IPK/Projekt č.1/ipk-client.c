/**
 * Projekt IPK-proj1
 * Implementace klienta
 * @brief Implementace klienta
 * @Author Matej Mitas, xmitas02
 * @file ipk-client.c
 */

#include "ipk-client.h"

/**
 * Enum pro nastaveni modu vraceneho obsahu
 */
enum ipk18_client_ret {
    empty,
    name,
    dir,
    list
};

/**
 * Struktura pro parametry
 */
typedef struct ipk18_client_Args {
    char *host; // client pro pripojeni
    int port; // port, se kterým chceme pracovat
    char *login; // prihlasovaci udaje
    // co vlastne mame vracet
    enum ipk18_client_ret ret_type; 
} ipk18_client_Args;

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 */
void ipk18_client_arg_usage() {
    fprintf(stderr, "\n\
./ipk-client -h host -p port [-n|-f|-l] login\n\n\
    host (IP adresa nebo fully-qualified DNS name) identifikace clientu jakožto koncového bodu komunikace klienta;\n\
    port (číslo) cílové číslo portu;\n\
    -n značí, že bude vráceno plné jméno uživatele včetně případných dalších informací pro uvedený login (User ID Info);\n\
    -f značí, že bude vrácena informace o domácím adresáři uživatele pro uvedený login (Home directory);\n\
    -l značí, že bude vrácen seznam všech uživatelů, tento bude vypsán tak, že každé uživatelské jméno bude na zvláštním\n    řádku; v tomto případě je login nepovinný. Je-li však uveden bude použit jako prefix pro výběr uživatelů.\n\
    login určuje přihlašovací jméno uživatele pro výše uvedené operace.");
}

/**
 * @brief Preved retezec na cislo
 * @param Retezec
 * @return Cislo
 */

int ipk18_client_get_num(char *str) {
    // pomocne promenne
    char *str_remainder;
    long number;
    // prevedeme na cislo
    number = strtol(str, &str_remainder, 10);
    // chceme opravdu jenom cislo, a vetsi nez 0
    if (strcmp(str_remainder, "") != 0 || number < 1) {
        ipk18_error_msg("Spatne zadany ciselny argument pro port");
    }
    // oseknuti rozsahu
    number = (int)number;
    return number;
}

/**
 * @brief Precti parametry a zjisti, jestli odpovidaji vzoru
 * @param Pocet argumentu
 * @param Pole s argumenty
 * @return Struktura s potrebnym datovym obsahem
 */
ipk18_client_Args * ipk18_client_param_read(int argc, char *argv[]) {
    int option = 0;
    bool opt_set = false;

    // zkontroluj, jestli vubec ma smysl volat getopt
    if (argc < 6 || argc > 7) {
        ipk18_error_msg("Nesedi pocet argumentu");
        return NULL;
    }

    // vytvoreni struktury pro argumenty
    ipk18_client_Args *params = (ipk18_client_Args *) malloc(sizeof(ipk18_client_Args));
    // prosty malloc
    if (params == NULL)
        return NULL;
    // nastav 
    params->port = -1;
    params->host = NULL;
    params->login = NULL;
    params->ret_type = empty;

    // podivej se na vsechy parametry
    while ((option = getopt(argc, argv,"h:p:n:f:l")) != -1 && !opt_set) {
        switch (option) {
            case 'h' : 
                params->host = (char *) malloc(sizeof(char) * strlen(optarg));
                //
                if (params->host == NULL) {
                    free(params);
                    return NULL;
                }

                strcpy(params->host, optarg);
                break;
            case 'p' : 
                params->port = ipk18_client_get_num(optarg);
                // musime mit port vetsi nebo rovno 0
                if (params->port < 1 || params->port > UINT16_MAX) {
                    ipk18_error_msg(">Port nema ocekavany format");
                    free(params->host);
                    free(params);
                    return NULL;
                }
                break;
            // volitelny argument
            case 'n':
            case 'f':
            case 'l':
                // zastav vykonavani cyklu
                opt_set = !opt_set;
                // pro lepsi porovnavani
                char flag = (char)option;

                switch (flag) {
                    case 'n':
                        params->ret_type = name;
                        break;
                    case 'f':
                        params->ret_type = dir;
                        break;
                    case 'l': 
                        params->ret_type = list;
                        // zde je parametr optional, neni treba ho mit
                        if (argv[6] != NULL) {
                            optarg = argv[6];
                        }
                        break;
                }
                // pokud mame login (mozny u vsech parametru)
                if (optarg != NULL) {
                    // nepodporujeme login, ktery je delsi, nez je udano
                    // ve standartu zapisu /etc/passwd
                    if (strlen(optarg) <= LOGIN_LEN) {
                        // uloz login
                        params->login = (char *) malloc(sizeof(char) * strlen(optarg));
                        // zde jiz musime uvolnovat povicero parametru
                        if (params->login == NULL) {
                            free(params->host);
                            free(params);
                            return NULL;
                        }
                        // jednoducha kopie
                        strcpy(params->login, optarg);
                    } else {
                        free(params->host);
                        free(params);
                        return NULL;
                    }
                }

                break;
            // kdyz dostaneme neco, co nechceme
            default:
                ipk18_client_arg_usage(); 
                return NULL;
        }
    }
    // zde resime, co se stane, kdyz switch dopadne, ale dostaneme
    // nejakou neocekavanou hodnotu
    if (params->ret_type == empty || params->port == -1 || params->host  == NULL) {
        ipk18_client_arg_usage(); 
        return NULL;
    }

    return params;
}


char * ipk18_server_ip(char *host_in) {
    struct in_addr *addr;
    struct hostent *host;
         
    if ((host = gethostbyname(host_in)) == NULL) {
        return NULL;
    }

    addr = (struct in_addr *) host->h_addr_list[0];
    return inet_ntoa(addr[0]);
}



int main(int argc, char *argv[]) {

    // cteni parametru prikazove radky
    ipk18_client_Args *p = ipk18_client_param_read(argc, argv);
    if (p == NULL) {
        return 1;
    }
    
    // nachystame si ip
    // zde je jasna jeji delka, nemuzeme to presahnout
    char ip[IP_LEN];
    char *got_ip = ipk18_server_ip(p->host);

    if (got_ip == NULL) {
        return ipk18_error_msg("Nenasli jsme server, opravdu existuje?");
    } else {
        strcpy(ip, got_ip);
    }
    
    // adresy pro komunikaci
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    // uzivatelsky socket
    int client_sock;


    // vytvorime uzivatelsky socket
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        return ipk18_error_msg("Nepodarilo se vytvorit socket");
    }
    // vytvorime adresu uzivatele
    client_addr.sin_family=PF_INET;
    client_addr.sin_port=htons(0);
    client_addr.sin_addr.s_addr=INADDR_ANY;

    // nabindujeme klientskou adresu na socket
    if (bind(client_sock, (struct sockaddr*) &client_addr, sizeof(client_addr)) == -1) { 
        ipk18_error_msg("Nepodarilo se nabindovat klientsky socket"); 
        // poresit zavirani socketu
        close(client_sock); 
        return 1;
    }

    // vytvorime adresu serveru, kam se chceme pripojit
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(p->port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // pripoj se na server
    if (connect(client_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) { 
        ipk18_error_msg("Tak server existuje, ale opravdu to tam bezi?"); 
        // poresit zavirani socketu
        close(client_sock); 
        return 1;
    }

    // vytvorime si buffer
    char buffer[BUFFER_LEN];
    // precteme to, co nam prislo ze serveru
    int bytesRead;

    // nahodime do nej request 
    // volame bez loginu pro prefix
    if (p->login == NULL && p->ret_type == list)
        ipk18_protocol_encode(buffer, p->ret_type, 1, "");
    else
        ipk18_protocol_encode(buffer, p->ret_type, strlen(p->login) + 1, p->login);
    
    // tady je to posilani
    write(client_sock, buffer, BUFFER_LEN);
    
    // prijimame data tak dlouho, dokud jsou nejake zpravy
    while (1) {
        bytesRead = read(client_sock, buffer, BUFFER_LEN);
        // cteme dokud neco mame
        if (bytesRead == 0) 
            break;
        // tady muze byt problem

        char *ret = ipk18_protocol_decode(buffer);

        if (ret) {
            printf("%s\n", ret);
        } else {
            ipk18_error_msg("Ze serveru neprisla zadna data");
        }
    }

    return 0;
}
