CFLAGS = -std=c11 -Wall -Werror -Wextra -pedantic
# nastaveni vsech polozek
all: lookup

lookup:
	gcc $(CFLAGS) -o ipk-lookup ipk-lookup.c ipk-args.c ipk-error.c ipk-name.c ipk-header.c
clean:
	rm ipk-lookup