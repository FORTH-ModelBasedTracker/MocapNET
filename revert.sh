#!/bin/bash

#Reminder , to get a single file out of the repo use  "git checkout -- path/to/file.c"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

#All OpenGL/BVH code relies on RGBDAcquisition code so we need to sync this as well


if [ -f dependencies/RGBDAcquisition/README.md ]; then
echo "RGBDAcquisition appears to exist, reverting it .."
cd dependencies/RGBDAcquisition
git reset --hard HEAD
git pull origin master
cd "$DIR"
else
echo "Could not find RGBDAcquisition, please rerun the initialize.sh script .." 
fi



if [ -f dependencies/AmmarServer/README.md ]; then
echo "AmmarServer appears to exist, reverting it .."
cd dependencies/AmmarServer
git reset --hard HEAD
git pull origin master
cd "$DIR"
fi


#Now sync rest of code
cd "$DIR"
git reset --hard HEAD
git pull origin master
  
exit 0
