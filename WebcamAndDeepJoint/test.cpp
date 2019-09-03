/** @file test.cpp
 *  @brief This is an all-in-one live demo. It combines data acquisition using OpenCV, 2D Joint estimation using Tensorflow ( via VNECT/OpenPose/FORTH estimators ),
 *  and 3D BVH output using MocapNET.
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include "opencv2/opencv.hpp"
using namespace cv;

#include <cstdlib>

#include "../MocapNETLib/jsonCocoSkeleton.h"
#include "../MocapNETLib/jsonMocapNETHelpers.cpp"

#include "../Tensorflow/tensorflow.hpp"
#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/bvh.hpp"
#include "../MocapNETLib/visualization.hpp"

#include "cameraControl.hpp"
#include "utilities.hpp"


#define DISPLAY_ALL_HEATMAPS 0



/**
 * @brief This function performs 2D estimation.. You give her a Tensorflow instance of a 2D estimator, a BGR image some thresholds and sizes and it will yield a vector of 2D points.
 * @ingroup demo
 * @bug This code is oriented to a single 2D skeleton detected, Multiple skeletons will confuse it and there is no logic to handle them
 * @retval A 2D Skeleton detected in the bgr OpenCV image
 */
std::vector<cv::Point_<float> > predictAndReturnSingleSkeletonOf2DCOCOJoints(
                                                                            struct TensorflowInstance * net,
                                                                            const cv::Mat &bgr ,
                                                                            float minThreshold,
                                                                            int visualize ,
                                                                            unsigned int frameNumber,
                                                                            unsigned int inputWidth2DJointDetector,
                                                                            unsigned int inputHeight2DJointDetector,
                                                                            unsigned int heatmapWidth2DJointDetector,
                                                                            unsigned int heatmapHeight2DJointDetector,
                                                                            unsigned int numberOfHeatmaps
                                                                          )
{
  // preprocess image. Actually resize
  float scaleX = (float) inputWidth2DJointDetector/bgr.cols;
  float scaleY = (float) inputHeight2DJointDetector/bgr.rows;
  cv::Mat fr_res;
  cv::resize(bgr, fr_res, cv::Size(0,0), scaleX, scaleY);
  cv::Mat smallBGR = fr_res.clone();

  if (visualize)
        {
         cv::imshow("BGR",fr_res); 
        }
  fr_res.convertTo(fr_res,CV_32FC3);
  // pass the frame to the Estimator


  std::vector<std::vector<float> > result = predictTensorflowOnArrayOfHeatmaps(
                                                                              net,
                                                                              (unsigned int) fr_res.cols,
                                                                              (unsigned int) fr_res.rows,
                                                                              (float*) fr_res.data,
                                                                               heatmapWidth2DJointDetector,
                                                                               heatmapHeight2DJointDetector
                                                                             );

  unsigned int rows = heatmapWidth2DJointDetector;
  unsigned int cols = heatmapHeight2DJointDetector;
  unsigned int hm = numberOfHeatmaps;
  std::vector<cv::Mat> heatmaps;
  for(int i=0; i<hm; ++i)
  {
    cv::Mat h(rows,cols, CV_32FC1);
    for(int r=0; r<rows; ++r)
    {
      for(int c=0; c<cols; ++c)
      {
        int pos = r*cols+c;
        h.at<float>(r,c) = result[i][pos];
      }
    }
    heatmaps.push_back(h);
  }


  //This code segment will display every returned heatmap in it's own window..
  #if DISPLAY_ALL_HEATMAPS
  if (visualize)
  {
  unsigned int x=0;
  unsigned int y=0;
  char windowLabel[512];
  for(int i=0;i<18;++i)
   {
    snprintf(windowLabel,512,"Heatmap %u",i);
    if (frameNumber==0)
     {
      cv::namedWindow(windowLabel,1);
      cv::moveWindow(windowLabel, x,y);
     }
    cv::imshow(windowLabel,heatmaps[i]);
    y=y+rows+30;
    if (y>700) { x=x+cols; y=0; }
   } 
  }
  #endif // DISPLAY_ALL_HEATMAPS

  return dj_getNeuralNetworkDetectionsForColorImage(bgr,smallBGR,heatmaps,minThreshold,visualize);
}





/**
 * @brief Retrieve MocapNET output vector from an image
 * @ingroup demo
 * @bug This code is oriented to a single 2D skeleton detected, Multiple skeletons will confuse it and there is no logic to handle them
 * @retval Vector of MocapNET output, in case of error it might be empty..!
 */
std::vector<float> returnMocapNETInputFrom2DDetectorOutput(
                                                            struct TensorflowInstance * net,
                                                            const cv::Mat &bgr,
                                                            struct boundingBox *bbox,
                                                            float minThreshold,
                                                            int visualize ,
                                                            float * fps,
                                                            unsigned int frameNumber,
                                                            unsigned int offsetX,
                                                            unsigned int offsetY,
                                                            unsigned int stolenWidth,
                                                            unsigned int stolenHeight,
                                                            unsigned int inputWidth2DJointDetector,
                                                            unsigned int inputHeight2DJointDetector,
                                                            unsigned int heatmapWidth2DJointDetector,
                                                            unsigned int heatmapHeight2DJointDetector,
                                                            unsigned int numberOfHeatmaps
                                                          )
{
  unsigned int frameWidth  =  bgr.size().width; //frame.cols
  unsigned int frameHeight =  bgr.size().height; //frame.rows

  unsigned long startTime  = GetTickCountMicroseconds();
  std::vector<cv::Point_<float> > points = predictAndReturnSingleSkeletonOf2DCOCOJoints(
                                                                                       net,
                                                                                       bgr,
                                                                                       minThreshold,
                                                                                       visualize,
                                                                                       frameNumber,
                                                                                       inputWidth2DJointDetector,
                                                                                       inputHeight2DJointDetector,
                                                                                       heatmapWidth2DJointDetector,
                                                                                       heatmapHeight2DJointDetector,
                                                                                       numberOfHeatmaps
                                                                                     );
  unsigned long endTime = GetTickCountMicroseconds();
  unsigned long openPoseComputationTimeInMilliseconds = (unsigned long) (endTime-startTime)/1000;
           if (!visualize)
           { //If we don't visualize using OpenCV output performance 
             std::cerr<<"OpenPose 2DSkeleton @ "<<openPoseComputationTimeInMilliseconds<<" ms \n";
           }
           
  *fps = (float) 1000000/(endTime-startTime);


  unsigned int i=0;
  struct skeletonCOCO sk={0};


  if (points.size()>=(UT_COCO_PARTS-1))
  {
    // Extract bounding box..
    
    populateBoundingBox(
                        bbox,
                        points
                       );
 
    for (i=0; i<points.size()-1; i++)
         {
          points[i].x+=offsetX;
          points[i].y+=offsetY;
         }

   convertUtilitiesSkeletonFormatToBODY25(&sk,points); 
  
  } else
  {
    std::cerr<<"Cannot Flatten Empty Skeleton (Got "<<points.size()<<" points and had to have at least "<<(UT_COCO_PARTS-1)<<")...\n";
    std::vector<float> emptyVector;
    return emptyVector;
  }


  return flattenskeletonCOCOToVector(&sk,frameWidth+stolenWidth,frameHeight+stolenHeight);
}


/**
 * @brief Convert start and end time to a framerate ( frames per second )
 * @ingroup demo
 * @retval Will return a framerate from two millisecond timestamps, if no time duration has been passed there is no division by zero.
 */
float convertStartEndTimeFromMicrosecondsToFPS(unsigned long startTime, unsigned long endTime)
{
 float timeInMilliseconds =  (float) (endTime-startTime)/1000;
 if (timeInMilliseconds ==0.0) { timeInMilliseconds=0.00001; } //Take care of division by null..
 return (float) 1000/timeInMilliseconds;
}


int main(int argc, char *argv[])
{
  //int i=system("killall firefox thunderbird"); //Don't run out of RAM..
  std::cerr<<"Attempting to open input device\n";

  if (initializeBVHConverter())
    {
     std::cerr<<"BVH allocation happened we are going to have BVH visualization \n";
    }

  unsigned int forceCPUMocapNET=1;
  unsigned int forceCPU2DJointEstimation=0;

  unsigned int live=0,frameNumber=0,skippedFrames=0,frameLimit=5000,visualize=1;
  float joint2DSensitivity=0.20;
  const char * webcam = 0;

  int constrainPositionRotation=1;
  int doCrop=1;
  int yawValue = 0;
  int pitchValue = 0;
  int rollValue = 0;
  int distance = 0; 


  //2D Joint Detector Configuration
  unsigned int inputWidth2DJointDetector = 368;
  unsigned int inputHeight2DJointDetector = 368;
  unsigned int heatmapWidth2DJointDetector = 46;
  unsigned int heatmapHeight2DJointDetector = 46;
  unsigned int numberOfHeatmaps = 19;
  const char   outputPathStatic[]="out.bvh";
  char * outputPath = (char*) outputPathStatic;
  const char   networkPathOpenPoseMiniStatic[]="combinedModel/openpose_model.pb";
  const char   networkPathVnectStatic[]="combinedModel/vnect_sm_pafs_8.1k.pb";
  char * networkPath = (char*) networkPathVnectStatic;
  const char   networkInputLayer[]="input_1";
  const char   networkOutputLayer[]="k2tfout_1";
  //const char   networkPath[]="combinedModel/mobnet2_tiny_vnect_sm_1.9k.pb",
  //const char   networkInputLayer[]="input_1",
  //const char   networkOutputLayer[]="k2tfout_0"
  //-------------------------------

  for (int i=0; i<argc; i++)
  {
    //In order to have an acceptable performance you should run 2D Joint estimation on GPU and MocapNET on CPU (which is the default configuration)
    //If you want to force everything on GPU use --gpu
    //If you want to force everything on CPU use --cpu

    if (strcmp(argv[i],"--novisualization")==0) { visualize=0; } else
    if (strcmp(argv[i],"--openposemini")==0)    { networkPath=(char*) networkPathOpenPoseMiniStatic; joint2DSensitivity=0.4; } else
    if (strcmp(argv[i],"--vnect")==0)           { networkPath = (char*) networkPathVnectStatic;      joint2DSensitivity=0.20; } else
    if (strcmp(argv[i],"--2dmodel")==0)         { networkPath=argv[i+1]; } else
    if (strcmp(argv[i],"--output")==0)          { outputPath=argv[i+1]; } else 
    if (strcmp(argv[i],"-o")==0)                { outputPath=argv[i+1]; } else 
    if (strcmp(argv[i],"--frames")==0)          { frameLimit=atoi(argv[i+1]); } else
    //if (strcmp(argv[i],"--cpu")==0)           { setenv("CUDA_VISIBLE_DEVICES", "", 1); } else //Alternate way to force CPU everywhere
    if (strcmp(argv[i],"--cpu")==0)             { forceCPUMocapNET=1; forceCPU2DJointEstimation=1; } else
    if (strcmp(argv[i],"--gpu")==0)             { forceCPUMocapNET=0; forceCPU2DJointEstimation=0; } else
    if (strcmp(argv[i],"--unconstrained")==0)   { constrainPositionRotation=0; } else
    if (strcmp(argv[i],"--nocrop")==0)          { doCrop=0; } else
    if (strcmp(argv[i],"--live")==0)            { live=1; } else
    if (strcmp(argv[i],"--from")==0)            { if (argc>i+1) { webcam = argv[i+1]; } }
  }

  cv::Mat controlMat = Mat(Size(inputWidth2DJointDetector,30),CV_8UC3);
 
  VideoCapture cap(webcam); // open the default camera
  if (webcam==0)
     {
      std::cerr<<"Trying to open webcam\n";
      cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
      cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
     } else
     {
      std::cerr<<"Trying to open "<<webcam<<"\n";
     }

  if (!cap.isOpened())  // check if succeeded to connect to the camera
          { 
            std::cerr<<"Openning input stream  `"<<webcam<<" ` failed\n"; 
            return 1;
           }

  cv::Mat frame;
  struct boundingBox cropBBox={0};
  unsigned int croppedDimensionWidth=0,croppedDimensionHeight=0,offsetX=0,offsetY=0;

  struct TensorflowInstance net={0};
  struct MocapNET mnet={0};



  std::vector<float> flatAndNormalizedPoints;
  std::vector<std::vector<float> > bvhFrames;
  std::vector<float> bvhOutput;

  if ( loadMocapNET(&mnet,"test",forceCPUMocapNET) )
  {
   if (
       loadTensorflowInstance(
                              &net,
                               networkPath,
                               networkInputLayer,
                               networkOutputLayer,
                               forceCPU2DJointEstimation
                             )

      )
   {
      frameNumber=0;
      while ( (live) || (frameNumber<frameLimit) )
      {
        // Get Image
        unsigned long acquisitionStart = GetTickCountMicroseconds();
        
        cap >> frame; // get a new frame from camera
        
        unsigned int frameWidth  =  frame.size().width;  //frame.cols
        unsigned int frameHeight =  frame.size().height; //frame.rows
        if ( (frameWidth!=0)  && (frameHeight!=0)  ) 
        { 
        unsigned long acquisitionEnd = GetTickCountMicroseconds();


        float fpsAcquisition = convertStartEndTimeFromMicrosecondsToFPS(acquisitionStart,acquisitionEnd);

         //------------------------------------------------------------------------
         // If cropping is enabled
         if (doCrop)
         {
          //And there was some previous BVH output
          if (bvhOutput.size()>0)
          {
           // Try to crop around the last closest
           //------------------------------------------------------------------------
           getBestCropWindow(
                             &offsetX,
                             &offsetY,
                             &croppedDimensionWidth,
                             &croppedDimensionHeight,
                             &cropBBox,
                             inputWidth2DJointDetector,
                             inputHeight2DJointDetector,
                             frameWidth,
                             frameHeight
                            );

           cv::Rect rectangleROI(offsetX,offsetY,croppedDimensionWidth,croppedDimensionHeight);
           frame = frame(rectangleROI);
           cropBBox.populated=0;
          } else
          {
           fprintf(stderr,"Haven't detected a person, so seeking a skeleton in the full image, regardless of distortion..\n");
          }
         }
         //------------------------------------------------------------------------

        if ( (frameWidth>0) && (frameHeight>0) )
        {
         // Get 2D Skeleton Input from Frame
          float fps2DJointDetector = 0;
          flatAndNormalizedPoints = returnMocapNETInputFrom2DDetectorOutput(
                                                                            &net,
                                                                            frame,
                                                                            &cropBBox,
                                                                            joint2DSensitivity,
                                                                            visualize,
                                                                            &fps2DJointDetector,
                                                                            frameNumber,
                                                                            offsetX,
                                                                            offsetY,
                                                                            frameWidth-croppedDimensionWidth,
                                                                            frameHeight-croppedDimensionHeight,
                                                                            inputWidth2DJointDetector,
                                                                            inputHeight2DJointDetector,
                                                                            heatmapWidth2DJointDetector,
                                                                            heatmapHeight2DJointDetector,
                                                                            numberOfHeatmaps
                                                                           );

         // Get MocapNET prediction
         unsigned long startTime = GetTickCountMicroseconds();
         bvhOutput = runMocapNET(&mnet,flatAndNormalizedPoints);
         unsigned long endTime = GetTickCountMicroseconds();

         float fpsMocapNET = convertStartEndTimeFromMicrosecondsToFPS(startTime,endTime);
         
         if (!visualize)
           { //If we don't visualize using OpenCV output performance
             std::cerr<<"MocapNET 3DSkeleton @ "<<fpsMocapNET<<" fps \n";
           }

         //If we are not running live ( aka not from a webcam with no fixed frame limit )
         //Then we record the current bvh frame in order to save a .bvh file in the end..
         if (!live)
              { bvhFrames.push_back(bvhOutput); }


         //Force Skeleton Position and orientation to make it more easily visualizable
         if (constrainPositionRotation)
         {
          if (bvhOutput.size()>0)
          {
           bvhOutput[MOCAPNET_OUTPUT_HIP_XPOSITION]=0.0;
           bvhOutput[MOCAPNET_OUTPUT_HIP_YPOSITION]=0.0;
           bvhOutput[MOCAPNET_OUTPUT_HIP_ZPOSITION]=-160.0 - (float) distance;
           bvhOutput[MOCAPNET_OUTPUT_HIP_ZROTATION]=(float) rollValue;
           bvhOutput[MOCAPNET_OUTPUT_HIP_YROTATION]=(float) yawValue;
           bvhOutput[MOCAPNET_OUTPUT_HIP_XROTATION]=(float) pitchValue;
          }
         }


         //Display two sample joint configurations to console output
         //to demonstrate how easy it is to get the output joint information
         if (bvhOutput.size()>0)
         {
         fprintf(stderr,"Right Shoulder Z X Y = %0.2f,%0.2f,%0.2f\n",
                 bvhOutput[MOCAPNET_OUTPUT_RSHOULDER_ZROTATION],
                 bvhOutput[MOCAPNET_OUTPUT_RSHOULDER_XROTATION],
                 bvhOutput[MOCAPNET_OUTPUT_RSHOULDER_YROTATION]
                );

         fprintf(stderr,"Left Shoulder Z X Y = %0.2f,%0.2f,%0.2f\n",
                 bvhOutput[MOCAPNET_OUTPUT_LSHOULDER_ZROTATION],
                 bvhOutput[MOCAPNET_OUTPUT_LSHOULDER_XROTATION],
                 bvhOutput[MOCAPNET_OUTPUT_LSHOULDER_YROTATION]
                );
         }

         
        unsigned long totalTimeEnd = GetTickCountMicroseconds();        
        float fpsTotal = convertStartEndTimeFromMicrosecondsToFPS(acquisitionStart,totalTimeEnd);

        
         //OpenCV Visualization stuff
         //---------------------------------------------------
         if (visualize)
           {
             unsigned int visWidth=frameWidth;
             unsigned int visHeight=frameHeight;
             //If our input window is small enlarge it a little..
             if (visWidth<700)
              {
                visWidth=(unsigned int) frameWidth*2.0;
                visHeight=(unsigned int) frameHeight*2.0;
              }
            visWidth=1024;
            visHeight=768;

            if (frameNumber==0)
             {
              cv::namedWindow("3D Control",WINDOW_NORMAL);
              cv::resizeWindow("3D Control",inputWidth2DJointDetector,inputHeight2DJointDetector);
              cv::namedWindow("3D Points Output");
              cv::moveWindow("2D NN Heatmaps",0,0);
              cv::moveWindow("BGR",0,inputHeight2DJointDetector);
              cv::moveWindow("3D Control",inputWidth2DJointDetector,inputHeight2DJointDetector);
              cv::moveWindow("2D Detections",inputWidth2DJointDetector,0);
              
             
             createTrackbar("Constrain Position/Rotation", "3D Control", &constrainPositionRotation, 1);
             createTrackbar("Automatic Crop", "3D Control", &doCrop, 1);
             createTrackbar("Distance  ", "3D Control", &distance,  150);
             createTrackbar("Yaw            ", "3D Control", &yawValue,  360);
             createTrackbar("Pitch          ", "3D Control", &pitchValue,360);
             createTrackbar("Roll            ", "3D Control", &rollValue, 360);
             }


             //Get rid of GLib-GObject-CRITICAL **: 10:36:18.934: g_object_unref: assertion 'G_IS_OBJECT (object)' failed opencv
             //by displaying an empty cv Mat on the window besides the trackbars
             cv::imshow("3D Control",controlMat);


             visualizePoints(
                                            "3D Points Output",
                                            frameNumber,
                                            skippedFrames,
                                            fpsTotal,
                                            fpsAcquisition,
                                            fps2DJointDetector,
                                            fpsMocapNET,
                                            visWidth,
                                            visHeight,
                                            bvhOutput
                                          );


            if (frameNumber==0)
             { 
              cv::moveWindow("3D Points Output",inputWidth2DJointDetector*2,0);
             }
            //Window Event Loop Time..
            cv::waitKey(1);
           } 
         //---------------------------------------------------
        }



        ++frameNumber;
      } else
      { 
          std::cerr<<"OpenCV failed to snap frame "<<frameNumber<<"from your input source ( "<<webcam<<") \n"; 
          std::cerr<<"Skipped frames  "<<skippedFrames<<" / "<<frameNumber<<" \n"; 
           ++skippedFrames;
      }
     
 } //Master While Frames Exist loop
      
      //After beeing done with the frames gathered the bvhFrames vector should be full of our data, so maybe we want to write it to a file..!
      if (!live)
      {
       std::cerr<<"Will now write BVH file to "<<outputPath<<"\n";   
       //just use BVH header   
       if ( writeBVHFile(outputPath,0,bvhFrames) )
            { std::cerr<<"Successfully wrote "<<bvhFrames.size()<<" frames to bvh file.. \n";  } else
            { std::cerr<<"Failed to write "<<bvhFrames.size()<<" frames to bvh file.. \n";     }
        if (skippedFrames>0)
            { std::cerr<<"Please note that while getting input "<<skippedFrames<<"  frames where skipped due to OpenCV related errors\n"; }    
      } else
      { std::cerr<<"Will not record a bvh file since live sessions can be arbitrarily long..\n"; }
    } else
    { std::cerr<<"Was not able to load Tensorflow model for 2D joint detection..\n"; }
   } else
   { std::cerr<<"Was not able to load MocapNET, please make sure you have the appropriate models downloaded..\n"; }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

