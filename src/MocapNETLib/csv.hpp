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
int writeCSVHeaderFromSkeleton(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height);


/**
 * @brief Write the body of a skeleton to a CSV.   Each time this call is repeated an extra line is appended
 * @param Path to output file i.e. "output.bvh"
 * @param Pointer to Skeleton that we want to save 
 * @param Width of Image where the skeleton was retreieved from
 * @param Height of Image where the skeleton was retreieved from
 * @retval 1=Success,0=Failure
 */
int writeCSVBodyFromSkeleton(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height);
 
 
 
  
 
 /**
 * @brief Write an array of C-Strings
 * @param Path to output file i.e. "output.bvh"
 * @param Pointer to array of C-Strings i.e. MocapNETInputUncompressedArrayNames
 * @param Number of elements of labels array, MOCAPNET_UNCOMPRESSED_JOINT_PARTS*3 in the case of MocapNETInputUncompressedArrayNames
 * @retval 1=Success,0=Failure
 */ 
int writeCSVHeaderFromVector(const char * filename,const char ** labels,unsigned int numberOfLabels);



/**
 * @brief Write the body of a skeleton in the form of std::vectors to a CSV.   Each time this call is repeated an extra line is appended 
 * @param Path to output file i.e. "output.bvh"
 * @param Pointer to Skeleton that we want to save 
 * @param Width of Image where the skeleton was retreieved from
 * @param Height of Image where the skeleton was retreieved from
 * @retval 1=Success,0=Failure
 */
int writeCSVBodyFromVector(const char * filename,std::vector<float> inputValues);


int writeCSVHeaderFromLabelsAndVectorOfVectors(const char * filename,const char ** labels,unsigned int numberOfLabels,std::vector<std::vector<float> > inputFrames);