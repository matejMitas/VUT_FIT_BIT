/**
 proj1.c
 Purpose: Text manipulation
 
 @author Matej Mitas
 @login xmitas02
 */

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define LINE 16
#define S_BUFFER 200
#define R_BUFFER 3

/* ==========================================
 
 Utils functions
 
 ========================================== */


/**
 Decides whenever two strings are equal or not
 
 @param a first array to compare.
 @param b second array to compare.
 @return int state of comparison
 */

int u_compareStr(char a[], char b[]) {
    int c = 0;
    
    while (a[c] == b[c]) {
        if (a[c] == '\0' || b[c] == '\0')
            break;
        c++;
    }
    
    if (a[c] == '\0' && b[c] == '\0')
        return true;
    else
        return false;
}

/**
 Prints out simple manual
 */

void u_printMan() {
    printf("\nThis program take input from stdin and transforms it according to used flags\nYou can run this program in five different modes:\n1) without flags './proj1'\n- input string is the divided to chunks of 16 characters, address is added as well as hexadecimal representation\n\n2) './proj1 [-s M] [-n N]'\n- pretty much the same as previous mode, flags '-s' marks start index of reading, '-n' stands for number of characters printed\n\n3) './proj1 -x'\n- just plain conversion to hexadecimal representation\n\n4) './proj1 -S N'\n- prints you only printable strings (with blanks and spaces) if the string is longer than 'N' specified\n\n5) './proj1 -r'\n- hexadedimal characters are expected. Then they're converted to their text representation\n\n");
}


/* ==========================================
 
 Line & lineoptions functions
 
 ========================================== */

/**
 Prints hexadecimal representation of address used to store each byte
 
 @param lines first array to compare.
 @param startIndex second array to compare.
 */

void l_printAddress(int lines, int startIndex) {
    if (lines == 0 && startIndex > 0)
        printf("%08x  ", startIndex);
    else if (lines > 0 && startIndex > 0)
        printf("%08x  ", (LINE * lines) + startIndex);
    else
        printf("%08x  ", LINE * lines);
}

/**
 Adds data to the buffer
 
 @param B pointer to an array.
 @param character to add.
 @param position determines the index to which 'character' should be pushed.
 */

void l_addToBuffer(int *B, int character, int position) {
    B[position] = character;
}

/**
 Print out passed data
 
 @param array passing the whole array.
 @param len length of an passed array.
 @param coding hexa or text representation.
 */

void l_printBuffer(int array[], int len, char coding) {
    if (coding == 'c') {
        printf(" |");
    }
    
    for (int i = 0; i < LINE; i++) {
        
        int item;
        
        if (coding == 'x' || (isprint(array[i]) && coding == 'c')) {
            item = array[i];
        }
        
        else {
            item = '.';
        }
        
        // huge gap between 8 bytes
        if (coding == 'x' && i == 8) {
            printf(" ");
        }
        
        // completion of the line
        if (i > len) {
            if (coding == 'x') {
                printf("   ");
            } else if (coding == 'c') {
                printf(" ");
            }
        } else {
            if (coding == 'x') {
                printf("%02x ", item);
                
            } else if (coding == 'c') {
                printf("%c", item);
            }
        }
    }
    
    if (coding == 'c') {
        printf("|");
    }
}

/**
 Print out passed data
 
 @param buffer passing the whole array.
 @param len length of an passed array.
 @param rowIndex hexa or text representation.
 @param startIndex hexa or text representation.
 */
void l_printLine(int buffer[], int len, int rowIndex, int startIndex) {
    l_printAddress(rowIndex, startIndex);
    l_printBuffer(buffer, len, 'x');
    l_printBuffer(buffer, len, 'c');
}

/**
 Print out passed data
 
 @param startIndex passing the whole array.
 @param numberOfChars length of an passed array.
 */
void l_main(double startIndex, double numberOfChars) {
    // main character var
    int c;
    
    // helper vars
    int i = 0;
    int rowHelper = 0;
    int offsetHelper = 0;
    int offsetBegin = startIndex;
    bool emptyFile = false;
    
    // buffer
    int buffer[17] = {0};
    
    
    // main while loop
    while((c = getchar()) != EOF) {
        
        // check if not only empty stdin
        if (c == 10 && i == 0) {
            emptyFile = true;
        }
        
        // deal with offset
        if (offsetBegin != 0) {
            offsetBegin--;
        } else {
            if (offsetHelper < numberOfChars) { // you limit the range here
                
                if (i < LINE) {
                    // check if printable
                    l_addToBuffer(buffer, c, i);
                    i++;
                    
                } if (i == LINE) {
                    // set to printing function
                    if (startIndex != 0) {
                        l_printLine(buffer, LINE, rowHelper, startIndex);
                    } else {
                        l_printLine(buffer, LINE, rowHelper, 0);
                    }
                    
                    // reset all variables
                    i = 0;
                    rowHelper++;
                    
                    // break line after each line
                    printf("\n");
                }
            }
            
            offsetHelper++;
        }
    }
    
    if (c == EOF && !emptyFile) {
        if (startIndex != 0) {
            l_printLine(buffer, (i - 1), rowHelper, startIndex); // get rid of last character
        } else {
            l_printLine(buffer, (i - 1), rowHelper, 0); // get rid of last character
        }
        printf("\n");
    }
}

/* ==========================================
 
 Hexadecimal print funtions
 
 ========================================== */

/**
 Print out hexadimal characters
 
 */

void h_main() {
    int c;
    
    // while for getting char
    while((c = getchar()) != EOF) {
        printf("%02x", c);
    }
}


/* ==========================================
 
 Valid strings print funtions
 
 ========================================== */
int bufferCount = 0;

/**
 Print out hexadimal characters
 
 @param array working with array.
 @param len of array.
 */

void s_printBuffer(int array[], int len) {
    
    for (int i = 0; i < len; i++) {
        printf("%c", array[i]);
    }
    
    printf("\n");
}

/**
 Print out hexadimal characters
 
 @param character to be added to the array.
 @param min maximum number of chars not to be printed out.
 */

void s_check(char character, double min, int buff[]) {
    
    if (isprint(character) && bufferCount < 200) {
        
        buff[bufferCount] = character;
        bufferCount++;
        
    } else {
        
        if (bufferCount >= min) {
            s_printBuffer(buff, bufferCount);
        }
        
        bufferCount = 0;
    }
}

/**
 Print out hexadimal characters
 
 @param min maximum number of chars not to be printed out.
 */

void s_main(double min) {
    int c;
    int buff[200];
    
    
    // while for getting char
    while((c = getchar()) != EOF) {
        s_check(c, min, buff);
    }
    
    if (c == EOF) {
        if (bufferCount >= min) {
            s_printBuffer(buff, bufferCount);
        }
    }
}


/* ==========================================
 
 Reverse print funtions
 
 ========================================== */

/**
 Convert hexa to chars
 
 */

void r_printBuffer(char buffer[]) {
    char *ptr;
    long ret;
    
    ret = strtol(buffer, &ptr, 16);
    printf("%c", (char)ret);
}

/**
 Main function for r mode
 
 */

void r_main() {
    int c;
    int i = 0;
    char buffer[R_BUFFER];
    
    // main while loop
    while((c = getchar()) != EOF) {
        
        if (!isblank(c) && isxdigit(c)) {
            
            if (i % 2 != 0) {
                buffer[i] = c;
                r_printBuffer(buffer);
                
                
                i = -1;
            } else {
                buffer[i] = c;
            }
            
            i++;
        } else if (c != 10) {
            fprintf(stderr, ">Non hexadecimal number\n");
            exit(EXIT_FAILURE);
        }
    }
    
    if (c == EOF) {
        // check for leftover char
        if (i == 1) {
            
            buffer[1] = buffer[0];
            buffer[0] = '0';
            
            r_printBuffer(buffer);
        }
    }
}


/* ==========================================
 
 MAIN
 
 ========================================== */

int main(int argc, char *argv[]) {
    
    // bool values
    bool nFlag = false;
    bool sFlag = false;
    long nFlagNum = -1;
    long sFlagNum = -1;
    
    
    bool SFlag = false;
    long SFlagNum = 0;
    
    
    bool rFlag = false;
    bool xFlag = false;
    
    
    // args loop
    if (argc > 1) {
        
        int i;
        long val;
        char *next;
        
        // Process each argument given.
        
        for (i = 1; i < argc; i++) {
            // Get value with failure detection.
            val = strtol (argv[i], &next, 10);
            
            // Check for empty string and characters left after conversion.
            if ((next == argv[i]) || (*next != '\0')) {
                
                // strings
                if (u_compareStr("-r", argv[i]))
                    rFlag = true;
                else if (u_compareStr("-x", argv[i]))
                    xFlag = true;
                else if (u_compareStr("-S", argv[i]))
                    SFlag = true;
                else if (u_compareStr("-s", argv[i]))
                    sFlag = true;
                else if (u_compareStr("-n", argv[i]))
                    nFlag = true;
                
                
                // NON-VALID string argument
                else {
                    fprintf(stderr, ">Non-valid argument '%s' \n", argv[i]);
                    u_printMan();
                    exit(EXIT_FAILURE);
                }
                
                
            } else {
                
                // numbers
                if (nFlag && nFlagNum == -1) {
                    nFlagNum = val;
                } else if (sFlag && sFlagNum == -1) {
                    sFlagNum = val;
                } else if (SFlag && SFlagNum == 0) {
                    SFlagNum = val;
                } else {
                    fprintf(stderr, ">Nonassignable number argument '%ld' \n", val);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    
    // deciding logic
    if (argc == 1) {
        l_main(0, 0xFFFFFFFF);
    } else if (argc == 2 && xFlag) {
        h_main();
    } else if (argc == 2 && rFlag) {
        r_main();
    } else if (argc == 3 && SFlag && SFlagNum > -1) {
        s_main(SFlagNum);
    } else if (argc == 3 && nFlag && nFlagNum > -1) {
        l_main(0, nFlagNum);
    } else if (argc == 3 && sFlag && sFlagNum > -1) {
        l_main(sFlagNum, (0xFFFFFFFF - sFlagNum));
    } else if (argc == 5 && sFlag && sFlagNum > -1 && nFlag && nFlagNum > -1) {
        l_main(sFlagNum, nFlagNum);
    } else {
        fprintf(stderr, ">Wrong number of arguments for a given function\n");
        u_printMan();
        exit(EXIT_FAILURE);
    }
    
    
    exit(EXIT_SUCCESS);
}
