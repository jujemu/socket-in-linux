CC = gcc
TARGET = server client
SERVER = S/server.c
CLIENT = C/client.c
LINK = -lssl -lcrypto

all : $(TARGET)

server : $(SERVER)
	$(CC) -o $@ $^ $(LINK)

client : $(CLIENT)
	$(CC) -o $@ $^ $(LINK)
