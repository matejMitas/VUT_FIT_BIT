/**
 proj2.c
 Purpose: Logarithm & pow function implemented just with basic mathematical operations (+,-,*,/)
 
 @author Matej Mitas
 @login xmitas02
 */

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define START_VALUE -1

/* ==========================================
 
 Log functions
 
 ========================================== */

/**
 Returns log of number x using two formulas
 
 @param x number to use logarithm function on.
 @param n number of iterations.
 @return double result of the mathematical operation.
 */

double taylor_log(double x, unsigned int n) {
    
    double result = 0.0;
    
    if (isinf(x)) {
        if (x == -INFINITY) {
            result = NAN;
        } else {
            result = INFINITY;
        }
    } else if (x == 0) {
        result = -INFINITY;
    } else if (x > 0) {
        if (x > 1) {
            double numerator = 1;
            
            for (unsigned int i = 1; i <= n; i++) {
                numerator *= (x - 1) / x;
                result += numerator / i;
            }
        }
            
        else if (x > 0 && x < 1) {
            
            x = 1 - x;
            
            double numerator = 1;
            
            for (unsigned int i = 1; i <= n; i++) {
                numerator *= x;
                result -= numerator / i;
            }
        }
    } else {
        result = NAN;
    }
    
    return result;
}

/**
 Returns log of number x using continued fractions
 @param x number to use logarithm function on.
 @param n number of iterations.
 @return double result of the mathematical operation.
 */

double cfrac_log(double x, unsigned int n) {
    
    double result = 0.0;
    
    if (isinf(x)) {
        if (x == -INFINITY) {
            result = NAN;
        } else {
            result = INFINITY;
        }
    } else if (x == 0) {
        result = -INFINITY;
    } else if (x > 0) {
        double cf = 0.0;
        double a, b;
        
        x = (x - 1) / (x + 1);
        
        for (; n > 0; n--) {
            a = (x*x) * (n*n);
            b = (2 * (n + 1) - 1);
            cf = a / (b - cf);
        }
        
        result = 2 * x / (1.0 - cf);
    } else {
        result = NAN;
    }
    
    return result;
}

/* ==========================================
 
 Pow functions
 
 ========================================== */

/**
 Returns pow of number x using predefined formula
 @param x number to use logarithm function on.
 @param y exponent
 @param n number of iterations.
 @param mode which method to use.
 @return double result of the mathematical operation.
 */

double common_pow(double x, double y, unsigned int n, char mode) {
    

    double result = 0;
    
    if (isnan(x) || isnan(y) || x <= 0) {
        result = NAN;
    }
    
    if (x > 0 && x != 1) {
        double log = 0.0;
        result++;
        
        if (mode == 't') {
            log = taylor_log(x, n);
        } else if (mode == 'c') {
            log = cfrac_log(x, n);
        }
        
        double numerator = 1;
        double denominator = 1;
        
        for (unsigned int i = 1; i <= n; i++) {
            
            numerator *= y * log;
            denominator *= i;
            
            result += numerator / denominator;
        }
    }
    
    if (x == 1 || y == 0) {
        result = 1;
    }
    
    if (isinf(x)) {
        if (y == INFINITY) {
            result = INFINITY;
        } else if (y < 0) {
            result = 0;
        }
    }
    
    if ((x > 0 && x < 1 && y == INFINITY) || (x > 1 && y == -INFINITY)) {
        result = 0;
    }
    
    return result;
}

/**
 Returns pow of number x using taylor polynom
 @param x number to use logarithm function on.
 @param y exponent
 @param n number of iterations.
 @return double result of the mathematical operation.
 */

double taylor_pow(double x, double y, unsigned int n) {
    return common_pow(x, y, n, 't');
}

/**
 Returns pow of number x using continued fractions
 @param x number to use logarithm function on.
 @param y exponent
 @param n number of iterations.
 @return double result of the mathematical operation.
 */

double taylorcf_pow(double x, double y, unsigned int n) {
    return common_pow(x, y, n, 'c');
}

/* ==========================================
 
 Print functions
 
 ========================================== */

/**
 Prints out the results of pow
 @param x number to use pow function on.
 @param y exponent
 @param n number of iterations.
 */

void print_pow(double x, double y, unsigned int n) {
    printf("         pow(%g, %g) = %.12g\n  taylor_pow(%g, %g) = %.12g\ntaylorcf_pow(%g, %g) = %.12g\n", x, y, pow(x, y), x, y, taylor_pow(x, y, n),  x, y, taylorcf_pow(x, y, n));
}

/**
 Prints out the results of logarithm
 @param x number to use logarithm function on.
 @param n number of iterations.
 */

void print_log(double x, unsigned int n) {
    printf("       log(%g) = %.12g\n cfrac_log(%g) = %.12g\ntaylor_log(%g) = %.12g\n", x, log(x), x, cfrac_log(x, n), x, taylor_log(x, n));
}

/* ==========================================
 
 Utils functions
 
 ========================================== */

/**
 Prints out the results of logarithm
 @param argc number arguments.
 @param argv pointer to the array of containing the arguments.
 @return state of arguments parsing
 */


int u_parse_cmdline(int argc, char *argv[]) {
    
    // bool values
    bool logFlag = false;
    bool powFlag = false;
    
    // check if values modified
    bool logSt[] = {0};
    bool powSt[] = {0,0};
    
    double logFlagNum = 0;
    double powFlagNumX = 0;
    double powFlagNumY = 0;
    double iterations = 0;
    
    // args loop
    if (argc > 1) {
        
        
        int i;
        double val;
        char *next;
        
        // Process each argument given.
        
        for (i = 1; i < argc; i++) {
            // Get value with failure detection.
            val = strtod(argv[i], &next);
            
            // Check for empty string and characters left after conversion.
            if ((next == argv[i]) || (*next != '\0')) {
                
                // strings
                if (!strcmp("--log", argv[i])) {
                    if (argc == 4) {
                        logFlag = true;
                    } else {
                        fprintf(stderr, ">Wrong number of arguments for --log, needed 2, got %d \n", (argc - 2));
                        return EXIT_FAILURE;
                    }
                                    }
                else if (!strcmp("--pow", argv[i])) {
                    if (argc == 5) {
                        powFlag = true;
                    } else {
                        fprintf(stderr, ">Wrong number of arguments for --pow, needed 3, got %d \n", (argc - 2));
                        return EXIT_FAILURE;
                    }
                }
                
                // NON-VALID string argument
                else {
                    fprintf(stderr, ">Non-valid argument '%s' \n", argv[i]);
                    return EXIT_FAILURE;
                }
                
                
            } else {
                // numbers
                
                if (logFlag && !logSt[0]) {
                    logFlagNum = val;
                    logSt[0] = true;
                } else if (powFlag && !powSt[0]) {
                    powFlagNumX = val;
                    powSt[0] = true;
                } else if (powFlag && powSt[0] && !powSt[1]) {
                    powFlagNumY = val;
                    powSt[1] = true;
                } else if ((powFlag && powSt[0] && powSt[1]) || (logFlag && logSt[0])) {
                    if (val > 0) {
                        iterations = val;
                    } else {
                        fprintf(stderr, ">Number of iterations must be greater than 0\n");
                        return EXIT_FAILURE;
                    }
                } else {
                    fprintf(stderr, ">Nonassignable number argument '%g' \n", val);
                    return EXIT_FAILURE;
                }
            }
        }
    }
    
    // deciding logic
    if (logFlag && logSt[0] && iterations > START_VALUE)
        print_log(logFlagNum, iterations);
    else if (powFlag && powSt[0] && powSt[1] && iterations > -DBL_MAX)
        print_pow(powFlagNumX, powFlagNumY, iterations);
    else {
        fprintf(stderr, ">Not enough valid arguments for a given function\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}


/* ==========================================
 
 MAIN
 
 ========================================== */

int main(int argc, char *argv[]) {
    
    int program_status = u_parse_cmdline(argc, argv);

    return program_status;
}
