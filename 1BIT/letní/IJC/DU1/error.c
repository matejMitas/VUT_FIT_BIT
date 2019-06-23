// error.c
// Řešení IJC-DU1, příklad a), 28.02.2017
// Autor: Matěj Mitaš, FIT

#include "error.h"

void warning_msg(const char *fmt, ...) {
    // init args variable
    va_list args;
    // append args into the var, as well as format
    va_start(args, fmt);
    // print it out
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, args);
    // clear out args
    va_end(args);
}

void error_msg(const char *fmt, ...) {
    // init args variable
    va_list args;
    // append args into the var, as well as format
    va_start(args, fmt);
    // print it out
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr,fmt, args);
    // clear out args
    va_end(args);
    
    // exit program with error
    exit(1);
}
