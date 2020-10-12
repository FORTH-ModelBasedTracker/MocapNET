#pragma once
/** @file gestureRecognition.hpp
 *  @brief MocapNET Gesture recognition is implemented here
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include <iostream>
#include <vector>
 

/**
 * @brief This is an array of names for the hardcoded gestures included in the dataset/gestures/  subdirectory
 * Please remember to update hardcodedPoseNumber, its value should be the number of arguments in this array
 * If you have arguments 0 - 10 you should set  hardcodedGestureNumber to 11
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
  "leftkick.bvh",              //8
  "rightkick.bvh",             //9
  "tpose.bvh",                           //10
  "handsup.bvh",                  //7
  "",                 //
  ""                           //13
  //hardcodedGestureNumber should be kept in sync
};


/**
 * @brief This needs to be kept in sync with hardcodedGestureName */
const unsigned int hardcodedGestureNumber=10;


/**
 * @brief Gesture tuning controls for the brave.. 
 **/
const unsigned int GESTURE_ACTIVATION_COOLDOWN_IN_FRAMES=50;
const float GESTURE_COMPLETION_PERCENT=80.0;
const float GESTURE_ANGLE_SENSITIVITY=25.0;


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