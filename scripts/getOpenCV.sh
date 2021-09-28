#/bin/bash
 

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR/../dependencies"
 
DEPENDENCIES_PATH=`pwd`

sudo apt-get install build-essential cmake git libgtk2.0-dev pkg-config ffmpeg libavcodec-dev libavformat-dev libavcodec-dev libavformat-dev libswscale-dev libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff5-dev  libdc1394-22-dev libeigen3-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev sphinx-common libtbb-dev yasm libfaac-dev libopencore-amrnb-dev libopencore-amrwb-dev libopenexr-dev libgstreamer-plugins-base1.0-dev libavutil-dev libavfilter-dev libavresample-dev libpython3-all-dev python3-numpy python3-dev 

#cd "$1"
echo "Will Install @ `pwd`"


#Want a different version?
#Check https://opencv.org/releases/ for versions and 
#change the OPENCV_VERSION in the next line..
OPENCV_VERSION="3.2.0"
echo "Downloading"

wget http://ammar.gr/programs/opencv-$OPENCV_VERSION.zip
wget http://ammar.gr/programs/opencv_contrib-$OPENCV_VERSION.tar.gz

#wget https://codeload.github.com/opencv/opencv/zip/$OPENCV_VERSION -O opencv-$OPENCV_VERSION.zip
#wget https://codeload.github.com/opencv/opencv_contrib/zip/$OPENCV_VERSION -O opencv_contrib-$OPENCV_VERSION.tar.gz

echo "Extracting"

#tar xvzf opencv_contrib-$OPENCV_VERSION.tar.gz
unzip opencv_contrib-$OPENCV_VERSION.zip
unzip opencv-$OPENCV_VERSION.zip

echo "Building"

cd opencv-$OPENCV_VERSION
mkdir build
cd build
cmake -DOPENCV_ENABLE_NONFREE=ON -DOPENCV_EXTRA_MODULES_PATH=$DEPENDENCIES_PATH/opencv_contrib-$OPENCV_VERSION/modules ..
make -j5

echo "Do you want to install OpenCV to your system ? " 
echo
echo -n " (Y/N)?"
read answer
if test "$answer" != "N" -a "$answer" != "n";
 then
   sudo make install
fi

cd "$DIR"

echo "Done"

exit 0
