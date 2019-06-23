// ppm.c
// Řešení IJC-DU1, příklad a), 13.03.2017
// Autor: Matěj Mitaš, FIT

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

#include "ppm.h"

// read file and
struct ppm *ppm_read(const char * filename) {
    // create new file
    static struct ppm *newFile;
    // read file
    FILE *source;
    source = fopen(filename, "r");
    
    // all the neccesary vars
    char output;
    int counter = 1;
    
    // temporary vars for checking whenever it fits the limit of not
    int x = 0;
    int y = 0;
    
    if (source != NULL) {
        // read files size
        fscanf(source, "P6\n%d %d\n255\n", &x, &y);
        
        // formula xsize * ysize * 3
        int arraySize = x * y * 3;
        
        // if coordinates fit into implementation limit
        if (arraySize < 1000*1000*3) {
            
            // create array of primes
            ba_create(primesToCompare, 3000000);
            Eratosthenes(primesToCompare);
            
            // allocate space for struct
            newFile = malloc(sizeof(struct ppm) + arraySize * sizeof(char));
            
            // store coordinates into the object
            newFile->xsize = x;
            newFile->ysize = y;
            
            // loop over it
            while (fscanf(source, "%c", &output) != EOF) {
                newFile->data[counter] = output;
                counter++;
            }
            
            // set links for main file
            int breakChar = 0;
            char test[1] = {0};
            int i = 0;
            int endFlag = 0;
            
            // loop for reading the numbers
            while (1 && i < arraySize) {
                
                if (ba_get_bit(primesToCompare, i) == 0) {
                    if (breakChar < 7) {
                        BA_SET_BIT_(test, breakChar, BA_GET_BIT_(newFile->data, i * 8));
                        breakChar++;
                    } else {
                        // =====================================
                        //
                        printf("%c", test[0]);
                        //
                        // =====================================
                        
                        if (test[0] == '\0') {
                            printf("\n");
                            endFlag = 1;
                            break;
                        }
                        
                        breakChar = 0;
                    }
                }
                
                i++;
            }
            
            // if the string is not complete with '\0' at the end
            if (!endFlag) {
                warning_msg("Soubor neobsahuje konecny znak\n");
            }
            
        } else {
            warning_msg("Chybny format\n");
            return NULL;
        }
        
        fclose(source);
        
    } else {
        error_msg("Soubor nemohl byt otevren\n");
    }
    
    return newFile;
}


// write
int ppm_write(struct ppm *p, const char * filename) {
    
    // open file
    FILE *source;
    source = fopen(filename, "r+");
    
    if (source != NULL) {
        // print out header to the file
        fprintf(source, "P6\n%d %d\n255\n", p->xsize, p->ysize);
        
        // get array size
        int arraySize = p->xsize * p->ysize * 3;
        
        // print out data
        for (int i = 1; i < arraySize; i++) {
            fprintf(source, "%c", p->data[i]);
        }
        
        fclose(source);
        
    } else {
        warning_msg("Chyba zapisu\n");
        return -1;
    }
    
    
    return 0;
}
