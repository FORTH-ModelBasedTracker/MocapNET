#include "../Tensorflow/tf_utils.hpp"

#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/nsdm.hpp"
#include "../MocapNETLib/remoteExecution.hpp"
#include "../MocapNETLib/gestureRecognition.hpp"
#include "../MocapNETLib/jsonCocoSkeleton.h"

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
        }
    return 1;
}


std::vector <float> fillInTheBlanks(std::vector <float> previousVector,std::vector <float> currentVector)
{
    //fprintf(stderr,"filling In Blanks from previous 2D detection:\n");

    if (previousVector.size()==currentVector.size())
    {
        std::vector <float> smoothedVector=currentVector;
        unsigned int filledBlanks=0;
        unsigned int observedBlanks=0;
        int i=0;
        for (int i=0; i<smoothedVector.size()/3; i++)
             {
               if (
                    //(smoothedVector[i*3+0]==0) &&
                    //(smoothedVector[i*3+1]==0) &&
                    (smoothedVector[i*3+2]==0)
                  )
                  {
                    ++observedBlanks;
                    //fprintf(stderr,"Joint %s is blank (%0.2f,%0.2f,%0.2f)\n",MocapNETInputUncompressedArrayNames[i*3+2],smoothedVector[i*3+0],smoothedVector[i*3+1],smoothedVector[i*3+2]);
                    //We have a hole..! Can we fill it?
                    if (
                        //(previousVector[i*3+0]!=0) &&
                        //(previousVector[i*3+1]!=0) &&
                        (previousVector[i*3+2]!=0)
                       )
                       {
                         smoothedVector[i*3+0]=previousVector[i*3+0];
                         smoothedVector[i*3+1]=previousVector[i*3+1];
                         smoothedVector[i*3+2]=previousVector[i*3+2];
                         fprintf(stderr,GREEN "2DJoint %s got filled (%0.2f,%0.2f,%0.2f)\n" NORMAL,MocapNETInputUncompressedArrayNames[i*3+2],smoothedVector[i*3+0],smoothedVector[i*3+1],smoothedVector[i*3+2]);
                         ++filledBlanks;
                       }
                  } else
                  {
                    //fprintf(stderr,"%s is not blank (%0.2f,%0.2f,%0.2f)\n",MocapNETInputUncompressedArrayNames[i*3+2],smoothedVector[i*3+0],smoothedVector[i*3+1],smoothedVector[i*3+2]);
                  }
             }

        fprintf(stderr,"observed %u blanks / filled %u blanks\n",observedBlanks,filledBlanks);
        return smoothedVector;
    }

    fprintf(stderr,"nothing done\n");
    return currentVector;
}



std::vector <float> smoothVector(std::vector <float> previousVector,std::vector <float> currentVector,float magnitude)
{
    if (previousVector.size()==currentVector.size())
    {
        std::vector <float> smoothedVector;
        int i=0;
        for (i=0; i<previousVector.size(); i++)
        {
            float smoothedValue =  currentVector[i]+ ( currentVector[i]-previousVector[i] ) * magnitude;

            smoothedVector.push_back(smoothedValue);
        }
        return smoothedVector;
    }
    return currentVector;
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


void commonInitialization(struct MocapNET * mnet)
{ 
   //Reset pose history..
   mnet->poseHistoryStorage.maxPoseHistory=150;
   mnet->poseHistoryStorage.history.clear();
   
   if (!loadGestures(&mnet->recognizedGestures))
   {
       fprintf(stderr,RED "Failed to read recognized Gestures\n" NORMAL);
   }
   
  fprintf(stderr,"Initializing output filters : "); 
  float approximateFramerate = 30.0;
  //---------------------------------------------------
  initButterWorth(&mnet->directionSignal,approximateFramerate,5.0);
  for (int i=0;  i<MOCAPNET_OUTPUT_NUMBER; i++ )
                {
                   fprintf(stderr,"."); 
                   initButterWorth(&mnet->outputSignals[i],approximateFramerate,5.0);     
                }
  fprintf(stderr,"\n");    
}


int connectToMocapNETServer(struct MocapNET * mnet,const char * url,int port)
{
   mnet->useRemoteMocapNETServer=1;
   snprintf(mnet->remoteMocapNETServerURL,128,"%s",url);
   mnet->remoteMocapNETServerPort=port;
   
   mnet->remoteContext =  intializeRemoteExecution(url,port,10000);
   
   commonInitialization(mnet);
   //Do a connection test here..
   return 1;
}




int loadMocapNET(struct MocapNET * mnet,const char * filename,float qualitySetting,int mode,unsigned int forceCPU)
{
    char modelPath[1024]= {0};
    int result = 0;
   
    
   commonInitialization(mnet);
   
    switch (mode)
        {
        case 3:
            mnet->mode=3;
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //                                                  Regular 3 Model setup such as the BMVC 2019 Work..
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mode%u/%0.1f/all.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->models[0]  ,modelPath  ,"input_all"  ,"result_all/concat",forceCPU);
            mnet->modelLimits[0].isFlipped=0;
            mnet->modelLimits[0].numberOfLimits=1;
            mnet->modelLimits[0].minimumYaw1=-360.0;
            mnet->modelLimits[0].maximumYaw1=360.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mode%u/%0.1f/front.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->models[1],modelPath,"input_front","result_front/concat",forceCPU);
            mnet->modelLimits[1].isFlipped=0;
            mnet->modelLimits[1].numberOfLimits=1;
            mnet->modelLimits[1].minimumYaw1=-90.0;
            mnet->modelLimits[1].maximumYaw1=90.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mode%u/%0.1f/back.pb",mode,qualitySetting);
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
            snprintf(modelPath,1024,"dataset/combinedModel/mode%u/%0.1f/all.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->models[0]  ,modelPath  ,"input_all"  ,"result_all/concat",forceCPU);
            mnet->modelLimits[0].isFlipped=0;
            mnet->modelLimits[0].numberOfLimits=1;
            mnet->modelLimits[0].minimumYaw1=-360.0;
            mnet->modelLimits[0].maximumYaw1=360.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mode%u/%0.1f/front.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->models[1],modelPath,"input_front","result_front/concat",forceCPU);
            mnet->modelLimits[1].isFlipped=0;
            mnet->modelLimits[1].numberOfLimits=1;
            mnet->modelLimits[1].minimumYaw1=-45.0;
            mnet->modelLimits[1].maximumYaw1=45.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mode%u/%0.1f/back.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->models[2],modelPath,"input_back","result_back/concat",forceCPU);
            mnet->modelLimits[2].isFlipped=0;
            mnet->modelLimits[2].numberOfLimits=1;
            mnet->modelLimits[2].minimumYaw1=135.0;//-90.0;
            mnet->modelLimits[2].maximumYaw1=225.0;//-270.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mode%u/%0.1f/left.pb",mode,qualitySetting);
            result += loadTensorflowInstance(&mnet->models[3] ,modelPath ,"input_left" ,"result_left/concat",forceCPU);
            mnet->modelLimits[3].isFlipped=0;
            mnet->modelLimits[3].numberOfLimits=1;
            mnet->modelLimits[3].minimumYaw1=-135.0;
            mnet->modelLimits[3].maximumYaw1=45.0; 

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"dataset/combinedModel/mode%u/%0.1f/right.pb",mode,qualitySetting);//
            result += loadTensorflowInstance(&mnet->models[4] ,modelPath ,"input_right" ,"result_right/concat",forceCPU);
            mnet->modelLimits[4].isFlipped=0;
            mnet->modelLimits[4].numberOfLimits=1;
            mnet->modelLimits[4].minimumYaw1=45.0;
            mnet->modelLimits[4].maximumYaw1=135.0; 

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
            //---------------------------------------------------
            for (int i=0;  i<mnet->loadedModels; i++ )
                {
                    std::vector<float>  prediction = predictTensorflow(&mnet->models[i],emptyValues);
                    if (prediction.size()>0)
                        {
                            fprintf(stderr, GREEN "Caching model %u (%s) was successful\n" NORMAL ,i,mnet->models[i].modelPath);
                        }
                    else
                        {
                            fprintf(stderr,RED "Caching model %u (%s) was unsuccessful\n" NORMAL,i,mnet->models[i].modelPath);
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
        } else 
    if (orientation<0)
        {
         orientation=180-orientation;
        } else
        {
         orientation=180-orientation;
        }
    return orientation;
}



int  getMocapNETOrientationFromOutputVector(std::vector<float> direction)
{
  if (direction.size()>0)
        {
            float orientation = direction[0];
              
             if ( (orientation>=-45.0) && (orientation<=45.0) )
                {
                    //Front ---------------------------------------------- 
                    return MOCAPNET_ORIENTATION_FRONT;
                }
            else
            if ( (orientation>=45.0) && (orientation<=135.0) )
                {
                    //Right ----------------------------------------------
                    return MOCAPNET_ORIENTATION_RIGHT;
                }
            else
            if ( (orientation>=-135.0) && (orientation<=-45.0) )
                {
                    //Left ---------------------------------------------- 
                    return MOCAPNET_ORIENTATION_LEFT;  
                }
            else
            if ( (orientation<=-135) && (orientation>=-225) )
                {
                    //Back  ----------------------------------------------
                    return MOCAPNET_ORIENTATION_BACK;
                }
               else
            if ( (orientation>=135) && (orientation<=225) )
                {
                    //Back  ---------------------------------------------- 
                    return MOCAPNET_ORIENTATION_BACK;
                }  else
                {
                    fprintf(stderr,RED "[Unhandled orientation]\n" NORMAL);
                    fprintf(stderr,"This probably means difficult input\n");
                    return MOCAPNET_ORIENTATION_BACK;
                }
        }
  fprintf(stderr,RED "Empty Direction Vector\n" NORMAL);
  return MOCAPNET_ORIENTATION_NONE;
}
 

std::vector<float>  MNET3Classes(struct MocapNET * mnet,std::vector<float> mnetInput,std::vector<float> direction )
{
    std::vector<float> result;
    
    if (direction.size()>0)
        { 
            fprintf(stderr,NORMAL "Direction is : %0.2f " NORMAL , direction[0] );
            //===========================================================
            if ( (direction[0]<-90) || (direction[0]>90) )
                {
                    //Back ----------------------------------------------=
                    fprintf(stderr,"Back\n");
                    result = predictTensorflow(&mnet->models[2],mnetInput);
                    if (result.size()>4)
                            { result[4]=undoOrientationTrickForBackOrientation(result[4]); }
                } else
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


std::vector<float>  MNET5Classes(struct MocapNET * mnet,std::vector<float> mnetInput,std::vector<float> direction )
{
    std::vector<float> result;
    
    if (direction.size()>0)
        {
            fprintf(stderr,NORMAL "5Class  Direction is : %0.2f " NORMAL , direction[0] );
            //Output of each Neural Network is -45.0  to 0.0 to 45.0
            //We need to correct it ..
            
            int orientation = getMocapNETOrientationFromOutputVector(direction);
            switch (orientation)
            {
                //=========================================================== 
                case MOCAPNET_ORIENTATION_FRONT: 
                    fprintf(stderr,"Front\n");
                    result = predictTensorflow(&mnet->models[1],mnetInput); 
                break; 
                //===========================================================
                case MOCAPNET_ORIENTATION_BACK:
                    fprintf(stderr,"Back\n");
                    result = predictTensorflow(&mnet->models[2],mnetInput); 
                     if (result.size()>4)
                        { 
                          fprintf(stderr,"Orientation changed from %0.2f ",result[4]); 
                          //result[4]=undoOrientationTrickForBackOrientation(result[4]); 
                          result[4]-=180.0; 
                          fprintf(stderr,"to %0.2f\n ",result[4]); 
                        }
                break; 
                //===========================================================
                case MOCAPNET_ORIENTATION_LEFT:
                    fprintf(stderr,"Left\n");
                    result = predictTensorflow(&mnet->models[3],mnetInput); 
                     if (result.size()>4)
                        { 
                          fprintf(stderr,"Orientation changed from %0.2f ",result[4]); 
                          result[4]-=90.0; 
                          fprintf(stderr,"to %0.2f\n ",result[4]);  
                        }                    
                break; 
                //===========================================================
                case MOCAPNET_ORIENTATION_RIGHT:
                   result = predictTensorflow(&mnet->models[4],mnetInput); 
                     if (result.size()>4)
                        { 
                         fprintf(stderr,"Orientation changed from %0.2f ",result[4]); 
                         result[4]+=90.0;
                         fprintf(stderr,"to %0.2f\n ",result[4]);  
                        }
                break; 
                //===========================================================
                default :
                    fprintf(stderr,RED "Unhandled orientation, using front as a last resort \n" NORMAL);
                    result = predictTensorflow(&mnet->models[1],mnetInput); 
                break;
                //=========================================================== 
            };
        } else
        {
            fprintf(stderr,NORMAL "5Class  Direction is not defined\n" NORMAL ); 
        }
       return result;         
}




std::vector<float> localExecution(struct MocapNET * mnet,std::vector<float> mnetInput,int doOutputFiltering)
{
    std::vector<float> result; 
    std::vector<float> direction = predictTensorflow(&mnet->models[0],mnetInput); 
    
    if ( (doOutputFiltering) && (direction.size()>0) )
    {
        direction[0] = filter(&mnet->directionSignal,direction[0]);
    } 
    //----------------------------------------------------------------------------------------------
    if (direction.size()>0)
        {
            mnet->lastSkeletonOrientation=direction[0];
            
            switch(mnet->mode)
            {
              case 3: result=MNET3Classes(mnet,mnetInput,direction); break;   
              case 5: result=MNET5Classes(mnet,mnetInput,direction); break;
              //-----------------------------------------------------------
              default:
               fprintf(stderr,RED "MocapNET: Incorrect Mode %u ..\n" NORMAL,mnet->mode); 
              break; 
            };
        } else
        {
         fprintf(stderr,RED "Unable to predict pose direction..\n" NORMAL);
        }
   return result;
}


std::vector<float> runMocapNET(struct MocapNET * mnet,std::vector<float> input,int doGestureDetection,int doOutputFiltering)
{
    std::vector<float> mnetInput;
    std::vector<float> emptyResult;

    if (mnet==0)
    {
           fprintf(stderr,RED "MocapNET: Cannot work without initialization\n" NORMAL);
           return emptyResult;        
    }
    

    //Check if input is ok or if prepareMocapNETInputFromUncompressedInput is needed
    //----------------------------------------------------------------------------------------------
    if (input.size()==749)
        {
            fprintf(stderr,GREEN "MocapNET: Input was given precompressed\n" NORMAL);
            mnetInput = input;
        } else 
    if (input.size()==171)
        {
            //This is the default case so dont issue any warnings..
            //std::cerr<<"MocapNET: COCO input has "<<input.size()<<" elements (should be 171)\n";
            mnetInput = prepareMocapNETInputFromUncompressedInput(input);
            //std::cerr<<"MocapNET: COCO 171 input has been converted in to MocapNET input with "<<mnetInput.size()<<" elements (should be 749)\n";
        } else 
    if (input.size()!=171)
        {
            fprintf(stderr,RED "MocapNET: Incorrect size of COCO input  was %lu (but should be 171) \n" NORMAL,input.size());
            return emptyResult;
        }
    //----------------------------------------------------------------------------------------------
        
    //The code block above should have done everything needed to make a 749 element vector
    //If input is not correct then we should stop right here
    //----------------------------------------------------------------------------------------------
    if (mnetInput.size()!=749)
        {
            fprintf(stderr,RED "MocapNET: Incorrect size of MocapNET input .. \n" NORMAL);
            return emptyResult;
        }

    //First run the network that can predict the orientation of the skeleton so we will use the correct
    //network to get the full BVH result from it 
    //----------------------------------------------------------------------------------------------
    std::vector<float> result; 
    
    
    if (mnet->useRemoteMocapNETServer)
    {
      //Send smaller input so that the network is not spammed
      result = remoteExecution(mnet,input); 
    } else
    {
      result = localExecution(mnet,mnetInput,doOutputFiltering); 
    }
    


//---------------------------------------------------------------------------------------------------------------------------------------------    
//The original armature of the BMVC 2019 paper used the DAZ friendly CMU BVH conversion armature for datasets 01-19
//https://www.sites.google.com/a/cgspeed.com/cgspeed/motion-capture/cmu-bvh-conversion/
//I have added a head on top of it, in order to keep everything compatible  we detect if we use an
//old 132 d.o.f neural network and adding some zeros to pad the empty head joints.. 
//---------------------------------------------------------------------------------------------------------------------------------------------    
    if (result.size()==132)
    {    //This is output that targets the old body only armature..!
          //We will add 78 - (6 we had eyes before) empty joints @ joint 15
          
         std::vector<float> emptyHeadJoints;
         for (int i=0; i<234-15-9; i++)
         {
             emptyHeadJoints.push_back(0.0);
         }
        std::vector<float> resultOld=result; 
        
        result.insert(result.begin()+15, emptyHeadJoints.begin(), emptyHeadJoints.end());
         
         /*
         fprintf(stderr,"Old Result size is %lu \n",result.size());
         fprintf(stderr,"New Result size is %lu \n",result.size());
         for (int i =0; i<resultOld.size(); i++)
         {  fprintf(stderr,"i=%u / old=%0.2f / new=%0.2f \n",i,resultOld[i],result[i]); }
         for (int i =resultOld.size(); i<result.size();  i++)
         { fprintf(stderr,"i=%u / old= - / new=%0.2f \n",i,result[i]); }
         */ 
    }
//---------------------------------------------------------------------------------------------------------------------------------------------    
    
    
    //Debugging visualization
    //----------------------------------------------------------------------------
    /*
    fprintf(stderr,"runMocapNET: Evaluation yielded %lu results\n",result.size());
    for (int i=0; i<result.size(); i++)
    {
       fprintf(stderr,"%u - %s - %0.2f\n",i,MocapNETOutputArrayNames[i],result[i]); 
    }
    */
    //----------------------------------------------------------------------------
    
             
    if (result.size()>0)
        {
            if (doOutputFiltering)
            {
              if (result.size()==MOCAPNET_OUTPUT_NUMBER)  
              {
                for (int i=0;  i<result.size(); i++ )
                   {
                     result[i] = filter(&mnet->outputSignals[i],result[i]);  
                   }
              } else
              {
               fprintf(stderr,RED "MocapNET: Incorrect number of output elements/Cannot filter output as a result..!\n" NORMAL);   
               fprintf(stderr,RED "Result size = %lu , MocapNET output = %u \n" NORMAL,result.size(),MOCAPNET_OUTPUT_NUMBER); 
              }
            }
            
            addToMotionHistory(&mnet->poseHistoryStorage,result);
             //----------------------------------------------------------------------------------------------
             if (doGestureDetection)
             {
             int gestureDetected=compareHistoryWithKnownGestures(
                                                                 &mnet->recognizedGestures,
                                                                 &mnet->poseHistoryStorage,
                                                                 80.0,//Percentage complete..
                                                                 20.0 //Angle thrshold
                                                                );
             if (gestureDetected!=0)
              {
                mnet->lastActivatedGesture=gestureDetected;
                mnet->gestureTimestamp=mnet->recognizedGestures.gestureChecksPerformed;
                fprintf(stderr,GREEN "Gesture Detection : %u\n" NORMAL,gestureDetected);
              } 
             }
            //----------------------------------------------------------------------------------------------
            
            //Force parts of the output to zero.. 
            //----------------------------------------------------------------------------------------------
            silenceDeadJoints(result);
            //----------------------------------------------------------------------------------------------
            
            return result;
        } 

    fprintf(stderr,RED "MocapNET: failed to retrieve a result..\n" NORMAL);
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
