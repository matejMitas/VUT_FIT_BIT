// ppm.h
// Řešení IJC-DU1, příklad a), 13.03.2017
// Autor: Matěj Mitaš, FIT

#ifndef ppm_h
#define ppm_h

#include <stdio.h>

#include "bit_array.h"
#include "error.h"
#include "eratosthenes.h"

#endif /* ppm_h */

struct ppm {
    unsigned xsize;
    unsigned ysize;
    char data[];    // RGB bajty, celkem 3*xsize*ysize
};

// I/O functions
struct ppm * ppm_read(const char * filename);

int ppm_write(struct ppm *p, const char * filename);
