
/** @file livedemo.cpp
 *  @brief This is the main "demo" offered in this repository, it will take a stream from a webcam or video file using OpenCV and run
*   2D pose estimation + MocapNET giving you a nice 3D visualization as well as an output .bvh file
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include <stdio.h>
#include <unistd.h>
//-----------------------------------------------------------------

#include "PCA/PCA.h"
#include "JSON/readModelConfiguration.h"
#include "JSON/readListFile.h"

#include "NSxM/NSxM.h"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */



void testPCA(struct PCAData * pca)
{
    float output[210]={0};
    int outputSize = 210;

    float input[458]={0};
    int inputSize = 458;
    for (int i=0; i<inputSize; i++)
    {
        input[i]=1.0;
    }

    doPCATransform(output,&outputSize,pca,input,inputSize,210);

    for (int i=0; i<outputSize; i++)
    {
        fprintf(stderr,"%u = %f \n",i,output[i]);
    }
}




int main(int argc, char *argv[])
{
    //struct MocapNET2Options options= {0};
    //struct MocapNET4 mnet={0};

    struct PCAData pcaUpperBody={0};
    loadPCADataFromJSON(&pcaUpperBody,"dataset/combinedModel/mocapnet4/mode1/1.0/step1_upperbody_all/upperbody_all.pca");
    // the camera will be deinitialized automatically in VideoCapture destructor

    struct ModelConfigurationData modelConfigurationUpperBody={0};
    loadModelConfigurationData(&modelConfigurationUpperBody,"dataset/combinedModel/mocapnet4/mode1/1.0/step1_upperbody_all/upperbody_configuration.json");
    printModelConfigurationData(&modelConfigurationUpperBody);

    printf("Now will load I/O lists..\n");

    struct listFileData neuralNetworkInputsUpperBody  = {0};
    struct listFileData neuralNetworkOutputsUpperBody = {0};
    struct listFileData neuralNetworkJointsUpperBody  = {0};
    //----------------------------------------------------------------------------------------------------------------------------
    readListFile(&neuralNetworkInputsUpperBody,"dataset/combinedModel/mocapnet4/mode1/1.0/step1_upperbody_all/neuralNetworkInputs.list");
    printListFile(&neuralNetworkInputsUpperBody,"neuralNetworkInputs");
    //----------------------------------------------------------------------------------------------------------------------------
    readListFile(&neuralNetworkOutputsUpperBody,"dataset/combinedModel/mocapnet4/mode1/1.0/step1_upperbody_all/neuralNetworkOutputs.list");
    printListFile(&neuralNetworkOutputsUpperBody,"neuralNetworkOutputs");
    //----------------------------------------------------------------------------------------------------------------------------
    readListFile(&neuralNetworkJointsUpperBody,"dataset/combinedModel/mocapnet4/mode1/1.0/step1_upperbody_all/neuralNetworkJoints.list");
    printListFile(&neuralNetworkJointsUpperBody,"neuralNetworkJoints");
    //----------------------------------------------------------------------------------------------------------------------------


    unsigned int data2DLength = 25 * 3; //BODY25 * (x,y,vis)
    float * data2D = (float*) malloc(sizeof(float) * data2DLength);
    //---------------------------------------------------------------
    for (int i=0; i<25; i++)
    {
        data2D[i*3 + 0 ] = 0.1 * i;
        data2D[i*3 + 1 ] = 0.1 * i;
        data2D[i*3 + 2 ] = 1.0;
    }

    struct inputRouting routeUpperBody={0};
    //---------------------------------------------------------------
    generateRouteFromLabels(
                             &modelConfigurationUpperBody,
                             &routeUpperBody,
                             Body25Labels,
                             25
                           );

    struct descriptor upperBodyNSRM={0};
    //---------------------------------------------------------------
    if (data2D!=0)
    {
     createDescriptor(
                      &upperBodyNSRM,
                      data2D,
                      data2DLength,
                      &routeUpperBody,
                      &modelConfigurationUpperBody,
                      &pcaUpperBody,
                      &neuralNetworkJointsUpperBody,
                      &neuralNetworkOutputsUpperBody
                     );

     fprintf(stderr,"\nGenerated UpperBody Descriptor : \n");
     for (int i=0; i<upperBodyNSRM.numberOfElements; i++)
     {
        fprintf(stderr,"%u = %f \n",i,upperBodyNSRM.values[i]);
     }
    }

    return 0;
}


