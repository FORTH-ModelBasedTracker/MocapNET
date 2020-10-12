#pragma once

#include <stdio.h>
#include <vector>

struct applicationState
{
    int rotation;
    int selectedJoint,previousSelection;
    int visualizationType,previousVisualizationType;
    int stop;
    int save;
    int redraw;
//-----------------
    unsigned long * accumulatedImage;
    unsigned long accumulatedSamples;
    unsigned long maxAccumulatedSample;
};


std::vector<std::vector<float> > collectPoses(
    struct applicationState * state,
    std::vector<char>   &activeJoints,
    struct BVH_MotionCapture * bvhMotion,
    float distance,
    unsigned int width,
    unsigned int height
);