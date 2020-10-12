#!/bin/bash 


DATASET=""
if (( $#<1 ))
then 
 echo "Please provide arguments first argument is dataset "
 exit 1
else
 DATASET=$1-data
fi
# $1 holds the video file we want to process ( it should be something like path/to/videofile.mp4 )
#DATASET now holds the output directory we will create ( it should be something like path/to/videofile.mp4-data/ )


#Remember the directory where user started
STARTDIR=`pwd`

#Remember the directory where the script is ( and where MocapNET is :) )
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#Please give the path to openpose here..
OPENPOSE_DIR="/home/ammar/Documents/3dParty/openpose/"
OPENPOSE_BINARY_DIR="/home/ammar/Documents/3dParty/openpose/build/examples/openpose/"
 
#Dataset dumping using ffmpeg
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#Create the new dataset directory
mkdir $DATASET

#Dump the video files using our naming scheme
ffmpeg -i $1  -r 30 -q:v 1  $DATASET/colorFrame_0_%05d.jpg

#Make sure we start at frame 0
cp $DATASET/colorFrame_0_00001.jpg $DATASET/colorFrame_0_00000.jpg

#We now want to grab an absolute path to our dataset
cd $DATASET
FULL_PATH_TO_DATASET=`pwd`
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------



#2D pose estimation using OpenPose
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
cd $OPENPOSE_DIR
LD_LIBRARY_PATH=/usr/local/cuda-9.0/lib64 $OPENPOSE_BINARY_DIR/openpose.bin -number_people_max 1 --hand --face --write_json $FULL_PATH_TO_DATASET  -image_dir $FULL_PATH_TO_DATASET  $@
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------



#2D pose conversion to CSV using our tool
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
cd "$DIR"
cd ..

./convertOpenPoseJSONToCSV  --from $DATASET
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------



#3D pose estimation using MocapNET2
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
cd "$DIR"
cd ..

./MocapNET2CSV --from $FULL_PATH_TO_DATASET/2dJoints_v1.4.csv --mt --show 3 --save
cp out.bvh $FULL_PATH_TO_DATASET/predicted.bvh
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------





cd $STARTDIR 
exit 0

