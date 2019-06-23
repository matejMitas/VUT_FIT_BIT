// io.c
// Řešení IJC-DU2, příklad 2, 18.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Implementace funkce get_word pro načítání
// jednotlivých slov do dalších funkcí 


#include "io.h"

/**
 Read one word from input
 
 @param s pointer to the newly allocated string
 @param max maximum lenght of string
 @param f file to be read from
 @return count of read chars
 */
int get_word(char *s, int max, FILE *f) {
    
    // check for empty of corrupted file
    if (f == NULL) {
        return EOF;
    }
    
    // main variable for reading char
    int c;
    int read_chars = 1;
    
    // skip first chars
    while ((c = fgetc(f)) != EOF && isspace(c));
    
    // check for eof in the file
    if (c == EOF) {
        return EOF;
    }
    
    // get the first item since the skipped char was used in comparing with first char
    s[0] = c;
    
    // skip all the space chars
    while ((c = getc(f)) != EOF) {
        
        if (isspace(c)) {
            break;
        } else {
            // get char to array
            s[read_chars] = c;
            // if number of chars exceeds limit
            if (read_chars >= max) {
                // TODO: print out warning just once
                // flush the rest of the file
                while ((c = fgetc(f)) != EOF && !isspace(c));
                // break the while
                break;
            } else {
                read_chars++;
            }
        }
    }
    
    // append ending zero, which is possible by incrementing last item
    s[read_chars] = '\0';
    
    return read_chars;
}
