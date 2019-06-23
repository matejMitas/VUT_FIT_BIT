// htab_remove.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Odstranění položky

#include "htab.h"
#include "hash_function.h"

/**
 Remove individual item from the table
 
 @param t table
 @param key string to find
 @return bool true if the items was cleared, otherwise false
 */

bool htab_remove(htab_t * t, char *key) {

    // check if table and index are valid
    if (t == NULL) {
        fprintf(stderr, ">>Table is corrupted or non-existent");
        return false;
    } else if (key == NULL) {
        fprintf(stderr, ">>Non valid key");
        return false;
    }
    
    // get index of items
    int index = hash_function(key) % t->arr_size;
    
    // neccessary variables
    struct htab_listitem *current_item = t->ptr[index];
    struct htab_listitem *preceeding_item = NULL; // last item in the list
    
    if (current_item != NULL) {
        
        while (current_item != NULL) {
        
            if (strcmp(key, current_item->key) == 0) {
                
                // if item is not first in the row
                if (preceeding_item != NULL) {
                    preceeding_item->next = current_item->next;
                } else {
                    t->ptr[index] = current_item->next;
                }
                
                // throw out newly created item
                free(current_item->key);
                free(current_item);
                
                // return new item
                return true;
            }
        
            // retain last valid record
            preceeding_item = current_item;
            // move the current pointer to the first unoccupied item
            current_item = current_item->next;
            
        }
        
        
        printf(">>Item to remove doesn't exist");
        return false;
        
    } else {
        printf(">>Item to remove doesn't exist");
        return false;
    }
}
