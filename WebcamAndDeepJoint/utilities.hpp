#pragma once

#include <string>
#include <memory>
#include <exception>
#include <opencv2/opencv.hpp>


using namespace std;


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






std::vector<cv::Point> dj_getNeuralNetworkDetectionsForColorImage( cv::Mat colorImageOriginal , std::vector<cv::Mat> heatmaps ,  float minThreshold  , int visualize );





