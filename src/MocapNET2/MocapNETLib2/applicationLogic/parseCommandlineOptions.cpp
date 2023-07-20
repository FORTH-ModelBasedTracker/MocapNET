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

void  defaultMocapNET2Options(struct MocapNET2Options * options)
{
    memset(options,0,sizeof(struct MocapNET2Options));

    options->isJSONFile=0;
    options->isCSVFile=0;
    options->webcamSource = 0;
    options->path=0;
    options->datasetPath=0;
    options->jointEstimatorUsed = JOINT_2D_ESTIMATOR_OPENPOSE; // JOINT_2D_ESTIMATOR_FORTH;

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
    options->iterations=5;
    options->epochs=30;

    //Test under Simulated Gaussian Noise
    options->addNormalizedPixelGaussianNoiseX=0.0;
    options->addNormalizedPixelGaussianNoiseY=0.0;

    options->outputPath = (char*) outputPathStatic;

    options->visualize=1;
    options->doMultiThreadedIK=0;
    options->useOpenGLVisualization=0;
    options->save3DVisualization=0;
    options->save2DVisualization=0;
    options->saveVisualization=0;
    options->saveCSV3DFile=0;
    options->constrainPositionRotation=1;

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

    options->inputFramerate = 30.0;

    options->quality=1.0;
    options->mocapNETMode=5;
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
 snprintf(command,2048,"wget -qO- \"http://ammar.gr/mocapnet/version/index.php?h=%s&v=%s\"&",hostname,MocapNETVersion);
 int i=0;
 //i = system(command); ammar.gr is not working :(
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
            if (strcmp(argv[i],"--forceOutputPositionRotation")==0)
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
            if (strcmp(argv[i],"--rotateSkeleton")==0)
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

            if (strcmp(argv[i],"--inputFramerate")==0)
                {
                    if (argc>i+1)
                        {
                           options->inputFramerate = atof(argv[i+1]);
                           fprintf(stderr,"Input Framerate set to %0.2f \n", options->inputFramerate);
                        }
                }
             else

            if (strcmp(argv[i],"--msg")==0)
                {
                    if (argc>i+1)
                        {
                           fprintf(stderr,"Message set to %s \n",argv[i+1]);
                           snprintf(options->message,512,"%s",argv[i+1]);
                        }
                }
             else
            if (strcmp(argv[i],"--map")==0)
                {
                    if (argc>i+1)
                        {
                           fprintf(stderr,"Configuring Map to use file %s \n",argv[i+1]);
                          snprintf(options->mapFilePath,512,"%s",argv[i+1]);
                          options->visualizationType=2;
                        }
                }
             else
            if (strcmp(argv[i],"--unconstrained")==0)
                {
                    options->constrainPositionRotation=0;
                }
            else if  (  (strcmp(argv[i],"-o")==0) || (strcmp(argv[i],"--output")==0) )
                {
                  if (argc>i+1)
                        {
                         options->outputPath=argv[i+1];
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if  (strcmp(argv[i],"--dontbend")==0)
            {
                options->dontBend=1;
            }
            else if  (strcmp(argv[i],"--opengl")==0)
            {
                options->useOpenGLVisualization=1;
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
            else if (strcmp(argv[i],"--noik")==0)
                {
                    options->useInverseKinematics=0;
                }
            else if (strcmp(argv[i],"--ik")==0)
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
            else if (strcmp(argv[i],"--nv")==0)
                {
                    fprintf(stderr,"Visualization disabled\n");
                    options->visualize=0;
                }
            else if (strcmp(argv[i],"--novisualization")==0)
                {
                    fprintf(stderr,"Visualization disabled\n");
                    options->visualize=0;
                }
            else if (strcmp(argv[i],"-v")==0)
                {
                    fprintf(stderr,"Visualization enabled\n");
                    options->visualize=1;
                }
            else if (strcmp(argv[i],"--visualize")==0)
                {
                    fprintf(stderr,"Visualization enabled\n");
                    options->visualize=1;
                }
            else if (strcmp(argv[i],"--frameskip")==0)
                {
                   if (argc>i+1)
                        {
                          options->frameSkip=atoi(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],"--nofilter")==0)
                {
                    fprintf(stderr,"Filtering Disabled\n");
                    options->doOutputFiltering=0;
                }
            else if (strcmp(argv[i],"--scale")==0)
                {
                  if (argc>i+1)
                        {
                         options->scale=atof(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],"--scaleX")==0)
                {
                    if (argc>i+1)
                        {
                         options->scaleX=atof(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],"--scaleY")==0)
                {
                    if (argc>i+1)
                        {
                          options->scaleY=atof(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],"--focalLength")==0)
                {
                    overrideBVHSubsystemFocalLength(atof(argv[i+1]),atof(argv[i+2]));
                }
            else if (strcmp(argv[i],"--fscale")==0)
                {
                    if (argc>i+2)
                        {
                         options->fScaleX=atof(argv[i+1]);
                         options->fScaleY=atof(argv[i+2]);
                        } else
                        { argumentError(i,2,argc,argv); }
                }
            else if (strcmp(argv[i],"--noise")==0)
                {
                    if (argc>i+2)
                        {
                         options->addNormalizedPixelGaussianNoiseX=atof(argv[i+1]);
                         options->addNormalizedPixelGaussianNoiseY=atof(argv[i+2]);
                        } else
                        { argumentError(i,2,argc,argv); }
                }

            else if (strcmp(argv[i],"--front")==0)
                {
                    options->forceFront=1;
                }
            else if (strcmp(argv[i],"--back")==0)
                {
                    options->forceBack=1;
                }
            else if (strcmp(argv[i],"--left")==0)
                {
                    options->forceLeft=1;
                }
            else if (strcmp(argv[i],"--right")==0)
                {
                    options->forceRight=1;
                }
            else if (strcmp(argv[i],"--bvhcenter")==0)
                {
                    options->bvhCenter=1;
                }
            else if (strcmp(argv[i],"--forth")==0)
                {
                    options->jointEstimatorUsed = JOINT_2D_ESTIMATOR_FORTH;
                }
            else if (strcmp(argv[i],"--openpose")==0)
                {
                    options->jointEstimatorUsed = JOINT_2D_ESTIMATOR_OPENPOSE;
                }
            else if (strcmp(argv[i],"--vnect")==0)
                {
                    options->jointEstimatorUsed = JOINT_2D_ESTIMATOR_VNECT;
                }
            else if (strcmp(argv[i],"--frames")==0)
                {
                  if (argc>i+1)
                        {
                         options->frameLimit=atoi(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],"--maxFrames")==0)
                {
                  if (argc>i+1)
                        {
                         options->frameLimit=atoi(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],"--from")==0)
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
            else if (strcmp(argv[i],"--quality")==0)
                {
                 if (argc>i+1)
                        {
                         options->quality=atof(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
             else
                if (strcmp(argv[i],"--mt")==0)
                    {
                       options->doMultiThreadedIK=1;
                    }
             else
                if (strcmp(argv[i],"--cpu")==0)
                    {
                        options->useCPUOnlyForMocapNET=1;
                        options->useCPUOnlyFor2DEstimator=1;
                    }
            else
                //if (strcmp(argv[i],"--cpu")==0)        { setenv("CUDA_VISIBLE_DEVICES", "", 1); } else
                if (strcmp(argv[i],"--gpu")==0)
                    {
                        options->useCPUOnlyForMocapNET=0;
                        options->useCPUOnlyFor2DEstimator=0;
                    }
            else if (strcmp(argv[i],"--delay")==0)
                {
                    //If you want to take some time to check the results that
                    //might otherwise pass by very fast
                    if (argc>i+1)
                        {
                          options->delay=atoi(argv[i+1]);
                        } else
                        { argumentError(i,1,argc,argv); }
                }
            else if (strcmp(argv[i],"--skip")==0)
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
            else if (strcmp(argv[i],"--tpose")==0)
                {
                   options-> prependTPose=1;
                }
            else if (strcmp(argv[i],"--nolowerbody")==0)
                {
                    options->doLowerBody=0;
                }
            else if (strcmp(argv[i],"--show")==0)
                {
                    options->visualizationType=atoi(argv[i+1]);
                }
            else if (strcmp(argv[i],"--nohands")==0)
                {
                    options->doHands=0;
                }
            else if (strcmp(argv[i],"--hands")==0)
                {
                    options->doHands=1;
                }
            else if (strcmp(argv[i],"--face")==0)
                {
                    options->doFace=1;
                }
            else if (strcmp(argv[i],"--noface")==0)
                {
                    options->doFace=0;
                }
                else if (strcmp(argv[i],"--label")==0)
                    {
                      if (argc>i+1)
                        {
                         options->label = argv[i+1];
                        } else
                        { argumentError(i,1,argc,argv); }
                    }
                 else if (strcmp(argv[i],"--seriallength")==0)
                    {
                        if (argc>i+1)
                        {
                          options->serialLength = atoi(argv[i+1]);
                        }  else
                        { argumentError(i,1,argc,argv); }
                    }
                else if  (  (strcmp(argv[i],"-o")==0) || (strcmp(argv[i],"--output")==0) )
                    {
                        if (argc>i+1)
                        {
                          options->outputPath=argv[i+1];
                        }  else
                        { argumentError(i,1,argc,argv); }
                    }
                else if (strcmp(argv[i],"--mode")==0)
                    {
                        if (argc>i+1)
                        {
                          options->mocapNETMode=atoi(argv[i+1]);
                        }  else
                        { argumentError(i,1,argc,argv); }
                    }
                else if (strcmp(argv[i],"--size")==0)
                    {
                        if (argc>i+2)
                        {
                         options->width = atoi(argv[i+1]);
                         options->height = atoi(argv[i+2]);
                        }  else
                        { argumentError(i,2,argc,argv); }
                    }
                else if (strcmp(argv[i],"--visualizationSize")==0)
                    {
                        if (argc>i+2)
                        {
                          options->visWidth = atoi(argv[i+1]);
                          options->visHeight = atoi(argv[i+2]);
                        }  else
                        { argumentError(i,2,argc,argv); }
                    }
                else if (strcmp(argv[i],"--gestures")==0)
                    {
                        options->doGestureDetection=1;
                    }
        }

return 1;
}


int  loadOptionsAfterBVHLoadFromCommandlineOptions(struct MocapNET2Options * options,int argc, char *argv[])
{
       options->hasInit=0;
        for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--changeFeetDimensions")==0)
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
            if (strcmp(argv[i],"--scaleAllJointDimensions")==0)
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
            else if (strcmp(argv[i],"--changeJointDimensions")==0)
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
