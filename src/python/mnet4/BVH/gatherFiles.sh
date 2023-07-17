#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

REPO="/home/ammar/Documents/Programming/RGBDAcquisition"



ITEM="tools/AmMatrix"
cd "$DIR"
mkdir -p $ITEM
cd $ITEM
cp $REPO/$ITEM/matrixTools* ./
cp $REPO/$ITEM/matrix3x3Tools* ./
cp $REPO/$ITEM/matrix4x4Tools* ./
cp $REPO/$ITEM/matrixCalculations* ./
cp $REPO/$ITEM/matrixOpenGL* ./
cp $REPO/$ITEM/quaternions* ./
cp $REPO/$ITEM/simpleRenderer* ./
cp $REPO/$ITEM/solveLinearSystemGJ* ./
cp $REPO/$ITEM/solveHomography* ./

ITEM="tools/PThreadWorkerPool"
cd "$DIR"
mkdir -p $ITEM
cd $ITEM
cd ..
cp -R $REPO/$ITEM/ ./


ITEM="opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Applications/BVHTester"
cd "$DIR"
mkdir -p $ITEM
cd $ITEM
cp $REPO/$ITEM/bvhConverter.py ./
cp $REPO/$ITEM/bvhLibrary.py ./
cp $REPO/$ITEM/bvhLibrary.h ./
cp $REPO/$ITEM/main.c ./
cd "$DIR"
cp $REPO/$ITEM/bvhLibrary.py ./

#Also copy the two most important files in root 
cd "$DIR"
cp $REPO/$ITEM/bvhConverter.py ./
cp $REPO/$ITEM/bvhLibrary.py ./

ITEM="opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader"
cd "$DIR"
mkdir -p $ITEM
cd $ITEM
cd ..
cp -R $REPO/$ITEM/ ./

ITEM="opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader"
cd "$DIR"
mkdir -p $ITEM
cd $ITEM
cd ..
cp -R $REPO/$ITEM/ ./

ITEM="opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser"
cd "$DIR"
mkdir -p $ITEM
cd $ITEM
cp $REPO/$ITEM/InputParser_C.* ./
cp $REPO/$ITEM/TrajectoryParserDataStructures.* ./
cp $REPO/$ITEM/TrajectoryParser* ./
cp $REPO/$ITEM/hashmap* ./


exit 0
