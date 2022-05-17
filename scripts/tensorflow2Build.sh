#!/bin/bash

#Script last updated 25-03-2022

#Script that seems to be able to build bazel/tf2 on Ubuntu 20.04
#I really deeply dislike the bazel build system which is bloated and obfuscated for no reason, just Google "NIH syndrome"
#However Tensorflow2 is a great NN framework
# See this video "How To Make Package Managers Cry" ->  https://www.youtube.com/watch?v=NSemlYagjIU#t=19m0s 
 
echo "Check CUDA"
ls /usr/local/cuda/extras/CUPTI/lib64
ls /usr/local/cuda
ls /usr/local/cuda/lib64/ | grep libcudnn.so 
nvcc -V


VERSION="2.8"

#Get number of bytes in RAM
RAM=`free | grep Mem | tr -s ' '| cut -f2 -d ' '`


BazelPleaseSlowDown="--local_resources 2048,.5,1.0"

if [ "$RAM" -gt "11286464" ]; then
  echo "More than 12GB";
BazelPleaseSlowDown=" "  
fi
if [ "$RAM" -gt "15297500" ]; then
  echo "More than 16GB";
BazelPleaseSlowDown=" "  
fi
if [ "$RAM" -gt "31861780" ]; then
  echo "More than 32GB";
BazelPleaseSlowDown=" "  
fi


#Tensorflow is a great Neural network library that unfortunately is coupled to the terrible Bazel build system
#This is a download and build script for Ubuntu 20.04, that should work building release 2.8  

sudo apt-get install python3-dev python3-pip python3-venv python3-tk

pip3 install -U --user pip numpy wheel packaging
pip3 install -U --user keras_preprocessing --no-deps

cd ~/Documents
mkdir 3dParty
cd 3dParty

#wget https://github.com/bazelbuild/bazel/releases/download/4.2.1/bazel-4.2.1-installer-linux-x86_64.sh
#chmod +x bazel-4.2.1-installer-linux-x86_64.sh
#./bazel-4.2.1-installer-linux-x86_64.sh --user

#r2.8
mkdir -p "$HOME/.bazel/bin" && cd "$HOME/.bazel/bin" && curl -fLO https://releases.bazel.build/4.2.1/release/bazel-4.2.1-linux-x86_64 && chmod +x bazel-4.2.1-linux-x86_64

#Create shared directory
if [ -f ~/.bashrc ]
then 
 if cat ~/.bashrc | grep -q "BAZEL_CANCER"
then
   echo "Bazel includes seem to be set-up.." 
else 
  USER=`whoami` 
  echo "#BAZEL_CANCER" >> ~/.bashrc
  echo "source $HOME/.bazel/bin/bazel-complete.bash" >> ~/.bashrc
  echo "export PATH=\"\$PATH:\$HOME/bin\"" >> ~/.bashrc
  source ~/.bashrc 
 fi
fi

if [ ! -d tensorflow ]
then 
git clone https://github.com/tensorflow/tensorflow.git
fi

cd tensorflow
git pull
git checkout r$VERSION


#Make sure to check your target CPU and when asked used the correct -march= / -mtune= 
# for example for an old intel i7 -march=nehalem is used..
#https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html


echo "Answers to configure questions :" 
echo "/usr/bin/python3"
echo "/usr/lib/python3/dist-packages"
echo "cuda Y"
echo "tensorrt Y"
echo "CUDA 11.2"
echo "CuDNN 8"
echo "TensorRT 8"
echo "/usr/local/cuda/,/usr/local/cuda/include/,/usr/local/cuda/bin/,/usr/local/cuda/lib64/,/usr/local/cuda/lib64/,/usr/local/tensorrt-8.2.3/,/usr/local/tensorrt-8.2.3/include/,/usr/local/tensorrt-8.2.3/lib/,"
echo "Compute capability 6.1 ( for GTX 1050 + cards )"

#Attempt to inform the configure script on how to find the CUDA stuff..
export CUDNN_INSTALL_PATH=/usr/local/cuda/,/usr/local/cuda/include/,/usr/local/cuda/bin/,/usr/local/cuda/lib64/,/usr/local/cuda/lib64/,/usr/local/tensorrt-8.2.3/,/usr/local/tensorrt-8.2.3/include/,/usr/local/tensorrt-8.2.3/lib/


./configure

bazel clean --expunge



#You should use CUDA 11.2 and cudnn-11.2-linux-x64-v8.1.1.33 and TensorRT 8.2.3

bazel build --config=opt --config=cuda --config=mkl --config=monolithic $BazelPleaseSlowDown  //tensorflow/tools/pip_package:build_pip_package
./bazel-bin/tensorflow/tools/pip_package/build_pip_package ~/Documents/3dParty/
#To install
#pip3 --user install ~/Documents/3dParty/tensorflow-2.4.0-cp36-cp36m-linux_x86_64.whl



bazel build --config opt --config=cuda --config=monolithic  //tensorflow/tools/lib_package:libtensorflow
cp bazel-bin/tensorflow/tools/lib_package/libtensorflow.tar.gz ~/Documents/3dParty/libtensorflow-r$VERSION.tar.gz


#Build tensorflow lite
#https://www.tensorflow.org/lite/guide/build_cmake
mkdir tflite_buld
cd tflite_buld/
cmake ../tensorflow/lite
cmake --build . -j


echo "Please visit ~/Documents/3dParty/ to collect your tensorflow python3 wheel, and C Library.."
echo "Will now use  :    python -c 'import tensorflow as tf;'      to test your tensorflow"

python -c 'import tensorflow as tf;'

exit 0
