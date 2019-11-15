#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

ORIG_DIR=`pwd`
 
echo "Generating shortcut"
echo "[Desktop Entry]" > mocapnet.desktop
echo "Type=Application" >> mocapnet.desktop
echo "Name=MocapNET Demo" >> mocapnet.desktop
echo "Version=1.0" >> mocapnet.desktop
echo "GenericName=MocapNET" >> mocapnet.desktop
echo "Icon=$ORIG_DIR/doc/icon.png" >> mocapnet.desktop
echo "Exec=$ORIG_DIR/MocapNETLiveWebcamDemo --from /dev/video0 --live --dir \"$ORIG_DIR\"" >> mocapnet.desktop
echo "Terminal=false" >> mocapnet.desktop
echo "StartupNotify=false" >> mocapnet.desktop
echo "Categories=Application;Graphics;3DGraphics;2DGraphics;" >> mocapnet.desktop
chmod +x mocapnet.desktop


# sudo apt-get install build-essential cmake libopencv-dev libjpeg-dev libpng-dev

if [ ! -f dataset/MotionCapture/READMEFIRST.txt ]; then
cd "$DIR/dataset"
echo "Could not find MotionCapture"
wget http://ammar.gr/datasets/CMUMotionCaptureDatasets.zip
unzip CMUMotionCaptureDatasets.zip
mv CMUMotionCaptureDatasets.zip MotionCapture
fi

cd "$DIR"

#also force download a Video sample
if [ ! -f shuffle.webm ]; then
  wget http://ammar.gr/mocapnet/shuffle.webm
fi
#--------------------------------------------

if [ ! -f dataset/makehuman.tri ]; then
  cd "$DIR/dataset"
  wget http://ammar.gr/mocapnet/makehuman.tri
fi

cd "$DIR"
  



cd "$DIR/dataset"
mkdir combinedModel
cd combinedModel
mkdir mode3
mkdir mode5

LIST_OF_QUALITY="1.0 2.0" # 1.5 2.0

for QUALITY in $LIST_OF_QUALITY; do
#--------------------------------------------------------------------
echo "Downloading Models for mode 3/quality setting $QUALITY"
cd "$DIR/dataset/combinedModel/mode3"
mkdir $QUALITY
cd $QUALITY 

if [ ! -f all.pb ]; then
  wget http://ammar.gr/datasets/combinedModel/mode3/$QUALITY/all.pb
fi

if [ ! -f back.pb ]; then
  wget http://ammar.gr/datasets/combinedModel/mode3/$QUALITY/back.pb
fi

if [ ! -f front.pb ]; then
  wget http://ammar.gr/datasets/combinedModel/mode3/$QUALITY/front.pb
fi

cd ..
done
#--------------------------------------------------------------------

#Rest of combined models..
echo "Downloading 2D Joint Estimator models"
cd "$DIR/dataset/combinedModel"

if [ ! -f openpose_model.pb ]; then
  wget http://ammar.gr/datasets/combinedModel/openpose_model.pb
fi

if [ ! -f vnect_sm_pafs_8.1k.pb ]; then
  wget http://ammar.gr/datasets/combinedModel/vnect_sm_pafs_8.1k.pb
fi

if [ ! -f mobnet2_tiny_vnect_sm_1.9k.pb ]; then
  wget http://ammar.gr/datasets/combinedModel/mobnet2_tiny_vnect_sm_1.9k.pb
fi
 
cd "$DIR"





#If you have an old GPU then use older version 
TENSORFLOW_VERSION="1.14.0" # 1.12.0 for CUDA 9.0 / 1.11.0 for CUDA9 with  older compute capabilities (5.2) .. / 1.8.0 for CUDA9 and a device with compute capability 3.0  / 1.4.1 for CUDA 8 
ARCHITECTURE="gpu" #can be gpu or cpu
#https://www.tensorflow.org/install/lang_c
#https://github.com/tensorflow/tensorflow/tree/master/tensorflow/c

#if you want the latest version 
#you can download it from https://storage.googleapis.com/tensorflow-nightly/github/tensorflow/lib_package/libtensorflow-gpu-linux-x86_64.tar.gz
#however it is based on TF2.0 which is not yet tested ..!

#I have a special version of tensorflow 1.11.0 tailored for Intel Core 2 and NVIDIA 7XX cards ( compute capabilities ) that you can find here
#wget https://ammar.gr/mocapnet/libtensorflow-oldgpu-linux-x86_64-1.11.0.tar.gz


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
   #wget https://ammar.gr/mocapnet/libtensorflow-gpu-linux-x86_64-$TENSORFLOW_VERSION.tar.gz
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




cd "$DIR"
if [ -f dependencies/RGBDAcquisition/README.md ]; then
echo "RGBDAcquisition appears to already exist .."
else
 cd "$DIR/dependencies"
 git clone https://github.com/AmmarkoV/RGBDAcquisition
 cd RGBDAcquisition
 mkdir build
 cd build
 cmake ..
 #We dont need to make it 
 #make 
 cd ../opengl_acquisition_shared_library/opengl_depth_and_color_renderer
 mkdir build
 cd build
 cmake ..
 #We dont need to make it 
 #make 
 cd "$DIR"
 #Also retrieve Renderer 
 #ln -s dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/Renderer 
fi





cd "$DIR"
if [ -f dependencies/AmmarServer/README.md ]
then
echo "AmmarServer appears to already exist .."
else
  echo "Do you want to download AmmarServer and enable MocapNETServer build ? " 
  echo "(You probably don't need this)" 
  echo
  echo -n " (Y/N)?"
  read answer
  if test "$answer" != "N" -a "$answer" != "n";
  then 
      cd "$DIR/dependencies"
      git clone https://github.com/AmmarkoV/AmmarServer
      AmmarServer/scripts/get_dependencies.sh
      cd AmmarServer
      mkdir build
      cd build
      cmake ..
     make 
     cd "$DIR"
  fi
fi

 
 
 
#Now that we have everything lets build..
echo "Now to try and build MocapNET.."
cd "$DIR"
mkdir build
cd build
cmake ..
make 
cd "$DIR"
 


exit 0
