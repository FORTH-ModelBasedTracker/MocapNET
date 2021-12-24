#include "rightHandSym.hpp"

#include "../../MocapNETLib2/config.h"
#include "../../MocapNETLib2/IO/bvh.hpp"
#include "../../MocapNETLib2/IO/conversions.hpp"
#include "../../MocapNETLib2/core/core.hpp"
#include "../../MocapNETLib2/visualization/drawHands.hpp"
#include "../NSDM/generated_lhand.hpp"
#include "../tools.hpp"

#include "../../../../dependencies/RGBDAcquisition/tools/AmMatrix/quaternions.h"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


std::vector<float> horizontalFlipPositionalInput(std::vector<float> positionalInput)
{
    std::vector<float> result;
    for (unsigned int i=0; i<positionalInput.size()/3; i++)
    {
        float x = positionalInput[i*3+0];
        float y = positionalInput[i*3+1];
        float v = positionalInput[i*3+2];
        if (v>0.0)
          {  
            result.push_back(1.0-x);
          } else
          {
            result.push_back(x); 
          }
         result.push_back(y); 
         result.push_back(v); 
    }
   return result;
}

#define DO_QUATERNION_FLIP_FOR_SYMMETRY 0

std::vector<float> flipLHandToRhand(std::vector<float> bvh)
{
    std::vector<float> result;
    if (bvh.size()>7)
    {
    //--------------------------------
    result.push_back(-1.0* bvh[0]); // X
    result.push_back(bvh[1]);       // Y
    result.push_back(bvh[2]);       // Z
    //--------------------------------
     
    #if DO_QUATERNION_FLIP_FOR_SYMMETRY
    //-------------------------------- 
     float quat[4]={bvh[3],bvh[4],bvh[5],bvh[6]};
     float euler[3]={0.0,0.0,0.0};
     quaternions2Euler(euler,quat,qWqXqYqZ);
    
     //flip Y rotation..!
     euler[1]=-1.0*euler[1];
    
     euler2Quaternions(quat,euler,qWqXqYqZ);
      result.push_back(quat[0]);    // qW
      result.push_back(quat[1]);    // qX
      result.push_back(quat[2]);    // qY
      result.push_back(quat[3]);    // qZ 
    //-------------------------------- 
    #else
    //-------------------------------- 
      result.push_back(bvh[3]);     // qW
      result.push_back(bvh[4]);     // qX
      result.push_back(bvh[5]);     // qY
      result.push_back(bvh[6]);     // qZ 
    #endif
    //-------------------------------- 
    
    //-------------------------------- 

    for (unsigned int i=7; i<bvh.size(); i++)
    {
        result.push_back(-1.0*bvh[i]);
    }
        
    }
    return result;
}


std::vector<float> mocapnetRightHandSym_evaluateInput(struct MocapNET2 * mnet,struct skeletonSerialized * input,float scaleInput)
{
    std::vector<float> result;
    mnet->rightHand.positionalInput = deriveMocapNET2InputUsingAssociations(
                                                                              mnet,
                                                                              input,
                                                                              &mnet->rhandSelectionIndexesArePopulated,
                                                                              mnet->rhandSelectionIndexes,
                                                                              mocapNET_InputLength_WithoutNSDM_rhand,
                                                                              mocapNET_rhand,
                                                                              0 // Enable to debug input
                                                                             );
                                                                             
    if (mnet->rightHand.positionalInput.size()==0)
    {
        return result;
    }
    
    std::vector<float> modifiedPositionalInput  = horizontalFlipPositionalInput(mnet->rightHand.positionalInput);
    float angleToRotate = 0.0;
    
    if  ( modifiedPositionalInput.size()!=mnet->rightHand.positionalInput.size())
    {
        fprintf(stderr,RED "horizontalFlipPositionalInput produced erroneous output %lu vs %lu\n" NORMAL,modifiedPositionalInput.size(),mnet->rightHand.positionalInput.size());
        exit(1);
    }
    
    if (scaleInput!=1.0)
                    { 
                      fprintf(stderr,YELLOW "RHand 2D Input scaled *= %0.2f \n" NORMAL,scaleInput);
                     for (unsigned int i=0; i<modifiedPositionalInput.size(); i++)
                       {    
                        modifiedPositionalInput[i] *= scaleInput; 
                       }
                    }
    
    
    int eNSRM = (!mnet->options->regularNSRMMatrix);
    if (eNSRM) //(mnet->rightHand.perform2DAlignmentBeforeEvaluation)
    {
      unsigned int pivotJoint = 0; //R Hand
      unsigned int referenceJoint = 4 ; //finger3-1.r
    
      //Force alignment of middle finger to make it easier on the neural network
      angleToRotate = getAngleToAlignToZero(modifiedPositionalInput,pivotJoint,referenceJoint);                                                                            
      fprintf(stderr,YELLOW "Correcting right hand by rotating it %0.2f degrees\n" NORMAL,angleToRotate);
      rotate2DPointsBasedOnJointAsCenter(modifiedPositionalInput,angleToRotate,0);
    }
    
                                                                             
      mnet->rightHand.NSDM  = rhandCreateNDSM(modifiedPositionalInput,angleToRotate,0/*NSDM Positional*/,1/*NSDM Angular */,0 /*Use Normalization*/,eNSRM); 
      mnet->rightHand.neuralNetworkReadyInput.clear(); 
      mnet->rightHand.neuralNetworkReadyInput.insert(mnet->rightHand.neuralNetworkReadyInput.end(),modifiedPositionalInput.begin(),modifiedPositionalInput.end());
      mnet->rightHand.neuralNetworkReadyInput.insert(mnet->rightHand.neuralNetworkReadyInput.end(),mnet->rightHand.NSDM.begin(), mnet->rightHand.NSDM.end());


    //---------------------------------------------------------------------------------------------- 
    //The code block above should have done everything needed to make a  vector with the number of elements required
    //If input is not correct then we should stop right here
    //----------------------------------------------------------------------------------------------
    if (mnet->rightHand.neuralNetworkReadyInput.size()!=MNET_RHAND_IN_NUMBER)
        {
            fprintf(stderr,RED "mocapnetRightHand: Incorrect size of MocapNET input (expected %lu got %u ) .. \n" NORMAL,mnet->rightHand.neuralNetworkReadyInput.size(),MNET_RHAND_IN_NUMBER);
            std::vector<float> emptyResult; 
            return emptyResult;
        }


    if (
         vectorcmp(
                    mnet->rightHand.lastNeuralNetworkReadyInput,
                    mnet->rightHand.neuralNetworkReadyInput,
                    0.01
                  )!=0
       )
    {
        //Special stuff requried for special case
        
        
        
    //First run the network that can predict the orientation of the skeleton so we will use the correct
    //network to get the full BVH result from it 
    //----------------------------------------------------------------------------------------------
    
    std::vector<float> flippedResult; 
    flippedResult = localExecution(
                            &mnet->leftHand, //<---- WE USE THE LEFT HAND TO REGRESS THE FLIPPED POSITION
                            mnet->rightHand.neuralNetworkReadyInput,
                            mnet->orientation,
                            0 /*Right hand has no orientation or orientation trick*/
                           ); 
    //----------------------------------------------------------------------------------------------
    
    //We got a Left Hand here..!
    //We need to flip it to a right one..!
    result = flipLHandToRhand(flippedResult);
    
    
    mnet->rightHand.result = result;
    mnet->rightHand.lastNeuralNetworkReadyInput = mnet->rightHand.neuralNetworkReadyInput;
     
         
         
    return result; 
    } else
    {
      fprintf(stderr,GREEN "Nothing changed on right hand, returning previous result.. \n" NORMAL);
      return mnet->rightHand.result;
    }
}

