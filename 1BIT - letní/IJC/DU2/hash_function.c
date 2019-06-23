// hash_function.c
// Řešení IJC-DU2, příklad 2, 21.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Hashovací funkce pro tabulku

#include "hash_function.h"

/**
 Hash function
 
 @param str string be to indexed with
 @return index to be particular item in the table
 */

unsigned int hash_function(const char *str) {
    unsigned int h = 0;
    const unsigned char *p;
    for(p = (const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}
