#pragma once
/** @file mocapnet.hpp
 *  @brief The MocapNET C library
 *  As seen in https://www.youtube.com/watch?v=fH5e-KMBvM0 , the MocapNET network requires two types of input.
 *  The first is an uncompressed list of (x,y,v) joints and the second an NSDM array. To add to those the output consists of BVH
 *  frames that must be accompanied by a header. This library internally handles all of these details.
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include "../Tensorflow/tensorflow.hpp"
#include "../MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp"

#include <iostream>
#include <vector>

 



/**
 * @brief 2D Skeletons Retreieved
 */
struct Skeletons2DDetected
{ 
   unsigned int numberOfSkeletonsDetected;
   struct skeletonStructure skeletons[16];
};



struct InputImageCrop
{
   unsigned int offsetX;
   unsigned int offsetY;
   unsigned int croppedDimensionWidth;
   unsigned int croppedDimensionHeight;

   //cropBBox,
   unsigned int frameWidth;
   unsigned int frameHeight;
};
 
/**
 * @brief MocapNET consists of separate classes/ensembles that are invoked for particular orientations.
 * This structure holds the required tensorflow instances to make MocapNET work.
 */
struct JointEstimator2D
{
   struct TensorflowInstance network;
   char * networkPath;
   float joint2DSensitivityPercent;
   unsigned int numberOfOutputTensors;
   unsigned int heatmapWidth2DJointDetector;
   unsigned int heatmapHeight2DJointDetector;
   unsigned int inputWidth2DJointDetector;
   unsigned int inputHeight2DJointDetector;
   unsigned int numberOfHeatmaps; 
    
    struct InputImageCrop crop;
    int doCrop;
};




/**
 * @brief Each Blob is a rectangular area..
 * with one peak..
 */
struct HeatmapBlob
{ 
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;

  unsigned int peakX;
  unsigned int peakY;
  float peakValue;
  
  float subpixelPeakX;
  float subpixelPeakY;
};


/**
 * @brief This is a list of included Joint 2D Estimators included.
 */
enum JOINT_2D_ESTIMATOR_SELECTED
{
 JOINT_2D_ESTIMATOR_NONE=0,
 JOINT_2D_ESTIMATOR_FORTH,
 JOINT_2D_ESTIMATOR_VNECT,
 JOINT_2D_ESTIMATOR_OPENPOSE, 
 //----------------------------- 
 JOINT_2D_ESTIMATOR_NUMBER
};


/**
 * @brief This is an array of names for the input Joints expected from the utilities.
 */
static const char * UT_COCOBodyNames[] =
{
  "Nose",        //0
  "Neck",        //1
  "RShoulder",   //2
  "RElbow",      //3
  "RWrist",      //4
  "LShoulder",   //5
  "LElbow",      //6
  "LWrist",      //7
  "RHip",        //8
  "RKnee",       //9
  "RAnkle",      //10
  "LHip",        //11
  "LKnee",       //12
  "LAnkle",      //13
  "REye",        //14
  "LEye",        //15
  "REar",        //16
  "LEar",        //17
  "Bkg",         //18
//=================
    "End of Joint Names"
};



/**
 * @brief This is a list of included Joint 2D Estimators included.
 */
enum JOINT_2D_ESTIMATOR_PART_LABEL
{
 JOINT_2D_ESTIMATOR_PART_OF_NOTHING=0,
 JOINT_2D_ESTIMATOR_PART_OF_TORSO,
 JOINT_2D_ESTIMATOR_PART_OF_HEAD,
 JOINT_2D_ESTIMATOR_PART_OF_LEFT,
 JOINT_2D_ESTIMATOR_PART_OF_RIGHT,
 //----------------------------- 
 JOINT_2D_ESTIMATOR_NUMBER_OF_PARTS
};


/**
 * @brief We want to distinguish limbs with labels ( to color them )
 */
static const int UT_COCOSkeletonJointsPartLabel[] =
{
    // Part of                                                                                                          Joint
  JOINT_2D_ESTIMATOR_PART_OF_TORSO,                        //UT_COCO_Nose,
  JOINT_2D_ESTIMATOR_PART_OF_TORSO,                        //UT_COCO_Neck,
  JOINT_2D_ESTIMATOR_PART_OF_RIGHT,                        //UT_COCO_RShoulder,
  JOINT_2D_ESTIMATOR_PART_OF_RIGHT,                   //UT_COCO_RElbow,
  JOINT_2D_ESTIMATOR_PART_OF_RIGHT,                      //UT_COCO_RWrist,
  JOINT_2D_ESTIMATOR_PART_OF_LEFT,                        //UT_COCO_LShoulder,
  JOINT_2D_ESTIMATOR_PART_OF_LEFT,                   //UT_COCO_LElbow,
  JOINT_2D_ESTIMATOR_PART_OF_LEFT,                      //UT_COCO_LWrist,
  JOINT_2D_ESTIMATOR_PART_OF_RIGHT,                        //UT_COCO_RHip,
  JOINT_2D_ESTIMATOR_PART_OF_RIGHT,                        //UT_COCO_RKnee,
  JOINT_2D_ESTIMATOR_PART_OF_RIGHT,                       //UT_COCO_RAnkle,
  JOINT_2D_ESTIMATOR_PART_OF_LEFT,                        //UT_COCO_LHip,
  JOINT_2D_ESTIMATOR_PART_OF_LEFT,                        //UT_COCO_LKnee,
  JOINT_2D_ESTIMATOR_PART_OF_LEFT,                       //UT_COCO_LAnkle,
  JOINT_2D_ESTIMATOR_PART_OF_HEAD,                        //UT_COCO_REye,
  JOINT_2D_ESTIMATOR_PART_OF_HEAD,                        //UT_COCO_LEye,
  JOINT_2D_ESTIMATOR_PART_OF_HEAD,                        //UT_COCO_REar,
  JOINT_2D_ESTIMATOR_PART_OF_HEAD,                        //UT_COCO_LEar,
  JOINT_2D_ESTIMATOR_PART_OF_NOTHING                        //UT_COCO_Bkg
};



/**
 * @brief This is a programmer friendly enumerator of joint names expected from the utilities ( hence the prefix UT_ ) .
 * Please notice that these are not necessarily the same when converting to different data formats ( COCO / BODY25 / MocapNET etc )
 */
enum UT_COCOSkeletonJoints
{
  UT_COCO_Nose=0,
  UT_COCO_Neck,
  UT_COCO_RShoulder,
  UT_COCO_RElbow,
  UT_COCO_RWrist,
  UT_COCO_LShoulder,
  UT_COCO_LElbow,
  UT_COCO_LWrist,
  UT_COCO_RHip,
  UT_COCO_RKnee,
  UT_COCO_RAnkle,
  UT_COCO_LHip,
  UT_COCO_LKnee,
  UT_COCO_LAnkle,
  UT_COCO_REye,
  UT_COCO_LEye,
  UT_COCO_REar,
  UT_COCO_LEar,
  UT_COCO_Bkg,
   //---------------------
  UT_COCO_PARTS
};


/**
 * @brief This is the parent relation map of COCO skeleton joints , we can easily find a joints parent using this enumerator
 */
static const int UT_COCOSkeletonJointsParentRelationMap[] =
{
    // Parent                        Joint
  UT_COCO_Nose,                        //UT_COCO_Nose,
  UT_COCO_Nose,                        //UT_COCO_Neck,
  UT_COCO_Neck,                        //UT_COCO_RShoulder,
  UT_COCO_RShoulder,                   //UT_COCO_RElbow,
  UT_COCO_RElbow,                      //UT_COCO_RWrist,
  UT_COCO_Neck,                        //UT_COCO_LShoulder,
  UT_COCO_LShoulder,                   //UT_COCO_LElbow,
  UT_COCO_LElbow,                      //UT_COCO_LWrist,
  UT_COCO_Neck,                        //UT_COCO_RHip,
  UT_COCO_RHip,                        //UT_COCO_RKnee,
  UT_COCO_RKnee,                       //UT_COCO_RAnkle,
  UT_COCO_Neck,                        //UT_COCO_LHip,
  UT_COCO_LHip,                        //UT_COCO_LKnee,
  UT_COCO_LKnee,                       //UT_COCO_LAnkle,
  UT_COCO_Nose,                        //UT_COCO_REye,
  UT_COCO_Nose,                        //UT_COCO_LEye,
  UT_COCO_REye,                        //UT_COCO_REar,
  UT_COCO_LEye,                        //UT_COCO_LEar,
  UT_COCO_Bkg                          //UT_COCO_Bkg
};


/**
 * @brief An array of indexes to the parents of BODY25 skeleton joints
 */
static const int heatmapCorrespondenceToBODY25[] =
{
  BODY25_Nose, // UT_COCO_Nose=0,
  BODY25_Neck, // UT_COCO_Neck,
  BODY25_RShoulder, //UT_COCO_RShoulder,
  BODY25_RElbow,// UT_COCO_RElbow,
  BODY25_RWrist,// UT_COCO_RWrist,
  BODY25_LShoulder,//UT_COCO_LShoulder,
  BODY25_LElbow,//UT_COCO_LElbow,
  BODY25_LWrist,//UT_COCO_LWrist,
  BODY25_RHip,//UT_COCO_RHip,
  BODY25_RKnee,//UT_COCO_RKnee,
  BODY25_RAnkle,//UT_COCO_RAnkle,
  BODY25_LHip,//UT_COCO_LHip,
  BODY25_LKnee,//UT_COCO_LKnee,
  BODY25_LAnkle,//UT_COCO_LAnkle,
  BODY25_REye,//UT_COCO_REye,
  BODY25_LEye,//UT_COCO_LEye,
  BODY25_REar,//UT_COCO_REar,
  BODY25_LEar,//UT_COCO_LEar,
  BODY25_Bkg,//UT_COCO_Bkg,
};


unsigned long GetTickCountMicrosecondsJointEstimator();



/**
 * @brief Load a 2D Joint Estimator from a .pb file on disk
 * @ingroup jointestimator
 * @param Pointer to a struct JointEstimator2D that will hold the tensorflow instances on load.
 * @param Quality setting, can currently be 1.0 ( highest quality ), 1.5  or 2.0 ( highest performance )
 * @param Flag that controls the use of PAFs 
 * @param Force the usage of CPU for MocapNET ( should be 1 as MocapNET is designed for CPU while GPU handles 2D ) 
 * @retval 1 = Success loading the files  , 0 = Failure
 */
int loadJointEstimator2D(struct JointEstimator2D * jnet,int joint2DEstimatorSelected,int usePAFs,unsigned int forceCPU);


int unloadJointEstimator2D(struct JointEstimator2D * jnet);

int convertNormalized2DJointsToOriginalImageCoordinates(
                                                         struct JointEstimator2D * jnet,
                                                         float * x,
                                                         float * y,
                                                         int correctOffset
                                                        );

int  estimate2DSkeletonsFromImage(struct JointEstimator2D * jnet,struct Skeletons2DDetected * result,unsigned char * rgbData,unsigned int width,unsigned int height);




int restore2DJointsToInputFrameCoordinates(struct JointEstimator2D * jnet,struct Skeletons2DDetected * input);


float percentOf2DPointsMissing(struct Skeletons2DDetected * input);

int estimate2DSkeletonsFromHeatmaps(struct JointEstimator2D * jnet,struct Skeletons2DDetected * result,std::vector<std::vector<float> > heatmaps);


std::vector<std::vector<float> >  getHeatmaps(struct JointEstimator2D * jnet,unsigned char * rgbData,unsigned int width,unsigned int height);