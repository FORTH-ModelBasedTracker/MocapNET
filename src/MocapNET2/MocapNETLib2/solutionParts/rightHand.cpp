#include "rightHand.hpp"
#include "rightHandSym.hpp"


#include "../../MocapNETLib2/config.h"
#include "../../MocapNETLib2/IO/bvh.hpp"
#include "../../MocapNETLib2/IO/conversions.hpp"
#include "../../MocapNETLib2/core/core.hpp"
#include "../../MocapNETLib2/visualization/drawHands.hpp"
#include "../NSDM/generated_rhand.hpp"
#include "../tools.hpp"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

int mocapnetRightHand_initializeAssociations(struct MocapNET2 * mnet, struct skeletonSerialized * input)
{
   return  initializeAssociationsForSubsetOfSkeleton(
                                                      &mnet->rhandSelectionIndexesArePopulated,
                                                      mnet->rhandSelectionIndexes,
                                                      mocapNET_InputLength_WithoutNSDM_rhand,
                                                      mocapNET_rhand,
                                                      input
                                                     );
}

//This is crap but it is the easiest way to do this.. for HandTest 
int mocapnetRightHand_initializeUsingAssociationsOfLeftHand(struct MocapNET2 * mnet, struct skeletonSerialized * input)
{
   return  initializeAssociationsForSubsetOfSkeleton(
                                                      &mnet->rhandSelectionIndexesArePopulated,
                                                      mnet->rhandSelectionIndexes,
                                                      mocapNET_InputLength_WithoutNSDM_rhand,
                                                      mocapNET_lhand,
                                                      input
                                                     );
}


int mocapnetRightHand_initialize(struct MocapNET2 * mnet,const char * filename,float qualitySetting,unsigned int mode,unsigned int forceCPU)
{
    #if RHAND_FLIP
     fprintf(stderr,GREEN "Using RHand flip so no initialization required :D\n" NORMAL);
     mnet->rightHand.loadedModels=1;
     return 1;
    #endif 
    
    char modelPath[1024]= {0};
    int result = 0;
    
    unsigned int NUMBER_OF_INPUTS = MNET_RHAND_IN_NUMBER;
    unsigned int NUMBER_OF_OUTPUTS = MOCAPNET_RHAND_OUTPUT_NUMBER;
    
    //Force mode 1 for now..
    mode=1;
    
    switch (mode)
        {
        case 1:
              mnet->rightHand.mode=1;
              //snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/rhand_half_all.pb",mode,qualitySetting);
              snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/rhand_all.pb",mode,qualitySetting);
              //mnet->rightHand.models[0].model.inputIsHalfFloats=1;
              //mnet->rightHand.models[0].model.outputIsHalfFloats=1;
              
              result += neuralNetworkLoad(&mnet->rightHand.models[0],modelPath,"input_all","result_all/concat",NUMBER_OF_INPUTS,MOCAPNET_RHAND_OUTPUT_NUMBER,forceCPU);
              mnet->rightHand.loadedModels=result;  
            break;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        case 3:
            mnet->rightHand.mode=3;
              fprintf(stderr,RED "Fatal: Mode 3 No longer supported on MocapNET 2 \n" NORMAL);
              exit(0); 
            break;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        case 5:
            mnet->rightHand.mode=5; 
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //                                                  Regular 3 Model setup such as the BMVC 2019 Work..
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            /*snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/categorize_body.pb",mode,qualitySetting);
            result += neuralNetworkLoad(&mnet->rightHand.models[0]  ,modelPath  ,"input_all"  ,"result_all/concat",forceCPU);
            mnet->rightHand.modelLimits[0].isFlipped=0;
            mnet->rightHand.modelLimits[0].numberOfLimits=1;
            mnet->rightHand.modelLimits[0].minimumYaw1=-360.0;
            mnet->rightHand.modelLimits[0].maximumYaw1=360.0;*/

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/rhand_front.pb",mode,qualitySetting);
            result += neuralNetworkLoad(&mnet->rightHand.models[0],modelPath,"input_front","result_front/concat",NUMBER_OF_INPUTS,MOCAPNET_RHAND_OUTPUT_NUMBER,forceCPU);
            mnet->rightHand.modelLimits[0].isFlipped=0;
            mnet->rightHand.modelLimits[0].numberOfLimits=1;
            mnet->rightHand.modelLimits[0].minimumYaw1=-45.0;
            mnet->rightHand.modelLimits[0].maximumYaw1=45.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/rhand_back.pb",mode,qualitySetting);
            result += neuralNetworkLoad(&mnet->rightHand.models[1],modelPath,"input_back","result_back/concat",NUMBER_OF_INPUTS,MOCAPNET_RHAND_OUTPUT_NUMBER,forceCPU);
            mnet->rightHand.modelLimits[1].isFlipped=0;
            mnet->rightHand.modelLimits[1].numberOfLimits=1;
            mnet->rightHand.modelLimits[1].minimumYaw1=135.0;//-90.0;
            mnet->rightHand.modelLimits[1].maximumYaw1=225.0;//-270.0;
            

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/rhand_left.pb",mode,qualitySetting);
            result += neuralNetworkLoad(&mnet->rightHand.models[2] ,modelPath ,"input_left" ,"result_left/concat",NUMBER_OF_INPUTS,MOCAPNET_RHAND_OUTPUT_NUMBER,forceCPU);
            mnet->rightHand.modelLimits[2].isFlipped=0;
            mnet->rightHand.modelLimits[2].numberOfLimits=1;
            mnet->rightHand.modelLimits[2].minimumYaw1=-135.0;
            mnet->rightHand.modelLimits[2].maximumYaw1=45.0; 

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/rhand_right.pb",mode,qualitySetting);//
            result += neuralNetworkLoad(&mnet->rightHand.models[3] ,modelPath ,"input_right" ,"result_right/concat",NUMBER_OF_INPUTS,MOCAPNET_RHAND_OUTPUT_NUMBER,forceCPU);
            mnet->rightHand.modelLimits[3].isFlipped=0;
            mnet->rightHand.modelLimits[3].numberOfLimits=1;
            mnet->rightHand.modelLimits[3].minimumYaw1=45.0;
            mnet->rightHand.modelLimits[3].maximumYaw1=135.0; 


            
            if(result==4)  
                {
                    result=1;
                    mnet->rightHand.loadedModels=4;  
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
            for (int i=0;  i<mnet->rightHand.loadedModels; i++ )
                {
                    std::vector<float>  prediction = neuralNetworkExecute(&mnet->rightHand.models[i],emptyValues);
                    if (prediction.size()>0)
                        {
                            fprintf(stderr, GREEN "Caching model %u (%s) was successful\n" NORMAL ,i,neuralNetworkGetPath(&mnet->rightHand.models[i]));
                        }
                    else
                        {
                            fprintf(stderr,RED "Caching model %u (%s) was unsuccessful\n" NORMAL,i,neuralNetworkGetPath(&mnet->rightHand.models[i]));
                        }
                }
                
        }

    return result;
}



int mocapnetRightHand_unload(struct MocapNET2 * mnet)
{
    #if RHAND_FLIP
     fprintf(stderr,GREEN "Using RHand flip so no deinitialization required :D\n" NORMAL);
     return 1;
    #endif 
    
    unsigned int result=0;
    for (int i=0;  i<mnet->rightHand.loadedModels; i++ )
        {
         result+=neuralNetworkUnload(&mnet->rightHand.models[i]);
        }

    if (result==mnet->rightHand.loadedModels) { result=1; } else
                                              { result=0; }
        
    mnet->rightHand.loadedModels=0;

    return result;
}



std::vector<float> mocapnetRightHand_evaluateInput(struct MocapNET2 * mnet,struct skeletonSerialized * input,float scaleInput)
{ 
    #if RHAND_FLIP
     fprintf(stderr,GREEN "Using mocapnetRightHandSym_evaluateInput :D\n" NORMAL);
     return mocapnetRightHandSym_evaluateInput(mnet,input,scaleInput);
    #endif 
    
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
    
    std::vector<float> modifiedPositionalInput  = mnet->rightHand.positionalInput;
    float angleToRotate = 0.0;
    
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
    //First run the network that can predict the orientation of the skeleton so we will use the correct
    //network to get the full BVH result from it 
    //----------------------------------------------------------------------------------------------
    result = localExecution(
                            &mnet->rightHand,
                            mnet->rightHand.neuralNetworkReadyInput,
                            mnet->orientation,
                            0 /*Right hand has no orientation or orientation trick*/
                           ); 
    //----------------------------------------------------------------------------------------------
    mnet->rightHand.result = result;
    mnet->rightHand.lastNeuralNetworkReadyInput = mnet->rightHand.neuralNetworkReadyInput;
     
         
    //RHand standalone visualization
    //------------------------------
    //int useQuaternionHand=1;
    //std::vector<float> standaloneHand;
    //mocapnetRightHand_fillStandaloneResultVector(standaloneHand,result,useQuaternionHand);
    //standaloneHand[0]=-10; standaloneHand[1]=0; standaloneHand[2]=-35;
    //draw3DRhand(standaloneHand);
    //------------------------------
    
     
    return result; 
    } else
    {
      fprintf(stderr,GREEN "Nothing changed on right hand, returning previous result.. \n" NORMAL);
      return mnet->rightHand.result;
    }
}


int mocapnetRightHand_fillResultVector(std::vector<float> &result ,std::vector<float> resultRHand)
{
    int storedOutput=0;
    
    if (resultRHand.size()>=MOCAPNET_RHAND_OUTPUT_NUMBER)
    {    //This is output that targets the old body only armature..!   
         //fprintf(stderr,GREEN "MocapNET: Execution yielded a correct %lu items \n" NORMAL,resultRHand.size());
         result[MOCAPNET_OUTPUT_RHAND_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_ZROTATION];
         result[MOCAPNET_OUTPUT_RHAND_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_XROTATION]; 
         result[MOCAPNET_OUTPUT_RHAND_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_YROTATION];
         result[MOCAPNET_OUTPUT_FINGER2_1_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_1_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_1_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_1_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_1_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_1_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_2_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_2_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_2_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_2_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_2_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_2_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_3_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_3_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_3_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_3_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER2_3_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_3_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_1_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_1_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_1_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_1_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_1_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_1_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_2_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_2_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_2_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_2_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_2_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_2_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_3_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_3_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_3_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_3_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER3_3_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_3_R_YROTATION];
         result[MOCAPNET_OUTPUT_FINGER4_1_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_1_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_1_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_1_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_1_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_1_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_2_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_2_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_2_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_2_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_2_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_2_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_3_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_3_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_3_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_3_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER4_3_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_3_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_1_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_1_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_1_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_1_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_1_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_1_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_2_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_2_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_2_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_2_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_2_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_2_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_3_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_3_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_3_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_3_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER5_3_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_3_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_RTHUMBBASE_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMBBASE_ZROTATION];
         result[MOCAPNET_OUTPUT_RTHUMBBASE_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMBBASE_XROTATION];
         result[MOCAPNET_OUTPUT_RTHUMBBASE_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMBBASE_YROTATION]; 
         result[MOCAPNET_OUTPUT_RTHUMB_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMB_ZROTATION]; 
         result[MOCAPNET_OUTPUT_RTHUMB_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMB_XROTATION]; 
         result[MOCAPNET_OUTPUT_RTHUMB_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMB_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_2_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_2_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_2_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_2_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_2_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_2_R_YROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_3_R_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_3_R_ZROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_3_R_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_3_R_XROTATION]; 
         result[MOCAPNET_OUTPUT_FINGER1_3_R_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_3_R_YROTATION];   
         
         storedOutput=1;
    } 
    
    if (resultRHand.size()!=MOCAPNET_RHAND_OUTPUT_NUMBER)
    { 
        fprintf(stderr,YELLOW "MocapNET: Execution yielded %lu items for Right Hand (instead of %u) don't know what to do with them.. \n" NORMAL,resultRHand.size(),MOCAPNET_RHAND_OUTPUT_NUMBER);
    }
  return storedOutput;  
}




int mocapnetRightHand_fillStandaloneResultVector(std::vector<float> &result ,std::vector<float> resultRHand,int usesQuaternion)
{
    int storedOutput=0;
    
    int offset=1;
    if (usesQuaternion) { offset=0; }
    
    unsigned int STANDALONE_OUTPUT_ELEMENTS = MNET_STANDALONE_RHAND_OUTPUT_NUMBER-offset;
    
    if (result.size() != STANDALONE_OUTPUT_ELEMENTS)
    {
      fprintf(stderr,"Rhand standalone qbvh initially had a wrong number of elements (%lu)\n",result.size());
      
      result.clear();
      for (unsigned int i=0; i<STANDALONE_OUTPUT_ELEMENTS; i++)
      {
        result.push_back(0.0);
      }
      
      fprintf(stderr,"Rhand standalone qbvh extended to the correct number of elements (%lu)\n",result.size());
    }
    
    
    if (resultRHand.size()>=MOCAPNET_RHAND_OUTPUT_NUMBER)
    {    //This is output that targets the old body only armature..!
         result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_XPOSITION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_XPOSITION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_YPOSITION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_YPOSITION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_ZPOSITION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_ZPOSITION];
         if (usesQuaternion)
         {
         result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_WROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_WROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_XROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_YROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_ZROTATION]=resultRHand[MOCAPNET_RHAND_OUTPUT_RHAND_ZROTATION];
         } else
         {
          fprintf(stderr,"mocapnetRightHand_fillStandaloneResultVector not populating rotation since it is not a quaternion\n");
          result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_WROTATION]=0;
          result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_XROTATION]=0;
          result[MNET_STANDALONE_RHAND_OUTPUT_RHAND_YROTATION]=0;
         }
         
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_1_R_ZROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_1_R_XROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_1_R_YROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER2_1_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_1_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER2_1_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_1_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER2_1_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_1_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER2_2_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_2_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER2_2_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_2_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER2_2_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_2_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER2_3_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_3_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER2_3_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_3_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER2_3_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER2_3_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_2_R_ZROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_2_R_XROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_2_R_YROTATION-offset]=0; 
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER3_1_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_1_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER3_1_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_1_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER3_1_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_1_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER3_2_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_2_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER3_2_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_2_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER3_2_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_2_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER3_3_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_3_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER3_3_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_3_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER3_3_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER3_3_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_3_R_ZROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_3_R_XROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_3_R_YROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_3_R_ZROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_3_R_XROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_3_R_YROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER4_1_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_1_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER4_1_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_1_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER4_1_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_1_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER4_2_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_2_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER4_2_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_2_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER4_2_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_2_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER4_3_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_3_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER4_3_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_3_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER4_3_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER4_3_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_4_R_ZROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_4_R_XROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_4_R_YROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_4_R_ZROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_4_R_XROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_4_R_YROTATION-offset]=0;
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER5_1_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_1_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER5_1_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_1_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER5_1_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_1_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER5_2_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_2_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER5_2_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_2_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER5_2_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_2_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER5_3_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_3_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER5_3_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_3_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER5_3_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER5_3_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RTHUMBBASE_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMBBASE_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RTHUMBBASE_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMBBASE_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RTHUMBBASE_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMBBASE_YROTATION]; 
         result[MNET_STANDALONE_RHAND_OUTPUT_RTHUMB_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMB_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RTHUMB_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMB_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_RTHUMB_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_RTHUMB_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER1_2_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_2_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER1_2_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_2_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER1_2_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_2_R_YROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER1_3_R_ZROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_3_R_ZROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER1_3_R_XROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_3_R_XROTATION];
         result[MNET_STANDALONE_RHAND_OUTPUT_FINGER1_3_R_YROTATION-offset]=resultRHand[MOCAPNET_RHAND_OUTPUT_FINGER1_3_R_YROTATION];
         storedOutput=1;
    } else 
    { 
        fprintf(stderr,YELLOW "MocapNET: Execution yielded %lu items for Right Hand (instead of %u) don't know what to do with them.. \n" NORMAL,resultRHand.size(),MOCAPNET_RHAND_OUTPUT_NUMBER);
    }
    
  return storedOutput;  
}
