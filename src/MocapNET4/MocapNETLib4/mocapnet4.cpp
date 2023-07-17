//MOCAPNET2 ------------------------------------
#include "../MocapNETLib4/mocapnet4.h"
//----------------------------------------------
#include "../MocapNETLib4/config.h"
#include "../MocapNETLib4/JSON/readModelConfiguration.h"
//----------------------------------------------

#include "tools.h"
#include "../../../dependencies/nxjson/nxjson.h"

#include <stdio.h>
#include <string.h>
 
#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */




int loadMocapNET4(
                   struct MocapNET4 * mnet,
                   const char * description
                 )
{
    
  unsigned int length = 0;
  char * data = readFileToMemory("dataset/combinedModel/mocapnet4/mode1/1.0/step1_upperbody_all",&length);
  
  struct ModelConfigurationData modelConfiguration={0};
  loadModelConfigurationData(&modelConfiguration,"dataset/combinedModel/mocapnet4/mode1/1.0/step1_upperbody_all/upperbody_configuration.json");
    
  return 0;
}



std::vector<float> runMocapNET4(
                                 struct MocapNET4 * mnet,
                                 struct skeletonSerialized * input,
                                 int doLowerbody,
                                 int doHands,
                                 int doFace,
                                 int doGestureDetection,
                                 unsigned int useInverseKinematics,
                                 int doOutputFiltering
                                )
{
   std::vector<float> emptyResult;
    return emptyResult;
}




int unloadMocapNET4(struct MocapNET4 * mnet)
{
  return 0;
}
