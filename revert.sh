#!/bin/bash

#Reminder , to get a single file out of the repo use  "git checkout -- path/to/file.c"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

#All OpenGL/BVH code relies on RGBDAcquisition code so we need to sync this as well
cd RGBDAcquisition
git reset --hard HEAD
git pull origin master

#Now sync rest of code
cd "$DIR"
git reset --hard HEAD
git pull origin master
  
exit 0
