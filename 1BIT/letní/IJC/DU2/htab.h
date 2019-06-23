// htab.h
// Řešení IJC-DU2, příklad 2, 21.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Implementace knihovny htab s hashovací
// tabulkou

#ifndef htab_h
#define htab_h


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//
// List item
//

struct htab_listitem {
    char *key; // dynamically allocated space for string literal
    unsigned int data; // number of occurences
    struct htab_listitem *next; // pointer to the next item
};

//
// Htab itself
//

typedef struct htab_t {
    unsigned int arr_size;
    unsigned int n;
    struct htab_listitem * ptr[];
} htab_t;

/**
 Initalises the table
 
 @param size size of the table
 @return new_table pointer to the created table
 */

htab_t * htab_init(unsigned int size);

/**
 Read the count of stored items
 
 @param t table
 @return arr_size size of the items in the table
 */

size_t htab_size(htab_t * t);

/**
 Number of items in the table to allocated (size as well)
 
 @param t table
 @return n number of all items
 */

size_t htab_bucket_count(htab_t * t);

/**
 Find (and optionally create) individual item from the table
 
 @param t table
 @param key string to find
 @return item found item from the function
 */

struct htab_listitem * htab_lookup_add(htab_t * t, char *key);

/**
 Find individual item from the table
 
 @param t table
 @param key string to find
 @return foundItem found item from the function
 */

struct htab_listitem * htab_find(htab_t * t, char *key);

/**
 Apply function for each item in the field
 
 @param t table
 @param func function to be executed upon each item
 */

void htab_foreach(htab_t * t, void (*func)(struct htab_listitem *));

/**
 Remove individual item from the table
 
 @param t table
 @param key string to find
 @return bool true if the items was cleared, otherwise false
 */

bool htab_remove(htab_t * t, char *key);

/**
 Clear all items int the table, it remains empty
 
 @param t table
 */

void htab_clear(htab_t * t);

/**
 Descructor of the table
 
 @param t table
 */

void htab_free(htab_t * t);

/**
 Moves the table
 
 @param newsize new size of the table
 @param t2 source table to get data from
 @return movedTable pointer to the created table
 */

htab_t * htab_move(unsigned int newsize, htab_t * t2);

#endif
