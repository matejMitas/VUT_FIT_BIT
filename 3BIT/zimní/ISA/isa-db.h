/**
 * Projekt ISA
 * Hlavickovy soubor pro ukladani DNS zaznamu
 * @brief Hlavickovy soubor pro ukladani DNS zaznamu
 * @Author Matej Mitas, xmitas02
 * @file isa-db.h
 */

#include <string.h>
#include <time.h>
#include <sys/time.h>

// sitarina
#include <sys/socket.h>  
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <sys/types.h>

#include "isa-structs.h"
#include "isa-defines.h"
#include "isa-error.h"

int isa_init_item();
void isa_print_db();
void isa_cleanup_db();
void isa_add_type(enum dns_data_types type);
void isa_add_source(char *source);
void isa_add_name(char *name);
void isa_add_payload(char *payload);
void isa_add_item();
void isa_print_item(isa_db_Item *item, char *server);
void isa_change_host(char *received_host);
