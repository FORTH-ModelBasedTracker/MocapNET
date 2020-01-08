/*
 * Export utility from OpenPose BODY 25 JSON format to a more regular CSV file
 * Sample call :   ./convertBody25JSONToCSV --from frames/GOPR3223.MP4-data/ --label colorFrame_0_ -o .
 * */

#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>

#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/tools.h"
#include "../MocapNETLib/csv.hpp"


int main(int argc, char *argv[])
{

    unsigned int width=1920 , height=1080 , frameLimit=100000 , processed = 0 , serialLength = 5;
    const char * path=0;
    const char * label=0;
    char outputPathFull[2048]={0};
    const char * outputPath=0;
    float version=1.4;

    for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--maxFrames")==0)
                {
                    frameLimit=atoi(argv[i+1]);
                }
            else if (strcmp(argv[i],"--from")==0)
                {
                    path = argv[i+1];
                }
            else if (strcmp(argv[i],"-i")==0)
                {
                    path = argv[i+1];
                }
            else if (strcmp(argv[i],"--label")==0)
                {
                    label = argv[i+1];
                }
            else if (strcmp(argv[i],"--out")==0)
                {
                    outputPath = argv[i+1];
                }
            else if (strcmp(argv[i],"-o")==0)
                {
                    outputPath = argv[i+1];
                }
            else if (strcmp(argv[i],"-v")==0)
                {
                    version = atof(argv[i+1]);
                }
            else if (strcmp(argv[i],"--seriallength")==0)
                {
                    serialLength = atoi(argv[i+1]);
                }
            else if (strcmp(argv[i],"--size")==0)
                {
                    width = atoi(argv[i+1]);
                    height = atoi(argv[i+2]);
                }
        }

    if (path==0)
        {
            path="./";
        }

    if (label==0)
        {
            label="colorFrame_0_";
        }

    if (outputPath==0)
        {
            snprintf(outputPathFull,2048,"%s/2dJoints_v%0.1f.csv",path,version);
        }
    else
        {
            if (strstr(outputPath,".csv")!=0)
            {
               fprintf(stderr,"Output path is actually a csv file so treating it as one..\n"); 
              snprintf(outputPathFull,2048,"%s",outputPath);            
            } else
            {
              snprintf(outputPathFull,2048,"%s/2dJoints_v%0.1f.csv",outputPath,version);               
            }
        }


    char formatString[256]= {0};
    snprintf(formatString,256,"%%s/%%s%%0%uu_keypoints.json",serialLength);

    char filePathOfJSONFile[2048]= {0};
    snprintf(filePathOfJSONFile,2048,"%s/colorFrame_0_00001.jpg",path);

    if ( getImageWidthHeight(filePathOfJSONFile,&width,&height) )
        {
            fprintf(stderr,"Image dimensions changed from default to %ux%u",width,height);
        }
    else
        {
            fprintf(stderr,"Assuming default image dimensions %ux%u , you can change this using --size x y\n",width,height);
        }


    float totalTime=0.0;
    unsigned int totalSamples=0;

    struct skeletonCOCO skeleton= {0};

    unsigned int frameID=1;
    
    fprintf(stderr,"Processing : \n",filePathOfJSONFile);

    while (frameID<frameLimit)
        {
            snprintf(filePathOfJSONFile,1024,formatString,path,label,frameID);
            fprintf(stderr,"%s (%ux%u)\n",filePathOfJSONFile,width,height);

            if (parseJsonCOCOSkeleton(filePathOfJSONFile,&skeleton,0.4))
                {
                    if (processed==0)
                        {
                            writeOpenPoseCSVHeaderFromSkeleton(outputPathFull,&skeleton,width,height);
                        }
                     
                     if (skeleton.leftHand.isPopulated)    { fprintf(stderr,"Left Hand "); }   
                     if (skeleton.rightHand.isPopulated)  { fprintf(stderr,"Right Hand "); }   
                     if (skeleton.body.isPopulated)             { fprintf(stderr,"Body "); }   
                     if (skeleton.head.isPopulated)             { fprintf(stderr,"Head "); }   

                    writeOpenPoseCSVBodyFromSkeleton(outputPathFull,&skeleton,width,height);
                    ++processed;
                }
            else
                { 
                    fprintf(stderr,"Done processing %u frames..\n",frameID);
                    fprintf(stderr,"Output has been stored at %s \n",outputPathFull);
                    break;
                }

            ++frameID;
        }
}
