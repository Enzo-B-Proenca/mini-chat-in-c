CC=gcc
SRC=src
INCLUDE_DIR=include
CFLAGS=-I$(INCLUDE_DIR)-Wall -Wextra -O2 -fsanitize=address

all: server client

server: $(SRC)/chat_server.c
	$(CC) $(CFLAGS) -o chat_server $(SRC)/chat_server.c

client: $(SRC)/chat_client.c
	$(CC) $(CFLAGS) -o chat_client $(SRC)/chat_client.c

clean:
	rm -f chat_server chat_client
