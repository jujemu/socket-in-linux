CC = gcc
EXECUTABLE = client_app

SRC = client.c

LIBNAME = socket_tls
LDDIR = ../../build
HDIR = ../../Library/include/
LDFLAGS = -lssl -lcrypto -l$(LIBNAME) -L$(LDDIR) -I$(HDIR)

$(EXECUTABLE) : $(SRC)
	$(CC) -o $@ $^ $(LDFLAGS) $(IHDIR)
