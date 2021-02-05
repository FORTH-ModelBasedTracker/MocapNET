#include "opencv2/opencv.hpp"
/** @file bvhgui.cpp
 *  @brief This is a simple test file to play with the BVH armature
 *  #Hands up
 *  // ./BVHGUI --set 241 290.0 --set 242 80 --set 284 117 --set 283 113
 *  #Close up hands / face
 *  // ./BVHGUI --set 1 -70 --set 2 -60 --set 241 290.0 --set 242 80 --set 282 187 --set 283 252 --set 284 112
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include <stdio.h>
#include "../../MocapNET2/MocapNETLib2/mocapnet2.hpp"
#include "../../MocapNET2/MocapNETLib2/IO/bvh.hpp"
#include "../../MocapNET2/MocapNETLib2/visualization/visualization.hpp"
#include "../../MocapNET2/MocapNETLib2/visualization/opengl.hpp"
#include <vector>

using namespace cv;

int visualizeOpenGLEnabled=0;
    
struct applicationState
{
 std::vector<float>bvhConfiguration;
 std::vector<std::vector<float> > bvh2DPoints;
 int rotation;
 int selectedJoint,previousSelection;
 int visualizationType,previousVisualizationType;
 int stop;
 int save;
 int redraw;
};


int controls(cv::Mat & controlMat,struct applicationState * state)
{
  cv::imshow("3D Control",controlMat);
  cv::createTrackbar("Stop Demo", "3D Control", &state->stop, 1);
  if (visualizeOpenGLEnabled)
    { cv::createTrackbar("Visualization Type", "3D Control", &state->visualizationType, 1); }
  cv::createTrackbar("Save", "3D Control", &state->save, 1);
  cv::createTrackbar("Selected Joint", "3D Control", &state->selectedJoint,MOCAPNET_OUTPUT_NUMBER);
  cv::createTrackbar("Value            ", "3D Control", &state->rotation,  360); 
  return 1;
}






int main(int argc, char *argv[])
{
    unsigned int visWidth=1024;
    unsigned int visHeight=768;
    
    cv::Mat controlMat = Mat(Size(300,1),CV_8UC3, Scalar(0,0,0));
    memset(controlMat.data,255,300*1*3*sizeof(char));
    cv::Mat viewMat = Mat(Size(visWidth,visHeight),CV_8UC3, Scalar(0,0,0));
    struct applicationState state;
    state.rotation=0; 
    state.previousSelection=0;
    state.selectedJoint=0;
    state.visualizationType=0;
    state.stop=0;
    state.save=0;
    state.previousVisualizationType=2;// <- force redraw on first loop
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
                } else 
            if (strcmp(argv[i],"--opengl")==0)
                    {
                        visualizeOpenGLEnabled=1;
                        state.visualizationType=1; 
                        state.previousVisualizationType=state.visualizationType;
                    }
        }

    Mat edges;
    namedWindow("BVH",1);
    controls(controlMat,&state);
    cv::moveWindow("3D Control",0,0); //y=inputHeight2DJointDetector
    cv::moveWindow("BVH",300,0);
    

    initializeBVHConverter(0,visWidth,visHeight,1);

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

          if (state.visualizationType!=state.previousVisualizationType)
          {
            state.redraw=1; 
            state.previousVisualizationType=state.visualizationType;
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
            
            
            //cv::Mat * openGLMatForVisualization = 0;
            if ( (visualizeOpenGLEnabled) && (state.visualizationType) )
                  {
                   //fprintf(stderr,"updateOpenGLView\n");
                   updateOpenGLView(state.bvhConfiguration);

                   //fprintf(stderr,"visualizeOpenGL\n");
                   unsigned int openGLFrameWidth=visWidth,openGLFrameHeight=visHeight;
                   char * openGLFrame = visualizeOpenGL(&openGLFrameWidth,&openGLFrameHeight);
                   //=====================================================================
                   if (openGLFrame!=0)
                       {
                        fprintf(stderr,"Got Back an OpenGL frame..!\n");
                        cv::Mat openGLMat(openGLFrameHeight, openGLFrameWidth, CV_8UC3);
                        unsigned char *   initialPointer = openGLMat.data;
                        openGLMat.data=(unsigned char * ) openGLFrame;
                        cv::cvtColor(openGLMat,viewMat,COLOR_RGB2BGR);
                        openGLMat.data=initialPointer;
                       }//=====================================================================
                        else
                       {
                         fprintf(stderr,"Failed getting an OpenGL frame..!\n");
                       }
                    } 
                      else
                    {
                      if (state.bvhConfiguration.size()>0)
                         {
                            state.bvh2DPoints = convertBVHFrameTo2DPoints(state.bvhConfiguration); //visWidth,visHeight );
                         }
                       drawSkeleton(viewMat,state.bvh2DPoints,0,0,1);   
                    }      
            
            
            cv::putText(viewMat,MocapNETOutputArrayNames[state.previousSelection], cv::Point(10,30), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );
            
            state.redraw=0;
           }


           imshow("BVH", viewMat);

           waitKey(150);
        }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
