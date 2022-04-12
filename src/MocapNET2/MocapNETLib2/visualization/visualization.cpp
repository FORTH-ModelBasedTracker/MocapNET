#if USE_OPENCV
#include "opencv2/opencv.hpp"
using namespace cv;
#endif


#include "visualization.hpp"
#include "opengl.hpp"
#include "map.hpp"
#include "allInOne.hpp"
#include "camera_ready.hpp"
#include "widgets.hpp"
#include "drawSkeleton.hpp"
#include "template.hpp"
#include "../IO/jsonRead.hpp"
#include "../IO/bvh.hpp"
#include "../tools.hpp"
#include "../mocapnet2.hpp"
//#include "../MocapNETLib2/NSDM/legacyNSDM.hpp"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

std::vector<cv::Point> leftEndEffector;
std::vector<cv::Point> rightEndEffector;


int debug2DPointAlignment(
    const char * windowName, 
    std::vector<float> original2DPoints, 
    std::vector<float> rotated2DPoints, 
    unsigned int pivotPoint,
    unsigned int referencePoint, 
    unsigned int width,
    unsigned int height
)
{
#if USE_OPENCV
     if (original2DPoints.size()!=rotated2DPoints.size())
     {
         return 0;
     }
     
     cv::Mat img(height,width, CV_8UC3, cv::Scalar(0,0,0));
    
     for (int jointID=0; jointID<original2DPoints.size(); jointID++)
                {
                    float jointOutPointX,jointOutPointY;
                      
                    for (int i=0; i<2; i++)
                    {
                      if (i==0)
                      {
                        //original 
                        jointOutPointX = original2DPoints[jointID*3+0] * width;
                        jointOutPointY = original2DPoints[jointID*3+1] * height; 
                      }  else
                      {
                        //rotated
                        jointOutPointX = rotated2DPoints[jointID*3+0] * width;
                        jointOutPointY = rotated2DPoints[jointID*3+1] * height; 
                      }
                    //fprintf(stderr,"P x,y %0.2f,%0.2f \n",jointPointX,jointPointY);

                    if ( (jointOutPointX!=0) && (jointOutPointY!=0) )
                        {
                            cv::Point jointPoint(jointOutPointX+10,jointOutPointY+10);
                            
                            if (pivotPoint==jointID)
                            {
                              cv::circle(img,jointPoint,5,cv::Scalar(255,0,0),3,8,0); 
                              cv::putText(img, "Pivot"  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );
                            } else
                            if (referencePoint==jointID)
                            {
                              cv::circle(img,jointPoint,5,cv::Scalar(0,255,0),3,8,0); 
                              cv::putText(img, "Reference"  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );
                            } else
                            { 
                                //original
                                if (i==0) { cv::circle(img,jointPoint,5,cv::Scalar(255,255,0),3,8,0); }  else
                                //rotated
                                          { cv::circle(img,jointPoint,8,cv::Scalar(0,255,255),3,8,0); }   
                            }
                            
                        }
                        
                    }
                } 
                
                
    cv::imshow(windowName,img);
    return 1;
 #endif
 return 0;   
}



int doReprojectionCheck(struct skeletonSerialized * input,struct skeletonSerialized * result)
{
    //This needs work..
    return 0;
 
#if USE_OPENCV
  cv::Mat visualization(MocapNETTrainingHeight,MocapNETTrainingWidth,CV_8UC3,Scalar(0,0,0));
  cv::Point jointPoint(0,0);
  for (unsigned int jID=3; jID<input->skeletonHeaderElements/3; jID++)
  {
      
      jointPoint.x=input->skeletonBody[jID*3+0].value * MocapNETTrainingWidth;
      jointPoint.y=input->skeletonBody[jID*3+1].value * MocapNETTrainingHeight;
      //fprintf(stderr,"jIDIN %s,%s(%u)=%u,%u\n",input->skeletonHeader[jID*3+0].str,input->skeletonHeader[jID*3+1].str,jID,jointPoint.x,jointPoint.y);
      int thickness=-2;
      cv::circle(visualization,jointPoint,3,cv::Scalar(255,0,255),thickness,8,0);
  }    
 
   for (unsigned int jID=3; jID<result->skeletonHeaderElements/3; jID++)
  {   
      jointPoint.x=result->skeletonBody[jID*3+0].value * MocapNETTrainingWidth;
      jointPoint.y=result->skeletonBody[jID*3+1].value * MocapNETTrainingHeight;
      //fprintf(stderr,"jIDRES %s,%s(%u)=%u,%u\n",result->skeletonHeader[jID*3+0].str,result->skeletonHeader[jID*3+1].str,jID,jointPoint.x,jointPoint.y);
      int thickness=-2;
      cv::circle(visualization,jointPoint,3,cv::Scalar(255,0,0),thickness,8,0);
  }  
    
  cv::imshow("Reprojection Check",visualization);
  return 1;
  #endif
  
  return 0;  
    
}



#if USE_OPENCV

cv::Mat openGLFramePermanentMat;



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
    //TODO: FIX THIS
    return 0;
    
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
            distance=getJoint2DDistance_tools(x2D,y2D,xReprojected,yReprojected);
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
    
    //drawSkeleton(img,place2DSkeletonElsewhere(450,350,200,200,skeleton2D),0/*No 2D skeleton*/,0.0,0.0,1);
    
    //Was 450
    drawSkeleton(img,place2DSkeletonElsewhere(0,150,200,200,skeleton2D),0.0,0.0,1);
    
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

                   (  (joint>=MOCAPNET_OUTPUT___JAW_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_RISORIUS03_R_YROTATION) )  ||
                   (  (joint>=MOCAPNET_OUTPUT_TOE1_1_R_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_LBUTTOCK_YROTATION) )  ||
                   (  (joint>=MOCAPNET_OUTPUT_TOE1_1_L_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_NUMBER) )  ||

                   (  (joint>=MOCAPNET_OUTPUT_METACARPAL1_R_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_FINGER1_3_R_YROTATION) )  ||
                   (  (joint>=MOCAPNET_OUTPUT_METACARPAL1_L_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_FINGER1_3_L_YROTATION) )    ||
                   (  (joint>=MOCAPNET_OUTPUT_RBUTTOCK_ZROTATION) && (joint<=MOCAPNET_OUTPUT_RBUTTOCK_YROTATION) )    || 
                   (  (joint>=MOCAPNET_OUTPUT_LBUTTOCK_ZROTATION) && (joint<=MOCAPNET_OUTPUT_LBUTTOCK_YROTATION) )     ||
                   (  (joint>=MOCAPNET_OUTPUT_CHEST_ZROTATION)    && (joint<=MOCAPNET_OUTPUT_CHEST_YROTATION) )||
                   //(  (joint>=MOCAPNET_OUTPUT_LEFTEYE_ZROTATION)  && (joint<=MOCAPNET_OUTPUT_RIGHTEYE_YROTATION) )  ||
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







int visualizeInput2DSkeletonFromSkeletonStruct(
                                               cv::Mat &outputMat,
                                               struct skeletonStructure * skeleton,
                                               unsigned int skeletonWidth,
                                               unsigned int skeletonHeight,
                                               unsigned int x,unsigned int y,
                                               unsigned int width,unsigned int height
                                              )
{
    #if USE_TRANSPARENCY
     cv::Mat base;
     cv::Mat overlay(outputMat.size().height,outputMat.size().width, CV_8UC3, Scalar(0,0,0));
     outputMat.copyTo(base);
     cv::Mat * baseP = &base;
     cv::Mat * overlayP = &overlay ;
     cv::Mat * outputP = &outputMat;    
    #else
     //Not using transparency you just write everything on output 
     //and get rid of the add weighted last step that takes 6% of CPU time
     cv::Mat * baseP = &outputMat;
     cv::Mat * overlayP = &outputMat ;
     cv::Mat * outputP = &outputMat;
    #endif    
    
    cv::Point parentPoint = cv::Point(x+100,y+100);
    cv::Point targetPoint = cv::Point(x+100,y+100); 
    
    cv::Scalar color;
        
    for (int jointID=0; jointID<BODY25_PARTS; jointID++)
    { 
        float xNormalized = skeleton->body.joint2D[jointID].x / skeletonWidth ;
        float yNormalized = skeleton->body.joint2D[jointID].y / skeletonHeight ;
        
        float xParentNorm= skeleton->body.joint2D[Body25SkeletonJointsParentRelationMap[jointID]].x/skeletonWidth;
        float yParentNorm= skeleton->body.joint2D[Body25SkeletonJointsParentRelationMap[jointID]].y/skeletonHeight;
        
        if  ( (xNormalized!=0) && (yNormalized!=0) )
        {
        
        targetPoint.x =  x+xNormalized *width; 
        targetPoint.y =  y+yNormalized *height;
        
        parentPoint.x = x+xParentNorm*width;
        parentPoint.y = y+yParentNorm*height;
        //fprintf(stderr,"Point%u (%0.2f,%0.2f)",jointID,targetPoint.x, targetPoint.y );
        
        //cv::Scalar(0,123,123)
        cv::circle(*overlayP,targetPoint,3,cv::Scalar(255,0,255),3,8,0);  
        
        switch(jointID)
        {
            case BODY25_LAnkle :   color =  cv::Scalar(0,0,255); break;
            case BODY25_LKnee :   color =  cv::Scalar(0,0,255); break;
            case BODY25_LHip :   color =  cv::Scalar(0,0,255); break;
            case BODY25_LBigToe :   color =  cv::Scalar(0,0,255); break;
            case BODY25_LSmallToe :   color =  cv::Scalar(0,0,255); break;
            case BODY25_LElbow :   color =  cv::Scalar(0,0,255); break;
            case BODY25_LShoulder :   color =  cv::Scalar(0,0,255); break;
            case BODY25_LWrist :   color =  cv::Scalar(0,0,255); break;
            case BODY25_RAnkle :   color =  cv::Scalar(0,255,0); break;
            case BODY25_RKnee :   color =  cv::Scalar(0,255,0); break;
            case BODY25_RHip :   color =  cv::Scalar(0,255,0); break;
            case BODY25_RBigToe :   color =  cv::Scalar(0,255,0); break;
            case BODY25_RSmallToe :   color =  cv::Scalar(0,255,0); break;
            case BODY25_RElbow :   color =  cv::Scalar(0,255,0); break;
            case BODY25_RShoulder :   color =  cv::Scalar(0,255,0); break;
            case BODY25_RWrist :   color =  cv::Scalar(0,255,0); break;
            
            default : 
               color =  cv::Scalar(255,0,0);
        };
        
        if  ( (xParentNorm!=0) && (yParentNorm!=0) )
        {        
          cv::line(*overlayP,targetPoint,parentPoint,color,3.0);
        }
        }
    }
    
    if (skeleton->leftHand.isPopulated)
    {    
      for (int jointID=0; jointID<COCO_HAND_PARTS; jointID++)
      {
        float xNormalized = skeleton->leftHand.joint2D[jointID].x / skeletonWidth ;
        float yNormalized = skeleton->leftHand.joint2D[jointID].y / skeletonHeight ;
        targetPoint.x =  x+xNormalized *width; 
        targetPoint.y =  y+yNormalized *height;
        
        cv::circle(*overlayP,targetPoint,1,cv::Scalar(0,0,255),1,8,0);  
      } 
    }

    if (skeleton->rightHand.isPopulated)
    {    
      for (int jointID=0; jointID<COCO_HAND_PARTS; jointID++)
      {
        float xNormalized = skeleton->rightHand.joint2D[jointID].x / skeletonWidth ;
        float yNormalized = skeleton->rightHand.joint2D[jointID].y / skeletonHeight ;
        targetPoint.x =  x+xNormalized *width; 
        targetPoint.y =  y+yNormalized *height;
        
        cv::circle(*overlayP,targetPoint,1,cv::Scalar(0,255,0),1,8,0);  
      } 
    }

    
    if (skeleton->head.isPopulated)
    {
      for (int jointID=0; jointID<OP_HEAD_PARTS; jointID++)
      {
        float xNormalized = skeleton->head.joint2D[jointID].x / skeletonWidth ;
        float yNormalized = skeleton->head.joint2D[jointID].y / skeletonHeight ;
        targetPoint.x =  x+xNormalized *width; 
        targetPoint.y =  y+yNormalized *height;
        
        //fprintf(stderr,"head(%u,%u)",targetPoint.x,targetPoint.y);
        cv::circle(*overlayP,targetPoint,1,cv::Scalar(0,255,255),1,8,0);  
      }
    }
    
    #if USE_TRANSPARENCY
       cv::addWeighted(*baseP,0.8, *overlayP, 0.3, 0.0, *outputMatP);
    #endif
    
     return 1;
}






int visualizeInput2DSkeletonFromSkeletonSerialized(
                                                   cv::Mat &outputMat,
                                                   struct skeletonSerialized * skeleton,
                                                   unsigned int x,unsigned int y,
                                                   unsigned int width,unsigned int height
                                                  )
{
    /*
    fprintf(stderr,"Visualize %u,%u  -   Skeleton ",skeletonWidth,skeletonHeight);
    fprintf(stderr,"Offset %u,%u  -   ",x,y);
    fprintf(stderr,"Joints %u   -   ",skeleton->skeletonBodyElements);
    fprintf(stderr,"Dimensions  %u,%u  \n ",width,height);
     */  
     
    #if USE_TRANSPARENCY
     cv::Mat base;
     cv::Mat overlay(outputMat.size().height,outputMat.size().width, CV_8UC3, Scalar(0,0,0));
     outputMat.copyTo(base);
     cv::Mat * baseP = &base;
     cv::Mat * overlayP = &overlay ;
     cv::Mat * outputP = &outputMat;    
    #else
     //Not using transparency you just write everything on output 
     //and get rid of the add weighted last step that takes 6% of CPU time
     cv::Mat * baseP = &outputMat;
     cv::Mat * overlayP = &outputMat ;
     cv::Mat * outputP = &outputMat;
    #endif
    
     
    cv::Scalar color = cv::Scalar(0,255,255);
    cv::Point pt1 = cv::Point(x,y); 
    cv::Point pt2 = cv::Point(x+width,y+height); 
    cv::rectangle(*overlayP, pt1, pt2, color); 
       
       
    unsigned int pointsThatExist=0;
    unsigned int pointsThatSeemToBeWronglyNormalized=0;
    
    for (int jointID=3; jointID<skeleton->skeletonBodyElements/3; jointID++)
      {
          unsigned int jID =  jointID*3 ;
          
          float pointExists = skeleton->skeletonBody[jID+2].value;
          
          if (pointExists>0.0)
          {
            float xNormalized =  (float) skeleton->skeletonBody[jID+0].value / skeleton->width;
            float yNormalized =  (float) skeleton->skeletonBody[jID+1].value / skeleton->height; 
           
           if ( (xNormalized!=0.0) || (yNormalized!=0.0) )
           {
               //First of all bring output point to the range of our mini 
               //screen
                pt1.x = (unsigned int) (xNormalized * width); 
                pt1.y = (unsigned int) (yNormalized * height);
                
                //Check if we have a normalization error..!
                if ( (pt1.x>width) || (pt1.y>height) )
                {
                    /*
                    fprintf(
                            stderr,"Point %s,%s is out of bounds (%u,%u)\n", 
                            skeleton->skeletonHeader[jID+0].str,
                            skeleton->skeletonHeader[jID+1].str,
                            pt1.x ,
                            pt1.y 
                           );
                    */
                    ++pointsThatSeemToBeWronglyNormalized;
                }
                
                
                //Finally put the point in the target position on the overlay
                pt1.x += (unsigned int)  x; 
                pt1.y += (unsigned int)  y;
                 
                //And draw it.
                cv::circle(*overlayP,pt1,1,color,1,8,0); 
                ++pointsThatExist;
           }
          }
      }  
      
      if (pointsThatSeemToBeWronglyNormalized>0)
      {
        fprintf(stderr,"visualizeInput2DSkeletonFromSkeletonSerialized %u points seem to be incorrectly normalized\n",pointsThatSeemToBeWronglyNormalized);         
        fprintf(stderr,"signaled units are  %0.2f x %0.2f \n",skeleton->width,skeleton->height);         
      }
      
      float thickness=1.0;
      int fontUsed=cv::FONT_HERSHEY_SIMPLEX; 
      
      if (pointsThatExist==0)
      { 
        pt1.x=x+10;
        pt1.y=y+10;
        cv::putText(*overlayP,"No Valid 2D Points available",pt1,fontUsed,0.4,color,thickness,8);
      }  
      
      if (pointsThatSeemToBeWronglyNormalized>0)
      {
        pt1.x=x+10;
        pt1.y=y+30;
        cv::putText(*overlayP,"Invalid Normalized Input Points",pt1,fontUsed,0.4,color,thickness,8);
      }
      
    #if USE_TRANSPARENCY
       cv::addWeighted(*baseP,1.0,*overlayP, 0.5, 0.0,*outputMatP);
    #endif
       return 1;
}






int visualizeInput2DSkeletonFromVectorofVectors(
                                              cv::Mat &outputMat,
                                              std::vector<std::vector<float> > skeleton,
                                              unsigned int skeletonWidth,unsigned int skeletonHeight,
                                              unsigned int x,unsigned int y,
                                              unsigned int width,unsigned int height
                                            )
{ 
    
    
    /*
    fprintf(stderr,"Visualize %u,%u  -   Skeleton ",skeletonWidth,skeletonHeight);
    fprintf(stderr,"Offset %u,%u  -   ",x,y);
    fprintf(stderr,"Joints %lu   -   ",skeleton.size());
    fprintf(stderr,"Dimensions  %u,%u  \n ",width,height);
    */
  
          
    
    #if USE_TRANSPARENCY
     cv::Mat base;
     cv::Mat overlay(outputMat.size().height,outputMat.size().width, CV_8UC3, Scalar(0,0,0));
     outputMat.copyTo(base);
     cv::Mat * baseP = &base;
     cv::Mat * overlayP = &overlay ;
     cv::Mat * outputP = &outputMat;    
    #else
     //Not using transparency you just write everything on output 
     //and get rid of the add weighted last step that takes 6% of CPU time
     cv::Mat * baseP = &outputMat;
     cv::Mat * overlayP = &outputMat ;
     cv::Mat * outputP = &outputMat;
    #endif
    
     
    cv::Scalar color = cv::Scalar(123,123,0);
    cv::Point pt1 = cv::Point(x,y); 
    cv::Point pt2 = cv::Point(x+width,y+height); 
    cv::rectangle(*overlayP, pt1, pt2, color); 
        
    
    unsigned int pointsThatExist=0;
    unsigned int pointsThatSeemToBeWronglyNormalized=0;
    
    for (int jID=0; jID<skeleton.size(); jID++)
      { 
          float pointExists = ( ( skeleton[jID][0] !=0.0 ) || ( skeleton[jID][1] != 0.0 ) );
          
          if (pointExists>0.0)
          {
            float xNormalized =  (float) skeleton[jID][0] / skeletonWidth;  
            float yNormalized =  (float) skeleton[jID][1] / skeletonHeight; 
           
           if ( (xNormalized!=0.0) || (yNormalized!=0.0) )
           {
               //First of all bring output point to the range of our mini 
               //screen
                pt1.x = (unsigned int) (xNormalized * width); 
                pt1.y = (unsigned int) (yNormalized * height);
                
                //Check if we have a normalization error..!
                if ( (pt1.x>width) || (pt1.y>height) )
                {
                    /*
                    fprintf(
                            stderr,"Point is out of bounds (%u,%u) / (%u,%u) \n", 
                            (unsigned int) pt1.x ,
                            (unsigned int) pt1.y ,
                            width,
                            height
                           );
                         */  
                    ++pointsThatSeemToBeWronglyNormalized;
                }
                
                
                //Finally put the point in the target position on the overlay
                pt1.x += (unsigned int)  x; 
                pt1.y += (unsigned int)  y;
                 
                //And draw it.
                cv::circle(*overlayP,pt1,1,color,1,8,0); 
                ++pointsThatExist;
           }
          }
      }  
      
      if (pointsThatSeemToBeWronglyNormalized>0)
      {
        fprintf(stderr,"visualizeInput2DSkeletonFromSkeletonSerialized %u points seem to be incorrectly normalized\n",pointsThatSeemToBeWronglyNormalized);         
        fprintf(stderr,"signaled units are  %u x %u \n",skeletonWidth,skeletonHeight);         
      }
      
      float thickness=1.0;
      int fontUsed=cv::FONT_HERSHEY_SIMPLEX; 
      
      if (pointsThatExist==0)
      { 
        pt1.x=x+10;
        pt1.y=y+10;
        cv::putText(*overlayP,"No Valid 2D Points available",pt1,fontUsed,0.4,color,thickness,8);
      }  
      
      if (pointsThatSeemToBeWronglyNormalized>0)
      {
        pt1.x=x+10;
        pt1.y=y+30;
        cv::putText(*overlayP,"Invalid Normalized Input Points",pt1,fontUsed,0.4,color,thickness,8);
      }
      
    #if USE_TRANSPARENCY
       cv::addWeighted(*baseP,1.0,*overlayP, 0.5, 0.0,*outputMatP);
    #endif
       return 1;
}











int scaleSkeleton( std::vector<std::vector<float> > &sk, float scaleX,float scaleY)
{
    for (int i=0; i<sk.size(); i++)
    {
        if (sk[i].size()>=2)
        {
           sk[i][0]*=scaleX; 
           sk[i][1]*=scaleY; 
        } 
    }
    return 1;
}


int visualizeInput(
                    const char* windowName,
                    unsigned int frameNumber,
                    unsigned int saveVisualization,
                    cv::Mat * alreadyLoadedImage,
                    const char * path,
                    const char * label,
                    unsigned int serialLength,
                    unsigned int width,
                    unsigned int height, 
                    struct skeletonSerialized * skeleton,
                     struct MocapNET2 * mnet,
                    std::vector<std::vector<float> > points2DOutputGUIRealView,
                    std::vector<std::vector<float> > points2DOutputGUIForcedView,
                    std::vector<std::vector<float> > points2DOutputGUIForcedViewSide, 
                    unsigned int numberOfMissingJoints
                  )
{
#if USE_OPENCV
     int success=0;
    char finalFilename[2048]={0};
    
    if (alreadyLoadedImage==0)
    { 
     if (path==0) { fprintf(stderr,"Can't visualize input without path to RGB images\n"); return 0; }
     
    char formatString[256]= {0};
    snprintf(formatString,256,"%%s/%%s%%0%uu.jpg",serialLength);
     
    //colorFrame_0_00001.jpg
     snprintf(finalFilename,2048,formatString,path,label,frameNumber+1/*Frame ID*/);
    //snprintf(finalFilename,2048,"%s/colorFrame_0_%05d.jpg",path,frameNumber+1);
    }
        
        
     float scale=1.0;   
    cv::Mat image;
    cv::Rect roi;
    cv::Mat destinationROI;
    
    if ( (fileExists(finalFilename) ) || (alreadyLoadedImage!=0) )
    {
           if (fileExists(finalFilename) )
           {
              image = imread(finalFilename, cv::IMREAD_COLOR);   // older versions might want the CV_LOAD_IMAGE_COLOR  flag
           } else
           {
               image =  *alreadyLoadedImage;
           }
     
     if(image.data!=0)
      { 
          if (   image.size().height > image.size().width )
          {
             scale=(float) 720/image.size().height;
          } else
          {
             scale=(float) 1024/image.size().width; 
          } 
        if (scale>1.0) { scale=1.0; }
        if (scale!=1.0)
         {  
             cv::resize(image, image, cv::Size(0,0), scale, scale);
         }
      
      //fprintf(stderr,"Image ( %u x %u )\n",image.size().width,image.size().height);
      success=1;
      }

    } else
    {
      fprintf(stderr," Could not load %s image, cannot proceed to visualize it\n",finalFilename); 
      return 0;
    }
     
      
      //int offsetX=950;
      int offsetX=650;
      cv::Mat visualization(image.size().height,offsetX+image.size().width, CV_8UC3, Scalar(0,0,0));
      //fprintf(stderr,"Visualization will be ( %u x %u )\n",visualization.size().width,visualization.size().height);
       roi = cv::Rect( cv::Point(offsetX,0 ), cv::Size( image.size().width, image.size().height ));
      destinationROI = visualization( roi );
      image.copyTo( destinationROI );
      
    // visualizeInput2DSkeletonFromCOCOStruct(visualization,skeleton,width,height,offsetX,0, image.size().width,image.size().height);
      visualizeInput2DSkeletonFromSkeletonSerialized(visualization,skeleton,offsetX,0, image.size().width,image.size().height);
     
                                                                                                                   
     cv::Point pt1(offsetX,0);
     cv::Point pt2(offsetX+00,image.size().height);
     cv::rectangle(visualization,pt1,pt2,cv::Scalar(0,0,0),-1,8,0);
     
    cv::Scalar color= cv::Scalar(123,123,123,123 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);
    cv::Scalar black= cv::Scalar(0,0,0,0 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);
    cv::Point txtPosition;
    txtPosition.y=30;
    float thickness=2.2;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
    
    
    //FORCE VIEW..!
    //numberOfMissingJoints=0;
      
      if (numberOfMissingJoints>40)
      {
           txtPosition.x=60;
           txtPosition.y=350;
           cv::putText(visualization,"Incomplete Data..!",txtPosition,fontUsed,1.8,color,thickness,8);    
      } else
      {
        txtPosition.x=100;
        cv::putText(visualization,"Front View",txtPosition,fontUsed,1.1,color,thickness,8);   
    
       txtPosition.x=400;
       cv::putText(visualization,"Side View",txtPosition,fontUsed,1.1,color,thickness,8);   
    
    
       drawSkeleton(visualization,points2DOutputGUIForcedView,-350,-50,0);
       drawSkeleton(visualization,points2DOutputGUIForcedViewSide,-50,-50,0); 
      }
      
    txtPosition.x=offsetX+200;
    txtPosition.y=30;
    snprintf(finalFilename,2048,"Orientation Classifier Front(%0.1f)Back(%0.1f) Left(%0.1f) Right(%0.1f)  ",mnet->orientationClassifications[0],mnet->orientationClassifications[1],mnet->orientationClassifications[2],mnet->orientationClassifications[3]);
    cv::putText(visualization,finalFilename,txtPosition,fontUsed,0.7,black,thickness,8);   
    txtPosition.x+=2;
    cv::putText(visualization,finalFilename,txtPosition,fontUsed,0.7,color,thickness,8);   
    txtPosition.x-=2;

    txtPosition.x+=200;
    txtPosition.y=60;
    snprintf(finalFilename,2048,"Perceived Orientation  %s (%u) ",MocapNETOrientationNames[mnet->orientation],mnet->orientation);
    cv::putText(visualization,finalFilename,txtPosition,fontUsed,0.7,black,thickness,8);   
    txtPosition.x+=2;
    cv::putText(visualization,finalFilename,txtPosition,fontUsed,0.7,color,thickness,8);   
    txtPosition.x-=2;

    scaleSkeleton(points2DOutputGUIRealView,scale,scale);
    drawSkeleton(visualization,points2DOutputGUIRealView,offsetX-100,0,0);
     
      
     cv::imshow(windowName,visualization);
     
     if (saveVisualization)
     {
          char filename[1024];
          snprintf(filename,1024,"vis%05u.jpg",frameNumber);
         imwrite(filename,visualization);
     }

    //Did not find a file to show ..
    return success; 
#else
    fprintf(stderr,"OpenCV code not present in this build, cannot show visualization..\n");
#endif
return 0;
}












void  spawnVisualizationWindow(const char* windowName,unsigned int width,unsigned int height)
{
    cv::Mat img(height,width, CV_8UC3, Scalar(0,0,0));
    cv::namedWindow(windowName,cv::WINDOW_AUTOSIZE);
    imshow(windowName,img);
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
    float fpsInverseKinematics,
    float fpsJoint2DEstimator,
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
    struct MocapNET2 * mnet,
    struct skeletonSerialized * skeleton,
    std::vector<float> mocapNETInput,
    std::vector<float> mocapNETOutput,
    std::vector<float> mocapNETOutputWithGUIForcedView,
    std::vector<std::vector<float> > points2DInput,
    std::vector<std::vector<float> > points2DOutput,
    std::vector<std::vector<float> > points2DOutputGUIForcedView,
    int useOpenGLVisualization,
    unsigned int save3DVisualization
)
{  
    if ( (width<=0) || (height<=0) )
     {
         fprintf(stderr,YELLOW "visualizePoints with image %ux%u\n" NORMAL,width,height); 
         return 0;
     }


#if USE_OPENCV
    char textInfo[512]; 
    cv::Scalar color= cv::Scalar(220,220,220,0 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);
    cv::Point txtPosition;
    txtPosition.x=20;
    txtPosition.y=20;
    float thickness=1;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
    
    int drawOrientation = drawNSDM;

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
    //txtPosition.y+=30;
    //cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);


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
    
    
    /* These are covered by the new orientation visualization..
    if (mocapNETOutput.size()>4)
    {
      int foundDirection=0;  
      
      snprintf(textInfo,512,"Orientation Classifier Front(%0.1f)Back(%0.1f) Left(%0.1f) Right(%0.1f)  ",mnet->orientationClassifications[0],mnet->orientationClassifications[1],mnet->orientationClassifications[2],mnet->orientationClassifications[3]);
      txtPosition.y+=30;
      cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);
     
     if ( (mocapNETOutput[4]>=-45) && (mocapNETOutput[4]<=45) )
           { snprintf(textInfo,512,"Direction : Front (%0.2f->%0.2f) " , originalOrientation, mocapNETOutput[4]); foundDirection=1; } else
     if ( (mocapNETOutput[4]<=-45) && (mocapNETOutput[4]>=-135) )
           { snprintf(textInfo,512,"Direction : Left (%0.2f->%0.2f) " , originalOrientation,  mocapNETOutput[4]); foundDirection=1; } else
     if ( (mocapNETOutput[4]>=45) && (mocapNETOutput[4]<=135) )
           { snprintf(textInfo,512,"Direction : Right (%0.2f->%0.2f) " , originalOrientation,  mocapNETOutput[4]); foundDirection=1; } else 
     if ( (mocapNETOutput[4]<=-135) && (mocapNETOutput[4]>=-225) )
           { snprintf(textInfo,512,"Direction : Back A (%0.2f->%0.2f) " , originalOrientation, mocapNETOutput[4]); foundDirection=1; } else  
     if ( (mocapNETOutput[4]>=135) && (mocapNETOutput[4]<=225) )
           { snprintf(textInfo,512,"Direction : Back B (%0.2f->%0.2f) " , originalOrientation,  mocapNETOutput[4]); foundDirection=1; } 
     if (foundDirection)
          {
              txtPosition.y+=30;
             cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);
          }
    }*/
    
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

    if (fpsAcquisition!=0.0)
    {
     txtPosition.y+=30;
     snprintf(textInfo,512,"Acquisition loop : %0.2f fps",fpsAcquisition);
     cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);
    }

    if (fpsJoint2DEstimator!=0.0)
    {
     txtPosition.y+=30;
     snprintf(textInfo,512,"2D Joint Detector : %0.2f fps",fpsJoint2DEstimator);
     cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);
    }

    if (fpsInverseKinematics!=0.0)
    {
     txtPosition.y+=30; 
     
     if (codeOptimizationsForIKEnabled())
     {
       snprintf(textInfo,512,"Inverse Kinematics (SSE%u/MT:%u) : %0.2f fps",codeOptimizationsForIKEnabled(),mnet->options->doMultiThreadedIK,fpsInverseKinematics);        
     } else
     {
       snprintf(textInfo,512,"Inverse Kinematics (Unoptimized/MT:%u) : %0.2f fps",mnet->options->doMultiThreadedIK,fpsInverseKinematics);          
     }
     cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);
    }


    txtPosition.y+=30;
    snprintf(textInfo,512,"Neural Network (4 orientations) : %0.2f fps",fpsMocapNET);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8);

    txtPosition.y+=30;
    snprintf(textInfo,512,"Total : %0.2f fps",fpsTotal);
    cv::putText(img,textInfo,txtPosition,fontUsed,0.8,color,thickness,8); 
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------

 
    if (height<600)
    {
        //If the resolution is very small unclutter visualization..
        drawNSDM=0;
        drawOrientation=0;
    }

    //-----------------------
    //     NSDM matrix 
    //-----------------------
    unsigned int NSDMWidth=120;  //body leftHand rightHand
    unsigned int NSDMHeight=120;
    unsigned  int  NSDMPositionLeftColumnX = 20;
    unsigned  int  NSDMPositionRightColumnX = 170;
            
    if (drawNSDM)
        {
            
            int positionY = height-438;
            
            /* This is obsolete
            if(mnet->body.loadedModels>0)
            {
              visualizeNSDM(img,"All Body",mnet->body.NSDM,2 ,20,positionY,NSDMWidth,NSDMHeight);
            }*/
            
            if(mnet->face.loadedModels>0)
            {
              visualizeNSDM(img,"Face",mnet->face.NSDM,2 /*regular*/,NSDMPositionRightColumnX,positionY,NSDMWidth,NSDMHeight);
            }
            
            positionY+=160;
            visualizeNSDM(img,"Upper Body",mnet->upperBody.NSDM,1 /*angles*/,NSDMPositionLeftColumnX,positionY,NSDMWidth,NSDMHeight);
            visualizeNSDM(img,"Lower Body",mnet->lowerBody.NSDM,1 /*angles*/,NSDMPositionRightColumnX,positionY,NSDMWidth,NSDMHeight);

            positionY+=160;
            if(mnet->leftHand.loadedModels>0)
            {
              visualizeNSDM(img,"Left Hand",mnet->leftHand.NSDM,1/*regular*/,NSDMPositionLeftColumnX,positionY,NSDMWidth,NSDMHeight);
            }
            
            if(mnet->rightHand.loadedModels>0)
            {
              visualizeNSDM(img,"Right Hand",mnet->rightHand.NSDM,1 /*regular*/,NSDMPositionRightColumnX,positionY,NSDMWidth,NSDMHeight);
            }
        }
    //-----------------------    
    
    
    if (drawOrientation)
    {
    int positionY = height-438;
    visualizeOrientation(
        img,
        "Orientation",
        mnet->currentSolution[4], //Orientation value
        mnet->orientationClassifications[0],
        mnet->orientationClassifications[1],
        mnet->orientationClassifications[2],
        mnet->orientationClassifications[3],
        20,
        positionY,
        NSDMWidth,
        NSDMHeight
    );
        
    }
    
    
    if (mocapNETOutput.size()>4)
    {
      drawScale(img,"Distance",width-34,height-238,-1*mocapNETOutput[2]*10,800,4000);
      //drawScale(img,"Height",950,480,mocapNETOutput[1]*10,-500,500);
    }

    //-----------------------
    //      OpenGL stuff 
    //-----------------------
    cv::Mat * openGLMatForVisualization = 0;
    if (useOpenGLVisualization) {
                                  //fprintf(stderr,"updateOpenGLView\n");
                                  updateOpenGLView(mocapNETOutput);

                                  //fprintf(stderr,"visualizeOpenGL\n");
                                  unsigned int openGLFrameWidth=width,openGLFrameHeight=height;
                                  char * openGLFrame = visualizeOpenGL(&openGLFrameWidth,&openGLFrameHeight);
                                  //=====================================================================
                                  if (openGLFrame!=0)
                                       {
                                         fprintf(stderr,"Got Back an OpenGL frame..!\n");
                                         cv::Mat openGLMat(openGLFrameHeight, openGLFrameWidth, CV_8UC3);
                                         unsigned char *   initialPointer = openGLMat.data;
                                         openGLMat.data=(unsigned char * ) openGLFrame;

                                         cv::cvtColor(openGLMat,openGLFramePermanentMat,cv::COLOR_RGB2BGR);
                                         //cv::imshow("OpenGL",openGLFramePermanentMat);
                                         openGLMatForVisualization = &openGLFramePermanentMat;
                                         openGLMat.data=initialPointer;
                                       }
                                  //=====================================================================
                                                        }
                                                        
   if (openGLMatForVisualization!=0)
   { 
     cv::Mat * glMat = (cv::Mat *) openGLMatForVisualization;
     //float alpha=0.3;
     //NOT NEEDED cv::addWeighted(*glMat, alpha, img, 1 - alpha, 0, img) ;
     
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
                floorDimension
            );
        }
//------------------------------------------------------------------------------------------



    //-----------------------------------------------------------------------------------------------------------------------------
    if (endEffectorHistory)
        {
             drawEndEffectorTrack(img,points2DOutputGUIForcedView);
        }
    //-----------------------------------------------------------------------------------------------------------------------------

 
    unsigned int offsetX=width-454;
    unsigned int offsetY=height-262;
    visualizeInput2DSkeletonFromSkeletonSerialized(img,skeleton,offsetX,offsetY,444,250); 


    //View .. 
    visualizeInput2DSkeletonFromVectorofVectors(img,points2DOutputGUIForcedView,width,height,offsetX,offsetY,444,250);

    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------
   //The main star of the show , the skeleton..
    //drawSkeleton(img,points2DOutputGUIForcedView,0/*No 2D skeleton*/,0.0,0.0,1);
    drawSkeleton(img,points2DOutputGUIForcedView,0.0,0.0,1);
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





    //-------------------------------------------------------------------------------
    // If Poses are enabled.. Draw them..
    //-------------------------------------------------------------------------------
     if(mnet->activePose>0)
      { 
           snprintf(textInfo,512,"Pose : %s  (%u)",hardcodedPoseName[mnet->activePose],mnet->activePose);     
            txtPosition.y+=30;
            cv::putText(img,textInfo,txtPosition,fontUsed,1.5,cv::Scalar(0,255,255),thickness,8); 
            cv::circle(img,txtPosition,2*gestureFrame,cv::Scalar(0,255,255),3,8,0);   
        }
    


/*
   if (deadInputPoints>MAXIMUM_NUMBER_OF_NSDM_ELEMENTS_MISSING) 
   {
     txtPosition.y+=30;
     cv::putText(img,"Bad Input - Filtered out..",txtPosition,fontUsed,1.0,cv::Scalar(0,0,255),thickness,8);   
   }
*/

    //At last we are able to show the window..
    if ( (img.size().width >0) && (img.size().height>0) ) { cv::imshow(windowName,img); } else 
                                                          { std::cerr<<"Invalid visualization frame.. \n"; }


    if (save3DVisualization)
    {
        char filename[512];
        snprintf(filename,512,"vis%05u.jpg",frameNumber) ;
        cv::imwrite(filename,img);
    }
    
    //Only handle messages if they are not handled elsewhere
    if (handleMessages)
        { cv::waitKey(1); }

    //Initially place window top left..
    //if (frameNumber==0)
    //  { cv::moveWindow(windowName,0,0); }



    return 1;
#else
    fprintf(stderr,"OpenCV code not present in this build, cannot show visualization..\n");
    return 0;
#endif
}





int visualizationCommander(
    struct MocapNET2 * mnet,
    struct MocapNET2Options * options,
    struct skeletonSerialized * skeleton,
    cv::Mat * frame,
    std::vector<float>  result,
    std::vector<std::vector<float> >   exactMocapNET2DOutput,
    unsigned int frameID,
    unsigned int callingProcessPromisesToHandleMessages
)
{
  if (result.size()==0) { return 0; } 
  
  
            //Retreive gesture name to display it
            char * gestureName=0;
            if (options->doGestureDetection)
                {
                    if (mnet->lastActivatedGesture>0)
                        {
                            gestureName=mnet->recognizedGestures.gesture[mnet->lastActivatedGesture-1].label;
                        }
                }
  
    std::vector<std::vector<float> >  points2DInput;
    //Force Skeleton Position and orientation to make it more easily visualizable
    std::vector<float>  resultCenteredOnScreen = result;
    if (options->constrainPositionRotation>0)
        {
            if (resultCenteredOnScreen.size()>=MOCAPNET_OUTPUT_HIP_XROTATION)
                {
                    float distance=0,rollValue=0,yawValue=0,pitchValue=0;

                    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_XPOSITION]=0.0;
                    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_YPOSITION]=0.0;
                    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_ZPOSITION]=-190.0 - (float) distance;
                    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_ZROTATION]=(float) rollValue;
                    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_YROTATION]=(float) yawValue;
                    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_XROTATION]=(float) pitchValue;
                }
        }

    std::vector<std::vector<float> > points2DOutput = convertBVHFrameTo2DPoints(resultCenteredOnScreen); // ,options->visWidth,options->visHeight

    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_XPOSITION]=0.0;
    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_YPOSITION]=0.0;
    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_ZPOSITION]=-190.0;
    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_ZROTATION]=(float) 0.0;
    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_YROTATION]=(float) 0.0;
    resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_XROTATION]=(float) 0.0;
    std::vector<std::vector<float> > points2DFrontOutput = convertBVHFrameTo2DPoints(resultCenteredOnScreen); // ,options->visWidth,options->visHeight

    if (options->datasetPath)
        {
            resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_XPOSITION]=0.0;
            resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_YPOSITION]=0.0;
            resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_ZPOSITION]=-190.0;
            resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_ZROTATION]=(float) 0.0;
            resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_YROTATION]=(float) 90.0;
            resultCenteredOnScreen[MOCAPNET_OUTPUT_HIP_XROTATION]=(float) 0.0;
            std::vector<std::vector<float> > points2DOutputSide = convertBVHFrameTo2DPoints(resultCenteredOnScreen); // ,options->visWidth,options->visHeight
            
            
            /* //Not needed
                  visualizeInput(
                                                "Input Visualization",
                                                 frameID,
                                                 saveVisualization,
                                                 0,
                                                 datasetPath,
                                                 label,
                                                 serialLength,
                                                 width,
                                                 height,
                                                 &skeleton,
                                                 &mnet,
                                                 exactMocapNET2DOutput,
                                                 points2DOutput,
                                                 points2DOutputSide,
                                                 numberOfMissingJoints
                                               );
            */

            /* This is ugly
            visualizeCameraReady(
                "Camera Ready",
                frameID,
                options->saveVisualization,
                frame,
                options->datasetPath,
                options->label,
                options->serialLength,
                options->width,
                options->height,
                skeleton,
                mnet,
                exactMocapNET2DOutput,
                points2DFrontOutput,
                points2DOutputSide,
                options->numberOfMissingJoints
            ); */
        }


    if (options->visualizationType==0)
        {
            std::vector<float> inputValues;

            //Default visualization
            visualizePoints(
                "3D Points Output",
                frameID,
                0, // broken frames
                0, // totalNumberOfFrames
                0, // numberOfFramesToGrab
                options->CPUName,
                options->GPUName,
                0,//drawFloor
                1,//drawNSDM
                options->totalLoopFPS, //<- total fps
                options->fpsAcquisition,
                mnet->inverseKinematicsFramerate,
                options->fps2DEstimator,
                mnet->neuralNetworkFramerate,
                options->mocapNETMode,
                options->visWidth,
                options->visHeight,
                1,
                0,//Dead Input Points
                mnet->lastActivatedGesture,
                gestureName,
                mnet->recognizedGestures.gestureChecksPerformed - mnet->gestureTimestamp, //gesture stuff
                mnet->lastSkeletonOrientation,
                mnet,
                skeleton,
                inputValues,
                result,
                result,
                points2DInput,
                points2DOutput,
                points2DOutput,
                options->useOpenGLVisualization,
                options->saveVisualization // No 3D visualization saved..
            );
        }
    else if (options->visualizationType==1)
        {
            //Motion history ( graphs of each rotation )
            visualizeMotionHistory("3D Points Output",mnet->poseHistoryStorage.history,points2DFrontOutput); //points2DOutput
            if (!callingProcessPromisesToHandleMessages) { visualizeHandleMessages(); }
        }
    else if (options->visualizationType==2)
        { //Map visualization
            std::vector<float> inputValues;
            visualizeMap(
                "3D Points Output",
                frameID,
                options->width,
                options->height,
                skeleton,
                mnet,
                result,
                exactMocapNET2DOutput,
                options->numberOfMissingJoints, 
                mnet->lastActivatedGesture,
                gestureName,
                mnet->recognizedGestures.gestureChecksPerformed - mnet->gestureTimestamp//gesture stuff
            );    
            if (!callingProcessPromisesToHandleMessages) { visualizeHandleMessages(); }
        } else
         if (options->visualizationType==3)
        {//All in one visualization
            std::vector<float> inputValues;
                 visualizeAllInOne(
                "3D Points Output",
                frameID,
                options->saveVisualization,
                frame,
                options->datasetPath,
                options->label,
                options->serialLength,
                options->width,
                options->height,
                skeleton,
                mnet,
                options,
                exactMocapNET2DOutput,
                points2DFrontOutput,
                options->numberOfMissingJoints
            ); 
            if (!callingProcessPromisesToHandleMessages) { visualizeHandleMessages(); } 
        }      else
         if (options->visualizationType==4)
        { 
            //TEMPLATE VISUALIZATION 
            std::vector<float> inputValues;
            
            visualizeTemplate(
                "3D Points Output",
                frameID,
                options->saveVisualization,
                0,
                options->datasetPath,
                options->label,
                options->serialLength,
                options->width,
                options->height,
                skeleton,
                mnet,
                options,
                exactMocapNET2DOutput,
                points2DFrontOutput,
                options->numberOfMissingJoints
            ); 
           if (!callingProcessPromisesToHandleMessages) { visualizeHandleMessages(); } 
        }    
    else
        {
            fprintf(stderr,"Unhandled visualization type %u\n",options->visualizationType);
        }

    return 1;
}

