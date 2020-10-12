#include "template.hpp"
#include "visualization.hpp"
#include "widgets.hpp"

#include "drawSkeleton.hpp"

#include "../IO/jsonRead.hpp"
#include "../IO/bvh.hpp"
#include "../tools.hpp"
#include "../mocapnet2.hpp"




int visualizeTemplate(
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
    cv::Mat raytraced;


    snprintf(finalFilename,256,"%s/raytraced2/%04u.png",path,frameNumber);
    raytraced = imread(finalFilename,cv::IMREAD_COLOR);   // older versions might want the CV_LOAD_IMAGE_COLOR  flag

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
            
                
            
        visualizeSkeletonSerialized(
                                    image,
                                    skeleton,
                                    1,//Show left hand
                                    1,//Show right hand
                                    1,//Show face
                                    0,0,
                                    image.size().width,
                                    image.size().height
                                   ); 
        drawSkeleton(image,points2DOutputGUIRealView,-330,-0,0);    
                

    int NSDMWidth=160;  //body leftHand rightHand
    int NSDMHeight=160;
     
    int positionX = image.size().width - NSDMWidth - 100;
    int positionY = 240;
    
    cv::rectangle(image,cv::Rect(positionX-20,positionY-40,NSDMWidth+40,620+showFramerate),cv::Scalar(0,0,0),-1);
    
    visualizeNSDM(image,"Upper Body",mnet->upperBody.NSDM,1 /*angles*/,positionX,positionY,NSDMWidth,NSDMHeight);
    positionY+=200;
    visualizeNSDM(image,"Lower Body",mnet->lowerBody.NSDM,1 /*angles*/,positionX,positionY,NSDMWidth,NSDMHeight);
    positionY+=200;

    visualizeOrientation(
        image,"Orientation",
        mnet->currentSolution[4], //Orientation value
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
    //-----------------------------------------------------------------
    float thickness=1.7;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
    cv::Scalar fontColor= cv::Scalar(255,255,255);
    cv::Point txtPosition(positionX+10,positionY+190);
    char fpsString[128];
    snprintf(fpsString,128,"%0.2f fps",options->totalLoopFPS);
    cv::putText(image,fpsString,txtPosition,fontUsed,0.8,fontColor,thickness,8);
    //-----------------------------------------------------------------
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
