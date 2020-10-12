#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

ORIG_DIR=`pwd`
 


#Simple dependency checker that will apt-get stuff if something is missing
# sudo apt-get install build-essential cmake libopencv-dev libjpeg-dev libpng-dev libglew-dev libpthread-stubs0-dev
SYSTEM_DEPENDENCIES="build-essential cmake libopencv-dev libjpeg-dev libpng-dev libglew-dev libpthread-stubs0-dev"

for REQUIRED_PKG in $SYSTEM_DEPENDENCIES
do
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
echo "Checking for $REQUIRED_PKG: $PKG_OK"
if [ "" = "$PKG_OK" ]; then

  echo "No $REQUIRED_PKG. Setting up $REQUIRED_PKG."

  #If this is uncommented then only packages that are missing will get prompted..
  #sudo apt-get --yes install $REQUIRED_PKG

  #if this is uncommented then if one package is missing then all missing packages are immediately installed..
  sudo apt-get install $SYSTEM_DEPENDENCIES  
  break
fi
done
#------------------------------------------------------------------------------




#We generate a Linux desktop shortcut to easily start the live demo
echo "Generating shortcut"
echo "[Desktop Entry]" > mocapnet.desktop
echo "Type=Application" >> mocapnet.desktop
echo "Name=MocapNET Demo" >> mocapnet.desktop
echo "Version=1.0" >> mocapnet.desktop
echo "GenericName=MocapNET" >> mocapnet.desktop
echo "Icon=$ORIG_DIR/doc/icon.png" >> mocapnet.desktop
echo "Exec=$ORIG_DIR/MocapNET2LiveWebcamDemo --from /dev/video0 --live --dir \"$ORIG_DIR\"" >> mocapnet.desktop
echo "Terminal=false" >> mocapnet.desktop
echo "StartupNotify=false" >> mocapnet.desktop
echo "Categories=Application;Graphics;3DGraphics;2DGraphics;" >> mocapnet.desktop
chmod +x mocapnet.desktop





cd "$DIR"
if [ -f dataset/MotionCapture/READMEFIRST.txt ]
then
echo "CMU BVH datasets appear to have been downloaded.."
else
  echo "         Do you want to download the CMU BVH datasets ? " 
  echo "The download is approximately 1GB and will take 4GB of disk space " 
  echo "(You probably don't need this if you dont want to use the GenerateGroundTruth utility)" 
  echo
  echo -n " (Y/N)?"
  read answer
  if test "$answer" != "N" -a "$answer" != "n";
  then 
     cd "$DIR/dataset"
     echo "Could not find MotionCapture"
     
     #This is a richer armature that also contains provisons for head and feet animation 
     wget http://ammar.gr/datasets/CMUPlusHeadMotionCapture.zip
     unzip CMUPlusHeadMotionCapture.zip
     mv CMUPlusHeadMotionCapture.zip MotionCapture 

     cd "$DIR"
  fi
fi




cd "$DIR"
#Force download of a Video sample
if [ ! -f shuffle.webm ]; then
  wget http://ammar.gr/mocapnet/shuffle.webm
fi
#--------------------------------------------

if [ ! -f dataset/makehuman.tri ]; then
  cd "$DIR/dataset"
  #TRI is the internal 3D format used by my 3D renderer to handle 3D meshes
  #https://github.com/AmmarkoV/RGBDAcquisition/blob/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader_tri.h
  wget http://ammar.gr/mocapnet/makehuman.tri
  
  #Also provide the OpenCollada file in case someone wants to create their own .tri by `sudo apt-get install libassimp-dev` and then compiling and using the project 
  # https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/submodules/Assimp
  #that you will find in $ROOT_DIR/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/submodules/Assimp/
  #./assimpTester --convert $ROOT_DIR/dataset/makehuman.dae $ROOT_DIR/dataset/makehuman.tri --paint 123 123 123   
  #This dae file has been created usign makehuman(http://www.makehumancommunity.org/) and the CMU+Face Rig (http://www.makehumancommunity.org/content/cmu_plus_face.html)
  wget http://ammar.gr/mocapnet/makehuman.dae
fi

cd "$DIR"




cd "$DIR/dataset"
mkdir -p combinedModel/mocapnet2/mode5/1.0/ 
cd "$DIR/combinedModel/mocapnet2/mode5/1.0/"


#New ICPR pretrained networks 

LIST_OF_NETWORKS="categorize_lowerbody_all.pb lowerbody_left.pb upperbody_left.pb categorize_upperbody_all.pb lowerbody_right.pb upperbody_right.pb lowerbody_back.pb upperbody_back.pb lowerbody_front.pb upperbody_front.pb"

for NETWORK in $LIST_OF_NETWORKS; do
if [ ! -f $NETWORK  ]; then
  wget http://ammar.gr/datasets/icpr2020/$NETWORK 
fi
done




#--------------------------------------------------------------------
cd "$DIR/combinedModel"
#--------------------------------------------------------------------

#We also downloar pre-trained models for the 2D joint estimation
#We have 3D flavours available, openpose, vnect and our own 2D detector
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




#This webserver stuff is really not needed, and just adds complexity to everything so it is disabled
#cd "$DIR"
#if [ -f dependencies/AmmarServer/README.md ]
#then
#echo "AmmarServer appears to already exist .."
#else
#  echo "Do you want to download AmmarServer and enable MocapNETServer build ? " 
#  echo "(You probably don't need this)" 
#  echo
#  echo -n " (Y/N)?"
#  read answer
#  if test "$answer" != "N" -a "$answer" != "n";
#  then 
#      cd "$DIR/dependencies"
#      git clone https://github.com/AmmarkoV/AmmarServer
#      AmmarServer/scripts/get_dependencies.sh
#      cd AmmarServer
#      mkdir build
#      cd build
#      cmake ..
#     make 
#     cd "$DIR"
#  fi
#fi

 
 
 
#Now that we have everything lets build..
echo "Now to try and build MocapNET.."
cd "$DIR"

#if there is an already existing build directory and you called initialize.sh
#it will get initialized as well.. (https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/19)
if [ -d "build" ]; then
  echo "Build directory already exists, but since the initialize.sh script was called "    
  echo "this means the user wants to also initialize the build directory, so doing this"    
  echo "to prevent problems like https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/19"   
  rm -rf build/
fi

mkdir build
cd build
cmake ..
make 
cd "$DIR"
 


exit 0
