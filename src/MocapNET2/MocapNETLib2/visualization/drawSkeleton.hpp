#pragma once
/** @file visualization.hpp
 *  @brief Code that handles GUI output and visualization using OpenCV.
 *  If OpenCV is not be available, CMake will not declare the USE_OPENCV compilation flag and the whole code will not be included.
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>
#include "../mocapnet2.hpp"

#include "../applicationLogic/parseCommandlineOptions.hpp"

#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv; 
#endif


#if USE_OPENCV
#if USE_BVH

int visualizeSkeletonSerialized(
                                cv::Mat &outputMat,
                                struct skeletonSerialized * skeleton, 
                                unsigned int drawLeftFingers,
                                unsigned int drawRightFingers, 
                                unsigned int drawFace,
                                unsigned int x,unsigned int y,
                                unsigned int width,unsigned int height
                               );

  int drawSkeleton(cv::Mat &outputMat,std::vector<std::vector<float> > points2DOutputGUIForcedView,float offsetX,float offsetY,int labels);
#endif
#endif
