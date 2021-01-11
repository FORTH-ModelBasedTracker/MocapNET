#include "allInOne.hpp"
#include "visualization.hpp"
#include "widgets.hpp"
#include "drawSkeleton.hpp"
#include "opengl.hpp"


#include "../IO/jsonRead.hpp"
#include "../IO/bvh.hpp"
#include "../tools.hpp"
#include "../mocapnet2.hpp"


std::vector<float> frameRateHistoryTotal;
std::vector<float> frameRateIKHistory;
std::vector<float> frameRateMNET2History;


void appendToHistory(std::vector<float> & frameRateHistory, float newValue, unsigned int maximumValues)
{
   //Keep framerate history
   frameRateHistory.push_back(newValue);
   if (frameRateHistory.size()>maximumValues)
    {
       frameRateHistory.erase(frameRateHistory.begin());
    } 
}


#if USE_OPENCV
cv::Mat openGLAllInOneFramePermanentMat;


cv::Mat offsetImageWithPadding(const Mat& originalImage, int offsetX, int offsetY, Scalar backgroundColour)
{
        cv::Mat padded = Mat(originalImage.rows + 2 * abs(offsetY), originalImage.cols + 2 * abs(offsetX), CV_8UC3, backgroundColour);
        originalImage.copyTo(padded(Rect(abs(offsetX), abs(offsetY), originalImage.cols, originalImage.rows)));
        return Mat(padded,Rect(abs(offsetX) + offsetX, abs(offsetY) + offsetY, originalImage.cols, originalImage.rows));
}


#endif


int extraWindow(
                std::vector<std::vector<float> > points2DOutputGUIRealView,
                std::vector<std::vector<float> > points2DOutputGUIForcedView
               )
{
    //cv::Mat image = cv::Mat(800, 800, CV_8UC3);
    cv::Mat image = cv::Mat::zeros(1920, 1080, CV_8UC3); 
    
    //Switch that controls drawing skeleton labels
    int draw3DSkeletonJointLabels = 1; 
                               
    //This is the 3D skeleton visualized to match the RGB image           
    //drawSkeleton(image,points2DOutputGUIRealView,-1000,0,draw3DSkeletonJointLabels);    

    //This is the 3D skeleton nailed to be visualized to match the RGB image           
    drawSkeleton(image,points2DOutputGUIForcedView,-800,0,draw3DSkeletonJointLabels); 

    cv::imshow("New Visualization",image);   
    return 1;
}



int visualizeAllInOne(
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
    struct MocapNET2Options * options,
    std::vector<std::vector<float> > points2DOutputGUIRealView,
    std::vector<std::vector<float> > points2DOutputGUIForcedView, 
    unsigned int numberOfMissingJoints
)
{

#if USE_OPENCV
    int success=0;
    char finalFilename[2048]= {0}; 
    
    
    //Handle Background
    //-----------------------------------------------------------------------------------
    if (alreadyLoadedImage==0)
        {
            char formatString[256]= {0};
            snprintf(formatString,256,"%%s/%%s%%0%uu.jpg",serialLength);

            //colorFrame_0_00001.jpg
            snprintf(finalFilename,2048,formatString,path,label,frameNumber/*Frame ID*/);
            //snprintf(finalFilename,2048,"%s/colorFrame_0_%05d.jpg",path,frameNumber+1);
        }

    int showFramerate=30; //30 or 0 

    float scale=1.0;
    cv::Mat image;
    cv::Rect roi;
    cv::Mat destinationROI;

    if ( (fileExists(finalFilename) ) || (alreadyLoadedImage!=0) )
        {
            if (alreadyLoadedImage!=0)
                {
                    image = *alreadyLoadedImage;
                    
                    if  ( 
                          ( image.size().width < 640 ) || 
                          ( image.size().height < 480 )  
                        )
                        {
                            fprintf(stderr,"Very small given frame ( %ux%u..\n",image.size().width,image.size().height);
                            return 0;
                        }
                   
                    //cv::imshow(windowName,image); //
                    //cv::waitKey(0); 
                }
                 else
            if (fileExists(finalFilename) )
                {
                    image = imread(finalFilename,cv::IMREAD_COLOR);   // older versions might want the CV_LOAD_IMAGE_COLOR  flag
                }
    //-----------------------------------------------------------------------------------


    //The skeleton offset controls offset for the 3D skeleton rendering on top of the image
    //Offsets are wrong for resolutions other than 1920x1080 since they are hardcoded..
    float offsetX = (float) -1*image.size().width/4;
    float offsetY = 0.0;// (float) -1*image.size().height/1080;
    
    //If you uncomment the next like the 3D overlay will stay exactly on top of the skeleton
    //offsetX=0;


    //---------------------------------------------------------------------
    //                       OpenGL overlay stuff 
    //---------------------------------------------------------------------
    cv::Mat * openGLMatForVisualization = 0;
    if (options->useOpenGLVisualization) {
                                           //fprintf(stderr,"updateOpenGLView\n");
                                           updateOpenGLView(mnet->currentSolution);

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

                                                     cv::cvtColor(openGLMat,openGLAllInOneFramePermanentMat,cv::COLOR_RGB2BGR);
                                                     //cv::imshow("OpenGL",openGLAllInOneFramePermanentMat);
                                                     openGLMatForVisualization = &openGLAllInOneFramePermanentMat;
                                                     openGLMat.data=initialPointer;
                                                   }
                                          //=====================================================================
                                         }

   if (openGLMatForVisualization!=0)
   {
     cv::Mat * glMat = (cv::Mat *) openGLMatForVisualization;
     //float alpha=0.3; 
     //image=cv::max(*glMat,image);
     
     //example use with black borders along the right hand side and top:
     cv::Mat offsetImage = offsetImageWithPadding(*glMat, +offsetX, 0 , Scalar(0,0,0));
     image=overlay(image,offsetImage);
     //image=cv::max(offsetImage,image);
     //cv::addWeighted(offsetImage,1.0,image,0.8,0.0,image); 
     //cv::add(offsetImage,image,image);
   }
   //---------------------------------------------------------------------------------

            
    //This is the 2D visualization sitting on top of the RGB image
    //fprintf(stderr,"2D Visualization skeleton(%0.2f,%0.2f) rendered on %ux%u image\n",skeleton->width,skeleton->height,image.size().width,image.size().height);
    visualizeSkeletonSerialized(
                                image,
                                skeleton,
                                (mnet->leftHand.loadedModels>0),
                                (mnet->rightHand.loadedModels>0),
                                options->doFace,
                                0,0,
                                image.size().width,
                                image.size().height
                               ); 
                               
                               
    //Switch that controls drawing skeleton labels
    int draw3DSkeletonJointLabels = 0; 
                               
    //This is the 3D skeleton visualized to match the RGB image           
    drawSkeleton(image,points2DOutputGUIRealView,offsetX,offsetY,draw3DSkeletonJointLabels);    
                
    //Uncomment to spawn an extra window with a centered skeleton..!
    //extraWindow(points2DOutputGUIRealView,points2DOutputGUIForcedView);
    
    //If you dont want to see widgets just turn them off using this switch
    int showWidgets = 1; 
                
                 
    if (showWidgets)
    {
    int borderToScreen = 15;
    int NSDMWidth=160;  //body leftHand rightHand
    int NSDMHeight=160;
     
    int positionX = image.size().width - NSDMWidth - borderToScreen;
    int positionY = 40;
    
    cv::rectangle(image,cv::Rect(positionX-20,positionY-40,NSDMWidth+40,620+showFramerate),cv::Scalar(0,0,0),-1);
    
    visualizeNSDM(image,"Upper Body",mnet->upperBody.NSDM,1 /*angles*/,positionX,positionY,NSDMWidth,NSDMHeight);
    positionY+=200;
    visualizeNSDM(image,"Lower Body",mnet->lowerBody.NSDM,1 /*angles*/,positionX,positionY,NSDMWidth,NSDMHeight);
    positionY+=200;

  
    if ( (mnet->leftHand.loadedModels>0) || (mnet->rightHand.loadedModels>0) )
    {
      int positionLX = borderToScreen;
      int positionLY = 40;
      cv::rectangle(image,cv::Rect(positionLX-20,positionLY-40,NSDMWidth+40,420),cv::Scalar(0,0,0),-1);
    
      visualizeNSDM(image,"Left Hand",mnet->leftHand.NSDM,1 /*angles*/,positionLX,positionLY,NSDMWidth,NSDMHeight);
      positionLY+=200;
      visualizeNSDM(image,"Right Hand",mnet->rightHand.NSDM,1 /*angles*/,positionLX,positionLY,NSDMWidth,NSDMHeight);
      positionLY+=200; 
    }


    visualizeOrientation(
        image,"Orientation",
        mnet->currentSolution[4],
        mnet->orientationClassifications[0],
        mnet->orientationClassifications[1],
        mnet->orientationClassifications[2],
        mnet->orientationClassifications[3],
        positionX,
        positionY,
        NSDMWidth,
        NSDMHeight
    );
    
     
    if (showFramerate>0)
    { 
     unsigned int framerateWidgetWidth  = NSDMWidth + 40;    
     unsigned int framerateWidgetHeight = NSDMHeight;    
        
        
     //Keep framerate history
     appendToHistory(frameRateHistoryTotal,options->totalLoopFPS,framerateWidgetWidth);
     appendToHistory(frameRateIKHistory,mnet->inverseKinematicsFramerate,framerateWidgetWidth);
     appendToHistory(frameRateMNET2History,mnet->neuralNetworkFramerate,framerateWidgetWidth);
  
     //Plot framerate history
     plotFloatVector(
                     image,
                     1,//Erase background
                     frameRateHistoryTotal,
                     positionX-20,
                     positionY+210,
                     framerateWidgetWidth, //Have same size as NSDM matrix
                     framerateWidgetHeight //Have same size as NSDM matrix
                   );             
                   /*
      //Not enough space..
      plotFloatVector(
                     image,
                     0,//Erase background
                     frameRateIKHistory,
                     positionX-20,
                     positionY+210,
                     framerateWidgetWidth, //Have same size as NSDM matrix
                     framerateWidgetHeight //Have same size as NSDM matrix
                   );        
      plotFloatVector(
                     image,
                     0,//Erase background
                     frameRateMNET2History,
                     positionX-20,
                     positionY+210,
                     framerateWidgetWidth, //Have same size as NSDM matrix
                     framerateWidgetHeight //Have same size as NSDM matrix
                   ); */
                   
    //-----------------------------------------------------------------
    float thickness=1.7;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
    cv::Scalar fontColor= cv::Scalar(255,255,255);
    cv::Point  txtPosition(positionX,positionY+190);
    char fpsString[128];

    snprintf(fpsString,128,"Framerate"); //2D->3D 
    cv::putText(image,fpsString,txtPosition,fontUsed,0.6,fontColor,thickness,8);
    txtPosition.y+=30;
    txtPosition.x+=10;

    snprintf(fpsString,128,"%0.2f fps",options->totalLoopFPS);
    cv::putText(image,fpsString,txtPosition,fontUsed,0.8,fontColor,thickness,8);
    
    if (
         strlen(options->message)>0 
       )
    {
      txtPosition.y=60;
      txtPosition.x=30;        
      fontColor= cv::Scalar(0,0,0);
      cv::putText(image,options->message,txtPosition,fontUsed,1.8,fontColor,3.0,8);     
      txtPosition.x+=3;
      txtPosition.y+=3;
      fontColor= cv::Scalar(255,255,255);
      cv::putText(image,options->message,txtPosition,fontUsed,1.8,fontColor,3.0,8);     
    }
    
    //-----------------------------------------------------------------
    } 
    
    
    
    }

            if(image.data!=0)
                {
                    if ( image.size().height > image.size().width )
                        {
                            scale=(float) 720/image.size().height;
                        }
                    else
                        {
                            scale=(float) 1024/image.size().width;
                        }
                    if (scale>1.0)
                        {
                            scale=1.0;
                        }
                    if (scale!=1.0)
                        {
                            cv::resize(image, image, cv::Size(0,0), scale, scale);
                        }

                    //fprintf(stderr,"Image ( %u x %u )\n",image.size().width,image.size().height);
                    success=1;
                }

        }
    else
        {
            fprintf(stderr," Could not load %s image, cannot proceed to visualize it\n",finalFilename);
            return 0;
        }
    
    if (saveVisualization)
    { 
        char filename[512];
        snprintf(filename,512,"vis%05u.jpg",frameNumber) ;
        cv::imwrite(filename,image);
    }
    
    cv::imshow(windowName,image); //
    //Did not find a file to show ..
    return success;
#else
    fprintf(stderr,"OpenCV code not present in this build, cannot show visualization..\n");
#endif
    return 0;
}
