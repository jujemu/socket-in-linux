#!/bin/bash
source ./server.sh
source ./client.sh

# shared library
cd /root/projects/echo/
cd ./lib/
make
mv libecho.so /usr/lib64
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

# LD_LIBRARY_PATH update
export LD_LIBRARY_PATH=/usr/lib64:$LD_LIBRARY_PATH
