#include "opencv2/opencv.hpp"
using namespace cv;

#include <cstdlib>

#include "../MocapNETLib/jsonCocoSkeleton.h"
#include "../MocapNETLib/jsonMocapNETHelpers.cpp"

#include "../Tensorflow/tensorflow.hpp"
#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/bvh.hpp"
#include "../MocapNETLib/visualization.hpp"
#include "utilities.hpp"


#define DISPLAY_ALL_HEATMAPS 0


struct boundingBox
{
   char populated;
   float minimumX;
   float maximumX;
   float minimumY;
   float maximumY;
};


std::vector<cv::Point_<int> > predictAndReturnSingleSkeletonOf2DCOCOJoints(
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
  #endif // DISPLAY_ALL_HEATMAPS



  return dj_getNeuralNetworkDetectionsForColorImage(bgr,heatmaps,minThreshold,visualize);
}


int getBestCropWindow(
                       unsigned int * x,
                       unsigned int * y,
                       unsigned int * width,
                       unsigned int * height,
                       struct boundingBox * bbox,
                       unsigned int fullFrameWidth,
                       unsigned int fullFrameHeight
                     )
{
   //fprintf(stderr,"Previous crop started at %u,%u\n", *x,*y);
   if (bbox->populated)
    {
     //fprintf(stderr,"This means that the bounding box (%0.2f,%0.2f) -> (%0.2f,%0.2f)\n",bbox->minimumX,bbox->minimumY,bbox->maximumX,bbox->maximumY);
     bbox->minimumX+=(float) *x;
     bbox->maximumX+=(float) *x;
     bbox->minimumY+=(float) *y;
     bbox->maximumY+=(float) *y;
     //fprintf(stderr,"is actually (%0.2f,%0.2f) -> (%0.2f,%0.2f)\n",bbox->minimumX,bbox->minimumY,bbox->maximumX,bbox->maximumY);
    }

   float bodyWidth = bbox->maximumX - bbox->minimumX;
   //fprintf(stderr,"Body starts at %0.2f and ends at %0.2f for a total of %0.2f pixels\n", bbox->minimumX , bbox->maximumX , bbox->maximumX-bbox->minimumX);
   if (fullFrameWidth>=fullFrameHeight) {
                                         //fprintf(stderr,"Since the whole frame has %u pixels\n",fullFrameWidth);
                                         //fprintf(stderr,"And we will use the dimension of the Y axis aka %u pixels to crop a rectangle\n",fullFrameHeight);
                                         *width=fullFrameHeight;
                                         *height=fullFrameHeight;

                                         if (bbox->populated)
                                         {
                                           float bodyCenterX =  bbox->minimumX+ (float) bodyWidth/2;
                                           //fprintf(stderr,"The center X of the body lies at %0.2f\n",bodyCenterX);

                                           //fprintf(stderr,"We can afford %u pixels left and right of the body center \n",(unsigned int) *width/2);
                                           float cropStartX = bodyCenterX - *width/2;

                                           if (cropStartX<0) { cropStartX=0; } //Overflow..
                                           if (cropStartX>fullFrameWidth-fullFrameHeight) { cropStartX=fullFrameWidth-fullFrameHeight; }

                                           //Neural Networks cause flicker, if we don't have exactly the same
                                           //bounding box it's ok just keep the previous, it will help with flicker a lot..
                                           if ( abs(*x - (unsigned int) cropStartX) > 4 )
                                             {
                                              *x = (unsigned int) cropStartX;
                                             }
                                           //fprintf(stderr,"This means starting at %u and ending at %u\n",(unsigned int) *x, *x+*width);
                                         } else
                                         {
                                           //No skeleton? just crop in the center..
                                           *x=(fullFrameWidth-fullFrameHeight)/2;
                                         }
                                      }

  return 1;
}


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
  unsigned int frameWidth =  bgr.size().width; //frame.cols
  unsigned int frameHeight =  bgr.size().height; //frame.rows

  unsigned long startTime = GetTickCountMicroseconds();
  std::vector<cv::Point_<int> > points = predictAndReturnSingleSkeletonOf2DCOCOJoints(
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
  std::cerr<<"OpenPose 2DSkeleton @ "<<openPoseComputationTimeInMilliseconds<<" ms \n";
  *fps = (float) 1000000/(endTime-startTime);


  unsigned int i=0;
  struct skeletonCOCO sk={0};


  if (points.size()>=(UT_COCO_PARTS-1))
  {
    // Extract bounding box..
    if (bbox!=0)
     { //----------------------------------------
      bbox->populated=0;

      if (points.size()>0)
      {
       bbox->populated=1;
       bbox->minimumX=100000;
       bbox->maximumX=0.0;
       bbox->minimumY=100000;
       bbox->maximumY=0.0;
       for (i=0; i<points.size()-1; i++)
       {
        if ( (points[i].x!=0) && (points[i].y!=0) )
        {
         if (bbox->minimumX > points[i].x) { bbox->minimumX=points[i].x; }
         if (bbox->maximumX < points[i].x) { bbox->maximumX=points[i].x; }
         if (bbox->minimumY > points[i].y) { bbox->minimumY=points[i].y; }
         if (bbox->maximumY < points[i].y) { bbox->maximumY=points[i].y; }
        }
       }
      }
     } //----------------------------------------

  for (i=0; i<points.size()-1; i++)
       {
          points[i].x+=offsetX;
          points[i].y+=offsetY;
       }


  sk.joint2D[BODY25_Nose].x = points[UT_COCO_Nose].x;
  sk.joint2D[BODY25_Nose].y = points[UT_COCO_Nose].y;

  sk.joint2D[BODY25_Neck].x = points[UT_COCO_Neck].x;
  sk.joint2D[BODY25_Neck].y = points[UT_COCO_Neck].y;

  sk.joint2D[BODY25_RShoulder].x = points[UT_COCO_RShoulder].x;
  sk.joint2D[BODY25_RShoulder].y = points[UT_COCO_RShoulder].y;

  sk.joint2D[BODY25_RElbow].x = points[UT_COCO_RElbow].x;
  sk.joint2D[BODY25_RElbow].y = points[UT_COCO_RElbow].y;

  sk.joint2D[BODY25_RWrist].x = points[UT_COCO_RWrist].x;
  sk.joint2D[BODY25_RWrist].y = points[UT_COCO_RWrist].y;

  sk.joint2D[BODY25_LShoulder].x = points[UT_COCO_LShoulder].x;
  sk.joint2D[BODY25_LShoulder].y = points[UT_COCO_LShoulder].y;

  sk.joint2D[BODY25_LElbow].x = points[UT_COCO_LElbow].x;
  sk.joint2D[BODY25_LElbow].y = points[UT_COCO_LElbow].y;

  sk.joint2D[BODY25_LWrist].x = points[UT_COCO_LWrist].x;
  sk.joint2D[BODY25_LWrist].y = points[UT_COCO_LWrist].y;

  if (
       (points[UT_COCO_RHip].x!=0) && (points[UT_COCO_LHip].x!=0) &&
       (points[UT_COCO_RHip].y!=0) && (points[UT_COCO_LHip].y!=0)
     )
  {
   sk.joint2D[BODY25_MidHip].x = (points[UT_COCO_RHip].x+points[UT_COCO_LHip].x)/2;
   sk.joint2D[BODY25_MidHip].y = (points[UT_COCO_RHip].y+points[UT_COCO_LHip].y)/2;
  }

  sk.joint2D[BODY25_RHip].x = points[UT_COCO_RHip].x;
  sk.joint2D[BODY25_RHip].y = points[UT_COCO_RHip].y;

  sk.joint2D[BODY25_RKnee].x = points[UT_COCO_RKnee].x;
  sk.joint2D[BODY25_RKnee].y = points[UT_COCO_RKnee].y;

  sk.joint2D[BODY25_RAnkle].x = points[UT_COCO_RAnkle].x;
  sk.joint2D[BODY25_RAnkle].y = points[UT_COCO_RAnkle].y;

  sk.joint2D[BODY25_LHip].x = points[UT_COCO_LHip].x;
  sk.joint2D[BODY25_LHip].y = points[UT_COCO_LHip].y;

  sk.joint2D[BODY25_LKnee].x = points[UT_COCO_LKnee].x;
  sk.joint2D[BODY25_LKnee].y = points[UT_COCO_LKnee].y;

  sk.joint2D[BODY25_LAnkle].x = points[UT_COCO_LAnkle].x;
  sk.joint2D[BODY25_LAnkle].y = points[UT_COCO_LAnkle].y;

  sk.joint2D[BODY25_REye].x = points[UT_COCO_REye].x;
  sk.joint2D[BODY25_REye].y = points[UT_COCO_REye].y;

  sk.joint2D[BODY25_LEye].x = points[UT_COCO_LEye].x;
  sk.joint2D[BODY25_LEye].y = points[UT_COCO_LEye].y;

  sk.joint2D[BODY25_REar].x = points[UT_COCO_REar].x;
  sk.joint2D[BODY25_REar].y = points[UT_COCO_REar].y;

  sk.joint2D[BODY25_LEar].x = points[UT_COCO_LEar].x;
  sk.joint2D[BODY25_LEar].y = points[UT_COCO_LEar].y;

  sk.joint2D[BODY25_Bkg].x = points[UT_COCO_Bkg].x;
  sk.joint2D[BODY25_Bkg].y = points[UT_COCO_Bkg].y;
  } else
  {
    std::cerr<<"Cannot Flatten Empty Skeleton (Got "<<points.size()<<" points and had to have at least "<<(UT_COCO_PARTS-1)<<")...\n";
    std::vector<float> emptyVector;
    return emptyVector;
  }


  return flattenskeletonCOCOToVector(&sk,frameWidth+stolenWidth,frameHeight+stolenHeight);
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

  unsigned int live=0,frameNumber=0,frameLimit=5000,visualize=1,doCrop=1,constrainPositionRotation=1;
  float joint2DSensitivity=0.15;
  const char * webcam = 0;

  int yawValue = 0;
  int pitchValue = 0;
  int rollValue = 0;


  //2D Joint Detector Configuration
  unsigned int inputWidth2DJointDetector = 368;
  unsigned int inputHeight2DJointDetector = 368;
  unsigned int heatmapWidth2DJointDetector = 46;
  unsigned int heatmapHeight2DJointDetector = 46;
  unsigned int numberOfHeatmaps = 19;
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

    if (strcmp(argv[i],"--openposemini")==0)   { networkPath=(char*) networkPathOpenPoseMiniStatic; joint2DSensitivity=0.4; } else
    if (strcmp(argv[i],"--vnect")==0)          { networkPath = (char*) networkPathVnectStatic;      joint2DSensitivity=0.15; } else
    if (strcmp(argv[i],"--2dmodel")==0)        { networkPath=argv[i+1]; } else
    if (strcmp(argv[i],"--frames")==0)         { frameLimit=atoi(argv[i+1]); } else
    //if (strcmp(argv[i],"--cpu")==0)  { setenv("CUDA_VISIBLE_DEVICES", "", 1); } else //Alternate way to force CPU everywhere
    if (strcmp(argv[i],"--cpu")==0)            { forceCPUMocapNET=1; forceCPU2DJointEstimation=1; } else
    if (strcmp(argv[i],"--gpu")==0)            { forceCPUMocapNET=0; forceCPU2DJointEstimation=0; } else
    if (strcmp(argv[i],"--unconstrained")==0)  { constrainPositionRotation=0; } else
    if (strcmp(argv[i],"--nocrop")==0)         { doCrop=0; } else
    if (strcmp(argv[i],"--live")==0)           { live=1; } else
    if (strcmp(argv[i],"--from")==0)           {
                                                 if (argc>i+1) { webcam = argv[i+1]; }
                                               }
  }


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
          { std::cerr<<"Openning camera failed\n"; return 1; }

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
        cap >> frame; // get a new frame from camera
        unsigned int frameWidth  =  frame.size().width;  //frame.cols
        unsigned int frameHeight =  frame.size().height; //frame.rows

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


         //Get Framerate of MocapNET evaluation..
         float mocapNETComputationTimeInMilliseconds =  (float) (endTime-startTime)/1000;
         if (mocapNETComputationTimeInMilliseconds==0.0) { mocapNETComputationTimeInMilliseconds=0.00001; } //Take care of division by null..
         float fpsMocapNET = (float) 1000000/(endTime-startTime);

         std::cerr<<"MocapNET 3DSkeleton @ "<<mocapNETComputationTimeInMilliseconds<<" ms \n";

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
           bvhOutput[MOCAPNET_OUTPUT_HIP_ZPOSITION]=-120.0;
           bvhOutput[MOCAPNET_OUTPUT_HIP_ZROTATION]=(float) pitchValue;
           bvhOutput[MOCAPNET_OUTPUT_HIP_YROTATION]=(float) yawValue;
           bvhOutput[MOCAPNET_OUTPUT_HIP_XROTATION]=(float) rollValue;
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


         //OpenCV Visualization stuff
         //---------------------------------------------------
         if (visualize)
           {
             unsigned int visWidth=frameWidth;
             unsigned int visHeight=frameHeight;
             //If our input window is small enlarge it a little..
             if (visWidth<800)
              {
                visWidth=(unsigned int) frameWidth*2.0;
                visHeight=(unsigned int) frameHeight*2.0;
              }

            if (frameNumber==0)
             {
              cv::moveWindow("BG",0,0);
              cv::moveWindow("BGR",0,inputHeight2DJointDetector);
              cv::moveWindow("DETECTION",inputWidth2DJointDetector,0);
             }


             //Create trackbar to change 3D orientation..
             createTrackbar("Yaw", "3D Points Output", &yawValue, 360);
             //createTrackbar("Pitch", "3D Points Output", &pitchValue, 360);
             //createTrackbar("Roll", "3D Points Output", &rollValue, 360);


             visualizePoints("3D Points Output",frameNumber,fps2DJointDetector,fpsMocapNET,visWidth,visHeight,bvhOutput);

            if (frameNumber==0)
             {
              cv::moveWindow("3D Points Output",1920-visWidth,1080-visHeight);
             }
           } else
           {
            //Window Event Loop Time..
            waitKey(1);
           }
         //---------------------------------------------------
        }



        ++frameNumber;
      }

      if (!live)
      {
       if (//just use BVH header
           writeBVHFile( "out.bvh",0,bvhFrames)
          )
          { std::cerr<<"Successfully wrote "<<bvhFrames.size()<<" frames to bvh file.. \n";  } else
          { std::cerr<<"Failed to write "<<bvhFrames.size()<<" frames to bvh file.. \n";  }
      } else
      { std::cerr<<"Did not record a bvh file since live sessions can be arbitrarily long..\n"; }
    } else
    { std::cerr<<"Was not able to load Tensorflow model for 2D joint detection..\n"; }
   } else
   { std::cerr<<"Was not able to load MocapNET, please make sure you have the appropriate models downloaded..\n"; }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

