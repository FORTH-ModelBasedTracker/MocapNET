#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv;
#endif

#include "jsonCocoSkeleton.h"
#include "visualization.hpp"
#include "bvh.hpp"
#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/nsdm.hpp"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


std::vector<cv::Point> leftEndEffector;
std::vector<cv::Point> rightEndEffector;

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
    std::vector<float> NSDM = compressMocapNETInputToNSDM(mocapNETInput,addSyntheticPoints,doScaleCompensation);

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
                            
                            //Visualize error
                            if (  (NSDM[item]>1.0) || (NSDM[item+1]>1.0)  )
                               { 
                                 cv::Point topRight(x+xI*boxX+boxX,y+yI*boxY);
                                 cv::Point bottomLeft(x+xI*boxX,y+yI*boxY+boxY);
                                 cv::line(img,topLeft,bottomRight, cv::Scalar(0,0,255), 1.0);
                                 cv::line(img,topRight,bottomLeft, cv::Scalar(0,0,255), 1.0);
                               }
  
                            
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
    float m=10.0; //Minimum 
    
    for (int jointID=0; jointID<gridPoints2D.size(); jointID++)
        { 
            cv::Point jointPoint( gridPoints2D[jointID][0], gridPoints2D[jointID][1]);

            if (jointID+floorDimension<gridPoints2D.size())
                {
                    verticalPoint.x=gridPoints2D[jointID+floorDimension][0];
                    verticalPoint.y=gridPoints2D[jointID+floorDimension][1];
                }
            else
                {
                    verticalPoint.x=0.0;
                    verticalPoint.y=0.0;
                }


            if ( (jointPoint.x>m) && (jointPoint.y>m) )
                {
                    cv::circle(img,jointPoint,2,cv::Scalar(255,255,0),3,8,0);
                  if ( (jointPoint.x>m) && (jointPoint.y>m) &&  (verticalPoint.x>m) && (verticalPoint.y>m) )
                        {
                            cv::line(img,jointPoint,verticalPoint, cv::Scalar(255,255,0), 1.0);
                        }
                }

 
            if (jointID%floorDimension!=0)
                {
             if ( (jointPoint.x>m) && (jointPoint.y>m) && (parentPoint.x>m) && (parentPoint.y>m) )
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
    if (
        (points2DInput.size()==0)
    )
        {
            fprintf(stderr,YELLOW "visualizeSkeletonCorrespondence cannot display something without the input 2D points\n" NORMAL);
            return 0;
        }

    if (
        (points2DOutput.size()==0)
    )
        {
            fprintf(stderr,YELLOW "visualizeSkeletonCorrespondence cannot display something without the output 2D points\n" NORMAL);
            return 0;
        }


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






int visualizeCameraIntensities(const char* windowName, cv::Mat &imgOriginal,int forceColors)
{
    float fontSize = 0.3;
    unsigned int verticalSpace=25;
    unsigned int horizontalSpace=10;

    unsigned int x=0,y=10;
    char text[512];
    if ( (imgOriginal.rows!=0) && (imgOriginal.cols!=0) )
        {
            cv::Mat img(32,32, CV_8UC3, cv::Scalar(0,0,0));
            cv::resize(imgOriginal, img, img.size() ,0,0,INTER_NEAREST);
            //cv::imshow(windowName,img);
            cv::Mat imgV((3+img.cols) * horizontalSpace *3 ,img.rows * verticalSpace, CV_8UC3,cv::Scalar(0,0,0));
            for(int r=0; r<img.rows; ++r)
                {
                    x=0;
                    for(int c=0; c<img.cols; ++c)
                        {
                            unsigned char * p = img.ptr(r,c); // Y first, X after
                            cv::Point jointPoint(x,y);
                            snprintf(text,512,"%u",p[0]);
                            if (forceColors) p[0]=255;
                            cv::putText(imgV, text  , jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(p[0],0,0), 0.2, cv::LINE_4 );
                            x=x+horizontalSpace;
                            jointPoint.x=x;
                            snprintf(text,512,"%u",p[1]);
                            if (forceColors) p[1]=255;
                            cv::putText(imgV, text  , jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,p[1],0), 0.2, cv::LINE_4 );
                            x=x+horizontalSpace;
                            jointPoint.x=x;
                            snprintf(text,512,"%u",p[2]);
                            if (forceColors) p[2]=255;
                            cv::putText(imgV, text  , jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,0,p[2]), 0.2, cv::LINE_4 );
                            x=x+horizontalSpace;
                            jointPoint.x=x;
                        }
                    y=y+verticalSpace;
                }


            cv::Point jointPoint(10,10);
            fontSize = 0.5;
            cv::putText(imgV,"Raw RGB Image Data", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,255), 0.2, cv::LINE_8);
            cv::imshow(windowName,imgV);
            cv::waitKey(1);
            return 1;
        }
    return 0;
}




int visualizeCameraChannels(const char* windowName,cv::Mat &img,int channelNumber)
{
    if ( (img.rows!=0) && (img.cols!=0) )
        {

            Mat channel[3];
            // The actual splitting.
            split(img, channel);

            cv::Point jointPoint(10,10);
            float fontSize = 0.5;


            if (channelNumber<3)
                {
                    if (channelNumber==0)
                        {
                            cv::putText(channel[channelNumber],"Blue channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,0,0), 0.2, cv::LINE_8);
                        }
                    else if (channelNumber==1)
                        {
                            cv::putText(channel[channelNumber],"Green channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,255,0), 0.2, cv::LINE_8);
                        }
                    else if (channelNumber==2)
                        {
                            cv::putText(channel[channelNumber],"Red channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,0,255), 0.2, cv::LINE_8);
                        }
                    cv::imshow(windowName, channel[channelNumber]);
                }
            else if (channelNumber>=3)
                {
                    cv::Mat imageMerged;

                    if (channelNumber==3)
                        {
                            channel[0]=cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
                            cv::merge(channel,3,imageMerged);
                            cv::putText(imageMerged,"Green + Red channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,255,255), 0.2, cv::LINE_8);
                        }
                    else   //Set blue channel to 0
                        if (channelNumber==4)
                            {
                                channel[1]=cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
                                cv::merge(channel,3,imageMerged);
                                cv::putText(imageMerged,"Blue + Red channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,0,255), 0.2, cv::LINE_8);
                            }
                        else   //Set blue channel to 0
                            if (channelNumber==5)
                                {
                                    channel[2]=cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
                                    cv::merge(channel,3,imageMerged);
                                    cv::putText(imageMerged,"Green + Blue channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,0), 0.2, cv::LINE_8);
                                }
                            else     //Set blue channel to 0
                                {
                                    cv::merge(channel,3,imageMerged);
                                    cv::putText(imageMerged,"Red + Green + Blue channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,255), 0.2, cv::LINE_8);
                                }

                    cv::imshow(windowName,imageMerged);
                }


            return 1;
        }
    return 0;
}



int visualizeCameraEdges(const char* windowName,cv::Mat &img)
{
    cv::Mat edges;
    cv::cvtColor(img, edges, CV_BGR2GRAY);

    cv::Canny(edges, edges, 30, 60);

    cv::Point jointPoint(10,10);
    float fontSize = 0.5;
    cv::putText(edges,"Edges", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,255), 0.2, cv::LINE_8);
    cv::imshow(windowName, edges);
    return 1;
}



int visualizeCameraFeatures(const char* windowName,cv::Mat &img)
{
    std::vector<cv::KeyPoint> keypoints;

    cv::Mat imgCopy; //= img.clone();
    cv::resize(img, imgCopy, img.size() ,0,0,INTER_NEAREST);
    cv::FAST(imgCopy,keypoints,100.0,false);
    char coordinates[256];


    for (int i=0; i<keypoints.size(); i++)
        {
            cv::circle(imgCopy,keypoints[i].pt,5,cv::Scalar(0,255,0),3,8,0);
            cv::Point jointPoint(keypoints[i].pt.x,keypoints[i].pt.y);
            snprintf(coordinates,256,"%0.1f,%0.1f",keypoints[i].pt.x,keypoints[i].pt.y);
            cv::putText(imgCopy,coordinates, jointPoint, cv::FONT_HERSHEY_DUPLEX,0.4, cv::Scalar(0,255,255), 0.2, cv::LINE_4);
        }


    cv::Point jointPoint(10,10);
    float fontSize = 0.5;
    cv::putText(imgCopy,"Features", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,255), 0.2, cv::LINE_8);
    cv::imshow(windowName,imgCopy);
}



int visualizeFigure(const char* windowName,cv::Mat &img)
{
    if ( (img.rows!=0) && (img.cols!=0) )
        {
            cv::imshow(windowName, img);
            return 1;
        }
    return 0;
}




int drawSkeleton(cv::Mat &outputMat,std::vector<std::vector<float> > points2DOutputGUIForcedView)
{
    char textInfo[512];
    
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
                                    
                                    cv::Scalar color = cv::Scalar(0,255,0);
                                    const char * jointName = getBVHJointName(jointID);
                                    const char * parentName = getBVHJointName(parentID);
                                    if ( (parentName!=0) && (jointName!=0) )
                                      {
                                        if (strcmp("hip",jointName)==0)     { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("head",jointName)==0)    { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("neck",jointName)==0)    { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("chest",jointName)==0)   { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("chest",parentName)==0)  { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("abdomen",jointName)==0) { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("head",parentName)==0)   { color = cv::Scalar(255,0,0); }  else 
                                        //-------------------------------------------------------------------------
                                        if (strcmp("lShldr",jointName)==0)  { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lForeArm",jointName)==0){ color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lHand",jointName)==0)   { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lHand",parentName)==0)  { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lButtock",jointName)==0){ color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lShin",jointName)==0)   { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lFoot",jointName)==0)   { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lFoot",parentName)==0)  { color = cv::Scalar(0,0,255);  }  
                                        //-------------------------------------------------------------------------
                                        /* //Default is Green so don't need to spam strcmp..
                                        if (strcmp("rShldr",jointName)==0)  { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rForeArm",jointName)==0){ color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rHand",jointName)==0)   { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rHand",parentName)==0)  { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rButtock",jointName)==0){ color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rShin",jointName)==0)   { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rFoot",jointName)==0)   { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rFoot",parentName)==0)  { color = cv::Scalar(0,255,0);  }
                                        */
                                    }
                                    
                                    
                                    if ( (parentPointX!=0) && (parentPointY!=0) )
                                        {
                                            cv::line(outputMat,jointPoint,parentPoint,color,5.0);
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
                    cv::Point jointPoint(jointPointX,jointPointY);
                    int thickness=-2;
                    cv::circle(outputMat,jointPoint,5,cv::Scalar(255,0,255),thickness,8,0);

                    int filterOut = 1;    
                    const char * jointName = getBVHJointName(jointID);
                    if (jointName!=0)
                        {
                            snprintf(textInfo,512,"%s",jointName);
                            
                            if (strcmp("head",jointName)==0)      { filterOut = 0; }  else
                            if (strcmp("neck",jointName)==0)      { filterOut = 0; }  else
                            if (strcmp("chest",jointName)==0)     { filterOut = 0; }  else
                            if (strcmp("lShldr",jointName)==0)    { filterOut = 0; }  else
                            if (strcmp("rShldr",jointName)==0)    { filterOut = 0; }  else
                            if (strcmp("rShin",jointName)==0)     { filterOut = 0; }  else
                            if (strcmp("lShin",jointName)==0)     { filterOut = 0; }  else
                            if (strcmp("rForeArm",jointName)==0)  { filterOut = 0; }  else
                            if (strcmp("lForeArm",jointName)==0)  { filterOut = 0; }  else
                            if (strcmp("rFoot",jointName)==0)     { filterOut = 0; }  else
                            if (strcmp("lFoot",jointName)==0)     { filterOut = 0; }   
                        }
                    else
                        {
                            snprintf(textInfo,512,"-(%u)",jointID);
                        }
                     
                    
                    jointPoint.x+=10;
                    if (!filterOut)
                      { cv::putText(outputMat, textInfo  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 ); }
                }
        }
        
     return 1;   
}




int drawEndEffectorTrack(cv::Mat &outputMat,std::vector<std::vector<float> > points2DOutputGUIForcedView)
{ 
             int maxEndEffectorHistory=10;
            unsigned int jointIDLeftHand= getBVHJointIDFromJointName("lHand");
            cv::Point leftHand(points2DOutputGUIForcedView[jointIDLeftHand][0],points2DOutputGUIForcedView[jointIDLeftHand][1]);
            leftEndEffector.push_back(leftHand);
            if (leftEndEffector.size()>maxEndEffectorHistory)
                {
                    leftEndEffector.erase(leftEndEffector.begin());
                }

            unsigned int jointIDRightHand= getBVHJointIDFromJointName("rHand");
            cv::Point rightHand(points2DOutputGUIForcedView[jointIDRightHand][0],points2DOutputGUIForcedView[jointIDRightHand][1]);
            rightEndEffector.push_back(rightHand);
            if (rightEndEffector.size()>maxEndEffectorHistory)
                {
                    rightEndEffector.erase(rightEndEffector.begin());
                }

             float stepColorD=(float) 255/maxEndEffectorHistory;
            for (int step=0; step<rightEndEffector.size(); step++)
                {
                    float stepColor = 255 - step*stepColorD;
                    if (step>1)
                    {
                       cv::line(outputMat,leftEndEffector[step],leftEndEffector[step-1], cv::Scalar(0,stepColor,stepColor), 1.0);
                       cv::line(outputMat,rightEndEffector[step],rightEndEffector[step-1], cv::Scalar(0,stepColor,stepColor), 1.0);
                        
                    }
                    cv::circle(outputMat,leftEndEffector[step],1,cv::Scalar(0,stepColor,stepColor),3,8,0);
                    cv::circle(outputMat,rightEndEffector[step],1,cv::Scalar(0,stepColor,stepColor),3,8,0);
                }
                
        return 1;        
}




cv::Scalar getColorFromIndex(unsigned int i)
{
    cv::Scalar color = cv::Scalar::all(255);
    
    i = i%107;
    
    color[0] = lineColorIndex[i*3+0];
    color[1] = lineColorIndex[i*3+1];
    color[2] = lineColorIndex[i*3+2];
    
    
    return color;
}


std::vector<std::vector<float> > place2DSkeletonElsewhere(unsigned int x,unsigned int y,
unsigned int width,
unsigned int height, 
std::vector<std::vector<float> >  skeleton2D)
{
    int jointID=0;
    
    for (jointID=0; jointID<skeleton2D.size(); jointID++)
    {
        skeleton2D[jointID][0]+=x;
        skeleton2D[jointID][1]+=y;
    }
    
    return skeleton2D;
}
#endif


int visualizeHandleMessages()
{
#if USE_OPENCV
    cv::waitKey(1); 
    return 1;
 #else
   fprintf(stderr,"visualizeHandleMessages cannot be compiled without OpenCV\n");
   return 0;
#endif   
}

int visualizeMotionHistory(const char* windowName, std::vector<std::vector<float> > history, std::vector<std::vector<float> >  skeleton2D)
{
#if USE_OPENCV
    if (history.size()==0)
    {
        return 0;
    }


    unsigned int visualizeWidth=1170;
    unsigned int visualizeHeight=1024;
    cv::Mat img(visualizeHeight,visualizeWidth, CV_8UC3, cv::Scalar(0,0,0));
    
    drawSkeleton(img,place2DSkeletonElsewhere(450,350,200,200,skeleton2D));
    
    unsigned int widthOfGraphs=165;
    unsigned int heightOfGraphs=100;
    unsigned int i=0,joint=0;
    
    unsigned int plotPosX=0;
    unsigned int plotPosY=0;
    unsigned int shiftY=15;
    
    char labelOfPlot[512];
    
    
    for (i=1; i<history.size(); i++)
    {
        plotPosX=0;
        plotPosY=shiftY;
        
       for (joint=0; joint<history[i].size(); joint++)
       { 
        if ( 
                   (  (joint>=MOCAPNET_OUTPUT_RTHUMB1_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_RPINKY2_YROTATION) )  ||
                   (  (joint>=MOCAPNET_OUTPUT_LTHUMB1_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_LPINKY2_YROTATION) )    ||
                   (  (joint>=MOCAPNET_OUTPUT_RBUTTOCK_ZROTATION) && (joint<=MOCAPNET_OUTPUT_RBUTTOCK_YROTATION) )    || 
                   (  (joint>=MOCAPNET_OUTPUT_LBUTTOCK_ZROTATION) && (joint<=MOCAPNET_OUTPUT_LBUTTOCK_YROTATION) )     ||
                   (  (joint>=MOCAPNET_OUTPUT_CHEST_ZROTATION)    && (joint<=MOCAPNET_OUTPUT_CHEST_YROTATION) )||
                   (  (joint>=MOCAPNET_OUTPUT_LEFTEYE_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_RIGHTEYE_YROTATION) )  ||
                   (  (joint>=MOCAPNET_OUTPUT_RCOLLAR_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_RCOLLAR_YROTATION) )   ||
                   (  (joint>=MOCAPNET_OUTPUT_LCOLLAR_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_LCOLLAR_YROTATION) ) 
              ) 
        {
            //Don't plot hands for now..
            //Don't plot dead joints
        } else
        {
           if (i==5)
            { 
             cv::Point labelPosition(plotPosX,    plotPosY);
             float absoluteValue = history[history.size()-1][joint];
             float velocityValue = history[history.size()-1][joint] - history[history.size()-2][joint];  
             float accelerationValue = velocityValue - ( history[history.size()-3][joint] - history[history.size()-4][joint] );  
              
             
             snprintf(labelOfPlot,512,"%s  -> %0.2f",MocapNETOutputArrayNames[joint],absoluteValue);
             cv::putText(img,labelOfPlot, labelPosition, cv::FONT_HERSHEY_DUPLEX, 0.3, cv::Scalar::all(255), 0.2, 8 );

             snprintf(labelOfPlot,512,"v %0.2f acc %0.2f",velocityValue,accelerationValue);
             labelPosition.y+=10;
             cv::putText(img,labelOfPlot, labelPosition, cv::FONT_HERSHEY_DUPLEX, 0.3, cv::Scalar::all(255), 0.2, 8 );

            }          
         
            cv::Point jointPointPrev(plotPosX+ i-1,    plotPosY+history[i-1][joint] + heightOfGraphs/2 );
            cv::Point jointPointNext(plotPosX+ i,        plotPosY+history[i][joint] + heightOfGraphs/2);
            cv::Scalar usedColor = getColorFromIndex(joint);
            
            if (joint==4)
            { 
                //Bright RED color for the Y Orientation which is very important 
                usedColor=cv::Scalar(0,0,255);
            }
            
            cv::line(img,jointPointPrev,jointPointNext,usedColor, 1.0); 
            plotPosY+=heightOfGraphs;       
            
           if (plotPosY+heightOfGraphs+shiftY>1000)
           {
             plotPosX+=widthOfGraphs;
             plotPosY=shiftY;
           }
        }
         
       } 
    } 
    
    cv::imshow(windowName, img);
    return 1;
 #else
   fprintf(stderr,"visualizeMotionHistory cannot be compiled without OpenCV\n");
   return 0;
#endif   
}


int visualizePoints(
    const char* windowName,
    unsigned int frameNumber,
    unsigned int skippedFrames,
    signed int totalNumberOfFrames,
    unsigned int numberOfFramesToGrab,
    const char * CPUName,
    const char * GPUName,                  
    int drawFloor,
    int drawNSDM,
    float fpsTotal,
    float fpsAcquisition,
    float joint2DEstimator,
    float fpsMocapNET,
    unsigned int mocapNETMode,
    unsigned int width,
    unsigned int height,
    unsigned int handleMessages,
    unsigned int deadInputPoints,
    unsigned int gestureDetected,
    const char * gestureName,
    unsigned int gestureFrame,
    float originalOrientation,
    std::vector<float> mocapNETInput,
    std::vector<float> mocapNETOutput,
    std::vector<float> mocapNETOutputWithGUIForcedView,
    std::vector<std::vector<float> > points2DInput,
    std::vector<std::vector<float> > points2DOutput,
    std::vector<std::vector<float> > points2DOutputGUIForcedView,
    void* optionalOpenGLCVMat
)
{  
#if USE_OPENCV

    char textInfo[512]; 
    cv::Scalar color= cv::Scalar(123,123,123,123 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);
    cv::Point txtPosition;
    txtPosition.x=20;
    txtPosition.y=20;
    float thickness=1;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
    

    cv::Mat img(height,width, CV_8UC3, Scalar(0,0,0));
    if (mocapNETOutput.size()==0)
        {
            txtPosition.x=100;
            txtPosition.y=400;
            fprintf(stderr,YELLOW "Won't visualize empty neural network output for frame %u\n" NORMAL,frameNumber); 
            cv::putText(img,"No visible person..",txtPosition,fontUsed,2.8,cv::Scalar(255,255,255),thickness,8);   
            cv::imshow(windowName,img);
            return 0;
        }

    if (points2DOutput.size()==0)
        {
            txtPosition.x=100;
            txtPosition.y=400;
            fprintf(stderr,YELLOW "Won't visualize empty 2D points for frame %u\n" NORMAL,frameNumber);
            cv::putText(img,"No 2D skeleton..",txtPosition,fontUsed,2.8,cv::Scalar(255,255,255),thickness,8);   
            cv::imshow(windowName,img);
            return 0;
        }



    //-----------------------------------------------------------------------------------------------------------------------------
    //  The yellow lines that track the hand movement! 
    int  endEffectorHistory=1;

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




 

    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //                  We print all the lines of text that give information
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------


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


    if (CPUName!=0)
    {
      if (strlen(CPUName)!=0) 
      {
        snprintf(textInfo,512,"CPU:%s",CPUName);
        txtPosition.y+=30;
        cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);   
      } 
    }
     
    if (GPUName!=0)
    {
      if (strlen(GPUName)!=0) 
      {
        snprintf(textInfo,512,"GPU:%s",GPUName);
        txtPosition.y+=30;
        cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);   
      } 
    }
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
 
    txtPosition.y+=30;
    snprintf(textInfo,512,"Acquisition : %0.2f fps",fpsAcquisition);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);


    txtPosition.y+=30;
    snprintf(textInfo,512,"2D Joint Detector : %0.2f fps",joint2DEstimator);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);

    txtPosition.y+=30;
    snprintf(textInfo,512,"MocapNET (%u-way) : %0.2f fps",mocapNETMode,fpsMocapNET);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);

    txtPosition.y+=30;
    snprintf(textInfo,512,"Total : %0.2f fps",fpsTotal);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8); 
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------

    if (mocapNETOutput.size()>4)
    {
      int foundDirection=0;  
     
     if ( (mocapNETOutput[4]>=-45) && (mocapNETOutput[4]<=45) )  
           { snprintf(textInfo,512,"Direction : Front (%0.2f->%0.2f) " , originalOrientation, mocapNETOutput[4]);  foundDirection=1; } else
     if ( (mocapNETOutput[4]<=-45) && (mocapNETOutput[4]>=-135) )  
           { snprintf(textInfo,512,"Direction : Left (%0.2f->%0.2f) " ,originalOrientation,  mocapNETOutput[4]); foundDirection=1;  } else
     if ( (mocapNETOutput[4]>=45) && (mocapNETOutput[4]<=135) )  
           { snprintf(textInfo,512,"Direction : Right (%0.2f->%0.2f) " ,originalOrientation,  mocapNETOutput[4]); foundDirection=1;  } else 
     if ( (mocapNETOutput[4]<=-135) && (mocapNETOutput[4]>=-225) )  
           { snprintf(textInfo,512,"Direction : Back A (%0.2f->%0.2f) " , originalOrientation, mocapNETOutput[4]);  foundDirection=1; }  else  
     if ( (mocapNETOutput[4]>=135) && (mocapNETOutput[4]<=225) )  
          { snprintf(textInfo,512,"Direction : Back B (%0.2f->%0.2f) " ,originalOrientation,  mocapNETOutput[4]); foundDirection=1;  } 
     if (foundDirection)
          { 
              txtPosition.y+=30;
             cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8); 
          }     
    }
 

    //-----------------------
    //     NSDM matrix 
    //-----------------------
    if (drawNSDM)
        {
            visualizeNSDM(
                img,
                mocapNETInput,
                20,
                480,
                200,
                200
            );
        }
    //-----------------------    



    //-----------------------
    //      OpenGL stuff 
    //-----------------------
   if (optionalOpenGLCVMat!=0)
   { 
     cv::Mat * glMat = (cv::Mat *) optionalOpenGLCVMat;
     //float alpha=0.3;
     //cv::addWeighted(*glMat, alpha, img, 1 - alpha, 0, img) ;
     
     img=cv::max(*glMat, img);
   } else
   {
       

//------------------------------------------------------------------------------------------
//  Draw floor as a grid of lines 
//------------------------------------------------------------------------------------------
    if (drawFloor)
        {
            float floorX=0,floorY=0,floorZ=0;

            if (mocapNETOutputWithGUIForcedView.size()>5)
                {
                    floorX=mocapNETOutputWithGUIForcedView[3];
                    floorY=mocapNETOutputWithGUIForcedView[4];
                    floorZ=mocapNETOutputWithGUIForcedView[5];
                }

            unsigned int floorDimension=20;
            drawFloorFromPrimitives(
                img,
                floorX,
                floorY,
                floorZ,
                floorDimension,
                width,
                height
            );
        }
//------------------------------------------------------------------------------------------



    //-----------------------------------------------------------------------------------------------------------------------------
    if (endEffectorHistory)
        {
             drawEndEffectorTrack(img,points2DOutputGUIForcedView);
        }
    //-----------------------------------------------------------------------------------------------------------------------------

 

    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
   //The main star of the show , the skeleton..
    drawSkeleton(img,points2DOutputGUIForcedView);
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------

   }
    //-----------------------




    //-------------------------------------------------------------------------------
    // If gestures are enabled.. Draw them..
    //-------------------------------------------------------------------------------
    if (gestureName!=0)
    {//-------------------------------------------------------------------------------
     if(gestureDetected)
      {
        if (gestureFrame<25)
        {
           snprintf(textInfo,512,"%s  (%u)",gestureName,gestureDetected);
          
          if (endEffectorHistory)   
          {//If we track end effectors we will use their position to emit circles
            if (leftEndEffector.size()>0)
            { 
                cv::circle(img,leftEndEffector[leftEndEffector.size()-1],5*gestureFrame,cv::Scalar(0,255,255),3,8,0);  
            }
          if (rightEndEffector.size()>0)
            {
                //Right is always left :P so there is space for the string
                cv::putText(img,textInfo,rightEndEffector[rightEndEffector.size()-1],fontUsed,2.2,cv::Scalar(0,255,255),thickness,8); 
                cv::circle(img,rightEndEffector[rightEndEffector.size()-1],5*gestureFrame,cv::Scalar(0,255,255),3,8,0);  
            }
          } else
          {
            txtPosition.y+=30;
            cv::putText(img,textInfo,txtPosition,fontUsed,1.5,cv::Scalar(0,255,255),thickness,8); 
            cv::circle(img,txtPosition,2*gestureFrame,cv::Scalar(0,255,255),3,8,0);  
          }
        }
     } else
     {
       //We did not detect a gesture.. Let's check the  framerate
       if (fpsTotal<13)
       {
           txtPosition.y+=30;
           cv::putText(img,"Framerate is too slow for reliable gesture recognition..",txtPosition,fontUsed,0.8,color,thickness,8);  
       }
     }
   } //-------------------------------------------------------------------------------



   if (deadInputPoints>102)
   {
     txtPosition.y+=30;
     cv::putText(img,"Bad Input - Filtered out..",txtPosition,fontUsed,1.0,cv::Scalar(0,0,255),thickness,8);   
   }


    //At last we are able to show the window..
    cv::imshow(windowName,img);
    
    //Only handle messages if they are not handled elsewhere
    if (handleMessages)
        { cv::waitKey(1); }

    //Initially place window top left..
    if (frameNumber==0)
      { cv::moveWindow(windowName,0,0); }



    return 1;
#else
    fprintf(stderr,"OpenCV code not present in this build, cannot show visualization..\n");
    return 0;
#endif
}
