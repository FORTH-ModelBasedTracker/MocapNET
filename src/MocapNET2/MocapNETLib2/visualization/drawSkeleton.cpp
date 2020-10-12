#include "drawSkeleton.hpp"


#if USE_OPENCV
#if USE_BVH
#include "../IO/bvh.hpp"

#define GOOFY_EYES 0

     
void drawCircleIfPointsExist(cv::Mat &outputMat,cv::Point point,float radious,cv::Scalar color,float thickness)
{
    if  ( (point.x!=0) || (point.y!=0) )
        {
           cv::circle(outputMat,point,radious,color,thickness,8,0);
        }
   return;
}


void drawLineIfPointsExist(cv::Mat &outputMat,cv::Point start,cv::Point finish,cv::Scalar color,float thickness)
{
    if ( ( (start.x!=0) || (start.y!=0) ) && ( (finish.x!=0) || (finish.y!=0) ) )
        {
           cv::line(outputMat,start,finish,color,thickness); 
        }
   return;
}

int visualizeSkeletonSerialized(
                                cv::Mat &outputMat,
                                struct skeletonSerialized * skeleton, 
                                unsigned int drawLeftFingers,
                                unsigned int drawRightFingers, 
                                unsigned int drawFace,
                                unsigned int x,unsigned int y,
                                unsigned int width,unsigned int height
                               )
{
    /*
    fprintf(stderr,"Visualize %u,%u  -   Skeleton ",skeletonWidth,skeletonHeight);
    fprintf(stderr,"Offset %u,%u  -   ",x,y);
    fprintf(stderr,"Joints %u   -   ",skeleton->skeletonBodyElements);
    fprintf(stderr,"Dimensions  %u,%u  \n ",width,height);
    char filename[512];
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

    float thickness=1.0;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;

    unsigned int pointsThatExist=0;
    unsigned int pointsThatSeemToBeWronglyNormalized=0;
    
    
    //Uncomment for a string to jID match
    //for (int jointID=3; jointID<skeleton->skeletonBodyElements; jointID++)
    //    {
    //        fprintf(stderr,"skeleton[%u]=%s\n",jointID,skeleton->skeletonHeader[jointID].str);
    //    }

    cv::Scalar faceColor(255,255,0);
    cv::Scalar rightColor(0,255,0);
    cv::Scalar leftColor(0,0,255);

    float faceThickness=2.0;
    float fingerThickness=2.0;
    float bodyThickness=2.0;
    
    
    for (int jointID=3; jointID<skeleton->skeletonBodyElements/3; jointID++)
        {
            unsigned int jID =  jointID*3;

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


    float xRatio = (float) width  / skeleton->width;
    float yRatio = (float) height / skeleton->height;

    unsigned int jID; 
    
    
    jID=16 * 3;
    cv::Point reye(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=17 * 3;
    cv::Point leye(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=18 * 3;
    cv::Point rear(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=19 * 3;
    cv::Point lear(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);


    jID=1 * 3;
    cv::Point head(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=2 * 3;
    cv::Point neck(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);

    jID=9 * 3;
    cv::Point hip(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);

    jID=3 * 3;
    cv::Point rshoulder(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=4 * 3;
    cv::Point relbow(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=5 * 3;
    cv::Point rhand(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);

    jID=6 * 3;
    cv::Point lshoulder(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=7 * 3;
    cv::Point lelbow(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=8 * 3;
    cv::Point lhand(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);

    jID=10 * 3;
    cv::Point rhip(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=11 * 3;
    cv::Point rknee(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=12 * 3;
    cv::Point rfoot(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=25 * 3;
    cv::Point rheel(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=23 * 3;
    cv::Point rbigtoe(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=24 * 3;
    cv::Point rsmalltoe(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);

    jID=13 * 3;
    cv::Point lhip(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=14 * 3;
    cv::Point lknee(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=15 * 3;
    cv::Point lfoot(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=22 * 3;
    cv::Point lheel(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=20 * 3;
    cv::Point lbigtoe(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=21 * 3;
    cv::Point lsmalltoe(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);

  
    drawLineIfPointsExist(*overlayP,lshoulder,lelbow,leftColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,lhand,lelbow,leftColor,bodyThickness);

    drawLineIfPointsExist(*overlayP,rshoulder,relbow,rightColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,rhand,relbow,rightColor,bodyThickness);


    drawLineIfPointsExist(*overlayP,lhip,hip,leftColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,lhip,lknee,leftColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,lknee,lfoot,leftColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,lfoot,lheel,leftColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,lfoot,lbigtoe,leftColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,lfoot,lsmalltoe,leftColor,bodyThickness);

    drawLineIfPointsExist(*overlayP,rhip,hip,rightColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,rhip,rknee,rightColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,rknee,rfoot,rightColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,rfoot,rheel,rightColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,rfoot,rbigtoe,rightColor,bodyThickness);
    drawLineIfPointsExist(*overlayP,rfoot,rsmalltoe,rightColor,bodyThickness);


    drawLineIfPointsExist(*overlayP,lshoulder,rshoulder,cv::Scalar(255,0,0),bodyThickness);
    drawLineIfPointsExist(*overlayP,hip,rshoulder,cv::Scalar(255,0,0),bodyThickness);
    drawLineIfPointsExist(*overlayP,hip,lshoulder,cv::Scalar(255,0,0),bodyThickness);
    drawLineIfPointsExist(*overlayP,neck,rshoulder,cv::Scalar(255,0,0),bodyThickness);
    drawLineIfPointsExist(*overlayP,neck,lshoulder,cv::Scalar(255,0,0),bodyThickness);
    
    //Also draw joints
    drawCircleIfPointsExist(*overlayP,lhand,5,cv::Scalar(255,0,255),bodyThickness);
    drawCircleIfPointsExist(*overlayP,rhand,5,cv::Scalar(255,0,255),bodyThickness);
    drawCircleIfPointsExist(*overlayP,lshoulder,5,cv::Scalar(255,0,255),bodyThickness);
    drawCircleIfPointsExist(*overlayP,rshoulder,5,cv::Scalar(255,0,255),bodyThickness);
    drawCircleIfPointsExist(*overlayP,lelbow,5,cv::Scalar(255,0,255),bodyThickness);
    drawCircleIfPointsExist(*overlayP,relbow,5,cv::Scalar(255,0,255),bodyThickness);
    drawCircleIfPointsExist(*overlayP,lhip,5,cv::Scalar(255,0,255),bodyThickness);
    drawCircleIfPointsExist(*overlayP,rhip,5,cv::Scalar(255,0,255),bodyThickness);
    drawCircleIfPointsExist(*overlayP,lknee,5,cv::Scalar(255,0,255),bodyThickness);
    drawCircleIfPointsExist(*overlayP,rknee,5,cv::Scalar(255,0,255),bodyThickness);
    
    
    if (drawLeftFingers)
    { 
    //Left Hand 
    //---------------------------------------------------------------------------------------------------------
    jID=28 * 3;
    cv::Point lthumb(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=29 * 3;
    cv::Point lthumb1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=30 * 3;
    cv::Point lthumb2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=31 * 3;
    cv::Point lthumbES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
    jID=32 * 3;
    cv::Point lpointer(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=33 * 3;
    cv::Point lpointer1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=34 * 3;
    cv::Point lpointer2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=35 * 3;
    cv::Point lpointerES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
    jID=36 * 3;
    cv::Point lmiddle(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=37 * 3;
    cv::Point lmiddle1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=38 * 3;
    cv::Point lmiddle2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=39 * 3;
    cv::Point lmiddleES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
    jID=40 * 3;
    cv::Point lring(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=41 * 3;
    cv::Point lring1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=42 * 3;
    cv::Point lring2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=43 * 3;
    cv::Point lringES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
    jID=44 * 3;
    cv::Point lpinky(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=45 * 3;
    cv::Point lpinky1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=46 * 3;
    cv::Point lpinky2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=47 * 3;
    cv::Point lpinkyES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
     

    drawLineIfPointsExist(*overlayP,lhand,lthumb,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lhand,lpointer,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lhand,lmiddle,leftColor,fingerThickness); 
    drawLineIfPointsExist(*overlayP,lhand,lring,leftColor,fingerThickness); 
    drawLineIfPointsExist(*overlayP,lhand,lpinky,leftColor,fingerThickness); 

    drawLineIfPointsExist(*overlayP,lthumb,lthumb1,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lthumb1,lthumb2,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lthumb2,lthumbES,leftColor,fingerThickness); 

    drawLineIfPointsExist(*overlayP,lpointer,lpointer1,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lpointer1,lpointer2,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lpointer2,lpointerES,leftColor,fingerThickness); 
    
    drawLineIfPointsExist(*overlayP,lmiddle,lmiddle1,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lmiddle1,lmiddle2,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lmiddle2,lmiddleES,leftColor,fingerThickness); 
    
    drawLineIfPointsExist(*overlayP,lring,lring1,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lring1,lring2,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lring2,lringES,leftColor,fingerThickness); 
    
    drawLineIfPointsExist(*overlayP,lpinky,lpinky1,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lpinky1,lpinky2,leftColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,lpinky2,lpinkyES,leftColor,fingerThickness); 
    }
    
    
    
    
    if (drawRightFingers)
    { 
    //Right Hand 
    //---------------------------------------------------------------------------------------------------------
    jID=49 * 3;
    cv::Point rthumb(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=50 * 3;
    cv::Point rthumb1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=51 * 3;
    cv::Point rthumb2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=52 * 3;
    cv::Point rthumbES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
    jID=53 * 3;
    cv::Point rpointer(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=54 * 3;
    cv::Point rpointer1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=55 * 3;
    cv::Point rpointer2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=56 * 3;
    cv::Point rpointerES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
    jID=57 * 3;
    cv::Point rmiddle(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=58 * 3;
    cv::Point rmiddle1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=59 * 3;
    cv::Point rmiddle2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=60 * 3;
    cv::Point rmiddleES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
    jID=61 * 3;
    cv::Point rring(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=62 * 3;
    cv::Point rring1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=63 * 3;
    cv::Point rring2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=64 * 3;
    cv::Point rringES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
    jID=65 * 3;
    cv::Point rpinky(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=66 * 3;
    cv::Point rpinky1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=67 * 3;
    cv::Point rpinky2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=68 * 3;
    cv::Point rpinkyES(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); //EndSite
    //--------------------------------------------------------------------------------------------------------- 
     

    drawLineIfPointsExist(*overlayP,rhand,rthumb,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rhand,rpointer,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rhand,rmiddle,rightColor,fingerThickness); 
    drawLineIfPointsExist(*overlayP,rhand,rring,rightColor,fingerThickness); 
    drawLineIfPointsExist(*overlayP,rhand,rpinky,rightColor,fingerThickness); 

    drawLineIfPointsExist(*overlayP,rthumb,rthumb1,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rthumb1,rthumb2,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rthumb2,rthumbES,rightColor,fingerThickness); 

    drawLineIfPointsExist(*overlayP,rpointer,rpointer1,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rpointer1,rpointer2,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rpointer2,rpointerES,rightColor,fingerThickness); 
    
    drawLineIfPointsExist(*overlayP,rmiddle,rmiddle1,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rmiddle1,rmiddle2,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rmiddle2,rmiddleES,rightColor,fingerThickness); 
    
    drawLineIfPointsExist(*overlayP,rring,rring1,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rring1,rring2,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rring2,rringES,rightColor,fingerThickness); 
    
    drawLineIfPointsExist(*overlayP,rpinky,rpinky1,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rpinky1,rpinky2,rightColor,fingerThickness);
    drawLineIfPointsExist(*overlayP,rpinky2,rpinkyES,rightColor,fingerThickness); 
    }
    
    
    if (drawFace)
    {
    //Face  
    //---------------------------------------------------------------------------------------------------------
    jID=69 * 3;
    cv::Point rchin0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=70 * 3;
    cv::Point rchin1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=71 * 3;
    cv::Point rchin2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=72 * 3;
    cv::Point rchin3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=73 * 3;
    cv::Point rchin4(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); 
    jID=74 * 3;
    cv::Point rchin5(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); 
    jID=75 * 3;
    cv::Point rchin6(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); 
    jID=76 * 3;
    cv::Point rchin7(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); 
    //--------------------------------------------------------------------------------------------------------- 
    
    
    //---------------------------------------------------------------------------------------------------------
    jID=77 * 3;
    cv::Point rchin(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    //--------------------------------------------------------------------------------------------------------- 
    
    
    //---------------------------------------------------------------------------------------------------------
    jID=78 * 3;
    cv::Point lchin7(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=79 * 3;
    cv::Point lchin6(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=80 * 3;
    cv::Point lchin5(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=81 * 3;
    cv::Point lchin4(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=82 * 3;
    cv::Point lchin3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); 
    jID=83 * 3;
    cv::Point lchin2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); 
    jID=84 * 3;
    cv::Point lchin1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); 
    jID=85 * 3;
    cv::Point lchin0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); 
    //--------------------------------------------------------------------------------------------------------- 
        
    drawLineIfPointsExist(*overlayP,rchin0,rchin1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,rchin1,rchin2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,rchin3,rchin4,faceColor,faceThickness); 
    drawLineIfPointsExist(*overlayP,rchin4,rchin5,faceColor,faceThickness); 
    drawLineIfPointsExist(*overlayP,rchin5,rchin6,faceColor,faceThickness); 
    drawLineIfPointsExist(*overlayP,rchin6,rchin7,faceColor,faceThickness); 
    
    drawLineIfPointsExist(*overlayP,rchin7,rchin,faceColor,faceThickness); 
    drawLineIfPointsExist(*overlayP,rchin,lchin7,faceColor,faceThickness); 
    
    drawLineIfPointsExist(*overlayP,lchin0,lchin1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,lchin1,lchin2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,lchin3,lchin4,faceColor,faceThickness); 
    drawLineIfPointsExist(*overlayP,lchin4,lchin5,faceColor,faceThickness); 
    drawLineIfPointsExist(*overlayP,lchin5,lchin6,faceColor,faceThickness); 
    drawLineIfPointsExist(*overlayP,lchin6,lchin7,faceColor,faceThickness); 
    
    drawLineIfPointsExist(*overlayP,neck,rchin,cv::Scalar(255,0,0),bodyThickness);  
    
    drawLineIfPointsExist(*overlayP,neck,rchin,cv::Scalar(255,0,0),bodyThickness);  
    
    drawLineIfPointsExist(*overlayP,neck,rchin,faceColor,bodyThickness);  
    drawLineIfPointsExist(*overlayP,rear,rchin2,faceColor,bodyThickness);  
    drawLineIfPointsExist(*overlayP,lear,lchin2,faceColor,bodyThickness);  
    //---------------------------------------------------------------------------------------------------------
    jID=86 * 3;
    cv::Point reyebrow0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=87 * 3;
    cv::Point reyebrow1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=88 * 3;
    cv::Point reyebrow2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=89 * 3;
    cv::Point reyebrow3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=90 * 3;
    cv::Point reyebrow4(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    //--------------------------------------------------------------------------------------------------------- 
    
    //---------------------------------------------------------------------------------------------------------
    jID=91 * 3;
    cv::Point leyebrow4(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=92 * 3;
    cv::Point leyebrow3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=93 * 3;
    cv::Point leyebrow2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=94 * 3;
    cv::Point leyebrow1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=95 * 3;
    cv::Point leyebrow0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    //--------------------------------------------------------------------------------------------------------- 
    

    drawLineIfPointsExist(*overlayP,reyebrow0,reyebrow1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,reyebrow1,reyebrow2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,reyebrow2,reyebrow3,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,reyebrow3,reyebrow4,faceColor,faceThickness);
    
    drawLineIfPointsExist(*overlayP,leyebrow0,leyebrow1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,leyebrow1,leyebrow2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,leyebrow2,leyebrow3,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,leyebrow3,leyebrow4,faceColor,faceThickness);
    
    
    
    //---------------------------------------------------------------------------------------------------------
    jID=96 * 3;
    cv::Point nosebone0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=97 * 3;
    cv::Point nosebone1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=98 * 3;
    cv::Point nosebone2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=99 * 3; 
    cv::Point nosebone3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value); 
    //--------------------------------------------------------------------------------------------------------- 
    jID=100 * 3;
    cv::Point nostrills0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=101 * 3;
    cv::Point nostrills1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=102 * 3;
    cv::Point nostrills2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=103 * 3;
    cv::Point nostrills3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=104 * 3;
    cv::Point nostrills4(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    
 
    drawLineIfPointsExist(*overlayP,nosebone0,nosebone1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,nosebone1,nosebone2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,nosebone2,nosebone3,faceColor,faceThickness); 
    
    drawLineIfPointsExist(*overlayP,nostrills0,nostrills1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,nostrills1,nostrills2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,nostrills2,nostrills3,faceColor,faceThickness); 
    drawLineIfPointsExist(*overlayP,nostrills3,nostrills4,faceColor,faceThickness); 
    
    
    
    
    //---------------------------------------------------------------------------------------------------------
    jID=105 * 3;
    cv::Point reye0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=106 * 3;
    cv::Point reye1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=107 * 3;
    cv::Point reye2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=108 * 3;
    cv::Point reye3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=109 * 3;
    cv::Point reye4(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=110 * 3;
    cv::Point reye5(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    //---------------------------------------------------------------------------------------------------------
    drawLineIfPointsExist(*overlayP,reye0,reye1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,reye1,reye2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,reye2,reye3,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,reye3,reye4,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,reye4,reye5,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,reye0,reye5,faceColor,faceThickness);
    //---------------------------------------------------------------------------------------------------------
    
    
    
    
    
    //---------------------------------------------------------------------------------------------------------
    jID=111 * 3;
    cv::Point leye0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=112 * 3;
    cv::Point leye1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=113 * 3;
    cv::Point leye2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=114 * 3;
    cv::Point leye3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=115 * 3;
    cv::Point leye4(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=116 * 3;
    cv::Point leye5(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    //---------------------------------------------------------------------------------------------------------
    drawLineIfPointsExist(*overlayP,leye0,leye1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,leye1,leye2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,leye2,leye3,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,leye3,leye4,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,leye4,leye5,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,leye0,leye5,faceColor,faceThickness);
    //---------------------------------------------------------------------------------------------------------

    
    
    //---------------------------------------------------------------------------------------------------------
    jID=117 * 3;
    cv::Point outmouth0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=118 * 3;
    cv::Point outmouth1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=119 * 3;
    cv::Point outmouth2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=120 * 3;
    cv::Point outmouth3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=121 * 3;
    cv::Point outmouth4(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=122 * 3;
    cv::Point outmouth5(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=123 * 3;
    cv::Point outmouth6(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=124 * 3;
    cv::Point outmouth7(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=125 * 3;
    cv::Point outmouth8(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=126 * 3;
    cv::Point outmouth9(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=127 * 3;
    cv::Point outmouth10(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=128 * 3;
    cv::Point outmouth11(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    //---------------------------------------------------------------------------------------------------------
    drawLineIfPointsExist(*overlayP,outmouth0,outmouth1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth1,outmouth2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth2,outmouth3,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth3,outmouth4,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth4,outmouth5,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth5,outmouth6,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth6,outmouth7,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth7,outmouth8,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth8,outmouth9,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth9,outmouth10,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth10,outmouth11,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,outmouth11,outmouth0,faceColor,faceThickness);
    //---------------------------------------------------------------------------------------------------------
  

    //---------------------------------------------------------------------------------------------------------
    jID=129 * 3;
    cv::Point inmouth0(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=130 * 3;
    cv::Point inmouth1(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=131 * 3;
    cv::Point inmouth2(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=132 * 3;
    cv::Point inmouth3(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=133 * 3;
    cv::Point inmouth4(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=134 * 3;
    cv::Point inmouth5(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=135 * 3;
    cv::Point inmouth6(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    jID=136 * 3;
    cv::Point inmouth7(x+xRatio*skeleton->skeletonBody[jID+0].value,y+yRatio*skeleton->skeletonBody[jID+1].value);
    //---------------------------------------------------------------------------------------------------------
    drawLineIfPointsExist(*overlayP,inmouth0,inmouth1,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,inmouth1,inmouth2,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,inmouth2,inmouth3,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,inmouth3,inmouth4,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,inmouth4,inmouth5,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,inmouth5,inmouth6,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,inmouth6,inmouth7,faceColor,faceThickness);
    drawLineIfPointsExist(*overlayP,inmouth7,inmouth0,faceColor,faceThickness);
    //---------------------------------------------------------------------------------------------------------
     
    
     #if GOOFY_EYES
       cv::circle(*overlayP,reye,6,cv::Scalar(255,255,255),faceThickness,8,0);
       cv::circle(*overlayP,reye,3,cv::Scalar(0,0,0),faceThickness,8,0);
       cv::circle(*overlayP,leye,6,cv::Scalar(255,255,255),faceThickness,8,0);
       cv::circle(*overlayP,leye,3,cv::Scalar(0,0,0),faceThickness,8,0);
     #endif
    }
      else
    {
     drawLineIfPointsExist(*overlayP,reye,head,faceColor,bodyThickness);
     drawLineIfPointsExist(*overlayP,leye,head,faceColor,bodyThickness);  
     drawLineIfPointsExist(*overlayP,neck,head,faceColor,bodyThickness);     
     
     #if GOOFY_EYES
       cv::circle(*overlayP,reye,6,cv::Scalar(255,255,255),faceThickness,8,0);
       cv::circle(*overlayP,reye,3,cv::Scalar(0,0,0),faceThickness,8,0);
       cv::circle(*overlayP,leye,6,cv::Scalar(255,255,255),faceThickness,8,0);
       cv::circle(*overlayP,leye,3,cv::Scalar(0,0,0),faceThickness,8,0);
     #endif
    }
    
    
    
    thickness=1.0;

    if (pointsThatSeemToBeWronglyNormalized>0)
        {
            fprintf(stderr,"visualizeInput2DSkeletonFromSkeletonSerialized %u points seem to be incorrectly normalized\n",pointsThatSeemToBeWronglyNormalized);
            fprintf(stderr,"signaled units are  %0.2f x %0.2f \n",skeleton->width,skeleton->height);
        }


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
     cv::addWeighted(*baseP,1.0, *overlayP, 0.5, 0.0, *outputMatP);
    #endif
    
    return 1;
}



int drawSkeleton(cv::Mat &outputMat,std::vector<std::vector<float> > points2DOutputGUIForcedView,float offsetX,float offsetY,int labels)
{
    if (points2DOutputGUIForcedView.size()==0)
    {
        return 0;
    }
    
    
    for (int jointID=0; jointID<points2DOutputGUIForcedView.size(); jointID++)
        {
            float jointPointX = points2DOutputGUIForcedView[jointID][0]+offsetX;
            float jointPointY = points2DOutputGUIForcedView[jointID][1]+offsetY;
            cv::Point jointPoint(jointPointX,jointPointY);
            //fprintf(stderr,"L x,y %0.2f,%0.2f \n",jointPointX,jointPointY);
            int isBasicBodyComponent=0;
 
            if ( (jointPointX!=0) && (jointPointY!=0) )
                {
                    //unsigned int parentID = Body25SkeletonJointsParentRelationMap[jointID];
                    unsigned int parentID = getBVHParentJoint(jointID);
                    if (parentID!=jointID)
                        {
                            if (parentID<points2DOutputGUIForcedView.size())
                                {
                                    float parentPointX = points2DOutputGUIForcedView[parentID][0]+offsetX;
                                    float parentPointY = points2DOutputGUIForcedView[parentID][1]+offsetY;
                                    cv::Point parentPoint(parentPointX,parentPointY);
                                    
                                    cv::Scalar color = cv::Scalar(255,0,0);
                                    const char * jointName = getBVHJointName(jointID);
                                    const char * parentName = getBVHJointName(parentID);
                                    if ( (parentName!=0) && (jointName!=0) )
                                      {
                                          /* //Default is Blue so don't need to spam strcmp..
                                        if (strcmp("hip",jointName)==0)     { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("head",jointName)==0)    { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("neck",jointName)==0)    { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("chest",jointName)==0)   { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("chest",parentName)==0)  { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("abdomen",jointName)==0) { color = cv::Scalar(255,0,0); }  else
                                        if (strcmp("head",parentName)==0)   { color = cv::Scalar(255,0,0); }  else */
                                        //-------------------------------------------------------------------------
                                        if (strcmp("lShldr",jointName)==0)  { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lForeArm",jointName)==0){ color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lHand",jointName)==0)   { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lHand",parentName)==0)  { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lButtock",jointName)==0){ color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lShin",jointName)==0)   { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lFoot",jointName)==0)   { color = cv::Scalar(0,0,255);  }  else
                                        if (strcmp("lFoot",parentName)==0)  { color = cv::Scalar(0,0,255);  }  else
                                        //------------------------------------------------------------------------- 
                                        if (strcmp("rShldr",jointName)==0)  { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rForeArm",jointName)==0){ color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rHand",jointName)==0)   { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rHand",parentName)==0)  { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rButtock",jointName)==0){ color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rShin",jointName)==0)   { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rFoot",jointName)==0)   { color = cv::Scalar(0,255,0);  }  else
                                        if (strcmp("rFoot",parentName)==0)  { color = cv::Scalar(0,255,0);  } 
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
 
    char textInfo[512]={0};
 
   //Just the points and text ( foreground )
    for (int jointID=0; jointID<points2DOutputGUIForcedView.size(); jointID++)
        {
            float jointPointX = points2DOutputGUIForcedView[jointID][0]+offsetX;
            float jointPointY = points2DOutputGUIForcedView[jointID][1]+offsetY;
            //fprintf(stderr,"P x,y %0.2f,%0.2f \n",jointPointX,jointPointY);
 
 
            if ( (jointPointX!=0) && (jointPointY!=0) )
                {
                    cv::Point jointPoint(jointPointX,jointPointY);
                    int thickness=-2;
                    cv::circle(outputMat,jointPoint,3,cv::Scalar(255,0,255),thickness,8,0);
 
                    int highlight = 0;    
                    int filterOut = 1;    
                    const char * jointName = getBVHJointName(jointID);
                    if (jointName!=0)
                        {
                            snprintf(textInfo,512,"%s",jointName);
                            
                            
                            #if GOOFY_EYES
                             if (strcmp("eye.l",jointName)==0)     { filterOut = 1; highlight = 1; }  else
                             if (strcmp("eye.r",jointName)==0)     { filterOut = 1; highlight = 1; }  else
                            #endif
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
                    if ((!filterOut) && (labels))
                      { cv::putText(outputMat, textInfo  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 ); }
                      
                    if (highlight)
                    { 
                      cv::circle(outputMat,jointPoint,6,cv::Scalar(255,255,255),thickness,8,0);
                      cv::circle(outputMat,jointPoint,3,cv::Scalar(0,0,0),thickness,8,0);
                    }
                }
        }
        
     return 1;   
}
#endif
#endif
