// hash_function.h
// Řešení IJC-DU2, příklad 2, 21.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Hashovací funkce pro tabulku

#ifndef hash_function_h
#define hash_function_h

#include <stdio.h>

/**
 Hash function
 
 @param str string be to indexed with
 @return index to be particular item in the table
 */

unsigned int hash_function(const char *str);

#endif
