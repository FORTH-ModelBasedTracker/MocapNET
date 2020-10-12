#pragma once
/** @file jsonCocoSkeleton.hpp
 *  @brief This is the code needed to parse an OpenPose JSON file to our struct skeletonStructure. This  JSON parser barely works for the specific JSON output and so should be treated with caution.
 * It is not comformant to the JSON spec nor will it work for an arbitrary JSON file..! 
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include <stdio.h>

#include "commonSkeleton.hpp"

/**
 * @brief Parse a JSON file and retrieve a skeleton
 * @param Path to JSON file
 * @param Pointer to a struct skeletonStructure that will hold the information loaded
 * @param Threshold to set a joint to active ( 0.4-0.5 is a good value )
 * @retval 1=Success/0=Failure
 */ 
int parseJsonCOCOSkeleton(const char * filename , struct skeletonStructure * skel,float acceptableThreshold,unsigned int frameID);
