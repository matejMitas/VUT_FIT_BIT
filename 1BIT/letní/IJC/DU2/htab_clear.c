// htab_clear.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Vyčištění tabulky

#include "htab.h"

/**
 Clear all items int the table, it remains empty
 
 @param t table
 */

void htab_clear(htab_t * t) {

    // check if table and index are valid
    if (t == NULL) {
        fprintf(stderr, ">>Table is corrupted or non-existent");
        return;
    }
    
    for (unsigned int i = 0; i < t->arr_size; i++) {
        // look for all populated items
        if (t->ptr[i] != NULL) {
            // clear all allocated items
        
            struct htab_listitem *current_item = t->ptr[i];
            struct htab_listitem *preceeding_item = NULL; // last item in the list
            
            while (current_item != NULL) {
                // retain last valid record
                preceeding_item = current_item;
                current_item = current_item->next;
                // free space
                free(preceeding_item->key);
                free(preceeding_item);
            }
            // set pointer to null afterwards
            t->ptr[i] = NULL;
        }
    }
    
    // set number of items to zero
    t->n = 0;
}
