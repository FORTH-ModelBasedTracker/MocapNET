
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>

#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/tools.h"
#include "../MocapNETLib/jsonCocoSkeleton.h"
#include "../MocapNETLib/jsonMocapNETHelpers.hpp"

int writeCSVHeader(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height)
{
  std::vector<float> inputValues = flattenskeletonCOCOToVector(skeleton,width,height);

  FILE * fp = fopen(filename,"w");
   if (fp!=0)
     {
       for (int i=0; i<inputValues.size(); i++)
       {
         fprintf(fp,"%s",MocapNETInputUncompressedArrayNames[i]);
         if (i<inputValues.size()-1) { fprintf(fp,","); }
       }
      fprintf(fp,"\n");
      fclose(fp);
      return 1;
     }
  return 0;
}

int writeCSVBody(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height)
{
  FILE * fp = fopen(filename,"a");
   if (fp!=0)
     {
       std::vector<float> inputValues = flattenskeletonCOCOToVector(skeleton,width,height);
       if (inputValues.size()==0)
           { fprintf(stderr,"Failed to read from JSON file..\n"); }

       for (int i=0; i<inputValues.size(); i++)
       {
         if (i%3==2)
         {
           fprintf(fp,"%0.1f",inputValues[i]);
         } else
         {
           fprintf(fp,"%f",inputValues[i]);
         }

         if (i<inputValues.size()-1) { fprintf(fp,","); }
       }
      fprintf(fp,"\n");
      fclose(fp);
      return 1;
     }
  return 0;
}



int main(int argc, char *argv[])
{
  unsigned int width=1920 , height=1080 , frameLimit=100000 , processed = 0;
  const char * path=0;
  char outputPathFull[2048];
  const char * outputPath=0;
  float version=1.2;

  for (int i=0; i<argc; i++)
  {
    if (strcmp(argv[i],"--maxFrames")==0)   { frameLimit=atoi(argv[i+1]); } else
    if (strcmp(argv[i],"--from")==0)        { path = argv[i+1];           } else
    if (strcmp(argv[i],"-i")==0)            { path = argv[i+1];           } else
    if (strcmp(argv[i],"--out")==0)         { outputPath = argv[i+1];     } else
    if (strcmp(argv[i],"-o")==0)            { outputPath = argv[i+1];     } else
    if (strcmp(argv[i],"-v")==0)            { version = atof(argv[i+1]);  }
  }

  if (path==0)
   {
     path="frames/dance.webm-data";
   }

  if (outputPath==0)
   {
     snprintf(outputPathFull,2048,"%s/2dJoints_v%0.1f.csv",path,version);
   } else
   {
     snprintf(outputPathFull,2048,"%s/2dJoints_v%0.1f.csv",outputPath,version);
   }

  char filePathOfJSONFile[2048]={0};
  snprintf(filePathOfJSONFile,2048,"%s/colorFrame_0_00000.jpg",path);

  if ( getImageWidthHeight(filePathOfJSONFile,&width,&height) )
  {
     fprintf(stderr,"Image dimensions changed from default to %ux%u",width,height);
  }


   float totalTime=0.0;
   unsigned int totalSamples=0;

   struct skeletonCOCO skeleton={0};

   unsigned int frameID=0;
   while (frameID<frameLimit)
    {
     snprintf(filePathOfJSONFile,1024,"%s/colorFrame_0_%05u_keypoints.json",path,frameID);
     fprintf(stderr,"Processing %s \n",filePathOfJSONFile);

     if (parseJsonCOCOSkeleton(filePathOfJSONFile,&skeleton))
       {
          if (processed==0)
          {
            writeCSVHeader(outputPathFull,&skeleton,width,height);
          }

          writeCSVBody(outputPathFull,&skeleton,width,height);
          ++processed;
       } else
       {
         fprintf(stderr,"Done processing %u frames..\n",frameID);
         break;
       }

     ++frameID;
    }
}
