CC = gcc
TARGET = server

SRC = server.c

HDIR = /root/projects/echo/include/
IHDIR = -I$(HDIR)
LIBNAME = echo
LDFLAGS = -lssl -lcrypto -l$(LIBNAME)

all : $(TARGET)

$(TARGET) : $(SRC)
	$(CC) -o $@ $^ $(LDFLAGS) $(IHDIR)
