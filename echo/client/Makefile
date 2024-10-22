CC = gcc
EXECUTABLE = client_app

SRC = client.c

HDIR = ../include/
LIBNAME = tcpsocket_tls
LIBDIR = ../build

LINK = -lssl -lcrypto -l$(LIBNAME) -L$(LIBDIR) -Wl,-rpath=.
LDFLAGS = $(LINK) -I$(HDIR) -Wall -Wextra -Wshadow -Wuninitialized -Wpedantic

$(EXECUTABLE) : $(SRC)
	$(CC) -o $@ $^ $(LDFLAGS)
