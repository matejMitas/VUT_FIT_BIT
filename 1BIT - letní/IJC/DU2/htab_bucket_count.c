// htab_bucket_count.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Počet záznamů v tabulce

#include "htab.h"

/**
 Number of items in the table to allocated (size as well)
 
 @param t table
 @return n number of all items
 */

size_t htab_bucket_count(htab_t * t) {
    
    if (t == NULL) {
        fprintf(stderr, ">>Table is corrupted or non-existent");
        return -1;
    }
    
    return t->n;
}
