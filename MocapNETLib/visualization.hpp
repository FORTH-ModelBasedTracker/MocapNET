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
  int visualizeMotionHistory(const char* windowName, std::vector<std::vector<float> > history, std::vector<std::vector<float> >  skeleton2D);
#endif 



static const int lineColorIndex[] =
{
247,252,253,
224,236,244,
191,211,230,
158,188,218,
140,150,198,
140,107,177,
136,65,157,
129,15,124,
77,0,75,
247,252,253,
229,245,249,
204,236,230,
153,216,201,
102,194,164,
65,174,118,
35,139,69,
0,109,44,
0,68,27,
247,252,240,
224,243,219,
204,235,197,
168,221,181,
123,204,196,
78,179,211,
43,140,190,
8,104,172,
8,64,129,
255,247,236,
254,232,200,
253,212,158,
253,187,132,
252,141,89,
239,101,72,
215,48,31,
179,0,0,
127,0,0,
255,247,251,
236,231,242,
208,209,230,
166,189,219,
116,169,207,
54,144,192,
5,112,176,
4,90,141,
2,56,88,
255,247,251,
236,226,240,
208,209,230,
166,189,219,
103,169,207,
54,144,192,
2,129,138,
1,108,89,
1,70,54,
247,244,249,
231,225,239,
212,185,218,
201,148,199,
223,101,176,
231,41,138,
206,18,86,
152,0,67,
103,0,31,
255,247,243,
253,224,221,
252,197,192,
250,159,181,
247,104,161,
221,52,151,
174,1,126,
122,1,119,
73,0,106,
255,255,229,
247,252,185,
217,240,163,
173,221,142,
120,198,121,
65,171,93,
35,132,67,
0,104,55,
0,69,41,
255,255,217,
237,248,177,
199,233,180,
127,205,187,
65,182,196,
29,145,192,
34,94,168,
37,52,148,
8,29,88,
255,255,229,
255,247,188,
254,227,145,
254,196,79,
254,153,41,
236,112,20,
204,76,2,
153,52,4,
102,37,6,
255,255,204,
255,237,160,
254,217,118,
254,178,76,
253,141,60,
252,78,42,
227,26,28,
189,0,38,
128,0,38
};

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
                     unsigned int gestureDetected,
                     const char * gestureName, 
                     unsigned int gestureFrame,
                     std::vector<float> mocapNETInput,
                     std::vector<float> mocapNETOutput,
                     std::vector<float> mocapNETOutputWithGUIForcedView,
                     std::vector<std::vector<float> > points2DInput,
                     std::vector<std::vector<float> > points2DOutput,
                     std::vector<std::vector<float> > points2DOutputGUIForcedView,
                     void* optionalOpenGLCVMat
                   );
