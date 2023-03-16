#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../mocapnet2.hpp"
#include "parseCommandlineOptions.hpp"
#include "../IO/bvh.hpp"
#include "../tools.hpp"

#include "../../../JointEstimator2D/jointEstimator2D.hpp"
#include "../IO/jsonMocapNETHelpers.hpp"
#include "../IO/csvRead.hpp"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

const char   outputPathStatic[]="out.bvh";


// To automatically generate the below seen lists you can use the following command..
// cat parseCommandlineOptions.cpp | grep strcmp | awk -F'[\"/]' '{print $2}' |  serializelist --name mnetParams


/**
 * @brief This is an auto-generated list of C-Strings made using serializelist
 * https://github.com/AmmarkoV/Unix-Commandline-Apps/tree/master/serializelist
 * Each entry of this mnetParamsNames[] array is linked with mnetParamsEnum[] to ensure no mistakes
 */
static const char *mnetParamsNames[]={
"--forceOutputPositionRotation",
"--rotateSkeleton",
"auto",
"--inputFramerate",
"--msg",
"--map",
"--unconstrained",
"-o",
"--dontbend",
"--opengl",
"--gdpr",
"--keep",
"--save",
"--save2D",
"--save3D",
"--saveCSV3D",
"--noik",
"--ik",
"--nv",
"--novisualization",
"-v",
"--visualize",
"--frameskip",
"--nofilter",
"--scale",
"--scaleX",
"--scaleY",
"--focalLength",
"--fscale",
"--noise",
"--front",
"--back",
"--left",
"--right",
"--bvhcenter",
"--openpose",
"--vnect",
"--frames",
"--maxFrames",
"--from",
"--quality",
"--mt",
"--cpu",
"--gpu",
"--delay",
"--skip",
"--tpose",
"--nolowerbody",
"--show",
"--nohands",
"--hands",
"--face",
"--noface",
"--label",
"--seriallength",
"-o",
"--mode",
"--size",
"--visualizationSize",
"--gestures",
"--changeFeetDimensions",
"--scaleHandsDimensions",
"--scaleHeadDimensions",
"--scaleAllJointDimensions",
"--changeJointDimensions",
"--nsrm",
"--forth",
"--penalizeSymmetriesHeuristic",
//-------------------------
"GUARD_FOR_+1_ERROR"
};




/**
 * @brief This is an auto-generated list of C-Strings made using serializelist
 * https://github.com/AmmarkoV/Unix-Commandline-Apps/tree/master/serializelist
 * Each entry of this mnetParamsEnum[] array is linked with mnetParamsNames[] to ensure no mistakes
 */
enum mnetParamsEnum {
MNETPARAMS_FORCEOUTPUTPOSITIONROTATION,
MNETPARAMS_ROTATESKELETON,
MNETPARAMS_AUTO,
MNETPARAMS_INPUTFRAMERATE,
MNETPARAMS_MSG,
MNETPARAMS_MAP,
MNETPARAMS_UNCONSTRAINED,
MNETPARAMS_O,
MNETPARAMS_DONTBEND,
MNETPARAMS_OPENGL,
MNETPARAMS_GDPR,
MNETPARAMS_KEEP,
MNETPARAMS_SAVE,
MNETPARAMS_SAVE2D,
MNETPARAMS_SAVE3D,
MNETPARAMS_SAVECSV3D,
MNETPARAMS_NOIK,
MNETPARAMS_IK,
MNETPARAMS_NV,
MNETPARAMS_NOVISUALIZATION,
MNETPARAMS_V,
MNETPARAMS_VISUALIZE,
MNETPARAMS_FRAMESKIP,
MNETPARAMS_NOFILTER,
MNETPARAMS_SCALE,
MNETPARAMS_SCALEX,
MNETPARAMS_SCALEY,
MNETPARAMS_FOCALLENGTH,
MNETPARAMS_FSCALE,
MNETPARAMS_NOISE,
MNETPARAMS_FRONT,
MNETPARAMS_BACK,
MNETPARAMS_LEFT,
MNETPARAMS_RIGHT,
MNETPARAMS_BVHCENTER,
MNETPARAMS_OPENPOSE,
MNETPARAMS_VNECT,
MNETPARAMS_FRAMES,
MNETPARAMS_MAXFRAMES,
MNETPARAMS_FROM,
MNETPARAMS_QUALITY,
MNETPARAMS_MT,
MNETPARAMS_CPU,
MNETPARAMS_GPU,
MNETPARAMS_DELAY,
MNETPARAMS_SKIP,
MNETPARAMS_TPOSE,
MNETPARAMS_NOLOWERBODY,
MNETPARAMS_SHOW,
MNETPARAMS_NOHANDS,
MNETPARAMS_HANDS,
MNETPARAMS_FACE,
MNETPARAMS_NOFACE,
MNETPARAMS_LABEL,
MNETPARAMS_SERIALLENGTH,
MNETPARAMS__O,
MNETPARAMS_MODE,
MNETPARAMS_SIZE,
MNETPARAMS_VISUALIZATIONSIZE,
MNETPARAMS_GESTURES,
MNETPARAMS_CHANGEFEETDIMENSIONS,
MNETPARAMS_SCALEHANDSDIMENSIONS,
MNETPARAMS_SCALEHEADDIMENSIONS,
MNETPARAMS_SCALEALLJOINTDIMENSIONS,
MNETPARAMS_CHANGEJOINTDIMENSIONS,
MNETPARAMS_NSRM,
MNETPARAMS_FORTH,
MNETPARAMS_PENALIZESYMMETRIESHEURISTIC,
//-------------------------
MNETPARAMS_NUMBER
};





void printHelp()
{
   fprintf(stderr,"List of recognized commands : \n");
   fprintf(stderr,"____________________________________\n");
   //If you want to update this then do the following.. to get the list of arguments :P
   //cat parseCommandlineOptions.cpp | grep strcmp | awk -F'[\"/]' '{print $2}'

   // mnetParamsNames[MNETPARAMS_OPENPOSE]
   for (unsigned int i=0; i<MNETPARAMS_NUMBER; i++)
   {
       //This is not a CPU friendly way to do the list, however the help function is never called
       //and this is the optimally easiest version of the help utility that can be maintainned..
       switch (i)
       {
         case MNETPARAMS_FORCEOUTPUTPOSITIONROTATION : fprintf(stderr,"%s XPos YPos ZPos Rot1 Rot2 Rot3\n Force skeleton output position and rotation \n",mnetParamsNames[i]); break;
         case MNETPARAMS_ROTATESKELETON :              fprintf(stderr,"%s Value\n Force skeleton orietnation angle , can be auto \n",mnetParamsNames[i]); break;
         //case MNETPARAMS_AUTO :                        fprintf(stderr,"%s Does nothing \n",mnetParamsNames[i]); break;
         case MNETPARAMS_INPUTFRAMERATE :              fprintf(stderr,"%s framerate\n Try to enforce a specific framerate \n",mnetParamsNames[i]); break;
         case MNETPARAMS_MSG :                         fprintf(stderr,"%s message\n  Display a message in visualization to label experiment\n",mnetParamsNames[i]); break;
         case MNETPARAMS_MAP :                         fprintf(stderr,"%s filename\n Load a map for this demo -> https://youtu.be/zQsOfPBX8RM \n",mnetParamsNames[i]); break;
         case MNETPARAMS_UNCONSTRAINED :               fprintf(stderr,"%s \n Do not constrain pose position/rotation\n",mnetParamsNames[i]); break;
         case MNETPARAMS_O :                           fprintf(stderr,"%s filename\n Set output BVH file path\n",mnetParamsNames[i]); break;
         case MNETPARAMS_DONTBEND :                    fprintf(stderr,"%s \n Workaround: Force no bending in spine\n",mnetParamsNames[i]); break;
         case MNETPARAMS_OPENGL :                      fprintf(stderr,"%s \n Enable OpenGL visualization (if compiled in your build)\n",mnetParamsNames[i]); break;
         case MNETPARAMS_GDPR :                        fprintf(stderr,"%s \n Enable GDPR mosaic in visualization over faces for privacy \n",mnetParamsNames[i]); break;
         case MNETPARAMS_KEEP :                        fprintf(stderr,"%s \n Keep intermediate image stills snapshots for every frame of output video",mnetParamsNames[i]); break;
         case MNETPARAMS_SAVE :                        fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_SAVE2D :                      fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_SAVE3D :                      fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_SAVECSV3D :                   fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_NSRM :                        fprintf(stderr,"%s \n Switch to Regular NSRM descriptor ( experimental requires specific neural network.. )\n",mnetParamsNames[i]); break;
         case MNETPARAMS_NOIK :                        fprintf(stderr,"%s \n Disable HCD Inverse Kinematics Module\n",mnetParamsNames[i]); break;
         case MNETPARAMS_IK :                          fprintf(stderr,"%s learningRate iterations epochs\n Enable HCD IK with specified settings\n",mnetParamsNames[i]); break;
         case MNETPARAMS_NV :                          fprintf(stderr,"%s \n Disable Visualization for headless execution\n",mnetParamsNames[i]); break;
         case MNETPARAMS_NOVISUALIZATION :             fprintf(stderr,"%s \n Disable Visualization for headless execution\n",mnetParamsNames[i]); break;
         case MNETPARAMS_V :                           fprintf(stderr,"%s \n Enable Visualization\n",mnetParamsNames[i]); break;
         case MNETPARAMS_VISUALIZE :                   fprintf(stderr,"%s \n Enable Visualization\n",mnetParamsNames[i]); break;
         case MNETPARAMS_FRAMESKIP :                   fprintf(stderr,"%s \n Skip input frames to keep up with input source\n",mnetParamsNames[i]); break;
         case MNETPARAMS_NOFILTER :                    fprintf(stderr,"%s \n Disable output butterworth filtering, useful if processing input frames with no temporal cohesion\n",mnetParamsNames[i]); break;
         case MNETPARAMS_SCALE :                       fprintf(stderr,"%s factor\n Scale input 2D points\n",mnetParamsNames[i]); break;
         case MNETPARAMS_SCALEX :                      fprintf(stderr,"%s factor\n Scale input 2D points in X dimension only\n",mnetParamsNames[i]); break;
         case MNETPARAMS_SCALEY :                      fprintf(stderr,"%s factor\n Scale input 2D points in Y dimension only\n",mnetParamsNames[i]); break;
         case MNETPARAMS_FOCALLENGTH :                 fprintf(stderr,"%s fx fy\n Enforce specific camera focal lengths \n",mnetParamsNames[i]); break;
         case MNETPARAMS_FSCALE :                      fprintf(stderr,"%s scaleX scaleY\n Scale skeleton from center in both dimensions\n",mnetParamsNames[i]); break;
         case MNETPARAMS_NOISE :                       fprintf(stderr,"%s noisePerturbationsInPixelsX noisePerturbationsInPixelsY\n Add Gaussian noise in X/Y joint coordinates \n",mnetParamsNames[i]); break;
         case MNETPARAMS_FRONT :                       fprintf(stderr,"%s \n Deprecated: Force front orientation of skeleton\n",mnetParamsNames[i]); break;
         case MNETPARAMS_BACK :                        fprintf(stderr,"%s \n Deprecated: Force back orientation of skeleton\n",mnetParamsNames[i]); break;
         case MNETPARAMS_LEFT :                        fprintf(stderr,"%s \n Deprecated: Force left orientation of skeleton\n",mnetParamsNames[i]); break;
         case MNETPARAMS_RIGHT :                       fprintf(stderr,"%s \n Deprecated: Force right orientation of skeleton\n",mnetParamsNames[i]); break;
         case MNETPARAMS_BVHCENTER :                   fprintf(stderr,"%s \n Force BVH skeleton outut to be centered at 0\n",mnetParamsNames[i]); break;
         case MNETPARAMS_FORTH :                       fprintf(stderr,"%s \n Switch 2D Joint Estimator engine to FORTH\n",mnetParamsNames[i]); break;
         case MNETPARAMS_OPENPOSE :                    fprintf(stderr,"%s \n Switch 2D Joint Estimator engine to homebrew'ed OpenPose\n",mnetParamsNames[i]); break;
         case MNETPARAMS_VNECT :                       fprintf(stderr,"%s \n Switch 2D Joint Estimator engine to homebrew'ed VNect\n",mnetParamsNames[i]); break;
         case MNETPARAMS_FRAMES :                      fprintf(stderr,"%s frameLimit \n Set a limit on input frames\n",mnetParamsNames[i]); break;
         case MNETPARAMS_MAXFRAMES :                   fprintf(stderr,"%s frameLimit \n Set a limit on input frames\n",mnetParamsNames[i]); break;
         case MNETPARAMS_FROM :                        fprintf(stderr,"%s path \n Declare an input source by supplying a system path\n",mnetParamsNames[i]); break;
         case MNETPARAMS_QUALITY :                     fprintf(stderr,"%s λ \n Legacy: Set neural network quality using lambda value\n",mnetParamsNames[i]); break;
         case MNETPARAMS_MT :                          fprintf(stderr,"%s \n Performance: Enable Multithreaded execution\n",mnetParamsNames[i]); break;
         case MNETPARAMS_CPU :                         fprintf(stderr,"%s \n Switch execution of neural networks to CPU\n",mnetParamsNames[i]); break;
         case MNETPARAMS_GPU :                         fprintf(stderr,"%s \n Switch execution of neural networks to GPU\n",mnetParamsNames[i]); break;
         case MNETPARAMS_DELAY :                       fprintf(stderr,"%s milliseconds\n Add a delay after each regressed frame to have time to see it better\n",mnetParamsNames[i]); break;
         case MNETPARAMS_SKIP :                        fprintf(stderr,"%s frameSkip\n Allow neural network frameskip up to a maximum number of frames\n",mnetParamsNames[i]); break;
         case MNETPARAMS_TPOSE :                       fprintf(stderr,"%s \n Prepend a T-Pose in BVH output to make 3D animation rigging easier\n",mnetParamsNames[i]); break;
         case MNETPARAMS_NOLOWERBODY :                 fprintf(stderr,"%s \n Ignore Lower body\n",mnetParamsNames[i]); break;
         case MNETPARAMS_SHOW :                        fprintf(stderr,"%s visualizationType\n Switch between different visualization types : ",mnetParamsNames[i]);
                                                       fprintf(stderr,"  0 = Default skeleton only wireframe visualization ");
                                                       fprintf(stderr,"  1 = Plot rotation angles ");
                                                       fprintf(stderr,"  2 = Map visualization seen in : https://youtu.be/zQsOfPBX8RM ");
                                                       fprintf(stderr,"  3 = Visualization of skeleton overlayed on RGB frame ");
                                                       fprintf(stderr,"  4 = Visualization using template code ( intended for user development ) ");
                                                       fprintf(stderr,"  5 = Visualization of RGB frame and on its right the skeleton on a solid color background");
         break;
         case MNETPARAMS_NOHANDS :                     fprintf(stderr,"%s \n Disable regression of hands\n",mnetParamsNames[i]); break;
         case MNETPARAMS_HANDS :                       fprintf(stderr,"%s \n Enable regression of hands\n",mnetParamsNames[i]); break;
         case MNETPARAMS_FACE :                        fprintf(stderr,"%s \n Enable regression of face\n",mnetParamsNames[i]); break;
         case MNETPARAMS_NOFACE :                      fprintf(stderr,"%s \n Disable regression of face\n",mnetParamsNames[i]); break;
         case MNETPARAMS_LABEL :                       fprintf(stderr,"%s label \n Control image input filenames for example colorFrame_0_",mnetParamsNames[i]); break;
         case MNETPARAMS_SERIALLENGTH :                fprintf(stderr,"%s length \n Control image input serial length\n",mnetParamsNames[i]); break;
         case MNETPARAMS__O :                          fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_MODE :                        fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_SIZE :                        fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_VISUALIZATIONSIZE :           fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_GESTURES :                    fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_CHANGEFEETDIMENSIONS :        fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_SCALEHEADDIMENSIONS :         fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_SCALEHANDSDIMENSIONS :        fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_SCALEALLJOINTDIMENSIONS :     fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_CHANGEJOINTDIMENSIONS :       fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
         case MNETPARAMS_PENALIZESYMMETRIESHEURISTIC : fprintf(stderr,"%s \n",mnetParamsNames[i]); break;
       };




   }

   exit(0);
}








void  defaultMocapNET2Options(struct MocapNET2Options * options)
{
    //This causes a problem in valgrind
    //memset(options,0,sizeof(struct MocapNET2Options));

    options->isJSONFile=0;
    options->isCSVFile=0;
    options->webcamSource = 0;
    options->path=0;
    options->datasetPath=0;
    options->jointEstimatorUsed = JOINT_2D_ESTIMATOR_OPENPOSE; //JOINT_2D_ESTIMATOR_FORTH;

    options->doUpperBody=1;
    options->doLowerBody=1;
    options->doFace=0;
    options->doHands=0;
    options->forceFront=0;
    options->forceLeft=0;
    options->forceRight=0;
    options->forceBack=0;
    options->visualizationType=0;

    //Default IK options
    options->useInverseKinematics=0;
    options->learningRate=0.01;
    options->spring =20.0;
    options->iterations=15; // 5
    options->epochs=30;

    //Test under Simulated Gaussian Noise
    options->addNormalizedPixelGaussianNoiseX=0.0;
    options->addNormalizedPixelGaussianNoiseY=0.0;

    options->outputPath = (char*) outputPathStatic;

    //Butterworth options..
    options->inputFramerate=30.0;
    options->filterCutoff=5.0;

    options->regularNSRMMatrix=0;

    options->visualize=1;
    options->doMultiThreadedIK=0;
    options->useOpenGLVisualization=0;
    options->save3DVisualization=0;
    options->save2DVisualization=0;
    options->saveVisualization=0;
    options->saveCSV3DFile=0;
    options->constrainPositionRotation=1;
    options->keepIntermediateFiles=0;

    options->delay=0;
    options->prependTPose=0;
    options->serialLength=5;
    options->bvhCenter=0;

    if (getCPUName(options->CPUName,512))
        {
            fprintf(stderr,"CPU : %s\n",options->CPUName);
        }

    if (getGPUName(options->GPUName,512))
        {
            fprintf(stderr,"GPU : %s\n",options->GPUName);
        }

    options->gdpr=0;
    options->quality=1.0;
    options->mocapNETMode=1;
    options->doGestureDetection=0;
    options->doOutputFiltering=1;
    options->useCPUOnlyForMocapNET=1; //Use CPU for MocapNET
    options->useCPUOnlyFor2DEstimator=0; // Use GPU for 2D estimator
    options->brokenFrames=0;
    options->numberOfMissingJoints=0;

    options->visWidth=1920;
    options->visHeight=1080;
    options->width=1920;
    options->height=1080;

    options->penalizeSymmetriesHeuristic = 0;

    options->scale=1.0;
    options->scaleX=1.0;
    options->scaleY=1.0;
    options->fScaleX=1.0;
    options->fScaleY=1.0;

    options->loopStartTime=0;
    options->loopEndTime=1000;
    options->totalLoopFPS=0.0;
    options->fpsMocapNET=0.0;
    options->fps2DEstimator=0.0;

    options->frameSkip=0;
    options->frameLimit=0;
}

void checkVersion()
{
 char hostname[1024];
 gethostname(hostname, 1024);
 //==========================
 //char username[1024];
 //getlogin_r(username,1024);
 //==========================

 char command[2048];
 snprintf(command,2048,"wget -qO- \"http://ammar.gr/mocapnet/version/index.php?h=%s&v=%s\" &",hostname,MocapNETVersion);
 int i=0;
 i = system(command);
}

void argumentError(int currentlyAt,int extraAt,int argc, char *argv[])
{
    fprintf(stderr,RED "Incorrect number of arguments, %u required ( @ %s )..\n" NORMAL,extraAt,argv[currentlyAt]);
    exit(1);
}


int loadOptionsFromCommandlineOptions(struct MocapNET2Options * options,int argc, char *argv[])
{
    checkVersion();
//(struct BVH_MotionCapture * bvhMotion,float shoulderToElbowLength,float elbowToHandLength,float hipToKneeLength,float kneeToFootLength)
    //------------------------------------------------------
    //                Parse arguments
    //------------------------------------------------------
    for (int i=0; i<argc; i++)
        {
             if (
                  (strcmp(argv[i],"-h")==0) ||
                  (strcmp(argv[i],"--help")==0) ||
                  (strcmp(argv[i],"?")==0)
                )
                {
                    printHelp();
                }
                else
            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_FORCEOUTPUTPOSITIONROTATION])==0) // "--forceOutputPositionRotation"
                {
                    if (argc>i+1)
                        {
                          options->forceOutputPositionRotation=1;
                          options->outputPosRot[0]=atof(argv[i+1]);
                          options->outputPosRot[1]=atof(argv[i+2]);
                          options->outputPosRot[2]=atof(argv[i+3]);
                          options->outputPosRot[3]=atof(argv[i+4]);
                          options->outputPosRot[4]=atof(argv[i+5]);
                          options->outputPosRot[5]=atof(argv[i+6]);
                        }
                }
             else
            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_ROTATESKELETON])==0) // "--rotateSkeleton"
                {
                    if (argc>i+1)
                        {
                            if (strcmp(argv[i+1],"auto")==0)
                            {
                              options->skeletonRotation=360.0;
                            } else
                            {
                              options->skeletonRotation=atof(argv[i+1]);
                            }
                        }
                }
             else

            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_INPUTFRAMERATE])==0) //"--inputFramerate"
                {
                    if (argc>i+1)
                        {
                           options->inputFramerate = atof(argv[i+1]);
                           fprintf(stderr,"Input Framerate set to %0.2f \n", options->inputFramerate);
                        }
                }
             else

            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_MSG])==0) //"--msg"
                {
                    if (argc>i+1)
                        {
                           fprintf(stderr,"Message set to %s \n",argv[i+1]);
                           snprintf(options->message,512,"%s",argv[i+1]);
                        }
                }
             else
            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_MAP])==0) //"--map"
                {
                    if (argc>i+1)
                        {
                           fprintf(stderr,"Configuring Map to use file %s \n",argv[i+1]);
                          snprintf(options->mapFilePath,512,"%s",argv[i+1]);
                          options->visualizationType=2;
                        }
                }
             else
            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_UNCONSTRAINED])==0) //"--unconstrained"
                {
                    options->constrainPositionRotation=0;
                }
            else if  (
                        (strcmp(argv[i],"-o")==0) ||
                        (strcmp(argv[i],"--o")==0) ||
                        (strcmp(argv[i],"--output")==0)
                     )
                {
                  if (argc>i+1)
                        {
                         options->outputPath=argv[i+1];
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if  (strcmp(argv[i],mnetParamsNames[MNETPARAMS_DONTBEND])==0) // "--dontbend"
            {
                options->dontBend=1;
            }
            else if  (strcmp(argv[i],mnetParamsNames[MNETPARAMS_OPENGL])==0) //"--opengl"
            {
                options->useOpenGLVisualization=1;
            }
            else if  (strcmp(argv[i], mnetParamsNames[MNETPARAMS_GDPR])==0) //"--gdpr"
            {
                options->gdpr=1;
            }
            else if  (strcmp(argv[i],mnetParamsNames[MNETPARAMS_KEEP])==0) //"--keep"
                {
                    options->keepIntermediateFiles=1;
                }
            else if  (strcmp(argv[i],"--save")==0)
                {
                    options->saveVisualization=1;
                }
            else if  ( (strcmp(argv[i],"--save2D")==0) || (strcmp(argv[i],"--save2d")==0) )
                {
                    options->save2DVisualization=1;
                }
            else if  ( (strcmp(argv[i],"--save3D")==0) || (strcmp(argv[i],"--save3d")==0) )
                {
                    options->save3DVisualization=1;
                }
            else if  ( (strcmp(argv[i],"--saveCSV3D")==0) || (strcmp(argv[i],"--savecsv3d")==0) )
                {
                    options->saveCSV3DFile=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_NOIK])==0) //"--noik"
                {
                    fprintf(stderr,RED "Inverse Kinematics module disabled..\n" NORMAL);
                    options->useInverseKinematics=0;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_IK])==0) //"--ik"
                {
                    if (argc>i+3)
                        {
                            options->useInverseKinematics=1;
                            options->learningRate=atof(argv[i+1]);
                            options->iterations=atoi(argv[i+2]);
                            options->epochs=atoi(argv[i+3]);
                            //options->spring=atof(argv[i+4]);
                        }
                          else
                        { argumentError(i,3,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_NSRM])==0) //"--nv"
                {
                    fprintf(stderr,"Switched back to NSRM\n");
                    options->regularNSRMMatrix=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_NV])==0) //"--nv"
                {
                    fprintf(stderr,"Visualization disabled\n");
                    options->visualize=0;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_NOVISUALIZATION])==0) //"--novisualization"
                {
                    fprintf(stderr,"Visualization disabled\n");
                    options->visualize=0;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_V])==0) //"-v"
                {
                    fprintf(stderr,"Visualization enabled\n");
                    options->visualize=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_VISUALIZE])==0) //"--visualize"
                {
                    fprintf(stderr,"Visualization enabled\n");
                    options->visualize=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_FRAMESKIP])==0) //"--frameskip"
                {
                   if (argc>i+1)
                        {
                          options->frameSkip=atoi(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_NOFILTER])==0) //"--nofilter"
                {
                    fprintf(stderr,"Filtering Disabled\n");
                    options->doOutputFiltering=0;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SCALE])==0) //"--scale"
                {
                  if (argc>i+1)
                        {
                         options->scale=atof(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SCALEX])==0) //"--scaleX"
                {
                    if (argc>i+1)
                        {
                         options->scaleX=atof(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SCALEY])==0) //"--scaleY"
                {
                    if (argc>i+1)
                        {
                          options->scaleY=atof(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_FOCALLENGTH])==0) //"--focalLength"
                {
                    overrideBVHSubsystemFocalLength(atof(argv[i+1]),atof(argv[i+2]));
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_FSCALE])==0) //"--fscale"
                {
                    if (argc>i+2)
                        {
                         options->fScaleX=atof(argv[i+1]);
                         options->fScaleY=atof(argv[i+2]);
                        } else
                        { argumentError(i,2,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_NOISE])==0) //"--noise"
                {
                    if (argc>i+2)
                        {
                         options->addNormalizedPixelGaussianNoiseX=atof(argv[i+1]);
                         options->addNormalizedPixelGaussianNoiseY=atof(argv[i+2]);
                        } else
                        { argumentError(i,2,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_FRONT])==0) //"--front"
                {
                    options->forceFront=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_BACK])==0) //"--back"
                {
                    options->forceBack=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_LEFT])==0) //"--left"
                {
                    options->forceLeft=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_RIGHT])==0) //"--right"
                {
                    options->forceRight=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_BVHCENTER])==0) // "--bvhcenter"
                {
                    options->bvhCenter=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_FORTH])==0) //"--forth"
                {
                    options->jointEstimatorUsed = JOINT_2D_ESTIMATOR_FORTH;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_OPENPOSE])==0) //"--openpose"
                {
                    options->jointEstimatorUsed = JOINT_2D_ESTIMATOR_OPENPOSE;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_VNECT])==0) //"--vnect"
                {
                    options->jointEstimatorUsed = JOINT_2D_ESTIMATOR_VNECT;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_FRAMES])==0) //"--frames"
                {
                  if (argc>i+1)
                        {
                         options->frameLimit=atoi(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_MAXFRAMES])==0) //"--maxFrames"
                {
                  if (argc>i+1)
                        {
                         options->frameLimit=atoi(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_FROM])==0) //"--from"
                {
                    if (argc>i+1)
                        {
                            options->webcamSource = argv[i+1];
                            options->path = argv[i+1];
                        if (strstr(options->path,".json")!=0)
                            {
                                fprintf(stderr,"JSON files no longer directly supported, please use the converter tool to convert them to .CSV");
                                options->isJSONFile=1;
                                options->isCSVFile=0;
                            }
                        if (strstr(options->path,".csv")!=0)
                            {
                                options->isJSONFile=0;
                                options->isCSVFile=1;
                            }
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_QUALITY])==0) //"--quality"
                {
                 if (argc>i+1)
                        {
                         options->quality=atof(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
             else
                if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_MT])==0) //"--mt"
                    {
                       options->doMultiThreadedIK=1;
                    }
             else
                if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_PENALIZESYMMETRIESHEURISTIC])==0) //"--mt"
                    {
                       options->penalizeSymmetriesHeuristic=1;
                    }
             else
                if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_CPU])==0) //"--cpu"
                    {
                        options->useCPUOnlyForMocapNET=1;
                        options->useCPUOnlyFor2DEstimator=1;
                    }
            else
                //if (strcmp(argv[i],"--cpu")==0)        { setenv("CUDA_VISIBLE_DEVICES", "", 1); } else
                if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_GPU])==0) //"--gpu"
                    {
                        options->useCPUOnlyForMocapNET=0;
                        options->useCPUOnlyFor2DEstimator=0;
                    }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_DELAY])==0) //"--delay"
                {
                    //If you want to take some time to check the results that
                    //might otherwise pass by very fast
                    if (argc>i+1)
                        {
                          options->delay=atoi(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SKIP])==0) //"--skip"
                {
                    //Allow skipping of frames in the neural network and using only the very fast IK module..
                    options->maximumNeuralNetworkSkipFrames=atoi(argv[i+1]);

                    if (options->maximumNeuralNetworkSkipFrames>0)
                    {
                      options->skipNeuralNetworkIfItIsNotNeeded=1;
                      if (options->maximumNeuralNetworkSkipFrames==1)
                      {
                       fprintf(stderr,"Skipping 1 frame every one frame makes no sense, assuming user wants to skip 1 frame every 2 frames\n");
                       options->maximumNeuralNetworkSkipFrames=2;
                      }
                    }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_TPOSE])==0) //"--tpose"
                {
                   options-> prependTPose=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_NOLOWERBODY])==0) //"--nolowerbody"
                {
                    options->doLowerBody=0;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SHOW])==0) //"--show"
                {
                    options->visualizationType=atoi(argv[i+1]);
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_NOHANDS])==0) //"--nohands"
                {
                    options->doHands=0;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_HANDS])==0) //"--hands"
                {
                    options->doHands=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_FACE])==0) //"--face"
                {
                    options->doFace=1;
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_NOFACE])==0) //"--noface"
                {
                    options->doFace=0;
                }
                else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_LABEL])==0) //"--label"
                    {
                      if (argc>i+1)
                        {
                         options->label = argv[i+1];
                        } else
                        { argumentError(i,1,argc,argv); }
                    }
                 else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SERIALLENGTH])==0) //"--seriallength"
                    {
                        if (argc>i+1)
                        {
                          options->serialLength = atoi(argv[i+1]);
                        }  else
                        { argumentError(i,1,argc,argv); }
                    }
                    /*
                else if  (  (strcmp(argv[i],"-o")==0) ||
                            (strcmp(argv[i],"--output")==0) )
                    {
                        if (argc>i+1)
                        {
                          options->outputPath=argv[i+1];
                        }  else
                        { argumentError(i,1,argc,argv); }
                    }*/
                else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_MODE])==0) //"--mode"
                    {
                        if (argc>i+1)
                        {
                          options->mocapNETMode=atoi(argv[i+1]);
                        }  else
                        { argumentError(i,1,argc,argv); }
                    }
                else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SIZE])==0) //"--size"
                    {
                        if (argc>i+2)
                        {
                         options->width = atoi(argv[i+1]);
                         options->height = atoi(argv[i+2]);
                        }  else
                        { argumentError(i,2,argc,argv); }
                    }
                else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_VISUALIZATIONSIZE])==0) //"--visualizationSize"
                    {
                        if (argc>i+2)
                        {
                          options->visWidth = atoi(argv[i+1]);
                          options->visHeight = atoi(argv[i+2]);
                        }  else
                        { argumentError(i,2,argc,argv); }
                    }
                else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_GESTURES])==0) //"--gestures"
                    {
                        options->doGestureDetection=1;
                    }
        }


    if (options->label==0)
        {
            options->label="colorFrame_0_";
        }

return 1;
}


int  loadOptionsAfterBVHLoadFromCommandlineOptions(struct MocapNET2Options * options,int argc, char *argv[])
{
       options->hasInit=0;
        for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_CHANGEFEETDIMENSIONS])==0) //"--changeFeetDimensions"
                {
                    if(argc>i+2)
                        {
                            if (!options->hasInit)
                                {
                                    initializeBVHConverter(0,options->visWidth,options->visHeight,0);
                                    options->hasInit=1;
                                }

                            changeFeetDimensions(
                                                  atof(argv[i+1]),
                                                  atof(argv[i+2])
                                                );
                        } else
                        { argumentError(i,2,argc,argv); }
                }
            else
            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SCALEALLJOINTDIMENSIONS])==0) // "--scaleAllJointDimensions"
                {
                    if(argc>i+1)
                        {
                            if (!options->hasInit)
                                {
                                    initializeBVHConverter(0,options->visWidth,options->visHeight,0);
                                    options->hasInit=1;
                                }
                            scaleAllJoints(atof(argv[i+1]));
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else
            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SCALEHANDSDIMENSIONS])==0) // "--scaleHandsDimensions"
                {
                    if(argc>i+1)
                        {
                            if (!options->hasInit)
                                {
                                    initializeBVHConverter(0,options->visWidth,options->visHeight,0);
                                    options->hasInit=1;
                                }
                            changeHandsScale(atof(argv[i+1]));
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else
            if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_SCALEHEADDIMENSIONS])==0) // "--scaleHeadDimensions"
                {
                    if(argc>i+1)
                        {
                            if (!options->hasInit)
                                {
                                    initializeBVHConverter(0,options->visWidth,options->visHeight,0);
                                    options->hasInit=1;
                                }
                            changeHeadScale(atof(argv[i+1]));
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],mnetParamsNames[MNETPARAMS_CHANGEJOINTDIMENSIONS])==0) //"--changeJointDimensions"
                {
                    if(argc>i+9)
                        {
                            if (!options->hasInit)
                                {
                                    initializeBVHConverter(0,options->visWidth,options->visHeight,0);
                                    options->hasInit=1;
                                }
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
                            argumentError(i,9,argc,argv);
                            fprintf(stderr,"Incorrect number of parameters given..\n");
                            return 0;
                        }
                }
        }

    return 1;
}




int  takeCareOfScalingInputAndAddingNoiseAccordingToOptions(struct MocapNET2Options * options,struct skeletonSerialized * skeleton)
{
                            //---------------------------------------------------------------------------------------------------------------------
                            //                            Changing Input 2D Point Cloud by Scaling it
                            //---------------------------------------------------------------------------------------------------------------------
                            if (options->scale!=1.0)
                                {
                                    uniformlyScaleSerializedSkeleton(skeleton,options->scale);
                                }
                            //---------------------------------------------------------------------------------------------------------------------
                            if (options->scaleX!=1.0)
                                {
                                    if (!scaleSerializedSkeletonX(skeleton,options->scaleX))
                                        {
                                            fprintf(stderr,RED "Failed to scale skeleton in X axis\n" NORMAL);
                                        }
                                }
                            //---------------------------------------------------------------------------------------------------------------------
                            if (options->scaleY!=1.0)
                                {
                                    if (!scaleSerializedSkeletonY(skeleton,options->scaleY))
                                        {
                                            fprintf(stderr,RED "Failed to scale skeleton in Y axis\n" NORMAL);
                                        }
                                }
                            //---------------------------------------------------------------------------------------------------------------------
                            if ( (options->fScaleX!=1.0) || (options->fScaleY!=1.0) )
                                {
                                    if (! scaleSerializedSkeletonFromCenter(skeleton,options->fScaleX,options->fScaleY) )
                                        {
                                            fprintf(stderr,RED "Failed to scale skeleton to approximate focal length change\n" NORMAL);
                                        }
                                }
                            //---------------------------------------------------------------------------------------------------------------------

                            if ( (options->addNormalizedPixelGaussianNoiseX>0.0) || (options->addNormalizedPixelGaussianNoiseY>0.0) )
                                {
                                    fprintf(stderr,RED "Note: You are running adding gaussian noise of std deviation %0.2f,%0.2f pixels on 2D joints\n" NORMAL,
                                            options->width * options->addNormalizedPixelGaussianNoiseX,
                                            options->height * options->addNormalizedPixelGaussianNoiseY
                                           );
                                    perturbSerializedSkeletonUsingGaussianNoise(skeleton,options->addNormalizedPixelGaussianNoiseX,options->addNormalizedPixelGaussianNoiseY);
                                }
    return 1;
}
