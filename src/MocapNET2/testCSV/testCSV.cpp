/*
 * Utility to extract BVH files straight from OpenPose JSON output
 * Sample usage ./MocapNETCSV  --from test.csv   --visualize 
 */
 
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include <unistd.h>
 
#include "../MocapNETLib2/mocapnet2.hpp" 
#include "../MocapNETLib2/IO/csvRead.hpp" 


int main(int argc, char *argv[])
{
    struct MocapNET2 mnet;
     struct CSVFileContext csvT={0};
     
     fprintf(stderr,"Run : \n");
     fprintf(stderr,"./convertOpenPoseJSONToCSV --from frames/GOPR3246.MP4-data/ -o newtest.csv\n");
     fprintf(stderr,"to update the newtest.csv to the latest version..\n\n");

     
     if (openCSVFile(&csvT,"newtest.csv"))
                {
                    fprintf(stderr,"CSV file had %u lines\n",csvT.lineNumber);
                    //----------------------------------------------------
                    struct skeletonSerialized skeletonS= {0};
                    while ( parseNextCSVCOCOSkeleton(&csvT,&skeletonS) )
                    //----------------------------------------------------
                                {
                                     initializeMocapNET2InputAssociation(&mnet,&skeletonS,1,1,1);
                                     fprintf(stderr,"...\n");
                                     
     fprintf(stderr,"Don't forget, Run : \n");
     fprintf(stderr,"./convertOpenPoseJSONToCSV --from frames/GOPR3246.MP4-data/ -o newtest.csv\n");
     fprintf(stderr,"to update the newtest.csv to the latest version..\n\n");
     
                                     exit(0);
                                }
                    //----------------------------------------------------
                    closeCSVFile(&csvT);
                }


     fprintf(stderr,"Don't forget, Run : \n");
     fprintf(stderr,"./convertOpenPoseJSONToCSV --from frames/GOPR3246.MP4-data/ -o newtest.csv\n");
     fprintf(stderr,"to update the newtest.csv to the latest version..\n\n");

    exit(0);    
}
