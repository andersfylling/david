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
make -j 

cd bin
chmod +x chess_ann_tests
./chess_ann_tests
