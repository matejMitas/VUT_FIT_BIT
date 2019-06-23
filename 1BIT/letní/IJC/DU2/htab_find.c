// htab_find.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Nalezení indexu v tabulce

#include "htab.h"
#include "hash_function.h"

/**
 Find individual item from the table
 
 @param t table
 @param key string to find
 @return foundItem found item from the function
 */

struct htab_listitem * htab_find(htab_t * t, char *key) {

    // check if table and index are valid
    if (t == NULL) {
        fprintf(stderr, ">>Table is corrupted or non-existent");
        return NULL;
    } else if (key == NULL) {
        fprintf(stderr, ">>Non valid key");
        return NULL;
    }
    
    // get index of items
    int index = hash_function(key) % t->arr_size;
    
    // neccessary variables
    struct htab_listitem *current_item = t->ptr[index];
    
    // item exists
    if (current_item != NULL) {
        
        while (current_item != NULL) {
            
            if (strcmp(key, current_item->key) == 0) {
                return current_item;
            }
            
            // move onto next item
            current_item = current_item->next;
        }
        
        printf(">>Item doesn't exist\n");
        return NULL;

    }
    // item doesn't exist
    else {
        
        printf(">>Item doesn't exist\n");
        return NULL;
    }
}

