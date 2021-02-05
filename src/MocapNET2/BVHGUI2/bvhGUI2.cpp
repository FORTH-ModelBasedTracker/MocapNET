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
#include "../MocapNETLib2/mocapnet2.hpp"
#include "../MocapNETLib2/IO/bvh.hpp"
#include "../MocapNETLib2/visualization/visualization.hpp"
#include "../MocapNETLib2/visualization/opengl.hpp"
#include <vector>

#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_rename.h"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


using namespace cv;

int visualizeOpenGLEnabled=0;

struct applicationState
{
    std::vector<float>bvhConfiguration;
    std::vector<std::vector<float> > bvh2DPoints;
    unsigned int numberOfBVHMotionValuesPerFrame;
    int previousDistance;
    int distance;
    int positiveRotation;
    int negativeRotation;
    int previousPositiveRotation;
    int previousNegativeRotation;
    int rotation;
    int selectedMotionChannel,previousSelection;
    int visualizationType,previousVisualizationType;
    int play;
    int stop;
    int save; 
    int copyClipboard;
    int frameID;
    int numberOfFrames;
    int redraw;
};


int controls(cv::Mat & controlMat,struct applicationState * state)
{
    cv::imshow("3D Control",controlMat);
    cv::createTrackbar("Stop Demo", "3D Control", &state->stop, 1);

    cv::createTrackbar("Play", "3D Control", &state->play,1);
    cv::createTrackbar("Frame", "3D Control", &state->frameID,state->numberOfFrames);
    cv::createTrackbar("Clipboard Pose", "3D Control", &state->copyClipboard, 1);

    if (visualizeOpenGLEnabled)
        {
            cv::createTrackbar("Visualization Type", "3D Control", &state->visualizationType, 1);
        }

    cv::createTrackbar("Save", "3D Control", &state->save, 1); 
    
    cv::createTrackbar("Distance            ", "3D Control", &state->distance,  1360);

    cv::createTrackbar("Selected Motion Channel", "3D Control", &state->selectedMotionChannel, state->numberOfBVHMotionValuesPerFrame-1);
    cv::createTrackbar("Positive Value ", "3D Control", &state->positiveRotation,  360);
    cv::createTrackbar("Negative Value ", "3D Control", &state->negativeRotation,  360);

    return 1;
}



static void onMouse( int event, int x, int y, int, void* stateUncast)
{
    struct applicationState * state = (struct applicationState *) stateUncast;
    if( event != EVENT_LBUTTONDOWN )
        return;

    if (visualizeOpenGLEnabled)
        {
            clickOpenGLView(x,y,event);
        }
    else
        {
            fprintf(stderr,"Unhandled click @ %u,%u \n",x,y);
        }
}




int main(int argc, char *argv[])
{
    unsigned int visWidth=1920;
    unsigned int visHeight=1080;

    struct applicationState state;
    state.numberOfBVHMotionValuesPerFrame=MOCAPNET_OUTPUT_NUMBER;
    state.rotation=0;
    state.previousDistance=0;
    state.distance=150;
    state.numberOfFrames=1;
    state.previousSelection=0;
    state.selectedMotionChannel=0;
    state.visualizationType=0;
    state.stop=0;
    state.save=0;
    state.copyClipboard=0;
    state.previousVisualizationType=2;// <- force redraw on first loop
    state.redraw=1;

    int useBVH=0;
    int forcePosition=1; //By default we want the output to be centered..
    int forceRotation=0;
    float forcedRotation=0;

    const char * bvhFilename=0;

    struct BVH_MotionCapture bvhMotion= {0};


    for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--distance")==0)
                {
                    if(argc>i+1)
                        {
                            state.distance=atoi(argv[i+1]);
                        }
                }
            else if (strcmp(argv[i],"--size")==0)
                {
                    if(argc>i+2)
                        {
                            visWidth=atoi(argv[i+1]);
                            visHeight=atoi(argv[i+2]);
                        }
                }
             else if (strcmp(argv[i],"--unconstrained")==0)
                { 
                    forcePosition=0; 
                    forceRotation=0;  
                }
            else if (strcmp(argv[i],"--rotation")==0)
                {
                    if(argc>i+1)
                        {
                            forceRotation=1;
                            forcedRotation=atof(argv[i+1]);
                            state.selectedMotionChannel=4;
                            state.rotation=atoi(argv[i+1]);
                        }
                }
            else if (strcmp(argv[i],"--from")==0)
                {
                    if(argc>i+1)
                        {
                            if ( bvh_loadBVH(argv[i+1], &bvhMotion, 1.0) )
                                {
                                    bvhFilename=argv[i+1];
                                    bvh_renameJointsForCompatibility(&bvhMotion);
                                    state.frameID=0;
                                    state.numberOfFrames=bvhMotion.numberOfFrames;
                                    state.play=1;
                                    state.numberOfBVHMotionValuesPerFrame=bvhMotion.numberOfValuesPerFrame;
                                    useBVH=1;
                                }
                            else
                                {
                                    fprintf(stderr,"File `%s` does not exist\n",argv[i+1]);
                                    return 1;
                                }
                        }
                }
            else

                if (strcmp(argv[i],"--opengl")==0)
                    {
                        visualizeOpenGLEnabled=1;
                        state.visualizationType=1;
                        state.previousVisualizationType=state.visualizationType;
                        state.redraw=3; //Draw 3 consequtive frames to take care of OGL initialization
                        state.previousSelection=6;
                        state.selectedMotionChannel=6;
                    }
        }


    if ( !initializeBVHConverter(bvhFilename,visWidth,visHeight,1) )
        {
            fprintf(stderr,"Failed to initialize BVH code from %s ..\n",bvhFilename);
            return 0;
        }

    if (getBVHNumberOfValuesPerFrame() != state.numberOfBVHMotionValuesPerFrame)
        {
            fprintf(stderr,"Inconsistency, internal BVH state reports different number of frames compared to external BVH state..\n");
            fprintf(stderr,"Internal(%u) External(%u) ..\n",getBVHNumberOfValuesPerFrame(),state.numberOfBVHMotionValuesPerFrame);
            fprintf(stderr,"Cannot continue ..\n");
            return 0;
        }

    state.bvhConfiguration.clear();
    for (int i=0; i<state.numberOfBVHMotionValuesPerFrame; i++)
        {
            state.bvhConfiguration.push_back(0.0);
        }
    state.bvhConfiguration[2]=(float) -1 * state.distance;

    fprintf(stderr,"Our bvh configuration will be held in vectors of %lu elements\n",state.bvhConfiguration.size());

    fprintf(stderr,"Initializing OpenCV Visualization code ... ");
    cv::Mat viewMat = Mat(Size(visWidth,visHeight),CV_8UC3, Scalar(0,0,0));
    cv::imshow("BVH", viewMat);
    cv::moveWindow("BVH",300,0);
    setMouseCallback("BVH", onMouse, (void*) &state );


    cv::Mat controlMat = Mat(Size(300,1),CV_8UC3, Scalar(0,0,0));
    controls(controlMat,&state);
    cv::moveWindow("3D Control",0,0); //y=inputHeight2DJointDetector

    fprintf(stderr,"ok!\n");


    for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--changeJointDimensions")==0)
                {
                    if(argc>i+7)
                        {
                            changeJointDimensions(
                                atof(argv[i+1]),
                                atof(argv[i+2]),
                                atof(argv[i+3]),
                                atof(argv[i+4]),
                                atof(argv[i+5]),
                                atof(argv[i+6]),
                                atof(argv[i+7]),
                                atof(argv[i+8]),
                                atof(argv[i+9])
                            );
                        }
                    else
                        {
                            fprintf(stderr,"Incorrect number of parameters given..\n");
                            return 0;
                        }
                }
            else if (strcmp(argv[i],"--set")==0)
                {
                    // To focus only on head..
                    //./BVHGUI2 --set 1 -65 --set 2 -25
                    // In OpenGL mode
                    //./BVHGUI2 --set 1 -14.8 --set 2 2 --opengl
                    // To focus on RHand
                    //  ./BVHGUI2 --from dataset/rhand.bvh --set 2 -45 --set 3 -90 --set 5 -90
                    if(argc>i+2)
                        {
                            unsigned int bvhMotionID = atoi(argv[i+1]);
                            float bvhMotionValue = atof(argv[i+2]);

                            unsigned int maximumValidMotionID = getBVHNumberOfValuesPerFrame();
                            if (useBVH)
                                {
                                    maximumValidMotionID = bvhMotion.jointHierarchySize;
                                }

                            if (bvhMotionID<maximumValidMotionID)
                                {
                                    char name[512]= {0};


                                    if (useBVH)
                                        {
                                            bvh_getMotionChannelName(&bvhMotion,bvhMotionID,name,512);
                                            bvhMotion.motionValues[bvhMotionID]=bvhMotionValue;
                                        }
                                    else
                                        {
                                            getBVHMotionValueName(bvhMotionID,name,512);
                                        }

                                    fprintf(stderr,YELLOW "joint[%s]=%0.2f\n" NORMAL,name,bvhMotionValue);
                                    state.bvhConfiguration[bvhMotionID]=bvhMotionValue;
                                    if (bvhMotionID==2)
                                        {
                                            state.distance=-1*bvhMotionValue;
                                        }
                                }
                            else
                                {
                                    fprintf(stderr,RED "Set command jID out of bounds ( requested %u , limit is %u )\n" NORMAL,bvhMotionID,maximumValidMotionID);
                                }
                        }
                    else
                        {
                            fprintf(stderr,RED "Not enough arguments for set command\n" NORMAL);
                        }
                }
        }

    while(!state.stop)
        {
            if (useBVH)
                {
                    unsigned int mIDStart=state.frameID * bvhMotion.numberOfValuesPerFrame;

                    if (state.bvhConfiguration.size() != state.numberOfBVHMotionValuesPerFrame)
                        {
                            fprintf(stderr,"Inconsistency in number of vector allocated and motion values loaded..\n");
                        }
                    else if (bvhMotion.numberOfFrames * bvhMotion.numberOfValuesPerFrame != bvhMotion.motionValuesSize)
                        {
                            fprintf(stderr,"Inconsistency in number of frames and motion values..\n");
                        }
                    else if (mIDStart<bvhMotion.motionValuesSize)
                        {
                            for (unsigned int i=0; i<state.numberOfBVHMotionValuesPerFrame; i++)
                                {
                                    state.bvhConfiguration[i]=bvhMotion.motionValues[(mIDStart+i)%bvhMotion.motionValuesSize];
                                }
                                //----------------------------------------------------------------------------------------
                                if (forcePosition)
                                {
                                  state.bvhConfiguration[0]=0;
                                  state.bvhConfiguration[1]=0;
                                 state.bvhConfiguration[2]=(float) -1 * state.distance;
                                }
                                //----------------------------------------------------------------------------------------
                            if (forceRotation)
                                {
                                    state.bvhConfiguration[3]=0;
                                    state.bvhConfiguration[4]=forcedRotation;
                                    state.bvhConfiguration[5]=0;
                                }
                                //----------------------------------------------------------------------------------------
                            if (!state.redraw)
                                {
                                    state.redraw=1;
                                }
                        }
                    else
                        {
                            fprintf(stderr,"Frame %u exceeds allocated number of motion values\n",state.frameID);
                        }
                }

            if (state.copyClipboard)
            {
                char valueStr[10];
                char motionVectorStringUnsafe[10000]={0};
                snprintf(motionVectorStringUnsafe,10000,"echo \"%0.2f",state.bvhConfiguration[0]);
                
                for (unsigned int i=1; i<state.numberOfBVHMotionValuesPerFrame; i++)
                                { 
                                    snprintf(valueStr,10,",%0.2f",state.bvhConfiguration[i]);
                                    strcat(motionVectorStringUnsafe,valueStr);
                                }
                strcat(motionVectorStringUnsafe,"\" | xclip -selection clipboard");
                int i=system(motionVectorStringUnsafe);
                if (i!=0)
                {
                    fprintf(stderr,"Failed copying to clipboard.. :( \n");
                }
                state.copyClipboard=0;
            }

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
                    fprintf(stderr,GREEN "Saved..\n" NORMAL);
                }
                 

            if (state.previousDistance!=state.distance)
                {
                    if (!state.redraw)
                        {
                            state.redraw=1;
                        }
                    state.previousDistance=state.distance;
                    state.bvhConfiguration[2]=(float) -1 * state.distance;
                }

            //if (!state.play)
            //{

            if (state.visualizationType!=state.previousVisualizationType)
                {
                    if (!state.redraw)
                        {
                            state.redraw=1;
                        }
                    state.previousVisualizationType=state.visualizationType;
                }

            if (state.selectedMotionChannel!=state.previousSelection)
                {
                    if (!state.redraw)
                        {
                            state.redraw=1;
                        }
                    state.previousSelection=state.selectedMotionChannel;
                    state.rotation=state.bvhConfiguration[state.selectedMotionChannel];
                    if (state.rotation<0)
                        {
                            state.negativeRotation=-1*state.rotation;
                        }
                    else
                        {
                            state.positiveRotation=state.rotation;
                        }
                    controls(controlMat,&state);
                }


            if (state.previousPositiveRotation!=state.positiveRotation)
                {
                    state.previousPositiveRotation=state.positiveRotation;
                    state.rotation=state.positiveRotation;
                }
            else if (state.previousNegativeRotation!=state.negativeRotation)
                {
                    state.previousNegativeRotation=state.negativeRotation;
                    state.rotation=-1* state.negativeRotation;
                }

            if ( state.bvhConfiguration[state.selectedMotionChannel]!=state.rotation )
                {
                    if (!state.redraw)
                        {
                            state.redraw=1;
                        }
                    state.bvhConfiguration[state.selectedMotionChannel]=state.rotation;
                    if (state.rotation<0)
                        {
                            state.negativeRotation=-1*state.rotation;
                        }
                    else
                        {
                            state.positiveRotation=state.rotation;
                        }
                }
            // }

            if (state.redraw)
                {
                    //fprintf(stderr,"redraw..\n");
                    memset(viewMat.data,0,visWidth*visHeight*3*sizeof(char));


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
                                    //fprintf(stderr,"Converting %lu bvh vector\n",state.bvhConfiguration.size());
                                    state.bvh2DPoints = convertBVHFrameTo2DPoints(state.bvhConfiguration);//visWidth,visHeight);

                                    drawSkeleton(viewMat,state.bvh2DPoints,0,0,1);
                                }
                        }


                    char message[1024];
                    snprintf(message,1024,"Frame  : %u / %u",state.frameID,state.numberOfFrames);
                    cv::putText(viewMat,message, cv::Point(10,30), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );


                    if (useBVH)
                        {
                            char motionChannelName[512];
                            getBVHMotionValueName(state.selectedMotionChannel,motionChannelName,512);
                            snprintf(message,1024,"Selected motion channel #%u : %s ( value %0.2f )",state.selectedMotionChannel,motionChannelName,state.bvhConfiguration[state.selectedMotionChannel]);
                        }
                    else
                        {
                            snprintf(message,1024,"Selected motion channel #%u : %s ( value %0.2f )",state.selectedMotionChannel,MocapNETOutputArrayNames[state.previousSelection],state.bvhConfiguration[state.selectedMotionChannel]);
                        }
                    cv::putText(viewMat,message, cv::Point(10,50), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );

                    if (visualizeOpenGLEnabled)
                        {
                            snprintf(message,1024,"OpenGL Model motion channel name : %s",OpenCOLLADANames[state.previousSelection/3]);
                            cv::putText(viewMat,message, cv::Point(10,70), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );
                        }

                    if (state.redraw>=1)
                        {
                            --state.redraw;
                        }
                    //state.redraw=0;
                }


            imshow("BVH", viewMat);

            waitKey(15);

            if (useBVH)
                {
                    if (state.play)
                        {
                            ++state.frameID;
                            state.frameID = state.frameID % state.numberOfFrames;
                        }
                }
        }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
