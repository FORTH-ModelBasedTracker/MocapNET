#pragma once
/** @file gestureRecognition.hpp
 *  @brief MocapNET Gesture recognition is implemented here
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include <iostream>
#include <vector>
 

/**
 * @brief This is an array of names for the input Joints expected from MocapNET.
 * Please notice that these 57 joints have actually result to three times the number of parameters since we require
 * x,y,v ( v for visibility ) information for each joint.
 * That gives us 57*3 = 171 input parameters. Values of this array can be accessed using the enumerator MOCAPNET_Uncompressed_Joints
 * For a full list of the 171 input value labels see MocapNETUncompressedArrayNames
 */ 
static const char * hardcodedGestureName[] =
{
  "help.bvh",                  //0
  "push.bvh",                  //1
  "lefthandcircle.bvh",        //2
  "righthandcircle.bvh",       //3
  "waveleft.bvh",              //4
  "doubleclap.bvh",            //5
  "waveright.bvh",             //6
  ""                           //7
  "tpose.bvh",                 //8
  "comeright.bvh",             //9
  "comeleft.bvh",              //10
  "leftkick.bvh",              //11
  "rightkick.bvh",             //12
  ""                           //13
  //hardcodedGestureNumber should be kept in sync
};


/**
 * @brief This needs to be kept in sync with hardcodedGestureName */
const unsigned int hardcodedGestureNumber=7;

/**
 * @brief  history of poses
 */
struct PoseHistory
{ 
    unsigned int maxPoseHistory;
    std::vector<std::vector<float> > history;
};


/**
 * @brief  recorded gestures that can be used
 */
struct RecordedGesture
{
    unsigned int lastActivation;
    float percentageComplete;
    char loaded;
    char label[128];
    std::vector<std::vector<float> > gesture;
    std::vector<char> usedJoints;
    void * gestureCallback;
};


/**
 * @brief  gesture detection context, to facilitate gestures
 */
struct GestureDatabase
{
    unsigned int gestureChecksPerformed;
    unsigned int numberOfLoadedGestures;
    struct RecordedGesture gesture[hardcodedGestureNumber];
    unsigned long previousGestureCheckTimestamp;    
};


int loadGestures(struct GestureDatabase * gestureDB);

int addToMotionHistory(struct PoseHistory * poseHistoryStorage,std::vector<float> pose);


int dumpMotionHistory(const char * filename,struct PoseHistory * poseHistoryStorage);


int compareHistoryWithKnownGestures(struct GestureDatabase * gestureDB,struct PoseHistory * poseHistoryStorage,float percentageForDetection,float threshold);