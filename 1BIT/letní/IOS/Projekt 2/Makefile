CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
PROGS=proj2

all: $(PROGS)

proj2: params.o
	gcc $(CFLAGS) -o proj2 main.c params.o -lrt

params.o: params.c
	gcc $(CFLAGS) -c params.c -o params.o
	