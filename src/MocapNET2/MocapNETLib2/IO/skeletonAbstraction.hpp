#pragma once


#include "commonSkeleton.hpp"
#include <vector>



int areFeetMissing(struct skeletonSerialized * skel);


int isLeftHardExtended(std::vector<float> result);
int isRightHardExtended(std::vector<float> result);


int  getPointOrientation(std::vector<float> result,float *x, float *y,float *r);