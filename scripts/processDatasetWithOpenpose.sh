#!/bin/bash

#This script should be run after you have dumped a video to a directory using the dump_video.sh utility
#You should give a full path to this utility i.e. ./processDatasetWithOpenpose.sh "~/myDatasets/yourvideo.mp4-data/" 
#and also dont forget to change the PATH_TO_OPENPOSE to the directory that has your openpose.bin!
PATH_TO_OPENPOSE="PLEASE/CHANGE/THIS/"


STARTDIR=`pwd`
#Switch to this directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


DATASET=""

if (( $#<1 ))
then 
 echo "Please provide arguments first argument is dataset "
 exit 1
else
 DATASET=$1

$PATH_TO_OPENPOSE/openpose.bin -number_people_max 1 --hand --face  --write_json $DATASET  -image_dir $DATASET  $@


cd "$DIR"
echo "Went back to $DIR"
echo "Hopefully the path you have given is an absolute path.."
./convertBody25JSONToCSV -i $DATASET

exit 0

