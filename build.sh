#!/bin/bash

mkdir -p build

# shared library
cd Library/
make
cp libsocket_tls.so ../build/
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
