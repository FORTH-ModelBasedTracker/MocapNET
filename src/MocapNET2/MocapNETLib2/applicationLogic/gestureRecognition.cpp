#include "gestureRecognition.hpp"
#include "../mocapnet2.hpp"
#include "../tools.hpp"
#include "../IO/bvh.hpp"
#include "../IO/csvRead.hpp"
#include "poseRecognition.hpp"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

int addToMotionHistory(struct PoseHistory * poseHistoryStorage,std::vector<float> pose)
{
    poseHistoryStorage->history.push_back(pose);
    if (poseHistoryStorage->history.size() > poseHistoryStorage->maxPoseHistory)
        {
            poseHistoryStorage->history.erase(poseHistoryStorage->history.begin());
        }
    return 1;
}

int dumpMotionHistory(const char * filename,struct PoseHistory * poseHistoryStorage)
{
    return writeBVHFile(filename,0,0,poseHistoryStorage->history);
}

int updateGestureActivity(std::vector<float> vecA,std::vector<float> vecB,std::vector<char> &active,float threshold)
{
    if (vecA.size()!=vecB.size())
        {
            fprintf(stderr,"updateGestureActivity cannot compare different sized vectors..\n");
            return 0;
        }

    if (vecA.size()!=active.size())
        {
            fprintf(stderr,"updateGestureActivity cannot compare without allocated active vector..\n");
            return 0;
        }


    //Always skip X pos, Y pos, Z pos , and rotation
    for (int i=6; i<active.size(); i++)
        {
            float difference=abs(vecA[i]-vecB[i]);
            if  (difference>threshold)
                {
                    active[i]=1;
                }
        }
    return 1;
}


int automaticallyObserveActiveJointsInGesture(struct RecordedGesture * gesture)
{
    //fprintf(stderr,"automaticallyObserveActiveJointsInGesture:");
    if (gesture->gesture.size()==0)
        {
            fprintf(stderr,"Failed to automatically observe active joints\n");
            return 0;
        }

    std::vector<float> initialPose = gesture->gesture[0];

    gesture->usedJoints.clear();
    for (int jointID=0; jointID<initialPose.size(); jointID++)
        {
            gesture->usedJoints.push_back(0);
        }

    for (int gestureID=0; gestureID<gesture->gesture.size(); gestureID++)
        {
            updateGestureActivity(
                initialPose,
                gesture->gesture[gestureID],
                gesture->usedJoints,
                5
            );
        }

    //fprintf(stderr,"done\n");
    return 1;
}


int loadGestures(struct GestureDatabase * gestureDB)
{
    unsigned  int gestureID;
    char gesturePath[512]= {0};

    gestureDB->gestureChecksPerformed=0;

    for (gestureID=0; gestureID<hardcodedGestureNumber; gestureID++)
        {
            snprintf(gesturePath,512,"dataset/gestures/%s",hardcodedGestureName[gestureID]);
            fprintf(stderr,"Loading Gesture %u -> %s  : ",gestureID,gesturePath);

            gestureDB->gesture[gestureID].gesture=loadBVHFileMotionFrames(gesturePath);
            //gestureDB->gesture[gestureID].bvhMotionGesture=loadBVHFile(gesturePath);

            //if (gestureDB->gesture[gestureID].bvhMotionGesture!=0)
            if (gestureDB->gesture[gestureID].gesture.size()>0)
                {
                    fprintf(stderr,"Gesture %s has %lu frames with %lu fields each\n",gesturePath,gestureDB->gesture[gestureID].gesture.size(),gestureDB->gesture[gestureID].gesture[0].size());
                    gestureDB->gesture[gestureID].loaded=1;
                    gestureDB->gesture[gestureID].gestureCallback=0;
                    snprintf(gestureDB->gesture[gestureID].label,128,"%s",hardcodedGestureName[gestureID]);
                    gestureDB->numberOfLoadedGestures +=1;

                    //Populate
                    //gestureDB->gesture[gestureID].gesture;
                    if (automaticallyObserveActiveJointsInGesture(&gestureDB->gesture[gestureID]))
                        {
                            for (int jointID=0; jointID<gestureDB->gesture[gestureID].usedJoints.size(); jointID++)
                                {
                                    if (gestureDB->gesture[gestureID].usedJoints[jointID])
                                        {
                                            fprintf(stderr,YELLOW "Joint %u/%s is going to be used for gesture #%u\n" NORMAL,jointID,MocapNETOutputArrayNames[jointID],gestureID);
                                        }
                                }
                        }
                    else
                        {
                            fprintf(stderr,YELLOW "Failure observing active joints\n" NORMAL);
                        }

                    fprintf(stderr,GREEN "Success , loaded %lu frames\n" NORMAL,gestureDB->gesture[gestureID].gesture.size());
                }
            else
                {
                    fprintf(stderr,RED "Failure\n" NORMAL);
                }

        }

    return  (gestureDB->numberOfLoadedGestures>0);
}






int compareHistoryWithGesture(
    struct RecordedGesture * gesture,
    struct PoseHistory * poseHistoryStorage,
    unsigned int checkSerialNumber,
    float currentFramerate,
    float targetFramerate,
    float percentageForDetection,
    float threshold
)
{
    unsigned int matchingFrames=0;
    if (poseHistoryStorage->history.size()==0)
        {
            fprintf(stderr,"Requested comparison with empty history..\n");
            return 0;
        }

    if (gesture->lastActivation>checkSerialNumber)
        {
            fprintf(stderr,RED "compareHistoryWithGesture: inverted timestmaps, something is terribly wrong..\n" NORMAL);
        }
    else
        {
            if (checkSerialNumber-gesture->lastActivation<GESTURE_ACTIVATION_COOLDOWN_IN_FRAMES)
                {
                    fprintf(stderr,YELLOW "gesture %s on cooldown..\n" NORMAL,gesture->label);
                    return 0;
                }
        }


    //TODO: Compensate for different  targetFramerate / currentFramerate here..!
    //In case of the currentFramerate being larger than the targetFramerate there should be more frames  checked to increase accuracy
    //In case of the currentFramerate being smaller than the targetFramerate there should be skipped frames

    if (poseHistoryStorage->history.size()>=gesture->gesture.size())
        {
            unsigned int frameID=0;
            unsigned int historyStart=poseHistoryStorage->history.size() - gesture->gesture.size();
            //unsigned int jointNumber=poseHistoryStorage->history[0].size();
            //unsigned int jointID=0;
            for (frameID=0; frameID<gesture->gesture.size(); frameID++)
                {
                    matchingFrames+=areTwoBVHFramesCloseEnough(gesture->gesture[frameID],poseHistoryStorage->history[frameID+historyStart],gesture->usedJoints,threshold);
                }

            gesture->percentageComplete = (float) matchingFrames/gesture->gesture.size();
            float percentComplete = 100*gesture->percentageComplete;

            if (percentComplete > 100 )
                {
                    fprintf(stderr,RED);
                    fprintf(stderr,"There is something wrong in the gesture files ( size = %lu )\n ",gesture->gesture.size());
                }
            else if (percentComplete >= percentageForDetection )
                {
                    fprintf(stderr,GREEN);
                    gesture->lastActivation=checkSerialNumber;
                }
            else if (percentComplete >= 50.0 )
                {
                    fprintf(stderr,YELLOW);
                }
            fprintf(stderr,"Gesture %s - %0.2f %% %u/%lu\n" NORMAL,gesture->label,percentComplete,matchingFrames,gesture->gesture.size());

            //If we have more than 75% or whatever is the target percentage correct trigger..!
            return (percentComplete >= percentageForDetection );
        }

    return 0;
}



int compareHistoryWithKnownGestures(struct GestureDatabase * gestureDB,struct PoseHistory * poseHistoryStorage,float percentageForDetection,float threshold)
{
    unsigned long now=GetTickCountMicrosecondsMN();
    unsigned long previousRun=gestureDB->previousGestureCheckTimestamp;
    gestureDB->previousGestureCheckTimestamp = now;

    float currentFramerate = convertStartEndTimeFromMicrosecondsToFPS(previousRun,now);
    float targetFramerate=16.0;

    unsigned int gestureID=0;
    gestureDB->gestureChecksPerformed+=1;

    for (gestureID=0; gestureID<gestureDB->numberOfLoadedGestures; gestureID++)
        {
            if (
                compareHistoryWithGesture(
                    &gestureDB->gesture[gestureID],
                    poseHistoryStorage,
                    gestureDB->gestureChecksPerformed,
                    currentFramerate,
                    targetFramerate,
                    percentageForDetection,
                    threshold
                )
            )
                {
                    return 1+gestureID;
                }
        }
    return 0;
}
