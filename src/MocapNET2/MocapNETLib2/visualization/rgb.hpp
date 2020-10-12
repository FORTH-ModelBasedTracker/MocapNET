#pragma once
/** @file rgb.hpp
 *  @brief Code that handles GUI output and visualization using OpenCV.
 *  If OpenCV is not be available, CMake will not declare the USE_OPENCV compilation flag and the whole code will not be included.
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv; 

int visualizeCameraIntensities(const char* windowName, cv::Mat &imgOriginal,int forceColors);
int visualizeCameraChannels(const char* windowName,cv::Mat &img,int channelNumber);
int visualizeCameraEdges(const char* windowName,cv::Mat &img);
int visualizeCameraFeatures(const char* windowName,cv::Mat &img);
int visualizeFigure(const char* windowName,cv::Mat &img);
#endif