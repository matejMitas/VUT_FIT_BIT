/**
 * Projekt IPK-proj1
 * Implementace pro serverovou cast
 * @brief Implementace pro serverovou cast
 * @Author Matej Mitas, xmitas02
 * @file ipk-server.c
 */

#include "ipk-server.h"

/**
 * @brief Precti vstupni parametry
 * @param Pocet parametru
 * @param Pole s argumety
 * @return Cislo portu
 */
int ipk18_server_param_read(int argc, char *argv[]) {
	// nesedi pocet argumentu
	if (argc != 3) {
		return ipk18_error_msg("Nesedi pocet argumentu");
	}
	// -p
	if (strcmp(argv[1], "-p") != 0) {
		return ipk18_error_msg("Prvni argument musi byt -p");
	}
	// reseni portu
	char *str_remainder;
   	long port;

   	port = strtol(argv[2], &str_remainder, 10);
   	
   	if (strcmp(str_remainder, "") != 0 || port < 1) {
   		return ipk18_error_msg("Spatne zadany ciselny argument pro port");
   	}
   	// vratime ji
	return (int)port;
}

void sig_handler(int so) {
	if (so == SIGINT) {
    	end = 1;
    	exit(1);
  	}
}

int main(int argc, char *argv[]) {

	// reseni SIGINT
	if (signal(SIGINT, sig_handler) == SIG_ERR)
  		return ipk18_error_msg("Nemuzeme odchytavat SIGNIT");

	//incializujeme port
	int server_port = ipk18_server_param_read(argc, argv);
	// pokud nemame port, co se muze stat ve vice 
	if (server_port == -1)
		return ipk18_error_msg(">Neexistujici cislo portu");


	// adresy pro komunikaci
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    // uzivatelsky socket
    int server_sock;
    int acpt_sock;

    // vytvor serverovy socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        return ipk18_error_msg("Nepodarilo se vytvorit socket");
    }

    // vytvorime adresu serveru
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // nabindujeme serverovou adresu na socket
    if (bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) { 
    	close(server_sock); 
        return ipk18_error_msg("Nepodarilo se nabindovat serverovy socket"); 
    }

    // povolime naslouchani k requestum
    if (listen(server_sock, 1)) { 
        close(server_sock);
    	return ipk18_error_msg("Nepodarilo se zapnout poslech");  
   	}

   	// precteme to, co nam prislo ze serveru
    int bytesRead;
    char buffer[BUFFER_LEN];
    // urcovani typu zpravy
    int parse_flag;
    char flag;
    // C proste neumi brat adresu R-hodnoty, musime tu mit promennou
	unsigned client_addr_size = sizeof(client_addr);
	// pro chybu (vystup)
	char *error = "Uzivatel nebyl nalezen";


	// dokud neskoncime program
  	while (!end)  { 	
	   	// povolime akceptovani socketu
		if ((acpt_sock = accept(server_sock, (struct sockaddr*) &client_addr, &client_addr_size)) == -1) { 
		    return ipk18_error_msg("Nepodarilo se nabindovat klientsky socket"); 
		}

	  	// cteme data od klienta
	  	bytesRead=read(acpt_sock, buffer, BUFFER_LEN);	

	  	// musime neco precist, jinak neni co resit
	  	if (bytesRead > 0) {
	  		char *ret = ipk18_protocol_decode(buffer);
	  		// kdyz dostaneme zpravu
		  	if (ret) {
		  		// zvoleni hodnoty
		  		parse_flag = buffer[21];
	    		// urcime typ requestu
	    		switch (parse_flag) {
	    			case 1:
	    				flag = 'n';
	  					break;
	    			case 2:
	    				flag = 'f';
	    				break;
	    			case 3:
	    				flag = 'l';
	    				break;
	    		}
	    		// rozparsujeme, posilame primo z parseru
	    		bool data_ok = ipk18_server_parse(acpt_sock, flag, ret);
	    		// zde piseme 
	    		if (!data_ok) {
	    			ipk18_protocol_encode(buffer, 6, strlen(error) + 1, error);
	    			// zapiseme
	    			write(acpt_sock, buffer, BUFFER_LEN);
	    		}
		  	} else {
		  		// vytvorime chybovou zpravu
	    		char *error = "Neocekavany format zpravy";
	    		ipk18_protocol_encode(buffer, 6, strlen(error) + 1, error);
	    		// zapis
	    		write(acpt_sock, buffer, BUFFER_LEN);
		  	}
		  	// uvolnime
	  	} else {
	    	char *error = "Klient musi poslat data";
	    	ipk18_protocol_encode(buffer, 6, strlen(error) + 1, error);
	    	// zde neni nic precteno, ukoncime spojeni
	  		break;
	  	}
		// ukoncime spojeni s klientem
		close(acpt_sock);
  	}
	// zavreme socket, na kterem jsme poslouchali zadosti o spojeni
	close(server_sock);
	return 0;
}
