// htab_move.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Přesun tabulky do nové tabulky

#include "htab.h"

/**
 Moves the table
 
 @param newsize new size of the table
 @param t2 source table to get data from
 @return movedTable pointer to the created table
 */

htab_t * htab_move(unsigned int newsize, htab_t * t2) {

    // check if table and index are valid
    if (t2 == NULL) {
        fprintf(stderr, ">>Table used as a source not corrupted or non-existent");
        return NULL;
    }

    // init the table
    htab_t * movedTable = htab_init(newsize);

    if (movedTable == NULL) {
        fprintf(stderr, ">>Couldn't allocated space for new table");
        return NULL;
    }
    
    // init individual item
    struct htab_listitem * item = NULL;
    // loop through all items in the field
    for (unsigned int i = 0; i < t2->arr_size; i++) {
        // individual item
        item = t2->ptr[i];
        // if item has proceeding items
        while (item != NULL) {
            htab_lookup_add(movedTable, item->key);
            item = item->next;
        }
    }
    
    // clear the old table
    htab_clear(t2);
    
    // return the table
    return movedTable;
}

