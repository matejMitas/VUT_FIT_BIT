// eratosthenes.c
// Řešení IJC-DU1, příklad a), 26.03.2017
// Autor: Matěj Mitaš, FIT

#include "eratosthenes.h"


void Eratosthenes(bit_array_t *pole) {
    // set first to bits on 1 since they can't be a prime number just by definition
    ba_set_bit(pole, 0, 1);
    ba_set_bit(pole, 1, 1);
    
    // get size of the field
    unsigned long max = ba_size(pole);
    
    //sieve itself
    for (int i = 2; i < sqrt(max); i++) {
        if (ba_get_bit(pole, i) == 0) {
            for (int j = i; i*j < max; j++) {
                ba_set_bit(pole, j*i, 1);
            }
        }
    }
}
