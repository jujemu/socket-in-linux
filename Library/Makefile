CC = gcc

LIBNAME = libsocket_tls.so
SRC = src/server_tls.c src/server_socket.c src/client_tls.c src/client_socket.c src/error_handler.c
OBJ = $(SRC:.c=.o)

CFLAGS = -c -fPIC
LDFLAGS = -shared -o
HDIR = include/
IHDIR = -I$(HDIR)

$(LIBNAME) : $(OBJ)
	$(CC) $(LDFLAGS) $@ $(OBJ)

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@ $(IHDIR)

clean :
	rm -f $(OBJ)
