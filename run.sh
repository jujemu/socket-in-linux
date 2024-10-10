#!/bin/bash

# server
cd ./S
make
mv ./server ..

# client
cd ../C
make
mv ./client ..

cd ..