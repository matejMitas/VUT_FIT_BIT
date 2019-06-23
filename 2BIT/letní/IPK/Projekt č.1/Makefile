CFLAGS = -std=c11 -Wall -Wextra -pedantic
# nastaveni vsech polozek
all: server client

server:
	gcc $(CFLAGS) -o ipk-server ipk-server.c ipk-error.c ipk-protocol.c ipk-parse.c
client: 
	gcc $(CFLAGS) -o ipk-client ipk-client.c ipk-error.c ipk-protocol.c ipk-parse.c
clean:
	rm ipk-client ipk-server