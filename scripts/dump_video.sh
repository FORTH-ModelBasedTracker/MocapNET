#!/bin/bash 

#This is a script to dump a video file to JPG files
#it uses ffmpeg so make sure you have sudo apt-get install ffmpeg
#If you do ./dump_video.sh yourvideo.mp4 
#There should be an output folder yourvideo.mp4-data that has all the frames of the video
#you can then point OpenPose to this directory to convert them to JSON 2D detections 

STARTDIR=`pwd`
#Switch to this directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

DATASET=""

if (( $#<1 ))
then 
 echo "Please provide arguments first argument is dataset "
 exit 1
else
 DATASET=$1-data
fi
 
THEDATETAG=`date +"%y-%m-%d_%H-%M-%S"` 

mkdir $DATASET

ffmpeg -i $1  -r 30 -q:v 1  $DATASET/colorFrame_0_%05d.jpg

cp $DATASET/colorFrame_0_00001.jpg $DATASET/colorFrame_0_00000.jpg

cd $DATASET
cd ..

cd $STARTDIR 
exit 0

