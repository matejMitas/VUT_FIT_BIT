// tail2.cc
// Řešení IJC-DU2, příklad 1b), 22.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Simulace POSIXOVÉ utility tail v jazyce C

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <deque>
#include <sstream>
#include <fstream>



#define DEFAULT_LINES 10

using namespace std;

struct params {
    bool n_flag;
    int lines;
    char *source;
};

/**
 Parses input arguments.
 
 @param argc number of arguments passed to the program.
 @param argv array of passed arguments.
 @param status signalizing the function status.
 @return struct params returns set file.
 */
struct params read_params(int argc, char * argv[], int *status) {
    // create new file and init it with default values
    // create new file and init it with default values
    params my_params = {
        false,
        DEFAULT_LINES,
        NULL
    };
    
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
                        my_params.source = argv[3];
                    }
                } else {
                    *status = EXIT_FAILURE;
                    fprintf(stderr, ">> -n takes valid positive integrer value\n");
                }
                
            } else {
                *status = EXIT_FAILURE;
                fprintf(stderr, ">> Unrecongisable flag\n");
            }
            
        } else if (argc == 2) {
            my_params.source = argv[1];
        }
    } else {
        *status = EXIT_FAILURE;
        fprintf(stderr, ">> Wrong number of parameters\n");
    }
    
    return my_params;
}

/**
 Tail function for reading last n line of the passed file
 
 @param input name of the file to be opened
 @param lines number of line submited by the user
 */
void tail(char *input, int lines) {
    // read lines
    int read_lines = 0;
    // array for containing tail items
    deque<std::string> buffer;
    // one line
    string temp;
    // if file is not opened
    if (input == NULL) {
        while (getline(std::cin, temp)) {
            read_lines++;
            buffer.push_back(temp);
            
            if (read_lines > lines) {
                buffer.pop_front();
            }
        }
    } else {
        // open file
        ifstream file(input);
        
        if (!file.is_open()) {
            return;
        }
        while (std::getline(file, temp)) {
            read_lines++;
            buffer.push_back(temp);
            
            if (read_lines > lines) {
                buffer.pop_front();
            }
        }
        // close file
        file.close();
    }
    
    for (int i = 0; i < lines && i < read_lines; i++) {
        cout << buffer.at(i) << endl;
    }
}


int main(int argc, char *argv[]) {
    // disable for faster moving
    ios::sync_with_stdio(false);
    // init status var
    int status = EXIT_SUCCESS;
    // read arguments
    struct params params = read_params(argc, argv, &status);
    // exit if not valid
    if (status == EXIT_FAILURE) {
        return status;
    }
    // call the function
    tail(params.source, params.lines);
    
    return status;
}
