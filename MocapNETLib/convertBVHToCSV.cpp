/*
 * Export utility from out BVH Files back to CSV compatible input  
 * Sample call :     
*    We can turn one of the gestures included to a 360 pose bvh file to test orientation code
*       ./GroundTruthDumper --from dataset/gestures/comeleft.bvh --360 1 --bvh test.bvh
*    We can use this utility to convert the bvh file to a CSV file 
*       ./convertBVHToCSV --from test.bvh -o test.csv
*    We can then try using the file  
*       ./MocapNETJSON --from test.csv --visualize
 * */

#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>

#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/jsonMocapNETHelpers.hpp"
#include "../MocapNETLib/tools.h"
#include "../MocapNETLib/csv.hpp"
#include "../MocapNETLib/bvh.hpp"


int main(int argc, char *argv[])
{

    unsigned int width=1920 , height=1080 , frameLimit=100000 , processed = 0 , serialLength = 5;
    const char * path=0;
    const char * label=0;
    char outputPathFull[2048]={0};
    const char * outputPath=0;
    float version=1.2;

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

   
   
    std::vector<std::vector<float> > bvhFile = loadBVHFileMotionFrames(path);


//       ./GroundTruthDumper --from dataset/gestures/comeleft.bvh --360 1 --bvh test.bvh
//       ./convertBVHToCSV --from test.bvh -o test.csv
//       ./MocapNETJSON --from test.csv --visualize --unconstrained --delay 100

    struct skeletonCOCO skeleton= {0};
     writeCSVHeaderFromSkeleton(outputPathFull,&skeleton,width,height);
    for (int frameID=0; frameID<bvhFile.size(); frameID++)
    { 
          fprintf(stderr,".");
          convertBVHFrameToSkeletonCOCO(&skeleton,bvhFile[frameID],width,height); 
          writeCSVBodyFromSkeleton(outputPathFull,&skeleton,width,height);
    }


  fprintf(stderr,"Done..\n");



 
}
