# Makefile
# Řešení IJC-DU1, Makefile, 26.03.2017
# Autor: Matěj Mitaš, FIT

CFLAGS= -O2 -std=c99 -Wall -pedantic -lm

build: primes primes-i steg-decode

# Build primes
eratosthenes.o: eratosthenes.c eratosthenes.h bit_array.h error.h

error.o: error.c error.h

primes.o: primes.c error.h bit_array.h eratosthenes.h

# Build STEG-DECODE
ppm.o: ppm.c ppm.h eratosthenes.h bit_array.h error.h

steg-decode.o: steg-decode.c bit_array.h error.h eratosthenes.h ppm.h

#sestaveni jednotlivych programu
primes: primes.o error.o eratosthenes.o
	gcc $(CFLAGS) primes.o error.o eratosthenes.o -o primes

primes-i: primes.o error.o eratosthenes.o
	gcc $(CFLAGS) -DUSE_INLINE primes.o error.o eratosthenes.o -o primes-i

steg-decode: steg-decode.o ppm.o error.o eratosthenes.o
	gcc $(CFLAGS) steg-decode.o error.o eratosthenes.o ppm.o -o steg-decode


# Run command
run: primes primes-i
	time ./primes
	time ./primes-i

# Clean dir
clr:
	rm -f *.o