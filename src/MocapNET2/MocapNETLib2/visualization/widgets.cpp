#include "widgets.hpp"

#if USE_OPENCV

#include "opencv2/opencv.hpp"
using namespace cv;

#include "../IO/bvh.hpp"
#include "../tools.hpp"
#include "../mocapnet2.hpp"


cv::Mat overlay(cv::Mat base,cv::Mat overlay)
{
  //--------------------------------------
  if ( (base.size().width==overlay.size().width) && (base.size().height==overlay.size().height) )
  {
    if (  ( base.channels()==3 ) && ( base.channels()==overlay.channels() ) )
    { 
      for (unsigned int y=0; y<base.size().height; y++)
      { 
       unsigned char * baseP = base.ptr<uchar>(y);
       unsigned char * overlayP = overlay.ptr<uchar>(y);
       
       for (unsigned int x=0; x<base.size().width; x++)
       {
        unsigned char bO = *overlayP; ++overlayP;
        unsigned char gO = *overlayP; ++overlayP;
        unsigned char rO = *overlayP; ++overlayP; 
        
        if ( (rO!=0) || (gO!=0) || (bO!=0) )
        {
            *baseP = bO; ++baseP;
            *baseP = gO; ++baseP;
            *baseP = rO; ++baseP;
        } else
        {
           baseP+=3;
        }
      }
     }
    return base;
   }
  }
     
  return cv::max(overlay,base);
}



int drawScale(cv::Mat &outputMat,const char * description,float x,float y,float value,float minimum,float maximum)
{  
  cv::Point startPoint(x,y);
  cv::Point endPoint(x,y+200); 


  cv::Scalar color = cv::Scalar(123,123,123);
  cv::line(outputMat,startPoint,endPoint, color , 3.0);  
  //
  
  float fraction = (float) value/(maximum-minimum);
  
  float yPos = y + 200 * fraction; 
  
  //Don't let it get out of range..
  if (yPos<y) { yPos=y; } 
  if (yPos>y+200) { yPos=y+200; } 

  cv::Point distancePos(x,yPos); 
  cv::circle(outputMat,distancePos,1,cv::Scalar(0,255,0),3,8,0);
  
  
  char label[64];

  snprintf(label,64,"%s",description);
  startPoint.x=startPoint.x-20;
  startPoint.y=startPoint.y-10;
  cv::putText(outputMat,label, startPoint, cv::FONT_HERSHEY_DUPLEX, 0.3, cv::Scalar::all(255), 0.2, 8 );

  if (value<0)       { snprintf(label,64,"Negative!"); } else 
  if (value>maximum) { snprintf(label,64,"Too Large!"); } else 
                     { snprintf(label,64,"%0.2f",value); }
  distancePos.x=distancePos.x-20;
  distancePos.y=distancePos.y-10;
  cv::putText(outputMat,label, distancePos, cv::FONT_HERSHEY_DUPLEX, 0.3, cv::Scalar::all(255), 0.2, 8 );
  
 return 1;        
}


int plotFloatVector(
                     cv::Mat & img,
                     char cleanBackground,
                     std::vector<float> history,
                     unsigned int x,
                     unsigned int y,
                     unsigned int width,
                     unsigned int height
                   )
{
  #if USE_OPENCV
    if (history.size()<2) { return 0; }

 
    unsigned int plotPosX=x;
    unsigned int plotPosY=y;
    unsigned int widthOfGraphs=width;
    unsigned int heightOfGraphs=height;
    unsigned int i=0,joint=0;
       
    if (cleanBackground)
      { cv::rectangle(img,cv::Rect(x,y,width,height),cv::Scalar(0,0,0),-1); }
    
    float resolution = 2.0;
    float speed=2.0;
    unsigned int plotStart = 1; 
    unsigned int plotEnd   = history.size(); 
    
    if (speed!=1.0)
    {
       plotStart = (unsigned int ) history.size() / speed;
    }
    
    for (i=plotStart; i<plotEnd; i++)
    {
        cv::Point jointPointPrev(plotPosX+ ((unsigned int) (i-plotStart) * speed),    plotPosY+heightOfGraphs-(unsigned int) history[i-1]/resolution);
        cv::Point jointPointNext(plotPosX+ ((unsigned int) (i-plotStart+1) * speed),  plotPosY+heightOfGraphs-(unsigned int) history[i]/resolution  );
        
        
        cv::Scalar usedColor = cv::Scalar(0,0,255);
        float lastRecordedFramerate = history.back();
        if (lastRecordedFramerate >= 60) { usedColor = cv::Scalar(0,255,0);   } else
        if (lastRecordedFramerate >30) { usedColor = cv::Scalar(0,255,255); }

    
        cv::line(img,jointPointPrev,jointPointNext,usedColor,2.0);
            
    } 
    return 1;
 #else
   fprintf(stderr,"plotFloatVector cannot be compiled without OpenCV\n");
   return 0;
#endif   
}


int visualizeOrientation(
    cv::Mat &img,
    const char * label,
    float orientationDegrees,
    float frontClass,
    float backClass,
    float leftClass,
    float rightClass,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
)
{
    float thickness=2.5;
    cv::Point startPoint(x,y);
    cv::Point endPoint(x+width,y+height);

    int addSyntheticPoints=1;
    int doScaleCompensation=0;

    thickness=1;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
    cv::Scalar fontColor= cv::Scalar(255,255,255);
    cv::Point txtPosition(x,y-15);
    cv::putText(img,label,txtPosition,fontUsed,0.6,fontColor,thickness,8);
    txtPosition.y+=10;


    cv::Scalar color= cv::Scalar(0,0,255);


    unsigned int cX = x + width / 2;
    unsigned int cY = y + height / 2;

    startPoint.x=cX;
    startPoint.y=cY;

    cv::Scalar color0 = cv::Scalar(0,0,255);
    cv::Scalar color45 = cv::Scalar(0,0,255);
    cv::Scalar color135 = cv::Scalar(0,0,255);
    cv::Scalar color225 = cv::Scalar(0,0,255);
    cv::Scalar color315 = cv::Scalar(0,0,255);

    cv::Scalar font0 = cv::Scalar(255,255,0);
    cv::Scalar font45 = cv::Scalar(255,255,0);
    cv::Scalar font135 = cv::Scalar(255,255,0);
    cv::Scalar font225 = cv::Scalar(255,255,0);
    cv::Scalar font315 = cv::Scalar(255,255,0);


    if (  (frontClass>backClass) && (frontClass>leftClass) && (frontClass>rightClass) )
        {
            color45=cv::Scalar(0,255,0);
            font45=cv::Scalar(255,0,255);
        }
    else if (  (backClass>frontClass) && (backClass>leftClass) && (backClass>rightClass) )
        {
            color225=cv::Scalar(0,255,0);
            font225=cv::Scalar(255,0,255);
        }
    else if (  (leftClass>backClass) && (leftClass>frontClass) && (leftClass>rightClass) )
        {
            color135=cv::Scalar(0,255,0);
            font135=cv::Scalar(255,0,255);
        }
    else if (  (rightClass>backClass) && (rightClass>frontClass) && (rightClass>leftClass) )
        {
            color0=cv::Scalar(0,255,0);
            color315=color0;
            font0=cv::Scalar(255,0,255);
            font315=cv::Scalar(255,0,255);
        }

    for (unsigned int i=0; i<360; i++)
        {
            float rad=((float) 3.1415/180*i);
            endPoint.x=cX+cos(rad)* (width/2);
            endPoint.y=cY+sin(rad)* (height/2);

            if (i==0)
                {
                    color=color0;
                }
            else if (i==45)
                {
                    color=color45;
                }
            else if (i==135)
                {
                    color=color135;
                }
            else if (i==225)
                {
                    color=color225;
                }
            else if (i==315)
                {
                    color=color315;
                }

            cv::line(
                     img,
                     startPoint,
                     endPoint,
                     color,
                     2.0
                    );


        }
        
        
        
    
    //Draw an arrow in our orientation..
    unsigned int rad = height/2;

    int lineStyle = 0; // https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/30 has problems with CV_AA
    
    cv::Point arrowPointing;
    arrowPointing.x =  (int)(startPoint.x + (rad+10) * cos((90.0+orientationDegrees) * CV_PI / 180.0));
    arrowPointing.y =  (int)(startPoint.y + (rad+10) * sin((90.0+orientationDegrees) * CV_PI / 180.0));
    cv::line(img,startPoint,arrowPointing,cv::Scalar(0,255,255),4,lineStyle,0);
    cv::Point arrowSide;
    arrowSide.x =  (int)(startPoint.x + (rad) * cos((90.0+orientationDegrees-5) * CV_PI / 180.0));
    arrowSide.y =  (int)(startPoint.y + (rad) * sin((90.0+orientationDegrees-5) * CV_PI / 180.0));
    cv::line(img,arrowSide,arrowPointing,cv::Scalar(0,255,255),4,lineStyle,0);
    arrowSide.x =  (int)(startPoint.x + (rad) * cos((90.0+orientationDegrees+5) * CV_PI / 180.0));
    arrowSide.y =  (int)(startPoint.y + (rad) * sin((90.0+orientationDegrees+5) * CV_PI / 180.0));
    cv::line(img,arrowSide,arrowPointing,cv::Scalar(0,255,255),4,lineStyle,0);

    thickness=2;
    startPoint.x-= width / 7;

    char str[512];
    snprintf(str,512,"%0.2f",frontClass);
    txtPosition = startPoint;
    txtPosition.y += 5+height/4;
    cv::putText(img,str,txtPosition,fontUsed,0.6,font45,thickness,8);

    snprintf(str,512,"%0.2f",backClass);
    txtPosition = startPoint;
    txtPosition.y -= height/4;
    cv::putText(img,str,txtPosition,fontUsed,0.6,font225,thickness,8);

    snprintf(str,512,"%0.2f",rightClass);
    txtPosition = startPoint;
    txtPosition.x += width/4;
    cv::putText(img,str,txtPosition,fontUsed,0.6,font0,thickness,8);

    snprintf(str,512,"%0.2f",leftClass);
    txtPosition = startPoint;
    txtPosition.x -= width/4;
    cv::putText(img,str,txtPosition,fontUsed,0.6,font135,thickness,8);

    return 1;
}




int visualizeNSDM(
    cv::Mat &img,
    const char * label,
    std::vector<float> NSDM,
    unsigned int channelsPerNSDMElement,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
)
{
    float thickness=2.5;
    //cv::Point topLeft(x,y);
    //cv::Point bottomRight(x+width,y+height);
    //cv::line(img,topLeft,bottomRight, cv::Scalar(0,255,0),thickness);
    //cv::rectangle(img, topLeft,bottomRight, cv::Scalar(0,255,0),thickness, 8, 0);

    int addSyntheticPoints=1;
    int doScaleCompensation=0;

    thickness=1.5;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
    cv::Scalar color= cv::Scalar(255,255,255,0 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);
    cv::Point txtPosition(x,y-15);
    cv::putText(img,label,txtPosition,fontUsed,0.8,color,thickness,8);
    txtPosition.y+=10;

    if (channelsPerNSDMElement==2)
        {
            cv::putText(img,"NSDM",txtPosition,fontUsed,0.3,color,thickness,8);
        }
    else if (channelsPerNSDMElement==1)
        {
            cv::putText(img,"NSRM",txtPosition,fontUsed,0.3,color,thickness,8);
        }


    if (NSDM.size()==0)
        {
            txtPosition.x+=(width/2)-30;
            txtPosition.y+=height/2;
            cv::putText(img,"N/A",txtPosition,fontUsed,0.8,cv::Scalar(0,0,255),thickness,8);
            return 0;
        }


    if (NSDM.size()>0)
        {
            unsigned int xI,yI,item=0,dim=sqrt(NSDM.size()/channelsPerNSDMElement);
            unsigned int boxX=width/dim,boxY=height/dim;
            for (yI=0; yI<dim; yI++)
                {
                    for (xI=0; xI<dim; xI++)
                        {
                            cv::Point topLeft(x+xI*boxX,y+yI*boxY);
                            cv::Point bottomRight(x+xI*boxX+boxX,y+yI*boxY+boxY);

                            float xVal=1.0;
                            float yVal=1.0;

                            float blueChannel = 0;
                            float greenChannel= 0;
                            float redChannel  = 0;

                            if (channelsPerNSDMElement==2)
                                {
                                    xVal=NSDM[item];
                                    yVal=NSDM[item+1];

                                    blueChannel = (float) xVal*255.0;
                                    greenChannel= (float) yVal*255.0;
                                    redChannel  = (float) 255.0 * ( (xVal==0.0) && (yVal==0.0) );
                                }
                            else if (channelsPerNSDMElement==1)
                                {
                                    yVal=0;
                                    xVal=0;
                                    if (NSDM[item]<0)
                                        {
                                            yVal=abs(NSDM[item]);
                                        }
                                    else
                                        {
                                            xVal=NSDM[item];
                                        }

                                    //fprintf(stderr,"Item %u => %0.2f\n",item,NSDM[item]);
                                    greenChannel = (float) 255.0 * (float) xVal/2;// /180.0;
                                    blueChannel = (float) 255.0 * (float) yVal/2;// /180.0;
                                    redChannel  = (float) 255.0 * ( (xVal==0.0) && (yVal==0.0) );
                                }


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
                            if (  (  (xVal>2.0) || (yVal>2.0)  ) && (channelsPerNSDMElement==2) )
                                {
                                    cv::Point topRight(x+xI*boxX+boxX,y+yI*boxY);
                                    cv::Point bottomLeft(x+xI*boxX,y+yI*boxY+boxY);
                                    cv::Point topMiddle(x+xI*boxX+boxX/2,y+yI*boxY);
                                    cv::Point bottomMiddle(x+xI*boxX+boxX/2,y+yI*boxY+boxY);
                                    cv::line(img,topLeft,bottomRight, cv::Scalar(0,0,255), 1.0);
                                    cv::line(img,topRight,bottomLeft, cv::Scalar(0,0,255), 1.0);
                                    cv::line(img,topMiddle,bottomMiddle, cv::Scalar(0,0,255), 1.0);
                                }
                            else if (  (  (xVal>1.5) || (yVal>1.5)  ) && (channelsPerNSDMElement==2) )
                                {
                                    cv::Point topRight(x+xI*boxX+boxX,y+yI*boxY);
                                    cv::Point bottomLeft(x+xI*boxX,y+yI*boxY+boxY);
                                    cv::line(img,topLeft,bottomRight, cv::Scalar(0,0,255), 1.0);
                                    cv::line(img,topRight,bottomLeft, cv::Scalar(0,0,255), 1.0);
                                }
                            else if  (  (  (xVal>1.0) || (yVal>1.0)  ) && (channelsPerNSDMElement==2) )
                                {
                                    cv::Point topRight(x+xI*boxX+boxX,y+yI*boxY);
                                    cv::Point bottomLeft(x+xI*boxX,y+yI*boxY+boxY);
                                    cv::line(img,topLeft,bottomRight, cv::Scalar(0,0,255), 1.0);
                                }

                            item+=channelsPerNSDMElement;
                        }
                }
            return 1;
        }
    return 0;
}



int visualizeNSDMAsBar(
    cv::Mat &img,
    std::vector<float> NSDM,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
)
{
    float thickness=2.5;
    //cv::Point topLeft(x,y);
    //cv::Point bottomRight(x+width,y+height);
    //cv::line(img,topLeft,bottomRight, cv::Scalar(0,255,0),thickness);
    //cv::rectangle(img, topLeft,bottomRight, cv::Scalar(0,255,0),thickness, 8, 0);

    int addSyntheticPoints=1;
    int doScaleCompensation=0;

    if (NSDM.size()>0)
        {

            float thickness=1;
            int fontUsed=cv::FONT_HERSHEY_SIMPLEX;


            unsigned int xI,yI,item=0,dim=sqrt(NSDM.size()/2);
            unsigned int errors=0;

            for (yI=0; yI<dim; yI++)
                {
                    for (xI=0; xI<dim; xI++)
                        {
                            if ( (NSDM[item]==0.0) && (NSDM[item+1]==0.0) )
                                {
                                    ++errors;
                                }
                            item+=2;
                        }
                }


            cv::Scalar color;
            float qualityPercentage=(float) errors/(dim*dim*2);
            qualityPercentage=(1.0-qualityPercentage);

            if (qualityPercentage>0.9)
                {
                    color=cv::Scalar(0,255,0);
                }
            else if (qualityPercentage>0.8)
                {
                    color=cv::Scalar(0,123,123);
                    qualityPercentage-=0.2;
                }
            else
                {
                    color=cv::Scalar(0,0,255);
                    qualityPercentage-=0.6;
                }

            if (qualityPercentage>1.0)
                {
                    qualityPercentage=1.0;
                }
            if (qualityPercentage<0.0)
                {
                    qualityPercentage=0.0;
                }

            cv::Point topLeft(x,y);
            cv::Point bottomRight(x+(qualityPercentage*width),y+height);


            cv::rectangle(img, topLeft,bottomRight,color,-thickness, 8, 0);

            cv::Point txtPosition(x,y-15);
            cv::putText(img,"Quality : ",txtPosition,fontUsed,0.8,color,thickness,8);
            return 1;
        }
    return 0;
}




int drawFloorFromPrimitives(
    cv::Mat &img,
    float roll,
    float pitch,
    float yaw,
    unsigned int floorDimension
)
{
    std::vector<std::vector<float> > gridPoints2D = convert3DGridTo2DPoints(
                roll,
                pitch,
                yaw, 
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
    return 1;
}

#endif
