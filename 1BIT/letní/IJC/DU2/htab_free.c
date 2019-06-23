// htab_free.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Destruktor tabulky

#include "htab.h"

/**
 Descructor of the table
 
 @param t table
 */

void htab_free(htab_t * t) {
    
    // check if table and index are valid
    if (t == NULL) {
        fprintf(stderr, ">>Table is corrupted or non-existent");
        return;
    }

    htab_clear(t);
    free(t);
}
