// htab_lookup_add.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Přidání nebo nalezení položky v tabulce

#include "htab.h"
#include "hash_function.h"

/**
 Number of items in the table to allocated (size as well)
 
 @param t table
 @return n number of all items
 */

struct htab_listitem * htab_lookup_add(htab_t * t, char *key) {
	// just for adding items
    
    // malloc space for new item
    struct htab_listitem *new_item = malloc(sizeof(*new_item));
    // treat errors
    if (new_item == NULL) {
        fprintf(stderr, ">>Error occured while allocating space for item");
        return NULL;
    }
    
    // add data to the file
    new_item->data = 1;
    
    // allocated place for string
    new_item->key = malloc((strlen(key) + 1) * sizeof(char));
    // treat errors
    if (new_item->key == NULL) {
        // delete allocated item
        free(new_item);
        fprintf(stderr, ">>Error occured while allocating space for item key");
        return NULL;
    }
    
    // copy it to the item
    strcpy(new_item->key, key);
    
    new_item->next = NULL;
    
    // get index of items
    int index = hash_function(key) % t->arr_size;
    
    //printf(">>Klic: %s, Index: %d\n", key, index);
    
    // neccessary variables
    struct htab_listitem *current_item = t->ptr[index];
    struct htab_listitem *preceeding_item = NULL; // last item in the list
    

    // check if the index is already occupied
    if (current_item != NULL) {
        
        //printf(">>Polozka existuje\n");

        // loop as long as last item is valid
        while (current_item != NULL) {
            
            if (strcmp(key, current_item->key) == 0) {
                // we found match of indexes
                // throw out newly created item
                free(new_item->key);
                free(new_item);
                // increment the counter of occurence
                current_item->data++;
                // return new item
                return current_item;
            }
            
            // retain last valid record
            preceeding_item = current_item;
            // move the current pointer to the first unoccupied item
            current_item = current_item->next;
        }
        
        // asign item to the last used record
        preceeding_item->next = new_item;
        
    } else {
        // asign item to the table
        t->ptr[index] = new_item;
    }
    
    // increase number of items in the list
    t->n++;
    // just temporary for addition
    return new_item;
}
