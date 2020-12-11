#include "upperBody.hpp"

#include "../../MocapNETLib2/config.h"
#include "../../MocapNETLib2/IO/conversions.hpp"
#include "../../MocapNETLib2/core/core.hpp"
#include "../../MocapNETLib2/tools.hpp"
#include "../tools.hpp"

#include "../visualization/visualization.hpp" //debug alignment


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

int mocapnetUpperBody_initializeAssociations(struct MocapNET2 * mnet, struct skeletonSerialized * input)
{
    return initializeAssociationsForSubsetOfSkeleton(
               &mnet->upperBodySelectionIndexesArePopulated,
               mnet->upperBodySelectionIndexes,
               mocapNET_InputLength_WithoutNSDM_upperbody,
               mocapNET_upperbody,
               input
           );
}



int mocapnetUpperBody_initialize(struct MocapNET2 * mnet,const char * filename,float qualitySetting,unsigned int mode,unsigned int forceCPU)
{
    char modelPath[1025]= {0};
    int result = 0;

    int modelNumber=0;
    switch (mode)
        {
        case 3:
            mnet->upperBody.mode=3;
            fprintf(stderr,RED "Fatal: Mode 3 No longer supported on MocapNET 2 \n" NORMAL);
            exit(0);
            break;
        //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        case 5:
            mnet->upperBody.mode=5;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //                                                  Regular 3 Model setup such as the BMVC 2019 Work..
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/upperbody_front.pb",mode,qualitySetting);
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/categorize_upperbody_all.pb",mode,qualitySetting);
            if (fileExists(modelPath))
                {
                    //result += loadTensorflowInstance(&mnet->upperBody.models[modelNumber],modelPath,"input_front","result_front/concat",forceCPU);
                    result += loadTensorflowInstance(&mnet->upperBody.models[modelNumber],modelPath,"input_all","result_all/concat",forceCPU);
                    mnet->upperBody.modelLimits[modelNumber].isFlipped=0;
                    mnet->upperBody.modelLimits[modelNumber].numberOfLimits=1;
                    mnet->upperBody.modelLimits[modelNumber].minimumYaw1=-360.0;
                    mnet->upperBody.modelLimits[modelNumber].maximumYaw1=360.0;
                    modelNumber+=1;
                }
            else
                {
                    fprintf(stderr,YELLOW "Could not find a pose categorization classifier for UpperBody..\n " NORMAL );
                }

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/upperbody_front.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->upperBody.models[modelNumber],modelPath,"input_front","result_front/concat",forceCPU);
            mnet->upperBody.modelLimits[modelNumber].isFlipped=0;
            mnet->upperBody.modelLimits[modelNumber].numberOfLimits=1;
            mnet->upperBody.modelLimits[modelNumber].minimumYaw1=-45.0;
            mnet->upperBody.modelLimits[modelNumber].maximumYaw1=45.0;
            modelNumber+=1;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/upperbody_back.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->upperBody.models[modelNumber],modelPath,"input_back","result_back/concat",forceCPU);
            mnet->upperBody.modelLimits[modelNumber].isFlipped=0;
            mnet->upperBody.modelLimits[modelNumber].numberOfLimits=1;
            mnet->upperBody.modelLimits[modelNumber].minimumYaw1=135.0;//-90.0;
            mnet->upperBody.modelLimits[modelNumber].maximumYaw1=225.0;//-270.0;
            modelNumber+=1;


            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/upperbody_left.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->upperBody.models[modelNumber],modelPath,"input_left","result_left/concat",forceCPU);
            mnet->upperBody.modelLimits[modelNumber].isFlipped=0;
            mnet->upperBody.modelLimits[modelNumber].numberOfLimits=1;
            mnet->upperBody.modelLimits[modelNumber].minimumYaw1=-135.0;
            mnet->upperBody.modelLimits[modelNumber].maximumYaw1=45.0;
            modelNumber+=1;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mocapnet2/mode%u/%0.1f/upperbody_right.pb",mode,qualitySetting);//
            result += loadTensorflowInstance(&mnet->upperBody.models[modelNumber],modelPath,"input_right","result_right/concat",forceCPU);
            mnet->upperBody.modelLimits[modelNumber].isFlipped=0;
            mnet->upperBody.modelLimits[modelNumber].numberOfLimits=1;
            mnet->upperBody.modelLimits[modelNumber].minimumYaw1=45.0;
            mnet->upperBody.modelLimits[modelNumber].maximumYaw1=135.0;
            modelNumber+=1;



            if(result==modelNumber) // make this 5
                {
                    result=1;
                    mnet->upperBody.loadedModels=modelNumber; //make this 5
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
            fprintf(stderr,RED "You requested a MocapNET configuration that is incorrect  ( mode=%u )\n" NORMAL, mode);
            return 0;
            break;
        };

    if (result)
        {
            fprintf(stderr,"Caching networks after initialization to be ready for use..\n");
            std::vector<float> emptyValues;
            for (int i=0; i<MNET_UPPERBODY_IN_NUMBER; i++)
                {
                    emptyValues.push_back(0.0);
                }
            //---------------------------------------------------
            for (int i=0;  i<mnet->upperBody.loadedModels; i++ )
                {
                    std::vector<float>  prediction = predictTensorflow(&mnet->upperBody.models[i],emptyValues);
                    if (prediction.size()>0)
                        {
                            fprintf(stderr, GREEN "Caching model %u (%s) was successful\n" NORMAL,i,mnet->upperBody.models[i].modelPath);
                        }
                    else
                        {
                            fprintf(stderr,RED "Caching model %u (%s) was unsuccessful\n" NORMAL,i,mnet->upperBody.models[i].modelPath);
                        }
                }

        }

    return result;
}



int mocapnetUpperBody_unload(struct MocapNET2 * mnet)
{
    unsigned int result=0;
    for (int i=0;  i<mnet->upperBody.loadedModels; i++ )
        {
            result+=unloadTensorflow(&mnet->upperBody.models[i]);
        }

    if (result==mnet->upperBody.loadedModels)
        {
            result=1;
        }
    else
        {
            result=0;
        }

    mnet->upperBody.loadedModels=0;

    return result;
}



int mocapnetUpperBody_getOrientation(struct MocapNET2 * mnet,struct skeletonSerialized * input)
{
    mnet->upperBody.positionalInput = deriveMocapNET2InputUsingAssociations(
                                          mnet,
                                          input,
                                          &mnet->upperBodySelectionIndexesArePopulated,
                                          mnet->upperBodySelectionIndexes,
                                          mocapNET_InputLength_WithoutNSDM_upperbody,
                                          mocapNET_upperbody,
                                          0 // Enable to debug input
                                      );

    if (mnet->upperBody.perform2DAlignmentBeforeEvaluation)
    {
       unsigned int pivotJoint = 0; //Hip
       unsigned int referenceJoint = 1; //Neck

       //std::vector<float> original2DPoints  = mnet->upperBody.positionalInput;
       //rotate2DPointsBasedOnJointAsCenter(mnet->upperBody.positionalInput,20,0);

      //Force alignment of middle finger to make it easier on the neural network mnet->upperBody.positionalInput

      float angleToRotate = getAngleToAlignToZero(mnet->upperBody.positionalInput,pivotJoint,referenceJoint);
      fprintf(stderr,YELLOW "Correcting upperbody skeleton by rotating it %0.2f degrees\n" NORMAL,angleToRotate);
      rotate2DPointsBasedOnJointAsCenter(mnet->upperBody.positionalInput,angleToRotate,0);

      //debug2DPointAlignment("debugUpperAlignment",original2DPoints,mnet->upperBody.positionalInput,pivotJoint,referenceJoint,800,600);
    }


    mnet->upperBody.NSDM  = upperbodyCreateNDSM(mnet->upperBody.positionalInput,0/*NSDM Positional*/,1/*NSDM Angular */,0/*Do Scale Compensation*/);

    mnet->upperBody.neuralNetworkReadyInput.clear();
    mnet->upperBody.neuralNetworkReadyInput.insert(mnet->upperBody.neuralNetworkReadyInput.end(),mnet->upperBody.positionalInput.begin(), mnet->upperBody.positionalInput.end());
    mnet->upperBody.neuralNetworkReadyInput.insert(mnet->upperBody.neuralNetworkReadyInput.end(),mnet->upperBody.NSDM.begin(), mnet->upperBody.NSDM.end());
    //----------------------------------------------------------------------------------------------
    //The code block above should have done everything needed to make a 749 element vector
    //If input is not correct then we should stop right here
    //----------------------------------------------------------------------------------------------
    if (mnet->upperBody.neuralNetworkReadyInput.size()!=MNET_UPPERBODY_IN_NUMBER)
        {
            fprintf(stderr,RED "MocapNET: Incorrect size of MocapNET input .. \n" NORMAL);
            return 0;
        }

    int orientationReceived = MOCAPNET_ORIENTATION_NONE;

   if (mnet->options->forceFront)
    {
         mnet->orientation = MOCAPNET_ORIENTATION_FRONT;
    } else
    if (mnet->options->forceLeft)
    {
         mnet->orientation = MOCAPNET_ORIENTATION_LEFT;
    } else
    if (mnet->options->forceBack)
    {
         mnet->orientation = MOCAPNET_ORIENTATION_BACK;
    } else
    if (mnet->options->forceRight)
    {
         mnet->orientation = MOCAPNET_ORIENTATION_RIGHT;
    } else
    {
     orientationReceived = localOrientationExtraction(&mnet->upperBody,mnet->upperBody.neuralNetworkReadyInput);
     if (orientationReceived!=MOCAPNET_ORIENTATION_NONE)
        {
            mnet->orientation=orientationReceived;

            mnet->orientationClassifications[0]= mnet->upperBody.orientationClassifications[0];
            mnet->orientationClassifications[1]= mnet->upperBody.orientationClassifications[1];
            mnet->orientationClassifications[2]= mnet->upperBody.orientationClassifications[2];
            mnet->orientationClassifications[3]= mnet->upperBody.orientationClassifications[3];
        } else
        {
            fprintf(stderr,RED "MocapNET: Incorrect orientation retrieved from upperbody .. \n" NORMAL);
        }
    }

    return orientationReceived;
}


std::vector<float> mocapnetUpperBody_evaluateInput(struct MocapNET2 * mnet,struct skeletonSerialized * input)
{
    mnet->upperBody.positionalInput = deriveMocapNET2InputUsingAssociations(
                                          mnet,
                                          input,
                                          &mnet->upperBodySelectionIndexesArePopulated,
                                          mnet->upperBodySelectionIndexes,
                                          mocapNET_InputLength_WithoutNSDM_upperbody,
                                          mocapNET_upperbody,
                                          0 // Enable to debug input
                                      );

    if (mnet->upperBody.perform2DAlignmentBeforeEvaluation)
    {
       unsigned int pivotJoint = 0; //Hip
       unsigned int referenceJoint = 1; //Neck

       //std::vector<float> original2DPoints  = mnet->upperBody.positionalInput;
       //rotate2DPointsBasedOnJointAsCenter(mnet->upperBody.positionalInput,20,0);

      //Force alignment of middle finger to make it easier on the neural network mnet->upperBody.positionalInput

      float angleToRotate = getAngleToAlignToZero(mnet->upperBody.positionalInput,pivotJoint,referenceJoint);
      fprintf(stderr,YELLOW "Correcting upperbody skeleton by rotating it %0.2f degrees\n" NORMAL,angleToRotate);
      rotate2DPointsBasedOnJointAsCenter(mnet->upperBody.positionalInput,angleToRotate,0);

      //debug2DPointAlignment("debugUpperAlignment",original2DPoints,mnet->upperBody.positionalInput,pivotJoint,referenceJoint,800,600);
    }

    mnet->upperBody.NSDM  = upperbodyCreateNDSM(mnet->upperBody.positionalInput,0/*NSDM Positional*/,1/*NSDM Angular */,0/*Do Scale Compensation*/);
    mnet->upperBody.neuralNetworkReadyInput.clear();
    mnet->upperBody.neuralNetworkReadyInput.insert(mnet->upperBody.neuralNetworkReadyInput.end(),mnet->upperBody.positionalInput.begin(), mnet->upperBody.positionalInput.end());
    mnet->upperBody.neuralNetworkReadyInput.insert(mnet->upperBody.neuralNetworkReadyInput.end(),mnet->upperBody.NSDM.begin(), mnet->upperBody.NSDM.end());
    //----------------------------------------------------------------------------------------------
    //The code block above should have done everything needed to make a 749 element vector
    //If input is not correct then we should stop right here
    //----------------------------------------------------------------------------------------------
    if (mnet->upperBody.neuralNetworkReadyInput.size()!=MNET_UPPERBODY_IN_NUMBER)
        {
            fprintf(stderr,RED "MocapNET: Incorrect size of MocapNET input .. \n" NORMAL);
            std::vector<float> emptyResult;
            return emptyResult;
        }

    if (
         vectorcmp(
                    mnet->upperBody.lastNeuralNetworkReadyInput,
                    mnet->upperBody.neuralNetworkReadyInput,
                    0.1
                  )!=0
       )
    {
     //First run the network that can predict the orientation of the skeleton so we will use the correct
     //network to get the full BVH result from it
     //----------------------------------------------------------------------------------------------
     std::vector<float> result = localExecution(
                                               &mnet->upperBody,
                                                mnet->upperBody.neuralNetworkReadyInput,
                                                mnet->orientation,
                                                NN_ORIENTATIONS_TRAINED_AROUND_ZERO_AND_REQUIRE_TRICK /*Body Requires orientation trick*/
                                              );

     mnet->upperBody.result = result;
     mnet->upperBody.lastNeuralNetworkReadyInput = mnet->upperBody.neuralNetworkReadyInput;

     return result;
    } else
    {
      fprintf(stderr,GREEN "Nothing changed on upper body, returning previous result.. \n" NORMAL);
      return mnet->upperBody.result;
    }
}


int mocapnetUpperBody_fillResultVector(std::vector<float> &result,std::vector<float> resultUpperBody)
{
    if (resultUpperBody.size()==MOCAPNET_UPPERBODY_OUTPUT_NUMBER)
        {
            //This is output that targets the old body only armature..!
            //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            result[MOCAPNET_OUTPUT_HIP_XPOSITION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_HIP_XPOSITION];
            result[MOCAPNET_OUTPUT_HIP_YPOSITION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_HIP_YPOSITION];
            result[MOCAPNET_OUTPUT_HIP_ZPOSITION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_HIP_ZPOSITION];
            result[MOCAPNET_OUTPUT_HIP_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_HIP_ZROTATION];
            result[MOCAPNET_OUTPUT_HIP_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_HIP_YROTATION];
            result[MOCAPNET_OUTPUT_HIP_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_HIP_XROTATION];
            //Abdomen/Chest encoders seem to be wrong, we overwrite them..
            //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            result[MOCAPNET_OUTPUT_ABDOMEN_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_ABDOMEN_ZROTATION];
            result[MOCAPNET_OUTPUT_ABDOMEN_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_ABDOMEN_XROTATION];
            result[MOCAPNET_OUTPUT_ABDOMEN_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_ABDOMEN_YROTATION];
            result[MOCAPNET_OUTPUT_ABDOMEN_ZROTATION]=(float) 0.0;
            result[MOCAPNET_OUTPUT_ABDOMEN_XROTATION]=(float) 0.0;
            result[MOCAPNET_OUTPUT_ABDOMEN_YROTATION]=(float) 0.0;
            result[MOCAPNET_OUTPUT_CHEST_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_CHEST_ZROTATION];
            result[MOCAPNET_OUTPUT_CHEST_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_CHEST_XROTATION];
            result[MOCAPNET_OUTPUT_CHEST_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_CHEST_YROTATION];
            result[MOCAPNET_OUTPUT_CHEST_ZROTATION]=(float) 0.0;
            result[MOCAPNET_OUTPUT_CHEST_XROTATION]=(float) 0.0;
            result[MOCAPNET_OUTPUT_CHEST_YROTATION]=(float) 0.0;
            //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            result[MOCAPNET_OUTPUT_NECK_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_NECK_ZROTATION];
            result[MOCAPNET_OUTPUT_NECK_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_NECK_XROTATION];
            result[MOCAPNET_OUTPUT_NECK_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_NECK_YROTATION];
            result[MOCAPNET_OUTPUT_HEAD_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_HEAD_ZROTATION];
            result[MOCAPNET_OUTPUT_HEAD_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_HEAD_XROTATION];
            result[MOCAPNET_OUTPUT_HEAD_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_HEAD_YROTATION];
            result[MOCAPNET_OUTPUT_EYE_L_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_EYE_L_ZROTATION];
            result[MOCAPNET_OUTPUT_EYE_L_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_EYE_L_XROTATION];
            result[MOCAPNET_OUTPUT_EYE_L_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_EYE_L_YROTATION];
            result[MOCAPNET_OUTPUT_EYE_R_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_EYE_R_ZROTATION];
            result[MOCAPNET_OUTPUT_EYE_R_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_EYE_R_XROTATION];
            result[MOCAPNET_OUTPUT_EYE_R_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_EYE_R_YROTATION];
            result[MOCAPNET_OUTPUT_RSHOULDER_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_RSHOULDER_ZROTATION];
            result[MOCAPNET_OUTPUT_RSHOULDER_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_RSHOULDER_XROTATION];
            result[MOCAPNET_OUTPUT_RSHOULDER_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_RSHOULDER_YROTATION];
            result[MOCAPNET_OUTPUT_RELBOW_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_RELBOW_ZROTATION];
            result[MOCAPNET_OUTPUT_RELBOW_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_RELBOW_XROTATION];
            result[MOCAPNET_OUTPUT_RELBOW_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_RELBOW_YROTATION];
            result[MOCAPNET_OUTPUT_RHAND_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_RHAND_ZROTATION];
            result[MOCAPNET_OUTPUT_RHAND_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_RHAND_XROTATION];
            result[MOCAPNET_OUTPUT_RHAND_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_RHAND_YROTATION];
            result[MOCAPNET_OUTPUT_LSHOULDER_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_LSHOULDER_ZROTATION];
            result[MOCAPNET_OUTPUT_LSHOULDER_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_LSHOULDER_XROTATION];
            result[MOCAPNET_OUTPUT_LSHOULDER_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_LSHOULDER_YROTATION];
            result[MOCAPNET_OUTPUT_LELBOW_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_LELBOW_ZROTATION];
            result[MOCAPNET_OUTPUT_LELBOW_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_LELBOW_XROTATION];
            result[MOCAPNET_OUTPUT_LELBOW_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_LELBOW_YROTATION];
            result[MOCAPNET_OUTPUT_LHAND_ZROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_LHAND_ZROTATION];
            result[MOCAPNET_OUTPUT_LHAND_XROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_LHAND_XROTATION];
            result[MOCAPNET_OUTPUT_LHAND_YROTATION]=resultUpperBody[MOCAPNET_UPPERBODY_OUTPUT_LHAND_YROTATION];
            return 1;
        }
    else
        {
            fprintf(stderr,YELLOW "MocapNET: Execution yielded %lu items ( expected %u ) for upper body don't know what to do with them.. \n" NORMAL,resultUpperBody.size(),MOCAPNET_UPPERBODY_OUTPUT_NUMBER);
        }
    return 0;
}
