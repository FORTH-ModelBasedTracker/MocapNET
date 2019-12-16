#include "opencv2/opencv.hpp"
/** @file bvhgui.cpp
 *  @brief This is a simple test file to play with the BVH armature
 *  #Hands up
 *  // ./BVHGUI --set 241 290.0 --set 242 80 --set 284 117 --set 283 113
 *  #Close up hands / face
 *  // ./BVHGUI --set 1 -70 --set 2 -60 --set 241 -80.0 --set 242 80 --set 284 117 --set 283 113
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
 int rotation;
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
  cv::createTrackbar("Rotation            ", "3D Control", &state->rotation,  360); 
}






int main(int argc, char *argv[])
{
    cv::Mat controlMat = Mat(Size(300,2),CV_8UC3, Scalar(0,0,0));
    cv::Mat viewMat = Mat(Size(1024,768),CV_8UC3, Scalar(0,0,0));
    struct applicationState state;
    state.rotation=0; 
    state.selectedJoint=0;
    state.stop=0;
    state.save=0;
    state.redraw=1;



    for (int i=0; i<MOCAPNET_OUTPUT_NUMBER; i++)
     {
       state.bvhConfiguration.push_back(0.0);
     } 
    state.bvhConfiguration[2]=(float) -150.0;
    
    for (int i=0; i<argc; i++)
        { 
            if (strcmp(argv[i],"--set")==0)
                {
                    if(argc>i+2)
                    { 
                     unsigned int bvhJoint = atoi(argv[i+1]);
                     float bvhValue = atof(argv[i+2]);  
                     if (bvhJoint<MOCAPNET_OUTPUT_NUMBER) 
                        {
                           fprintf(stderr,"joint[%s]=%0.2f",MocapNETOutputArrayNames[bvhJoint],bvhValue); 
                           state.bvhConfiguration[bvhJoint]=bvhValue; 
                        } 
                    }
                }
        }

    Mat edges;
    namedWindow("BVH",1);
    controls(controlMat,&state);
    cv::moveWindow("3D Control",0,0); //y=inputHeight2DJointDetector
    cv::moveWindow("BVH",300,0);
    

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
            state.rotation=state.bvhConfiguration[state.selectedJoint];
            controls(controlMat,&state);
            state.redraw=1;
          }

          if ( state.bvhConfiguration[state.selectedJoint]!=state.rotation )
            {
              state.bvhConfiguration[state.selectedJoint]=state.rotation;
              state.redraw=1;
            }

           if (state.redraw)
           {
            memset(viewMat.data,0,1024*768*3*sizeof(char));
            cv::putText(viewMat,MocapNETOutputArrayNames[state.previousSelection], cv::Point(10,30), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );
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
