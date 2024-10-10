CC = gcc
TARGET = client

OBJS = client.o client_tls.o client_socket.o error_handler.o
LINK = -lssl -lcrypto

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(LINK)

%o : %c
	$(CC) -c $< -o $@
