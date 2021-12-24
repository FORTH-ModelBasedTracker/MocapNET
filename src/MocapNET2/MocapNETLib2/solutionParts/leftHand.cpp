#include "leftHand.hpp"

#include "../../MocapNETLib2/config.h"
#include "../../MocapNETLib2/IO/bvh.hpp"
#include "../../MocapNETLib2/IO/conversions.hpp"
#include "../../MocapNETLib2/core/core.hpp"
#include "../../MocapNETLib2/visualization/drawHands.hpp"
#include "../NSDM/generated_lhand.hpp"
#include "../tools.hpp"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

int mocapnetLeftHand_initializeAssociations(struct MocapNET2 * mnet, struct skeletonSerialized * input)
{
   return  initializeAssociationsForSubsetOfSkeleton(
                                                      &mnet->lhandSelectionIndexesArePopulated,
                                                      mnet->lhandSelectionIndexes,
                                                      mocapNET_InputLength_WithoutNSDM_lhand,
                                                      mocapNET_lhand,
                                                      input
                                                     );
}



int mocapnetLeftHand_initialize(struct MocapNET2 * mnet,const char * filename,float qualitySetting,unsigned int mode,unsigned int forceCPU)
{
    char modelPath[1024]= {0};
    int result = 0;
    
    unsigned int NUMBER_OF_INPUTS = MNET_LHAND_IN_NUMBER;
    unsigned int NUMBER_OF_OUTPUTS = MOCAPNET_LHAND_OUTPUT_NUMBER;
    
    //Force mode 1 for now..
    mode=1;

    switch (mode)
        {
        case 1:
              mnet->leftHand.mode=1;
              //snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lhand_half_all.pb",mode,qualitySetting);
              snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lhand_all.pb",mode,qualitySetting);
              //mnet->leftHand.models[0].model.inputIsHalfFloats=1;
              //mnet->leftHand.models[0].model.outputIsHalfFloats=1;
              
              result += neuralNetworkLoad(&mnet->leftHand.models[0],modelPath,"input_all","result_all/concat",NUMBER_OF_INPUTS,MOCAPNET_LHAND_OUTPUT_NUMBER,forceCPU);
              mnet->leftHand.loadedModels=result;  
            break;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        case 3:
            mnet->leftHand.mode=3;
              fprintf(stderr,RED "Fatal: Mode 3 No longer supported on MocapNET 2 \n" NORMAL);
              exit(0); 
            break;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        case 5:
            mnet->leftHand.mode=5; 
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //                                                  Regular 3 Model setup such as the BMVC 2019 Work..
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            /*snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/categorize_body.pb",mode,qualitySetting);
            result += neuralNetworkLoad(&mnet->leftHand.models[0]  ,modelPath  ,"input_all"  ,"result_all/concat",forceCPU);
            mnet->leftHand.modelLimits[0].isFlipped=0;
            mnet->leftHand.modelLimits[0].numberOfLimits=1;
            mnet->leftHand.modelLimits[0].minimumYaw1=-360.0;
            mnet->leftHand.modelLimits[0].maximumYaw1=360.0;*/

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lhand_front.pb",mode,qualitySetting);
            result += neuralNetworkLoad(&mnet->leftHand.models[0],modelPath,"input_front","result_front/concat",NUMBER_OF_INPUTS,MOCAPNET_LHAND_OUTPUT_NUMBER,forceCPU);
            mnet->leftHand.modelLimits[0].isFlipped=0;
            mnet->leftHand.modelLimits[0].numberOfLimits=1;
            mnet->leftHand.modelLimits[0].minimumYaw1=-45.0;
            mnet->leftHand.modelLimits[0].maximumYaw1=45.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lhand_back.pb",mode,qualitySetting);
            result += neuralNetworkLoad(&mnet->leftHand.models[1],modelPath,"input_back","result_back/concat",NUMBER_OF_INPUTS,MOCAPNET_LHAND_OUTPUT_NUMBER,forceCPU);
            mnet->leftHand.modelLimits[1].isFlipped=0;
            mnet->leftHand.modelLimits[1].numberOfLimits=1;
            mnet->leftHand.modelLimits[1].minimumYaw1=135.0;//-90.0;
            mnet->leftHand.modelLimits[1].maximumYaw1=225.0;//-270.0;
            

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lhand_left.pb",mode,qualitySetting);
            result += neuralNetworkLoad(&mnet->leftHand.models[2] ,modelPath ,"input_left" ,"result_left/concat",NUMBER_OF_INPUTS,MOCAPNET_LHAND_OUTPUT_NUMBER,forceCPU);
            mnet->leftHand.modelLimits[2].isFlipped=0;
            mnet->leftHand.modelLimits[2].numberOfLimits=1;
            mnet->leftHand.modelLimits[2].minimumYaw1=-135.0;
            mnet->leftHand.modelLimits[2].maximumYaw1=45.0; 

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lhand_right.pb",mode,qualitySetting);//
            result += neuralNetworkLoad(&mnet->leftHand.models[3] ,modelPath ,"input_right" ,"result_right/concat",NUMBER_OF_INPUTS,MOCAPNET_LHAND_OUTPUT_NUMBER,forceCPU);
            mnet->leftHand.modelLimits[3].isFlipped=0;
            mnet->leftHand.modelLimits[3].numberOfLimits=1;
            mnet->leftHand.modelLimits[3].minimumYaw1=45.0;
            mnet->leftHand.modelLimits[3].maximumYaw1=135.0; 


            
            if(result==4)  
                {
                    result=1;
                    mnet->leftHand.loadedModels=4; 
                }
            else
                {
                    result=0;
                }
            break;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


        default:
            fprintf(stderr,RED "You requested a MocapNET configuration that is incorrect  ( mode=%u )\n" NORMAL , mode);
            return 0;
        break;
        };

    if (result)
        {
            fprintf(stderr,"Caching networks after initialization to be ready for use input [1,%u] / output [1,%u]..\n",NUMBER_OF_INPUTS,NUMBER_OF_OUTPUTS);
            std::vector<float> emptyValues;
            for (int i=0; i<NUMBER_OF_INPUTS; i++)
                {
                    emptyValues.push_back(0.0);
                }
            //---------------------------------------------------
            for (int i=0;  i<mnet->leftHand.loadedModels; i++ )
                {
                    std::vector<float>  prediction = neuralNetworkExecute(&mnet->leftHand.models[i],emptyValues);
                    if (prediction.size()>0)
                        {
                            fprintf(stderr, GREEN "Caching model %u (%s) was successful\n" NORMAL ,i,neuralNetworkGetPath(&mnet->leftHand.models[i]));
                        }
                    else
                        {
                            fprintf(stderr,RED "Caching model %u (%s) was unsuccessful\n" NORMAL,i,neuralNetworkGetPath(&mnet->leftHand.models[i]));
                        }
                }
                
        }

    return result;
}



int mocapnetLeftHand_unload(struct MocapNET2 * mnet)
{
    unsigned int result=0;
    for (int i=0;  i<mnet->leftHand.loadedModels; i++ )
        {
         result+=neuralNetworkUnload(&mnet->leftHand.models[i]);
        }

    if (result==mnet->leftHand.loadedModels) { result=1; } else
                                             { result=0; }
        
    mnet->leftHand.loadedModels=0;

    return result;
}



std::vector<float> mocapnetLeftHand_evaluateInput(struct MocapNET2 * mnet,struct skeletonSerialized * input,float scaleInput)
{
    std::vector<float> result;
    mnet->leftHand.positionalInput =  deriveMocapNET2InputUsingAssociations(
                                                                              mnet,
                                                                              input,
                                                                              &mnet->lhandSelectionIndexesArePopulated,
                                                                              mnet->lhandSelectionIndexes,
                                                                              mocapNET_InputLength_WithoutNSDM_lhand,
                                                                              mocapNET_lhand,
                                                                              0 // Enable to debug input
                                                                           );
                                                                             
    if (mnet->leftHand.positionalInput.size()==0)
    {
        return result;
    }
                                                       
    std::vector<float> modifiedPositionalInput  = mnet->leftHand.positionalInput; 
    float angleToRotate = 0.0;
                                                       
    if (scaleInput!=1.0)
                    { 
                      fprintf(stderr,YELLOW "LHand 2D Input scaled *= %0.2f \n" NORMAL,scaleInput);
                      for (unsigned int i=0; i<modifiedPositionalInput.size(); i++)
                       {    
                        modifiedPositionalInput[i] *= scaleInput; 
                       }
                    }
                                                       
    
    int eNSRM = (!mnet->options->regularNSRMMatrix);
    
    //If we are dealing with an eNSRM do the orientation correction..
    if (eNSRM) //(mnet->leftHand.perform2DAlignmentBeforeEvaluation)
    {
      unsigned int pivotJoint = 0; //R Hand
      unsigned int referenceJoint = 4 ; //finger3-1.r
    
      //Force alignment of middle finger to make it easier on the neural network
      angleToRotate = getAngleToAlignToZero(modifiedPositionalInput,pivotJoint,referenceJoint);                                                                            
      fprintf(stderr,YELLOW "Correcting left hand by rotating it %0.2f degrees\n" NORMAL,angleToRotate);
      rotate2DPointsBasedOnJointAsCenter(modifiedPositionalInput,angleToRotate,0);
    }

    mnet->leftHand.NSDM = lhandCreateNDSM(modifiedPositionalInput,angleToRotate,0/*NSDM Positional*/,1/*NSDM Angular */,0 /*Use Normalization*/,eNSRM); 
    mnet->leftHand.neuralNetworkReadyInput.clear();
    mnet->leftHand.neuralNetworkReadyInput.insert(mnet->leftHand.neuralNetworkReadyInput.end(), modifiedPositionalInput.begin(), modifiedPositionalInput.end());
    mnet->leftHand.neuralNetworkReadyInput.insert(mnet->leftHand.neuralNetworkReadyInput.end(), mnet->leftHand.NSDM.begin(), mnet->leftHand.NSDM.end());
    //---------------------------------------------------------------------------------------------- 
    //The code block above should have done everything needed to make a 749 element vector
    //If input is not correct then we should stop right here
    //----------------------------------------------------------------------------------------------
    if (mnet->leftHand.neuralNetworkReadyInput.size()!=MNET_LHAND_IN_NUMBER)
        {
            fprintf(stderr,RED "mocapnetLeftHand: Incorrect size of MocapNET input (expected %lu got %u ).. \n" NORMAL,mnet->leftHand.neuralNetworkReadyInput .size(),MNET_LHAND_IN_NUMBER);
            std::vector<float> emptyResult; 
            return emptyResult;
        }

    if (
         vectorcmp(
                    mnet->leftHand.lastNeuralNetworkReadyInput,
                    mnet->leftHand.neuralNetworkReadyInput,
                    0.01
                  )!=0
       )
    {
    //First run the network that can predict the orientation of the skeleton so we will use the correct
    //network to get the full BVH result from it 
    //----------------------------------------------------------------------------------------------
     result = localExecution(
                             &mnet->leftHand,
                             mnet->leftHand.neuralNetworkReadyInput,
                             mnet->orientation,
                             0 /*Left hand has no orientation or orientation trick*/
                            ); 
    //----------------------------------------------------------------------------------------------
    
    if (result.size()>3)
    {
        if (result[2]<0)
        {
            //result[2]=-1*result[2];
            fprintf(stderr,"Initial depth retreived is %f!\n",result[2]); 
        }
    }
    
    mnet->leftHand.result = result;
    mnet->leftHand.lastNeuralNetworkReadyInput = mnet->leftHand.neuralNetworkReadyInput;
    
    
    //LHand standalone visualization
    //------------------------------
    //int useQuaternionHand=1;
    //std::vector<float> standaloneHand;
    //mocapnetLeftHand_fillStandaloneResultVector(standaloneHand,result,useQuaternionHand);
    //standaloneHand[0]=-10; standaloneHand[1]=0; standaloneHand[2]=-35;
    //draw3DLhand(standaloneHand);
    //------------------------------
    
    
    return result;    
    } else
    {
      fprintf(stderr,GREEN "Nothing changed on left hand, returning previous result..\n" NORMAL );
      return mnet->leftHand.result;
    }
}


int mocapnetLeftHand_fillResultVector(std::vector<float> &result ,std::vector<float> resultLHand)
{ 
    int storedOutput=0; 
    
    if (resultLHand.size()>=MOCAPNET_LHAND_OUTPUT_NUMBER)
    {
         result[MOCAPNET_OUTPUT_LHAND_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_ZROTATION];
         result[MOCAPNET_OUTPUT_LHAND_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_XROTATION]; 
         result[MOCAPNET_OUTPUT_LHAND_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_YROTATION];
         result[MOCAPNET_OUTPUT_FINGER2_1_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_1_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_1_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_1_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_1_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_1_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_2_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_2_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_2_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_2_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_2_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_2_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_3_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_3_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_3_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_3_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_3_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_3_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_1_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_1_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_1_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_1_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_1_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_1_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_2_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_2_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_2_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_2_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_2_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_2_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_3_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_3_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_3_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_3_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_3_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_3_L_YROTATION];
         result[MOCAPNET_OUTPUT_FINGER4_1_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_1_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_1_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_1_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_1_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_1_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_2_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_2_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_2_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_2_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_2_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_2_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_3_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_3_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_3_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_3_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_3_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_3_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_1_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_1_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_1_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_1_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_1_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_1_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_2_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_2_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_2_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_2_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_2_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_2_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_3_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_3_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_3_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_3_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_3_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_3_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_LTHUMBBASE_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMBBASE_ZROTATION];
         result[MOCAPNET_OUTPUT_LTHUMBBASE_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMBBASE_XROTATION];
         result[MOCAPNET_OUTPUT_LTHUMBBASE_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMBBASE_YROTATION]; 
         result[MOCAPNET_OUTPUT_LTHUMB_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMB_ZROTATION]; 
         result[MOCAPNET_OUTPUT_LTHUMB_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMB_XROTATION]; 
         result[MOCAPNET_OUTPUT_LTHUMB_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMB_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_2_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_2_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_2_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_2_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_2_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_2_L_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_3_L_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_3_L_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_3_L_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_3_L_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_3_L_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_3_L_YROTATION];   
         storedOutput = 1;
    } 
    
    
    if (resultLHand.size()!=MOCAPNET_LHAND_OUTPUT_NUMBER)
    { 
        fprintf(stderr,YELLOW "MocapNET: Execution yielded %lu items for Left Hand (instead of %u) don't know what to do with them.. \n" NORMAL,resultLHand.size(),MOCAPNET_LHAND_OUTPUT_NUMBER);
    }
    
  return storedOutput;  
}



int mocapnetLeftHand_fillStandaloneResultVector(std::vector<float> &result,std::vector<float> resultLHand,int usesQuaternion)
{
    int storedOutput=0;
    
    int offset=1;
    if (usesQuaternion) { offset=0; }
    
    unsigned int STANDALONE_OUTPUT_ELEMENTS = MNET_STANDALONE_LHAND_OUTPUT_NUMBER-offset;
    
    if (result.size() != STANDALONE_OUTPUT_ELEMENTS)
    {
      //fprintf(stderr,"Lhand standalone qbvh initially had a wrong number of elements (%lu)\n",result.size());
      
      result.clear();
      for (unsigned int i=0; i<STANDALONE_OUTPUT_ELEMENTS; i++)
      {
        result.push_back(0.0);
      }
      
      //fprintf(stderr,"Lhand standalone qbvh extended to the correct number of elements (%lu/%u)\n",result.size(),STANDALONE_OUTPUT_ELEMENTS);
    }
    
    //fprintf(stderr,"lHand standalone output with quaternions=%u has %lu elements\n",usesQuaternion,result.size());
    
    if (resultLHand.size()>=MOCAPNET_LHAND_OUTPUT_NUMBER)
    {    //This is output that targets the old body only armature..!
         result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_XPOSITION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_XPOSITION];
         result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_YPOSITION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_YPOSITION];
         result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_ZPOSITION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_ZPOSITION];
         if (usesQuaternion)
         {
          result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_WROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_WROTATION];
          result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_XROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_XROTATION];
          result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_YROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_YROTATION];
          result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_ZROTATION]=resultLHand[MOCAPNET_LHAND_OUTPUT_LHAND_ZROTATION];
         } else
         {
          fprintf(stderr,"mocapnetLeftHand_fillStandaloneResultVector not populating rotation since it is not a quaternion\n");
          result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_WROTATION]=0;
          result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_XROTATION]=0;
          result[MNET_STANDALONE_LHAND_OUTPUT_LHAND_YROTATION]=0;
         }
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_1_L_ZROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_1_L_XROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_1_L_YROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER2_1_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_1_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER2_1_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_1_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER2_1_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_1_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER2_2_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_2_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER2_2_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_2_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER2_2_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_2_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER2_3_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_3_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER2_3_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_3_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER2_3_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER2_3_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_2_L_ZROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_2_L_XROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_2_L_YROTATION-offset]=0; 
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER3_1_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_1_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER3_1_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_1_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER3_1_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_1_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER3_2_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_2_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER3_2_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_2_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER3_2_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_2_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER3_3_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_3_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER3_3_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_3_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER3_3_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER3_3_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT__METACARPAL_3_L_ZROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT__METACARPAL_3_L_XROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT__METACARPAL_3_L_YROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_3_L_ZROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_3_L_XROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_3_L_YROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER4_1_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_1_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER4_1_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_1_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER4_1_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_1_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER4_2_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_2_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER4_2_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_2_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER4_2_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_2_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER4_3_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_3_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER4_3_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_3_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER4_3_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER4_3_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT__METACARPAL_4_L_ZROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT__METACARPAL_4_L_XROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT__METACARPAL_4_L_YROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_4_L_ZROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_4_L_XROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_METACARPAL_4_L_YROTATION-offset]=0;
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER5_1_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_1_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER5_1_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_1_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER5_1_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_1_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER5_2_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_2_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER5_2_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_2_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER5_2_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_2_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER5_3_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_3_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER5_3_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_3_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER5_3_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER5_3_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_LTHUMBBASE_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMBBASE_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_LTHUMBBASE_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMBBASE_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_LTHUMBBASE_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMBBASE_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_LTHUMB_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMB_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_LTHUMB_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMB_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_LTHUMB_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_LTHUMB_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER1_2_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_2_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER1_2_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_2_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER1_2_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_2_L_YROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER1_3_L_ZROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_3_L_ZROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER1_3_L_XROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_3_L_XROTATION];
         result[MNET_STANDALONE_LHAND_OUTPUT_FINGER1_3_L_YROTATION-offset]=resultLHand[MOCAPNET_LHAND_OUTPUT_FINGER1_3_L_YROTATION];
         storedOutput=1;
    } else 
    { 
        fprintf(stderr,YELLOW "MocapNET: Execution yielded %lu items for Right Hand (instead of %u) don't know what to do with them.. \n" NORMAL,resultLHand.size(),MOCAPNET_LHAND_OUTPUT_NUMBER);
    }
    
  return storedOutput;  
}

