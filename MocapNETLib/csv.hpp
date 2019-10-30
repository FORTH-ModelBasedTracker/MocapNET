#pragma once
/** @file csv.hpp
 *  @brief These are some simple functions to facilitate CSV writing
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include "../MocapNETLib/jsonCocoSkeleton.h"

/**
 * @brief Write a skeleton to a CSV. This is the first call that prepares the header 
 * @param Path to output file i.e. "output.bvh"
 * @param Pointer to Skeleton that we want to save 
 * @param Width of Image where the skeleton was retreieved from
 * @param Height of Image where the skeleton was retreieved from
 * @retval 1=Success,0=Failure
 */
int writeCSVHeader(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height);


/**
 * @brief Write the body of a skeleton to a CSV.   This call has 
 * @param Path to output file i.e. "output.bvh"
 * @param Pointer to Skeleton that we want to save 
 * @param Width of Image where the skeleton was retreieved from
 * @param Height of Image where the skeleton was retreieved from
 * @retval 1=Success,0=Failure
 */
int writeCSVBody(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height);
 