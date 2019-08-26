#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv;
#endif

#include "jsonCocoSkeleton.h"
#include "visualization.hpp"
#include "bvh.hpp"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

int visualizePoints(const char* windowName,unsigned int frameNumber,float fpsAcquisition,float joint2DEstimator,float fpsMocapNET,unsigned int width,unsigned int height,std::vector<float> mocapNETOutput)
{
#if USE_OPENCV
 if (mocapNETOutput.size()==0)
 {
   fprintf(stderr,YELLOW "Won't visualize empty neural network output for frame %u\n" NORMAL,frameNumber);
   return 0;
 }

 char textInfo[512];

 std::vector<std::vector<float> > points2D = convertBVHFrameTo2DPoints(mocapNETOutput,width,height);
 cv::Mat img(height,width, CV_8UC3, Scalar(0,0,0));



#define DRAW_FLOOR 0
 //------------------------------------------------------------------------------------------
 //Draw floor
 //------------------------------------------------------------------------------------------
 #if DRAW_FLOOR
 std::vector<std::vector<float> > gridPoints2D = convert3DGridTo2DPoints(
                                                                          mocapNETOutput[3],
                                                                          mocapNETOutput[4],
                                                                          mocapNETOutput[5],
                                                                          width,
                                                                          height
                                                                        );
 for (int jointID=0; jointID<gridPoints2D.size(); jointID++)
        {
          float jointPointX = gridPoints2D[jointID][0];
          float jointPointY = gridPoints2D[jointID][1];

          if ( (jointPointX!=0) && (jointPointY!=0) )
           {
             cv::Point jointPoint(jointPointX,jointPointY);
             cv::circle(img,jointPoint,5,cv::Scalar(0,255,0),3,8,0);
           }
         }
 #endif
 //------------------------------------------------------------------------------------------



 if (points2D.size()==0)
 {
   fprintf(stderr,"Can't visualize empty 2D projecte points for frame %u ..\n",frameNumber);
   return 0;
 }
 //fprintf(stderr,"visualizePoints %u points\n",points2D.size());
 //Just the lines ( background layer)
 for (int jointID=0; jointID<points2D.size(); jointID++)
        {
          float jointPointX = points2D[jointID][0];
          float jointPointY = points2D[jointID][1];
          cv::Point jointPoint(jointPointX,jointPointY);
          //fprintf(stderr,"L x,y %0.2f,%0.2f \n",jointPointX,jointPointY);

          if ( (jointPointX!=0) && (jointPointY!=0) )
          {

          //unsigned int parentID = Body25SkeletonJointsParentRelationMap[jointID];
          unsigned int parentID = getBVHParentJoint(jointID);
          if (parentID!=jointID)
          {
           if (parentID<points2D.size())
            {
            float parentPointX = points2D[parentID][0];
            float parentPointY = points2D[parentID][1];
            cv::Point parentPoint(parentPointX,parentPointY);

            if ( (parentPointX!=0) && (parentPointY!=0) )
            {
             cv::line(img,jointPoint,parentPoint, cv::Scalar(0,255,0), 5.0);
            }
           } else
           {
             std::cerr<<"Joint Out Of Bounds..";
           }

          }

          }
        }


 //Just the points and text ( foreground )
 for (int jointID=0; jointID<points2D.size(); jointID++)
        {
          float jointPointX = points2D[jointID][0];
          float jointPointY = points2D[jointID][1];
          //fprintf(stderr,"P x,y %0.2f,%0.2f \n",jointPointX,jointPointY);


          if ( (jointPointX!=0) && (jointPointY!=0) )
           {
             cv::Point jointPoint(jointPointX,jointPointY);
             cv::circle(img,jointPoint,5,cv::Scalar(255,0,0),3,8,0);

             const char * jointName = getBVHJointName(jointID);
             if (jointName!=0)
              {
               snprintf(textInfo,512,"%s(%u)",jointName,jointID);
              } else
              {
               snprintf(textInfo,512,"-(%u)",jointID);
              }
             cv::putText(img, textInfo  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar::all(255), 1,8);
           }
        }

  snprintf(textInfo,512,"Frame %u",frameNumber);
  cv::Point txtPosition; txtPosition.x=20; txtPosition.y=50;
  int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
  cv::Scalar color= cv::Scalar(123,123,123,123 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);
  cv::putText(img,textInfo,txtPosition,fontUsed,1.0,color,4,8);

  txtPosition.y=90;
  snprintf(textInfo,512,"Acquisition : %0.2f fps",fpsAcquisition);
  cv::putText(img,textInfo,txtPosition,fontUsed,1.0,color,4,8);

  txtPosition.y=130;
  snprintf(textInfo,512,"2D Joint Detector : %0.2f fps",joint2DEstimator);
  cv::putText(img,textInfo,txtPosition,fontUsed,1.0,color,4,8);

  txtPosition.y=170;
  snprintf(textInfo,512,"MocapNET : %0.2f fps",fpsMocapNET);
  cv::putText(img,textInfo,txtPosition,fontUsed,1.0,color,4,8);

  cv::imshow(windowName,img);
  cv::waitKey(1);
  return 1;
  #else
   fprintf(stderr,"OpenCV code not present in this build, cannot show visualization..\n");
   return 0;
  #endif
}
