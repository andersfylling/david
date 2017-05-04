#! /bin/bash

# Update externals first
./update.sh

# prepare to build
cd cmake
rm -rf build
mkdir build
cd build

# Run tests and build
cmake ../..
make 
