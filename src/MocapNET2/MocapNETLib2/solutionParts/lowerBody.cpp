#include "lowerBody.hpp"

#include "../../MocapNETLib2/config.h"
#include "../../MocapNETLib2/IO/conversions.hpp"
#include "../../MocapNETLib2/tools.hpp"
#include "../../MocapNETLib2/core/core.hpp"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

int mocapnetLowerBody_initializeAssociations(struct MocapNET2 * mnet, struct skeletonSerialized * input)
{
   return initializeAssociationsForSubsetOfSkeleton(
                                                    &mnet->lowerBodySelectionIndexesArePopulated,
                                                    mnet->lowerBodySelectionIndexes,
                                                    mocapNET_InputLength_WithoutNSDM_lowerbody,
                                                    mocapNET_lowerbody,
                                                    input
                                                   ); 
}



int mocapnetLowerBody_initialize(struct MocapNET2 * mnet,const char * filename,float qualitySetting,unsigned int mode,unsigned int forceCPU)
{
    char modelPath[1024]= {0};
    int result = 0;
    
    int modelNumber=0;
   
    switch (mode)
        {
        case 3:
            mnet->lowerBody.mode=3;
              fprintf(stderr,RED "Fatal: Mode 3 No longer supported on MocapNET 2 \n" NORMAL);
              exit(0); 
            break;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 
        case 5:
            mnet->lowerBody.mode=5; 
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //                                                  Regular 3 Model setup such as the BMVC 2019 Work..
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/categorize_lowerbody_all.pb",mode,qualitySetting);
            if (fileExists(modelPath))
            {
             result += loadTensorflowInstance(&mnet->lowerBody.models[modelNumber]  ,modelPath  ,"input_all"  ,"result_all/concat",forceCPU);
             mnet->lowerBody.modelLimits[modelNumber].isFlipped=0;
             mnet->lowerBody.modelLimits[modelNumber].numberOfLimits=1;
             mnet->lowerBody.modelLimits[modelNumber].minimumYaw1=-360.0;
             mnet->lowerBody.modelLimits[modelNumber].maximumYaw1=360.0;
            modelNumber+=1; 
            } else
            {
                fprintf(stderr,YELLOW "Could not find a pose categorization classifier for LowerBody..\n " NORMAL );
            }

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lowerbody_front.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->lowerBody.models[modelNumber],modelPath,"input_front","result_front/concat",forceCPU);
            mnet->lowerBody.modelLimits[modelNumber].isFlipped=0;
            mnet->lowerBody.modelLimits[modelNumber].numberOfLimits=1;
            mnet->lowerBody.modelLimits[modelNumber].minimumYaw1=-45.0;
            mnet->lowerBody.modelLimits[modelNumber].maximumYaw1=45.0;
            modelNumber+=1; 

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lowerbody_back.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->lowerBody.models[modelNumber],modelPath,"input_back","result_back/concat",forceCPU);
            mnet->lowerBody.modelLimits[modelNumber].isFlipped=0;
            mnet->lowerBody.modelLimits[modelNumber].numberOfLimits=1;
            mnet->lowerBody.modelLimits[modelNumber].minimumYaw1=135.0;//-90.0;
            mnet->lowerBody.modelLimits[modelNumber].maximumYaw1=225.0;//-270.0;
            modelNumber+=1; 
            

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lowerbody_left.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->lowerBody.models[modelNumber] ,modelPath ,"input_left" ,"result_left/concat",forceCPU);
            mnet->lowerBody.modelLimits[modelNumber].isFlipped=0;
            mnet->lowerBody.modelLimits[modelNumber].numberOfLimits=1;
            mnet->lowerBody.modelLimits[modelNumber].minimumYaw1=-135.0;
            mnet->lowerBody.modelLimits[modelNumber].maximumYaw1=45.0; 
            modelNumber+=1; 

            //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/lowerbody_right.pb",mode,qualitySetting);//
            result += loadTensorflowInstance(&mnet->lowerBody.models[modelNumber] ,modelPath ,"input_right" ,"result_right/concat",forceCPU);
            mnet->lowerBody.modelLimits[modelNumber].isFlipped=0;
            mnet->lowerBody.modelLimits[modelNumber].numberOfLimits=1;
            mnet->lowerBody.modelLimits[modelNumber].minimumYaw1=45.0;
            mnet->lowerBody.modelLimits[modelNumber].maximumYaw1=135.0; 
            modelNumber+=1; 


            
            if(result==modelNumber) 
                {
                    result=1;
                    mnet->lowerBody.loadedModels=modelNumber; 
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
            fprintf(stderr,"Caching networks after initialization to be ready for use..\n");
            std::vector<float> emptyValues;
            for (int i=0; i<MNET_LOWERBODY_IN_NUMBER; i++)
                {
                    emptyValues.push_back(0.0);
                }
            //---------------------------------------------------
            for (int i=0;  i<mnet->lowerBody.loadedModels; i++ )
                {
                    std::vector<float>  prediction = predictTensorflow(&mnet->lowerBody.models[i],emptyValues);
                    if (prediction.size()>0)
                        {
                            fprintf(stderr, GREEN "Caching model %u (%s) was successful\n" NORMAL ,i,mnet->lowerBody.models[i].modelPath);
                        }
                    else
                        {
                            fprintf(stderr,RED "Caching model %u (%s) was unsuccessful\n" NORMAL,i,mnet->lowerBody.models[i].modelPath);
                        }
                }
                
        }

    return result;
}



int mocapnetLowerBody_unload(struct MocapNET2 * mnet)
{
    unsigned int result=0;
    for (int i=0;  i<mnet->lowerBody.loadedModels; i++ )
        {
         result+=unloadTensorflow(&mnet->lowerBody.models[i]);
        }

    if (result==mnet->lowerBody.loadedModels) { result=1; } else
                                         { result=0; }
        
    mnet->lowerBody.loadedModels=0;

    return result;
}



std::vector<float> mocapnetLowerBody_evaluateInput(struct MocapNET2 * mnet,struct skeletonSerialized * input)
{
    mnet->lowerBody.positionalInput = deriveMocapNET2InputUsingAssociations( 
                                                                             mnet,
                                                                             input,
                                                                             &mnet->lowerBodySelectionIndexesArePopulated, 
                                                                             mnet->lowerBodySelectionIndexes,
                                                                             mocapNET_InputLength_WithoutNSDM_lowerbody,
                                                                             mocapNET_lowerbody,
                                                                             0 // Enable to debug input
                                                                            );
                                                                            
                                                                            
                                                                            
    
    mnet->lowerBody.NSDM  = lowerbodyCreateNDSM(mnet->lowerBody.positionalInput,0/*NSDM Positional*/,1/*NSDM Angular */,0 /*Use Normalization*/);
    
    /*
    fprintf(stderr,"LowerBodyNSDM:\n");
    for (unsigned int y=0; y<17; y++)
    {
     for (unsigned int x=0; x<17; x++)
     { 
        fprintf(stderr,"%0.1f ",mnet->lowerBody.NSDM[y*17+x]); 
     }       
     fprintf(stderr,"\n");  
    }*/
    
    mnet->lowerBody.neuralNetworkReadyInput.clear();
    mnet->lowerBody.neuralNetworkReadyInput.insert(mnet->lowerBody.neuralNetworkReadyInput.end(),mnet->lowerBody.positionalInput.begin(), mnet->lowerBody.positionalInput.end());
    mnet->lowerBody.neuralNetworkReadyInput.insert(mnet->lowerBody.neuralNetworkReadyInput.end(),mnet->lowerBody.NSDM.begin(), mnet->lowerBody.NSDM.end());
    //---------------------------------------------------------------------------------------------- 
    //The code block above should have done everything needed to make a 749 element vector
    //If input is not correct then we should stop right here
    //----------------------------------------------------------------------------------------------
    if (mnet->lowerBody.neuralNetworkReadyInput.size()!=MNET_LOWERBODY_IN_NUMBER)
        {
            fprintf(stderr,RED "MocapNET: Incorrect size of MocapNET input .. \n" NORMAL); 
            std::vector<float> emptyResult; 
            return emptyResult;
        }
     
     /*
      * Skip extra calculations by relying on upperbody orientation..
      * 
     int orientationReceived =localOrientationExtraction(&mnet->lowerBody,mnet->lowerBody.neuralNetworkReadyInput); 
     if (orientationReceived!=MOCAPNET_ORIENTATION_NONE) 
         { 
             mnet->orientation=orientationReceived;
             mnet->orientationClassifications[0]= mnet->lowerBody.orientationClassifications[0];
             mnet->orientationClassifications[1]= mnet->lowerBody.orientationClassifications[1];
             mnet->orientationClassifications[2]= mnet->lowerBody.orientationClassifications[2];
             mnet->orientationClassifications[3]= mnet->lowerBody.orientationClassifications[3]; 
         }
      */
    if (
         vectorcmp(
                    mnet->lowerBody.lastNeuralNetworkReadyInput,
                    mnet->lowerBody.neuralNetworkReadyInput,
                    0.1
                  )!=0
        )
    {
     //First run the network that can predict the orientation of the skeleton so we will use the correct
     //network to get the full BVH result from it 
     //----------------------------------------------------------------------------------------------
     std::vector<float> result = localExecution(
                                                 &mnet->lowerBody,
                                                 mnet->lowerBody.neuralNetworkReadyInput,
                                                 mnet->orientation,
                                                 NN_ORIENTATIONS_TRAINED_AROUND_ZERO_AND_REQUIRE_TRICK /*Body Requires orientation trick*/ 
                                               ); 
     mnet->lowerBody.result = result;
     mnet->lowerBody.lastNeuralNetworkReadyInput = mnet->lowerBody.neuralNetworkReadyInput;
    
    return result;
    } else
    {
      fprintf(stderr,GREEN "Nothing changed on lower body, returning previous result.. \n" NORMAL);
      return mnet->lowerBody.result;
    }
}


int mocapnetLowerBody_fillResultVector(std::vector<float> &result ,std::vector<float> resultBody)
{
    if (resultBody.size()==MOCAPNET_LOWERBODY_OUTPUT_NUMBER)
    {    //This is output that targets the old body only armature..!  
         //result[MOCAPNET_OUTPUT_HIP_XPOSITION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_HIP_XPOSITION];
         //result[MOCAPNET_OUTPUT_HIP_YPOSITION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_HIP_YPOSITION];
         //result[MOCAPNET_OUTPUT_HIP_ZPOSITION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_HIP_ZPOSITION];
         //result[MOCAPNET_OUTPUT_HIP_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_HIP_ZROTATION];
         //result[MOCAPNET_OUTPUT_HIP_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_HIP_YROTATION];
         //result[MOCAPNET_OUTPUT_HIP_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_HIP_XROTATION];
         //result[MOCAPNET_OUTPUT_ABDOMEN_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_ABDOMEN_ZROTATION];
         //result[MOCAPNET_OUTPUT_ABDOMEN_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_ABDOMEN_XROTATION];
         //result[MOCAPNET_OUTPUT_ABDOMEN_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_ABDOMEN_YROTATION];
         //result[MOCAPNET_OUTPUT_CHEST_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_CHEST_ZROTATION];
         //result[MOCAPNET_OUTPUT_CHEST_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_CHEST_XROTATION];
         //result[MOCAPNET_OUTPUT_CHEST_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_CHEST_YROTATION];
         //result[MOCAPNET_OUTPUT_NECK_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_NECK_ZROTATION];
         //result[MOCAPNET_OUTPUT_NECK_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_NECK_XROTATION];
         //result[MOCAPNET_OUTPUT_NECK_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_NECK_YROTATION];
         result[MOCAPNET_OUTPUT_RHIP_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_RHIP_ZROTATION];
         result[MOCAPNET_OUTPUT_RHIP_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_RHIP_XROTATION];
         result[MOCAPNET_OUTPUT_RHIP_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_RHIP_YROTATION];
         result[MOCAPNET_OUTPUT_RKNEE_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_RKNEE_ZROTATION];
         result[MOCAPNET_OUTPUT_RKNEE_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_RKNEE_XROTATION];
         result[MOCAPNET_OUTPUT_RKNEE_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_RKNEE_YROTATION];
         result[MOCAPNET_OUTPUT_RFOOT_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_RFOOT_ZROTATION];
         result[MOCAPNET_OUTPUT_RFOOT_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_RFOOT_XROTATION];
         result[MOCAPNET_OUTPUT_RFOOT_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_RFOOT_YROTATION];
         result[MOCAPNET_OUTPUT_TOE1_2_R_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_R_ZROTATION];
         result[MOCAPNET_OUTPUT_TOE1_2_R_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_R_XROTATION];
         result[MOCAPNET_OUTPUT_TOE1_2_R_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_R_YROTATION];
         result[MOCAPNET_OUTPUT_TOE5_3_R_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_R_ZROTATION];
         result[MOCAPNET_OUTPUT_TOE5_3_R_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_R_XROTATION];
         result[MOCAPNET_OUTPUT_TOE5_3_R_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_R_YROTATION];
         result[MOCAPNET_OUTPUT_LHIP_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_LHIP_ZROTATION];
         result[MOCAPNET_OUTPUT_LHIP_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_LHIP_XROTATION];
         result[MOCAPNET_OUTPUT_LHIP_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_LHIP_YROTATION];
         result[MOCAPNET_OUTPUT_LKNEE_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_LKNEE_ZROTATION];
         result[MOCAPNET_OUTPUT_LKNEE_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_LKNEE_XROTATION];
         result[MOCAPNET_OUTPUT_LKNEE_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_LKNEE_YROTATION];
         result[MOCAPNET_OUTPUT_LFOOT_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_LFOOT_ZROTATION];
         result[MOCAPNET_OUTPUT_LFOOT_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_LFOOT_XROTATION];
         result[MOCAPNET_OUTPUT_LFOOT_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_LFOOT_YROTATION];
         result[MOCAPNET_OUTPUT_TOE1_2_L_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_L_ZROTATION];
         result[MOCAPNET_OUTPUT_TOE1_2_L_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_L_XROTATION];
         result[MOCAPNET_OUTPUT_TOE1_2_L_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_L_YROTATION];
         result[MOCAPNET_OUTPUT_TOE5_3_L_ZROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_L_ZROTATION];
         result[MOCAPNET_OUTPUT_TOE5_3_L_XROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_L_XROTATION];
         result[MOCAPNET_OUTPUT_TOE5_3_L_YROTATION]=resultBody[MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_L_YROTATION]; 
         return 1;
    } else
    { 
        fprintf(stderr,YELLOW "MocapNET: Execution yielded %lu items ( expected %u ) for lower body don't know what to do with them.. \n" NORMAL,resultBody.size(),MOCAPNET_LOWERBODY_OUTPUT_NUMBER);
    }
  return 0;  
}
