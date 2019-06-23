// primes.c
// Řešení IJC-DU1, příklad a), 28.02.2017
// Autor: Matěj Mitaš, FIT

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "error.h"
#include "bit_array.h"
#include "eratosthenes.h"

#define ARRAY_SIZE 303000000

int main(int argc, const char * argv[]) {
    //
    // create array for accomodating all bits
    ba_create(eras_array, ARRAY_SIZE);
    
    // send it to the function
    Eratosthenes(eras_array);
    
    //print out primes
    unsigned long i = ba_size(eras_array);
    unsigned long primes[10] = {0, };
    int count = 10;
    
    while (count > 0) {
        
        if (ba_get_bit(eras_array, (i - 1)) == 0) {
            count--;
            primes[count] = (i - 1);
        }
        
        i--;
    }
    
    // print the in ascending order
    for (int j = 0; j < 10 ; j++) {
        printf("%lu\n", primes[j]);
    }
    
    return 0;
}
