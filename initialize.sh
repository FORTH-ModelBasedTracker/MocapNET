#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

ORIG_DIR=`pwd`
  
ASK_QUESTIONS=1
if [[ $* == *--collab* ]]
then 
 echo "Using non-blocking collab mode"
 ASK_QUESTIONS=0
fi


if [ "$ASK_QUESTIONS" -eq "0" ]; then
   echo "We assume collab machines have everything we need.."
else
#Simple dependency checker that will apt-get stuff if something is missing
# sudo apt-get install build-essential cmake libopencv-dev libjpeg-dev libpng-dev libglew-dev libpthread-stubs0-dev
SYSTEM_DEPENDENCIES="wget git build-essential cmake libopencv-dev libjpeg-dev libpng-dev libglew-dev libpthread-stubs0-dev"
#------------------------------------------------------------------------------
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
fi 




cd "$DIR"
if [ -f dependencies/RGBDAcquisition/README.md ]; then
 echo "RGBDAcquisition appears to already exist .."
 #cd "$DIR/dependencies/RGBAcquisition"
 #echo "We can make sure that it is up to date to avoid issues like https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/48.."
 #echo "However someone might not want to update the code.. what to do.."
 #git pull
 #cd "$DIR"
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
cd src/python/mnet4
rm -rf BVH/
ln -s ../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_re
nderer/src/Applications/BVHTester/ BVH
cd BVH
./makeLibrary.sh

cd ..
#Install rest of python stuff..
./setup.sh


 
 
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
