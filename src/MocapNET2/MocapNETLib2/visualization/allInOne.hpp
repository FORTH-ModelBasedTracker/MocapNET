#pragma once
/** @file allInOne.hpp
 *  @brief Code that handles GUI output and visualization using OpenCV.
 *  If OpenCV is not be available, CMake will not declare the USE_OPENCV compilation flag and the whole code will not be included.
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>


#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv;

int visualizeAllInOne(
    const char* windowName,
    unsigned int frameNumber,
    unsigned int saveVisualization,
    cv::Mat * alreadyLoadedImage,
    const char * path,
    const char * label,
    unsigned int serialLength,
    unsigned int width,
    unsigned int height,
    struct skeletonSerialized * skeleton,
    struct MocapNET2 * mnet,
    struct MocapNET2Options * options,
    std::vector<std::vector<float> > points2DOutputGUIRealView,
    std::vector<std::vector<float> > points2DOutputGUIForcedView, 
    unsigned int numberOfMissingJoints
);


#endif
