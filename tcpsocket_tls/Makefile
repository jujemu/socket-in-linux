CC = gcc

LIBNAME = libtcpsocket_tls.so
SRC =  common_tls.c server_socket.c server_tls.c client_socket.c client_tls.c error_handler.c
OBJ = $(SRC:.c=.o)

CFLAGS = -c -fPIC
HDIR = .
LDFLAGS = -shared -I$(HDIR) -Wall -Wextra -Werror -Wshadow -Wuninitialized -Wpedantic -o

$(LIBNAME) : $(OBJ)
	$(CC) $(LDFLAGS) $@ $(OBJ)

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f $(OBJ)
