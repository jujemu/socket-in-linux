CC = gcc
TARGET = $(LIBNAME)

LIBNAME = libecho.so
SRC = server_tls.c server_socket.c client_tls.c client_socket.c error_handler.c
OBJ = $(SRC:.c=.o)

CFLAGS = -c -fPIC
LDFLAGS = -shared -o
HDIR = /root/projects/echo/include/
IHDIR = -I$(HDIR)

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(LDFLAGS) $(LIBNAME) $(OBJ)

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@ $(IHDIR)

clean :
	rm -f $(OBJ) $(TARGET)
