CC = gcc
EXECUTABLE = client_app

SRC = client.c

HDIR = ../include/
LIBNAME = tcpsocket_tls
LIBDIR = ../build
LDFLAGS = -lssl -lcrypto -l$(LIBNAME) -L$(LIBDIR) -I$(HDIR)

$(EXECUTABLE) : $(SRC)
	$(CC) -o $@ $^ $(LDFLAGS)
