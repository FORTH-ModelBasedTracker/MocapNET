#include "../Tensorflow/tf_utils.hpp"
#include "mocapnet.hpp"
#include "jsonCocoSkeleton.h"
#include <math.h>

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

int writeBVHFile(
    const char * filename,
    const char * header,
    std::vector<std::vector<float> > bvhFrames
)
{
    FILE * fp = fopen(filename,"w");
    if (fp!=0)
        {
            if (header==0)
                {
                    header=bvhHeader;
                }
            fprintf(fp,"%s",header);
            fprintf(fp,"\nMOTION\n");
            fprintf(fp,"Frames: %lu \n",bvhFrames.size());
            fprintf(fp,"Frame Time: 0.04\n");

            unsigned int i=0,j=0;
            for (i=0; i< bvhFrames.size(); i++)
                {
                    std::vector<float> frame = bvhFrames[i];
                    //fprintf(fp,"%lu joints",frame.size());

                    //fprintf(fp,"0.0 0.0 0.0 ");
                    for (j=0; j<frame.size(); j++)
                        {
                            fprintf(fp,"%0.4f ",frame[j]);
                        }
                    fprintf(fp,"\n");
                }
            fclose(fp);
            return 1;
        }
    return 0;
}


int listNodesMN(const char * label , TF_Graph* graph)
{
    size_t pos = 0;
    TF_Operation* oper;
    fprintf(stdout, "Nodes list : \n");
    while ((oper = TF_GraphNextOperation(graph, &pos)) != nullptr)
        {
            fprintf(stderr," %s - %s \n",label,TF_OperationName(oper));
            //std::cout << label<<" - "<<TF_OperationName(oper)<<" "<< std::endl;
        }
    return 1;
}

int loadMocapNET(struct MocapNET * mnet,const char * filename,unsigned int forceCPU)
{
    int result = (
                     ( loadTensorflowInstance(&mnet->allModel  ,"combinedModel/all.pb"  ,"input_all"  ,"result_all/concat",forceCPU) ) &&
                     ( loadTensorflowInstance(&mnet->frontModel,"combinedModel/front.pb","input_front","result_front/concat",forceCPU) ) &&
                     ( loadTensorflowInstance(&mnet->backModel ,"combinedModel/back.pb" ,"input_back" ,"result_back/concat",forceCPU) )
                 );

    if (result)
        {
            fprintf(stderr,"Caching networks after initialization to be ready for use..\n");
            std::vector<float> emptyValues;
            for (int i=0; i<749; i++)
                {
                    emptyValues.push_back(0.0);
                }
            predictTensorflow(&mnet->allModel,emptyValues);
            predictTensorflow(&mnet->frontModel,emptyValues);
            predictTensorflow(&mnet->backModel,emptyValues);
        }

    return result;
}


float get2DPointsDistance(float x1,float y1,float x2,float y2)
{
    return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


std::vector<float> compressMocapNETInput(std::vector<float> mocapnetInput,int addSyntheticPoints,int doScaleCompensation)
{ 
    if ( (MOCAPNET_UNCOMPRESSED_JOINT_PARTS * 3!=mocapnetInput.size())||(mocapnetInput.size()!=171) )
        {
            fprintf(stderr,RED "mocapNET: compressMocapNETInput : wrong input size , received %lu expected 171\n" NORMAL,mocapnetInput.size());

            return mocapnetInput;
        }
 

    //---------------------------------------------------
    float rShoulderToHipDistance = get2DPointsDistance
                                   (
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_HIP*3+0],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_HIP*3+1],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_RSHOULDER*3+0],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_RSHOULDER*3+1]
                                   );
    //---------------------------------------------------
    float lShoulderToHipDistance = get2DPointsDistance
                                   (
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_HIP*3+0],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_HIP*3+1],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_LSHOULDER*3+0],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_LSHOULDER*3+1]
                                   );

    //std::cerr<<"rShoulderToHipDistance "<<rShoulderToHipDistance<<"\n";
    //std::cerr<<"lShoulderToHipDistance "<<lShoulderToHipDistance<<"\n";


    //---------------------------------------------------
    float scaleDistance=1.0;
    if ( (rShoulderToHipDistance!=0) && (lShoulderToHipDistance!=0) )
        {
            scaleDistance=(rShoulderToHipDistance+lShoulderToHipDistance)/2;
        }
    else if (rShoulderToHipDistance!=0)
        {
            scaleDistance=rShoulderToHipDistance;
        }
    else if (lShoulderToHipDistance!=0)
        {
            scaleDistance=lShoulderToHipDistance;
        }


    //std::cerr<<"mocapnetCompressed\n";

    std::vector<float>  mocapnetCompressed;
    for (unsigned int iI=0; iI<MocapNETInputCompressedArrayIndexesSize; iI++)
        {
            unsigned int i=MocapNETInputCompressedArrayIndexes[iI];
            for (unsigned int jJ=0; jJ<MocapNETInputCompressedArrayIndexesSize; jJ++)
                {
                    unsigned int j=MocapNETInputCompressedArrayIndexes[jJ];

                    if (i>=MOCAPNET_UNCOMPRESSED_JOINT_PARTS)
                        {
                            fprintf(stderr,RED "\nERROR at i=%u for EDM element  [%u,%u]\n",i,iI,jJ);
                            fprintf(stderr,RED "%s\n",MocapNETInputUncompressedArrayNames[i*3+0]);
                            fprintf(stderr,RED "%s\n",MocapNETInputUncompressedArrayNames[i*3+1]);
                            fprintf(stderr,RED "%s\n",MocapNETInputUncompressedArrayNames[i*3+2]);
                            exit(0);
                        }
                    if (j>=MOCAPNET_UNCOMPRESSED_JOINT_PARTS)
                        {
                            fprintf(stderr,RED "\nERROR at j=%u for EDM element [%u,%u]\n",j,iI,jJ);
                            fprintf(stderr,RED "%s\n",MocapNETInputUncompressedArrayNames[j*3+0]);
                            fprintf(stderr,RED "%s\n",MocapNETInputUncompressedArrayNames[j*3+1]);
                            fprintf(stderr,RED "%s\n",MocapNETInputUncompressedArrayNames[j*3+2]);
                            exit(0);
                        }

                    float iX = mocapnetInput[i*3+0];
                    float iY = mocapnetInput[i*3+1];
                    float jX = mocapnetInput[j*3+0];
                    float jY = mocapnetInput[j*3+1];

                    if ( (iX>1.0) || (iY>1.0) || (jX>1.0) || (jY>1.0) )
                        {
                            //This should never happen
                            fprintf(stderr,RED "\nBigger than 1.0 element at [%u,%u]\n",iI,jJ);
                            mocapnetCompressed.push_back(666.0);
                            mocapnetCompressed.push_back(666.0);
                        }
                    else if ( (iX==0) || (iY==0) || (jX==0) || (jY==0) )
                        {
                            mocapnetCompressed.push_back(0.0);
                            mocapnetCompressed.push_back(0.0);
                        }
                    else
                        {
                            //#--------------------------
                            //#     Synthetic Points
                            //#--------------------------
                            if (addSyntheticPoints)
                                {
                                    if (i==7)
                                        {
                                            iX=iX-0.3;
                                        }
                                    else if (i==8)
                                        {
                                            iX=iX+0.3;
                                        }
                                    //#--------------------------
                                    if (j==7)
                                        {
                                            jX=jX-0.3;
                                        }
                                    else if (j==8)
                                        {
                                            jX=jX+0.3;
                                        }
                                }
                            //#--------------------------

                            float iXMinusjXPlus0_5=0.5+iX-jX;
                            float iYMinusjYPlus0_5=0.5+iY-jY;

                            if (iXMinusjXPlus0_5>10.0)
                                {
                                    fprintf(stderr,RED "\nERROR at (%0.2f,%0.2f)/(%0.2f,%0.2f)  for NSDM element [%u,%u]\n",iX,iY,jX,jY,iI,jJ);
                                    fprintf(stderr,RED "%s-%s\n",MocapNETInputUncompressedArrayNames[i*3+0],MocapNETInputUncompressedArrayNames[j*3+0]);
                                    fprintf(stderr,RED "%s-%s\n",MocapNETInputUncompressedArrayNames[i*3+1],MocapNETInputUncompressedArrayNames[j*3+1]);
                                }
                            if (iYMinusjYPlus0_5>10.0)
                                {
                                    fprintf(stderr,RED "\nERROR at (%0.2f,%0.2f)/(%0.2f,%0.2f)  for NSDM element [%u,%u]\n",iX,iY,jX,jY,iI,jJ);
                                    fprintf(stderr,RED "%s-%s\n",MocapNETInputUncompressedArrayNames[i*3+0],MocapNETInputUncompressedArrayNames[j*3+0]);
                                    fprintf(stderr,RED "%s-%s\n",MocapNETInputUncompressedArrayNames[i*3+1],MocapNETInputUncompressedArrayNames[j*3+1]);
                                }

                            if ( (doScaleCompensation) && (scaleDistance>0.0) )
                                {
                                    mocapnetCompressed.push_back( (float) iXMinusjXPlus0_5/scaleDistance );
                                    mocapnetCompressed.push_back( (float) iYMinusjYPlus0_5/scaleDistance );
                                }
                            else
                                {
                                    mocapnetCompressed.push_back(iXMinusjXPlus0_5);
                                    mocapnetCompressed.push_back(iYMinusjYPlus0_5);
                                }
                        }
                }
        }

 
    return  mocapnetCompressed;
}





std::vector<float> prepareMocapNETInputFromUncompressedInput(std::vector<float> mocapnetInput)
{ 
    std::vector<float> mocapnetUncompressedAndCompressed;
    mocapnetUncompressedAndCompressed.clear();

    if ( (MOCAPNET_UNCOMPRESSED_JOINT_PARTS * 3!=mocapnetInput.size())||(mocapnetInput.size()!=171) )
        {
          fprintf(stderr,RED "mocapNET: prepareMocapNETInputFromUncompressedInput : wrong input size , received %lu expected 171\n" NORMAL,mocapnetInput.size());
          return mocapnetUncompressedAndCompressed;
        }
    
    
    int addSyntheticPoints=1;
    int doScaleCompensation=0;
    std::vector<float> mocapnetCompressed = compressMocapNETInput(mocapnetInput,addSyntheticPoints,doScaleCompensation);
    

    mocapnetUncompressedAndCompressed.insert(mocapnetUncompressedAndCompressed.end(), mocapnetInput.begin(), mocapnetInput.end());
    mocapnetUncompressedAndCompressed.insert(mocapnetUncompressedAndCompressed.end(), mocapnetCompressed.begin(), mocapnetCompressed.end());
    //mocapnetUncompressedAndCompressed.insert(mocapnetInput.end(), mocapnetCompressed.begin(), mocapnetCompressed.end());
    //std::cerr<<"done \n";
    return  mocapnetUncompressedAndCompressed;
}



float undoOrientationTrickForBackOrientation(float orientation)
{
    if (orientation==180)
        {
            orientation=0;
        }
    else if (orientation<0)
        {
            orientation=180-orientation;
        }
    else
        {
            orientation=180-orientation;
        }
    return orientation;
}

std::vector<float> runMocapNET(struct MocapNET * mnet,std::vector<float> input)
{
    std::vector<float> emptyResult;
    std::vector<float> mnetInput;

    if (input.size()==749)
        {
            fprintf(stderr,"MocapNET: Input was given precompressed\n");
            mnetInput = input;
        }
    else if (input.size()==171)
        {
            //This is the default case so dont issue any warnings..
            //std::cerr<<"MocapNET: COCO input has "<<input.size()<<" elements (should be 171)\n";
            mnetInput = prepareMocapNETInputFromUncompressedInput(input);
            //std::cerr<<"MocapNET: COCO 171 input has been converted in to MocapNET input with "<<mnetInput.size()<<" elements (should be 749)\n";
        }
    else if (input.size()!=171)
        {
            fprintf(stderr,"MocapNET: Incorrect size of COCO input  was %lu (but should be 171) \n",input.size());
            return emptyResult;
        }

    if (mnetInput.size()!=749)
        {
            fprintf(stderr,"MocapNET: Incorrect size of MocapNET input .. \n");
            return emptyResult;
        }

    std::vector<float> direction = predictTensorflow(&mnet->allModel,mnetInput);

    if (direction.size()>0)
        {
            fprintf(stderr,NORMAL "Direction is : %0.2f " NORMAL , direction[0] );
            if ( (direction[0]<-90) || (direction[0]>90) )
                {
                    //Back ----------------------------------------------=
                    fprintf(stderr,"Back\n");
                    std::vector<float> result = predictTensorflow(&mnet->backModel,mnetInput);
                    result[4]=undoOrientationTrickForBackOrientation(result[4]);
                    return result;
                }
            else
                {
                    //Front ----------------------------------------------
                    fprintf(stderr,"Front\n");
                    std::vector<float> result = predictTensorflow(&mnet->frontModel,mnetInput);
                    return result;
                }
        }
    else
        {
            fprintf(stderr,"Unable to predict pose direction..\n");
        }

//-----------------
    return emptyResult;
}




int unloadMocapNET(struct MocapNET * mnet)
{
    return (
               ( unloadTensorflow(&mnet->allModel  ) ) &&
               ( unloadTensorflow(&mnet->frontModel) ) &&
               ( unloadTensorflow(&mnet->backModel) )
           );
}
