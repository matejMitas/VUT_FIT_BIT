// wordcount.cc
// Řešení IJC-DU2, příklad 2), 23.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Počítání slov ze stdin

#include <stdio.h>

#include "htab.h"
#include "io.h"

#define MAX 127
#define SIZE 18233

/**
 Jak zvolit tuto velikost
    Obecně se doporučuje mít 'load factor' (naplnění tabulky) 75%, po překročení této hodnoty se dostáváme
    do úzkých ohledně možného shlukování a tedy zpomalení práce s tabulkou. Předchozí ovšem záleží na kvalitě
    hashovací funkce, která má dramatický dopad rychlost aplikace.
    Graf závislosti naplnění a možnosti shlukování -> https://en.wikipedia.org/wiki/Hash_table#/media/File:Hash_table_average_insertion_time.png
    Při zvolení prvocisla omezíme možnost shlukování -> http://srinvis.blogspot.com/2006/07/hash-table-lengths-and-prime-numbers.html
 */

/**
 Prints out single tab item
 
 @param item individual tab item
 */
void print_item(struct htab_listitem *item) {
    printf("%s\t%d\n", item->key, item->data);
}

int main() {
    // buffer
    // ensure space for ending zero
    char buffer[MAX+1] = {0,};
    // add htab
    htab_t *my_table = htab_init(SIZE);

    if (my_table == NULL) {
        fprintf(stderr, ">>Couldn't create the table");
        return EXIT_FAILURE;
    }
    // alokace mista pro tabulku
    while (get_word(buffer, MAX, stdin) != EOF) {
        htab_lookup_add(my_table, buffer);
    }
    // print foreach item
    htab_foreach(my_table, print_item);
    // clear the table
    htab_free(my_table);
    // exit
    return EXIT_SUCCESS;
}
