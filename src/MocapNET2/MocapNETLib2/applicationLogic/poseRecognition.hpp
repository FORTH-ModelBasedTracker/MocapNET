#pragma once
/** @file poseRecognition.hpp
 *  @brief MocapNET Pose recognition is implemented here
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include <iostream>
#include <vector>


/**
 * @brief This is an array of names for the hardcoded poses included in the dataset/poses/  subdirectory
 * Please remember to update hardcodedPoseNumber, its value should be the number of arguments in this array
 * If you have arguments 0 - 10 you should set  hardcodedPoseNumber to 11
 */ 
static const char * hardcodedPoseName[] =
{
    "neutral.bvh",
    "tpose.bvh", //0
    "x.bvh",
    "handsup.bvh",
     "leftwave.bvh",
      "rightright.bvh",
      "leftleft.bvh",
      "push.bvh",
      "rightwave.bvh",  
     "rightkick.bvh",   
     "leftkick.bvh",
       ""
  //hardcodedPoseName should be kept in sync
};


/**
 * @brief This needs to be kept in sync with hardcodedPoseName */
const unsigned int hardcodedPoseNumber=9;


/**
 * @brief  recorded gestures that can be used
 */
struct RecordedPose
{
    unsigned int lastActivation;
    float percentageComplete;
    char loaded;
    char label[128];
    std::vector<float> pose;
    std::vector<char> usedJoints;
    void * poseCallback;
};


/**
 * @brief  gesture detection context, to facilitate gestures
 */
struct PoseDatabase
{
    unsigned int poseChecksPerformed;
    unsigned int numberOfLoadedPoses;
    struct RecordedPose pose[hardcodedPoseNumber];
    unsigned long previousPoseCheckTimestamp;    
};


int loadPoses(struct PoseDatabase * poseDB);

int areTwoBVHFramesCloseEnough(std::vector<float> vecA,std::vector<float> vecB,std::vector<char> active,float threshold);


int isThisPoseFamiliar(
    struct PoseDatabase * poseDB,
    std::vector<float> currentPose,
    float percentageForDetection,
    float threshold
);