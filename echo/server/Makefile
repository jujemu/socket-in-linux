CC = gcc
EXECUTABLE = server_app

SRC = server.c

HDIR = ../include/
LIBNAME = tcpsocket_tls
LIBDIR = ../build
LDFLAGS = -lssl -lcrypto -l$(LIBNAME) -L$(LIBDIR) -I$(HDIR) -Wall -Wextra -Werror -Wshadow -Wuninitialized -Wpedantic

$(EXECUTABLE) : $(SRC)
	$(CC) -o $@ $^ $(LDFLAGS)
