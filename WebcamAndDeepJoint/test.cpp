/** @file test.cpp
 *  @brief This is an all-in-one live demo. It combines data acquisition using OpenCV, 2D Joint estimation using Tensorflow ( via VNECT/OpenPose/FORTH estimators ),
 *  and 3D BVH output using MocapNET.
 *  @author Ammar Qammaz (AmmarkoV)

 */

#include "opencv2/opencv.hpp"
using namespace cv;

#include <cstdlib>
#include <unistd.h>

#include "../MocapNETLib/jsonCocoSkeleton.h"
#include "../MocapNETLib/jsonMocapNETHelpers.cpp"

#include "../Tensorflow/tensorflow.hpp"
#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/bvh.hpp"
#include "../MocapNETLib/gestureRecognition.hpp"
#include "../MocapNETLib/opengl.hpp"
#include "../MocapNETLib/tools.h"
#include "../MocapNETLib/visualization.hpp"

#include "cameraControl.hpp"
#include "utilities.hpp"


#define DISPLAY_ALL_HEATMAPS 0



/**
 * @brief This function performs 2D estimation.. You give her a Tensorflow instance of a 2D estimator, a BGR image some thresholds and sizes and it will yield a vector of 2D points.
 * @ingroup demo
 * @bug This code is oriented to a single 2D skeleton detected, Multiple skeletons will confuse it and there is no logic to handle them
 * @retval A 2D Skeleton detected in the bgr OpenCV image
 */
std::vector<cv::Point_<float> > predictAndReturnSingleSkeletonOf2DCOCOJoints(
    struct TensorflowInstance * net,
    const cv::Mat &bgr ,
    float minThreshold,
    int visualize ,
    int saveVisualization,
    unsigned int frameNumber,
    unsigned int inputWidth2DJointDetector,
    unsigned int inputHeight2DJointDetector,
    unsigned int heatmapWidth2DJointDetector,
    unsigned int heatmapHeight2DJointDetector,
    unsigned int numberOfHeatmaps,
    unsigned int numberOfOutputTensors
)
{
    // preprocess image. Actually resize
    float scaleX = (float) inputWidth2DJointDetector/bgr.cols;
    float scaleY = (float) inputHeight2DJointDetector/bgr.rows;
    cv::Mat fr_res;
    cv::resize(bgr, fr_res, cv::Size(0,0), scaleX, scaleY);
    cv::Mat smallBGR = fr_res;//ECONOMY .clone();

    if (visualize)
        {
            //This is not very useful anymore
            //cv::imshow("BGR",fr_res);
        }
    fr_res.convertTo(fr_res,CV_32FC3);
    // pass the frame to the Estimator


    std::vector<std::vector<float> > result = predictTensorflowOnArrayOfHeatmaps(
                net,
                (unsigned int) fr_res.cols,
                (unsigned int) fr_res.rows,
                (float*) fr_res.data,
                heatmapWidth2DJointDetector,
                heatmapHeight2DJointDetector,
                numberOfOutputTensors
            );

    if (result.size()<3)
        {
            fprintf(stderr,"Our 2D neural network did not produce an array of 2D heatmaps..\n");
            fprintf(stderr,"Cannot continue with this output...\n");
            std::vector<cv::Point_<float> > emptyVectorOfPoints;
            return emptyVectorOfPoints;
        }


    unsigned int rows = heatmapWidth2DJointDetector;
    unsigned int cols = heatmapHeight2DJointDetector;
    unsigned int hm = numberOfHeatmaps;
    std::vector<cv::Mat> heatmaps;
    for(int i=0; i<hm; ++i)
        {
            cv::Mat h(rows,cols, CV_32FC1);
            for(int r=0; r<rows; ++r)
                {
                    for(int c=0; c<cols; ++c)
                        {
                            int pos = r*cols+c;
                            h.at<float>(r,c) = result[i][pos];
                        }
                }
            heatmaps.push_back(h);
        }


    //This code segment will display every returned heatmap in it's own window..
#if DISPLAY_ALL_HEATMAPS
    if (visualize)
        {
            unsigned int x=0;
            unsigned int y=0;
            char windowLabel[512];
            for(int i=0; i<18; ++i)
                {
                    snprintf(windowLabel,512,"Heatmap %u",i);
                    if (frameNumber==0)
                        {
                            cv::namedWindow(windowLabel,1);
                            cv::moveWindow(windowLabel, x,y);
                        }
                    cv::imshow(windowLabel,heatmaps[i]);
                    y=y+rows+30;
                    if (y>700)
                        {
                            x=x+cols;
                            y=0;
                        }
                }
        }
#endif // DISPLAY_ALL_HEATMAPS

    return dj_getNeuralNetworkDetectionsForColorImage(bgr,smallBGR,heatmaps,minThreshold,frameNumber,visualize,saveVisualization,0);
}





int feetHeuristics(struct skeletonCOCO * sk)
{
    //There are various problems with the 2D detections when it comes to feet
    //one pretty regular problem is that knees get somehow mixed up
    if (sk!=0)
        {
            if (
                ( sk->joint2D[BODY25_RAnkle] .x< sk->joint2D[BODY25_LAnkle].x ) &&
                ( sk->joint2D[BODY25_RKnee].x  > sk->joint2D[BODY25_LKnee].x )  &&
                ( sk->joint2D[BODY25_RHip].x < sk->joint2D[BODY25_LHip].x )
               )
                {
                    struct point2D tmp = sk->joint2D[BODY25_RKnee];
                    sk->joint2D[BODY25_RKnee]=sk->joint2D[BODY25_LKnee];
                    sk->joint2D[BODY25_LKnee] = tmp;
                    return 1;
                }
            else if (
                     ( sk->joint2D[BODY25_RAnkle] .x > sk->joint2D[BODY25_LAnkle].x ) &&
                     ( sk->joint2D[BODY25_RKnee].x  < sk->joint2D[BODY25_LKnee].x )  &&
                     ( sk->joint2D[BODY25_RHip].x < sk->joint2D[BODY25_LHip].x )
                    )
                {
                    struct point2D tmp = sk->joint2D[BODY25_RAnkle];
                    sk->joint2D[BODY25_RAnkle]=sk->joint2D[BODY25_LAnkle];
                    sk->joint2D[BODY25_LAnkle] = tmp;
                    return 1;
                }
        }
    return 0;
}


/**
 * @brief Retrieve MocapNET output vector from an image
 * @ingroup demo
 * @bug This code is oriented to a single 2D skeleton detected, Multiple skeletons will confuse it and there is no logic to handle them
 * @retval Vector of MocapNET output, in case of error it might be empty..!
 */
std::vector<float> returnMocapNETInputFrom2DDetectorOutput(
    struct TensorflowInstance * net,
    const cv::Mat &bgr,
    struct boundingBox *bbox,
    std::vector<std::vector<float> > & points2DInput,
    float minThreshold,
    int visualize ,
    int saveVisualization,
    float * fps,
    unsigned int frameNumber,
    unsigned int offsetX,
    unsigned int offsetY,
    unsigned int stolenWidth,
    unsigned int stolenHeight,
    unsigned int inputWidth2DJointDetector,
    unsigned int inputHeight2DJointDetector,
    unsigned int heatmapWidth2DJointDetector,
    unsigned int heatmapHeight2DJointDetector,
    unsigned int numberOfHeatmaps,
    unsigned int numberOfOutputTensors,
    int doFeetHeuristics
)
{
    unsigned int frameWidth  =  bgr.size().width; //frame.cols
    unsigned int frameHeight =  bgr.size().height; //frame.rows

    unsigned long startTime  = GetTickCountMicroseconds();
    std::vector<cv::Point_<float> > pointsOf2DSkeleton = predictAndReturnSingleSkeletonOf2DCOCOJoints(
                net,
                bgr,
                minThreshold,
                visualize,
                saveVisualization,
                frameNumber,
                inputWidth2DJointDetector,
                inputHeight2DJointDetector,
                heatmapWidth2DJointDetector,
                heatmapHeight2DJointDetector,
                numberOfHeatmaps,
                numberOfOutputTensors
            );
    unsigned long endTime = GetTickCountMicroseconds();
    *fps = convertStartEndTimeFromMicrosecondsToFPS(startTime,endTime);

    if (!visualize)
        {
            //If we don't visualize using OpenCV output performance
            fprintf(stderr,"OpenPose 2DSkeleton @ %0.2f fps \n",*fps);
        }



    unsigned int i=0;
    struct skeletonCOCO sk= {0};


    if (pointsOf2DSkeleton.size()>=(UT_COCO_PARTS-1))
        {
            // Extract bounding box..

            populateBoundingBox(
                bbox,
                pointsOf2DSkeleton
            );

            for (i=0; i<pointsOf2DSkeleton.size()-1; i++)
                {
                    pointsOf2DSkeleton[i].x+=offsetX;
                    pointsOf2DSkeleton[i].y+=offsetY;
                }

            convertUtilitiesSkeletonFormatToBODY25(&sk,pointsOf2DSkeleton);

            if (doFeetHeuristics)
                {
                    if (feetHeuristics(&sk))
                        {
                            fprintf(stderr,"Feet heuristics changed something.. \n");
                        }
                }

            //----------------------------------------------------------------------------------
            //             Recover points to parent function
            //----------------------------------------------------------------------------------
            points2DInput.clear();
            for (i=0; i<BODY25_PARTS-1; i++)
                {
                    std::vector<float> newPoint;
                    newPoint.push_back(sk.joint2D[i].x);
                    newPoint.push_back(sk.joint2D[i].y);
                    points2DInput.push_back(newPoint);
                }
            //----------------------------------------------------------------------------------

        }
    else
        {
            fprintf(stderr,"Cannot Flatten Empty Skeleton (Got %lu points and had to have at least %u)...\n",pointsOf2DSkeleton.size(),(UT_COCO_PARTS-1));
            std::vector<float> emptyVector;
            return emptyVector;
        }


    return flattenskeletonCOCOToVector(&sk,frameWidth+stolenWidth,frameHeight+stolenHeight);
}


unsigned int getNumberOfEmptyNSDMElements(std::vector<float> mocapNETInput)
{
    unsigned int numberOfZeros=0;

    for (int i=0; i<mocapNETInput.size(); i++)
        {
            if (mocapNETInput[i]==0)
                {
                    ++numberOfZeros;
                }

        } 
    return numberOfZeros;
}


int populateDemoFigures(cv::Mat * figures)
{
    fprintf(stderr,"Populating Demo Figures.. You probably don't need that, but I did in Researcher's night 2019..\n");
    float reduction=0.5;
    figures[0] = cv::imread("doc/figureA.png", CV_LOAD_IMAGE_COLOR);   // Read the file
    cv::resize(figures[0],figures[0], cv::Size(), reduction,reduction);
    figures[1] = cv::imread("doc/figureB.png", CV_LOAD_IMAGE_COLOR);   // Read the file
    cv::resize(figures[1],figures[1], cv::Size(), reduction, reduction);
    // figures[2] = cv::imread("doc/figureC.png", CV_LOAD_IMAGE_COLOR);   // Read the file
    // cv::resize(figures[2],figures[2], cv::Size(), reduction, reduction);
    return 1;
}

int adjustInputSamplingToMatchFramerateTarget(
    cv::VideoCapture &cap,
    cv::Mat  &frame,
    unsigned int frameNumber,
    float fpsTarget,
    float fpsTotal,
    float fpsAcquisition,
    float fps2DJointDetector,
    float fpsMocapNET
)
{
    if (frameNumber<10)
        {
            fprintf(stderr,"Allow some frames to correctly measure framerates..\n");
            return 0;
        }


    if (fpsTarget==fpsTotal)
        {
            fprintf(stderr,"Running @ %0.2f as the user requested..\n",fpsTotal);
            return 1;
        }
    else if (fpsTarget<fpsTotal)
        {
            fprintf(stderr,"We are running @ %0.2f which is faster than the user target which is %0.2f\n",fpsTotal,fpsTarget);

            float sleepPerSecond=fpsTotal-fpsTarget;
            unsigned int sleepMicroseconds=(unsigned int) sleepPerSecond*1000;
            fprintf(stderr,"We will make up for it by sleeping %0.2f frames per second ( %u μs) \n",sleepPerSecond,sleepMicroseconds);
            usleep(sleepMicroseconds);
            return 2;
        }
    else if (fpsTarget>fpsTotal)
        {
            fprintf(stderr,"We are running @ %0.2f which is slower than the user target which is %0.2f\n",fpsTotal,fpsTarget);

            float skipPerSecond=fpsTarget-fpsTotal;
            unsigned int skipMicroseconds=(unsigned int) skipPerSecond*1000;
            unsigned int framesSkipped=0;

            unsigned long frameSkipStart = GetTickCountMicroseconds();
            unsigned long frameSkipEnd = frameSkipStart;

            //Do skip frames until it's time for processing
            while (frameSkipEnd-frameSkipStart < skipMicroseconds)
                {
                    frameSkipEnd = GetTickCountMicroseconds();
                    cap >> frame; // get a new frame from camera
                    ++framesSkipped;
                }
            fprintf(stderr,"We skipped %u frames to reach target .. \n",framesSkipped);
            return 3;
        }

    return 1;
}



int main(int argc, char *argv[])
{
    fprintf(stderr,"Welcome to the MocapNET demo\n");

    unsigned int mocapNETMode=3;
    unsigned int forceCPUMocapNET=1;
    unsigned int forceCPU2DJointEstimation=0;
    float quality=1.0;
    float scale=1.0;

    unsigned int frameNumber=0,skippedFrames=0,frameLimit=5000,frameLimitSet=0,visualize=1,save2DVisualization=0,visualizeOpenGLEnabled=0;
    int joint2DSensitivityPercent=35;
    const char * webcam = 0;

    unsigned int deadInputPoints=0;
    int live=0,stop=0,visualizationType=0;
    int constrainPositionRotation=1,rotate=0;
    int doCrop=1,tryForMaximumCrop=0,doSmoothing=5,drawFloor=1,drawNSDM=1,doGestureDetection=1,doOutputFiltering=1,frameSkip=0;
    int targetSpecificFramerate=0;
    float fpsTotal=1.0,fpsTarget=30.0,fpsAcquisition=1.0,fps2DJointDetector=1.0,fpsMocapNET=1.0;

    int distance = 0,rollValue = 0,pitchValue = 0, yawValue = 0,autoDirection=0,autoCount=0;
    int doFeetHeuristics=0;
    int rememberPrevious2DPositions=0;

    int borderTop=0, borderBottom=0, borderLeft=0, borderRight=0;
    
    int coveringRectangle=0;
    int coveringRectangleX=0, coveringRectangleY=0, coveringRectangleWidth=0, coveringRectangleHeight=0;

    unsigned int delay=1; //Just a little time to view the window..

    unsigned int quitAfterNBadFrames = 10000;
    //2D Joint Detector Configuration
    unsigned int inputWidth2DJointDetector = 368;
    unsigned int inputHeight2DJointDetector = 368;
    unsigned int heatmapWidth2DJointDetector = 46;
    unsigned int heatmapHeight2DJointDetector = 46;
    unsigned int numberOfHeatmaps = 19;
    const char   outputPathStatic[]="out.bvh";
    char * outputPath = (char*) outputPathStatic;
    const char   networkPathOpenPoseMiniStatic[]="combinedModel/openpose_model.pb";
    const char   networkPathVnectStatic[]="combinedModel/vnect_sm_pafs_8.1k.pb";
    const char   networkPathFORTHStatic[]="combinedModel/mobnet2_tiny_vnect_sm_1.9k.pb";

    char   networkInputLayer[]="input_1";
    char   networkOutputLayer[]="k2tfout_0";
    unsigned int numberOfOutputTensors = 3;
    char * networkPath = (char*) networkPathFORTHStatic;
    //-------------------------------

    //---------------------------------------------------------
    // Researcher's night figures..
    //---------------------------------------------------------
    cv::Mat demoFigures[3];
    populateDemoFigures(demoFigures);
    //---------------------------------------------------------

    char CPUName[512]= {0};
    if (getCPUName(CPUName,512))
        {
            fprintf(stderr,"CPU : %s\n",CPUName);
        }


    char GPUName[512]= {0};
    if (getGPUName(GPUName,512))
        {
            fprintf(stderr,"GPU : %s\n",GPUName);
        }


    for (int i=0; i<argc; i++)
        {
            //In order to have an acceptable performance you should run 2D Joint estimation on GPU and MocapNET on CPU (which is the default configuration)
            //If you want to force everything on GPU use --gpu
            //If you want to force everything on CPU use --cpu

            //Switch between different 2D detectors --------------------------------------------------------
            if (strcmp(argv[i],"--openpose")==0)
                {
                    networkPath=(char*) networkPathOpenPoseMiniStatic;
                    networkOutputLayer[8]='1';
                    joint2DSensitivityPercent=40;
                    numberOfOutputTensors = 4;
                }
            else if (strcmp(argv[i],"--forth")==0)
                {
                    networkPath=(char*) networkPathFORTHStatic;
                    networkOutputLayer[8]='0';
                    joint2DSensitivityPercent=30;
                    numberOfOutputTensors = 3;
                }
            else if (strcmp(argv[i],"--vnect")==0)
                {
                    networkPath = (char*) networkPathVnectStatic;
                    networkOutputLayer[8]='1';
                    joint2DSensitivityPercent=20;
                    numberOfOutputTensors = 4;
                }
            else
                // Various other switches -------------------------------------------------------------------
                if (strcmp(argv[i],"--dir")==0)
                    {
                        chdir(argv[i+1]);
                    }
                else if (strcmp(argv[i],"--quality")==0)
                    {
                        quality=atof(argv[i+1]);
                    }

                else if (strcmp(argv[i],"--maxbadframes")==0)
                    {
                        quitAfterNBadFrames=atoi(argv[i+1]);
                    }
                else if  ( (strcmp(argv[i],"--save2D")==0) || (strcmp(argv[i],"--save2d")==0) )
                    {
                        save2DVisualization=1;
                    }
                else if (strcmp(argv[i],"--opengl")==0)
                    {
                        visualizeOpenGLEnabled=1;
                    }
                else if (strcmp(argv[i],"--targetframerate")==0)
                    {
                        targetSpecificFramerate=1;
                        fpsTarget=atoi(argv[i+1]);
                    }
                else if (strcmp(argv[i],"--frameskip")==0)
                    {
                        frameSkip=atoi(argv[i+1]);
                    }
                else if (strcmp(argv[i],"--nofilter")==0)
                    {
                        doOutputFiltering=0;
                    }
                else if (strcmp(argv[i],"--novisualization")==0)
                    {
                        visualize=0;
                    }
                else if (strcmp(argv[i],"--horizontalpadding")==0)
                    {
                        borderLeft=atoi(argv[i+1])/2;
                        borderRight=atoi(argv[i+1])/2;
                    }
                else if (strcmp(argv[i],"--verticalpadding")==0)
                    {
                        borderTop=atoi(argv[i+1])/2;
                        borderBottom=atoi(argv[i+1])/2;
                    } 
                else if (strcmp(argv[i],"--rectangle")==0)
                    {
                        coveringRectangle=1; 
                        coveringRectangleX=atoi(argv[i+1]);
                        coveringRectangleY=atoi(argv[i+2]);
                        coveringRectangleWidth=atoi(argv[i+3]); 
                        coveringRectangleHeight=atoi(argv[i+4]);
                    }
                else if (strcmp(argv[i],"--2dmodel")==0)
                    {
                        networkPath=argv[i+1];
                    }
                else if  (  (strcmp(argv[i],"-o")==0) || (strcmp(argv[i],"--output")==0) )
                    {
                        outputPath=argv[i+1];
                    }
                else if (strcmp(argv[i],"--delay")==0)
                    {
                        //If you want to take some time to check the results that
                        //might otherwise pass by very fast
                        delay=atoi(argv[i+1]);
                    }
                else if (strcmp(argv[i],"--frames")==0)
                    {
                        frameLimit=atoi(argv[i+1]);
                        frameLimitSet=1;
                    }
                else if (strcmp(argv[i],"--scale")==0)
                    {
                        scale=atof(argv[i+1]);
                    }
                else if (strcmp(argv[i],"--show")==0)
                    {
                        visualizationType=atof(argv[i+1]);
                    }
                else if (strcmp(argv[i],"--remember")==0)
                    {
                        rememberPrevious2DPositions=1;
                    }
                else if (strcmp(argv[i],"--rotate")==0)
                    {
                        rotate=atoi(argv[i+1]);
                    }
                else if (strcmp(argv[i],"--cpu")==0)
                    {
                        //setenv("CUDA_VISIBLE_DEVICES", "", 1);   //Alternate way to force CPU everywhere
                        forceCPUMocapNET=1;
                        forceCPU2DJointEstimation=1;
                    }
                else if (strcmp(argv[i],"--gpu")==0)
                    {
                        forceCPUMocapNET=0;
                        forceCPU2DJointEstimation=0;
                    }
                else if (strcmp(argv[i],"--unconstrained")==0)
                    {
                        constrainPositionRotation=0;
                    }
                else if (strcmp(argv[i],"--nocrop")==0)
                    {
                        doCrop=0;
                    }
                else if (strcmp(argv[i],"--mode")==0)
                    {
                        mocapNETMode=atoi(argv[i+1]);
                    }
                else if (strcmp(argv[i],"--live")==0)
                    {
                        live=1;
                        frameLimit=0;
                    }
                else if (strcmp(argv[i],"--from")==0)
                    {
                        if (argc>i+1)
                            {
                                webcam = argv[i+1];
                            }
                    }
        }


    if (initializeBVHConverter())
        {
            fprintf(stderr,"BVH allocation happened we are going to have BVH visualization \n");
        }

    fprintf(stderr,"Attempting to open input device\n");
    cv::Mat controlMat = Mat(Size(inputWidth2DJointDetector,2),CV_8UC3, Scalar(0,0,0));


    //sudo echo 64 > /sys/module/usbcore/parameters/usbfs_memory_mb
    cv::VideoCapture cap(webcam); // open the default camera
    if (webcam==0)
        {
            fprintf(stderr,"Trying to open webcam\n");
            cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
            cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
        }
    else
        {
            fprintf(stderr,"Trying to open %s\n",webcam);
        }

    if (!cap.isOpened())  // check if succeeded to connect to the camera
        {
            fprintf(stderr,"Openning input stream `%s` failed\n",webcam);
            return 1;
        }

    signed int totalNumberOfFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    fprintf(stderr,"totalNumberOfFrames by quering capture device %s is %u \n",webcam,totalNumberOfFrames);
    if (frameLimitSet)
        {
            fprintf(stderr,"User has set his own frame limit so we override total number of frames with %u \n",frameLimit);
            totalNumberOfFrames = frameLimit;
        }
    else if ( (totalNumberOfFrames>0) && (!frameLimitSet) )
        {
            frameLimit=totalNumberOfFrames;
        }

    cv::Mat frame;
    cv::Mat openGLFramePermanentMat;
    struct boundingBox cropBBox= {0};
    unsigned int croppedDimensionWidth=0,croppedDimensionHeight=0,offsetX=0,offsetY=0;

    struct TensorflowInstance net= {0};
    struct MocapNET mnet= {0};



    std::vector<float> flatAndNormalized2DPoints;
    std::vector<float> previousFlatAndNormalized2DPoints;
    std::vector<std::vector<float> > bvhFrames;
    std::vector<float> bvhOutput;
    std::vector<float> previousBvhOutput;
    std::vector<std::vector<float> > points2DOutput;
    std::vector<std::vector<float> > points2DOutputGUIForcedView;

    if ( loadMocapNET(&mnet,"test",quality,mocapNETMode,forceCPUMocapNET) )
        {
            if (
                loadTensorflowInstance(
                    &net,
                    networkPath,
                    networkInputLayer,
                    networkOutputLayer,
                    forceCPU2DJointEstimation
                )

            )
                {
                    frameNumber=0;
                    while ( ( (live) || (frameNumber<frameLimit) ) &&  (!stop) )
                        {
                            // Get Image
                            unsigned long acquisitionStart = GetTickCountMicroseconds();

                            //---------------------------------------------------------------------------------------------------
                            //---------------------------------------------------------------------------------------------------
                            cap >> frame; // get a new frame from camera
                            //---------------------------------------------------------------------------------------------------
                            //---------------------------------------------------------------------------------------------------


                            //A typical problem with low-end computers is that they might
                            //struggle to run the 2D joint estimator. We can  workaround this
                            //for the live demo to reduce frame latency and improve the overall experience
                            //Especially when performing gesture recognition we need to
                            //know at what speed we are processing framerates to match
                            //the execution of the gesture.
                            //There are two flavors of frame skipping, a constant frame skip ( --frameskip X  )
                            //An adaptable target framerate ( --targetframerate X ) that will try to speed up/slow down
                            //Depending on your current situation  
                            //Please note that there is no frame skipping involved in any of the
                            //reported framerates of the paper.
                            //---------------------------------------------------------------------------------------------------
                            if (frameSkip)
                                {
                                    for (int i=0; i<frameSkip; i++)
                                        {
                                            cap >> frame;
                                        }
                                }
                            else if (targetSpecificFramerate)
                                {
                                    adjustInputSamplingToMatchFramerateTarget(
                                        cap,
                                        frame,
                                        frameNumber,
                                        fpsTarget,
                                        fpsTotal,
                                        fpsAcquisition,
                                        fps2DJointDetector,
                                        fpsMocapNET
                                    );
                                }
                                //-------------------------------------------------------------------------------------------------------
                                
                                
                            //Some datasets have persons that appear in parts of the image, we might want to cover them using a rectangle
                            //We do this before adding any borders or otherwise change of the ROI of the image, however we do this 
                            //after possible frame skips for the obviously increased performance..
                            if (coveringRectangle)
                            {
                                cv::Point pt1(coveringRectangleX,coveringRectangleY);
                                cv::Point pt2(coveringRectangleX+coveringRectangleWidth,coveringRectangleY+coveringRectangleHeight);
                                cv::rectangle(frame,pt1,pt2,cv::Scalar(0,0,0),-1,8,0); 
                            }    
                                
                            //If we want to add a border to our frame to pad it this is done here
                            //----------------------------------------------------------------------------------------------------------
                            if ( ( borderTop!=0 ) || ( borderBottom!=0 ) || ( borderLeft!=0 ) || ( borderRight==0 ) )
                                {
                                    Scalar colorValue(0,0,0);
                                    copyMakeBorder(frame,frame,borderTop,borderBottom,borderLeft,borderRight,BORDER_CONSTANT,colorValue);
                                }

                            //If we want to  resize our frame this is done here
                            //----------------------------------------------------------------------------------------------------------
                            if (scale!=1.0)
                                {
                                    cv::resize(frame, frame, cv::Size(0,0), scale, scale);
                                }

                            //If we want to  rotate our frame this is done here
                            //----------------------------------------------------------------------------------------------------------
                            if (rotate)
                                { 
                                    switch(rotate)
                                        {
                                        case 1 :
                                            cv::rotate(frame, frame, cv::ROTATE_90_CLOCKWISE);
                                            break;
                                        case 2 :
                                            cv::rotate(frame, frame, cv::ROTATE_180);
                                            break;
                                        case 3 :
                                            cv::rotate(frame, frame, cv::ROTATE_90_COUNTERCLOCKWISE);
                                            break;
                                        } ;
                                }
                                
                             //Our frame at this point is considered captured and almost ready for use..!
                             //-------------------------------------------------------------------------------------------------------------
                                
                                
                             //We keep a copy of the cv::Mat since we will add a ROI on it
                             //We could perform a deep copy but this is too wasteful
                            cv::Mat frameOriginal = frame; //ECONOMY .clone();  
                            unsigned int frameWidth  =  frame.size().width;  //frame.cols
                            unsigned int frameHeight =  frame.size().height; //frame.rows


                            //-------------------------------------------------------------
                            // Decide on Visualization Window Size
                            //-------------------------------------------------------------
                            unsigned int visWidth=frameWidth;
                            unsigned int visHeight=frameHeight;
                            //If our input window is small enlarge it a little..
                            if (visWidth<700)
                                {
                                    visWidth=(unsigned int) frameWidth*2.0;
                                    visHeight=(unsigned int) frameHeight*2.0;
                                }
                            visWidth=1024;
                            visHeight=768; 
                            //-------------------------------------------------------------

                            
                            //We need to guard against broken input  of empty frames..
                            if ( (frameWidth!=0)  && (frameHeight!=0)  )
                                {
                                    unsigned long acquisitionEnd = GetTickCountMicroseconds(); 
                                    fpsAcquisition = convertStartEndTimeFromMicrosecondsToFPS(acquisitionStart,acquisitionEnd);

                                    //------------------------------------------------------------------------
                                    // If cropping is enabled
                                    if (doCrop)
                                        {
                                            //And there was some previous BVH output
                                            if (bvhOutput.size()>0)
                                                {
                                                    // Try to crop around the last closest
                                                    //------------------------------------------------------------------------
                                                    if (
                                                        getBestCropWindow(
                                                            tryForMaximumCrop,
                                                            &offsetX,
                                                            &offsetY,
                                                            &croppedDimensionWidth,
                                                            &croppedDimensionHeight,
                                                            &cropBBox,
                                                            inputWidth2DJointDetector,
                                                            inputHeight2DJointDetector,
                                                            frameWidth,
                                                            frameHeight
                                                        )
                                                    )
                                                        {
                                                            if (croppedDimensionWidth!=croppedDimensionHeight)
                                                                {
                                                                    fprintf(stderr,"Bounding box produced was not a rectangle (%ux%u)..!\n",croppedDimensionWidth,croppedDimensionHeight);
                                                                }
                                                            cv::Rect rectangleROI(offsetX,offsetY,croppedDimensionWidth,croppedDimensionHeight);
                                                            frame = frame(rectangleROI);
                                                            cropBBox.populated=0;
                                                        }
                                                }
                                            else
                                                {
                                                    fprintf(stderr,"Haven't detected a person, so seeking a skeleton in the full image, regardless of distortion..\n");
                                                }
                                        }
                                    //------------------------------------------------------------------------



                                    //We might have changed frame dimensions so let's check them again..
                                    //-------------------------------------------------------------------------------------------------------------------------
                                    if ( (frameWidth>0) && (frameHeight>0) )
                                        {
                                            std::vector<std::vector<float> >  points2DInput;

                                            // Get 2D Skeleton Input from Frame
                                            fps2DJointDetector = 0;
                                            flatAndNormalized2DPoints = returnMocapNETInputFrom2DDetectorOutput(
                                                                            &net,
                                                                            frame,
                                                                            &cropBBox,
                                                                            points2DInput,
                                                                            (float) joint2DSensitivityPercent/100,
                                                                            visualize,
                                                                            save2DVisualization,
                                                                            &fps2DJointDetector,
                                                                            frameNumber,
                                                                            offsetX,
                                                                            offsetY,
                                                                            frameWidth-croppedDimensionWidth,
                                                                            frameHeight-croppedDimensionHeight,
                                                                            inputWidth2DJointDetector,
                                                                            inputHeight2DJointDetector,
                                                                            heatmapWidth2DJointDetector,
                                                                            heatmapHeight2DJointDetector,
                                                                            numberOfHeatmaps,
                                                                            numberOfOutputTensors,
                                                                            doFeetHeuristics
                                                                        );


                                            if (rememberPrevious2DPositions)
                                                {
                                                    flatAndNormalized2DPoints = fillInTheBlanks(previousFlatAndNormalized2DPoints,flatAndNormalized2DPoints);
                                                    previousFlatAndNormalized2DPoints = flatAndNormalized2DPoints;
                                                }


                                            previousBvhOutput=bvhOutput;
                                            // Get MocapNET prediction
                                            unsigned long startTime = GetTickCountMicroseconds();
                                            bvhOutput = runMocapNET(&mnet,flatAndNormalized2DPoints,doGestureDetection,doOutputFiltering);
                                            unsigned long endTime = GetTickCountMicroseconds();
                                            fprintf(stderr,"Just run MocapNET and got back a %lu sized vector \n",bvhOutput.size());


                                            if (doOutputFiltering)
                                                {
                                                    deadInputPoints = getNumberOfEmptyNSDMElements(flatAndNormalized2DPoints);
                                                    if (deadInputPoints>102)
                                                        {
                                                            fprintf(stderr,RED "Too many dead input elements %u\n" NORMAL,deadInputPoints);
                                                            bvhOutput=previousBvhOutput;
                                                        }
                                                }

                                            //-------------------------------------------------------------------------------------------------------------------------
                                            // Adding a vary baseline smoothing after a project request, it should be noted
                                            // that no results during the original BMVC2019 used any smoothing of any kind
                                            //-------------------------------------------------------------------------------------------------------------------------
                                            if(doSmoothing)
                                                {
                                                    //-------------------------------------------------------------------------------------------------------------------------
                                                    if ( (previousBvhOutput.size()>0) && (bvhOutput.size()>0) )
                                                        {
                                                            smoothVector(previousBvhOutput,bvhOutput,(float) doSmoothing/10);
                                                        }
                                                } //-------------------------------------------------------------------------------------------------------------------------


                                            fpsMocapNET = convertStartEndTimeFromMicrosecondsToFPS(startTime,endTime);


                                            std::vector<float> bvhForcedViewOutput=bvhOutput;

                                            if (!visualize)
                                                {
                                                    //If we don't visualize using OpenCV output performance
                                                    fprintf(stderr,"MocapNET 3DSkeleton @ %0.2f fps \n",fpsMocapNET);
                                                }

                                            if (constrainPositionRotation==2)
                                                {
                                                    //Specific position rotation constraint that scans left and right..
                                                    ++autoCount;
                                                    if (autoCount>=1)
                                                        {
                                                            autoCount=0;

                                                            if ( (autoDirection==1) && (yawValue==0) )
                                                                {
                                                                    autoDirection=0;
                                                                }
                                                            else if ( (autoDirection==0) && (yawValue==360) )
                                                                {
                                                                    autoDirection=1;
                                                                }

                                                            if (autoDirection==0)
                                                                {
                                                                    yawValue+=1;
                                                                }
                                                            else if (autoDirection==1)
                                                                {
                                                                    yawValue-=1;
                                                                }
                                                        }
                                                }//---------------------------------------------------------------------------------------------------


                                            //Force Skeleton Position and orientation to make it more easily visualizable
                                            if (constrainPositionRotation>0)
                                                {
                                                    if (bvhForcedViewOutput.size()>0)
                                                        {
                                                            bvhForcedViewOutput[MOCAPNET_OUTPUT_HIP_XPOSITION]=0.0;
                                                            bvhForcedViewOutput[MOCAPNET_OUTPUT_HIP_YPOSITION]=0.0;
                                                            bvhForcedViewOutput[MOCAPNET_OUTPUT_HIP_ZPOSITION]=-190.0 - (float) distance;
                                                            bvhForcedViewOutput[MOCAPNET_OUTPUT_HIP_ZROTATION]=(float) rollValue;
                                                            bvhForcedViewOutput[MOCAPNET_OUTPUT_HIP_YROTATION]=(float) yawValue;
                                                            bvhForcedViewOutput[MOCAPNET_OUTPUT_HIP_XROTATION]=(float) pitchValue;
                                                        }
                                                }


                                            //If we are not running live ( aka not from a webcam with no fixed frame limit )
                                            //Then we record the current bvh frame in order to save a .bvh file in the end..
                                            if (!live)
                                                {
                                                    bvhFrames.push_back(bvhForcedViewOutput); //bvhOutput
                                                }



                                            if (bvhForcedViewOutput.size()>0)
                                                {
                                                    points2DOutputGUIForcedView = convertBVHFrameTo2DPoints(
                                                                                                             bvhForcedViewOutput,
                                                                                                             visWidth,
                                                                                                             visHeight
                                                                                                           );
                                                }

                                            if (bvhOutput.size()>0)
                                                {
                                                    points2DOutput = convertBVHFrameTo2DPoints(
                                                                                               bvhOutput,
                                                                                               1920,//visWidth,
                                                                                               1080//visHeight
                                                                                              );
                                                }



                                            //Display two sample joint configurations to console output
                                            //to demonstrate how easy it is to get the output joint information
                                            if (bvhOutput.size()>0)
                                                {
                                                    fprintf(stderr,"Right Shoulder Rotations Z X Y = %0.2f,%0.2f,%0.2f\n",
                                                            bvhOutput[MOCAPNET_OUTPUT_RSHOULDER_ZROTATION],
                                                            bvhOutput[MOCAPNET_OUTPUT_RSHOULDER_XROTATION],
                                                            bvhOutput[MOCAPNET_OUTPUT_RSHOULDER_YROTATION]
                                                           );

                                                    fprintf(stderr,"Left Shoulder Rotations Z X Y = %0.2f,%0.2f,%0.2f\n",
                                                            bvhOutput[MOCAPNET_OUTPUT_LSHOULDER_ZROTATION],
                                                            bvhOutput[MOCAPNET_OUTPUT_LSHOULDER_XROTATION],
                                                            bvhOutput[MOCAPNET_OUTPUT_LSHOULDER_YROTATION]
                                                           );
                                                }


                                            unsigned long totalTimeEnd = GetTickCountMicroseconds();
                                            fpsTotal = convertStartEndTimeFromMicrosecondsToFPS(acquisitionStart,totalTimeEnd);


                                            //OpenCV Visualization stuff
                                            //---------------------------------------------------
                                            if (visualize)
                                                {
                                                    if (frameNumber==0)
                                                        {
                                                            cv::imshow("3D Control",controlMat);

                                                            cv::createTrackbar("Stop Demo", "3D Control", &stop, 1);
                                                            cv::createTrackbar("Visualization Demo", "3D Control", &visualizationType,14);
                                                            cv::createTrackbar("Rotate Feed", "3D Control", &rotate, 4);
                                                            cv::createTrackbar("Gesture Detection", "3D Control", &doGestureDetection,1);
                                                            cv::createTrackbar("Filter Output", "3D Control", &doOutputFiltering,1);
                                                            cv::createTrackbar("Constrain Position/Rotation", "3D Control", &constrainPositionRotation, 2);
                                                            cv::createTrackbar("Automatic Crop", "3D Control", &doCrop, 1);
                                                            cv::createTrackbar("2D NN Sensitivity", "3D Control", &joint2DSensitivityPercent,100);
                                                            cv::createTrackbar("Remember Previous 2D", "3D Control", &rememberPrevious2DPositions,1);
                                                            cv::createTrackbar("Feet Heuristics", "3D Control", &doFeetHeuristics,1);
                                                            cv::createTrackbar("Smooth 3D Output", "3D Control", &doSmoothing, 10);
                                                            cv::createTrackbar("Maximize Crop", "3D Control", &tryForMaximumCrop, 1);
                                                            cv::createTrackbar("Draw Floor", "3D Control", &drawFloor, 1);
                                                            cv::createTrackbar("Draw NSDM", "3D Control", &drawNSDM, 1);
                                                            cv::createTrackbar("Distance  ", "3D Control", &distance,  150);
                                                            cv::createTrackbar("Yaw            ", "3D Control", &yawValue,  360);
                                                            cv::createTrackbar("Pitch          ", "3D Control", &pitchValue,360);
                                                            cv::createTrackbar("Roll            ", "3D Control", &rollValue, 360);


                                                            cv::namedWindow("3D Points Output");
                                                            cv::moveWindow("3D Control",inputWidth2DJointDetector,0); //y=inputHeight2DJointDetector
                                                            cv::moveWindow("2D NN Heatmaps",0,0);
                                                            //cv::moveWindow("BGR",inputWidth2DJointDetector,0);
                                                            //cv::moveWindow("2D Detections",inputWidth2DJointDetector,0);
                                                        }


                                                    //Get rid of GLib-GObject-CRITICAL **: 10:36:18.934: g_object_unref: assertion 'G_IS_OBJECT (object)' failed opencv
                                                    //by displaying an empty cv Mat on the window besides the trackbars
                                                    cv::imshow("3D Control",controlMat);





                                                    cv::Mat * openGLMatForVisualization = 0;
                                                    if (visualizeOpenGLEnabled)
                                                        {
                                                            //fprintf(stderr,"updateOpenGLView\n");
                                                            updateOpenGLView(bvhOutput);
                                                            
                                                            //fprintf(stderr,"visualizeOpenGL\n");
                                                            unsigned int openGLFrameWidth=visWidth,openGLFrameHeight=visHeight;
                                                            char * openGLFrame = visualizeOpenGL(&openGLFrameWidth,&openGLFrameHeight);
                                                            //=====================================================================
                                                            if (openGLFrame!=0)
                                                                {
                                                                    fprintf(stderr,"Got Back a frame..!\n");
                                                                    cv::Mat openGLMat(openGLFrameHeight, openGLFrameWidth, CV_8UC3);
                                                                    unsigned char *   initialPointer = openGLMat.data;
                                                                    openGLMat.data=(unsigned char * ) openGLFrame;

                                                                    cv::cvtColor(openGLMat,openGLFramePermanentMat,COLOR_RGB2BGR);
                                                                    //cv::imshow("OpenGL",openGLFramePermanentMat);
                                                                    openGLMatForVisualization = &openGLFramePermanentMat;
                                                                    openGLMat.data=initialPointer;
                                                                }//=====================================================================
                                                        }

                                                    //Retreive gesture name to display it
                                                    char * gestureName=0;
                                                    if (doGestureDetection)
                                                        {
                                                            if (mnet.lastActivatedGesture>0)
                                                                {
                                                                    gestureName=mnet.recognizedGestures.gesture[mnet.lastActivatedGesture-1].label;
                                                                }
                                                        }



                                                    if ( (visualizationType==0) || (visualizationType==1) )
                                                        {
                                                            cv::Mat * openGLMatForVisualizationSelected = openGLMatForVisualization;
                                                            
                                                            if (visualizationType==1)
                                                            {
                                                                openGLMatForVisualizationSelected=0;
                                                            }
                                                                
                                                            
                                                            visualizePoints(
                                                                            "3D Points Output",
                                                                            frameNumber,
                                                                            skippedFrames,
                                                                            totalNumberOfFrames,
                                                                            frameLimit,
                                                                            CPUName,
                                                                            GPUName,
                                                                            drawFloor,
                                                                            drawNSDM,
                                                                            fpsTotal,
                                                                            fpsAcquisition,
                                                                            fps2DJointDetector,
                                                                            fpsMocapNET,
                                                                            mocapNETMode,
                                                                            visWidth,
                                                                            visHeight,
                                                                            0,

                                                                            deadInputPoints,

                                                                            //mnet->recognizedGestures.
                                                                            mnet.lastActivatedGesture,
                                                                            gestureName,
                                                                            mnet.recognizedGestures.gestureChecksPerformed - mnet.gestureTimestamp ,//gesture stuff

                                                                            flatAndNormalized2DPoints,
                                                                            bvhOutput,
                                                                            bvhForcedViewOutput,
                                                                            points2DInput,
                                                                            points2DOutput,
                                                                            points2DOutputGUIForcedView,
                                                                            (void*) openGLMatForVisualizationSelected//openGLMatForVisualization
                                                                           );
                                                        }
                                                    else if (visualizationType==2)
                                                        {
                                                            visualizeMotionHistory("3D Points Output",mnet.poseHistoryStorage.history,points2DOutputGUIForcedView);
                                                        }
                                                    else  if (visualizationType==3)
                                                        {
                                                            visualizeFigure("3D Points Output",demoFigures[0]);
                                                        }
                                                    else if (visualizationType==4)
                                                        {
                                                            visualizeFigure("3D Points Output",demoFigures[1]);
                                                        }
                                                    else if (visualizationType==5)
                                                        {
                                                            visualizeCameraFeatures("3D Points Output",frameOriginal);
                                                        }
                                                    else if (visualizationType==6)
                                                        {
                                                            visualizeCameraEdges("3D Points Output",frame);
                                                        }
                                                    else if (visualizationType<=13)
                                                        {
                                                            visualizeCameraChannels("3D Points Output",frame,visualizationType-6);
                                                        }
                                                    else if (visualizationType==14)
                                                        {
                                                            visualizeCameraIntensities("3D Points Output",frame,0);
                                                        }
                                                    else if (visualizationType==15)
                                                        {
                                                            visualizeCameraIntensities("3D Points Output",frame,1);
                                                        }


                                                    if (frameNumber==0)
                                                        {
                                                            cv::resizeWindow("3D Control",inputWidth2DJointDetector,inputHeight2DJointDetector);
                                                            cv::moveWindow("3D Points Output",inputWidth2DJointDetector*2,0);
                                                            cv::moveWindow("2D Detections",0,inputHeight2DJointDetector);
                                                        }


                                                    //Window Event Loop Time and  Receiving Key Presses..
                                                    //----------------------------------------------------------------------------------------------------------
                                                    int key = cv::waitKey(delay) ;
                                                    key = 0x000000FF & key;

                                                    if (key!=255)
                                                        {
                                                            fprintf(stderr,"Keypress = %u \n",key);
                                                            if  (
                                                                (key == 113) //|| (key == 81)
                                                            )
                                                                {
                                                                    fprintf(stderr,"Stopping MocapNET after keypress..\n");
                                                                    stop=1;
                                                                } // stop capturing by pressing q
                                                            else if (key == 115) //S button
                                                                {
                                                                    fprintf(stderr,"Saving gesture after keypress..\n");
                                                                    dumpMotionHistory("gesture.bvh",&mnet.poseHistoryStorage);
                                                                }
                                                        }
                                                    //----------------------------------------------------------------------------------------------------------

                                                }
                                            //---------------------------------------------------
                                        }



                                    ++frameNumber;
                                }
                            else
                                {
                                    if (totalNumberOfFrames>0)
                                        {
                                            if (skippedFrames+frameNumber>=totalNumberOfFrames)
                                                {
                                                    fprintf(stderr,GREEN "Stream appears to have ended..\n" NORMAL);
                                                    break;
                                                }
                                            else
                                                {
                                                    ++skippedFrames;
                                                }
                                        }
                                    else
                                        {
                                            ++skippedFrames;
                                        }

                                    fprintf(stderr,YELLOW "OpenCV failed to snap frame %u from your input source (%s)\n" NORMAL,frameNumber,webcam);
                                    fprintf(stderr,NORMAL "Skipped frames %u/%u\n" NORMAL,skippedFrames,frameNumber);

                                    if (skippedFrames>quitAfterNBadFrames)
                                        {
                                            fprintf(stderr,RED "We have encountered %u bad input frames so quitting ..\n" NORMAL,quitAfterNBadFrames);
                                            fprintf(stderr,RED "If you don't want this to happen consider using the flag --maxskippedframes and providing a bigger value ..\n" NORMAL);
                                            break;
                                        }
                                }

                        } //Master While Frames Exist loop

                    //After beeing done with the frames gathered the bvhFrames vector should be full of our data, so maybe we want to write it to a file..!
                    if (!live)
                        {
                            fprintf(stderr,"Will now write BVH file to %s.. \n",outputPath);
                            //just use BVH header
                            if ( writeBVHFile(outputPath,0,bvhFrames) )
                                {
                                    fprintf(stderr,GREEN "Successfully wrote %lu frames to bvh file.. \n" NORMAL,bvhFrames.size());
                                }
                            else
                                {
                                    fprintf(stderr,RED "Failed to write %lu frames to bvh file.. \n" NORMAL,bvhFrames.size());
                                }
                            if (skippedFrames>0)
                                {
                                    fprintf(stderr,"Please note that while getting input %u frames where skipped due to OpenCV related errors\n",skippedFrames);
                                }
                        }
                    else
                        {
                            fprintf(stderr,"Will not record a bvh file since live sessions can be arbitrarily long..\n");
                        }
                }
            else
                {
                    fprintf(stderr,"Was not able to load Tensorflow model for 2D joint detection..\n");
                }
        }
    else
        {
            fprintf(stderr,"Was not able to load MocapNET, please make sure you have the appropriate models downloaded..\n");
            fprintf(stderr,"TIP: Rerun initialize.sh to make sure that your model files follow repository changes..\n");
        }
    // the camera will be deinitialized automatically in VideoCapture destructor


    fprintf(stderr,NORMAL "MocapNET Live Demo finished.\n" NORMAL);
    return 0;
}
