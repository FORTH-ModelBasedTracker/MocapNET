#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"


if [ -f dependencies/RGBDAcquisition/README.md ]; then
cd dependencies/RGBDAcquisition 
git pull origin master
cd "$DIR"
else
echo "Could not find RGBDAcquisition, please rerun the initialize.sh script .." 
fi



if [ -f dependencies/AmmarServer/README.md ]; then
echo "AmmarServer appears to exist, updating it .."
cd dependencies/AmmarServer 
git pull origin master
cd "$DIR"
fi


#Now sync rest of code
cd "$DIR"
git pull origin master
  

exit 0
