#include "bvh.hpp"
#include <string.h>

#include "../config.h"

#include "commonSkeleton.hpp"
#include "skeletonSerializedToBVHTransform.hpp"
#include "../mocapnet2.hpp"
#include "../visualization/opengl.hpp"

#include "../solutionParts/rightHand.hpp"
#include "../solutionParts/leftHand.hpp"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

#if USE_BVH
#include "../../../../dependencies/RGBDAcquisition/tools/Calibration/calibration.h"
#include "../../../../dependencies/RGBDAcquisition/tools/AmMatrix/matrix4x4Tools.h"
#include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
#include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/calculate/bvh_project.h"
#include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/calculate/bvh_transform.h"
#include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/ik/bvh_inverseKinematics.h"
#include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/ik/hardcodedProblems_inverseKinematics.h"


//Casting our solution/previous solution/penultimate solution in C arrays
struct MotionBuffer solution={0};
struct MotionBuffer previousSolution={0};
struct MotionBuffer penultimateSolution={0};

struct BVH_MotionCapture bvhMotion = {0};
struct BVH_Transform bvhTransform  = {0};
//---------------------------------------
struct BVH_MotionCapture standaloneLHand = {0};
struct BVH_MotionCapture standaloneRHand = {0};
//---------------------------------------
struct ikProblem * bodyProblem = 0;
struct ikProblem * leftHandProblem = 0;
struct ikProblem * rightHandProblem = 0;
struct ikProblem * faceProblem = 0;
//---------------------------------------
struct ikConfiguration ikConfig = {0};
//---------------------------------------
int haveBVHInit=0;
#else
#warning "BVH code not included.."
#endif // USE_BVH


float fX = 582.18394;
float fY = 582.52915;


#if USE_BVH
    struct simpleRenderer renderer = {0};
#endif


void overrideBVHSubsystemFocalLength(float newFx,float newFy)
{
    fX = newFx;
    fY = newFy;
}

int codeOptimizationsForIKEnabled()
{
  return codeHasSSE();
}

void * getRendererHandle()
{
    return (void*) &renderer;
}

void * getBVHMotionHandle()
{
    return (void*) &bvhMotion;
}

void * getBVHLHandQBVHMotionHandle()
{
    return (void*) &standaloneLHand;
}

void * getBVHRHandQBVHMotionHandle()
{
    return (void*) &standaloneRHand;
}

void printBVHSummary()
{
    bvh_printBVH(&bvhMotion);
}

void printBVHCodeVersion()
{
  fprintf(stderr,"BVH subsystem version %s\n",BVH_LOADER_VERSION_STRING);
}

int initializeIK(int initBody,int initLeftHand,int initRightHand,int initFace)
{
    if (
         (initBody==0) && (initLeftHand==0) && (initRightHand==0) && (initFace==0)
       )
       {
          fprintf(stderr,YELLOW "Don't need to initialize IK code..\n" NORMAL);
          return 1;
       }

    fprintf(stderr,"Initializing IK code..\n");
    int failures = 0;

    //------------------------------------------------------------------------------------------------------------------------
    if (initBody)
    {
     bodyProblem      =  allocateEmptyIKProblem();
     if (bodyProblem==0) { fprintf(stderr,"Failed to allocate memory for our IK bodyProblem..\n");  ++failures; }
    }
    //------------------------------------------------------------------------------------------------------------------------
    if (initLeftHand)
    {
     leftHandProblem  = allocateEmptyIKProblem();
     if (leftHandProblem==0) { fprintf(stderr,"Failed to allocate memory for our IK leftHandProblem..\n");  ++failures; }
    }
    //------------------------------------------------------------------------------------------------------------------------
    if (initRightHand)
    {
     rightHandProblem = allocateEmptyIKProblem();
     if (rightHandProblem==0) { fprintf(stderr,"Failed to allocate memory for our IK rightHandProblem..\n");  ++failures; }
    }
    //------------------------------------------------------------------------------------------------------------------------
    if (initFace)
    {
     faceProblem = allocateEmptyIKProblem();
     if (faceProblem==0) { fprintf(stderr,"Failed to allocate memory for our IK faceProblem..\n");  ++failures; }
    }
    //------------------------------------------------------------------------------------------------------------------------

    struct MotionBuffer * solution         = mallocNewMotionBuffer(&bvhMotion);
    failures += (solution==0);

    struct MotionBuffer * previousSolution = mallocNewMotionBuffer(&bvhMotion);
    failures += (previousSolution==0);

    fprintf(stderr,"Preparing IK problems..\n");
    //------------------------------------------------------------------------------------------------------------------------
    if (initBody)
    {
    if (!prepareDefaultBodyProblem(
                                     bodyProblem,
                                     &bvhMotion,
                                     &renderer,
                                     previousSolution,
                                     solution,
                                     &bvhTransform
                                  )
         )
         {
          fprintf(stderr,RED "MocapNET2/BVH: Could not initializeIK() for an IK solution\n" NORMAL);
          ++failures;
         }
    }
    //------------------------------------------------------------------------------------------------------------------------
    if (initRightHand)
    {
     if (!prepareDefaultRightHandProblem(
                                        rightHandProblem,
                                        &bvhMotion,
                                        &renderer,
                                        previousSolution,
                                        solution,
                                        &bvhTransform,
                                        (!initBody)
                                       )
         )
         {
          fprintf(stderr,RED "MocapNET2/BVH: Could not initializeIK() for an IK solution for right hand\n" NORMAL);
          ++failures;
         }
    }
    //------------------------------------------------------------------------------------------------------------------------
    if (initLeftHand)
    {
     if (!prepareDefaultLeftHandProblem(
                                       leftHandProblem,
                                       &bvhMotion,
                                       &renderer,
                                       previousSolution,
                                       solution,
                                       &bvhTransform,
                                        (!initBody)
                                      )
         )
         {
          fprintf(stderr,RED "MocapNET2/BVH: Could not initializeIK() for an IK solution for left hand\n" NORMAL);
          ++failures;
         }
    }
    //------------------------------------------------------------------------------------------------------------------------
    if (initFace)
    {
     if (!prepareDefaultFaceProblem(
                                       faceProblem,
                                       &bvhMotion,
                                       &renderer,
                                       previousSolution,
                                       solution,
                                       &bvhTransform,
                                        (!initBody)
                                      )
         )
         {
          fprintf(stderr,RED "MocapNET2/BVH: Could not initializeIK() for an IK solution for face \n" NORMAL);
          ++failures;
         }
    }
    //------------------------------------------------------------------------------------------------------------------------


    fprintf(stderr,"All Done with %u failures..\n",failures);


    if (failures>0)
    {
      fprintf(stderr,"%u failures encountered..\n",failures);

      //Dump everything on failure..!
      if (bodyProblem!=0)      { free(bodyProblem); }
      if (rightHandProblem!=0) { free(rightHandProblem); }
      if (leftHandProblem!=0)  { free(leftHandProblem); }
      if (faceProblem!=0)      { free(faceProblem); }

      freeMotionBuffer(&solution);
      solution=0;// Double make sure this is clean..
      freeMotionBuffer(&previousSolution);
      previousSolution=0;// Double make sure this is clean..
    }

    return (failures==0);
}




int initializeBVHConverter(const char * specificBVHFilename,int width,int height,int noIKNeeded)
{
#if USE_BVH
    fprintf(stderr,"Using BVH codebase version %s\n",BVH_LOADER_VERSION_STRING);

    simpleRendererDefaults(
        &renderer,
        width,//1920
        height,//1080
        fX, //570.0
        fY  //570.0
    );
    simpleRendererInitialize(&renderer);

     const char * selectedBVHFile = specificBVHFilename;
     if (specificBVHFilename==0) { selectedBVHFile="dataset/headerWithHeadAndOneMotion.bvh"; }


     int initBody=1;
     int initLeftHand=1;
     int initRightHand=1;
     int initFace=1;

     if (strstr(selectedBVHFile,"head.bvh")!=0)
     {
         fprintf(stderr,YELLOW "Initializing a solo Head..\n" NORMAL);
         initBody=0;
         initLeftHand=0;
         initRightHand=0;
         initFace=1;
     }

     if ( (strstr(selectedBVHFile,"rhand.bvh")!=0) || (strstr(selectedBVHFile,"rhand.qbvh")!=0) )
     {
         fprintf(stderr,YELLOW "Initializing a solo rHand..\n" NORMAL);
         initBody=0;
         initLeftHand=0;
         initFace=0;
     }

     if ( (strstr(selectedBVHFile,"lhand.bvh")!=0) || (strstr(selectedBVHFile,"lhand.qbvh")!=0) )
     {
         fprintf(stderr,YELLOW "Initializing a solo lHand..\n" NORMAL);
         initBody=0;
         initRightHand=0;
         initFace=0;
     }


     if (noIKNeeded)
     {
         initBody=0;
         initLeftHand=0;
         initRightHand=0;
         initFace=0;
     }

  if (!bvh_loadBVH("dataset/lhand.qbvh",&standaloneLHand,1.0) )
   {
       fprintf(stderr,"Could not load auxilary QBVH lhand\n");
       return 0;
   }
  if (!bvh_loadBVH("dataset/rhand.qbvh",&standaloneRHand,1.0) )
   {
       fprintf(stderr,"Could not load auxilary QBVH rhand\n");
       return 0;
   }

    //if ( bvh_loadBVH("dataset/headerAndOneMotion.bvh",&bvhMotion,1.0) ) //This is the old armature that only has the eyes
     if ( bvh_loadBVH(selectedBVHFile,&bvhMotion,1.0) ) // This is the new armature that includes the head
        {
            fprintf(stderr,"BVH subsystem initialized using %s that contains %u frames ( %u motions each )\n",selectedBVHFile,bvhMotion.numberOfFrames,bvhMotion.numberOfValuesPerFrame);
            if (!initializeIK(initBody,initLeftHand,initRightHand,initFace))
            {
              fprintf(stderr,RED "Failed initializing IK..\n" NORMAL);
              //exit(0);
              return 0;
            }

            //Test joint scaling..
            //changeJointDimensions(&bvhMotion);
            haveBVHInit=1;
            return 1;
        }
    else
        {
            fprintf(stderr,RED "initializeBVHConverter: Failed to bvh_loadBVH(header.bvh)..\n" NORMAL);
        }
#else
    fprintf(stderr,YELLOW "initializeBVHConverter:  BVH code not compiled in..\n" NORMAL);
#endif // USE_BVH
    return 0;
}



int loadCalibration(struct MocapNET2Options * options,const char* directory,const char * file)
{
    fprintf(stderr,"loadCalibration %s , %s \n",directory,file);

    char loadPath[512]={0};

    if (directory!=0)
      { snprintf(loadPath,512,"%s/%s",directory,file); } else
      { snprintf(loadPath,512,"%s",file); }
    //---------------------------
    struct calibration calib={0};
    if (ReadCalibration(loadPath,0,0,&calib) )
    {
        //-----------------------------------
        fX = calib.intrinsic[CALIB_INTR_FX];
        fY = calib.intrinsic[CALIB_INTR_FY];
        options->width = calib.width;
        options->height = calib.height;
        options->visWidth= calib.width;
        options->visHeight = calib.height;
        //-----------------------------------
        fprintf(stderr,GREEN "loadCalibration working \n" NORMAL);
        fprintf(stderr,"Image Resolution loaded is %u x %u \n",calib.width,calib.height);
        fprintf(stderr,"Focal Lengths fx=%0.2f fy=%0.2f \n",fX,fY);
        if (!options->hasInit)
            {
              initializeBVHConverter(0,calib.width,calib.height,0);
              options->hasInit=1;
            }

        #if USE_OPENGL
         //If we are using OpenGL we need to update the renderer settings..!
         if (options->useOpenGLVisualization)
         {
          if (!changeOpenGLCalibration(&calib))
          {
           fprintf(stderr,"Failed setting OpenGL calibration\n");
          }
         }
        #endif

        return 1;
    }
    fprintf(stderr,RED "Failed @ loadCalibration %s , %s \n" NORMAL,directory,file);
    return 0;
}



int stopIK()
{
    if(bodyProblem!=0)
    {
      cleanProblem(bodyProblem);
      free(bodyProblem);
    }

    if(leftHandProblem!=0)
    {
      cleanProblem(leftHandProblem);
      free(leftHandProblem);
    }

    if(rightHandProblem!=0)
    {
      cleanProblem(rightHandProblem);
      free(rightHandProblem);
    }


   fprintf(stderr,"Deallocating solution MotionBuffers...");
   if (solution.motion!=0)            { free(solution.motion);             solution.motion=0;            solution.bufferSize = 0;            }
   if (previousSolution.motion!=0)    { free(previousSolution.motion);     previousSolution.motion=0;    previousSolution.bufferSize = 0;    }
   if (penultimateSolution.motion!=0) { free(penultimateSolution.motion);  penultimateSolution.motion=0; penultimateSolution.bufferSize = 0; }
   fprintf(stderr,"Survived...\n");

  return 1;
}




//This should fix the wrong Z Y X rotation thing happening
//in https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/35
int fixBVHHip(
              std::vector<std::vector<float> > &bvhFrames
             )
{
  return 0;
  for (unsigned int frameID=0; frameID<bvhFrames.size(); frameID++)
    {
     if (bvhFrames[frameID].size() >= 6)
     {
       //bvhFrames[frameID][0]=-bvhFrames[frameID][0];
       //bvhFrames[frameID][1]= bvhFrames[frameID][1];
       //bvhFrames[frameID][2]=-bvhFrames[frameID][2];

       bvhFrames[frameID][3]=-bvhFrames[frameID][3];
       bvhFrames[frameID][4]=-bvhFrames[frameID][4];
       bvhFrames[frameID][5]=-bvhFrames[frameID][5];
     }
    }

  return 0;
}


int writeBVHFile(
                 const char * filename,
                 const char * header,
                 int prependTPose,
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

            unsigned long numberOfFramesToWrite =  bvhFrames.size();
            if (prependTPose)
            {
                numberOfFramesToWrite+=1;
            }



            fprintf(fp,"Frames: %lu \n",numberOfFramesToWrite);
            fprintf(fp,"Frame Time: 0.04\n");

            unsigned int i=0,j=0;

            if (prependTPose)
            {
               if (bvhFrames.size()>1)
               {
                for (j=0; j< bvhFrames[0].size(); j++)
                        {
                            fprintf(fp,"0 ");
                        }
                    fprintf(fp,"\n");
               }
            }

            for (i=0; i< bvhFrames.size(); i++)
                {
                    std::vector<float> frame = bvhFrames[i];
                    if (frame.size()>0)
                    {
                    //fprintf(fp,"%lu joints",frame.size());

                    //fprintf(fp,"0.0 0.0 0.0 ");
                     for (j=0; j<frame.size(); j++)
                        {
                            if ( (frame[j]<0.0001) && (frame[j]>-0.0001) ) { fprintf(fp,"0 "); }               else // Reduce .bvh size..
                                                                           { fprintf(fp,"%0.4f ",frame[j]); }
                        }
                     fprintf(fp,"\n");
                    }
                }
            fclose(fp);
            return 1;
        }
    return 0;
}





void * loadBVHFile(const char * filename)
{
#if USE_BVH
    //struct BVH_MotionCapture tmp={0};
    struct BVH_MotionCapture * newBVHLoadedFile = (struct BVH_MotionCapture *) malloc(sizeof(struct BVH_MotionCapture));

    if (newBVHLoadedFile!=0)
    {
     memset(newBVHLoadedFile,0,sizeof(struct BVH_MotionCapture));
     if ( bvh_loadBVH(filename,newBVHLoadedFile,1.0) )
        {
            return (void*) newBVHLoadedFile;
        }
    }
#endif // USE_BVH
    return 0;
}




std::vector<std::vector<float> > loadBVHFileMotionFrames(const char * filename)
{
    std::vector<std::vector<float> > result;
#if USE_BVH
    struct BVH_MotionCapture bvh={0};

     if ( bvh_loadBVH(filename,&bvh,1.0) )
        {
            unsigned int frameID=0, jointID=0, c=0;
            for (frameID=0; frameID<bvh.numberOfFramesEncountered; frameID++)
            {
             std::vector<float> currentFrame;
             currentFrame.clear();
               for (jointID=0; jointID<bvh.numberOfValuesPerFrame; jointID++)
               {
                   currentFrame.push_back(bvh.motionValues[c]);
                   c++;
               }
             result.push_back(currentFrame);
            }
            bvh_free(&bvh);
        }
#endif // USE_BVH

    return result;
}




int freeBVHFile(void * bvhMemoryHandler)
{
#if USE_BVH
    struct BVH_MotionCapture * BVHLoadedFile = (struct BVH_MotionCapture *) bvhMemoryHandler;
    if (  bvh_free(BVHLoadedFile) )
        {
            free(BVHLoadedFile);
            return 1;
        }
#endif // USE_BVH
    return 0;
}


int scaleAllJoints(float scaleRatio)
{
  return  bvh_scaleAllOffsets(
                              &bvhMotion,
                              scaleRatio
                              );
}


int changeHandsScale(float newHandsScale)
{
 #if USE_BVH
    if ( 
          (!bvh_scaleAllJointChildrenOffsets(&bvhMotion,"lhand",newHandsScale)) &&
          (!bvh_scaleAllJointChildrenOffsets(&bvhMotion,"rhand",newHandsScale))
       )
    {
       for (unsigned int jID=0; jID<bvhMotion.jointHierarchySize; jID++)
       {
        fprintf(stderr,"BVH Joint %u => %s \n",jID,bvhMotion.jointHierarchy[jID].jointName);
       }
       fprintf(stderr,"Error scaling head joints..\n");
       exit(0);
       return 0;
    }

    fprintf(stderr,"Hand dimensions have been scaled (newHandsScale=>%0.2f) \n",newHandsScale);
    //exit(0);
    return 1;

 #endif // USE_BVH
  return 0;
}


int changeHeadScale(float newHeadScale)
{
 #if USE_BVH
    if (!bvh_scaleAllJointChildrenOffsets(&bvhMotion,"head",newHeadScale))
    {
       for (unsigned int jID=0; jID<bvhMotion.jointHierarchySize; jID++)
       {
        fprintf(stderr,"BVH Joint %u => %s \n",jID,bvhMotion.jointHierarchy[jID].jointName);
       }
       fprintf(stderr,"Error scaling head joints..\n");
       exit(0);
       return 0;
    }

    fprintf(stderr,"Head dimensions have been scaled (newHeadScale=>%0.2f) \n",newHeadScale);
    //exit(0);
    return 1;

 #endif // USE_BVH
  return 0;
}


int changeFeetDimensions(
                          float hipToKneeLength,
                          float kneeToFootLength
                         )
{

 #if USE_BVH
    int errors = 0;
    if (!bvh_changeJointDimensions(&bvhMotion,"rshin",hipToKneeLength,1.0,1.0) )  { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"lshin",hipToKneeLength,1.0,1.0) )  { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"rfoot",kneeToFootLength,1.0,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"lfoot",kneeToFootLength,1.0,1.0) ) { ++errors; }

    if (errors>0)
    {

       for (unsigned int jID=0; jID<bvhMotion.jointHierarchySize; jID++)
       {
        fprintf(stderr,"BVH Joint %u => %s \n",jID,bvhMotion.jointHierarchy[jID].jointName);
       }

        fprintf(stderr,"Errors (%u) scaling joints..\n",errors);
        exit(0);
        return 0;
    }

    fprintf(stderr,"Feet dimensions have been scaled (hip->knee=>%0.2f) (knee->foot=>%0.2f)\n",hipToKneeLength,kneeToFootLength);
    //exit(0);
    return 1;

 #endif // USE_BVH
  return 0;
}





int getJointDimensions(
                       float * neckLength,
                       float * torsoLength,
                       float * chestWidth,
                       float * shoulderToElbowLength,
                       float * elbowToHandLength,
                       float * waistWidth,
                       float * hipToKneeLength,
                       float * kneeToFootLength,
                       float * shoeLength
                      )
{
 #if USE_BVH
    int success = 0, checks=0;

    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"neck",neckLength,0,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"neck1",neckLength,0,0);
    //--------------------------------------------------------------------------------------
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"abdomen",torsoLength,0,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"chest",torsoLength,0,0);
    //--------------------------------------------------------------------------------------
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"rShldr",0,0,chestWidth);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"lShldr",0,0,chestWidth);
    //--------------------------------------------------------------------------------------
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"rForeArm",0,0,shoulderToElbowLength);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"lForeArm",0,0,shoulderToElbowLength);
    //--------------------------------------------------------------------------------------
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"rHand",0,0,elbowToHandLength);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"lHand",0,0,elbowToHandLength);
    //--------------------------------------------------------------------------------------
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"rButtock",0,0,waistWidth);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"lButtock",0,0,waistWidth);
    //--------------------------------------------------------------------------------------
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"rShin",hipToKneeLength,0,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"lShin",hipToKneeLength,0,0);
    //--------------------------------------------------------------------------------------
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"rFoot",kneeToFootLength,0,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"lFoot",kneeToFootLength,0,0);
    //--------------------------------------------------------------------------------------
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe1-1.R",0,shoeLength,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe2-1.R",0,shoeLength,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe3-1.R",0,shoeLength,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe4-1.R",0,shoeLength,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe5-1.R",0,shoeLength,0);
    //--------------------------------------------------------------------------------------
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe1-1.L",0,shoeLength,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe2-1.L",0,shoeLength,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe3-1.L",0,shoeLength,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe4-1.L",0,shoeLength,0);
    ++checks;    success += bvh_getJointDimensions(&bvhMotion,"toe5-1.L",0,shoeLength,0);
    //--------------------------------------------------------------------------------------
    return (success==checks);
  #else
    return 0;
  #endif
}



int changeJointDimensions(
                          float neckLength,
                          float torsoLength,
                          float chestWidth,
                          float shoulderToElbowLength,
                          float elbowToHandLength,
                          float waistWidth,
                          float hipToKneeLength,
                          float kneeToFootLength,
                          float shoeLength
                         )
{
 #if USE_BVH
    int errors = 0;

    if (!bvh_changeJointDimensions(&bvhMotion,"neck",neckLength,1.0,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"neck1",neckLength,1.0,1.0) ) { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"abdomen",torsoLength,1.0,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"chest",torsoLength,1.0,1.0) ) { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"rShldr",1.0,1.0,chestWidth) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"lShldr",1.0,1.0,chestWidth) ) { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"rForeArm",1.0,1.0,shoulderToElbowLength) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"lForeArm",1.0,1.0,shoulderToElbowLength) ) { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"rHand",1.0,1.0,elbowToHandLength) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"lHand",1.0,1.0,elbowToHandLength) ) { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"rButtock",1.0,1.0,waistWidth) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"lButtock",1.0,1.0,waistWidth) ) { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"rShin",hipToKneeLength,1.0,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"lShin",hipToKneeLength,1.0,1.0) ) { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"rFoot",kneeToFootLength,1.0,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"lFoot",kneeToFootLength,1.0,1.0) ) { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"toe1-1.R",1.0,shoeLength,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"toe2-1.R",1.0,shoeLength,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"toe3-1.R",1.0,shoeLength,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"toe4-1.R",1.0,shoeLength,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"toe5-1.R",1.0,shoeLength,1.0) ) { ++errors; }
    //--------------------------------------------------------------------------------------
    if (!bvh_changeJointDimensions(&bvhMotion,"toe1-1.L",1.0,shoeLength,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"toe2-1.L",1.0,shoeLength,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"toe3-1.L",1.0,shoeLength,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"toe4-1.L",1.0,shoeLength,1.0) ) { ++errors; }
    if (!bvh_changeJointDimensions(&bvhMotion,"toe5-1.L",1.0,shoeLength,1.0) ) { ++errors; }
    //--------------------------------------------------------------------------------------


    if (errors>0)
    {

       for (unsigned int jID=0; jID<bvhMotion.jointHierarchySize; jID++)
       {
        fprintf(stderr,"BVH Joint %u => %s \n",jID,bvhMotion.jointHierarchy[jID].jointName);
       }

        fprintf(stderr,"Errors (%u) scaling joints..\n",errors);
        exit(0);
        return 0;
    }


    return 1;

 #endif // USE_BVH
  return 0;
}



float * mallocVector(std::vector<float> bvhFrame)
{
   if (bvhFrame.size()==0)
   {
       fprintf(stderr,"mallocVector given an empty vector..\n");
       //Empty bvh frame means no vector
       return 0;
   }

    float * newVector = (float*) malloc(sizeof(float) * bvhFrame.size());
    if (newVector!=0)
        {
            for (int i=0; i<bvhFrame.size(); i++)
                {
                    newVector[i]=(float) bvhFrame[i];
                }
        }
    return newVector;
}



int copyVectorToFloatArray(float * arr,std::vector<float> bvhFrame)
{
   if (bvhFrame.size()==0)
   {
       fprintf(stderr,"mallocVector given an empty vector..\n");
       //Empty bvh frame means no vector
       return 0;
   }

    if (arr!=0)
        {
            for (int i=0; i<bvhFrame.size(); i++)
                {
                    arr[i]=(float) bvhFrame[i];
                }
            return 1;
        }
    return 0;
}


unsigned int getBVHNumberOfValuesPerFrame()
{
#if USE_BVH
    return bvhMotion.numberOfValuesPerFrame;
#endif
    return 0;
}

unsigned int getBVHNumberOfJoints()
{
#if USE_BVH
    return bvhMotion.jointHierarchySize;
#endif
    return 0;
}


int getBVHJointOffset(unsigned int currentJoint,float * x,float *y,float *z)
{
#if USE_BVH
   if (currentJoint<bvhMotion.jointHierarchySize)
   {
     *x = bvhMotion.jointHierarchy[currentJoint].offset[0];
     *y = bvhMotion.jointHierarchy[currentJoint].offset[1];
     *z = bvhMotion.jointHierarchy[currentJoint].offset[2];
     return 1;
   }
#endif
    return 0;
}

unsigned int getBVHParentJoint(unsigned int currentJoint)
{
#if USE_BVH
    return bhv_getJointParent(&bvhMotion,currentJoint);
#endif
    return 0;
}


const char * getBVHJointNameFromMotionStructure(struct BVH_MotionCapture * mc,unsigned int currentJoint)
{
#if USE_BVH
    if (mc->jointHierarchySize>currentJoint)
        {
            return mc->jointHierarchy[currentJoint].jointName; //I could use the lowercase version here
        }
#endif
    return 0;
}


const char * getBVHJointName(unsigned int currentJoint)
{
#if USE_BVH
    if (bvhMotion.jointHierarchySize>currentJoint)
        {
            return bvhMotion.jointHierarchy[currentJoint].jointName; //I could use the lowercase version here
        }
#endif
    return 0;
}


int getBVHMotionValueName(unsigned int currentMotionValue,char * target,unsigned int targetLength)
{
#if USE_BVH
    if (bvhMotion.numberOfValuesPerFrame > currentMotionValue)
        {
            return bvh_getMotionChannelName(&bvhMotion,currentMotionValue,target,targetLength);
        }
#endif
    return 0;
}



unsigned int getBVHJointIDFromJointName(const char * jointName)
{
#if USE_BVH
    int i=0;
    for (i=0; i<bvhMotion.jointHierarchySize; i++)
        {
            if ( strcmp(jointName,bvhMotion.jointHierarchy[i].jointName)==0 )
                {
                    return  i;
                }
        }
#endif
    return 0;
}




#if USE_BVH

void bvh2DCopy(
               struct BVH_Transform * out,
               struct skeletonSerialized * in,
               BVHJointID jID,
               unsigned int skeletonSerializedID_2DX,
               unsigned int skeletonSerializedID_2DY,
               float width,
               float height
              )
{
  if (
       (out!=0) &&
       (out->joint!=0)
     )
     {
       if (jID<out->numberOfJointsSpaceAllocated)
       {
        out->joint[jID].pos2D[0] = (float) in->skeletonBody[skeletonSerializedID_2DX].value * width;
        out->joint[jID].pos2D[1] = (float) in->skeletonBody[skeletonSerializedID_2DY].value * height;
        out->joint[jID].pos2DCalculated = ( (out->joint[jID].pos2D[0]!=0.0) || (out->joint[jID].pos2D[1]!=0.0) );
        out->joint[jID].isBehindCamera = !out->joint[jID].pos2DCalculated;
       } else
       {
        fprintf(stderr,"bvh2DCopy: Error accessing joint %u/%u\n",jID,out->numberOfJointsSpaceAllocated);
       }
     } else
     {
        fprintf(stderr,"bvh2DCopy: Error accessing joint transform\n");
     }
}



void bvh2DBetween2PointsCopy(
                       struct BVH_Transform * out,
                       struct skeletonSerialized * in,
                       BVHJointID jID,
                       unsigned int skeletonSerializedID_2DX_A,
                       unsigned int skeletonSerializedID_2DY_A,
                       unsigned int skeletonSerializedID_2DX_B,
                       unsigned int skeletonSerializedID_2DY_B,
                       float width,
                       float height
                    )
{
  if (
       (out!=0) &&
       (out->joint!=0)
     )
     {
       if  (jID<out->numberOfJointsSpaceAllocated)
        {
         out->joint[jID].pos2D[0] = ((in->skeletonBody[skeletonSerializedID_2DX_A].value + in->skeletonBody[skeletonSerializedID_2DX_B].value)/2) * width;
         out->joint[jID].pos2D[1] = ((in->skeletonBody[skeletonSerializedID_2DY_A].value + in->skeletonBody[skeletonSerializedID_2DY_B].value)/2) * height;
         out->joint[jID].pos2DCalculated = ( (out->joint[jID].pos2D[0]!=0.0) || (out->joint[jID].pos2D[1]!=0.0) );
         out->joint[jID].isBehindCamera = !out->joint[jID].pos2DCalculated;
       } else
       {
        fprintf(stderr,"bvh2DBetween2PointsCopy: Error accessing joint %u/%u\n",jID,out->numberOfJointsSpaceAllocated);
       }
     } else
     {
        fprintf(stderr,"bvh2DBetween2PointsCopy: Error accessing joint transform\n");
     }
}



void convertFaceSkeletonSerializedToBVHTransform(
                                                   struct BVH_MotionCapture * bvhMotion,
                                                   struct simpleRenderer * renderer,
                                                   struct BVH_Transform * out,
                                                   struct skeletonSerialized * in
                                                 )
{
  //unsigned int mID;
  float w = renderer->width;
  float h = renderer->height;

  bvh2DCopy(out,in,BVH_MOTION_LHAND,SKELETON_SERIALIZED_2DX_LHAND,SKELETON_SERIALIZED_2DY_LHAND,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_ORBICULARIS03_R   ,SKELETON_SERIALIZED_2DX_HEAD_REYE_2     ,SKELETON_SERIALIZED_2DY_HEAD_REYE_2,w,h);
  bvh2DCopy(out,in,BVH_MOTION_EYE_R             ,SKELETON_SERIALIZED_2DX_HEAD_REYE       ,SKELETON_SERIALIZED_2DY_HEAD_REYE,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ORBICULARIS04_R   ,SKELETON_SERIALIZED_2DX_HEAD_REYE_5     ,SKELETON_SERIALIZED_2DY_HEAD_REYE_5,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_ORBICULARIS03_L   ,SKELETON_SERIALIZED_2DX_HEAD_LEYE_2     ,SKELETON_SERIALIZED_2DY_HEAD_LEYE_2,w,h);
  bvh2DCopy(out,in,BVH_MOTION_EYE_L             ,SKELETON_SERIALIZED_2DX_HEAD_LEYE       ,SKELETON_SERIALIZED_2DY_HEAD_LEYE,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ORBICULARIS04_L   ,SKELETON_SERIALIZED_2DX_HEAD_LEYE_5     ,SKELETON_SERIALIZED_2DY_HEAD_LEYE_5,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_SPECIAL04         ,SKELETON_SERIALIZED_2DX_HEAD_CHIN       ,SKELETON_SERIALIZED_2DY_HEAD_CHIN,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_ORIS03_R          ,SKELETON_SERIALIZED_2DX_HEAD_INMOUTH_1  ,SKELETON_SERIALIZED_2DY_HEAD_INMOUTH_1,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ORIS05            ,SKELETON_SERIALIZED_2DX_HEAD_INMOUTH_2  ,SKELETON_SERIALIZED_2DY_HEAD_INMOUTH_2,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ORIS03_L          ,SKELETON_SERIALIZED_2DX_HEAD_INMOUTH_3  ,SKELETON_SERIALIZED_2DY_HEAD_INMOUTH_3,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_ORIS07_R          ,SKELETON_SERIALIZED_2DX_HEAD_INMOUTH_7  ,SKELETON_SERIALIZED_2DY_HEAD_INMOUTH_7,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ORIS01            ,SKELETON_SERIALIZED_2DX_HEAD_INMOUTH_6  ,SKELETON_SERIALIZED_2DY_HEAD_INMOUTH_6,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ORIS07_L          ,SKELETON_SERIALIZED_2DX_HEAD_INMOUTH_5  ,SKELETON_SERIALIZED_2DY_HEAD_INMOUTH_5,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
}



void convertBodySkeletonSerializedToBVHTransform(
                                                 struct BVH_MotionCapture * bvhMotion,
                                                 struct simpleRenderer * renderer,
                                                 struct BVH_Transform * out,
                                                 struct skeletonSerialized * in
                                                )
{
  //unsigned int mID;
  float w = renderer->width;
  float h = renderer->height;


  //Nose
  bvh2DCopy(out,in,BVH_MOTION_SPECIAL03         ,SKELETON_SERIALIZED_2DX_HEAD             ,SKELETON_SERIALIZED_2DY_HEAD,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------

  //Eyes
  bvh2DCopy(out,in,BVH_MOTION_EYE_L             ,SKELETON_SERIALIZED_2DX_ENDSITE_EYE_L    ,SKELETON_SERIALIZED_2DY_ENDSITE_EYE_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_EYE_R             ,SKELETON_SERIALIZED_2DX_ENDSITE_EYE_R    ,SKELETON_SERIALIZED_2DY_ENDSITE_EYE_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------

  //Ears
  bvh2DCopy(out,in,BVH_MOTION_TEMPORALIS02_L    ,SKELETON_SERIALIZED_2DX_LEAR             ,SKELETON_SERIALIZED_2DY_LEAR,w,h);
  bvh2DCopy(out,in,BVH_MOTION_TEMPORALIS02_R    ,SKELETON_SERIALIZED_2DX_REAR             ,SKELETON_SERIALIZED_2DY_REAR,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------

  bvh2DCopy(out,in,BVH_MOTION_NECK              ,SKELETON_SERIALIZED_2DX_NECK             ,SKELETON_SERIALIZED_2DY_NECK,w,h);
  bvh2DCopy(out,in,BVH_MOTION_HIP               ,SKELETON_SERIALIZED_2DX_HIP              ,SKELETON_SERIALIZED_2DY_HIP,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------

  // No abdomen in 2D input!     ->     mID = BVH_MOTION_ABDOMEN;
  // No chest in 2D input!       ->     mID = BVH_MOTION_CHEST;

  //------------------------------------------------------------------------------------------------
  bvh2DBetween2PointsCopy(out,in,BVH_MOTION_RCOLLAR,  SKELETON_SERIALIZED_2DX_NECK        ,SKELETON_SERIALIZED_2DY_NECK,
                                                      SKELETON_SERIALIZED_2DX_RSHOULDER   ,SKELETON_SERIALIZED_2DY_RSHOULDER,w,h);
  //bvh2DCopy(out,in,BVH_MOTION_RCOLLAR           ,SKELETON_SERIALIZED_2DX_RSHOULDER        ,SKELETON_SERIALIZED_2DY_RSHOULDER,w,h); //<- This is not needed
  bvh2DCopy(out,in,BVH_MOTION_RSHLDR            ,SKELETON_SERIALIZED_2DX_RSHOULDER        ,SKELETON_SERIALIZED_2DY_RSHOULDER,w,h);
  bvh2DCopy(out,in,BVH_MOTION_RFOREARM          ,SKELETON_SERIALIZED_2DX_RELBOW           ,SKELETON_SERIALIZED_2DY_RELBOW,w,h);
  bvh2DCopy(out,in,BVH_MOTION_RHAND             ,SKELETON_SERIALIZED_2DX_RHAND            ,SKELETON_SERIALIZED_2DY_RHAND,w,h);
  //------------------------------------------------------------------------------------------------
  bvh2DBetween2PointsCopy(out,in,BVH_MOTION_LCOLLAR,  SKELETON_SERIALIZED_2DX_NECK        ,SKELETON_SERIALIZED_2DY_NECK,
                                                      SKELETON_SERIALIZED_2DX_LSHOULDER   ,SKELETON_SERIALIZED_2DY_LSHOULDER,w,h);
  //bvh2DCopy(out,in,BVH_MOTION_LCOLLAR           ,SKELETON_SERIALIZED_2DX_LSHOULDER        ,SKELETON_SERIALIZED_2DY_LSHOULDER,w,h); //<- This is not needed
  bvh2DCopy(out,in,BVH_MOTION_LSHLDR            ,SKELETON_SERIALIZED_2DX_LSHOULDER        ,SKELETON_SERIALIZED_2DY_LSHOULDER,w,h);
  bvh2DCopy(out,in,BVH_MOTION_LFOREARM          ,SKELETON_SERIALIZED_2DX_LELBOW           ,SKELETON_SERIALIZED_2DY_LELBOW,w,h);
  bvh2DCopy(out,in,BVH_MOTION_LHAND             ,SKELETON_SERIALIZED_2DX_LHAND            ,SKELETON_SERIALIZED_2DY_LHAND,w,h);
  //------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_RBUTTOCK          ,SKELETON_SERIALIZED_2DX_RHIP             ,SKELETON_SERIALIZED_2DY_RHIP,w,h);
  bvh2DCopy(out,in,BVH_MOTION_RTHIGH            ,SKELETON_SERIALIZED_2DX_RHIP             ,SKELETON_SERIALIZED_2DY_RHIP,w,h);
  bvh2DCopy(out,in,BVH_MOTION_RSHIN             ,SKELETON_SERIALIZED_2DX_RKNEE            ,SKELETON_SERIALIZED_2DY_RKNEE,w,h);
  bvh2DCopy(out,in,BVH_MOTION_RFOOT             ,SKELETON_SERIALIZED_2DX_RFOOT            ,SKELETON_SERIALIZED_2DY_RFOOT,w,h);
  bvh2DCopy(out,in,BVH_MOTION_TOE1_2_R          ,SKELETON_SERIALIZED_2DX_ENDSITE_TOE1_2_R ,SKELETON_SERIALIZED_2DY_ENDSITE_TOE1_2_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_TOE5_3_R          ,SKELETON_SERIALIZED_2DX_ENDSITE_TOE5_3_R ,SKELETON_SERIALIZED_2DY_ENDSITE_TOE5_3_R,w,h);
  //------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_LBUTTOCK          ,SKELETON_SERIALIZED_2DX_LHIP             ,SKELETON_SERIALIZED_2DY_LHIP,w,h);
  bvh2DCopy(out,in,BVH_MOTION_LTHIGH            ,SKELETON_SERIALIZED_2DX_LHIP             ,SKELETON_SERIALIZED_2DY_LHIP,w,h);
  bvh2DCopy(out,in,BVH_MOTION_LSHIN             ,SKELETON_SERIALIZED_2DX_LKNEE            ,SKELETON_SERIALIZED_2DY_LKNEE,w,h);
  bvh2DCopy(out,in,BVH_MOTION_LFOOT             ,SKELETON_SERIALIZED_2DX_LFOOT            ,SKELETON_SERIALIZED_2DY_LFOOT,w,h);
  bvh2DCopy(out,in,BVH_MOTION_TOE1_2_L          ,SKELETON_SERIALIZED_2DX_ENDSITE_TOE1_2_L ,SKELETON_SERIALIZED_2DY_ENDSITE_TOE1_2_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_TOE5_3_L          ,SKELETON_SERIALIZED_2DX_ENDSITE_TOE5_3_L ,SKELETON_SERIALIZED_2DY_ENDSITE_TOE5_3_L,w,h);
  //------------------------------------------------------------------------------------------------
}






void convertLHandSkeletonSerializedToBVHTransform(
                                                   struct BVH_MotionCapture * bvhMotion,
                                                   struct simpleRenderer * renderer,
                                                   struct BVH_Transform * out,
                                                   struct skeletonSerialized * in
                                                 )
{
  //unsigned int mID;
  float w = renderer->width;
  float h = renderer->height;

  bvh2DCopy(out,in,BVH_MOTION_LHAND,SKELETON_SERIALIZED_2DX_LHAND,SKELETON_SERIALIZED_2DY_LHAND,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_METACARPAL1_L,SKELETON_SERIALIZED_2DX_FINGER2_1_L,SKELETON_SERIALIZED_2DY_FINGER2_1_L,w,h);  //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_FINGER2_1_L,SKELETON_SERIALIZED_2DX_FINGER2_1_L,SKELETON_SERIALIZED_2DY_FINGER2_1_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER2_2_L,SKELETON_SERIALIZED_2DX_FINGER2_2_L,SKELETON_SERIALIZED_2DY_FINGER2_2_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER2_3_L,SKELETON_SERIALIZED_2DX_FINGER2_3_L,SKELETON_SERIALIZED_2DY_FINGER2_3_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER2_3_L,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER2_3_L,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER2_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_METACARPAL2_L,SKELETON_SERIALIZED_2DX_FINGER3_1_L,SKELETON_SERIALIZED_2DY_FINGER3_1_L,w,h);  //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_FINGER3_1_L,SKELETON_SERIALIZED_2DX_FINGER3_1_L,SKELETON_SERIALIZED_2DY_FINGER3_1_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER3_2_L,SKELETON_SERIALIZED_2DX_FINGER3_2_L,SKELETON_SERIALIZED_2DY_FINGER3_2_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER3_3_L,SKELETON_SERIALIZED_2DX_FINGER3_3_L,SKELETON_SERIALIZED_2DY_FINGER3_3_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER3_3_L,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER3_3_L,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER3_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_METACARPAL3_L,SKELETON_SERIALIZED_2DX_FINGER4_1_L,SKELETON_SERIALIZED_2DY_FINGER4_1_L,w,h);  //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_FINGER4_1_L,SKELETON_SERIALIZED_2DX_FINGER4_1_L,SKELETON_SERIALIZED_2DY_FINGER4_1_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER4_2_L,SKELETON_SERIALIZED_2DX_FINGER4_2_L,SKELETON_SERIALIZED_2DY_FINGER4_2_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER4_3_L,SKELETON_SERIALIZED_2DX_FINGER4_3_L,SKELETON_SERIALIZED_2DY_FINGER4_3_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER4_3_L,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER4_3_L,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER4_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_METACARPAL4_L,SKELETON_SERIALIZED_2DX_FINGER5_1_L,SKELETON_SERIALIZED_2DY_FINGER5_1_L,w,h);  //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_FINGER5_1_L,SKELETON_SERIALIZED_2DX_FINGER5_1_L,SKELETON_SERIALIZED_2DY_FINGER5_1_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER5_2_L,SKELETON_SERIALIZED_2DX_FINGER5_2_L,SKELETON_SERIALIZED_2DY_FINGER5_2_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER5_3_L,SKELETON_SERIALIZED_2DX_FINGER5_3_L,SKELETON_SERIALIZED_2DY_FINGER5_3_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER5_3_L,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER5_3_L,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER5_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  //bvh2DCopy(out,in,BVH_MOTION_LTHUMBBASE,SKELETON_SERIALIZED_2DX_LTHUMB,SKELETON_SERIALIZED_2DY_LTHUMB,w,h);                 //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_LTHUMB,SKELETON_SERIALIZED_2DX_LTHUMB,SKELETON_SERIALIZED_2DY_LTHUMB,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER1_2_L,SKELETON_SERIALIZED_2DX_FINGER1_2_L,SKELETON_SERIALIZED_2DY_FINGER1_2_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER1_3_L,SKELETON_SERIALIZED_2DX_FINGER1_3_L,SKELETON_SERIALIZED_2DY_FINGER1_3_L,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER1_3_L,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER1_3_L,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER1_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
}



void convertStandaloneLHandSkeletonSerializedToBVHTransform(
                                                            struct BVH_MotionCapture * bvhMotion,
                                                            struct simpleRenderer * renderer,
                                                            struct BVH_Transform * out,
                                                            struct skeletonSerialized * in,
                                                            int skeletonSerializedInputIsAlreadyNormalized
                                                           )
{
  //unsigned int mID;
  float w = renderer->width;
  float h = renderer->height;

  if (skeletonSerializedInputIsAlreadyNormalized)
  {
    w=1.0;
    h=1.0;
  }

  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_LHAND,SS_LHAND_STANDALONE_2DX_LHAND,SS_LHAND_STANDALONE_2DY_LHAND,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_METACARPAL_1_L,SS_LHAND_STANDALONE_2DX_FINGER2_1_L,SS_LHAND_STANDALONE_2DY_FINGER2_1_L,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER2_1_L,SS_LHAND_STANDALONE_2DX_FINGER2_1_L,SS_LHAND_STANDALONE_2DY_FINGER2_1_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER2_2_L,SS_LHAND_STANDALONE_2DX_FINGER2_2_L,SS_LHAND_STANDALONE_2DY_FINGER2_2_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER2_3_L,SS_LHAND_STANDALONE_2DX_FINGER2_3_L,SS_LHAND_STANDALONE_2DY_FINGER2_3_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER2_3_L,SS_LHAND_STANDALONE_2DX_ENDSITE_FINGER2_3_L,SS_LHAND_STANDALONE_2DY_ENDSITE_FINGER2_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_METACARPAL_2_L,SS_LHAND_STANDALONE_2DX_FINGER3_1_L,SS_LHAND_STANDALONE_2DY_FINGER3_1_L,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER3_1_L,SS_LHAND_STANDALONE_2DX_FINGER3_1_L,SS_LHAND_STANDALONE_2DY_FINGER3_1_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER3_2_L,SS_LHAND_STANDALONE_2DX_FINGER3_2_L,SS_LHAND_STANDALONE_2DY_FINGER3_2_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER3_3_L,SS_LHAND_STANDALONE_2DX_FINGER3_3_L,SS_LHAND_STANDALONE_2DY_FINGER3_3_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER3_3_L,SS_LHAND_STANDALONE_2DX_ENDSITE_FINGER3_3_L,SS_LHAND_STANDALONE_2DY_ENDSITE_FINGER3_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND__METACARPAL_3_L,SS_LHAND_STANDALONE_2DX_FINGER4_1_L,SS_LHAND_STANDALONE_2DY_FINGER4_1_L,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_METACARPAL_3_L,SS_LHAND_STANDALONE_2DX_FINGER4_1_L,SS_LHAND_STANDALONE_2DY_FINGER4_1_L,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER4_1_L,SS_LHAND_STANDALONE_2DX_FINGER4_1_L,SS_LHAND_STANDALONE_2DY_FINGER4_1_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER4_2_L,SS_LHAND_STANDALONE_2DX_FINGER4_2_L,SS_LHAND_STANDALONE_2DY_FINGER4_2_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER4_3_L,SS_LHAND_STANDALONE_2DX_FINGER4_3_L,SS_LHAND_STANDALONE_2DY_FINGER4_3_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER4_3_L,SS_LHAND_STANDALONE_2DX_ENDSITE_FINGER4_3_L,SS_LHAND_STANDALONE_2DY_ENDSITE_FINGER4_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND__METACARPAL_4_L,SS_LHAND_STANDALONE_2DX_FINGER5_1_L,SS_LHAND_STANDALONE_2DY_FINGER5_1_L,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_METACARPAL_4_L,SS_LHAND_STANDALONE_2DX_FINGER5_1_L,SS_LHAND_STANDALONE_2DY_FINGER5_1_L,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER5_1_L,SS_LHAND_STANDALONE_2DX_FINGER5_1_L,SS_LHAND_STANDALONE_2DY_FINGER5_1_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER5_2_L,SS_LHAND_STANDALONE_2DX_FINGER5_2_L,SS_LHAND_STANDALONE_2DY_FINGER5_2_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER5_3_L,SS_LHAND_STANDALONE_2DX_FINGER5_3_L,SS_LHAND_STANDALONE_2DY_FINGER5_3_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER5_3_L,SS_LHAND_STANDALONE_2DX_ENDSITE_FINGER5_3_L,SS_LHAND_STANDALONE_2DY_ENDSITE_FINGER5_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  //bvh2DCopy(out,in,BVH_STANDALONE_LHAND_LTHUMBBASE,SS_LHAND_STANDALONE_2DX_LTHUMB,SS_LHAND_STANDALONE_2DY_LTHUMB,w,h);    //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_LTHUMB,SS_LHAND_STANDALONE_2DX_LTHUMB,SS_LHAND_STANDALONE_2DY_LTHUMB,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER1_2_L,SS_LHAND_STANDALONE_2DX_FINGER1_2_L,SS_LHAND_STANDALONE_2DY_FINGER1_2_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_LHAND_FINGER1_3_L,SS_LHAND_STANDALONE_2DX_FINGER1_3_L,SS_LHAND_STANDALONE_2DY_FINGER1_3_L,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER1_3_L,SS_LHAND_STANDALONE_2DX_ENDSITE_FINGER1_3_L,SS_LHAND_STANDALONE_2DY_ENDSITE_FINGER1_3_L,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
}


void convertRHandSkeletonSerializedToBVHTransform(
                                             struct BVH_MotionCapture * bvhMotion,
                                             struct simpleRenderer * renderer,
                                             struct BVH_Transform * out,
                                             struct skeletonSerialized * in
                                           )
{
  //unsigned int mID;
  float w = renderer->width;
  float h = renderer->height;

  bvh2DCopy(out,in,BVH_MOTION_RHAND,SKELETON_SERIALIZED_2DX_RHAND,SKELETON_SERIALIZED_2DY_RHAND,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_METACARPAL1_R,SKELETON_SERIALIZED_2DX_FINGER2_1_R,SKELETON_SERIALIZED_2DY_FINGER2_1_R,w,h);  //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_FINGER2_1_R,SKELETON_SERIALIZED_2DX_FINGER2_1_R,SKELETON_SERIALIZED_2DY_FINGER2_1_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER2_2_R,SKELETON_SERIALIZED_2DX_FINGER2_2_R,SKELETON_SERIALIZED_2DY_FINGER2_2_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER2_3_R,SKELETON_SERIALIZED_2DX_FINGER2_3_R,SKELETON_SERIALIZED_2DY_FINGER2_3_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER2_3_R,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER2_3_R,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER2_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_METACARPAL2_R,SKELETON_SERIALIZED_2DX_FINGER3_1_R,SKELETON_SERIALIZED_2DY_FINGER3_1_R,w,h);  //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_FINGER3_1_R,SKELETON_SERIALIZED_2DX_FINGER3_1_R,SKELETON_SERIALIZED_2DY_FINGER3_1_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER3_2_R,SKELETON_SERIALIZED_2DX_FINGER3_2_R,SKELETON_SERIALIZED_2DY_FINGER3_2_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER3_3_R,SKELETON_SERIALIZED_2DX_FINGER3_3_R,SKELETON_SERIALIZED_2DY_FINGER3_3_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER3_3_R,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER3_3_R,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER3_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_METACARPAL3_R,SKELETON_SERIALIZED_2DX_FINGER4_1_R,SKELETON_SERIALIZED_2DY_FINGER4_1_R,w,h);  //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_FINGER4_1_R,SKELETON_SERIALIZED_2DX_FINGER4_1_R,SKELETON_SERIALIZED_2DY_FINGER4_1_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER4_2_R,SKELETON_SERIALIZED_2DX_FINGER4_2_R,SKELETON_SERIALIZED_2DY_FINGER4_2_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER4_3_R,SKELETON_SERIALIZED_2DX_FINGER4_3_R,SKELETON_SERIALIZED_2DY_FINGER4_3_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER4_3_R,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER4_3_R,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER4_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_MOTION_METACARPAL4_R,SKELETON_SERIALIZED_2DX_FINGER5_1_R,SKELETON_SERIALIZED_2DY_FINGER5_1_R,w,h);  //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_FINGER5_1_R,SKELETON_SERIALIZED_2DX_FINGER5_1_R,SKELETON_SERIALIZED_2DY_FINGER5_1_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER5_2_R,SKELETON_SERIALIZED_2DX_FINGER5_2_R,SKELETON_SERIALIZED_2DY_FINGER5_2_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER5_3_R,SKELETON_SERIALIZED_2DX_FINGER5_3_R,SKELETON_SERIALIZED_2DY_FINGER5_3_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER5_3_R,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER5_3_R,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER5_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  //bvh2DCopy(out,in,BVH_MOTION_RTHUMBBASE,SKELETON_SERIALIZED_2DX_RTHUMB,SKELETON_SERIALIZED_2DY_RTHUMB,w,h);                 //This is not needed
  bvh2DCopy(out,in,BVH_MOTION_RTHUMB,SKELETON_SERIALIZED_2DX_RTHUMB,SKELETON_SERIALIZED_2DY_RTHUMB,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER1_2_R,SKELETON_SERIALIZED_2DX_FINGER1_2_R,SKELETON_SERIALIZED_2DY_FINGER1_2_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_FINGER1_3_R,SKELETON_SERIALIZED_2DX_FINGER1_3_R,SKELETON_SERIALIZED_2DY_FINGER1_3_R,w,h);
  bvh2DCopy(out,in,BVH_MOTION_ENDSITE_FINGER1_3_R,SKELETON_SERIALIZED_2DX_ENDSITE_FINGER1_3_R,SKELETON_SERIALIZED_2DY_ENDSITE_FINGER1_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
}



void convertStandaloneRHandSkeletonSerializedToBVHTransform(
                                                            struct BVH_MotionCapture * bvhMotion,
                                                            struct simpleRenderer * renderer,
                                                            struct BVH_Transform * out,
                                                            struct skeletonSerialized * in,
                                                            int skeletonSerializedInputIsAlreadyNormalized
                                                           )
{
  //unsigned int mID;
  float w = renderer->width;
  float h = renderer->height;

  if (skeletonSerializedInputIsAlreadyNormalized)
  {
    w=1.0;
    h=1.0;
  }

  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_RHAND,SS_RHAND_STANDALONE_2DX_RHAND,SS_RHAND_STANDALONE_2DY_RHAND,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_METACARPAL_1_R,SS_RHAND_STANDALONE_2DX_FINGER2_1_R,SS_RHAND_STANDALONE_2DY_FINGER2_1_R,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER2_1_R,SS_RHAND_STANDALONE_2DX_FINGER2_1_R,SS_RHAND_STANDALONE_2DY_FINGER2_1_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER2_2_R,SS_RHAND_STANDALONE_2DX_FINGER2_2_R,SS_RHAND_STANDALONE_2DY_FINGER2_2_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER2_3_R,SS_RHAND_STANDALONE_2DX_FINGER2_3_R,SS_RHAND_STANDALONE_2DY_FINGER2_3_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER2_3_R,SS_RHAND_STANDALONE_2DX_ENDSITE_FINGER2_3_R,SS_RHAND_STANDALONE_2DY_ENDSITE_FINGER2_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_METACARPAL_2_R,SS_RHAND_STANDALONE_2DX_FINGER3_1_R,SS_RHAND_STANDALONE_2DY_FINGER3_1_R,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER3_1_R,SS_RHAND_STANDALONE_2DX_FINGER3_1_R,SS_RHAND_STANDALONE_2DY_FINGER3_1_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER3_2_R,SS_RHAND_STANDALONE_2DX_FINGER3_2_R,SS_RHAND_STANDALONE_2DY_FINGER3_2_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER3_3_R,SS_RHAND_STANDALONE_2DX_FINGER3_3_R,SS_RHAND_STANDALONE_2DY_FINGER3_3_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER3_3_R,SS_RHAND_STANDALONE_2DX_ENDSITE_FINGER3_3_R,SS_RHAND_STANDALONE_2DY_ENDSITE_FINGER3_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND__METACARPAL_3_R,SS_RHAND_STANDALONE_2DX_FINGER4_1_R,SS_RHAND_STANDALONE_2DY_FINGER4_1_R,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_METACARPAL_3_R,SS_RHAND_STANDALONE_2DX_FINGER4_1_R,SS_RHAND_STANDALONE_2DY_FINGER4_1_R,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER4_1_R,SS_RHAND_STANDALONE_2DX_FINGER4_1_R,SS_RHAND_STANDALONE_2DY_FINGER4_1_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER4_2_R,SS_RHAND_STANDALONE_2DX_FINGER4_2_R,SS_RHAND_STANDALONE_2DY_FINGER4_2_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER4_3_R,SS_RHAND_STANDALONE_2DX_FINGER4_3_R,SS_RHAND_STANDALONE_2DY_FINGER4_3_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER4_3_R,SS_RHAND_STANDALONE_2DX_ENDSITE_FINGER4_3_R,SS_RHAND_STANDALONE_2DY_ENDSITE_FINGER4_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND__METACARPAL_4_R,SS_RHAND_STANDALONE_2DX_FINGER5_1_R,SS_RHAND_STANDALONE_2DY_FINGER5_1_R,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_METACARPAL_4_R,SS_RHAND_STANDALONE_2DX_FINGER5_1_R,SS_RHAND_STANDALONE_2DY_FINGER5_1_R,w,h);  //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER5_1_R,SS_RHAND_STANDALONE_2DX_FINGER5_1_R,SS_RHAND_STANDALONE_2DY_FINGER5_1_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER5_2_R,SS_RHAND_STANDALONE_2DX_FINGER5_2_R,SS_RHAND_STANDALONE_2DY_FINGER5_2_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER5_3_R,SS_RHAND_STANDALONE_2DX_FINGER5_3_R,SS_RHAND_STANDALONE_2DY_FINGER5_3_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER5_3_R,SS_RHAND_STANDALONE_2DX_ENDSITE_FINGER5_3_R,SS_RHAND_STANDALONE_2DY_ENDSITE_FINGER5_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
  //bvh2DCopy(out,in,BVH_STANDALONE_RHAND_RTHUMBBASE,SS_RHAND_STANDALONE_2DX_RTHUMB,SS_RHAND_STANDALONE_2DY_RTHUMB,w,h);    //This is not needed, just added so that IK target.svg looks better..
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_RTHUMB,SS_RHAND_STANDALONE_2DX_RTHUMB,SS_RHAND_STANDALONE_2DY_RTHUMB,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER1_2_R,SS_RHAND_STANDALONE_2DX_FINGER1_2_R,SS_RHAND_STANDALONE_2DY_FINGER1_2_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_RHAND_FINGER1_3_R,SS_RHAND_STANDALONE_2DX_FINGER1_3_R,SS_RHAND_STANDALONE_2DY_FINGER1_3_R,w,h);
  bvh2DCopy(out,in,BVH_STANDALONE_ENDSITE_FINGER1_3_R,SS_RHAND_STANDALONE_2DX_ENDSITE_FINGER1_3_R,SS_RHAND_STANDALONE_2DY_ENDSITE_FINGER1_3_R,w,h);
  //------------------------------------------------------------------------------------------------------------------------------------
}




int convertSkeletonSerializedToBVHTransform(
                                             struct BVH_MotionCapture * bvhMotion,
                                             struct simpleRenderer * renderer,
                                             struct BVH_Transform * out,
                                             struct skeletonSerialized * in
                                           )
{
  // dumpSkeletonSerializedToBVHTransformCode(bvhMotion,out,in);

   if (bvhMotion==0) { return 0; }
   if (bvhMotion->jointHierarchySize==0) { return 0; }
   if (out==0) { return 0; }
   if (in==0)  { return 0; }
   if (in->skeletonBody==0)  { return 0; }

  unsigned int w = renderer->width;
  unsigned int h = renderer->height;


  if ( bvh_allocateTransform(bvhMotion,out) )
  {
   //Make sure the structure is clean..
   for (BVHJointID jID=0;  jID<bvhMotion->jointHierarchySize; jID++ )
   {
      //bvhMotion->numberOfValuesPerFrame <- this should not be mID but jID
     out->joint[jID].pos2D[0] = 0.0;
     out->joint[jID].pos2D[1] = 0.0;
     out->joint[jID].pos2DCalculated = 1;
     out->joint[jID].isBehindCamera  = 0;
   }

   //------------------------------------------------------------------------------------------------
    convertBodySkeletonSerializedToBVHTransform(
                                                bvhMotion,
                                                renderer,
                                                out,
                                                in
                                               );
  //------------------------------------------------------------------------------------------------
   convertLHandSkeletonSerializedToBVHTransform(
                                                bvhMotion,
                                                renderer,
                                                out,
                                                in
                                                );
  //------------------------------------------------------------------------------------------------
   convertRHandSkeletonSerializedToBVHTransform(
                                                bvhMotion,
                                                renderer,
                                                out,
                                                in
                                                );
  //------------------------------------------------------------------------------------------------
   convertFaceSkeletonSerializedToBVHTransform(
                                                bvhMotion,
                                                renderer,
                                                out,
                                                in
                                                );
  //------------------------------------------------------------------------------------------------

 return 1;

  }
  return 0;
}
#endif


int forceLimits(std::vector<float> & bvhFrame)
{

//Minima/Maxima :

float minimumLimits[1024]={0};
float maximumLimits[1024]={0};
/*
//Ignore X,Y,Z Pos limits and X,Y,Z Rot limits
minimumLimits[0]=-391.23;
maximumLimits[0]=410.96;
minimumLimits[1]=-119.81;
maximumLimits[1]=209.66;
minimumLimits[2]=-293.22;
maximumLimits[2]=355.36;
minimumLimits[3]=-2893.85;
maximumLimits[3]=3595.49;
minimumLimits[4]=-1345.44;
maximumLimits[4]=1527.27;
minimumLimits[5]=-3095.27;
maximumLimits[5]=6833.06;
minimumLimits[6]=-179.80;
maximumLimits[6]=177.94;*/
minimumLimits[7]=-85.22;
maximumLimits[7]=89.06;
minimumLimits[8]=-177.62;
maximumLimits[8]=178.93;
minimumLimits[9]=-62.75;
maximumLimits[9]=59.55;
minimumLimits[10]=-59.95;
maximumLimits[10]=38.22;
minimumLimits[11]=-36.58;
maximumLimits[11]=35.48;
minimumLimits[12]=-79.55;
maximumLimits[12]=97.18;
minimumLimits[13]=-63.63;
maximumLimits[13]=81.78;
minimumLimits[14]=-63.31;
maximumLimits[14]=71.73;
minimumLimits[15]=-0.01;
maximumLimits[15]=0.01;
minimumLimits[16]=-0.01;
maximumLimits[16]=0.01;
minimumLimits[17]=-0.01;
maximumLimits[17]=0.01;
minimumLimits[18]=-180.00;
maximumLimits[18]=180.00;
minimumLimits[19]=-89.61;
maximumLimits[19]=86.77;
minimumLimits[20]=-179.98;
maximumLimits[20]=179.74;
minimumLimits[237]=-180.00;
maximumLimits[237]=180.00;
minimumLimits[238]=-89.62;
maximumLimits[238]=89.97;
minimumLimits[239]=-179.98;
maximumLimits[239]=179.97;
minimumLimits[240]=-43.35;
maximumLimits[240]=28.43;
minimumLimits[241]=-60.75;
maximumLimits[241]=16.89;
minimumLimits[242]=-102.92;
maximumLimits[242]=157.57;
//minimumLimits[MOCAPNET_OUTPUT_RHAND_ZROTATION]=-170.93;//243
//maximumLimits[MOCAPNET_OUTPUT_RHAND_ZROTATION]=7.43;//243
//minimumLimits[MOCAPNET_OUTPUT_RHAND_XROTATION]=-89.43;//244
//maximumLimits[MOCAPNET_OUTPUT_RHAND_XROTATION]=86.24;//244
//minimumLimits[MOCAPNET_OUTPUT_RHAND_YROTATION]=-180.00;//245
//maximumLimits[MOCAPNET_OUTPUT_RHAND_YROTATION]=180.00;//245
minimumLimits[315]=-180.00;
maximumLimits[315]=180.00;
minimumLimits[316]=-89.60;
maximumLimits[316]=89.88;
minimumLimits[317]=-180.00;
maximumLimits[317]=180.00;
minimumLimits[318]=-9.30;
maximumLimits[318]=42.50;
minimumLimits[319]=-60.74;
maximumLimits[319]=6.88;
minimumLimits[320]=-157.46;
maximumLimits[320]=8.76;
//minimumLimits[MOCAPNET_OUTPUT_LHAND_ZROTATION]=-7.17;//321
//maximumLimits[MOCAPNET_OUTPUT_LHAND_ZROTATION]=172.41;//321
//minimumLimits[MOCAPNET_OUTPUT_LHAND_XROTATION]=-88.13;//322
//maximumLimits[MOCAPNET_OUTPUT_LHAND_XROTATION]=88.61;//322
//minimumLimits[MOCAPNET_OUTPUT_LHAND_YROTATION]=-180.00;//323
//maximumLimits[MOCAPNET_OUTPUT_LHAND_YROTATION]=180.00;//323
minimumLimits[393]=-180.00;
maximumLimits[393]=179.99;
minimumLimits[394]=-89.97;
maximumLimits[394]=77.29;
minimumLimits[395]=-180.00;
maximumLimits[395]=180.00;
minimumLimits[396]=-180.00;
maximumLimits[396]=179.99;
minimumLimits[397]=-65.47;
maximumLimits[397]=89.86;
minimumLimits[398]=-180.00;
maximumLimits[398]=180.00;
minimumLimits[399]=-180.00;
maximumLimits[399]=180.00;
minimumLimits[400]=-88.69;
maximumLimits[400]=86.79;
minimumLimits[401]=-179.97;
maximumLimits[401]=179.99;
minimumLimits[447]=-180.00;
maximumLimits[447]=180.00;
minimumLimits[448]=-89.92;
maximumLimits[448]=85.35;
minimumLimits[449]=-180.00;
maximumLimits[449]=180.00;
minimumLimits[450]=-180.00;
maximumLimits[450]=180.00;
minimumLimits[451]=-45.83;
maximumLimits[451]=89.88;
minimumLimits[452]=-180.00;
maximumLimits[452]=180.00;
minimumLimits[453]=-180.00;
maximumLimits[453]=180.00;
minimumLimits[454]=-88.95;
maximumLimits[454]=88.66;
minimumLimits[455]=-179.97;
maximumLimits[455]=179.98;

unsigned int violations=0;
unsigned int mLim = 456;
if (mLim>bvhFrame.size()) {mLim=bvhFrame.size(); } //std::min() :P

char motionValueLabel[512]={0};

for (unsigned int i=0; i<mLim; i++)
{
    if ( (minimumLimits[i]!=0.0) && (maximumLimits[i]!=0.0) )
    {
      if (bvhFrame[i]<minimumLimits[i])
          {
            getBVHMotionValueName(i,motionValueLabel,512);
            fprintf(stderr,YELLOW "Hit Minimum @ %s(%u) ( limit %0.2f, value %0.2f ) ..\n" NORMAL,motionValueLabel,i,minimumLimits[i],bvhFrame[i]);
            bvhFrame[i]=minimumLimits[i];
            ++violations;
          }
    }

    if ( (minimumLimits[i]!=0.0) && (maximumLimits[i]!=0.0) )
    {
      if (bvhFrame[i]>maximumLimits[i])
          {
            getBVHMotionValueName(i,motionValueLabel,512);
            fprintf(stderr,YELLOW "Hit Maximum @ %s(%u) ( limit %0.2f, value %0.2f ) ..\n" NORMAL,motionValueLabel,i,maximumLimits[i],bvhFrame[i]);
            bvhFrame[i]=maximumLimits[i];
            ++violations;
          }
    }
}

 return violations;
}



int transferVectorToMotionBufferArray(struct MotionBuffer * mb,const std::vector<float> motionVector)
{
 if (motionVector.size()!=0)
 {
    if ((mb->motion!=0) && (motionVector.size()==mb->bufferSize))
      {
        //We already have the correct MotionBuffer allocated for our vector so just do a copy
        copyVectorToFloatArray(mb->motion,motionVector);
        mb->bufferSize = motionVector.size();
        return 1;
      } else
      {
        //We either not have a buffer or it doesn't have the correct size..

        //First free previous MotionBuffer
        if (mb->motion!=0) { free(mb->motion); mb->motion=0;  mb->bufferSize=0;  }

        //Then malloc for the specific vector
        mb->motion = mallocVector(motionVector);
        if (mb->motion!=0)
           { mb->bufferSize = motionVector.size(); }
        return 1;
      }
 }
 return 0;
}



std::vector<float> improveBVHFrameUsingInverseKinematics(
                                                          const std::vector<float> bvhPenultimateFrame,
                                                          const std::vector<float> bvhPreviousFrame,
                                                          const std::vector<float> bvhFrameInput,
                                                          //--------------
                                                          unsigned int frameNumber,
                                                          //--------------
                                                          int doLeftHand,
                                                          int doRightHand,
                                                          int doFace,
                                                          //--------------
                                                          struct skeletonSerialized * inputPoints2D,
                                                          //--------------
                                                          float learningRate,
                                                          unsigned int iterations,
                                                          unsigned int epochs,
                                                          float spring,
                                                          unsigned int springIgnoreChanges,
                                                          int dontUseHistory,
                                                          int multiThreading
                                                        )
{
 #if USE_BVH
 //fprintf(stderr,"improveBVHFrameUsingInverseKinematics\n");

 std::vector<float> bvhFrame = bvhFrameInput;

 #if APPLY_BVH_FIX_TO_IK_INPUT
  //Flip HIP X/Y/Z rotations here as well ?
  if (bvhFrame.size()>=6)
  {
   bvhFrame[3]=-1* bvhFrame[3];
   bvhFrame[4]=-1* bvhFrame[4];
   bvhFrame[5]=-1* bvhFrame[5];
  }
 #endif


 if (bvhMotion.numberOfValuesPerFrame!= bvhFrame.size())
 {
     fprintf(stderr,"improveBVHFrameUsingInverseKinematics: Mismatch in bvh frame received and bvh frame loaded..\n");
     fprintf(stderr,"BVH frame received had %lu elements..\n",bvhFrame.size());
     fprintf(stderr,"BVH file had %u elements..\n",bvhMotion.numberOfValuesPerFrame);
     return bvhFrame;
 }

  if (bvhFrame.size()==0)
 {
     fprintf(stderr,"improveBVHFrameUsingInverseKinematics: Can't improve empty frame using IK \n");
     return bvhFrame;
 }

 //Statistics on how well did we do..
 float initialMAEInPixels=0.0;
 float finalMAEInPixels=0.0;
 float initialMAEInMM=0.0;
 float finalMAEInMM=0.0;


  /*
 // ------------------------------------------------------
 // ------------------------------------------------------
 // ------------------------------------------------------
 if (bvhFrame.size()!=0)
 {
   solution.motion = mallocVector(bvhFrame);
   if (solution.motion!=0)
         { solution.bufferSize = bvhFrame.size(); }
 }
 // ------------------------------------------------------
 if (bvhPreviousFrame.size()!=0)
 {
   previousSolution.motion = mallocVector(bvhPreviousFrame);
   if (previousSolution.motion!=0)
         { previousSolution.bufferSize = bvhPreviousFrame.size(); }
 }
 // ------------------------------------------------------
 if (bvhPenultimateFrame.size()!=0)
 {
   penultimateSolution.motion = mallocVector(bvhPenultimateFrame);
   if (penultimateSolution.motion!=0)
         { penultimateSolution.bufferSize = bvhPenultimateFrame.size(); }
 }
 // ------------------------------------------------------
 // ------------------------------------------------------
 // ------------------------------------------------------
*/

if (
      (!transferVectorToMotionBufferArray(&solution,bvhFrame)) ||
      (!transferVectorToMotionBufferArray(&previousSolution,bvhPreviousFrame)) ||
      (!transferVectorToMotionBufferArray(&penultimateSolution,bvhPenultimateFrame))
   )
   {
       fprintf(stderr,"Failed transfering vectors to MotionBuffers..\n");
   }


 if ( (solution.motion!=0) /*&& (previousSolution.motion!=0) && (penultimateSolution.motion!=0)*/ )
 {
   std::vector<float> result = bvhFrame;
   struct BVH_Transform bvhTargetTransform={0}; //TODO : convert skeletonserialized to this..

   if (
        convertSkeletonSerializedToBVHTransform(
                                                &bvhMotion,
                                                &renderer,
                                                &bvhTargetTransform,
                                                inputPoints2D
                                               )
      )
      {
         //------------------------------------
         ikConfig.learningRate = learningRate;
         ikConfig.iterations = iterations;
         ikConfig.epochs = epochs;
         ikConfig.maximumAcceptableStartingLoss= 30000;//12000; //WARING < -  consider setting this to 0
         ikConfig.gradientExplosionThreshold = 50;
         ikConfig.spring=spring;
         ikConfig.dumpScreenshots = 0; // Dont thrash disk
         ikConfig.verbose = 0; //Dont spam console
         ikConfig.tryMaintainingLocalOptima=1; //Less Jittery but can be stuck at local optima
         ikConfig.dontUseSolutionHistory=dontUseHistory;
         ikConfig.ikVersion = IK_VERSION;
         //------------------------------------


         //ikConfig.dumpScreenshots=1;

         //======================================================================================================
         //======================================================================================================
         //======================================================================================================
         if (  approximateBodyFromMotionBufferUsingInverseKinematics(
                                                                     &bvhMotion,
                                                                     &renderer,
                                                                     bodyProblem,
                                                                     &ikConfig,
                                                                     //----------------
                                                                     &penultimateSolution,
                                                                     &previousSolution,
                                                                     &solution,
                                                                     0, //No ground truth..
                                                                     //----------------
                                                                     &bvhTargetTransform,
                                                                     //----------------
                                                                     multiThreading,// 0=single thread, 1=multi thread
                                                                     //----------------
                                                                     &initialMAEInPixels,
                                                                     &finalMAEInPixels,
                                                                     &initialMAEInMM,
                                                                     &finalMAEInMM
                                                                    )
            )
            {
              //fprintf(stderr,"Finished IK using LR=%0.2f/Epochs=%u/Iterations=%u fx=%0.2f/fy=%0.2f..\n",learningRate,epochs,iterations,fX,fY);

              //If we performed inverse kinematics, then copy the output..
              unsigned int elements=result.size();
              if (elements>solution.bufferSize)
                   { elements = solution.bufferSize; } //Take care

              for (unsigned int i=0; i<elements; i++)
               {
                 result[i] = solution.motion[i];
               }

               /*
               unsigned int limitViolations = forceLimits(result);
               if ( limitViolations > 0 )
               {
                  fprintf(stderr,YELLOW "Solution has %u crossed limits, we corrected it..\n" NORMAL,limitViolations);
               } */

            } else
            {
              fprintf(stderr,RED "Unable to perform Inverse Kinematics for body..\n" NORMAL);
              doLeftHand=0;
              doRightHand=0;
              doFace=0;
            }
         //======================================================================================================
         //======================================================================================================
         //======================================================================================================

         /*
         if (result[2]>4000)
         {
             fprintf(stderr,"We are too far to do hands..?\n");
             doLeftHand=0;
             doRightHand=0;
         }*/


         if (doFace)
         {
             //Not implemented..
         }

         //======================================================================================================
         //======================================================================================================
         //======================================================================================================
         if (doLeftHand)
         {
         if (  approximateBodyFromMotionBufferUsingInverseKinematics(
                                                                     &bvhMotion,
                                                                     &renderer,
                                                                     leftHandProblem,
                                                                     &ikConfig,
                                                                     //----------------
                                                                     &penultimateSolution,
                                                                     &previousSolution,
                                                                     &solution,
                                                                     0, //No ground truth..
                                                                     //----------------
                                                                     &bvhTargetTransform,
                                                                     //----------------
                                                                     multiThreading,// 0=single thread, 1=multi thread
                                                                     //----------------
                                                                     &initialMAEInPixels,
                                                                     &finalMAEInPixels,
                                                                     &initialMAEInMM,
                                                                     &finalMAEInMM
                                                                    )
            )
            {
              //fprintf(stderr,"Finished IK using LR=%0.2f/Epochs=%u/Iterations=%u fx=%0.2f/fy=%0.2f..\n",learningRate,epochs,iterations,fX,fY);

              unsigned int elements=result.size();
              if (elements>solution.bufferSize)
                    { elements = solution.bufferSize; } //Take care

              //If we performed inverse kinematics, then copy the output..
              for (unsigned int i=0; i<elements; i++)
               {
                 result[i] = solution.motion[i];
               }

            } else
            { fprintf(stderr,RED "Unable to perform Inverse Kinematics for left hand..\n" NORMAL); }
         }
         //======================================================================================================
         //======================================================================================================
         //======================================================================================================




         //======================================================================================================
         //======================================================================================================
         //======================================================================================================
         if (doRightHand)
         {
         if (  approximateBodyFromMotionBufferUsingInverseKinematics(
                                                                     &bvhMotion,
                                                                     &renderer,
                                                                     rightHandProblem,
                                                                     &ikConfig,
                                                                     //----------------
                                                                     &penultimateSolution,
                                                                     &previousSolution,
                                                                     &solution,
                                                                     0, //No ground truth..
                                                                     //----------------
                                                                     &bvhTargetTransform,
                                                                     //----------------
                                                                     multiThreading,// 0=single thread, 1=multi thread
                                                                     //----------------
                                                                     &initialMAEInPixels,
                                                                     &finalMAEInPixels,
                                                                     &initialMAEInMM,
                                                                     &finalMAEInMM
                                                                    )
            )
            {
              //fprintf(stderr,"Finished IK using LR=%0.2f/Epochs=%u/Iterations=%u fx=%0.2f/fy=%0.2f..\n",learningRate,epochs,iterations,fX,fY);

              unsigned int elements=result.size();
              if (elements>solution.bufferSize)
                    { elements = solution.bufferSize; } //Take care

              //If we performed inverse kinematics, then copy the output..
              for (unsigned int i=0; i<elements; i++)
               {
                 result[i] = solution.motion[i];
               }

            } else
            { fprintf(stderr,RED "Unable to perform Inverse Kinematics for right hand..\n" NORMAL); }
         }
         //======================================================================================================
         //======================================================================================================
         //======================================================================================================







         //======================================================================================================
         //======================================================================================================
         //======================================================================================================
         if (doFace)
         {
         if (  approximateBodyFromMotionBufferUsingInverseKinematics(
                                                                     &bvhMotion,
                                                                     &renderer,
                                                                     faceProblem,
                                                                     &ikConfig,
                                                                     //----------------
                                                                     &penultimateSolution,
                                                                     &previousSolution,
                                                                     &solution,
                                                                     0, //No ground truth..
                                                                     //----------------
                                                                     &bvhTargetTransform,
                                                                     //----------------
                                                                     multiThreading,// 0=single thread, 1=multi thread
                                                                     //----------------
                                                                     &initialMAEInPixels,
                                                                     &finalMAEInPixels,
                                                                     &initialMAEInMM,
                                                                     &finalMAEInMM
                                                                    )
            )
            {
              //fprintf(stderr,"Finished IK using LR=%0.2f/Epochs=%u/Iterations=%u fx=%0.2f/fy=%0.2f..\n",learningRate,epochs,iterations,fX,fY);

              unsigned int elements=result.size();
              if (elements>solution.bufferSize)
                    { elements = solution.bufferSize; } //Take care

              //If we performed inverse kinematics, then copy the output..
              for (unsigned int i=0; i<elements; i++)
               {
                 result[i] = solution.motion[i];
               }

            } else
            { fprintf(stderr,RED "Unable to perform Inverse Kinematics for face..\n" NORMAL); }
         }
         //======================================================================================================
         //======================================================================================================
         //======================================================================================================
      } else
      {
        fprintf(stderr,"Unable to convert skeleton serialized to a BVH_Transform\n");
      }



    //fprintf(stderr,"Reached bvh_freeTransform\n");
     bvh_freeTransform(&bvhTargetTransform);
    //fprintf(stderr,"Survived bvh_freeTransform\n");

     if ( (ikConfig.dumpScreenshots) && (frameNumber==10) )
     {
         fprintf(stderr,RED "DEBUG CODE ON : Stopping.. to dump screenshots\n" NORMAL);
         exit(0);
     }

    return result;
 } else
 {
   fprintf(stderr,YELLOW "Will not perform IK without current,previous and penultimate solutions\n" NORMAL);
   fprintf(stderr,YELLOW "Current Solution : %u \n" NORMAL,(solution.motion!=0));
   fprintf(stderr,YELLOW "Previous Solution : %u \n" NORMAL,(previousSolution.motion!=0));
   fprintf(stderr,YELLOW "Penultimate Solution : %u \n" NORMAL,(penultimateSolution.motion!=0));
 }

 #else
  fprintf(stderr,"BVH code not compiled in, unable to do inverse kinematics..\n");
 #endif

 return bvhFrame;
}


#if USE_BVH
std::vector<std::vector<float> > convertBVHFrameTo2DPointsFromMotion(struct BVH_MotionCapture *bvhMotion,const std::vector<float> bvhFrame)
{
    unsigned int width = renderer.width;
    unsigned int height = renderer.height;

    std::vector<std::vector<float> > result2DPointVector;
    result2DPointVector.clear();


    if ( (width>10000) || (height>10000) )
    {
      fprintf(stderr,"convertBVHFrameTo2DPoints: Cannot use this crazy resolution %u x %u \n",width,height);
      return result2DPointVector;
    }




    if (!haveBVHInit)
        {
           fprintf(stderr,"convertBVHFrameTo2DPoints: Cannot work without a renderer initialization\n");
           return result2DPointVector;
        }


    if (haveBVHInit)
        {
            if (bvhFrame.size() != bvhMotion->numberOfValuesPerFrame)
            {
              fprintf(stderr,"convertBVHFrameTo2DPoints was given an inconsistent number of values to convert ( expected %u, got %lu )\n",bvhMotion->numberOfValuesPerFrame,bvhFrame.size());
              return result2DPointVector;
            }

            float * motionBuffer= mallocVector(bvhFrame);

            if (motionBuffer!=0)
                {
                    bvh_cleanTransform(bvhMotion,&bvhTransform);

                    if ( (width!=renderer.width) || (height!=renderer.height) )
                    {
                      fprintf(stderr,"Resolution changed from %0.2f x %0.2f to %u x %u.. focal length should also be changed..",renderer.width,renderer.height,width,height);

                      // From BVH GroundTruthGenerator settings..
                      float renderingConfigurationfX=fX;
                      float renderingConfigurationfY=fY;

                      if (
                            (width==1920) && (height==1080)
                         )
                         {
                                fprintf(stderr,"Emulating Ground Truth Generator used while training..\n");
                                renderingConfigurationfX=582.18394;
                                renderingConfigurationfY=582.52915;
                         }

                      simpleRendererDefaults(
                                              &renderer,
                                              width,//1920
                                              height,//1080
                                              renderingConfigurationfX, //570.0
                                              renderingConfigurationfY  //570.0
                                             );
                      simpleRendererInitialize(&renderer);
                    }

                    if (
                           bvh_loadTransformForMotionBuffer(
                                                            bvhMotion,
                                                            motionBuffer,
                                                            &bvhTransform,
                                                            0//Dont need extra information
                                                           )
                        )
                        {
                            //-----------------
                            if (
                                 (
                                   bvh_projectTo2D(
                                                   bvhMotion,
                                                   &bvhTransform,
                                                   &renderer,
                                                   0,
                                                   0
                                                  )
                                  ) &&
                                  (bvhTransform.joint!=0) &&
                                  (bvhMotion->jointHierarchySize<=bvhTransform.numberOfJointsSpaceAllocated )
                               )
                                { //-----------------
                                    std::vector<float> point;
                                    //-----------------------
                                    for (unsigned int jID=0; jID<bvhMotion->jointHierarchySize; jID++)
                                        {
                                            point.clear();
                                            point.push_back((float) bvhTransform.joint[jID].pos2D[0]);
                                            point.push_back((float) bvhTransform.joint[jID].pos2D[1]);
                                            result2DPointVector.push_back(point);
                                        }
                                    //-----------------------
                                } //-----------------
                        } //-----------------
                    else
                        {
                            fprintf(stderr,"bvh_loadTransformForMotionBuffer failed..\n");
                        }
                    free(motionBuffer);
                }
            else
                {
                    fprintf(stderr,"Could not allocate enough memory..\n");
                }
        }
    else
        {
            fprintf(stderr,"Could not initialize BVH subsystem..\n");
        }
    return result2DPointVector;
}
#endif // USE_BVH



std::vector<std::vector<float> > convertBVHFrameTo2DPoints(const std::vector<float> bvhFrame)
{
#if USE_BVH
  return convertBVHFrameTo2DPointsFromMotion(&bvhMotion,bvhFrame);
#else
    std::vector<std::vector<float> > empty;
    return empty;
#endif // USE_BVH
}

std::vector<std::vector<float> > convertStandaloneLHandBVHFrameTo2DPoints(const std::vector<float> bvhFrame)
{
#if USE_BVH
  return convertBVHFrameTo2DPointsFromMotion(&standaloneLHand,bvhFrame);
#else
    std::vector<std::vector<float> > empty;
    return empty;
#endif // USE_BVH
}


std::vector<std::vector<float> > convertStandaloneRHandBVHFrameTo2DPoints(const std::vector<float> bvhFrame)
{
#if USE_BVH
  return convertBVHFrameTo2DPointsFromMotion(&standaloneRHand,bvhFrame);
  #else
    std::vector<std::vector<float> > empty;
    return empty;
#endif // USE_BVH
}




std::vector<float>  convertBVHFrameToFlat3DPointsFromMotion(struct BVH_MotionCapture *bvhMotion,std::vector<float> bvhFrame)
{
std::vector<float>  result;
#if USE_BVH

    if (!haveBVHInit)
        {
           fprintf(stderr,"convertBVHFrameToFlat3DPoints: Cannot work without a renderer initialization\n");
           return result;
        }


    if (haveBVHInit)
        {
            if (bvhFrame.size() != bvhMotion->numberOfValuesPerFrame)
            {
              fprintf(stderr,"convertBVHFrameToFlat3DPoints was given an inconsistent number of values to convert ( expected %u, got %lu )\n",bvhMotion->numberOfValuesPerFrame,bvhFrame.size());
              return result;
            }

            float * motionBuffer= mallocVector(bvhFrame);

            if (motionBuffer!=0)
                {
                    if (
                           bvh_loadTransformForMotionBuffer(
                                                            bvhMotion,
                                                            motionBuffer,
                                                            &bvhTransform,
                                                            1//We of course need the extra information for occlusions
                                                           )
                        )
                        {
                            //-----------------
                            if (
                                (
                                   bvh_projectTo2D(
                                                   bvhMotion,
                                                   &bvhTransform,
                                                   &renderer,
                                                   0,
                                                   0
                                                  )
                                ) &&
                                  (bvhTransform.joint!=0) &&
                                  (bvhMotion->jointHierarchySize<=bvhTransform.numberOfJointsSpaceAllocated )
                                )
                                {
                                    //-----------------
                                    for (unsigned int jID=0; jID<bvhMotion->jointHierarchySize; jID++)
                                        {
                                            result.push_back((float) bvhTransform.joint[jID].pos3D[0]);
                                            result.push_back((float) bvhTransform.joint[jID].pos3D[1]);
                                            result.push_back((float) bvhTransform.joint[jID].pos3D[2]);
                                        }
                                } //-----------------
                        } //-----------------
                    else
                        {
                            fprintf(stderr,"bvh_loadTransformForMotionBuffer failed..\n");
                        }
                    free(motionBuffer);
                }
            else
                {
                    fprintf(stderr,"Could not allocate enough memory..\n");
                }
        }
    else
        {
            fprintf(stderr,"Could not initialize BVH subsystem..\n");
        }


#else
    fprintf(stderr,"BVH code is not compiled in this version of MocapNET\n");

#endif // USE_BVH
    return result;
}


std::vector<float> convertBVHFrameToFlat3DPoints(std::vector<float> bvhFrame)
{
#if USE_BVH
  return convertBVHFrameToFlat3DPointsFromMotion(&bvhMotion,bvhFrame);
#else
    std::vector<float>  empty;
    return empty;
#endif // USE_BVH
}

std::vector<float>  convertStandaloneLHandBVHFrameToFlat3DPoints(const std::vector<float> bvhFrame)
{
#if USE_BVH
  return convertBVHFrameToFlat3DPointsFromMotion(&standaloneLHand,bvhFrame);
#else
    std::vector<float>  empty;
    return empty;
#endif // USE_BVH
}


std::vector<float>  convertStandaloneRHandBVHFrameToFlat3DPoints(const std::vector<float> bvhFrame)
{
#if USE_BVH
  return convertBVHFrameToFlat3DPointsFromMotion(&standaloneRHand,bvhFrame);
  #else
    std::vector<float>  empty;
    return empty;
#endif // USE_BVH
}
