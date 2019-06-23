// htab_foreach.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Funkce, která zavolá funkci na každou položkou v tabulce

#include "htab.h"

/**
 Apply function for each item in the field
 
 @param t table
 @param func function to be executed upon each item
 */

void htab_foreach(htab_t * t, void (*func)(struct htab_listitem *)) {
    
    // check if table and index are valid
    if (t == NULL) {
        fprintf(stderr, ">>Table is corrupted or non-existent");
        return;
    }

    // init individual item
    struct htab_listitem * item = NULL;
    
    // loop through all items in the field
    for (unsigned int i = 0; i < t->arr_size; i++) {
        // individual item
        item = t->ptr[i];
        // if item has proceeding items
        while (item != NULL) {
            func(item);
            item = item->next;
        }
    }
}

