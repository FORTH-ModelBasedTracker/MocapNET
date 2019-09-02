#pragma once
/** @file utilities.hpp
 *  @brief These are the headers of the utilities that facilitate extracting 2D points from heatmaps. This is needed since 2D estimators output heatmaps, while MocapNET operates on
 *  2D point input. This code does not utilize PAFs so although some care is taken to get subpixel accuracy from the heatmap maxima it is still less accurate than the
 *  original OpenPose implementation etc.
 *  @author Damien Michel, Pashalis Padeleris, Ammar Qammaz (AmmarkoV)
 */

#include <string>
#include <memory>
#include <exception>
#include <opencv2/opencv.hpp>


using namespace std;


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
 * @brief This function converts receives an RGB image and a vector of heatmaps and returns a vector of 2D Points with the peaks of the heatmaps
 * @ingroup utilities
 * @param An OpenCV RGB Color Image to give us the resolution and maybe used for visualization..
 * @param A vector of OpenCV Heatmaps where 1 is high and 0 is low detection
 * @param The minimum threshold for detections, smaller values mean more noisy input, more means less detections
 * @param Flag that controls visualization
 * @retval A vector of 2D Points that contains detections for each of the joints, A point marked as 0,0 means no detection
 */
std::vector<cv::Point_<float> > dj_getNeuralNetworkDetectionsForColorImage(
                                                                                                                                                           cv::Mat colorImageOriginal ,
                                                                                                                                                           cv::Mat colorImageSmall,
                                                                                                                                                           std::vector<cv::Mat> heatmaps ,
                                                                                                                                                           float minThreshold  ,
                                                                                                                                                           int visualize
                                                                                                                                                         );


void convertUtilitiesSkeletonFormatToBODY25(struct skeletonCOCO * sk, std::vector<cv::Point_<float> > points);


