#include "opencv2/opencv.hpp"
/** @file webcam.cpp
 *  @brief This is a simple test file to make sure your camera or video files can be opened using OpenCV
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include <stdio.h>


using namespace cv;

int main(int argc, char *argv[])
{
    unsigned int width = 640;
    unsigned int height = 480;
    
    
    const char * webcam = 0;
    for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--from")==0)
                {
                    if (argc>i+1)
                        {
                            webcam = argv[i+1];
                        }
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


    VideoCapture cap(webcam); // open the default camera
    if (webcam==0)
        {
            std::cerr<<"Trying to open webcam\n";
            cap.set(CAP_PROP_FRAME_WIDTH,width);
            cap.set(CAP_PROP_FRAME_HEIGHT,height);
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



    Mat edges;
    namedWindow("feed",1);
    for(;;)
        {
            Mat frame;
            cap >> frame;
            if ( (frame.size().width>0) && (frame.size().height>0) )
                {
                    imshow("feed", frame);
                }
            else
                {
                    std::cerr<<"Broken frame.. \n";
                }
            waitKey(1);
        }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
