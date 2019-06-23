CFLAGS = -std=c99  -pedantic -lpcap 
# nastaveni vsech polozek
all: dns-export

dns-export:
	gcc $(CFLAGS) -o dns-export isa-project.c isa-args.c isa-error.c isa-parser.c isa-db.c

run:
	./dns-export

clean:
	rm dns-export