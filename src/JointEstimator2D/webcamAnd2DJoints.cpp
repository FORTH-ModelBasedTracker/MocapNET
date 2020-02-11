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
    const char * webcam = 0;

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
                }
        }
    //------------------------------------------------------


    VideoCapture cap(webcam); // open the default camera
    if (webcam==0)
        {
            std::cerr<<"Trying to open webcam\n";
            cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
            cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
        }
    else
        {
            std::cerr<<"Trying to open "<<webcam<<"\n";
        }

    if (!cap.isOpened())  // check if succeeded to connect to the camera
        {
            std::cerr<<"Openning camera failed\n";
            return 1;
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
                            std::vector<std::vector<float> >  heatmaps = getHeatmaps(
                                                                                      &jointEstimator,
                                                                                      frame.data,
                                                                                      jointEstimator.inputWidth2DJointDetector,
                                                                                      jointEstimator.inputHeight2DJointDetector
                                                                                    );
                            if (heatmaps.size()>0)
                                {
                                    //visualizeHeatmaps(&jointEstimator,heatmaps,frameNumber);

                                    estimate2DSkeletonsFromHeatmaps(&jointEstimator,&result,heatmaps);

                                    dj_drawExtractedSkeletons(
                                        viewMat,
                                        &result,
                                        jointEstimator.inputWidth2DJointDetector,
                                        jointEstimator.inputHeight2DJointDetector
                                    );

                                    imshow("Skeletons", viewMat);

                                } 

                        }
                    else
                        {
                            std::cerr<<"Broken frame.. \n"; 
                        }
                    waitKey(1);
                    ++frameNumber;
                }
        }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
