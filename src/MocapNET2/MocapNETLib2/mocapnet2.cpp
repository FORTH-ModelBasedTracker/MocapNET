//MOCAPNET2 ------------------------------------
#include "../MocapNETLib2/mocapnet2.hpp"
//----------------------------------------------
#include "../MocapNETLib2/tools.hpp"
#include "../MocapNETLib2/IO/bvh.hpp"
#include "../MocapNETLib2/IO/jsonRead.hpp"
#include "../MocapNETLib2/IO/conversions.hpp"
//----------------------------------------------
#include "../MocapNETLib2/remoteExecution.hpp"
//----------------------------------------------
#include "../MocapNETLib2/core/multiThreaded.hpp"
//----------------------------------------------

#include <string.h>

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


int registerGestureEventCallbackWithMocapNET(struct MocapNET2 * mnet,void * callback)
{
    fprintf(stderr,GREEN "New Gesture Event Callback registered\n" NORMAL);
    mnet->newGestureEventCallback=callback;
    return 1;
}

void requestRealtimePriority()
{
    set_realtime_priority();
}


void commonInitialization(struct MocapNET2 * mnet)
{
    mnet->newGestureEventCallback=0;


    fprintf(stderr,"MocapNET2 Initializing..\n");

    //The following line will attempt to escalate the priority
    //to realtime, you probably need to run with sudo otherwise
    //it will silently fail.
    set_realtime_priority();

    //Artifacts logic ---------------
    if (mnet->options!=0)
    {
        fprintf(stderr,"Map will be initialized from file %s \n",mnet->options->mapFilePath);
        initializeArtifactsFromFile(&mnet->artifacts,mnet->options->mapFilePath);
    }
    fprintf(stderr," artifacts ok\n");
    //---------------------------------

    //Reset pose history..
    mnet->gesturesMasterSwitch=mnet->options->doGestureDetection;
    mnet->poseHistoryStorage.maxPoseHistory=150;
    mnet->poseHistoryStorage.history.clear();
    mnet->framesReceived=0;

    //Inverse Kinematics defaults
    mnet->learningRate=0.01;
    mnet->iterations=5;
    mnet->epochs=30;
    mnet->spring = 20;

    mnet->upperBody.perform2DAlignmentBeforeEvaluation=0;
    mnet->lowerBody.perform2DAlignmentBeforeEvaluation=0;
    //--------------------------------------------------
    mnet->leftHand.perform2DAlignmentBeforeEvaluation=1;
    mnet->rightHand.perform2DAlignmentBeforeEvaluation=1;

    snprintf(mnet->body.partName,64,"Whole Body");
    snprintf(mnet->upperBody.partName,64,"Upper Body");
    snprintf(mnet->lowerBody.partName,64,"Lower Body");
    snprintf(mnet->leftHand.partName,64,"Left Hand");
    snprintf(mnet->rightHand.partName,64,"Right Hand");

    if (mnet->gesturesMasterSwitch)
    {
      if (!loadGestures(&mnet->recognizedGestures))
        {
            fprintf(stderr,RED "Failed to read recognized Gestures\n" NORMAL);
            fprintf(stderr,RED "This is not fatal, but gestures/poses will be deactivated..\n" NORMAL);
            mnet->gesturesMasterSwitch=0;
        }

       if (!loadPoses(&mnet->recognizedPoses))
        {
            fprintf(stderr,RED "Failed to read recognized Poses\n" NORMAL);
            fprintf(stderr,RED "This is not fatal, but poses/gestures will be deactivated..\n" NORMAL);
            mnet->gesturesMasterSwitch=0;
        }
    }

    fprintf(stderr,"Initializing output filters : ");
    float filterCutoff = 5.0;
    float approximateFramerate = mnet->options->inputFramerate;
    //---------------------------------------------------
    //initButterWorth(&mnet->directionSignal,approximateFramerate,filterCutoff);

    for (int i=0;  i<MOCAPNET_ORIENTATION_NUMBER; i++ )
        {
            fprintf(stderr,".");
            initButterWorth(&mnet->upperBody.directionSignals[i],approximateFramerate,filterCutoff);
        }

    //XYZ are smoother
    for (int i=0;  i<3; i++ )
        {
            fprintf(stderr,".");
            initButterWorth(&mnet->outputSignals[i],approximateFramerate*2,filterCutoff);
        }
    //hip rotations are large and smoother
    for (int i=3;  i<6; i++ )
        {
            fprintf(stderr,".");
            initButterWorth(&mnet->outputSignals[i],approximateFramerate*2,filterCutoff);
        }

    for (int i=3;  i<MOCAPNET_OUTPUT_NUMBER; i++ )
        {
            fprintf(stderr,".");
            initButterWorth(&mnet->outputSignals[i],approximateFramerate,filterCutoff);
        }

    fprintf(stderr,"\n");
}


int loadMocapNET2(struct MocapNET2 * mnet, const char * description)
{
    commonInitialization(mnet);

    int result = 0;
    int target = 0;

    float qualitySetting = mnet->options->quality;
    int mode = mnet->options->mocapNETMode;
    unsigned int doUpperBody = mnet->options->doUpperBody;
    unsigned int doLowerBody = mnet->options->doLowerBody;
    unsigned int doFace = mnet->options->doFace;
    unsigned int doHands = mnet->options->doHands;
    unsigned int forceCPU = mnet->options->useCPUOnlyForMocapNET;

    if (doUpperBody)
        {
            result += mocapnetUpperBody_initialize(mnet,description,qualitySetting,mode,forceCPU);
            ++target;
        }

    if (doLowerBody)
        {
            result += mocapnetLowerBody_initialize(mnet,description,qualitySetting,mode,forceCPU);
            ++target;
        }

    if (doHands)
        {
            //TODO add hands
            //result += mocapnetRightHand_initialize(mnet,description,qualitySetting,mode,forceCPU);
            //result += mocapnetLeftHand_initialize(mnet,description,qualitySetting,mode,forceCPU);
            //target += 2;
        }

    //doFace

    if (target==0)
        {
            fprintf(stderr,RED "loadMocapNET2: No body pose estimation functionality was selected..\n" NORMAL);
            return 0;
        }

    return (result==target);
}




int initializeMocapNET2InputAssociation(struct MocapNET2 * mnet,struct skeletonSerialized * input,int doLowerbody,int doHands,int doFace)
{
    int results=0;
    int attempts=0;

    //==============================================

    fprintf(stderr,"Doing body associations..\n");
    results+=mocapnetBody_initializeAssociations(mnet,input);
    ++attempts;

    fprintf(stderr,"Doing upper body associations..\n");
    results+=mocapnetUpperBody_initializeAssociations(mnet,input);
    ++attempts;

    //if (doLowerbody) // always do lowerbody..
        {
            fprintf(stderr,"Doing lower body associations..\n");
            results+=mocapnetLowerBody_initializeAssociations(mnet,input);
            ++attempts;
        }

    if (doHands)
        {
            //TODO add hands
            //fprintf(stderr,"Doing lhand associations..\n");
            //results+= mocapnetLeftHand_initializeAssociations(mnet,input);
            //++attempts;

            //fprintf(stderr,"Doing rhand associations..\n");
            //results+= mocapnetRightHand_initializeAssociations(mnet,input);
            //++attempts;
        }

    //This message is confusing and removed - https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/31
    //fprintf(stderr,"TODO: do face associations..\n");

    //==============================================
    if (results==attempts)
        {
            mnet->indexesPopulated=1;
        }

    return (results==attempts);
}






std::vector<float> runMocapNET2(
                                 struct MocapNET2 * mnet,
                                 struct skeletonSerialized * input,
                                 int doLowerbody,
                                 int doHands,
                                 int doFace,
                                 int doGestureDetection,
                                 unsigned int useInverseKinematics,
                                 int doOutputFiltering
                                )
{

    std::vector<float> emptyResult;

    if (mnet==0)
        {
            fprintf(stderr,RED "MocapNET: Cannot work without initialization\n" NORMAL);
            return emptyResult;
        }
    if (input==0)
        {
            fprintf(stderr,RED "MocapNET: Cannot work without input\n" NORMAL);
            return emptyResult;
        }

    //We need indexes to be populated in order to be able to convert input to a usable vector
    if (!mnet->indexesPopulated)
        {
            if (initializeMocapNET2InputAssociation(mnet,input,doLowerbody,doHands,doFace))
                {
                    fprintf(stderr,GREEN "MocapNET adapted to source of input\n" NORMAL);
                }
            else
                {
                    fprintf(stderr,RED "MocapNET: Could not adapt to particular source of input\n" NORMAL);
                    exit(1);
                    return emptyResult;
                }
        }

    ++mnet->framesReceived;

    if (input->skeletonHeaderElements != input->skeletonBodyElements)
        {
            fprintf(stderr,RED "MocapNET: Mismatch of input Header/Body element number\n" NORMAL);
            fprintf(stderr,RED "Check your input source(!)\n" NORMAL);
            fprintf(stderr,RED "Header suggested %u elements, body has %u elements\n" NORMAL,input->skeletonHeaderElements,input->skeletonBodyElements);

            return emptyResult;
        }

    fprintf(stderr,GREEN "MocapNET: Received %u elements\n" NORMAL,input->skeletonHeaderElements);

    int useNeuralNetworkToExtractResult = ( (!mnet->options->skipNeuralNetworkIfItIsNotNeeded) || (mnet->framesReceived % mnet->options->maximumNeuralNetworkSkipFrames == 1) );

    std::vector<float> result;

    //We might not want to run the neural network based on our options
    //If we dont use the neural network we will rely on the inverse kinematics module..!
    //If we dont want to use the inverse kinematics module then we are forced to use the neural network..
    if ( (useNeuralNetworkToExtractResult) || (!useInverseKinematics) )
    {
           //If user has enabled skeleton Rotation turned on on auto ( auto = 360 ) then the skeleton will be automatically rotated..!
           //-------------------------------------------------------------
            char inputHasBeenAltered=0;
            struct skeletonSerialized pureInput;
            memcpy(&pureInput,input,sizeof(struct skeletonSerialized));
            //-----------------------------------
            if (mnet->options->skeletonRotation==360.0)
            {
              inputHasBeenAltered=1;
              makeSkeletonUpright(input); //Give MocapNET a hand with aligning the skeleton to an easier orientation
            } else
            {
              inputHasBeenAltered=1;
              affineSkeletonRotation(input,mnet->options->skeletonRotation);
            }
           //-------------------------------------------------------------


           //Run multithreaded mocapNET ( it will fallback to single threaded if multi threading is not enabled through options )
           long startTimeNeuralNetwork = GetTickCountMicrosecondsMN();
           //-----------------------------------------------------
           result = multiThreadedMocapNET(
                                            mnet,
                                            input,
                                            doLowerbody,
                                            doHands,
                                            doFace,
                                            doGestureDetection,
                                            useInverseKinematics,
                                            doOutputFiltering
                                           );
           //-----------------------------------------------------
           long endTimeNeuralNetwork = GetTickCountMicrosecondsMN();
           mnet->neuralNetworkFramerate = convertStartEndTimeFromMicrosecondsToFPS(startTimeNeuralNetwork,endTimeNeuralNetwork);

          if (inputHasBeenAltered)
          {
             //If we have altered the input to help the neural network lets restore it for the IK module..!
             memcpy(input,&pureInput,sizeof(struct skeletonSerialized));
          }
    } else
    {
        result = mnet->previousSolution;
    }

    if (result.size()>0)
        {

            if (useInverseKinematics)
                {
                    long startTimeIK = GetTickCountMicrosecondsMN();
                    unsigned int springIgnoreChanges=0;
                    result = improveBVHFrameUsingInverseKinematics(
                                 mnet->penultimateSolution,
                                 mnet->previousSolution,
                                 result,
                                 mnet->framesReceived,
                                 ( (doHands) && (mnet->leftHand.loadedModels>0) ),
                                 ( (doHands) && (mnet->rightHand.loadedModels>0)),
                                 doFace,
                                 input,
                                 mnet->learningRate,
                                 mnet->iterations,
                                 mnet->epochs,
                                 mnet->spring,
                                 springIgnoreChanges,
                                 mnet->options->doMultiThreadedIK
                             );
                    long endTimeIK = GetTickCountMicrosecondsMN();

                    mnet->inverseKinematicsFramerate = convertStartEndTimeFromMicrosecondsToFPS(startTimeIK,endTimeIK);
                }
            else
                {
                    //If we suddenly turn of IK we need to signal this in the framerate..
                    mnet->inverseKinematicsFramerate = 0.0;
                }


           //User wants to force a specific position and rotation
           //on BVH output
           if (mnet->options->forceOutputPositionRotation)
           {
             fprintf(stderr,YELLOW "forcing output position rotation \n" NORMAL);
             result[0]=mnet->options->outputPosRot[0];
             result[1]=mnet->options->outputPosRot[1];
             result[2]=mnet->options->outputPosRot[2];
             result[3]=mnet->options->outputPosRot[3];
             result[4]=mnet->options->outputPosRot[4];
             result[5]=mnet->options->outputPosRot[5];
           }


            if (doOutputFiltering)
                {
                    if (result.size()==MOCAPNET_OUTPUT_NUMBER)
                        {
                            for (int i=0;  i<result.size(); i++ )
                                {
                                    result[i] = filter(&mnet->outputSignals[i],result[i]);
                                }
                        }
                    else
                        {
                            fprintf(stderr,RED "MocapNET: Incorrect number of output elements/Cannot filter output as a result..!\n" NORMAL);
                            fprintf(stderr,RED "Result size = %lu , MocapNET output = %u \n" NORMAL,result.size(),MOCAPNET_OUTPUT_NUMBER);
                        }
                }

            addToMotionHistory(&mnet->poseHistoryStorage,result);
            //----------------------------------------------------------------------------------------------
            if (doGestureDetection)
                {
                    if (mnet->gesturesMasterSwitch)
                        {

                            int gestureDetected=compareHistoryWithKnownGestures(
                                                                              &mnet->recognizedGestures,
                                                                              &mnet->poseHistoryStorage,
                                                                              GESTURE_COMPLETION_PERCENT,//Percentage complete..
                                                                              GESTURE_ANGLE_SENSITIVITY //Angle threshold
                                                                             );
                            if (gestureDetected!=0)
                              {
                                mnet->lastActivatedGesture=gestureDetected;
                                mnet->gestureTimestamp=mnet->recognizedGestures.gestureChecksPerformed;
                                fprintf(stderr,GREEN "Gesture Detection : %u\n" NORMAL,gestureDetected);

                                if (mnet->newGestureEventCallback!=0)
                                  { //We have a callback associated..!
                                     void ( *DoCallback) (struct MocapNET2 * ,unsigned int)=0 ;
                                     DoCallback = (void(*) (struct MocapNET2 * ,unsigned int) ) mnet->newGestureEventCallback;
                                     DoCallback(mnet ,gestureDetected-1);
                                  }
                              }


                            //Check if the pose we see is familiar, based on the ones we know..
                            int poseDetected = isThisPoseFamiliar(
                                                                   &mnet->recognizedPoses,
                                                                   result,
                                                                   GESTURE_COMPLETION_PERCENT,//Percentage complete..
                                                                   34.0 //Angle threshold
                                                                 );
                            if (poseDetected!=0)
                                {
                                    if (mnet->lastActivatedPose==poseDetected)
                                        {
                                            fprintf(stderr,YELLOW "Pose is still on : %u\n" NORMAL,poseDetected);
                                            mnet->activePose=0;
                                        }
                                    else if (mnet->activePose!=poseDetected)
                                        {
                                            mnet->activePose=poseDetected;
                                            mnet->lastActivatedPose=poseDetected;
                                            fprintf(stderr,GREEN "Pose Detection : %u\n" NORMAL,poseDetected);
                                            if (mnet->newPoseEventCallback!=0)
                                                {
                                                    //We have a callback associated..!
                                                    void ( *DoCallback) (struct MocapNET2 *,unsigned int)=0 ;
                                                    DoCallback = (void(*) (struct MocapNET2 *,unsigned int) ) mnet->newPoseEventCallback;
                                                    DoCallback(mnet,poseDetected-1);
                                                }
                                        }
                                    else
                                        {
                                            fprintf(stderr,YELLOW "Pose Detection but filtered : %u\n" NORMAL,poseDetected);
                                            mnet->activePose=0;
                                        }
                                }
                            else
                                {
                                    mnet->activePose=0;
                                    mnet->lastActivatedPose=0;
                                }


                        }
                    else
                        {
                            fprintf(stderr,RED "Gesture Detection is enabled but it failed to initialize..\n" NORMAL);
                        }
                }
            //----------------------------------------------------------------------------------------------

            mnet->penultimateSolution = mnet->previousSolution;
            mnet->previousSolution = mnet->currentSolution;
            mnet->currentSolution = result;

            return result;
        }

    fprintf(stderr,RED "MocapNET: failed to retrieve a result..\n" NORMAL);
    //-----------------
    return emptyResult;
}




int unloadMocapNET2(struct MocapNET2 * mnet)
{
    if (mnet->threadPool.initialized)
   {
       if (!threadpoolDestroy(&mnet->threadPool))
       {
           fprintf(stderr,"Failed deleting IK thread pool\n");
       }
   }

    return (
             mocapnetBody_unload(mnet) &&
             mocapnetUpperBody_unload(mnet) &&
             mocapnetLowerBody_unload(mnet)
             //&& mocapnetRightHand_unload(mnet)
             //&& mocapnetLeftHand_unload(mnet)
            );
}
