// htab_init.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Velikost pole ukazatelů na prvky

#include "htab.h"

/**
 Read the count of stored items
 
 @param t table
 @return arr_size size of the items in the table
 */

size_t htab_size(htab_t * t) {
	// check if table is valid
	if (t == NULL) {
        fprintf(stderr, ">>Table is corrupted or non-existent");
    }
    
    return t->arr_size;
}
