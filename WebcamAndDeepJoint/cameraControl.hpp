#pragma once


#include "opencv2/opencv.hpp"
using namespace cv;

/**
 * @brief A structure to hold a bounding box
 */
struct boundingBox
{
   char populated;
   float minimumX;
   float maximumX;
   float minimumY;
   float maximumY;
};


int populateBoundingBox(
                        struct boundingBox *bbox,
                        std::vector<cv::Point_<float> > points
                       );

int getBestCropWindow(
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