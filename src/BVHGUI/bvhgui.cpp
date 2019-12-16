#include "opencv2/opencv.hpp"
/** @file webcam.cpp
 *  @brief This is a simple test file to make sure your camera or video files can be opened using OpenCV
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include <stdio.h>
#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/bvh.hpp"
#include "../MocapNETLib/visualization.hpp"
#include <vector>

using namespace cv;

struct applicationState
{
 std::vector<float>bvhConfiguration;
 std::vector<std::vector<float> > bvh2DPoints;
 int yawValue,pitchValue,rollValue;
 int selectedJoint,previousSelection;
 int stop;
 int save;
 int redraw;
};


int controls(cv::Mat & controlMat,struct applicationState * state)
{
  cv::imshow("3D Control",controlMat);
  cv::createTrackbar("Stop Demo", "3D Control", &state->stop, 1);
  cv::createTrackbar("Save", "3D Control", &state->save, 1);
  cv::createTrackbar("Selected Joint", "3D Control", &state->selectedJoint,MOCAPNET_OUTPUT_NUMBER);
  cv::createTrackbar("Yaw            ", "3D Control", &state->yawValue,  360);
  cv::createTrackbar("Pitch          ", "3D Control", &state->pitchValue,360);
  cv::createTrackbar("Roll            ", "3D Control", &state->rollValue, 360);
}






int main(int argc, char *argv[])
{
    cv::Mat controlMat = Mat(Size(300,2),CV_8UC3, Scalar(0,0,0));
    cv::Mat viewMat = Mat(Size(1024,768),CV_8UC3, Scalar(0,0,0));
    struct applicationState state;
    state.yawValue=0;
    state.pitchValue=0;
    state.rollValue=0;
    state.selectedJoint=0;
    state.stop=0;
    state.save=0;
    state.redraw=1;

    Mat edges;
    namedWindow("BVH",1);
    controls(controlMat,&state);

    for (int i=0; i<MOCAPNET_OUTPUT_NUMBER; i++)
     {
       state.bvhConfiguration.push_back(0.0);
     }

    state.bvhConfiguration[2]=(float) -200.0;

    initializeBVHConverter();

    while(!state.stop)
        {
          if (state.save)
          {
            std::vector<std::vector<float> > bvhFrames;
            bvhFrames.push_back(state.bvhConfiguration);
            state.save=0;
            controls(controlMat,&state);
            writeBVHFile(
                         "out.bvh",
                         bvhHeader,
                         0,// int prependTPose,
                         bvhFrames
                        );
          }

          if (state.selectedJoint!=state.previousSelection)
          {
            state.previousSelection=state.selectedJoint;
            state.yawValue=state.bvhConfiguration[state.selectedJoint];
            controls(controlMat,&state);
            state.redraw=1;
          }

          if ( state.bvhConfiguration[state.selectedJoint]!=state.yawValue )
            {
              state.bvhConfiguration[state.selectedJoint]=state.yawValue;
              state.redraw=1;
            }

           if (state.redraw)
           {
            memset(viewMat.data,0,1024*768*3*sizeof(char));
            cv::putText(viewMat,MocapNETOutputArrayNames[state.previousSelection], cv::Point(0,30), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );
            if (state.bvhConfiguration.size()>0)
              {
               state.bvh2DPoints = convertBVHFrameTo2DPoints(
                                                             state.bvhConfiguration,
                                                             1024,
                                                             768
                                                             );
               }
            drawSkeleton(viewMat,state.bvh2DPoints,0,0);
            state.redraw=0;
           }


           imshow("BVH", viewMat);

           waitKey(150);
        }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
