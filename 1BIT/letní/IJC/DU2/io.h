// io.h
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Implementace funkce get_word pro načítání
// jednotlivých slov do dalších funkcí 

#ifndef io_h
#define io_h

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

/**
 Read one word from input
 
 @param s pointer to the newly allocated string
 @param max maximum lenght of string
 @param f file to be read from
 @return count of read chars
 */
int get_word(char *s, int max, FILE *f);

#endif
