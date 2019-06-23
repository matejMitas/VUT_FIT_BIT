// params.c
// Řešení IOS-DU2, 27.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Čtení parametrů

#ifndef params_h
#define params_h

#include <stdio.h>
#include <stdlib.h>

struct params {
    int adults; // number of adult procesess a > 0
    int children; // number of children procesess c > 0

    int agt; // waiting for generation of adult
    int cgt; // waiting for generation of children
    int awt; // participation in the centre from adult
    int cwt; // participation in the centre from children
};

/**
 Parses input arguments.
 
 @param argc number of arguments passed to the program.
 @param argv array of passed arguments.
 @param status signalizing the function status.
 @return struct params returns set file.
 */
struct params read_params(int argc, const char * argv[], int *status);


#endif /* params_h */
