#!/bin/bash
set -e

cd ./C
make
mv ./client ..
cd ..