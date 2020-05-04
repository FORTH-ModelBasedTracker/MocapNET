#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"
 
mkdir build
cd build
cmake ..
make 

cd "$DIR"
./helloWorld

exit 0
