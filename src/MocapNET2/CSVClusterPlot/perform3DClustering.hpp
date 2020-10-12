#pragma once

#include <stdio.h>
#include <vector>


struct  voxelElement
{
    char element;
};


struct  clusteringOf3DPoses
{
    FILE *fp;
     struct  voxelElement * space;
     unsigned long allocatedSpaceMemorySize;
     unsigned int width,height,depth;
    unsigned long accumulatedSamples;
    unsigned long maxAccumulatedSample;
    
};




std::vector<std::vector<float> > collect3DPoses(
    struct  clusteringOf3DPoses * state,
    std::vector<char>   &activeJoints,
    struct BVH_MotionCapture * bvhMotion,
    float distance,
    unsigned int width,
    unsigned int height,
    unsigned int depth
);