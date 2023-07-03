#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

ORIG_DIR=`pwd`
  
ASK_QUESTIONS=0

cd "$DIR"


#Default Tensorflow to be downloaded is 2.x with CPU only stuff to improve compatibility
TENSORFLOW_VERSION="2.3.1"
ARCHITECTURE="cpu" #can be gpu or cpu
#https://www.tensorflow.org/install/lang_c
#https://github.com/tensorflow/tensorflow/tree/master/tensorflow/c


#Tensorflow 2.3.1 works well with CUDA 10 and cudnn-10.0-linux-x64-v7.6.5.32.tgz
#wget https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-gpu-linux-x86_64-2.3.1.tar.gz

#if you want the latest version 
#you can download it from https://storage.googleapis.com/tensorflow-nightly/github/tensorflow/lib_package/libtensorflow-gpu-linux-x86_64.tar.gz

#==========================================================================
#==========================================================================
#==========================================================================
clear
  echo "         Do you want to use your GPU in Tensorflow ? " 
  echo "If you select Y a GPU-enabled version will be downloaded " 
  echo "If you don't have a CUDA-enabled GPU its best to select N" 
  echo "GPU execution is mainly imporant for the RGB->2D neural networks" 
  echo
  echo -n " (Y/N)?"

  #Only ask if we can answer
  #_____________________________
  if [ "$ASK_QUESTIONS" -eq "0" ]; then
   answer="Y"
   else
   read answer
  fi 
  #_____________________________

  if test "$answer" != "N" -a "$answer" != "n";
  then  
     ARCHITECTURE="gpu"
  fi
#==========================================================================
#==========================================================================
#==========================================================================
#clear
#  echo "         Do you want to use Tensorflow 1.x instead of 2.x ? " 
#  echo "The project is compatible with both but if you have an older GPU it might be better for you " 
#  echo "to stick with Tensorflow 1.x " 
#  echo
#  echo -n " (Y/N)?"

  #Only ask if we can answer
  #_____________________________
#  if [ "$ASK_QUESTIONS" -eq "0" ]; then
#   answer="N"
#   else
#   read answer
#  fi 
  #_____________________________

#  if test "$answer" != "N" -a "$answer" != "n";
#  then  
#    TENSORFLOW_VERSION="1.14.0" # 1.12.0 for CUDA 9.0 / 1.11.0 for CUDA9 with  older compute capabilities (5.2) .. / 1.8.0 for CUDA9 and a device with compute capability 3.0  / 1.4.1 for CUDA 8 
#  fi
#==========================================================================
#==========================================================================
#==========================================================================
echo "Selected Tensorflow version $ARCHITECTURE/$TENSORFLOW_VERSION"


#I have a special version of tensorflow 1.11.0 tailored for Intel Core 2 and NVIDIA 7XX cards ( compute capabilities ) that you can find here
#wget http://ammar.gr/mocapnet/libtensorflow-oldgpu-linux-x86_64-1.11.0.tar.gz

#I have a special version of tensorflow 1.15.2 built for i7 950 CPUs without later AVX instrucitons but CUDA 10.0 compute capability 3.5 + GPUs
#wget http://ammar.gr/mocapnet/libtensorflow-1-15.2_CPUi7_970_CUDA10.tar.gz

cd "$DIR"
if [ -f /usr/local/lib/libtensorflow.so ]; then
 echo "Found a system wide tensorflow installation, not altering anything"
elif [ -f dependencies/libtensorflow/lib/libtensorflow.so ]; then
 echo "Found a local tensorflow installation, not altering anything"
else 
 echo "Did not find tensorflow already installed..!"
 if [ ! -f dependencies/libtensorflow-$ARCHITECTURE-linux-x86_64-$TENSORFLOW_VERSION.tar.gz ]; then
   echo "Did not find tensorflow tarball so will have to download it..!"
   cd "$DIR/dependencies"
   wget https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-$ARCHITECTURE-linux-x86_64-$TENSORFLOW_VERSION.tar.gz
   #Is the Google link down ? we have a mirror :)
   #wget http://ammar.gr/mocapnet/libtensorflow-gpu-linux-x86_64-$TENSORFLOW_VERSION.tar.gz
 else
   echo "The tensorflow tarball was already downloaded.."
 fi
 

cd "$DIR"
if [ -f dependencies/libtensorflow-$ARCHITECTURE-linux-x86_64-$TENSORFLOW_VERSION.tar.gz ]; then
 #Doing a local installation that requires no SUDO 
 cd "$DIR/dependencies"
 mkdir libtensorflow
 tar -C libtensorflow -xzf libtensorflow-$ARCHITECTURE-linux-x86_64-$TENSORFLOW_VERSION.tar.gz  
 #echo "Please give me sudo permissions to install Tensorflow $TENSORFLOW_VERSION C Bindings.."
 #sudo tar -C /usr/local -xzf libtensorflow-gpu-linux-x86_64-$TENSORFLOW_VERSION.tar.gz
 else
   echo "Failed to download/extract tensorflow.."
fi

fi
#---------------------------------------------------------------------------------------------------------------------------
