CC = gcc
TARGET = client_app

SRC = client.c

HDIR = ../../Library/include/
IHDIR = -I$(HDIR)
LIBNAME = socket_tls
LDFLAGS = -lssl -lcrypto -l$(LIBNAME)

all : $(TARGET)

$(TARGET) : $(SRC)
	$(CC) -o $@ $^ $(LDFLAGS) $(IHDIR) -L../../build
