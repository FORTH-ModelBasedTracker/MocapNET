#pragma once

#include <iostream>
#include <vector>
 


/**
 * @brief This is an array of names for the input Joints expected from MocapNET.
 * Please notice that these 57 joints have actually result to three times the number of parameters since we require
 * x,y,v ( v for visibility ) information for each joint.
 * That gives us 57*3 = 171 input parameters. Values of this array can be accessed using the enumerator MOCAPNET_Uncompressed_Joints
 * For a full list of the 171 input value labels see MocapNETUncompressedArrayNames
 */
 
 
 const unsigned int hardcodedGestureNumber=12;
 
static const char * hardcodedGestureName[] =
{
  "comeleft.bvh",             //0
  "help.bvh",                       //1
  "push.bvh",                      //2
  "comeright.bvh",          //3
  "lefthandcircle.bvh",  //4
  "righthandcircle.bvh",//5
  "waveleft.bvh",             //6
  "doubleclap.bvh",          //7
  "leftkick.bvh",                //8
  "rightkick.bvh",             //9
  "waveright.bvh",          //10
  "tpose.bvh",                   //11
  ""
};


/**
 * @brief MocapNET consists of separate classes/ensembles that are invoked for particular orientations.
 * This structure holds the required tensorflow instances to make MocapNET work.
 */
struct PoseHistory
{ 
    unsigned int maxPoseHistory;
    std::vector<std::vector<float> > history;
};


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

struct GestureDatabase
{
    unsigned int gestureChecksPerformed;
    unsigned int numberOfLoadedGestures;
    struct RecordedGesture gesture[hardcodedGestureNumber];
};


int loadGestures(struct GestureDatabase * gestureDB);

int addToMotionHistory(struct PoseHistory * poseHistoryStorage,std::vector<float> pose);


int dumpMotionHistory(const char * filename,struct PoseHistory * poseHistoryStorage);


int compareHistoryWithKnownGestures(struct GestureDatabase * gestureDB,struct PoseHistory * poseHistoryStorage,float percentageForDetection,float threshold);