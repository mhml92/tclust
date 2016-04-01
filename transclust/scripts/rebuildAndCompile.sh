#!/bin/bash

if [ -d "build" ]; then
   rm -R ./build
fi

mkdir build
cd build
cmake ..
make
cd ..
