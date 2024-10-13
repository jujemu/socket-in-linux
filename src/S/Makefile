CC = gcc
TARGET = server

OBJS = server.o server_tls.o server_socket.o
LINK = -lssl -lcrypto

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(LINK)

%o : %c
	$(CC) -c $< -o $@
