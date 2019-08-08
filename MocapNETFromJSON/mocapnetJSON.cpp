#include "../MocapNETLib/mocapnet.hpp"
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>

#include "../MocapNETLib/tools.h"
#include "../MocapNETLib/jsonCocoSkeleton.h"
#include "../MocapNETLib/jsonMocapNETHelpers.hpp"
#include "../MocapNETLib/bvh.hpp"
#include "../MocapNETLib/visualization.hpp"

int main(int argc, char *argv[])
{
  unsigned int width=1920 , height=1080 , frameLimit=10000 , visualize = 0, useCPUOnly=1;
  const char * path=0;

  if (initializeBVHConverter())
    {
     std::cerr<<"BVH code initalization successfull..\n";
    }


  for (int i=0; i<argc; i++)
  {
    if (strcmp(argv[i],"-v")==0)            { visualize=1; } else
    if (strcmp(argv[i],"--visualize")==0)   { visualize=1; } else
    if (strcmp(argv[i],"--maxFrames")==0)   { frameLimit=atoi(argv[i+1]); } else
    //if (strcmp(argv[i],"--cpu")==0)         { setenv("CUDA_VISIBLE_DEVICES", "", 1); } else
    if (strcmp(argv[i],"--gpu")==0)         { useCPUOnly=0; } else
    if (strcmp(argv[i],"--from")==0)        { path = argv[i+1];  } else
    if (strcmp(argv[i],"--size")==0)        { width = atoi(argv[i+1]); height = atoi(argv[i+2]); }
  }

  if (path==0)
   {
     path="frames/dance.webm-data";
   }






  struct MocapNET mnet={0};
  if ( loadMocapNET(&mnet,"test",useCPUOnly) )
  {
  char filePathOfJSONFile[1024]={0};
  snprintf(filePathOfJSONFile,1024,"%s/colorFrame_0_00000.jpg",path);

  if ( getImageWidthHeight(filePathOfJSONFile,&width,&height) )
  {
     fprintf(stderr,"Image dimensions changed from default to %ux%u",width,height);
  }



   float totalTime=0.0;
   unsigned int totalSamples=0;

   std::vector<std::vector<float> > bvhFrames;
   struct skeletonCOCO skeleton={0};

   unsigned int frameID=0;
   while (frameID<frameLimit)
    {
     snprintf(filePathOfJSONFile,1024,"%s/colorFrame_0_%05u_keypoints.json",path,frameID);

     if (parseJsonCOCOSkeleton(filePathOfJSONFile,&skeleton))
       {
          std::vector<float> inputValues = flattenskeletonCOCOToVector(&skeleton,width,height);
          if (inputValues.size()==0)
           { fprintf(stderr,"Failed to read from JSON file..\n"); }

          long startTime = GetTickCountMicrosecondsMN();
          //--------------------------------------------------------
           std::vector<float>  result = runMocapNET(&mnet,inputValues);
           bvhFrames.push_back(result);
          //--------------------------------------------------------
          long endTime = GetTickCountMicrosecondsMN();


          float sampleTime = (float) (endTime-startTime)/1000;
          if (sampleTime==0.0) { sampleTime=1.0; } //Take care of division by null..

          float fpsMocapNET = (float) 1000/sampleTime;
          fprintf(stderr,"Sample %u - %0.4fms - %0.4f fps\n",frameID,sampleTime,fpsMocapNET);


          if (visualize)
           {
             visualizePoints("3D Points Output",frameID,0,fpsMocapNET,width,height,result);
           }


          totalTime+=sampleTime;
          ++totalSamples;

       } else
       {
         std::cerr<<"Done.. \n";
         break;
       }

     ++frameID;
    }


    if (totalSamples>0)
    {
      char * bvhHeaderToWrite=0;
      if ( writeBVHFile("out.bvh",bvhHeaderToWrite, bvhFrames) )
              { std::cerr<<"Successfully wrote "<<bvhFrames.size()<<" frames to bvh file.. \n";  } else
              { std::cerr<<"Failed to write "<<bvhFrames.size()<<" frames to bvh file.. \n";     }



      float averageTime=(float) totalTime/totalSamples;
      std::cerr<<"\nTotal "<<totalTime<<"ms for "<<totalSamples<<" samples - Average "<<averageTime<<"ms - "<<1000/averageTime<<" fps\n";
    }



   unloadMocapNET(&mnet);
  }
}
