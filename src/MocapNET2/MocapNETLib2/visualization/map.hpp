#pragma once
/** @file map.hpp
 *  @brief Code that handles GUI output and visualization using OpenCV.
 *  If OpenCV is not be available, CMake will not declare the USE_OPENCV compilation flag and the whole code will not be included.
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>


#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv;

 int visualizeMap(
                    const char* windowName,
                    unsigned int frameNumber,
                    unsigned int width,
                    unsigned int height, 
                    struct skeletonSerialized * skeleton,
                     struct MocapNET2 * mnet,
                     std::vector<float>  result,
                    std::vector<std::vector<float> > points2DOutputGUIRealView, 
                    unsigned int numberOfMissingJoints,
                    unsigned int gestureDetected,
                    const char * gestureName,
                    unsigned int gestureFrame
                  );
#endif
