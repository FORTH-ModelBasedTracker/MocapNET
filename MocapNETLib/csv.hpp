#pragma once


#include "../MocapNETLib/jsonCocoSkeleton.h"

int writeCSVHeader(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height);

int writeCSVBody(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height);
 