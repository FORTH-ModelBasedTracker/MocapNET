#include "opencv2/opencv.hpp"
/** @file livedemo.cpp
 *  @brief This is the main "demo" offered in this repository, it will take a stream from a webcam or video file using OpenCV and run
*   2D pose estimation + MocapNET giving you a nice 3D visualization as well as an output .bvh file
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include <stdio.h>
#include <unistd.h>
//-----------------------------------------------------------------
#include "../../JointEstimator2D/cameraControl.hpp"
#include "../../JointEstimator2D/jointEstimator2D.hpp"
#include "../../JointEstimator2D/visualization.hpp"
//-----------------------------------------------------------------
#include "../../MocapNET2/MocapNETLib2/mocapnet2.hpp"
#include "../../MocapNET2/MocapNETLib2/applicationLogic/parseCommandlineOptions.hpp"
#include "../../MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp"
#include "../../MocapNET2/MocapNETLib2/IO/conversions.hpp"
#include "../../MocapNET2/MocapNETLib2/IO/bvh.hpp"
#include "../../MocapNET2/MocapNETLib2/IO/csvRead.hpp"
#include "../../MocapNET2/MocapNETLib2/IO/csvWrite.hpp"
#include "../../MocapNET2/MocapNETLib2/IO/skeletonAbstraction.hpp"
//-----------------------------------------------------------------
#include "../../MocapNET2/MocapNETLib2/visualization/visualization.hpp"
#include "../../MocapNET2/MocapNETLib2/visualization/map.hpp"
//-----------------------------------------------------------------
#include "../../MocapNET2/MocapNETLib2/tools.hpp"
using namespace cv;


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


int main(int argc, char *argv[])
{
    struct MocapNET2Options options= {0};
    struct MocapNET2 mnet= {0};
    mnet.options = & options;

    struct JointEstimator2D jointEstimator;

    std::vector<float> inputValues;
    std::vector<std::vector<float> > exactMocapNET2DOutput;
    std::vector<std::vector<float> > output3DPositions;
    std::vector<float> points3DFlatOutput;

    struct skeletonSerialized resultAsSkeletonSerialized= {0};

    float frameRateSummary = 0.0;
    unsigned int frameSamples=0;

    defaultMocapNET2Options(&options);

   /*
    *  Force effortless IK configuration on Webcam Demo
    */
    //Be unconstrained by default
     options.constrainPositionRotation=0;
     //Use IK  ========
     options.useInverseKinematics=1;
     options.learningRate=0.01;
     options.iterations=5;
     options.epochs=30.0;
     options.spring=1.0;
     //==============

    //640x480 should be a high framerate compatible resolution
    //for most webcams, you can change this using --size X Y commandline parameter
    options.width = 640;
    options.height = 480;


    loadOptionsFromCommandlineOptions(&options,argc,argv);

    std::cerr<<"Trying to open source ("<<options.webcamSource<<") \n";
    VideoCapture cap(options.webcamSource); // open the default camera

     int itIsTheFirstFrame=1;
     Mat singleImageFrame;
     Mat frame;
     Mat frameCentered;

     if ( 
          (strstr(options.webcamSource,".jpg")!=0) ||
          (strstr(options.webcamSource,".png")!=0)
        )
     {
       std::cerr<<"Source seems to be a single image ("<<options.webcamSource<<") \n";
       options.frameLimit = 20; //let it warmup for a few frames..
       options.inputIsSingleImage = 1;
       options.doOutputFiltering  = 0; //There is no motion to filter so skip this
       singleImageFrame=imread(options.webcamSource);
       singleImageFrame.copyTo(frame);
     } else     
     if (strstr(options.webcamSource,"/dev/video")!=0)
     {
       std::cerr<<"Source seems to be a webcam ("<<options.webcamSource<<" @ "<<options.width<<","<<options.height<<") \n";
       options.inputIsSingleImage = 0;
       cap.set(cv::CAP_PROP_FRAME_WIDTH,options.width);
       cap.set(cv::CAP_PROP_FRAME_HEIGHT,options.height);
     }

     //This needs to be grabbed to initialize window sizes either on video source, or on webcam source
     //--------------------------------------------------------------------
     if (!options.inputIsSingleImage)
                          {  cap >> frame; } 
     //--------------------------------------------------------------------

     //We will accept the input resolution and force it
     //on visualization..
     options.width     = frame.size().width;
     options.height    = frame.size().height;
     options.visWidth  = frame.size().width;
     options.visHeight = frame.size().height;
     //-----------------------------------------------------


    //We might want to load a special bvh file based on our options..!
    loadOptionsAfterBVHLoadFromCommandlineOptions(&options,argc,argv);

    //If the initialization didnt happen inside the previous call lets do it now
    if (!options.hasInit)
            {
               if (initializeBVHConverter(0,options.visWidth,options.visHeight,0))
                 {
                   fprintf(stderr,"BVH code initalization successfull..\n");
                   options.hasInit=1;
                 }
            }
            //--------------------------------------------------------------------------

    //Switch to realtime priority before opening tensorflow stuff..
    requestRealtimePriority();

    if (loadJointEstimator2D(
                             &jointEstimator,
                             options.
                             jointEstimatorUsed,
                             1,
                             options.useCPUOnlyFor2DEstimator
                            ))
        {
            if ( loadMocapNET2(&mnet,"Live Demo") )
                {
                    //------------------------------
                    mnet.learningRate=options.learningRate;
                    mnet.iterations=options.iterations;
                    mnet.epochs=options.epochs;
                    mnet.spring=options.spring;
                    //------------------------------

                    cv::Mat viewMat = Mat(Size(jointEstimator.inputWidth2DJointDetector,jointEstimator.inputHeight2DJointDetector),CV_8UC3, Scalar(0,0,0));

                    struct Skeletons2DDetected skeleton2DEstimations= {0};

                    if (options.visualize)
                    {
                     //cv::namedWindow("Video Input Feed",1);
                     //cv::moveWindow("Video Input Feed",0,368);
                     cv::namedWindow("3D Points Output",1);
                     cv::moveWindow("3D Points Output",0,0);
                     cv::namedWindow("Skeletons",1);
                     cv::moveWindow("Skeletons",1920-jointEstimator.inputWidth2DJointDetector-50,100);
                    }

                    unsigned int frameID=0;
                    unsigned int skippedFramesInARow=0;


                    std::vector<float> result;
                    std::vector<float> previousResult;

                    float totalTime=0.0;
                    unsigned int totalSamples=0;

                    std::vector<std::vector<float> > bvhFrames;
                    struct skeletonSerialized skeleton= {0};

                       while ( (options.frameLimit==0) || (frameID<options.frameLimit) )
                        {
                            options.loopStartTime = GetTickCountMicrosecondsMN();

                            if (options.inputIsSingleImage) { singleImageFrame.copyTo(frame); } else
                            if (itIsTheFirstFrame)          { itIsTheFirstFrame=0;            } else
                                                            { cap >> frame;                   }

                            //If we are running in a low-end computer and need to keep in sync with a live video feed we can frame-skip
                                 if (options.frameSkip)
                                {
                                    for (int i=0; i<options.frameSkip; i++)
                                        {
                                            cap >> frame;
                                        }
                                }

                            frame.copyTo(frameCentered);
                            if ( (frameCentered.size().width>0) && (frameCentered.size().height>0) )
                                {
                                    if (
                                        !cropAndResizeCVMatToMatchSkeleton(
                                            &jointEstimator,
                                            frameCentered,
                                            &skeleton2DEstimations
                                        )
                                    )
                                        {
                                            fprintf(stderr,"Failed to crop input video\n");
                                        }
                                    //imshow("Video Input Feed", frameCentered);

                                    frameCentered.copyTo(viewMat);
                                    // viewMat.setTo(Scalar(0,0,0));

                                    //Tensorflow works with Floating point input so we need to convert our buffer..
                                    frameCentered.convertTo(frameCentered,CV_32FC3);

                                    //At this point we are ready to execute the neural network
                                    long startTime2D = GetTickCountMicrosecondsMN();

                                    //We count the framerate of our acquisition
                                    options.fpsAcquisition = convertStartEndTimeFromMicrosecondsToFPS(options.loopStartTime,startTime2D);


                                    std::vector<std::vector<float> >  heatmaps = getHeatmaps(
                                                &jointEstimator,
                                                frameCentered.data,
                                                jointEstimator.inputWidth2DJointDetector,
                                                jointEstimator.inputHeight2DJointDetector
                                            );
                                    if (heatmaps.size()>0)
                                        {
                                            //This will spam with small heatmap windows
                                            //visualizeHeatmaps(&jointEstimator,heatmaps,frameID);

                                            estimate2DSkeletonsFromHeatmaps(&jointEstimator,&skeleton2DEstimations,heatmaps);

                                            long endTime2D = GetTickCountMicrosecondsMN();

                                            options.fps2DEstimator = convertStartEndTimeFromMicrosecondsToFPS(startTime2D,endTime2D);

                                            if (options.visualize)
                                            {
                                               dj_drawExtractedSkeletons(
                                                viewMat,
                                                &skeleton2DEstimations,
                                                jointEstimator.inputWidth2DJointDetector,
                                                jointEstimator.inputHeight2DJointDetector
                                                );
                                            }

                                            float percentageOf2DPointsMissing = percentOf2DPointsMissing(&skeleton2DEstimations);
                                            if (  percentageOf2DPointsMissing  < 50.0 ) //only work when less than 50% of information missing..
                                                {
                                                    skippedFramesInARow=0;
                                                    //We want to go from the original normalized values of skeleton2DEstimations to the original
                                                    //Resolution we grabbed our initial frame @ before cropping..
                                                    restore2DJointsToInputFrameCoordinates(&jointEstimator,&skeleton2DEstimations);

                                                    //Now that our points have their initial size let's perform a conversion to the internal
                                                    //serialized skeleton data structure that will prepare them for use in MocapNET
                                                    convertSkeletons2DDetectedToSkeletonsSerialized(
                                                        &skeleton,
                                                        &skeleton2DEstimations,
                                                        frameID,
                                                        jointEstimator.crop.frameWidth,
                                                        jointEstimator.crop.frameHeight
                                                    );

                                                    takeCareOfScalingInputAndAddingNoiseAccordingToOptions(&options,&skeleton);

                                                    unsigned int feetAreMissing=areFeetMissing(&skeleton);


                                                    long startTime = GetTickCountMicrosecondsMN();
                                                    //--------------------------------------------------------
                                                    previousResult = result;
                                                    result = runMocapNET2(
                                                                 &mnet,
                                                                 &skeleton,
                                                                 ( (options.doLowerBody) && (!feetAreMissing) ),
                                                                 options.doHands,
                                                                 options.doFace,
                                                                 options.doGestureDetection,
                                                                 options.useInverseKinematics,
                                                                 options.doOutputFiltering
                                                             );
                                                    bvhFrames.push_back(result);
                                                    //--------------------------------------------------------
                                                    long endTime = GetTickCountMicrosecondsMN();
                                                    options.fpsMocapNET = convertStartEndTimeFromMicrosecondsToFPS(startTime,endTime);
                                                    frameRateSummary += options.fpsMocapNET;
                                                    ++frameSamples;
                                                    //--------------------------------------------------------


                                                    options.numberOfMissingJoints = upperbodyCountMissingNSDMElements(mnet.upperBody.NSDM,0 /*Dont spam */);
                                                    //Don't spam with missing joints..
                                                    //fprintf(stderr,"Number of missing joints for UpperBody %u\n",options.numberOfMissingJoints);

                                                    //Convert BVH frame to 2D points to show on screen
                                                    exactMocapNET2DOutput = convertBVHFrameTo2DPoints(result);//,MocapNETTrainingWidth,MocapNETTrainingHeight);

                                                    if (options.saveCSV3DFile)
                                                        {
                                                            //Convert BVH frame to 3D points to output on a file
                                                            points3DFlatOutput=convertBVHFrameToFlat3DPoints(result);//,MocapNETTrainingWidth,MocapNETTrainingHeight);
                                                            output3DPositions.push_back(points3DFlatOutput); //3d Input
                                                        }

                                                    resultAsSkeletonSerialized.skeletonHeaderElements = skeleton.skeletonHeaderElements;
                                                    resultAsSkeletonSerialized.skeletonBodyElements   = skeleton.skeletonBodyElements;
                                                    if (
                                                        convertMocapNET2OutputToSkeletonSerialized(
                                                            &mnet,
                                                            &resultAsSkeletonSerialized,
                                                            exactMocapNET2DOutput,
                                                            frameID,
                                                            MocapNETTrainingWidth,
                                                            MocapNETTrainingHeight
                                                         )
                                                       )
                                                        {
                                                            //TODO : Compare resultAsSkeletonSerialized and skeleton
                                                            doReprojectionCheck(&skeleton,&resultAsSkeletonSerialized);
                                                        }

                                                }
                                            else
                                                {
                                                    if (skippedFramesInARow%30==0)
                                                                   { fprintf(stderr,"."); }
                                                    ++skippedFramesInARow;
                                                }

                                            if (options.visualize)
                                                {
                                                    visualizationCommander(
                                                        &mnet,
                                                        &options,
                                                        &skeleton,
                                                        &frame,
                                                        result,
                                                        exactMocapNET2DOutput,
                                                        frameID,
                                                        1// We will do the waitKey call ourselves
                                                    );

                                                   if ( (viewMat.size().width >0) && (viewMat.size().height>0) )
                                                      { imshow("Skeletons", viewMat); } else 
                                                      { std::cerr<<"Invalid skeleton visualization frame.. \n"; }
                                                }
                                        }
                                }
                            else
                                {
                                    std::cerr<<"Broken frame.. \n";
                                    ++options.brokenFrames;
                                    if (options.brokenFrames>10)
                                        {
                                            std::cerr<<"Too many broken frames.. \n";
                                            break;
                                        }
                                }

                            options.loopEndTime = GetTickCountMicrosecondsMN();

                            options.totalLoopFPS = convertStartEndTimeFromMicrosecondsToFPS(options.loopStartTime,options.loopEndTime);


                            //------------------------------------------------------
                            // These final calls add delays to frame processing so
                            // they are not counted in loop time
                            //------------------------------------------------------


                            //Frames should increment even when visualization is off.. 
                            ++frameID;


                            if (options.visualize)
                                            {
                            char key = 0;
                            if (options.delay!=0)
                                {
                                    key = waitKey(options.delay);
                                } else
                                {
                                    key = waitKey(1);
                                }

                            if (key==27)
                            {
                                fprintf(stderr,GREEN "Received Escape key from UI, terminating the application.." NORMAL);
                                break;
                            }
                                            }

                            if (options.delay!=0)
                                {
                                    nsleep(options.delay*1000);
                                }
                            //------------------------------------------------------
                        } // End of grabber loop


                    if (options.bvhCenter)
                        {
                            for (unsigned int i=0; i<bvhFrames.size(); i++)
                                {
                                    bvhFrames[i][0]=0;
                                    bvhFrames[i][1]=0;
                                    bvhFrames[i][2]=0;
                                }
                        }

                    if (options.dontBend)
                       {
                         for (unsigned int i=0; i<bvhFrames.size(); i++)
                                {
                                   if (bvhFrames[i][3]>10)  { bvhFrames[i][3]=10; } else
                                   if (bvhFrames[i][3]<-10) { bvhFrames[i][3]=-10; }
                                }
                       }

                    //fix https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/35
                    fixBVHHip(bvhFrames);

                    if ( writeBVHFile(options.outputPath,0,options.prependTPose,bvhFrames) )
                        {
                            fprintf(stderr,GREEN "Successfully wrote %lu frames to bvh file.. \n" NORMAL,bvhFrames.size());
                        }
                    else
                        {
                            fprintf(stderr,RED "Failed to write %lu frames to bvh file.. \n" NORMAL,bvhFrames.size());
                        }

                    if (options.saveCSV3DFile)
                        {
                            fprintf(stderr,"Will now write 3D output to out3DP.csv.. \n");
                            //----------------------------------------------------------------------------------------------------------------------------------
                            if ( writeCSVHeaderFromLabelsAndVectorOfVectors("out3DP.csv",MocapNET3DPositionalOutputArrayNames,MOCAPNET_3DPOINT_NUMBER,output3DPositions) )
                                {
                                    fprintf(stderr,GREEN "Successfully wrote %lu frames to csv file.. \n" NORMAL,output3DPositions.size());
                                }
                            else
                                {
                                    fprintf(stderr,RED "Failed to write %lu frames to bvh file.. \n" NORMAL,output3DPositions.size());
                                }
                        }


                    if (  (options.save3DVisualization) || (options.save2DVisualization) )
                        {
                            int highResEncoding=1;
                            char formatString[256];

                            if (highResEncoding)
                            {
                              snprintf(formatString,256,"ffmpeg -framerate %f -i vis%%05d.jpg  -s 1200x720  -y -r %f -pix_fmt yuv420p -threads 8 livelastRun3DHiRes.mp4 && rm ./*.jpg",options.inputFramerate,options.inputFramerate);
                            } else
                            {
                              snprintf(formatString,256,"ffmpeg -framerate %f -i vis%%05d.jpg -y -r %f -threads 8 -crf 9 -pix_fmt yuv420p lastRun3D.webm && rm ./*.jpg",options.inputFramerate,options.inputFramerate);
                            }
                            int i=system(formatString);


                            if (i==0)
                                {
                                    fprintf(stderr,"Successfully wrote video file.. \n");
                                }
                            else
                                {
                                    fprintf(stderr,"Failed to write a video file.. \n");
                                }
                        }

                    unloadMocapNET2(&mnet);



                } //3D pose estimator ok
        } //2D joint estimator ok

    //Offer a summary of system and the achieved framerate..!
    if (frameSamples!=0)
    {
     fprintf(stderr,"\n\nMocapNET v%s execution summary :\n",MocapNETVersion);
     fprintf(stderr,"__________________________________________\n");
     //neuralNetworkPrintVersion();
     printBVHCodeVersion();
     fprintf(stderr,"CPU : %s \n",options.CPUName);
     fprintf(stderr,"GPU : %s \n",options.GPUName);
     fprintf(stderr,"Average framerate for %u samples was %0.2f fps \n",frameSamples,((float) frameRateSummary/frameSamples) );

     //Offer some info on options of the run executed..
     //-------------------------------------------------
     if (options.doMultiThreadedIK)
                         { fprintf(stderr,"Multi-threading was on\n"); }
     //-------------------------------------------------
     if (codeOptimizationsForIKEnabled())
                         { fprintf(stderr,"Code optimizations where on\n"); }     
     //-------------------------------------------------
     if (options.jointEstimatorUsed==JOINT_2D_ESTIMATOR_FORTH)
                         { fprintf(stderr,"You can achieve better accuracy by using the homebrewed OpenPose 2D joint estimator using --openpose\n"); }
     //-------------------------------------------------
     if (options.jointEstimatorUsed==JOINT_2D_ESTIMATOR_OPENPOSE)
                         { fprintf(stderr,"You can achieve faster framerates with the bundled FORTH 2D joint estimator using --forth\n"); }
     //-------------------------------------------------
    }

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
