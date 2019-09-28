#include "../Tensorflow/tf_utils.hpp"
#include "mocapnet.hpp"
#include "nsdm.hpp"
#include "jsonCocoSkeleton.h"

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

int loadMocapNET(struct MocapNET * mnet,const char * filename,float qualitySetting,int mode,unsigned int forceCPU)
{
    char modelPath[1024]= {0};
    int result = 0;

    switch (mode)
        {
        case 3:
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            //                                                  Regular 3 Model setup such as the BMVC 2019 Work..
            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/all.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[0]  ,modelPath  ,"input_all"  ,"result_all/concat",forceCPU);
            mnet->modelLimits[0].isFlipped=0;
            mnet->modelLimits[0].numberOfLimits=1;
            mnet->modelLimits[0].minimumYaw1=-360.0;
            mnet->modelLimits[0].maximumYaw1=360.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/front.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[1],modelPath,"input_front","result_front/concat",forceCPU);
            mnet->modelLimits[1].isFlipped=0;
            mnet->modelLimits[1].numberOfLimits=1;
            mnet->modelLimits[1].minimumYaw1=-90.0;
            mnet->modelLimits[1].maximumYaw1=90.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            snprintf(modelPath,1024,"combinedModel/%0.1f/back.pb",qualitySetting);
            result += loadTensorflowInstance(&mnet->models[2] ,modelPath ,"input_back" ,"result_back/concat",forceCPU);
            mnet->modelLimits[2].isFlipped=0;
            mnet->modelLimits[2].numberOfLimits=2;
            mnet->modelLimits[2].minimumYaw1=-90.0;
            mnet->modelLimits[2].maximumYaw1=-270.0;
            mnet->modelLimits[2].minimumYaw1=90.0;
            mnet->modelLimits[2].maximumYaw1=270.0;

            //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            if(result==3)
                {
                    result=1;
                    mnet->loadedModels=3;
                }
            else
                {
                    result=0;
                }
            break;

        default:
            fprintf(stderr,RED "You requested a MocapNET configuration that is incorrect  ( mode=%u )\n" NORMAL , mode);
            return 0;
            break;
        };

    if (result)
        {
            fprintf(stderr,"Caching networks after initialization to be ready for use..\n");
            std::vector<float> emptyValues;
            for (int i=0; i<749; i++)
                {
                    emptyValues.push_back(0.0);
                }

            for (int i=0;  i<mnet->loadedModels; i++ )
                {
                    std::vector<float>  prediction = predictTensorflow(&mnet->models[i],emptyValues);
                    if (prediction.size()>0)
                        {
                            fprintf(stderr, GREEN "Caching and testing model %u was successful\n" NORMAL ,i);
                        }
                    else
                        {
                            fprintf(stderr,RED "Caching and testing model %u was unsuccessful\n" NORMAL,i);
                        }
                }
        }

    return result;
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
    std::vector<float> mocapnetCompressed = compressMocapNETInputToNSDM(mocapnetInput,addSyntheticPoints,doScaleCompensation);


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

    std::vector<float> direction = predictTensorflow(&mnet->models[0],mnetInput);

    if (direction.size()>0)
        {
            fprintf(stderr,NORMAL "Direction is : %0.2f " NORMAL , direction[0] );
            if ( (direction[0]<-90) || (direction[0]>90) )
                {
                    //Back ----------------------------------------------=
                    fprintf(stderr,"Back\n");
                    std::vector<float> result = predictTensorflow(&mnet->models[2],mnetInput);
                    result[4]=undoOrientationTrickForBackOrientation(result[4]);
                    return result;
                }
            else
                {
                    //Front ----------------------------------------------
                    fprintf(stderr,"Front\n");
                    std::vector<float> result = predictTensorflow(&mnet->models[1],mnetInput);
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
    unsigned int result=0;
    for (int i=0;  i<mnet->loadedModels; i++ )
        {
            result+=unloadTensorflow(&mnet->models[i]);
        }

    if (result==mnet->loadedModels)
        {
            result=1;
        }
    else
        {
            result=0;
        }
    mnet->loadedModels=0;

    return result;
}
