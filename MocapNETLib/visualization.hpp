/** @file visualization.h
 *  @brief Code that handles GUI output and visualization using OpenCV.
 *  If OpenCV is not be available, CMake will not declare the USE_OPENCV compilation flag and the whole code will not be included.
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>


#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv; 
  int visualizeCameraIntensities(const char* windowName,cv::Mat &imgOriginal,int forceColors);
  int visualizeCameraChannels(const char* windowName,cv::Mat &img,int channelNumber);
  int visualizeCameraEdges(const char* windowName,cv::Mat &img);
  int visualizeCameraFeatures(const char* windowName,cv::Mat &img);
  int visualizeFigure(const char* windowName,cv::Mat &img);
#endif 



/**
 * @brief Visualize MocapNET BVH output on a window
 * @ingroup visualization
 * @param CString with title of window
 * @param Current frame number
 * @param Framerate of Acquisition
 * @param Framerate of 2D Joint estimator
 * @param Framerate of MocapNET 3D Pose estimator
 * @param Output window width
 * @param Output window height
 * @param MocapNET output BVH frame that we want to visualize
 * @retval 1 = Success loading the files  , 0 = Failure
 */
int visualizePoints(
                     const char* windowName,
                     unsigned int frameNumber,
                     unsigned int skippedFrames,
                     signed int totalNumberOfFrames,
                     unsigned int numberOfFramesToGrab,
                     const char * CPUName,
                     const char * GPUName,                              
                     int drawFloor,
                     int drawNSDM, 
                     float fpsTotal,
                     float fpsAcquisition,
                     float joint2DEstimator,
                     float fpsMocapNET,
                     unsigned int width,
                     unsigned int height,
                     unsigned int handleMessages,
                     std::vector<float> mocapNETInput,
                     std::vector<float> mocapNETOutput,
                     std::vector<float> mocapNETOutputWithGUIForcedView,
                     std::vector<std::vector<float> > points2DInput,
                     std::vector<std::vector<float> > points2DOutput,
                     std::vector<std::vector<float> > points2DOutputGUIForcedView,
                     void* optionalOpenGLCVMat
                   );
