#pragma once


#include "opencv2/opencv.hpp"
using namespace cv;

 
int getBestCropWindow(
    int maximumCrop,
    unsigned int * x,
    unsigned int * y,
    unsigned int * width,
    unsigned int * height,
    struct boundingBox * bbox,
    unsigned int inputWidth2DJointDetector,
    unsigned int inputHeight2DJointDetector,
    unsigned int fullFrameWidth,
    unsigned int fullFrameHeight
);
