#include "../Tensorflow/tf_utils.hpp"
#include "mocapnet.hpp"
#include "nsdm.hpp"
#include "gestureRecognition.hpp"
#include "jsonCocoSkeleton.h"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


int listNodesMN(const char * label , TF_Graph* graph)
{
    size_t pos = 0;
    TF_Operation* oper;
    fprintf(stdout, "Nodes list : \n");
    while ((oper = TF_GraphNextOperation(graph, &pos)) != nullptr)
        {
            fprintf(stderr," %s - %s \n",label,TF_OperationName(oper));
            //std::cout << label<<" - "<<TF_OperationName(oper)<<" "<< std::endl;
        }
    return 1;
}

int loadMocapNET(struct MocapNET * mnet,const char * filename,float qualitySetting,int mode,unsigned int forceCPU)
{
    char modelPath[1024]= {0};
    int result = 0;
   
   //Reset pose history..
   mnet->poseHistoryStorage.maxPoseHistory=150;
   mnet->poseHistoryStorage.history.clear();
   
   if (!loadGestures(&mnet->recognizedGestures))
   {
       fprintf(stderr,RED "Failed to read recognized Gestures\n" NORMAL);
   }
  
    switch (mode)
        {


        case 3:
            mnet->mode=3;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //                                                  Regular 3 Model setup such as the BMVC 2019 Work..
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/all.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[0]  ,modelPath  ,"input_all"  ,"result_all/concat",forceCPU);
            mnet->modelLimits[0].isFlipped=0;
            mnet->modelLimits[0].numberOfLimits=1;
            mnet->modelLimits[0].minimumYaw1=-360.0;
            mnet->modelLimits[0].maximumYaw1=360.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/front.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[1],modelPath,"input_front","result_front/concat",forceCPU);
            mnet->modelLimits[1].isFlipped=0;
            mnet->modelLimits[1].numberOfLimits=1;
            mnet->modelLimits[1].minimumYaw1=-90.0;
            mnet->modelLimits[1].maximumYaw1=90.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/back.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[2] ,modelPath ,"input_back" ,"result_back/concat",forceCPU);
            mnet->modelLimits[2].isFlipped=0;
            mnet->modelLimits[2].numberOfLimits=2;
            mnet->modelLimits[2].minimumYaw1=-90.0;
            mnet->modelLimits[2].maximumYaw1=-270.0;
            mnet->modelLimits[2].minimumYaw2=90.0;
            mnet->modelLimits[2].maximumYaw2=270.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            if(result==3)
                {
                    result=1;
                    mnet->loadedModels=3;
                }
            else
                {
                    result=0;
                }
            break;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




        case 5:
            mnet->mode=5; 
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //                                                  Regular 3 Model setup such as the BMVC 2019 Work..
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/all.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[0]  ,modelPath  ,"input_all"  ,"result_all/concat",forceCPU);
            mnet->modelLimits[0].isFlipped=0;
            mnet->modelLimits[0].numberOfLimits=1;
            mnet->modelLimits[0].minimumYaw1=-360.0;
            mnet->modelLimits[0].maximumYaw1=360.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/frontA.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[1],modelPath,"input_frontA","result_frontA/concat",forceCPU);
            mnet->modelLimits[1].isFlipped=0;
            mnet->modelLimits[1].numberOfLimits=1;
            mnet->modelLimits[1].minimumYaw1=-90.0;
            mnet->modelLimits[1].maximumYaw1=0.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/frontB.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[2],modelPath,"input_frontB","result_frontB/concat",forceCPU);
            mnet->modelLimits[2].isFlipped=0;
            mnet->modelLimits[2].numberOfLimits=1;
            mnet->modelLimits[2].minimumYaw1=0.0;
            mnet->modelLimits[2].maximumYaw1=90.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/backA.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[3] ,modelPath ,"input_backA" ,"result_backA/concat",forceCPU);
            mnet->modelLimits[3].isFlipped=0;
            mnet->modelLimits[3].numberOfLimits=1;
            mnet->modelLimits[3].minimumYaw1=-90.0;
            mnet->modelLimits[3].maximumYaw1=-270.0; 

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/backB.pb",qualitySetting);//
            result += loadTensorflowInstance(&mnet->models[4] ,modelPath ,"input_backB" ,"result_backB/concat",forceCPU);
            mnet->modelLimits[4].isFlipped=0;
            mnet->modelLimits[4].numberOfLimits=1;
            mnet->modelLimits[4].minimumYaw1=-90.0;
            mnet->modelLimits[4].maximumYaw1=-270.0; 

            if(result==5)
                {
                    result=1;
                    mnet->loadedModels=5;
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
            for (int i=0; i<749; i++)
                {
                    emptyValues.push_back(0.0);
                }

            for (int i=0;  i<mnet->loadedModels; i++ )
                {
                    std::vector<float>  prediction = predictTensorflow(&mnet->models[i],emptyValues);
                    if (prediction.size()>0)
                        {
                            fprintf(stderr, GREEN "Caching and testing model %u (%s) was successful\n" NORMAL ,i,mnet->models[i].modelPath);
                        }
                    else
                        {
                            fprintf(stderr,RED "Caching and testing model %u (%s) was unsuccessful\n" NORMAL,i,mnet->models[i].modelPath);
                        }
                }
        }

    return result;
}



std::vector<float> prepareMocapNETInputFromUncompressedInput(std::vector<float> mocapnetInput)
{
    std::vector<float> mocapnetUncompressedAndCompressed;
    mocapnetUncompressedAndCompressed.clear();

    if ( (MOCAPNET_UNCOMPRESSED_JOINT_PARTS * 3!=mocapnetInput.size())||(mocapnetInput.size()!=171) )
        {
            fprintf(stderr,RED "mocapNET: prepareMocapNETInputFromUncompressedInput : wrong input size , received %lu expected 171\n" NORMAL,mocapnetInput.size());
            return mocapnetUncompressedAndCompressed;
        }


    int addSyntheticPoints=1;
    int doScaleCompensation=1;
    std::vector<float> mocapnetCompressed = compressMocapNETInputToNSDM(mocapnetInput,addSyntheticPoints,doScaleCompensation);


    mocapnetUncompressedAndCompressed.insert(mocapnetUncompressedAndCompressed.end(), mocapnetInput.begin(), mocapnetInput.end());
    mocapnetUncompressedAndCompressed.insert(mocapnetUncompressedAndCompressed.end(), mocapnetCompressed.begin(), mocapnetCompressed.end());
    //mocapnetUncompressedAndCompressed.insert(mocapnetInput.end(), mocapnetCompressed.begin(), mocapnetCompressed.end());
    //std::cerr<<"done \n";
    return  mocapnetUncompressedAndCompressed;
}



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


std::vector<float>  MNET3Classes(struct MocapNET * mnet,std::vector<float> mnetInput,std::vector<float> direction )
{
    std::vector<float> emptyResult;
    
    if (direction.size()>0)
        { 
            fprintf(stderr,NORMAL "Direction is : %0.2f " NORMAL , direction[0] );
            if ( (direction[0]<-90) || (direction[0]>90) )
                {
                    //Back ----------------------------------------------=
                    fprintf(stderr,"Back\n");
                    std::vector<float> result = predictTensorflow(&mnet->models[2],mnetInput);
                    if (result.size()>4)
                            { result[4]=undoOrientationTrickForBackOrientation(result[4]); }
                    return result;
                }
            else
                {
                    //Front ----------------------------------------------
                    fprintf(stderr,"Front\n");
                    std::vector<float> result = predictTensorflow(&mnet->models[1],mnetInput);
                    return result;
                } 
        }
    return emptyResult;       
}



std::vector<float>  MNET5Classes(struct MocapNET * mnet,std::vector<float> mnetInput,std::vector<float> direction )
{
    fprintf(stderr,NORMAL "5Class  Direction is : %0.2f " NORMAL , direction[0] );
    std::vector<float> result;
    
    if (direction.size()>0)
        {
    
            if ( (direction[0]>=-90) && (direction[0]<=0) )
                {
                    //Front ----------------------------------------------
                    fprintf(stderr,"Front A\n");
                    result = predictTensorflow(&mnet->models[1],mnetInput); 
                }
            else
            if ( (direction[0]<=90) && (direction[0]>=0) )
                {
                    //Front ----------------------------------------------
                    fprintf(stderr,"Front B\n");
                    result = predictTensorflow(&mnet->models[2],mnetInput); 
                }
            else
            if ( (direction[0]<=-90) && (direction[0]>=-180) )
                {
                    //Back  ----------------------------------------------
                    fprintf(stderr,"Back A\n");
                    result = predictTensorflow(&mnet->models[3],mnetInput); 
                     if (result.size()>4)
                        { result[4]=undoOrientationTrickForBackOrientation(result[4]); }
                } 
               else
            if ( (direction[0]>=90) && (direction[0]<=180) )
                {
                    //Back  ----------------------------------------------
                    fprintf(stderr,"Back B\n");
                    result = predictTensorflow(&mnet->models[4],mnetInput); 
                     if (result.size()>4)
                        {result[4]=undoOrientationTrickForBackOrientation(result[4]); }
                }  else
                {
                    fprintf(stderr,RED "Unhandled orientation \n" NORMAL);
                }
        }
       return result;         
}


int silenceDeadJoints(std::vector<float> & result)
{
  if (result.size()!=MOCAPNET_OUTPUT_NUMBER)
  {
     fprintf(stderr,RED "silenceDeadJoints: There is an inconsistency in the number of outputs\n" NORMAL); 
     return 0;
  }
    
 result[MOCAPNET_OUTPUT_LEFTEYE_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LEFTEYE_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LEFTEYE_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RIGHTEYE_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RIGHTEYE_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RIGHTEYE_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RCOLLAR_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RCOLLAR_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RCOLLAR_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RTHUMB1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RTHUMB1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RTHUMB1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RTHUMB2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RTHUMB2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RTHUMB2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RINDEX1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RINDEX1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RINDEX1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RINDEX2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RINDEX2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RINDEX2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RMID1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RMID1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RMID1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RMID2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RMID2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RMID2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RRING1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RRING1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RRING1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RRING2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RRING2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RRING2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RPINKY1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RPINKY1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RPINKY1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RPINKY2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RPINKY2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RPINKY2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LCOLLAR_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LCOLLAR_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LCOLLAR_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LTHUMB1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LTHUMB1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LTHUMB1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LTHUMB2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LTHUMB2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LTHUMB2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LINDEX1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LINDEX1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LINDEX1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LINDEX2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LINDEX2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LINDEX2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LMID1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LMID1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LMID1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LMID2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LMID2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LMID2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LRING1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LRING1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LRING1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LRING2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LRING2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LRING2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LPINKY1_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LPINKY1_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LPINKY1_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LPINKY2_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LPINKY2_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LPINKY2_YROTATION]=0;
 result[MOCAPNET_OUTPUT_RBUTTOCK_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_RBUTTOCK_XROTATION]=0;
 result[MOCAPNET_OUTPUT_RBUTTOCK_YROTATION]=0;
 result[MOCAPNET_OUTPUT_LBUTTOCK_ZROTATION]=0;
 result[MOCAPNET_OUTPUT_LBUTTOCK_XROTATION]=0;
 result[MOCAPNET_OUTPUT_LBUTTOCK_YROTATION]=0;
 
  return 1;  
}


std::vector<float> runMocapNET(struct MocapNET * mnet,std::vector<float> input,int doGestureDetection,int doOutputFiltering)
{
    std::vector<float> emptyResult;

    if (mnet==0)
    {
            fprintf(stderr,"MocapNET: invalid initialization\n");
           return emptyResult;        
    }
    
    std::vector<float> mnetInput;

    if (input.size()==749)
        {
            fprintf(stderr,"MocapNET: Input was given precompressed\n");
            mnetInput = input;
        }
    else if (input.size()==171)
        {
            //This is the default case so dont issue any warnings..
            //std::cerr<<"MocapNET: COCO input has "<<input.size()<<" elements (should be 171)\n";
            mnetInput = prepareMocapNETInputFromUncompressedInput(input);
            //std::cerr<<"MocapNET: COCO 171 input has been converted in to MocapNET input with "<<mnetInput.size()<<" elements (should be 749)\n";
        }
    else if (input.size()!=171)
        {
            fprintf(stderr,"MocapNET: Incorrect size of COCO input  was %lu (but should be 171) \n",input.size());
            return emptyResult;
        }

    if (mnetInput.size()!=749)
        {
            fprintf(stderr,"MocapNET: Incorrect size of MocapNET input .. \n");
            return emptyResult;
        }

    std::vector<float> direction = predictTensorflow(&mnet->models[0],mnetInput);

    if (direction.size()>0)
        {
            std::vector<float>  result;
            if (mnet->mode==3)
            {
                result=MNET3Classes(mnet,mnetInput,direction);
            } else
            if (mnet->mode==5)
            {
                result=MNET5Classes(mnet,mnetInput,direction);
            }    
            
            addToMotionHistory(&mnet->poseHistoryStorage,result);
             
             if (doGestureDetection)
             {
             int gestureDetected=compareHistoryWithKnownGestures(
                                                                                                                                             &mnet->recognizedGestures,
                                                                                                                                             &mnet->poseHistoryStorage,
                                                                                                                                             85.0,//Percentage complete..
                                                                                                                                             23.0 //Angle thrshold
                                                                                                                                         );
                                                                
             if (gestureDetected!=0)
             {
                mnet->lastActivatedGesture=gestureDetected;
                mnet->gestureTimestamp=mnet->recognizedGestures.gestureChecksPerformed;
                fprintf(stderr,GREEN "Gesture Detection : %u\n" NORMAL,gestureDetected);
              } 
             }                                                               
            
            silenceDeadJoints(result);
            
            return result;
        }
    else
        {
            fprintf(stderr,"Unable to predict pose direction..\n");
        }

//-----------------
    return emptyResult;
}




int unloadMocapNET(struct MocapNET * mnet)
{
    unsigned int result=0;
    for (int i=0;  i<mnet->loadedModels; i++ )
        {
            result+=unloadTensorflow(&mnet->models[i]);
        }

    if (result==mnet->loadedModels)
        {
            result=1;
        }
    else
        {
            result=0;
        }
    mnet->loadedModels=0;

    return result;
}
