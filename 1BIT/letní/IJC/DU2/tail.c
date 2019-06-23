// tail.c
// Řešení IJC-DU2, příklad 1a), 4.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Simulace POSIXOVÉ utility tail v jazyce C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX 1024
#define DEFAULT_LINES 10

struct params {
    bool n_flag;
    int lines;
    FILE *source;
};

/**
 Parses input arguments.
 
 @param argc number of arguments passed to the program.
 @param argv array of passed arguments.
 @param status signalizing the function status.
 @return struct params returns set file.
 */
struct params read_params(int argc, const char * argv[], int *status) {
    // create new file and init it with default values
    struct params my_params = { .n_flag = false, .lines = DEFAULT_LINES, .source = NULL};

    // set OK status
    *status = EXIT_SUCCESS;
    
    // limit max amount of params
    if (argc <= 4) {
        if (argc == 3 || argc == 4) {
            // check for -n
            if (!(strcmp(argv[1], "-n"))) {
                
                // check for number
                my_params.lines = atoi(argv[2]);
                
                if (my_params.lines > 0) {
                    // open file if present fourth argument
                    if (argc == 4) {
                        my_params.source = fopen(argv[3], "r");
                        if (my_params.source == NULL) {
                            *status = EXIT_FAILURE;
                            fprintf(stderr, ">> File couldn't be opened\n");
                        }
                    }
                } else {
                    *status = 1;
                    fprintf(stderr, ">> -n takes valid positive integrer value\n");
                }
                
            } else {
                *status = 1;
                fprintf(stderr, ">> Unrecongisable flag\n");
            }
            
        } else if (argc == 2) {
            my_params.source = fopen(argv[1], "r");
            if (my_params.source == NULL) {
                *status = EXIT_FAILURE;
                fprintf(stderr, ">> File couldn't be opened\n");
            }
        }
    } else {
        *status = EXIT_FAILURE;
        fprintf(stderr, ">> Wrong number of parameters\n");
    }
    
    return my_params;
}

/**
 Tail function for reading last n line of the passed file
 
 @param input file to process in the function
 @param lines number of line submited by the user
 */
void tail(FILE *input, int lines) {
    // read lines
    int read_lines = 0;
    int c;
    bool warning = false;
    // ensure that program can read from stdin if not file name is passed,
    // checking if the file is properly read happens in read_param function
    if (input == NULL) {
        input = stdin;
    }
    // create two dimensional field to contain all the lines
    char buffer[lines][MAX];
    // size of array
    unsigned long size = sizeof(buffer);
    // create two dimensional field to contain all the lines
    char temp[MAX];

    // check for line number
    while (true) {
        
        // temp placeholder for value
        char *value = fgets(temp, MAX, input);
        
        // end of the file or read lines
        if (value == NULL) {
            break;
        } else {
            read_lines++;
        }
        
        // indexy radku
        if (read_lines > lines) {
            read_lines = lines;
            memmove(&buffer[0], &buffer[1], size - (size / lines));
        }
        
        // if line is too long shorten it
        if (strchr(temp, '\n') == NULL) {
            // set the warning
            if (!warning) {
                fprintf(stderr, ">>Too long line\n");
                warning = true;
            }
            // throw out end of the line
            while ((c = fgetc(input)) != '\n' && c != EOF);
            // append zero to the end
            temp[MAX - 2] = '\n';
        }
        
        // add to the buffer
        strcpy(buffer[read_lines-1], temp);
        
        //printf("Nacteny radek: %d, Hodnota: %s", read_lines-1, temp);
    }
    
    if (read_lines > 0) {
        // print out the buffer
        for (int i = 0; i < lines; i++) {
            printf("%s", buffer[i]);
        }
    }
}





int main(int argc, const char * argv[]) {
    // init status var
    int status = EXIT_SUCCESS;
    // parse arguments
    struct params params = read_params(argc, argv, &status);
    // error reading params
    if (status == EXIT_FAILURE) {
        return status;
    }
    // call the function
    tail(params.source, params.lines);
    // if the file was opened close it
    if (params.source != NULL) {
        fclose(params.source);
    }
    // return
    return status;
}
