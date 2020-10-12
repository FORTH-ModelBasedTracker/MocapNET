#pragma once
/** @file widgets.hpp
 *  @brief Code that handles GUI output and visualization using OpenCV.
 *  If OpenCV is not be available, CMake will not declare the USE_OPENCV compilation flag and the whole code will not be included.
 *  @author Ammar Qammaz (AmmarkoV)
 */

#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv;

cv::Mat overlay(cv::Mat base,cv::Mat overlay);

int drawScale(cv::Mat &outputMat,const char * description,float x,float y,float value,float minimum,float maximum);

int plotFloatVector(
                     cv::Mat & img,
                     char cleanBackground,
                     std::vector<float> history,
                     unsigned int x,
                     unsigned int y,
                     unsigned int width,
                     unsigned int height
                   );
                   
                   
int visualizeOrientation(
    cv::Mat &img,
    const char * label,
    float orientationDegrees,
    float frontClass,
    float backClass,
    float leftClass,
    float rightClass,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
);



int visualizeNSDM(
    cv::Mat &img,
    const char * label,
    std::vector<float> NSDM,
    unsigned int channelsPerNSDMElement,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
);



int visualizeNSDMAsBar(
    cv::Mat &img,
    std::vector<float> NSDM,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
);


int drawFloorFromPrimitives(
    cv::Mat &img,
    float roll,
    float pitch,
    float yaw,
    unsigned int floorDimension
);
#endif