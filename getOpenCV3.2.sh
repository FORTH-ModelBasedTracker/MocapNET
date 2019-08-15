#/bin/bash
 

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"
 
sudo apt-get install build-essential cmake git libgtk2.0-dev pkg-config ffmpeg libavcodec-dev libavformat-dev libavcodec-dev libavformat-dev libswscale-dev libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff5-dev  libdc1394-22-dev libeigen3-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev sphinx-common libtbb-dev yasm libfaac-dev libopencore-amrnb-dev libopencore-amrwb-dev libopenexr-dev libgstreamer-plugins-base1.0-dev libavutil-dev libavfilter-dev libavresample-dev libpython3-all-dev python3-numpy python3-dev 

#cd "$1"
echo "Will Install @ `pwd`"

echo "Downloading"

wget http://ammar.gr/programs/opencv-3.2.0.zip
wget http://ammar.gr/programs/opencv_contrib-3.2.0.tar.gz

echo "Extracting"

tar xvzf opencv_contrib-3.2.0.tar.gz
unzip opencv-3.2.0.zip

echo "Building"

cd opencv-3.2.0
mkdir build
cd build
cmake -DOPENCV_ENABLE_NONFREE=ON -DOPENCV_EXTRA_MODULES_PATH=$DIR/opencv_contrib-3.2.0/modules ..
make -j5

echo "Done"

exit 0
