#include "core.hpp"

#include "../../../Tensorflow/tf_utils.hpp"

#include "../config.h"

#include <string.h>

//MOCAPNET2 ------------------------------------
#include "../../MocapNETLib2/mocapnet2.hpp"
#include "../../MocapNETLib2/tools.hpp"
#include "../../MocapNETLib2/IO/bvh.hpp"
#include "../../MocapNETLib2/IO/jsonRead.hpp"
#include "../../MocapNETLib2/IO/conversions.hpp"
//----------------------------------------------
#include "../../MocapNETLib2/remoteExecution.hpp"
//----------------------------------------------
#include "../../MocapNETLib2/solutionParts/body.hpp"
#include "../../MocapNETLib2/solutionParts/upperBody.hpp"
#include "../../MocapNETLib2/solutionParts/lowerBody.hpp"
//----------------------------------------------
#include "../../MocapNETLib2/core/singleThreaded.hpp"
#include "../../MocapNETLib2/core/multiThreaded.hpp"
//----------------------------------------------

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


float undoOrientationTrickForBackOrientation(float orientation)
{
    if (orientation==180)
        {
            orientation=0;
        }
    else if (orientation<0)
        {
            orientation=180-orientation;
        }
    else
        {
            orientation=180-orientation;
        }
    return orientation;
}


int getMocapNETOrientationFromAngle(float direction)
{
    float orientation = direction;

    if ( (FRONT_MIN_ORIENTATION<=orientation) && (orientation<=FRONT_MAX_ORIENTATION) )
        {
            return MOCAPNET_ORIENTATION_FRONT;
        }
    else if ( (RIGHT_MIN_ORIENTATION<=orientation) && (orientation<=RIGHT_MAX_ORIENTATION) )
        {
            // To visualy inspect these orienteations use :
            // ./BVHGUI2 --from dataset/headerWithHeadAndOneMotion.bvh --set 4 135
            // ./BVHGUI2 --from dataset/headerWithHeadAndOneMotion.bvh --set 4 45
            #if SWAP_LEFT_RIGHT_ENSEMBLES
             fprintf(stderr,YELLOW "Swapped 45<=range<=135 to left ensemble\n" NORMAL);
             return MOCAPNET_ORIENTATION_LEFT;
            #else
             return MOCAPNET_ORIENTATION_RIGHT;
            #endif
        }
    else if ( (LEFT_MIN_ORIENTATION<=orientation) && (orientation<=LEFT_MAX_ORIENTATION) )
        {
            // To visualy inspect these orienteations use :
            // ./BVHGUI2 --from dataset/headerWithHeadAndOneMotion.bvh --set 4 -135
            // ./BVHGUI2 --from dataset/headerWithHeadAndOneMotion.bvh --set 4 -45
            #if SWAP_LEFT_RIGHT_ENSEMBLES
             fprintf(stderr,YELLOW "Swapped -135<=range<=-45 to right ensemble\n" NORMAL);
             return MOCAPNET_ORIENTATION_RIGHT;
            #else
             return MOCAPNET_ORIENTATION_LEFT;
            #endif
        }
    else if ( (BACK_ALT_MIN_ORIENTATION<=orientation) && (orientation<=BACK_ALT_MAX_ORIENTATION) )
        {
            return MOCAPNET_ORIENTATION_BACK;
        }
    else if ( (BACK_MIN_ORIENTATION<=orientation) && (orientation<=BACK_MAX_ORIENTATION) )
        {
            return MOCAPNET_ORIENTATION_BACK;
        }
    else
        {
            fprintf(stderr,RED "[Unhandled orientation]\n" NORMAL);
            fprintf(stderr,"This probably means difficult input\n");
            return MOCAPNET_ORIENTATION_BACK;
        }
    fprintf(stderr,RED "Empty Direction Vector\n" NORMAL);
    return MOCAPNET_ORIENTATION_NONE;
}



int  getMocapNETOrientationFromOutputVector(std::vector<float> direction)
{
    if (direction.size()>0)
        {
            return getMocapNETOrientationFromAngle(direction[0]);
        }
    fprintf(stderr,RED "Empty Direction Vector\n" NORMAL);
    return MOCAPNET_ORIENTATION_NONE;
}


int  getMocapNETOrientationFromProbabilities(float frontProb,float backProb,float leftProb,float rightProb)
{
    if ((frontProb>backProb) && (frontProb>leftProb) && (frontProb>rightProb))
        {
            return MOCAPNET_ORIENTATION_FRONT;
        }
    else if ((backProb>frontProb) && (backProb>leftProb) && (backProb>rightProb))
        {
            return MOCAPNET_ORIENTATION_BACK;
        }
    else if ((leftProb>frontProb) && (leftProb>backProb) && (leftProb>rightProb))
        {
            return MOCAPNET_ORIENTATION_LEFT;
        }
    else if ((rightProb>frontProb) && (rightProb>backProb) && (rightProb>leftProb))
        {
            return MOCAPNET_ORIENTATION_RIGHT;
        }

    fprintf(stderr,RED "Inconclusive Direction Classification\n" NORMAL);
    return MOCAPNET_ORIENTATION_NONE;
}


std::vector<float>  MNET3Classes(struct MocapNET2SolutionPart * mnet,std::vector<float> mnetInput,int orientation)
{
    std::vector<float> result;

    if (orientation!=MOCAPNET_ORIENTATION_NONE)
        {
            fprintf(stderr,NORMAL "Orientation is : %d " NORMAL,orientation );
            //===========================================================
            if ( orientation==MOCAPNET_ORIENTATION_BACK )
                {
                    //Back ----------------------------------------------=
                    fprintf(stderr,"Back\n");
                    result = predictTensorflow(&mnet->models[2],mnetInput);
                    if (result.size()>4)
                        {
                            result[4]=undoOrientationTrickForBackOrientation(result[4]);
                        }
                }
            else
                //===========================================================
                {
                    //Front ----------------------------------------------
                    fprintf(stderr,"Front\n");
                    result = predictTensorflow(&mnet->models[1],mnetInput);
                }
            //===========================================================
        }
    return result;
}

std::vector<float>  MNET5Classes(struct MocapNET2SolutionPart * mnet,std::vector<float> mnetInput,int orientation,int targetHasOrientationTrick)
{
    std::vector<float> result;
    int targetModel=0;

    if (orientation!=MOCAPNET_ORIENTATION_NONE)
        {
            fprintf(stderr, "%s  ",mnet->partName);
            switch (orientation)
                {
                //===========================================================
                case MOCAPNET_ORIENTATION_FRONT:
                    fprintf(stderr,GREEN "Front " NORMAL);
                    targetModel=1;
                    if (mnet->loadedModels==4)
                        {
                            targetModel-=1;
                        }
                    result = predictTensorflow(&mnet->models[targetModel],mnetInput);

                    /*
                    if ( (targetHasOrientationTrick) && (result.size()>4) )
                        {
                            fprintf(stderr,"Orientation changed from %0.2f ",result[4]);
                            result[4]=-1.0 * result[4];
                            fprintf(stderr,"to %0.2f",result[4]);
                        }
                    */
                    break;
                //===========================================================
                case MOCAPNET_ORIENTATION_BACK:
                    fprintf(stderr,GREEN "Back " NORMAL);
                    targetModel=2;
                    if (mnet->loadedModels==4)
                        {
                            targetModel-=1;
                        }
                    result = predictTensorflow(&mnet->models[targetModel],mnetInput);

                    if ( (targetHasOrientationTrick) && (result.size()>4) )
                        {
                            fprintf(stderr,"Orientation changed from %0.2f ",result[4]);
                              result[4]=undoOrientationTrickForBackOrientation(result[4]);
                            //result[4]-=180.0;
                            fprintf(stderr,"to %0.2f",result[4]);
                        }
                    break;
                //===========================================================
                case MOCAPNET_ORIENTATION_LEFT:
                    fprintf(stderr,GREEN "Left " NORMAL);
                    targetModel=3;
                    if (mnet->loadedModels==4)
                        {
                            targetModel-=1;
                        }
                    result = predictTensorflow(&mnet->models[targetModel],mnetInput);

                    if ( (targetHasOrientationTrick) && (result.size()>4) )
                        {
                            fprintf(stderr,"Orientation changed from %0.2f ",result[4]);
                            result[4]+=90.0;
                            fprintf(stderr,"to %0.2f",result[4]);
                        }
                    break;
                //===========================================================
                case MOCAPNET_ORIENTATION_RIGHT:
                    fprintf(stderr,GREEN "Right " NORMAL);
                    targetModel=4;
                    if (mnet->loadedModels==4)
                        {
                            targetModel-=1;
                        }
                    result = predictTensorflow(&mnet->models[targetModel],mnetInput);

                    if ( (targetHasOrientationTrick) && (result.size()>4) )
                        {
                            fprintf(stderr,"Orientation changed from %0.2f ",result[4]);
                            result[4]+=-90.0;
                            fprintf(stderr,"to %0.2f",result[4]);
                        }
                    break;
                //===========================================================
                default :
                    fprintf(stderr,RED "Unhandled orientation, using front as a last resort \n" NORMAL);
                    result = predictTensorflow(&mnet->models[1],mnetInput);
                    break;
                    //===========================================================
                };
        }
    else
        {
            fprintf(stderr,NORMAL "5Class  Direction is not defined\n" NORMAL );
        }

    fprintf(stderr,"\n");
    return result;
}



int localOrientationExtraction(struct MocapNET2SolutionPart * mnet,std::vector<float> mnetInput)
{
    if (mnet->loadedModels!=5)
        {
            fprintf(stderr,YELLOW "%s does not have an orientation classifier..\n" NORMAL, mnet->partName);
            return  MOCAPNET_ORIENTATION_NONE;
        }
    else
        {
            fprintf(stderr,GREEN "%s has an orientation classifier..\n" NORMAL, mnet->partName);
        }

    std::vector<float> direction = predictTensorflow(&mnet->models[0],mnetInput);

    if (direction.size()>=4)
        {
            fprintf(stderr,NORMAL "Orientation : Front(%0.2f)/Back(%0.2f)/Left(%0.2f)/Right(%0.2f)\n" NORMAL,direction[0],direction[1],direction[2],direction[3]);
            //Output of each Neural Network is -45.0  to 0.0 to 45.0
            //We need to correct it ..

            //Apply smoothing to probabilities !
            direction[0] = filter(&mnet->directionSignals[0],direction[0]);
            direction[1] = filter(&mnet->directionSignals[1],direction[1]);
            direction[2] = filter(&mnet->directionSignals[2],direction[2]);
            direction[3] = filter(&mnet->directionSignals[3],direction[3]);

            //Cut negative values..
            if (direction[0]<0.0) { direction[0]=0.0; }
            if (direction[1]<0.0) { direction[1]=0.0; }
            if (direction[2]<0.0) { direction[2]=0.0; }
            if (direction[3]<0.0) { direction[3]=0.0; }

            mnet->hasOrientationScan=1;
            mnet->orientationClassifications[0]=direction[0];
            mnet->orientationClassifications[1]=direction[1];
            mnet->orientationClassifications[2]=direction[2];
            mnet->orientationClassifications[3]=direction[3];

            //int orientation = getMocapNETOrientationFromOutputVector(direction);
            return getMocapNETOrientationFromProbabilities(direction[0],direction[1],direction[2],direction[3]);
        }
    else
        {
            fprintf(stderr,RED "No Orientation Received ( direction vector -> %lu elements ) ..\n" NORMAL,direction.size());
        }
    return MOCAPNET_ORIENTATION_NONE;
}


std::vector<float> localExecution(struct MocapNET2SolutionPart * mnet,std::vector<float> mnetInput,int orientation,int targetHasOrientationTrick)
{
    std::vector<float> result;



    //Don't run empty data-------------------------------------------------
    unsigned int emptyInputElements=0;
    for (unsigned int i=0; i<mnetInput.size(); i++)
    {
      if (mnetInput[i]==0.0) { ++emptyInputElements; }
    }

    if ( (mnetInput.size()==0) || (emptyInputElements == mnetInput.size()) )
    {
      fprintf(stderr,"Won't execute dead input..\n");
      return result;
    }
    //----------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    if (orientation!=MOCAPNET_ORIENTATION_NONE)
        {
            switch(mnet->mode)
                {
                case 3:
                    result=MNET3Classes(mnet,mnetInput,orientation);
                    break;
                case 5:
                    result=MNET5Classes(mnet,mnetInput,orientation,targetHasOrientationTrick);
                    break;
                //-----------------------------------------------------------
                default:
                    fprintf(stderr,RED "MocapNET: Incorrect Mode %u for part %s ..\n" NORMAL,mnet->mode,mnet->partName);
                    break;
                };
        }
    else
        {
            fprintf(stderr,RED "Unable to predict pose orientation..\n" NORMAL);
        }
    return result;
}
