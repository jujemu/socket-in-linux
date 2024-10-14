CC = gcc
TARGET = client

SRC = client.c

HDIR = /root/projects/echo/include/
IHDIR = -I$(HDIR)
LIBNAME = echo
LDFLAGS = -lssl -lcrypto -l$(LIBNAME)

all : $(TARGET)

$(TARGET) : $(SRC)
	$(CC) -o $@ $^ $(LDFLAGS) $(IHDIR)
