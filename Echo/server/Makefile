CC = gcc
EXECUTABLE = server_app

SRC = server.c

HDIR = ../../Library/include/
IHDIR = -I$(HDIR)
LIBNAME = socket_tls
LDFLAGS = -lssl -lcrypto -l$(LIBNAME)

$(EXECUTABLE) : $(SRC)
	$(CC) -o $@ $^ $(LDFLAGS) $(IHDIR) -L../../build
