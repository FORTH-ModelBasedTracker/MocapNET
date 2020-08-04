#!/bin/bash

#Switch to this directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"
 

export MOCAPNET_DOXYGEN_INPUT="$DIR/.."
export MOCAPNET_DOXYGEN_OUTPUT="$DIR/../doc"

echo "Doxygen Input file will be $MOCAPNET_DOXYGEN_INPUT"
echo "Doxygen Output file will be $MOCAPNET_DOXYGEN_OUTPUT"


cd ..
doxygen doc/doxyfile
cd doc/latex
make
cd .. 
cd ..

ln -s doc/latex/refman.pdf MocapNETManual.pdf


cd "$DIR"
 

exit 0
