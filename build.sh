#!/bin/bash
mkdir -p echo/build

# shared library
cd tcpsocket_tls
make

rm -f ../echo/include/tcpsocket_tls.h ../echo/build/libtcpsocket_tls.so
mv libtcpsocket_tls.so ../echo/build/
cp tcpsocket_tls.h ../echo/include/

make clean
cd ..

# server
cd echo/server
make
rm -f ../../build/server_app
mv ./server_app ../build/
cd ../..

# client
cd echo/client
make
rm -f ../../build/client_app
mv ./client_app ../build/
cd ../..
