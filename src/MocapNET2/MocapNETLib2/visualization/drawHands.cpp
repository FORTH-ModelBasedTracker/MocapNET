#include "drawHands.hpp"

#include "../solutionParts/leftHand.hpp"
#include "../solutionParts/rightHand.hpp"

#include "../IO/bvh.hpp"

enum handJointNames
{ 
 HAND_JOINT_HAND=0,           //0
 HAND_JOINT_METACARPAL1,       //1
 HAND_JOINT_FINGER2_1,         //2
 HAND_JOINT_FINGER2_2,         //3
 HAND_JOINT_FINGER2_3,         //4
 HAND_JOINT_ENDSITE_FINGER2_3, //5
 HAND_JOINT_METACARPAL2,       //6
 HAND_JOINT_FINGER3_1,         //7
 HAND_JOINT_FINGER3_2,         //8
 HAND_JOINT_FINGER3_3,         //9
 HAND_JOINT_ENDSITE_FINGER3_3, //10
 HAND_JOINT__METACARPAL3,      //11
 HAND_JOINT_METACARPAL3,       //12
 HAND_JOINT_FINGER4_1,         //13
 HAND_JOINT_FINGER4_2,         //14
 HAND_JOINT_FINGER4_3,         //15
 HAND_JOINT_ENDSITE_FINGER4_3, //16
 HAND_JOINT__METACARPAL4,      //17
 HAND_JOINT_METACARPAL4,       //18
 HAND_JOINT_FINGER5_1,         //19
 HAND_JOINT_FINGER5_2,         //20
 HAND_JOINT_FINGER5_3,         //21
 HAND_JOINT_ENDSITE_FINGER5_3, //22
 HAND_JOINT__THUMB,            //23
 HAND_JOINT_THUMB,             //24
 HAND_JOINT_FINGER1_2,         //25
 HAND_JOINT_FINGER1_3,         //26
 HAND_JOINT_ENDSITE_FINGER1_3  //27
};

int handGroup[]=
{
 0,//HAND_JOINT_HAND=0,           //0
 0,//HAND_JOINT_METACARPAL1,       //1
 2,//HAND_JOINT_FINGER2_1,         //2
 2,//HAND_JOINT_FINGER2_2,         //3
 2,//HAND_JOINT_FINGER2_3,         //4
 2,//HAND_JOINT_ENDSITE_FINGER2_3, //5
 23,//HAND_JOINT_METACARPAL2,       //6
 3,//HAND_JOINT_FINGER3_1,         //7
 3,//HAND_JOINT_FINGER3_2,         //8
 3,//HAND_JOINT_FINGER3_3,         //9
 3,//HAND_JOINT_ENDSITE_FINGER3_3, //10
 34,//HAND_JOINT__METACARPAL3,      //11
 34,//HAND_JOINT_METACARPAL3,       //12
 4,//HAND_JOINT_FINGER4_1,         //13
 4,//HAND_JOINT_FINGER4_2,         //14
 4,//HAND_JOINT_FINGER4_3,         //15
 4,//HAND_JOINT_ENDSITE_FINGER4_3, //16
 9,//HAND_JOINT__METACARPAL4,      //17
 9,//HAND_JOINT_METACARPAL4,       //18
 5,//HAND_JOINT_FINGER5_1,         //19
 5,//HAND_JOINT_FINGER5_2,         //20
 5,//HAND_JOINT_FINGER5_3,         //21
 5,//HAND_JOINT_ENDSITE_FINGER5_3, //22
 9,//HAND_JOINT__THUMB,            //23
 1,//HAND_JOINT_THUMB,             //24
 1,//HAND_JOINT_FINGER1_2,         //25
 1,//HAND_JOINT_FINGER1_3,         //26
 1//HAND_JOINT_ENDSITE_FINGER1_3  //27
};


void drawLineIfPointsExist2(cv::Mat &outputMat,cv::Point start,cv::Point finish,cv::Scalar color,float thickness)
{
    if ( ( (start.x!=0) || (start.y!=0) ) && ( (finish.x!=0) || (finish.y!=0) ) )
        {
           cv::line(outputMat,start,finish,color,thickness); 
        }
   return;
}

void drawCircleIfPointsExist2(cv::Mat &outputMat,cv::Point point,float radious,cv::Scalar color,float thickness)
{
    if  ( (point.x!=0) || (point.y!=0) )
        {
           cv::circle(outputMat,point,radious,color,-thickness,8,0);
           
           if (color[0]<230 ) { color[0]+=25; }
           if (color[1]<230 ) { color[1]+=25; }
           if (color[2]<230 ) { color[2]+=25; }
           
           cv::circle(outputMat,point,radious/2,color,-thickness,8,0);
        }
   return;
}


void drawHand(const char * windowName,struct BVH_MotionCapture * bvhMotion,std::vector<std::vector<float> > points2D,const std::vector<float> bvhFrame,cv::Mat * retreiveImage)
{
    if (bvhMotion==0)
    {
      fprintf(stderr,"draw Hand called without a valid loaded BVH file..\n");
      return;
    }
    
   unsigned int fullImageWidth = 1920;
   unsigned int fullImageHeight = 1080;
   cv::Mat image(fullImageHeight,fullImageWidth,CV_8UC3,cv::Scalar(255,255,255));
  
   float thickness=1.7;
   int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
   cv::Scalar fontColor= cv::Scalar(123,123,123);
   char label[129]={0};
   
   //Do Conversions..
   //std::vector<std::vector<float> > points2D = convertBVHFrameTo2DPoints(bvhFrame);
   
   
   float lineThickness=6.7;
   char lineR = 20;
   char lineG = 25;
   char lineB = 6;
   
   //Draw palms
   //------------------------------------------------------------------------------------
   //Connect Hand with metacarpals 
   cv::Point previousPoint   = cv::Point(points2D[HAND_JOINT_HAND][0],points2D[HAND_JOINT_HAND][1]);
   cv::Point pointToDraw     = cv::Point(points2D[HAND_JOINT_METACARPAL1][0],points2D[HAND_JOINT_METACARPAL1][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   pointToDraw   = cv::Point(points2D[HAND_JOINT_METACARPAL2][0],points2D[HAND_JOINT_METACARPAL2][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   pointToDraw   = cv::Point(points2D[HAND_JOINT_METACARPAL3][0],points2D[HAND_JOINT_METACARPAL3][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   pointToDraw   = cv::Point(points2D[HAND_JOINT_METACARPAL4][0],points2D[HAND_JOINT_METACARPAL4][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   
   //Connect Fingers with metacarpals 
   previousPoint   = cv::Point(points2D[HAND_JOINT_METACARPAL1][0],points2D[HAND_JOINT_METACARPAL1][1]);
   pointToDraw     = cv::Point(points2D[HAND_JOINT_FINGER2_1][0],points2D[HAND_JOINT_FINGER2_1][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
    
   previousPoint   = cv::Point(points2D[HAND_JOINT_METACARPAL2][0],points2D[HAND_JOINT_METACARPAL2][1]);
   pointToDraw     = cv::Point(points2D[HAND_JOINT_FINGER3_1][0],points2D[HAND_JOINT_FINGER3_1][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   previousPoint   = cv::Point(points2D[HAND_JOINT_METACARPAL3][0],points2D[HAND_JOINT_METACARPAL3][1]);
   pointToDraw     = cv::Point(points2D[HAND_JOINT_FINGER4_1][0],points2D[HAND_JOINT_FINGER4_1][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   previousPoint   = cv::Point(points2D[HAND_JOINT_METACARPAL4][0],points2D[HAND_JOINT_METACARPAL4][1]);
   pointToDraw     = cv::Point(points2D[HAND_JOINT_FINGER5_1][0],points2D[HAND_JOINT_FINGER5_1][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   
   
   //Connect metacarpals 
   previousPoint   = cv::Point(points2D[HAND_JOINT_FINGER2_1][0],points2D[HAND_JOINT_FINGER2_1][1]);
   pointToDraw     = cv::Point(points2D[HAND_JOINT_FINGER3_1][0],points2D[HAND_JOINT_FINGER3_1][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
    
   previousPoint   = cv::Point(points2D[HAND_JOINT_FINGER4_1][0],points2D[HAND_JOINT_FINGER4_1][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   pointToDraw     = cv::Point(points2D[HAND_JOINT_FINGER5_1][0],points2D[HAND_JOINT_FINGER5_1][1]); 
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);   
   
   //Connect fingers 
   previousPoint   = cv::Point(points2D[HAND_JOINT_FINGER2_1][0],points2D[HAND_JOINT_FINGER2_1][1]);
   pointToDraw     = cv::Point(points2D[HAND_JOINT_THUMB][0],points2D[HAND_JOINT_THUMB][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   previousPoint   = cv::Point(points2D[HAND_JOINT_METACARPAL1][0],points2D[HAND_JOINT_METACARPAL1][1]);
   pointToDraw     = cv::Point(points2D[HAND_JOINT_METACARPAL2][0],points2D[HAND_JOINT_METACARPAL2][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
    
   previousPoint   = cv::Point(points2D[HAND_JOINT_METACARPAL3][0],points2D[HAND_JOINT_METACARPAL3][1]);
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   
   pointToDraw     = cv::Point(points2D[HAND_JOINT_METACARPAL4][0],points2D[HAND_JOINT_METACARPAL4][1]); 
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);   
   
   //Thumb has more joints
   previousPoint = cv::Point(points2D[HAND_JOINT_HAND][0],points2D[HAND_JOINT_HAND][1]);
   pointToDraw   = cv::Point(points2D[HAND_JOINT__THUMB][0],points2D[HAND_JOINT__THUMB][1]); 
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);   
   
   //Thumb
   previousPoint = cv::Point(points2D[HAND_JOINT__THUMB][0],points2D[HAND_JOINT__THUMB][1]);
   pointToDraw   = cv::Point(points2D[HAND_JOINT_THUMB][0],points2D[HAND_JOINT_THUMB][1]); 
   drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);  
   //------------------------------------------------------------------------------------
   
  for (unsigned int pointID=0; pointID<points2D.size(); pointID++)
  {

    pointToDraw=cv::Point(points2D[pointID][0],points2D[pointID][1]);

    if (pointID>1)
    { 
      if ( handGroup[pointID] == handGroup[pointID-1] )
      {
        previousPoint=cv::Point(points2D[pointID-1][0],points2D[pointID-1][1]);
        drawLineIfPointsExist2(image,previousPoint,pointToDraw,cv::Scalar(lineB,lineG,lineR),lineThickness);   
      } 
    }
    
    cv::Scalar sphereColor = cv::Scalar(lineB,lineG,lineR); 
    switch (handGroup[pointID])
    {
      case 1:
        sphereColor[0]=150;   sphereColor[1]=150; sphereColor[2]=150;
      break;
      case 2:
        sphereColor[0]=0;     sphereColor[1]=230; sphereColor[2]=230;
      break;
      case 3:
        sphereColor[0]=230;   sphereColor[1]=0;   sphereColor[2]=0;
      break;
      case 4:
        sphereColor[0]=0;     sphereColor[1]=0;   sphereColor[2]=230;
      break;
      case 5:
        sphereColor[0]=0;     sphereColor[1]=230; sphereColor[2]=0;
      break;
    }
     
    drawCircleIfPointsExist2(image,pointToDraw,8,sphereColor,1.0);

    //snprintf(label,128,"%u(%s)",pointID,bvhMotion->jointHierarchy[pointID].jointName); //2D->3D 
    snprintf(label,128,"%u",pointID); //2D->3D 
    cv::Point  txtPosition(points2D[pointID][0]+15,points2D[pointID][1]);
    //cv::putText(image,label,txtPosition,fontUsed,0.3,fontColor,thickness,8);

  }
  
  
  if (retreiveImage!=0)
  {
   //This code block will make a rectangular selection of the hand
   //for saving it as a snapshot..
   //--------------------------------------------------------------
   cv::Point minimum = cv::Point(points2D[0][0],points2D[0][1]);
   cv::Point maximum = cv::Point(points2D[0][0],points2D[0][1]);
   unsigned int width;
   unsigned int height;
   
  for (unsigned int pointID=0; pointID<points2D.size(); pointID++)
   {
       if (points2D[pointID][0]<minimum.x) { minimum.x=points2D[pointID][0]; }
       if (points2D[pointID][1]<minimum.y) { minimum.y=points2D[pointID][1]; }
       if (points2D[pointID][0]>maximum.x) { maximum.x=points2D[pointID][0]; }
       if (points2D[pointID][1]>maximum.y) { maximum.y=points2D[pointID][1]; } 
   }
   
   minimum.x-=10;
   minimum.y-=10;
   maximum.x+=10;
   maximum.y+=10;
   
   width  = maximum.x-minimum.x;
   height = maximum.y-minimum.y;
   
   if (width>height)
   {
      unsigned int offset=(width-height)/2;
      height=width;
      minimum.y-=offset;
      maximum.y=minimum.y+height;
   } else
   if (width<height)
   {
      unsigned int offset=(height-width)/2;
      width=height;
      minimum.x-=offset;
      maximum.x=minimum.x+width;
   }
   //--------------------------------------------------------------
  
  cv::Rect2i r;// = selectROI(im); 16
  
  r.x = minimum.x;
  r.y = minimum.y;
  r.width = width;
  r.height = height;
 
  if ( 
       (r.width==0) ||
       (r.height==0) ||
       (r.x > fullImageWidth) ||
       (r.y > fullImageHeight) ||
       (r.width > fullImageWidth) ||
       (r.height > fullImageHeight) || 
       (r.x+r.width > fullImageWidth) ||
       (r.y+r.height > fullImageHeight)  
     )
     {
       r.x=0;
       r.y=0;
       r.width=fullImageWidth;
       r.height=fullImageHeight;
       fprintf(stderr,"Failed ROI\n"); 
     }
  fprintf(stderr,"ROI (x=%u,y=%u | width=%u,height=%u)\n",r.x,r.y,r.width,r.height);
   
  cv::Point  txtPosition(20,20);
  snprintf(label,128,"Root position X/Y/Z  %0.2f/%0.2f/%0.2f",bvhFrame[0],bvhFrame[1],bvhFrame[2]);
  cv::putText(image,label,txtPosition,fontUsed,0.3,fontColor,thickness,8);
  
  *retreiveImage = image(r);
  //cv::imshow("standalone",*retreiveImage); 
  
  //cv::Mat imCrop = image(r);
  //cv::imshow("standalone",imCrop); 
   
  }
  
  cv::imshow(windowName,image); 
  
}


void draw3DLhand(const std::vector<float> bvhFrame)
{
  struct BVH_MotionCapture * bvhLHandMotion = (struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle(); 
   std::vector<std::vector<float> > points2D = convertStandaloneLHandBVHFrameTo2DPoints(bvhFrame); 
   drawHand("3D Left Hand",bvhLHandMotion,points2D,bvhFrame,0);
}

void draw3DRhand(const std::vector<float> bvhFrame)
{
  struct BVH_MotionCapture * bvhRHandMotion = (struct BVH_MotionCapture *) getBVHRHandQBVHMotionHandle();
   std::vector<std::vector<float> > points2D = convertStandaloneRHandBVHFrameTo2DPoints(bvhFrame); 
  drawHand("3D Right Hand",bvhRHandMotion,points2D,bvhFrame,0);
}

void draw2DHand(const std::vector<float> points2D)
{
  cv::Mat image(1080,1920,CV_8UC3,cv::Scalar(0,0,0));
  
   float thickness=1.7;
   int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
   cv::Scalar fontColor= cv::Scalar(255,255,255);
   char label[129]={0};
 
  for (unsigned int pointID=0; pointID<points2D.size()/3; pointID++)
  {
    float x = image.size().width  * points2D[pointID*3+0];
    float y = image.size().height * points2D[pointID*3+1];
    cv::Point pointToDraw(x,y);
    drawCircleIfPointsExist2(image,pointToDraw,5,cv::Scalar(255,0,255),1.0);
    
    snprintf(label,128,"%u(%s=%0.2f,%s=%0.2f)",pointID,
                       mocapNET_lhand[pointID*3+0],x,
                       mocapNET_lhand[pointID*3+1],y
                       //,mocapNET_lhand[pointID*3+2],points2D[pointID*3+2]
            ); //2D->3D 
    cv::Point  txtPosition(x+20,y);
    cv::putText(image,label,txtPosition,fontUsed,0.3,fontColor,thickness,8);
  }
  cv::imshow("Visualization 2D",image);
    
}



void draw2DHandSk(struct skeletonSerialized * skeleton)
{
  cv::Mat image(1080,1920,CV_8UC3,cv::Scalar(0,0,0));
  
   float thickness=1.0;
   int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
   cv::Scalar fontColor= cv::Scalar(255,255,255);
   char label[129]={0};
 
  for (unsigned int pointID=0; pointID<skeleton->skeletonHeaderElements/3; pointID++)
  {
    float x = image.size().width  * skeleton->skeletonBody[pointID*3+0].value;
    float y = image.size().height * skeleton->skeletonBody[pointID*3+1].value;
    cv::Point pointToDraw(x,y);
    drawCircleIfPointsExist2(image,pointToDraw,5,cv::Scalar(255,0,255),1.0);
    
    snprintf(label,128,"%u(%s=%0.2f,%s=%0.2f)",pointID,
    skeleton->skeletonHeader[pointID*3+0].str,x,
    skeleton->skeletonHeader[pointID*3+1].str,y
    //,mocapNET_lhand[pointID*3+2],points2D[pointID*3+2]
    ); //2D->3D 
    cv::Point  txtPosition(x+20,y);
    cv::putText(image,label,txtPosition,fontUsed,0.2,fontColor,thickness,8);
  }
  cv::imshow("Visualization 2D",image);
    
}
