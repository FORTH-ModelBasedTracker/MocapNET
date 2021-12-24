#pragma once


#include <vector>

#include "../IO/skeletonAbstraction.hpp"
#include "opencv2/opencv.hpp"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"

using namespace cv;

void drawLineIfPointsExist2(cv::Mat &outputMat,cv::Point start,cv::Point finish,cv::Scalar color,float thickness);

void drawCircleIfPointsExist2(cv::Mat &outputMat,cv::Point point,float radious,cv::Scalar color,float thickness);


void drawHand(const char * windowName,struct BVH_MotionCapture * bvhMotion,std::vector<std::vector<float> > points2D,const std::vector<float> bvhFrame,cv::Mat * retreiveImage);

void draw3DLhand(const std::vector<float> bvhFrame);
void draw3DRhand(const std::vector<float> bvhFrame);


void draw2DHand(const std::vector<float> points2D);


void draw2DHandSk(struct skeletonSerialized * skeleton);