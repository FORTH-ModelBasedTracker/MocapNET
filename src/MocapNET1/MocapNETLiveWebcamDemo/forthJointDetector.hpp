#ifndef FORTH_JOINT_DETECTOR_H_INCLUDED
#define FORTH_JOINT_DETECTOR_H_INCLUDED

#include <iostream>
#include <opencv2/opencv.hpp>

#include "jsonCocoSkeleton.h"


int forthJointDetectorStart(const char * model , const char * inputLayer , const char * outputLayer);

void populateSkeletonStructFromDeepJoint(std::vector<cv::Point> skeletons, struct skeletonCOCO * skel);

std::vector<cv::Point> getNeuralNetworkDetectionsForColorImage(
                                                                unsigned char * colorFrame ,
                                                                unsigned int colorWidth,
                                                                unsigned int colorHeight
                                                              );

#endif
