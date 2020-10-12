#include "opencv2/opencv.hpp"
/** @file webcam.cpp
 *  @brief This is a simple test file to make sure your camera or video files can be opened using OpenCV
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include <stdio.h>
#include "cameraControl.hpp"
#include "jointEstimator2D.hpp"
#include "visualization.hpp"

#include "../MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp"

using namespace cv;


int main(int argc, char *argv[])
{
    unsigned int width = 640;
    unsigned int height = 480;
    const char * webcam = 0;
    int heatmapDebugVisualizations=0;

    //------------------------------------------------------
    //                Parse arguments
    //------------------------------------------------------
    for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--from")==0)
                {
                    if (argc>i+1)
                        {
                            webcam = argv[i+1];
                        }  
                } else
            if (strcmp(argv[i],"--debug")==0)
                {
                    heatmapDebugVisualizations = 1;
                }   else 
             if (strcmp(argv[i],"--size")==0)
                    {
                      if (argc>i+2)
                        {
                        width = atoi(argv[i+1]);
                        height = atoi(argv[i+2]);
                        }
                    }
                    
        }
    //------------------------------------------------------


    VideoCapture cap(webcam); // open the default camera
     
     std::cerr<<"Trying to open webcam ("<<webcam<<" @ "<<width<<","<<height<<") \n";
    if (!cap.isOpened())  // check if succeeded to connect to the camera
        {
            std::cerr<<"Openning camera failed\n";
            return 1;
        }
        
     if (strstr(webcam,"/dev/video")!=0)
     {
     cap.set(cv::CAP_PROP_FRAME_WIDTH,width);
     cap.set(cv::CAP_PROP_FRAME_HEIGHT,height);
     }

    struct JointEstimator2D jointEstimator;
    if (loadJointEstimator2D(&jointEstimator,JOINT_2D_ESTIMATOR_FORTH,1,0))
        {
            cv::Mat viewMat = Mat(Size(jointEstimator.inputWidth2DJointDetector,jointEstimator.inputHeight2DJointDetector),CV_8UC3, Scalar(0,0,0));

            struct Skeletons2DDetected result= {0};
            //cv::namedWindow("Video Input Feed",1);
            //cv::moveWindow("Video Input Feed",0,368);
            cv::namedWindow("Skeletons",1);
            cv::moveWindow("Skeletons",0,0);
            
            unsigned int frameNumber=0;
            unsigned long elapsedTime=0;
            unsigned int brokenFrames = 0;
            
            for(;;)
                {
                    Mat frame;
                    cap >> frame;
                    if ( (frame.size().width>0) && (frame.size().height>0) )
                        {
                            if (
                                !cropAndResizeCVMatToMatchSkeleton(
                                    &jointEstimator,
                                    frame,
                                    &result
                                )
                               )
                                {
                                    fprintf(stderr,"Failed to crop input video\n");
                                }
                            //imshow("Video Input Feed", frame);
                            
                            frame.copyTo(viewMat);
                            // viewMat.setTo(Scalar(0,0,0));
                            
                            
                            //This needs to be done to get tensorflow output..
                            frame.convertTo(frame,CV_32FC3); 
                            
                            //Keep time 
                            unsigned long startTime = GetTickCountMicrosecondsJointEstimator();
                            
                            std::vector<std::vector<float> >  heatmaps = getHeatmaps(
                                                                                                                                                   &jointEstimator,
                                                                                                                                                   frame.data,
                                                                                                                                                   jointEstimator.inputWidth2DJointDetector,
                                                                                                                                                   jointEstimator.inputHeight2DJointDetector
                                                                                                                                                 );
                            if (heatmaps.size()>0)
                                {
                                    if (heatmapDebugVisualizations) 
                                         { visualizeHeatmaps(&jointEstimator,heatmaps,frameNumber); }

                                    estimate2DSkeletonsFromHeatmaps(&jointEstimator,&result,heatmaps);

                                   //Keep time 
                                   unsigned long endTime = GetTickCountMicrosecondsJointEstimator();
                                   elapsedTime += endTime - startTime;
 
                                    dj_drawExtractedSkeletons(
                                        viewMat,
                                        &result,
                                        jointEstimator.inputWidth2DJointDetector,
                                        jointEstimator.inputHeight2DJointDetector
                                    );

                                    imshow("Skeletons", viewMat);

                                } 

                           ++frameNumber;
                        }
                    else
                        {
                            std::cerr<<"Broken frame.. \n"; 
                            ++brokenFrames; 
                        }
                    waitKey(1);
                    if (brokenFrames>10) { break; }
                }
                
                if (frameNumber>0)
                {
                    fprintf(stderr,"Elapsed time is %lu microsconds for %u frames\n",elapsedTime,frameNumber);
                    fprintf(stderr,"Framerate : %0.2f fps\n",(float) elapsedTime/(1000* frameNumber));
                } 
        }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
