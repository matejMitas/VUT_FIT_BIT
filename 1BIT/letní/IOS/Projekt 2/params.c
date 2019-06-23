// params.c
// Řešení IOS-DU2, 27.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Čtení parametrů

#include "main.h"
#include "params.h"

#define INIT_COUNT 1
#define INIT_OPT_COUNT 0

/**
 Parses input arguments.
 
 @param argc number of arguments passed to the program.
 @param argv array of passed arguments.
 @param status signalizing the function status.
 @return struct params returns set file.
 */
struct params read_params(int argc, const char * argv[], int *status) {
    // init main params struct
    struct params my_params = { .adults = INIT_COUNT, .children = INIT_COUNT, .agt = 5, .cgt = INIT_OPT_COUNT, .awt = INIT_OPT_COUNT, .cwt = INIT_OPT_COUNT};
    
    if (argc == 7) {
        // loop through all the arguments
        for (int i = 1; i < 7; i++) {
            // check for temp value if is string
            
            char *ptr = NULL;
            long temp;
            temp = strtoul(argv[i], &ptr, 10);
            
            // condition, or else returns error
            if (temp >= 0 && *ptr == '\0') {
                
                if (i < 3 && temp > 0) {
                    if (i == 1) {
                        my_params.adults = (int)temp;
                    } else if (i == 2) {
                        my_params.children = (int)temp;
                    }
                } else if (i > 2 && temp < 5001) {
                    if (i == 3) {
                        my_params.agt = (int)temp;
                    } else if (i == 4) {
                        my_params.cgt = (int)temp;
                    } else if (i == 5) {
                        my_params.awt = (int)temp;
                    } else if (i == 6) {
                        my_params.cwt = (int)temp;
                    }
                    
                } else {
                    *status = PARAM_ERROR;
                    fprintf(stderr, ">> Wrong format of parameters\n");
                }
            } else {
                *status = PARAM_ERROR;
                fprintf(stderr, ">> Wrong format of decimal arguments\n");
                break;
            }
        }
    } else {
        *status = PARAM_ERROR;
        fprintf(stderr, ">> Wrong number of parameters\n");
    }
    
    // return newly created struct
    return my_params;
}
