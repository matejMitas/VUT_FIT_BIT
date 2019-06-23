// steg-decode.c
// Řešení IJC-DU1, příklad a), 13.03.2017
// Autor: Matěj Mitaš, FIT

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include "ppm.h"

int main(int argc, const char * argv[]) {
    // read file arguments
    if (argc == 2) {
        // create struct
        struct ppm *myFile = ppm_read(argv[1]);
        // remove from memory
        free(myFile);
    } else {
        error_msg("Spatny pocet vstupnich argumentu\n");
    }
    
    return 0;
}
