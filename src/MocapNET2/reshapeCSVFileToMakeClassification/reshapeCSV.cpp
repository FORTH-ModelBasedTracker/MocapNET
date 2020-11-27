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
#include "../MocapNETLib2/core/core.hpp" 
#include "../MocapNETLib2/IO/csvRead.hpp" 


int main(int argc, char *argv[])
{
     struct MocapNET2 mnet;
     struct CSVFileContext csvT={0};
     unsigned long processedLines=0; 

     fprintf(stderr,"Run : \n");
     fprintf(stderr,"./createRandomizedDataset.sh\n");
     fprintf(stderr,"to update the dataset/bvh_body_all.csv to the latest version..\n\n");
   
     fprintf(stdout,"Front,Back,Left,Right\n");  

     
     char csvFileInput[1024];
     snprintf(csvFileInput,1024,"dataset/bvh_body_all.csv");
     
     if (argc==2)
     {
         snprintf(csvFileInput,1024,"%s",argv[1]);
     } else
    {
       fprintf(stderr,"Please provide an argument i.e. \n   ./reshapeCSV dataset/bvh_body_all.csv"); 
      return 0; 
    }
     
     
     if (openCSVFile(&csvT,csvFileInput))
                {
                    fprintf(stderr,"CSV file had %u lines\n",csvT.lineNumber);
                    //----------------------------------------------------
                    struct skeletonSerialized skeletonS= {0};
                    while ( parseNextCSVCOCOSkeleton(&csvT,&skeletonS) )
                    //----------------------------------------------------
                                {
                                     if (processedLines==0)
                                         { initializeMocapNET2InputAssociation(&mnet,&skeletonS,1,1,1); }
                                         
                                     unsigned int correspondingClass = getMocapNETOrientationFromAngle(skeletonS.skeletonBody[4].value);
                                     
                                     if (processedLines%1000==0) { fprintf(stderr,"."); }
                                     //fprintf(stderr,".[%0.2f/%u]",skeletonS.skeletonBody[4].value,correspondingClass);
                                     
                                     /*
                                      enum MOCAPNET_Orientation
                                       {
                                          MOCAPNET_ORIENTATION_NONE=0,
                                          MOCAPNET_ORIENTATION_FRONT,
                                          MOCAPNET_ORIENTATION_BACK,
                                          MOCAPNET_ORIENTATION_LEFT,
                                          MOCAPNET_ORIENTATION_RIGHT,
                                          //----------------------------- 
                                          MOCAPNET_ORIENTATION_NUMBER
                                       };
                                      */
                                      
                                     switch(correspondingClass)
                                     {     
                                       //The series here is important for MocapNETLib2/core.cpp:
                                       case  MOCAPNET_ORIENTATION_NONE:  fprintf(stdout,"0,0,0,0\n");   break; //Add erroneous categories to catch bugs
                                       case  MOCAPNET_ORIENTATION_FRONT: fprintf(stdout,"1,0,0,0\n");   break; 
                                       case  MOCAPNET_ORIENTATION_BACK:  fprintf(stdout,"0,1,0,0\n");   break;
                                       case  MOCAPNET_ORIENTATION_LEFT:  fprintf(stdout,"0,0,1,0\n");   break;
                                       case  MOCAPNET_ORIENTATION_RIGHT: fprintf(stdout,"0,0,0,1\n");   break;
                                       default :                         fprintf(stdout,"0,0,0,0\n");   break; //Add erroneous categories to catch bugs
                                     };
                                     
                                     ++processedLines;
                                }
                    //----------------------------------------------------
                    closeCSVFile(&csvT);
                }


     fprintf(stderr,"\nDon't forget, Run : \n");
     fprintf(stderr,"./createRandomizedDataset.sh\n");
     fprintf(stderr,"to update the dataset/bvh_body_all.csv to the latest version..\n\n");

    exit(0);    
}
