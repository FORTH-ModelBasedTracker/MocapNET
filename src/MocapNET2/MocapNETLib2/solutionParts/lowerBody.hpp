#pragma once
/** @file lowerBody.hpp
 *  @brief Code that  handles getting 2D joints packed in their skeletonSerialized and vector formats and can run the tensorflow code retrieving a 3D BVH motion frame that estimates the human lower body
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include "../mocapnet2.hpp"
 
/**
 * @brief Since the C/C++ code in this repository depends on a seperate python trainer, and the networks can differ depending on training parameters (and this is a research project) the series of 2D joints
 * can often change. It is also very expensive to do string matching on every frame, so before evaluating any input this call has to be executed in order to perform the correct array associations and from then on
 * we can pass 2D data without searching for labels on each frame. It needs to be called before any mocapnetLowerBody_evaluateInput call
 * @param MocapNET instance
 * @param a skeletonSerialized structure that holds our 2D input
 * @retval 1=Success/0=Failure
 */ 
int mocapnetLowerBody_initializeAssociations(struct MocapNET2 * mnet, struct skeletonSerialized * input);

/**
 * @brief This call loads and initializes the required Tensorflow models for the specific configuration requested, it needs to be called before any mocapnetLowerBody_evaluateInput call
 * @param MocapNET instance
 * @param This parameter is currently omitted
 * @param MocapNET 1 supported multiple quality settings, however for MocapNET2 you should default to 1.0
 * @param MocapNET 1 used 3 ensembles, MocapNET 2 uses 5 ensembles, so mode should default to 5 
 * @param The network can be executed on the GPU or the CPU, if you supply 1 you will force CPU execution, if not MocapNET will try to run it on your GPU ( if tensorflow finds it )  
 * @retval 1=Success/0=Failure
 */ 
int mocapnetLowerBody_initialize(struct MocapNET2 * mnet,const char * filename,float qualitySetting,unsigned int mode,unsigned int forceCPU);

/**
 * @brief This call deallocates the tensorflow models
 * @param MocapNET instance
 * @retval 1=Success/0=Failure
 */ 
int mocapnetLowerBody_unload(struct MocapNET2 * mnet);

/**
 * @brief This call inserts the 3D pose extracted from tensorflow into the final resulting BVH vector
 * @param The final BVH vector that we want to populate with lower body data
 * @param The lower body result
 * @retval 1=Success/0=Failure
 */ 
int mocapnetLowerBody_fillResultVector(std::vector<float> &finalResultVector,std::vector<float> resultBody);

/**
 * @brief This call converts 2D input that is formatted on a skeletonSerialized structure to a 3D pose vector. You need to call mocapnetLowerBody_initializeAssociations and mocapnetLowerBody_initialize before calling this function 
 * and if you want to convert the output result to the final result vector you need to use the mocapnetLowerBody_fillResultVector call.
 * @param MocapNET instance
 * @param a skeletonSerialized structure that holds our 2D input 
 * correct ensemble for the perceived orientation.
 * @retval 3D pose output that needs to be processed through mocapnetLowerBody_fillResultVector to fill the final BVH buffer
 */ 
std::vector<float> mocapnetLowerBody_evaluateInput(struct MocapNET2 * mnet,struct skeletonSerialized * input);
