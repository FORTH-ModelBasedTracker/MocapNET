#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

 python3 -m venv venv
 source venv/bin/activate
 pip install tensorflow==1.3.0 
 pip install numpy==1.14.5 
 pip install scipy==1.1.0 
 pip install matplotlib==1.5.3 
 pip install torch==0.3.1 
 pip install opencv-python==3.4.1.15

 python evaluate_model.py

exit 0
