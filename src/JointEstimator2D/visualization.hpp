#pragma once

#include "opencv2/opencv.hpp"
/** @file webcam.cpp
 *  @brief This is a simple test file to make sure your camera or video files can be opened using OpenCV
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include <stdio.h>
#include <vector>
#include "jointEstimator2D.hpp"

#include "../MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp"

using namespace cv;

int visualizeHeatmaps(struct JointEstimator2D * jointEstimator,std::vector<std::vector<float> >  heatmapNNOutput,unsigned int frameNumber);

void dj_drawExtractedSkeletons(
    cv::Mat img,
    struct Skeletons2DDetected * sk,
    float factorX,
    float factorY
);



int cropAndResizeCVMatToMatchSkeleton(
    struct JointEstimator2D * jest,
    cv::Mat  & frame,
    struct Skeletons2DDetected * sk
);
