#!/bin/bash

#Script source repository https://github.com/AmmarkoV/MyScripts/blob/master/Tensorflow/tensorflowBuild.sh
#Script last updated 23-06-2020

#Script that seems to be able to build bazel/tf2 on Ubuntu 20.04
#I really deeply dislike the bazel build system which is bloated and obfuscated for no reason, just Google "NIH syndrome"
#However Tensorflow2 is a great NN framework
# See this video "How To Make Package Managers Cry" ->  https://www.youtube.com/watch?v=NSemlYagjIU#t=19m0s 
 

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
#This is a download and build script for Ubuntu 18.04, that should work building release 1.15  

sudo apt-get install python3-dev python3-pip python3-venv python3-tk

pip install -U --user pip six numpy wheel setuptools mock 'future>=0.17.1'
pip install -U --user keras_applications --no-deps
pip install -U --user keras_preprocessing --no-deps


cd ~/Documents
mkdir 3dParty
cd 3dParty

wget http://ammar.gr/mocapnet/bazel-0.24.1-installer-linux-x86_64-for-tensorflow-r1.15.sh
chmod +x bazel-0.24.1-installer-linux-x86_64-for-tensorflow-r1.15.sh
./bazel-0.24.1-installer-linux-x86_64-for-tensorflow-r1.15.sh --user

#Create shared directory
if [ -f ~/.bashrc ]
then 
 if cat ~/.bashrc | grep -q "BAZEL_CANCER"
then
   echo "Bazel includes seem to be set-up.." 
else 
  USER=`whoami` 
  echo "#BAZEL_CANCER" >> ~/.bashrc
  echo "source ~/.bazel/bin/bazel-complete.bash" >> ~/.bashrc
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
git checkout r1.15


./configure

bazel clean --expunge

#Flags for bazel if you have gcc<5.0
#--cxxopt="-D_GLIBCXX_USE_CXX11_ABI=0"
#--noincompatible_do_not_split_linking_cmdline

bazel build --config=opt --config=cuda --config=mkl --config=monolithic $BazelPleaseSlowDown  //tensorflow/tools/pip_package:build_pip_package
./bazel-bin/tensorflow/tools/pip_package/build_pip_package ~/Documents/3dParty/


bazel build --config opt --config=cuda --config=monolithic  //tensorflow/tools/lib_package:libtensorflow
cp bazel-bin/tensorflow/tools/lib_package/libtensorflow.tar.gz ~/Documents/3dParty/libtensorflow-r1.15.tar.gz

echo "Please visit ~/Documents/3dParty/ to collect your tensorflow python3 wheel, and C Library.."


exit 0
