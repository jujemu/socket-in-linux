#!/bin/bash
mkdir -p build

# shared library
cd tcpsocket_tls
make
mv libtcpsocket_tls.so ../Echo/_build
cp tcpsocket_tls.h ../Echo/_build
make clean
cd ..

# server
cd Echo/server
make
rm ../../build/server_app
mv ./server_app ../../build
cd ../..

# client
cd Echo/client
make
rm ../../build/client_app
mv ./client_app ../../build
cd ../..
