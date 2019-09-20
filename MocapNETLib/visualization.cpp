#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv;
#endif

#include "jsonCocoSkeleton.h"
#include "visualization.hpp"
#include "bvh.hpp"
#include "../MocapNETLib/mocapnet.hpp"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */




#if USE_OPENCV
int visualizeNSDM(
    cv::Mat &img,
    std::vector<float> mocapNETInput,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
)
{
    float thickness=2.5;
    cv::Point topLeft(x,y);
    cv::Point bottomRight(x+width,y+height);
    //cv::line(img,topLeft,bottomRight, cv::Scalar(0,255,0),thickness);
    //cv::rectangle(img, topLeft,bottomRight, cv::Scalar(0,255,0),thickness, 8, 0);

    int addSyntheticPoints=1;
    int doScaleCompensation=0;
    std::vector<float> NSDM = compressMocapNETInput(mocapNETInput,addSyntheticPoints,doScaleCompensation);

    if (NSDM.size()>0)
        {

            float thickness=1;
            int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
            cv::Scalar color= cv::Scalar(123,123,123,123 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);
            cv::Point txtPosition(x,y-15);
            cv::putText(img,"NSDM",txtPosition,fontUsed,0.8,color,thickness,8);


            unsigned int xI,yI,item=0,dim=sqrt(NSDM.size()/2);
            unsigned int boxX=width/dim,boxY=height/dim;
            for (yI=0; yI<dim; yI++)
                {
                    for (xI=0; xI<dim; xI++)
                        {
                            cv::Point topLeft(x+xI*boxX,y+yI*boxY);
                            cv::Point bottomRight(x+xI*boxX+boxX,y+yI*boxY+boxY);

                            float blueChannel=(float) NSDM[item]*255.0;
                            float greenChannel=(float) NSDM[item+1]*255.0;
                            float redChannel=(float) 255.0 * ( (NSDM[item]==0.0) && (NSDM[item+1]==0.0) );

                            cv::rectangle(
                                img,
                                topLeft,
                                bottomRight,
                                cv::Scalar(
                                    blueChannel,
                                    greenChannel,
                                    redChannel
                                ),
                                -1*thickness,
                                8,
                                0
                            );
                            item+=2;
                        }
                }
        }
}




int drawFloorFromPrimitives(
    cv::Mat &img,
    float roll,
    float pitch,
    float yaw,
    unsigned int floorDimension,
    unsigned int width,
    unsigned int height
)
{
    std::vector<std::vector<float> > gridPoints2D = convert3DGridTo2DPoints(
                roll,
                pitch,
                yaw,
                width,
                height,
                floorDimension
            );
    cv::Point parentPoint(gridPoints2D[0][0],gridPoints2D[0][1]);
    cv::Point verticalPoint(gridPoints2D[0][0],gridPoints2D[0][1]);
    for (int jointID=0; jointID<gridPoints2D.size(); jointID++)
        {
            float jointPointX = gridPoints2D[jointID][0];
            float jointPointY = gridPoints2D[jointID][1];
            cv::Point jointPoint(jointPointX,jointPointY);

            if (jointID+floorDimension<gridPoints2D.size())
                {
                    verticalPoint.x=gridPoints2D[jointID+20][0];
                    verticalPoint.y=gridPoints2D[jointID+20][1];
                }
            else
                {
                    verticalPoint.x=0.0;
                    verticalPoint.y=0.0;
                }

            if ( (jointPointX>0.0) && (jointPointY>0.0) )
                {
                    cv::circle(img,jointPoint,2,cv::Scalar(255,255,0),3,8,0);
                    if ( (verticalPoint.x>0.0) && (verticalPoint.y>0.0) )
                        {
                            cv::line(img,jointPoint,verticalPoint, cv::Scalar(255,255,0), 1.0);
                        }
                }

            if (jointID%floorDimension!=0)
                {
                    if ( (jointPointX>0.0) && (jointPointY>0.0) && (verticalPoint.x>0.0) && (verticalPoint.y>0.0) )
                        {
                            cv::line(img,jointPoint,parentPoint, cv::Scalar(255,255,0), 1.0);
                        }
                }
            parentPoint = jointPoint;
        }
}


float pointDistance(float xA,float yA,float xB,float yB)
{
 return sqrt( ((xA-xB)*(xA-xB)) + ((yA-yB)*(yA-yB)) );
}


static const char * reprojectBVHNames[] =
{
"lShldr",
"rShldr",
"lForeArm",
"rForeArm",
"lHand",
"rHand",
"lThigh",
"rThigh",
"lShin",
"rShin",
"lFoot",
"rFoot"
};



static int reproject2DIDs[] =
{
BODY25_LShoulder,    
BODY25_RShoulder,    
BODY25_LElbow,
BODY25_RElbow,
BODY25_LWrist,
BODY25_RWrist,
BODY25_LHip,
BODY25_RHip,
BODY25_LKnee,
BODY25_RKnee,
BODY25_LAnkle,
BODY25_RAnkle
};

static int numberOfReprojectionChecks=12;

int visualizeSkeletonCorrespondence(
    cv::Mat &imgO,
    std::vector<std::vector<float> > points2DInput,
    std::vector<std::vector<float> > points2DOutput,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
)
{

    height=1080;
    width=1920;
    int doFullReprojectionVisualization = 0;    
    
    if (doFullReprojectionVisualization)
    {
    cv::Mat img(height,width, CV_8UC3, Scalar(0,0,0));


//Just the points and text ( foreground )
    for (int jointID=0; jointID<points2DInput.size(); jointID++)
        {
            float jointInPointX = points2DInput[jointID][0];
            float jointInPointY = points2DInput[jointID][1];
            //fprintf(stderr,"P x,y %0.2f,%0.2f \n",jointPointX,jointPointY);

            if ( (jointInPointX!=0) && (jointInPointY!=0) )
                {
                    cv::Point jointPoint(jointInPointX+10,jointInPointY);
                    cv::circle(img,jointPoint,5,cv::Scalar(0,255,0),3,8,0);
                    const char * jointName  = Body25BodyNames[jointID];
                    if (jointName!=0)
                        {
                            cv::putText(img, jointName  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(123), 0.2, 8 );
                        }
                }
        }

    unsigned int midHipBVHJointID = getBVHJointIDFromJointName("Hip");
    float alignmentX = points2DInput[BODY25_MidHip][0]-points2DOutput[midHipBVHJointID][0];
    float alignmentY = points2DInput[BODY25_MidHip][1]-points2DOutput[midHipBVHJointID][1];


    for (int jointID=0; jointID<points2DOutput.size(); jointID++)
        {
            float jointOutPointX = points2DOutput[jointID][0]+alignmentX;
            float jointOutPointY = points2DOutput[jointID][1]+alignmentY;
            //fprintf(stderr,"P x,y %0.2f,%0.2f \n",jointPointX,jointPointY);

            if ( (jointOutPointX!=0) && (jointOutPointY!=0) )
                {
                    cv::Point jointPoint(jointOutPointX+10,jointOutPointY);
                    cv::circle(img,jointPoint,5,cv::Scalar(0,0,255),3,8,0);
                    const char * jointName = getBVHJointName(jointID);
                    if (jointName!=0)
                        {
                            cv::putText(img, jointName  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );
                        }
                }
        }
    
    cv::imshow("TEST",img);    
    }
    
     //-----------------------------------------------------------------------------------------------------------------------------
     //-----------------------------------------------------------------------------------------------------------------------------
     //-----------------------------------------------------------------------------------------------------------------------------
     unsigned int midHipBVHJointID = getBVHJointIDFromJointName("Hip");
     float alignmentX = points2DInput[BODY25_MidHip][0]-points2DOutput[midHipBVHJointID][0];
     float alignmentY = points2DInput[BODY25_MidHip][1]-points2DOutput[midHipBVHJointID][1];
    
     float x2D,y2D,xReprojected,yReprojected,distance,relativeDistance;
     unsigned int jointID2D,jointIDBVH;
 
     //-------------------------------------------------------------------------
     //-------------------------------------------------------------------------
     //-------------------------------------------------------------------------
 
     char textWarning[512];
     int i=0;
     for (i=0; i<numberOfReprojectionChecks; i++)
     {
      jointID2D = reproject2DIDs[i]; 
      jointIDBVH = getBVHJointIDFromJointName(reprojectBVHNames[i]);
      //-------------------------------------------------------------------------
      x2D = points2DInput[jointID2D][0];
      y2D = points2DInput[jointID2D][1];
      xReprojected = alignmentX + points2DOutput[jointIDBVH][0];
      yReprojected = alignmentY + points2DOutput[jointIDBVH][1];
      distance=pointDistance(x2D,y2D,xReprojected,yReprojected);
      relativeDistance=(float) distance/width;

      snprintf(textWarning,512,"%s reprojection error %0.2f %%",reprojectBVHNames[i],relativeDistance);
      if (relativeDistance>0.07)
      { 
          //fprintf(stderr,RED);
          
          cv::Point jointPoint(x,y);  
          cv::putText(imgO, textWarning  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0,0,255), 0.2, 8 );
          y+=15;
      }
      //fprintf(stderr,"%s\n" NORMAL,textWarning);         
     } 


    return 1;
}









#endif



int visualizePoints(
    const char* windowName,
    unsigned int frameNumber,
    unsigned int skippedFrames,
    signed int totalNumberOfFrames,
    unsigned int numberOfFramesToGrab,
    int drawFloor,
    int drawNSDM,
    float fpsTotal,
    float fpsAcquisition,
    float joint2DEstimator,
    float fpsMocapNET,
    unsigned int width,
    unsigned int height,
    unsigned int handleMessages,
    std::vector<float> mocapNETInput,
    std::vector<float> mocapNETOutput,
    std::vector<float> mocapNETOutputWithGUIForcedView,
    std::vector<std::vector<float> > points2DInput,
    std::vector<std::vector<float> > points2DOutput,
    std::vector<std::vector<float> > points2DOutputGUIForcedView
)
{
#if USE_OPENCV
    if (mocapNETOutput.size()==0)
        {
            fprintf(stderr,YELLOW "Won't visualize empty neural network output for frame %u\n" NORMAL,frameNumber);
            return 0;
        }

    char textInfo[512];

    //std::vector<std::vector<float> > points2D = convertBVHFrameTo2DPoints(mocapNETOutput,width,height);
    cv::Mat img(height,width, CV_8UC3, Scalar(0,0,0));



//------------------------------------------------------------------------------------------
//Draw floor
//------------------------------------------------------------------------------------------
    if (drawFloor)
        {
            unsigned int floorDimension=20;
            drawFloorFromPrimitives(
                img,
                mocapNETOutputWithGUIForcedView[3],
                mocapNETOutputWithGUIForcedView[4],
                mocapNETOutputWithGUIForcedView[5],
                floorDimension,
                width,
                height
            );
        }
//------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------------------------
//   Draw correspondance, post processing step to see if output is good
//---------------------------------------------------------------------------------------------------------------------
    int visualizeCorrespondence=1;

    if (visualizeCorrespondence)
        {
            visualizeSkeletonCorrespondence(
                img,
                points2DInput,
                points2DOutput,
                750, //X
                50, //Y
                width,
                height
            );
        }
//----------------------------------------------------------------------------------------------------------------------


    if (points2DOutputGUIForcedView.size()==0)
        {
            fprintf(stderr,"Can't visualize empty 2D projected points for frame %u ..\n",frameNumber);
            return 0;
        }
//fprintf(stderr,"visualizePoints %u points\n",points2DOutputGUIForcedView.size());
//Just the lines ( background layer)
    for (int jointID=0; jointID<points2DOutputGUIForcedView.size(); jointID++)
        {
            float jointPointX = points2DOutputGUIForcedView[jointID][0];
            float jointPointY = points2DOutputGUIForcedView[jointID][1];
            cv::Point jointPoint(jointPointX,jointPointY);
            //fprintf(stderr,"L x,y %0.2f,%0.2f \n",jointPointX,jointPointY);

            if ( (jointPointX!=0) && (jointPointY!=0) )
                {

                    //unsigned int parentID = Body25SkeletonJointsParentRelationMap[jointID];
                    unsigned int parentID = getBVHParentJoint(jointID);
                    if (parentID!=jointID)
                        {
                            if (parentID<points2DOutputGUIForcedView.size())
                                {
                                    float parentPointX = points2DOutputGUIForcedView[parentID][0];
                                    float parentPointY = points2DOutputGUIForcedView[parentID][1];
                                    cv::Point parentPoint(parentPointX,parentPointY);

                                    if ( (parentPointX!=0) && (parentPointY!=0) )
                                        {
                                            cv::line(img,jointPoint,parentPoint, cv::Scalar(0,255,0), 5.0);
                                        }
                                }
                            else
                                {
                                    fprintf(stderr,"Joint Out Of Bounds..");
                                }

                        }

                }
        }


//Just the points and text ( foreground )
    for (int jointID=0; jointID<points2DOutputGUIForcedView.size(); jointID++)
        {
            float jointPointX = points2DOutputGUIForcedView[jointID][0];
            float jointPointY = points2DOutputGUIForcedView[jointID][1];
            //fprintf(stderr,"P x,y %0.2f,%0.2f \n",jointPointX,jointPointY);


            if ( (jointPointX!=0) && (jointPointY!=0) )
                {
                    cv::Point jointPoint(jointPointX+10,jointPointY);
                    cv::circle(img,jointPoint,5,cv::Scalar(255,0,0),3,8,0);

                    const char * jointName = getBVHJointName(jointID);
                    if (jointName!=0)
                        {
                            snprintf(textInfo,512,"%s(%u)",jointName,jointID);
                        }
                    else
                        {
                            snprintf(textInfo,512,"-(%u)",jointID);
                        }
                    cv::putText(img, textInfo  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );
                }
        }


    cv::Point txtPosition;
    txtPosition.x=20;
    txtPosition.y=20;
    float thickness=1;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
    cv::Scalar color= cv::Scalar(123,123,123,123 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);

    if (numberOfFramesToGrab>0)
        {
            snprintf(textInfo,512,"Grabber will stop after collecting  %u frames",numberOfFramesToGrab);
        }
    else
        {
            snprintf(textInfo,512,"Live mode, looping forever will not produce a bvh file");
        }
    txtPosition.y+=30;
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);


    if (totalNumberOfFrames>0)
        {
            snprintf(textInfo,512,"Frame %u/%u",frameNumber,totalNumberOfFrames);
        }
    else
        {
            snprintf(textInfo,512,"Frame %u",frameNumber);
        }
    txtPosition.y+=30;
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);



    if (skippedFrames>0)
        {
            txtPosition.y+=30;
            snprintf(textInfo,512,"Skipped/Corrupted Frames %u",skippedFrames);
            cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);
        }

//  txtPosition.y+=40;
//  snprintf(textInfo,512,"Input Quality : %s",fpsAcquisition);
//  cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,4,8);

    txtPosition.y+=30;
    snprintf(textInfo,512,"Acquisition : %0.2f fps",fpsAcquisition);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);


    txtPosition.y+=30;
    snprintf(textInfo,512,"2D Joint Detector : %0.2f fps",joint2DEstimator);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);

    txtPosition.y+=30;
    snprintf(textInfo,512,"MocapNET : %0.2f fps",fpsMocapNET);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);

    txtPosition.y+=30;
    snprintf(textInfo,512,"Total : %0.2f fps",fpsTotal);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);

    if (drawNSDM)
        {
            visualizeNSDM(
                img,
                mocapNETInput,
                20,
                400,
                200,
                200
            );
        }

    cv::imshow(windowName,img);
    if (handleMessages)
        {
            cv::waitKey(1);
        }
    return 1;
#else
    fprintf(stderr,"OpenCV code not present in this build, cannot show visualization..\n");
    return 0;
#endif
}
