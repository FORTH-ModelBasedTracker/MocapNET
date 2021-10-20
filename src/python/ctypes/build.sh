#!/bin/bash

#gcc -c -fPIC c.c -o c.o
#gcc c.o -shared -o libcalci.so

gcc -shared -o libcalci.so -fPIC c.c

python3 p.py


exit 0
