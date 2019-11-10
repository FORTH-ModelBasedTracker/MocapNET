#include "jointEstimator2D.h"
#include <stdio.h>

int loadJointEstimator2D(struct JointEstimator2D * jnet,int joint2DEstimatorSelected,int usePAFs,unsigned int forceCPU)
{ 
    const char   networkPathOpenPoseMiniStatic[]="dataset/combinedModel/openpose_model.pb";
    const char   networkPathVnectStatic[]="dataset/combinedModel/vnect_sm_pafs_8.1k.pb";
    const char   networkPathFORTHStatic[]="dataset/combinedModel/mobnet2_tiny_vnect_sm_1.9k.pb"; 
    char * networkPath = (char*) networkPathFORTHStatic;
    unsigned int joint2DSensitivityPercent=30;
    
    char   networkInputLayer[]="input_1";
    char   networkOutputLayer[]="k2tfout_0";
    unsigned int numberOfOutputTensors = 3;
    
    
    
    //-----------------------------------------------------------------------------
    switch (joint2DEstimatorSelected)
    {
     case JOINT_2D_ESTIMATOR_FORTH : 
                     networkPath=(char*) networkPathFORTHStatic;
                    networkOutputLayer[8]='0';
                    joint2DSensitivityPercent=30;
                    numberOfOutputTensors = 3;
     break;
     case JOINT_2D_ESTIMATOR_VNECT : networkPath = (char*) networkPathVnectStatic;
                    networkOutputLayer[8]='1';
                    joint2DSensitivityPercent=20;
                    numberOfOutputTensors = 4;
     break;
     case JOINT_2D_ESTIMATOR_OPENPOSE :
                    networkPath=(char*) networkPathOpenPoseMiniStatic;
                    networkOutputLayer[8]='1';
                    joint2DSensitivityPercent=40;
                    numberOfOutputTensors = 4; 
     break; 
     default :
      fprintf(stderr,"No 2D Joint Estimator Selected..\n");
     break;
    };
    //-----------------------------------------------------------------------------

    if (
        loadTensorflowInstance(
                               &jnet->network,
                               networkPath,
                               networkInputLayer,
                               networkOutputLayer,
                               forceCPU
                              )
        ) { return 1; } 
        
        
    //-----------------------------------------------------------------------------
  return 0;
}



int unloadJointEstimator2D(struct JointEstimator2D * jnet)
{
  return unloadTensorflow(&jnet->network);
}


int  estimate2DSkeletonsFromImage(struct JointEstimator2D * jnet,struct Skeletons2DDetected * result,char * rgbData,unsigned int width,unsigned int height)
{
    /*
 unsigned long startTime  = GetTickCountMicroseconds();
    std::vector<cv::Point_<float> > pointsOf2DSkeleton = predictAndReturnSingleSkeletonOf2DCOCOJoints(
                &jnet->network,
                bgr,
                minThreshold,
                visualize,
                saveVisualization,
                frameNumber,
                inputWidth2DJointDetector,
                inputHeight2DJointDetector,
                heatmapWidth2DJointDetector,
                heatmapHeight2DJointDetector,
                numberOfHeatmaps,
                numberOfOutputTensors
            );
    unsigned long endTime = GetTickCountMicroseconds();
    *fps = convertStartEndTimeFromMicrosecondsToFPS(startTime,endTime);    */
  return 0;  
}