#pragma once
/** @file parseCommandlineOptions.hpp
 *  @brief MocapNET applications handle a large number of parameters. In order to simplify development and not having to manually sync all accepted parameters through all end-user applications this is the
 *  central module that parses commandline parameters and populates the MocapNET2Options structure that holds them
 *  @author Ammar Qammaz (AmmarkoV)
 */


/**
 * @brief MocapNET has many options depending on datasets etc, instead of storing them in each application
 * this is a central strcture to make it easier to parse them..
 */
struct MocapNET2Options
{
    const char * webcamSource;
    const char * path;
    char * datasetPath;
    unsigned int inputIsSingleImage;

    unsigned int isJSONFile;
    unsigned int isCSVFile;
    unsigned int jointEstimatorUsed;

    unsigned int doUpperBody,doLowerBody,doFace,doHands;
    unsigned int forceFront,forceLeft,forceRight,forceBack;
    unsigned int useInverseKinematics;
    unsigned int visualizationType;

    unsigned int skipNeuralNetworkIfItIsNotNeeded;
    unsigned int maximumNeuralNetworkSkipFrames;

    float inputFramerate;

    float learningRate;
    float spring;
    unsigned int iterations;
    unsigned int epochs;

    float addNormalizedPixelGaussianNoiseX,addNormalizedPixelGaussianNoiseY;

    char * outputPath;

    unsigned int visualize,useOpenGLVisualization,save3DVisualization,save2DVisualization,saveVisualization,saveCSV3DFile,constrainPositionRotation;

    char CPUName[512];
    char GPUName[512];
    char message[512];

    unsigned int delay;
    unsigned int prependTPose;
    unsigned int serialLength;
    const char * label;
    unsigned int bvhCenter;

    float quality;
    unsigned int mocapNETMode;
    int doGestureDetection;
    int doOutputFiltering;
    int doMultiThreadedIK;

    unsigned int useCPUOnlyForMocapNET;
    unsigned int useCPUOnlyFor2DEstimator;
    unsigned int brokenFrames;
    unsigned int numberOfMissingJoints;

    unsigned int visWidth,visHeight;
    unsigned int width,height;

    long loopStartTime,loopEndTime;
    float totalLoopFPS;
    float fpsAcquisition,fpsMocapNET,fps2DEstimator,fpsIK;
    unsigned int frameLimit;
    unsigned int frameSkip;

    float scale,scaleX,scaleY,fScaleX,fScaleY;


    float skeletonRotation;

    int  dontBend;
    char forceOutputPositionRotation;
    float outputPosRot[6];


    char mapFilePath[512];

    int hasInit;
};



void  defaultMocapNET2Options(struct MocapNET2Options * options);

int loadOptionsFromCommandlineOptions(struct MocapNET2Options * options,int argc, char *argv[]);

int  loadOptionsAfterBVHLoadFromCommandlineOptions(struct MocapNET2Options * options,int argc, char *argv[]);


int  takeCareOfScalingInputAndAddingNoiseAccordingToOptions(struct MocapNET2Options * options,struct skeletonSerialized * skeleton);
