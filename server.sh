#!/bin/bash
set -e

cd ./S
make
mv ./server ..
cd ..