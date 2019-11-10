#pragma once
/** @file mocapnet.hpp
 *  @brief The MocapNET C library
 *  As seen in https://www.youtube.com/watch?v=fH5e-KMBvM0 , the MocapNET network requires two types of input.
 *  The first is an uncompressed list of (x,y,v) joints and the second an NSDM array. To add to those the output consists of BVH
 *  frames that must be accompanied by a header. This library internally handles all of these details.
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include "../Tensorflow/tensorflow.hpp"
#include "../MocapNETLib/jsonCocoSkeleton.h"

#include <iostream>
#include <vector>

 



/**
 * @brief 2D Skeletons Retreieved
 */
struct Skeletons2DDetected
{
   unsigned int numberOfSkeletonsDetected;
   struct skeletonCOCO skeletons[16];
};



/**
 * @brief MocapNET consists of separate classes/ensembles that are invoked for particular orientations.
 * This structure holds the required tensorflow instances to make MocapNET work.
 */
struct JointEstimator2D
{
   struct TensorflowInstance network;
};


/**
 * @brief This is a list of included Joint 2D Estimators included.
 */
enum JOINT_2D_ESTIMATOR_SELECTED
{
 JOINT_2D_ESTIMATOR_NONE=0,
 JOINT_2D_ESTIMATOR_FORTH,
 JOINT_2D_ESTIMATOR_VNECT,
 JOINT_2D_ESTIMATOR_OPENPOSE, 
 //----------------------------- 
 JOINT_2D_ESTIMATOR_NUMBER
};


/**
 * @brief Load a 2D Joint Estimator from a .pb file on disk
 * @ingroup jointestimator
 * @param Pointer to a struct JointEstimator2D that will hold the tensorflow instances on load.
 * @param Quality setting, can currently be 1.0 ( highest quality ), 1.5  or 2.0 ( highest performance )
 * @param Flag that controls the use of PAFs 
 * @param Force the usage of CPU for MocapNET ( should be 1 as MocapNET is designed for CPU while GPU handles 2D ) 
 * @retval 1 = Success loading the files  , 0 = Failure
 */
int loadJointEstimator2D(struct JointEstimator2D * jnet,int joint2DEstimatorSelected,int usePAFs,unsigned int forceCPU);


int unloadJointEstimator2D(struct JointEstimator2D * jnet);


int  estimate2DSkeletonsFromImage(struct JointEstimator2D * jnet,struct Skeletons2DDetected * result,char * rgbData,unsigned int width,unsigned int height);