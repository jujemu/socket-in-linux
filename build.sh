#!/bin/bash
mkdir -p build

# shared library
cd /root/projects/echo/
cd ./lib/
make
mv libecho.so /usr/lib
make clean

# server
cd ../src/S
make
mv ./server ../../build
cd ../..

# client
cd ./src/C
make
mv ./client ../../build
cd ../..
