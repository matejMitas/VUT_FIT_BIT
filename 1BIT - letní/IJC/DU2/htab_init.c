// htab_init.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Vytvoření tabulky a alokace místa

#include "htab.h"

/**
 Initalises the table
 
 @param size size of the table
 @return new_table pointer to the created table
 */

htab_t * htab_init(unsigned int size) {
    // init the table
    htab_t * new_table = malloc(sizeof(htab_t) + (size * sizeof(struct htab_listitem *)));
    // handle error state
    if (new_table == NULL) {
        fprintf(stderr, ">>Error occured while allocating space for table");
        return NULL;
    }
    
    // clear all items in the array
    for (unsigned int i = 0; i < size; i++) {
        new_table->ptr[i] = NULL;
    }
    
    // aditional data
    new_table->arr_size = size;
    new_table->n = 0; // number of store items
      
    // return the table
    return new_table;
}
