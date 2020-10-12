#include "map.hpp"
#include "visualization.hpp"

#include "../applicationLogic/artifactRecognition.hpp"

#include "../IO/jsonRead.hpp"
#include "../IO/bvh.hpp"
#include "../IO/skeletonAbstraction.hpp"
#include "../tools.hpp"
#include "../mocapnet2.hpp"


std::vector<cv::Point> locationHistory;

int drawPerson(cv::Mat & img,float x,float y,float r)
{
    int maxLocationHistory=15;
    cv::Scalar color= cv::Scalar(0,0,255);
    cv::Scalar colorLook= cv::Scalar(100,0,90);

    unsigned int width = img.size().width;
    unsigned int height = img.size().height;


    cv::Point startPoint(0,0);
    cv::Point endPoint(0,0);

    unsigned int x2D = x*2 + width/2;
    unsigned int y2D = -1*y*2 + height/2;
    
    //Draw Cross
    //-----------------------------------------------
    startPoint.x = x2D - 10;
    startPoint.y = y2D;
    endPoint.x = x2D + 10;
    endPoint.y = y2D;
    cv::line(img,startPoint,endPoint,color,2.0);
    //-----------------------------------------------
    startPoint.x = x2D;
    startPoint.y = y2D - 10;
    endPoint.x = x2D;
    endPoint.y = y2D + 10;
    cv::line(img,startPoint,endPoint,color,2.0);
    //-----------------------------------------------

 
    float rad=((float) -1.0 * 3.1415/180*r);
    startPoint.x=x2D;
    startPoint.y=y2D;
    endPoint.x=x2D+cos(rad)* (width/5);
    endPoint.y=y2D+sin(rad)* (height/5);
     cv::line(img,startPoint,endPoint,colorLook,2.0); 
    


    //---------------------------------------------------------------------------------------------------------------------------------------------
    cv::Point p(x2D,y2D);
    locationHistory.push_back(p);
    if (locationHistory.size()>maxLocationHistory)
        {
            locationHistory.erase(locationHistory.begin());
        }
    float stepColorD=(float) 255/maxLocationHistory;
    for (int step=0; step<locationHistory.size(); step++)
        {
            float stepColor = 255 - step*stepColorD;
            if (step>1)
                {
                    cv::line(img,locationHistory[step],locationHistory[step-1], cv::Scalar(0,0,stepColor), 1.0);
                }
        }
    //---------------------------------------------------------------------------------------------------------------------------------------------

    return 1;
}


int visualizeMap(
    const char* windowName,
    unsigned int frameNumber,
    unsigned int width,
    unsigned int height,
    struct skeletonSerialized * skeleton,
    struct MocapNET2 * mnet,
    std::vector<float>  result,
    std::vector<std::vector<float> > points2DOutputGUIRealView,
    unsigned int numberOfMissingJoints,
    unsigned int gestureDetected,
    const char * gestureName,
    unsigned int gestureFrame
)
{
    fprintf(stderr,"visualizeMap called with %lu\n",points2DOutputGUIRealView.size());
    if (points2DOutputGUIRealView.size()==0)
        {
            return 0;
        }
#if USE_OPENCV

    unsigned int gridSpace=30;
    cv::Point startPoint(0,0);
    cv::Point endPoint(0,height);


    cv::Mat img(height,width, CV_8UC3, Scalar(60,30,30));

    cv::Scalar lookingColor= cv::Scalar(255,255,0);
    cv::Scalar activeColor= cv::Scalar(0,123,255);
    cv::Scalar color= cv::Scalar(253,123,30);

    for (unsigned int x=0; x<width; x+=gridSpace)
        {
            startPoint.x=x;
            endPoint.x=x;
            cv::line(img,startPoint,endPoint,color,1.0);
        }

    startPoint.x=0;
    endPoint.x=width;
    for (unsigned int y=0; y<height; y+=gridSpace)
        {
            startPoint.y=y;
            endPoint.y=y;
            cv::line(img,startPoint,endPoint,color,1.0);
        }



    float thickness=1.0;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
    char text[512];
    snprintf(text,512," Position  X=%0.2f Y=%0.2f Z=%0.2f     %0.2f|%0.2f|%0.2f",result[0],result[1],result[2],result[3],result[4],result[5]);

    startPoint.x=50;
    startPoint.y=50;
    cv::putText(img,text,startPoint,fontUsed,0.4,color,thickness,8);

   std::vector<float>  points3DFlatOutput=convertBVHFrameToFlat3DPoints(result); //,width,height
   //writeCSVHeaderFromLabelsAndVectorOfVectors("out3DP.csv",MocapNETOutputArrayNames,MOCAPNET_OUTPUT_NUMBER,output3DPositions) )

    float x = result[2];
    float y = result[0];
    float r = result[4];

     float lElbowX = points3DFlatOutput[MOCAPNET_3DPOINT_LELBOWZ];
     float lElbowY = points3DFlatOutput[MOCAPNET_3DPOINT_LELBOWX];
     float lHandX = points3DFlatOutput[MOCAPNET_3DPOINT_LHANDZ];
     float lHandY = points3DFlatOutput[MOCAPNET_3DPOINT_LHANDX];


     float rElbowX = points3DFlatOutput[MOCAPNET_3DPOINT_RELBOWZ];
     float rElbowY = points3DFlatOutput[MOCAPNET_3DPOINT_RHANDX];
     float rHandX = points3DFlatOutput[MOCAPNET_3DPOINT_RHANDZ];
     float rHandY = points3DFlatOutput[MOCAPNET_3DPOINT_RHANDX];

    float rad=((float) 1.0 * 3.1415/180*r);
    float x2=x+cos(rad)* (width/5);
    float y2=y+sin(rad)* (height/5);




    //-------------------------------------------------------------------------------
    // If gestures are enabled.. Draw them..
    //-------------------------------------------------------------------------------
    if (gestureName!=0)
    {//-------------------------------------------------------------------------------
        if(gestureDetected)
            {
        if (gestureFrame>25)
        {
           snprintf(text,512,"%s  (%u)",gestureName,gestureDetected);           
            cv::Point txtPosition(600,200);
            cv::putText(img,text,txtPosition,fontUsed,1.5,cv::Scalar(0,255,255),thickness,8); 
            cv::circle(img,txtPosition,2*gestureFrame,cv::Scalar(0,255,255),3,8,0);  
          }
        }
     } 



    //-------------------------------------------------------------------------------
    // If poses are enabled.. Draw them..
    //-------------------------------------------------------------------------------
        if(mnet->activePose>1)
        {
           snprintf(text,512,"%s  (%u)",hardcodedPoseName[mnet->activePose],mnet->activePose);           
            cv::Point txtPosition(600,200);
            cv::putText(img,text,txtPosition,fontUsed,1.5,cv::Scalar(0,255,255),thickness,8); 
            cv::circle(img,txtPosition,2*gestureFrame,cv::Scalar(0,255,255),3,8,0);  
          }







    for (unsigned int artifactID=0; artifactID<mnet->artifacts.numberOfArtifacts; artifactID++)
    {
        startPoint.x= mnet->artifacts.artifact[artifactID].x1*2 + width/2;
        startPoint.y= mnet->artifacts.artifact[artifactID].y1*-2 + height/2;
        endPoint.x= mnet->artifacts.artifact[artifactID].x2*2 + width/2;
        endPoint.y= mnet->artifacts.artifact[artifactID].y2*-2 + height/2;
        

        if (check3DArtifactCollision(
                                                                 &mnet->artifacts.artifact[artifactID],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_LELBOWZ],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_LELBOWX],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_LELBOWY],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_LHANDZ],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_LHANDX],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_LHANDY]
                                                              )
            )
        { 
             fprintf(stderr,"Left Hand Collision %u\n",artifactID);
        } else
       //---------------------------------------------------------------------------------------------------------------------------------------------
        if (check3DArtifactCollision(
                                                                 &mnet->artifacts.artifact[artifactID],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_RELBOWZ],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_RELBOWX],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_RELBOWY],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_RHANDZ],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_RHANDX],
                                                                 points3DFlatOutput[MOCAPNET_3DPOINT_RHANDY]
                                                              )
            )
        { 
             fprintf(stderr,"Right Hand Collision %u\n",artifactID);
        } else
       //---------------------------------------------------------------------------------------------------------------------------------------------
        if (checkArtifactCollision(&mnet->artifacts.artifact[artifactID],x, y, r))
        {
             fprintf(stderr,"ARTIFACT %u (%s) is collided \n",artifactID,mnet->artifacts.artifact[artifactID].label); 
            if ((mnet->artifacts.artifact[artifactID].activatesOnPosition) && (!mnet->artifacts.artifact[artifactID].active) )
            {
                mnet->artifacts.artifact[artifactID].active=1;
                int i=system(mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation);
                 if (i==0) { fprintf(stderr,"Executed %s \n",mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation); } else
                                 { fprintf(stderr,"Failed to execute %s \n",mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation); } 
            }
          cv::rectangle(img,startPoint,endPoint,activeColor,-1,8,0);    
           snprintf(text,512,"%s (aid=%u)",mnet->artifacts.artifact[artifactID].label,artifactID); 
           startPoint.y+=10;
          cv::putText(img,text,startPoint,fontUsed,0.4,cv::Scalar(0,0,255),thickness,8);
        } else
       //---------------------------------------------------------------------------------------------------------------------------------------------
         if (checkArtifactDirection(&mnet->artifacts.artifact[artifactID],x, y,x2,y2))
         {
             fprintf(stderr,"ARTIFACT %u (%s) is looked at \n",artifactID,mnet->artifacts.artifact[artifactID].label); 
            if ( (mnet->artifacts.artifact[artifactID].activateOnGesture) && (mnet->activePose>1) )
            {
                int i =system(mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation);
                 if (i==0) {  fprintf(stderr,"Executed Gesture %s \n",mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation);  } else
                                 {  fprintf(stderr,"Failed to execute gesture %s \n",mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation); } 
            } else
            if   ((mnet->artifacts.artifact[artifactID].activatesOnLook) &&  (!mnet->artifacts.artifact[artifactID].active))
            {
                mnet->artifacts.artifact[artifactID].active=1;
                int i = system(mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation);
                  if (i==0) {  fprintf(stderr,"Executed %s \n",mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation); } else
                                  {  fprintf(stderr,"Failed to execute gesture %s \n",mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation); } 
            }   
            
          cv::rectangle(img,startPoint,endPoint,lookingColor,-1,8,0);    
           snprintf(text,512,"%s (aid=%u)",mnet->artifacts.artifact[artifactID].label,artifactID); 
           startPoint.y+=10;
          cv::putText(img,text,startPoint,fontUsed,0.4,cv::Scalar(0,255,0),thickness,8);
         }
       //---------------------------------------------------------------------------------------------------------------------------------------------
          else
        {
             fprintf(stderr,"ARTIFACT %u (%s) is inactive \n",artifactID,mnet->artifacts.artifact[artifactID].label); 
            if  ((mnet->artifacts.artifact[artifactID].activatesOnPosition) && (mnet->artifacts.artifact[artifactID].active) )
            {
                mnet->artifacts.artifact[artifactID].active=0;
                int i = system(mnet->artifacts.artifact[artifactID].actionToExecuteOnDeactivation);
                   if (i==0) {  fprintf(stderr,"Executed %s \n",mnet->artifacts.artifact[artifactID].actionToExecuteOnDeactivation); } else
                                   {  fprintf(stderr,"Failed to execute %s \n",mnet->artifacts.artifact[artifactID].actionToExecuteOnActivation); } 
            }
           cv::rectangle(img,startPoint,endPoint,color,-1,8,0); 
           snprintf(text,512,"%s (aid=%u)",mnet->artifacts.artifact[artifactID].label,artifactID); 
           startPoint.y+=10;
          cv::putText(img,text,startPoint,fontUsed,0.4,cv::Scalar(255,0,0),thickness,8);
        }
      //--------------------------------------------------------------------------------------------------------------------------------------------- 
    }

    cv::Point leftHandStart(lElbowX*2 + width/2,-1* lElbowY*2 + height/2);
    cv::Point leftHandEnd(lHandX*2 + width/2,-1* lHandY*2 + height/2); 
     cv::line(img,leftHandStart,leftHandEnd,cv::Scalar(0,0,255),2.0); 
     
     
    cv::Point rightHandStart(rElbowX*2 + width/2,-1* rElbowY*2 + height/2);
    cv::Point rightHandEnd(rHandX*2 + width/2,-1* rHandY*2 + height/2); 
     cv::line(img,rightHandStart,rightHandEnd,cv::Scalar(0,255,0),2.0); 
    


    drawPerson(img,x,y,r);
    drawSkeleton(img,points2DOutputGUIRealView,-730,0,0);

    //Draw Camera -----------------------------------------------------
    startPoint.x = width - 50;
    startPoint.y = (height/2) - 20;
    endPoint.x = width - 100;
    endPoint.y = (height/2) + 20; 
    cv::rectangle(img,startPoint,endPoint,color,2,8,0);
    endPoint.y = (height/2) ;
    startPoint.x = endPoint.x-30;
    startPoint.y = endPoint.y+20;
    cv::line(img,startPoint,endPoint,color,2.0);
    startPoint.x = endPoint.x-30;
    startPoint.y = endPoint.y-20;
    cv::line(img,startPoint,endPoint,color,2.0);
    endPoint.x = endPoint.x-30;
    endPoint.y = endPoint.y+20;
    cv::line(img,startPoint,endPoint,color,2.0);
    //----------------------------------------------------------------------------

    cv::imshow(windowName,img);

    return 1;
#else
    fprintf(stderr,"OpenCV code not present in this build, cannot show visualization..\n");
#endif
    return 0;
}
