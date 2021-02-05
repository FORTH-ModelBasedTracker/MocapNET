#include "opencv2/opencv.hpp"
/** @file csvClusterPlot.cpp
 *  @brief
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include <stdio.h>

#include <sys/stat.h> //mkdir
#include "../MocapNETLib2/mocapnet2.hpp"
#include "../MocapNETLib2/IO/bvh.hpp"
#include "../MocapNETLib2/visualization/visualization.hpp"
#include "../MocapNETLib2/visualization/opengl.hpp"
#include "../MocapNETLib2/IO/csvRead.hpp"
#include <vector>

#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_filter.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_rename.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_randomize.h"

#include "perform2DClustering.hpp"
#include "perform3DClustering.hpp"


//#include <stdint.h>

using namespace cv;

int visualizeOpenGLEnabled=0;


#define BOOST_CONTRAST 2.0



char MocapNET1List[]="src/MocapNET2/CSVClusterPlot/listOfFilesToClusterMocapNET1";
char defaultList[]="src/MocapNET2/CSVClusterPlot/listOfFilesToCluster";


const char * perturbList1[] =
{
    " ",
    "rshoulder",
    "lshoulder",
    "rhip",
    "lhip"
};

const char * perturbList2[] =
{
    " ",
    "rhand",
    "relbow",
    "lelbow",
    "lhand",
    "lknee",
    "rknee",
    "lfoot",
    "rfoot"
};


const char * perturbList3[] =
{
    " ",
    "abdomen",
    "chest"
};

/*
#define rotationSetting 0"
#define handToCloseHipMin "45"
#define handToCloseHipMax "90"
#define handToFarHipMin "100"
#define handToFarHipMax "160"
#define handToHandMin "150"
#define handToHandMax "250"
*/


#define rotationSetting "90"
#define handToCloseHipMin "0"
#define handToCloseHipMax "120"
#define handToFarHipMin "0"
#define handToFarHipMax "120"
#define handToHandMin "0"
#define handToHandMax "150"


const char * removeNeutralPosesRules[] =
{
    "0", // 0 | Move Armature to 0 X Position
    "0", // 1 | Move Armature to 0 Y Position
    "-130.0", // 2 | Move Armature to -130.0 Z Position
    "0", // 3 | Rotate Armature to 0deg X Rotation
    rotationSetting, // 4 | Rotate Armature to 0deg Y Rotation
    "0", // 5 | Rotate Armature to 0deg Z Rotation
    //--------------------------------------------------------------------------
    "1920", // 6 |  Virtual 2D Rendering Width
    "1080",// 7 |  Virtual 2D Rendering Height
    "570.7",  // 8 |  Virtual 2D Rendering Focal Length X
    "570.3", // 9 |  Virtual 2D Rendering Focal Length Y
    //--------------------------------------------------------------------------
    "6",         // 10 |  Number of joint rules
    //--------------------------------------------------------------------------
    "rhand", // 11 | Joint A
    "lhip",   // 12 | Joint B
    handToFarHipMin,  // 13 | Minimum Acceptable 2D Distance
    handToFarHipMax, // 14 | Maximum Acceptable 2D Distance
    "rhand", // 15 | Joint A
    "rhip",    // 16| Joint B
    handToCloseHipMin,      // 17 | Minimum Acceptable 2D Distance
    handToCloseHipMax,     // 18 | Maximum Acceptable 2D Distance
    "rhand", // 19 | Joint A
    "lhand", // 20 | Joint B
    handToHandMin,    // 21 | Minimum Acceptable 2D Distance
    handToHandMax,   // 22 | Maximum Acceptable 2D Distance
    //--------------------------------------------------------------------------
    "lhand", // 23 | Joint A
    "rhip",   // 24| Joint B
    handToFarHipMin,  // 25 | Minimum Acceptable 2D Distance
    handToFarHipMax, // 26 | Maximum Acceptable 2D Distance
    "lhand", // 27 | Joint A
    "lhip",    // 28| Joint B
    handToCloseHipMin,      // 29 | Minimum Acceptable 2D Distance
    handToCloseHipMax,     // 30 | Maximum Acceptable 2D Distance
    "lhand", // 31 | Joint A
    "rhand", // 32 | Joint B
    handToHandMin,    // 33 | Minimum Acceptable 2D Distance
    handToHandMax   // 34 | Maximum Acceptable 2D Distance
};


void convertHeatmapValueToRGB(float heatmap,unsigned char * R,unsigned  char * G,unsigned char  * B)
{
    float r = fminf(1.0f, (fmaxf(0.5f, heatmap) - 0.5f) * 6.0f);
    float g = fminf(1.0f, heatmap * 3.0f) - fminf(1.0f, fmaxf(0.0f, heatmap - 0.666f) * 3.0f);
    float b = fmaxf(0.0f, 1.0f - fmaxf(0.0f, heatmap - 0.333f) * 6.0f);

    if (heatmap==0.0)
        {
            r=0.0;
            g=0.0;
            b=0.0;
        }
    else if (heatmap<0.1)
        {
            r=0.0;
            g=0.0;
            b=0.3+10*heatmap;
            if (b>=1.0)
                {
                    b=1.0;
                }
        }
    else if (heatmap>=1.0) //Cap
        {
            if (heatmap>=2.0)
                {
                    r=1.0;
                    g=0.0;
                    b=1.0; //Final Limit
                }
            else
                {
                    r=1.0 ;
                    g=0.0;
                    b=heatmap-1.0;
                }
        }

    r=r*255.0;
    g=g*255.0;
    b=b*255.0;

    *R =   (unsigned char) r;
    *G  =  (unsigned char) g;
    *B  =  (unsigned char) b;
}



int draw2DClustering(cv::Mat &viewMat,struct BVH_MotionCapture * bvhMotion,std::vector<std::vector<float> > aPose,struct applicationState * state,const char * line, char * videoPath,unsigned int width,unsigned int height)
{
    char buffer[1024]= {0};
    memset(viewMat.data,0,width*height*3*sizeof(char));
    /*
                                        if ( state->maxAccumulatedSample > state->accumulatedSamples )
                                            {
                                                state->maxAccumulatedSample=state->accumulatedSamples;
                                            }*/

    unsigned char R,G,B;
    //------------------------------------------------------------------------------
    for (int y=0; y<height; y++)
        {
            for (int x=0; x<width; x++)
                {
                    float heatmap = (float) state->accumulatedImage[y*width+x] / state->maxAccumulatedSample;
                    heatmap*=BOOST_CONTRAST;

                    convertHeatmapValueToRGB(heatmap,&R,&G,&B);

                    unsigned int pos=(y*width*3) + x*3;
                    viewMat.data[pos++]=B;
                    viewMat.data[pos++]=G;
                    viewMat.data[pos]=R;
                }


            //add a sidebar with the colors legend..
            for (int x=width-5; x<width; x++)
                {
                    float heatmapScaleValue = 1.0 - ((float) y / height);
                    heatmapScaleValue*=BOOST_CONTRAST; //;

                    convertHeatmapValueToRGB(heatmapScaleValue,&R,&G,&B);

                    unsigned int pos=(y*width*3) + x*3;
                    viewMat.data[pos++]=B;
                    viewMat.data[pos++]=G;
                    viewMat.data[pos]=R;
                }


            if (y%50==0)
                {
                    float heatmapScaleValue = 1.0 - ((float) y / height);
                    heatmapScaleValue*=state->maxAccumulatedSample;
                    snprintf(buffer,1024,"%0.0f",heatmapScaleValue);
                    cv::putText(viewMat,buffer, cv::Point(width-50,y), cv::FONT_HERSHEY_DUPLEX, 0.3, cv::Scalar::all(255), 0.2, 8 );
                }
        }

    drawSkeleton(viewMat,aPose,0,0,1);

    snprintf(buffer,1024,"%lu accumulated samples, max heatmap value = %lu",state->accumulatedSamples,state->maxAccumulatedSample);
    cv::putText(viewMat,buffer, cv::Point(10,30), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );

    snprintf(buffer,1024,"%s added with %u samples",line,bvhMotion->numberOfFrames);
    cv::putText(viewMat,buffer, cv::Point(10,50), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );

    if (videoPath!=0)
        {
            snprintf(buffer,1024,"Label : %s ",videoPath);
            cv::putText(viewMat,buffer, cv::Point(10,70), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar::all(255), 0.2, 8 );
        }
 return 1;
}




int draw3DClustering(cv::Mat &viewMat,struct clusteringOf3DPoses * state,unsigned int width,unsigned int height,unsigned int depth)
{
    memset(viewMat.data,0,width*height*3*sizeof(char));

    unsigned char R,G,B;
    //------------------------------------------------------------------------------
    for (int y=0; y<height; y++)
        {
            for (int x=0; x<width; x++)
                {
                    unsigned int hits=0;
                    for (int z=0; z<depth; z++)
                        {
                            unsigned long memoryLocation = (unsigned long) (z*width*height) + (y*width)+ x;
                                    
                            hits+=state->space[memoryLocation].element;
                        }
                    float heatmap = (float) hits / (depth*255);
                    heatmap*=10;

                    convertHeatmapValueToRGB(heatmap,&R,&G,&B);

                    unsigned int pos=(y*width*3) + x*3;
                    viewMat.data[pos++]=B;
                    viewMat.data[pos++]=G;
                    viewMat.data[pos]=R;
                }

        }
        
        return 1;
}



int main(int argc, char *argv[])
{
    struct clusteringOf3DPoses * state3D = (struct clusteringOf3DPoses *) malloc(sizeof(struct clusteringOf3DPoses) );
    if (state3D==0)
        {
            fprintf(stderr,"Could not allocate enough memory to start csvClusterPlot..\n");
            return 1;
        }
    memset(state3D,0,sizeof(struct clusteringOf3DPoses));


    struct applicationState * state = (struct applicationState *) malloc(sizeof(struct applicationState) );
    if (state==0)
        {
            fprintf(stderr,"Could not allocate enough memory to start csvClusterPlot..\n");
            free(state3D);
            return 1;
        }
    state->rotation=0;
    state->previousSelection=0;
    state->selectedJoint=0;
    state->visualizationType=0;
    state->stop=0;
    state->save=0;
    state->previousVisualizationType=2;// <- force redraw on first loop
    state->redraw=1;
    state->maxAccumulatedSample= 0;
    int randomize=0;
    int filterPoses=0;
    int go3DMode=0;

    unsigned int visualizationFrameNumber=0;
    int saveVideo=0;
    char * videoPath=0;
    char videoFrameFinalFilename[512]= {0};

    float skeletonDistance=-160;
    unsigned int WIDTH = 1000;
    unsigned int HEIGHT = 1000;
    unsigned int DEPTH = 1000;
    char * selectedSource = defaultList;

    for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--mocapnet1")==0)
                {
                    selectedSource=MocapNET1List;
                }
            else if (strcmp(argv[i],"--saveVideo")==0)
                {
                    if(argc>i+1)
                        {
                            videoPath=argv[i+1];
                            saveVideo=1;
                            mkdir(videoPath,0777);
                        }
                }
            else if (strcmp(argv[i],"--from")==0)
                {
                    if(argc>i+1)
                        {
                            selectedSource=argv[i+1];
                        }
                }
            else if (strcmp(argv[i],"--rotation")==0)
                {
                    if(argc>i+1)
                        {
                            state->rotation=atoi(argv[i+1]);
                        }
                }
            else if (strcmp(argv[i],"--randomize")==0)
                {
                    randomize=1;
                }
            else if (strcmp(argv[i],"--filter")==0)
                {
                    filterPoses=1;
                }
            else if (strcmp(argv[i],"--size")==0)
                {
                    //To produce higher resolution heatmap..
                    //   ./CSVClusterPlot --rotation 90 --onlyrhand --size 1920 1080 --distance -130 --filter
                    if(argc>i+2)
                        {
                            WIDTH=atoi(argv[i+1]);
                            HEIGHT=atoi(argv[i+2]);
                        }
                }
            else if (strcmp(argv[i],"--depth")==0)
                {
                    if(argc>i+1)
                        {
                            DEPTH=atoi(argv[i+1]);
                        }
                }
            else if (strcmp(argv[i],"--distance")==0)
                {
                    if(argc>i+1)
                        {
                            skeletonDistance=atof(argv[i+1]);
                        }
                }
        }


    state->accumulatedImage = (unsigned long * ) malloc( (WIDTH) * (HEIGHT) * sizeof (unsigned long) );
    if (state->accumulatedImage==0)
        {
            fprintf(stderr,"Could not allocate enough memory to start csvClusterPlot..\n");
            free(state);
            free(state3D);
            return 1;
        }
    for (int y=0; y<HEIGHT; y++)
        {
            for (int x=0; x<WIDTH; x++)
                {
                    state->accumulatedImage[y*WIDTH+x]=0;
                }
        }
    state->accumulatedSamples=0;;


    cv::Mat viewMat = Mat(Size(WIDTH,HEIGHT),CV_8UC3, Scalar(0,0,0));
    namedWindow("BVH",1);
    //controls(controlMat,&state);
    //cv::moveWindow("3D Control",0,0); //y=inputHeight2DJointDetector
    cv::moveWindow("BVH",0,0);


    initializeBVHConverter(0,WIDTH,HEIGHT,1);



    //------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------
    std::vector<char>   activeJoints;
    activeJoints.clear();
    for (int i=0; i<MOCAPNET_OUTPUT_NUMBER; i++)
        {
            activeJoints.push_back(1);
        }
    activeJoints[ getBVHJointIDFromJointName("hip")]=0;
    activeJoints[ getBVHJointIDFromJointName("lThigh")]=0;
    activeJoints[ getBVHJointIDFromJointName("rThigh")]=0;
    activeJoints[ getBVHJointIDFromJointName("lButtock")]=0;
    activeJoints[ getBVHJointIDFromJointName("rButtock")]=0;
    activeJoints[ getBVHJointIDFromJointName("abdomen")]=0;
    //------------------------------------------------------------------------------------------------------------------
    for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--3d")==0)
                {
                    //Sample use
                    // ./CSVClusterPlot  --3d rHand --size 500 500 --depth 500 --distance -250 
                    // ./CSVClusterPlot  --3d rHand --size 256 256 --depth 256 --distance -500 
                    go3DMode=1;
                    for (int z=0; z<MOCAPNET_OUTPUT_NUMBER; z++)
                        {
                            activeJoints[z]=0;
                        }
                    //activeJoints[ getBVHJointIDFromJointName("rForeArm")]=1;
                    if(argc>i+1)
                        {
                            activeJoints[ getBVHJointIDFromJointName(argv[i+1])]=1;
                        }
                    else
                        {
                            fprintf(stderr,"We need a limb to focus on e.g. rHand <- capitalization is imporant..\n");
                        }
                }
            else if (strcmp(argv[i],"--onlylimbs")==0)
                {
                    for (int z=0; z<MOCAPNET_OUTPUT_NUMBER; z++)
                        {
                            activeJoints[z]=0;
                        }
                    activeJoints[ getBVHJointIDFromJointName("rShin")]=1;
                    activeJoints[ getBVHJointIDFromJointName("rFoot")]=1;
                    activeJoints[ getBVHJointIDFromJointName("lShin")]=1;
                    activeJoints[ getBVHJointIDFromJointName("lFoot")]=1;
                    activeJoints[ getBVHJointIDFromJointName("neck")]=1;
                    activeJoints[ getBVHJointIDFromJointName("head")]=1;
                    activeJoints[ getBVHJointIDFromJointName("lForeArm")]=1;
                    activeJoints[ getBVHJointIDFromJointName("lHand")]=1;
                    activeJoints[ getBVHJointIDFromJointName("rForeArm")]=1;
                    activeJoints[ getBVHJointIDFromJointName("rHand")]=1;
                }
            else if (strcmp(argv[i],"--onlyrhand")==0)
                {
                    for (int z=0; z<MOCAPNET_OUTPUT_NUMBER; z++)
                        {
                            activeJoints[z]=0;
                        }
                    activeJoints[ getBVHJointIDFromJointName("rForeArm")]=1;
                    activeJoints[ getBVHJointIDFromJointName("rHand")]=1;
                }
            else if (strcmp(argv[i],"--onlylhand")==0)
                {
                    for (int z=0; z<MOCAPNET_OUTPUT_NUMBER; z++)
                        {
                            activeJoints[z]=0;
                        }
                    activeJoints[ getBVHJointIDFromJointName("lForeArm")]=1;
                    activeJoints[ getBVHJointIDFromJointName("lHand")]=1;
                }
            else if (strcmp(argv[i],"--onlyrleg")==0)
                {
                    for (int z=0; z<MOCAPNET_OUTPUT_NUMBER; z++)
                        {
                            activeJoints[z]=0;
                        }
                    activeJoints[ getBVHJointIDFromJointName("rShin")]=1;
                    activeJoints[ getBVHJointIDFromJointName("rFoot")]=1;
                }
            else if (strcmp(argv[i],"--onlylleg")==0)
                {
                    for (int z=0; z<MOCAPNET_OUTPUT_NUMBER; z++)
                        {
                            activeJoints[z]=0;
                        }
                    activeJoints[ getBVHJointIDFromJointName("lShin")]=1;
                    activeJoints[ getBVHJointIDFromJointName("lFoot")]=1;
                }
        }
    //------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------

    state->redraw=1;

    srand(time(NULL));

    //find dataset/MotionCapture/ -type f -name "*.bvh" >  src/MocapNET2/CSVClusterPlot/listOfFilesToCluster
    struct BVH_MotionCapture bvhMotion= {0};

    char buffer[1024]= {0};

    FILE * fp =0;

    fp=fopen(selectedSource,"r");

    if (fp==0)
        {
            fprintf(stderr,"Cannot open list of files to cluster\n");
            free(state->accumulatedImage);
            free(state);
            free(state3D);
            return 0;
        }


    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    unsigned int numberOfLinesProcessed=0;
    unsigned long numberOfPosesLoaded=0;

    while ((read = getline(&line, &len, fp)) != -1)
        { 
            if (line!=0)
                {
                    ++numberOfLinesProcessed;
                    //-----------------------------------------------------------------
                    int lineLength=strlen(line);
                    if (lineLength>0)
                        {
                            if (line[lineLength-1]==10)
                                {
                                    line[lineLength-1]=0;
                                }
                            if (line[lineLength-1]==13)
                                {
                                    line[lineLength-1]=0;
                                }
                        }
                    if (lineLength>1)
                        {
                            if (line[lineLength-2]==10)
                                {
                                    line[lineLength-2]=0;
                                }
                            if (line[lineLength-2]==13)
                                {
                                    line[lineLength-2]=0;
                                }
                        }
                    //-----------------------------------------------------------------
                    fprintf(stderr,"Loading `%s`\n",line);

                    if ( bvh_loadBVH(line, &bvhMotion, 1.0) )
                        {
                            numberOfPosesLoaded+=bvhMotion.numberOfFrames;

                            bvh_renameJointsForCompatibility(&bvhMotion);

                            if (filterPoses)
                                {
                                    filterOutPosesThatAreCloseToRules(&bvhMotion,35,removeNeutralPosesRules);
                                }

                            if (randomize)
                                {
                                    //--perturbJointAngles 4 38.0 rshoulder lshoulder rhip lhip --perturbJointAngles 8 10.0 rhand relbow lelbow lhand lknee rknee lfoot rfoot

                                    bvh_PerturbJointAngles(
                                        &bvhMotion,
                                        4,
                                        38.0,
                                        perturbList1,
                                        0
                                    );

                                    bvh_PerturbJointAngles(
                                        &bvhMotion,
                                        8,
                                        10.0,
                                        perturbList2,
                                        0
                                    );

                                    bvh_PerturbJointAngles(
                                        &bvhMotion,
                                        2,
                                        10.0,
                                        perturbList3,
                                        0
                                    );


                                }


                            if (go3DMode)
                                {
                                    //3D Voxel Clustering Mode
                                    std::vector<std::vector<float> > aPose = collect3DPoses(state3D,activeJoints,&bvhMotion,skeletonDistance,WIDTH,HEIGHT,DEPTH);

                                    //memset(viewMat.data,0,WIDTH*HEIGHT*3*sizeof(char));
                                    
                                    if (numberOfLinesProcessed%5==0) 
                                        { draw3DClustering(viewMat,state3D,WIDTH,HEIGHT,DEPTH); }  
                                    drawSkeleton(viewMat,aPose,0,0,1);
                                }
                            else
                                {
                                    //2D Projection Mode
                                    std::vector<std::vector<float> > aPose = collectPoses(state,activeJoints,&bvhMotion,skeletonDistance,WIDTH,HEIGHT);
                                    if ( aPose.size()>0 )
                                        {
                                            state->redraw=1;
                                        }

                                    if (state->redraw)
                                        {
                                            draw2DClustering(viewMat,&bvhMotion,aPose,state,line,videoPath,WIDTH,HEIGHT);

                                            state->redraw=1;
                                        }
                                }



                            fprintf(stderr,"Done with %s\n",line);

                            bvh_free(&bvhMotion);

                            fprintf(stderr,"deallocation complete..\n");


                            imshow("BVH", viewMat);

                            if (saveVideo)
                                {
                                    snprintf(videoFrameFinalFilename,512,"%s/colorFrame_0_%05u.png",videoPath,visualizationFrameNumber);
                                    imwrite(videoFrameFinalFilename,viewMat);
                                }

                            ++visualizationFrameNumber;

                            waitKey(1);
                        }
                    else
                        {
                            fprintf(stderr,"Failed to load %s \n",line);
                        }
                }//Line not empty
        } //New line read

    if (line!=0)
        {
            free(line);
            line=0;
            len=0;
        }
    fclose(fp);
    // the camera will be deinitialized automatically in VideoCapture destructor



    if (go3DMode)
        {
            fprintf(stderr,"3D clustering accumulated %lu samples \n",state3D->accumulatedSamples);
            if (state3D->fp!=0)
                {
                    fclose(state3D->fp);
                }
                
            //------------------------------------------------------------------------ 
            int i=system("cat dataset/header.bvh > filteredPoses.bvh");
            if (i!=0) { fprintf(stderr,"Could not use header for filterPoses.bvh\n"); }
            //------------------------------------------------------------------------
            i=system("echo \"MOTION\" >> filteredPoses.bvh");
            if (i!=0) { fprintf(stderr,"Could not add MOTION to filterPoses.bvh\n"); }
            //------------------------------------------------------------------------
            i=system("echo \"Frames: `cat filteredPoses.bvhm | wc -l`\" >> filteredPoses.bvh");
            if (i!=0) { fprintf(stderr,"Could not add number of poses\n"); }
            //------------------------------------------------------------------------
            i=system("echo \"Frame Time: 0.04\" >> filteredPoses.bvh");
            if (i!=0) { fprintf(stderr,"Could not add frame time\n"); }
            //------------------------------------------------------------------------
            i=system("cat filteredPoses.bvhm >> filteredPoses.bvh");
            if (i!=0) { fprintf(stderr,"Could not add motion frames\n"); }
            //------------------------------------------------------------------------
            i=system("rm filteredPoses.bvhm");
            if (i!=0) { fprintf(stderr,"Could not remove intermediate motion frames\n"); }
            //------------------------------------------------------------------------ 
        }


    free(state);
    free(state3D);

    fprintf(stderr,"Done overlaying %lu poses..!\n",numberOfPosesLoaded);
    imshow("BVH", viewMat);
    imwrite("csvCluster.png",viewMat);

    if (saveVideo)
        {
            //Low-Res video encoding
            //snprintf(videoFrameFinalFilename,512,"ffmpeg -framerate 30 -i %s/colorFrame_0_%%05d.png   -y -r 30 -threads 8 -crf 9 -pix_fmt yuv420p  %s/video.mp4",videoPath,videoPath);

            //High-Res video encoding
            snprintf(videoFrameFinalFilename,512,"ffmpeg -framerate 30 -i %s/colorFrame_0_%%05d.png  -s 1200x720  -y -r 30 -pix_fmt yuv420p -threads 8  %s/video.mp4",videoPath,videoPath);
            int i=system(videoFrameFinalFilename);


            if (i==0)
                {
                    fprintf(stderr,"Successfully wrote video file %s/video.mp4.. \n",videoPath);
                }
            else
                {
                    fprintf(stderr,"Failed to write a video file %s/video.mp4.. \n",videoPath);
                }

            fprintf(stderr,"Since we wanted to output video this is probably a scripted run, will not wait for a keystroke..\n");
            waitKey(1);
        }
    else
        {
            waitKey(0);
        }


   free(state->accumulatedImage);
   free(state);
   free(state3D);
            
    return 0;
}
