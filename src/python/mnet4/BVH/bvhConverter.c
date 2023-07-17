/** @file main.c
 *  @brief  A library that can parse BVH files and perform various processing options as a commandline tool
 *          X86 compilation: gcc -o -L/usr/X11/lib   main main.c
 *          X64 compilation: gcc -o -L/usr/X11/lib64 main main.c
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../../Library/TrajectoryParser/TrajectoryParserDataStructures.h"
#include "../../Library/MotionCaptureLoader/bvh_loader.h"
#include "../../Library/MotionCaptureLoader/calculate/bvh_to_tri_pose.h"
#include "../../Library/MotionCaptureLoader/calculate/smoothing.h"

#include "../../Library/MotionCaptureLoader/export/bvh_to_trajectoryParserTRI.h"
#include "../../Library/MotionCaptureLoader/export/bvh_to_trajectoryParserPrimitives.h"
#include "../../Library/MotionCaptureLoader/export/bvh_export.h"
#include "../../Library/MotionCaptureLoader/export/bvh_to_bvh.h"
#include "../../Library/MotionCaptureLoader/export/bvh_to_csv.h"
#include "../../Library/MotionCaptureLoader/export/bvh_to_c.h"

#include "../../Library/MotionCaptureLoader/edit/bvh_cut_paste.h"
#include "../../Library/MotionCaptureLoader/edit/bvh_randomize.h"
#include "../../Library/MotionCaptureLoader/edit/bvh_filter.h"
#include "../../Library/MotionCaptureLoader/edit/bvh_rename.h"
#include "../../Library/MotionCaptureLoader/edit/bvh_merge.h"
#include "../../Library/MotionCaptureLoader/edit/bvh_remapangles.h"
#include "../../Library/MotionCaptureLoader/edit/bvh_interpolate.h"

#include "../../Library/MotionCaptureLoader/ik/bvh_inverseKinematics.h"
#include "../../Library/MotionCaptureLoader/ik/hardcodedProblems_inverseKinematics.h"

#include "../../Library/MotionCaptureLoader/metrics/bvh_measure.h"
#include "../../Library/MotionCaptureLoader/tests/test.h"

#include  "../../../../../tools/AmMatrix/matrix4x4Tools.h"
#include  "../../../../../tools/AmMatrix/matrixOpenGL.h"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

void haltOnError(unsigned int haltingSwitch,const char * message)
{
  fprintf(stderr,RED "=======================================\n");
  fprintf(stderr,"=======================================\n");
  fprintf(stderr,"Encountered error during procedure %s \n",message);
  fprintf(stderr,"=======================================\n");
  fprintf(stderr,"=======================================\n" NORMAL);

  if (haltingSwitch)
    {
       fprintf(stderr,RED "Halting because of --haltonerror switch\n" NORMAL);
       exit(1);
    }
}

void incorrectArguments()
{
  fprintf(stderr,RED "Incorrect number of arguments.. \n" NORMAL);
  exit(1);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
struct BVH_MotionCapture         bvhAtomicMotion              = {0};
struct BVH_Transform             bvhTransformAtomic           = {0};
struct simpleRenderer            rendererAtomic               = {0};
struct BVH_RendererConfiguration renderingAtomicConfiguration = {0};
struct ikProblem * atomicFaceProblem  = 0;
struct ikProblem * atomicBodyProblem  = 0;
struct ikProblem * atomicLHandProblem = 0;
struct ikProblem * atomicRHandProblem = 0;
struct MotionBuffer * atomicPenultimateSolution=0;
struct MotionBuffer * atomicPreviousSolution=0;
struct MotionBuffer * atomicSolution=0;
struct ButterWorthArray * atomicSmoothingFilter = 0;
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
int bvhConverter_loadAtomic(const char *path)
{
  float scaleWorld=1.0;
  int immediatelyHaltOnError = 1;
  fprintf(stderr,"Attempting to load %s\n",path);
  if (!bvh_loadBVH(path, &bvhAtomicMotion, scaleWorld))
          {
            haltOnError(immediatelyHaltOnError,"Error loading bvh file..");
          }
  //Change joint names..
  bvh_renameJointsForCompatibility(&bvhAtomicMotion);


  // Emulate GoPro Hero4 @ FullHD mode by default..
  // https://gopro.com/help/articles/Question_Answer/HERO4-Field-of-View-FOV-Information
  renderingAtomicConfiguration.near   = 1.0;
  renderingAtomicConfiguration.far    = 10000.0;
  renderingAtomicConfiguration.width  = 1920;
  renderingAtomicConfiguration.height = 1080;
  renderingAtomicConfiguration.cX     = (float)renderingAtomicConfiguration.width/2;
  renderingAtomicConfiguration.cY     = (float)renderingAtomicConfiguration.height/2;
  renderingAtomicConfiguration.fX     = 582.18394;
  renderingAtomicConfiguration.fY     = 582.52915;
  //----------------------------------------------
  simpleRendererDefaults(
                          &rendererAtomic,
                          renderingAtomicConfiguration.width,
                          renderingAtomicConfiguration.height,
                          renderingAtomicConfiguration.fX,
                          renderingAtomicConfiguration.fY
                         );
  //----------------------------------------------
  simpleRendererInitialize(&rendererAtomic);
  //----------------------------------------------
  return bvhAtomicMotion.jointHierarchySize;
}


int bvhConverter_unloadAtomic()
{
    /* TODO: unload all this..!
    struct BVH_MotionCapture         bvhAtomicMotion={0};
struct BVH_Transform             bvhTransformAtomic={0};
struct simpleRenderer            rendererAtomic={0};
struct BVH_RendererConfiguration renderingAtomicConfiguration={0};
struct ikProblem * atomicFaceProblem  = 0;
struct ikProblem * atomicBodyProblem  = 0;
struct ikProblem * atomicLHandProblem = 0;
struct ikProblem * atomicRHandProblem = 0;
struct MotionBuffer * atomicPreviousSolution=0;
struct MotionBuffer * atomicSolution=0;
*/
  fprintf(stderr,"bvhConverter_unloadAtomic not implemented yet..\n");
  return 0;
}

int bvhConverter_rendererConfigurationAtomic(const char ** labels,const float * values,int numberOfElements)
{
  // Emulate GoPro Hero4 @ FullHD mode by default..
  // https://gopro.com/help/articles/Question_Answer/HERO4-Field-of-View-FOV-Information
  renderingAtomicConfiguration.near   = 1.0;
  renderingAtomicConfiguration.far    = 10000.0;
  renderingAtomicConfiguration.width  = 1920;
  renderingAtomicConfiguration.height = 1080;
  renderingAtomicConfiguration.cX     = (float)renderingAtomicConfiguration.width/2;
  renderingAtomicConfiguration.cY     = (float)renderingAtomicConfiguration.height/2;
  renderingAtomicConfiguration.fX     = 582.18394;
  renderingAtomicConfiguration.fY     = 582.52915;

  fprintf(stderr,"bvhConverter_rendererConfigurationAtomic received %u elements\n",numberOfElements);
  for (int i=0; i<numberOfElements; i++)
  {
      //fprintf(stderr," %u - %s->%0.2f\n",i,labels[i],values[i]);
      if (strcmp(labels[i],"near")==0)   { renderingAtomicConfiguration.near   = values[i]; } else
      if (strcmp(labels[i],"far")==0)    { renderingAtomicConfiguration.far    = values[i]; } else
      if (strcmp(labels[i],"width")==0)  { renderingAtomicConfiguration.width  = (unsigned int) values[i]; } else
      if (strcmp(labels[i],"height")==0) { renderingAtomicConfiguration.height = (unsigned int) values[i]; } else
      if (strcmp(labels[i],"cX")==0)     { renderingAtomicConfiguration.cX     = values[i]; } else
      if (strcmp(labels[i],"cY")==0)     { renderingAtomicConfiguration.cY     = values[i]; } else
      if (strcmp(labels[i],"fX")==0)     { renderingAtomicConfiguration.fX     = values[i]; } else
      if (strcmp(labels[i],"fY")==0)     { renderingAtomicConfiguration.fY     = values[i]; } else
        {
          fprintf(stderr,RED"bvhConverter_rendererConfigurationAtomic: Unknown command %u - %s->%0.2f\n" NORMAL,i,labels[i],values[i]);
        }
  }

   simpleRendererDefaults(
                          &rendererAtomic,
                          renderingAtomicConfiguration.width,
                          renderingAtomicConfiguration.height,
                          renderingAtomicConfiguration.fX,
                          renderingAtomicConfiguration.fY
                         );
    simpleRendererInitialize(&rendererAtomic);
  return 1;
}

int bvhConverter_processFrame(int frameID)
{
    int occlusions=1;
    return performPointProjectionsForFrame(
                                           &bvhAtomicMotion,
                                           &bvhTransformAtomic,
                                           frameID,
                                           &rendererAtomic,
                                           occlusions,
                                           renderingAtomicConfiguration.isDefined
                                          );
}


int bvhConverter_scale(float scaleRatio)
{
  fprintf(stderr,"Offset scaling ratio = %0.2f \n",scaleRatio);
  return bvh_scaleAllOffsets(
                              &bvhAtomicMotion,
                              scaleRatio
                            );
}

int bvhConverter_getNumberOfMotionValuesPerFrame()
{
 return bvhAtomicMotion.numberOfValuesPerFrame;
}

int bvhConverter_getNumberOfJoints()
{
 return bvhAtomicMotion.jointHierarchySize;
}

int bvhConverter_writeBVH(char * filename,int writeHierarchy,int writeMotion)
{
 return dumpBVHToBVH(
                     filename,
                     &bvhAtomicMotion,
                     writeHierarchy,
                     writeMotion
                    );
}

int bvhConverter_getMotionValueOfFrame(int fID,int mID)
{
 return bvh_getMotionValueOfFrame(&bvhAtomicMotion,fID,mID);
}

int bvhConverter_setMotionValueOfFrame(int fID,int mID,float value)
{
 float localValue = value;
 return bvh_setMotionValueOfFrame(&bvhAtomicMotion,fID,mID,&localValue);
}

int bvhConverter_getJointNameJointID(const char * jointName)
{
  //fprintf(stderr,"Asked to resolve %s\n",jointName);
  BVHJointID jID=0;
  if  (
        bvh_getJointIDFromJointNameNocase(
                                          &bvhAtomicMotion,
                                          jointName,
                                          &jID
                                         )
      )
      {
       return jID;
      }
  fprintf(stderr,RED "BVH library could not resolve joint \"%s\" \n" NORMAL,jointName);
  return -1;
}

const char * bvhConverter_getJointNameFromJointID(int jointID)
{
    if (jointID<bvhAtomicMotion.jointHierarchySize)
    {
        return bvhAtomicMotion.jointHierarchy[jointID].jointName;
    }
  fprintf(stderr,RED "BVH library could not resolve joint name for joint out bounds \"%u\" \n" NORMAL,jointID);
  return "";
}

int bvhConverter_getJointParent(int jointID)
{
    if (jointID<bvhAtomicMotion.jointHierarchySize)
    {
        return bvhAtomicMotion.jointHierarchy[jointID].parentJoint;
    }
  fprintf(stderr,RED "BVH library could not resolve joint parent for joint out bounds \"%u\" \n" NORMAL,jointID);
  return 0;
}

float bvhConverter_get3DX(int jointID)
{
  //fprintf(stderr,"bvhConverter_get3DX(%u)\n",jointID);
  if (jointID<bvhTransformAtomic.numberOfJointsToTransform)
     { return bvhTransformAtomic.joint[jointID].pos3D[0]; }
  return 0.0;
}

float  bvhConverter_get3DY(int jointID)
{
  //fprintf(stderr,"bvhConverter_get3DY(%u)\n",jointID);
  if (jointID<bvhTransformAtomic.numberOfJointsToTransform)
     { return bvhTransformAtomic.joint[jointID].pos3D[1]; }
  return 0.0;
}

float  bvhConverter_get3DZ(int jointID)
{
  //fprintf(stderr,"bvhConverter_get3DZ(%u)\n",jointID);
  if (jointID<bvhTransformAtomic.numberOfJointsToTransform)
     { return bvhTransformAtomic.joint[jointID].pos3D[2]; }
  return 0.0;
}

float bvhConverter_get2DX(int jointID)
{
  //fprintf(stderr,"bvhConverter_get2DX(%u)\n",jointID);
  if (jointID<bvhTransformAtomic.numberOfJointsToTransform)
     { return (float) bvhTransformAtomic.joint[jointID].pos2D[0]/renderingAtomicConfiguration.width; }
  return 0.0;
}

float  bvhConverter_get2DY(int jointID)
{
  //fprintf(stderr,"bvhConverter_get2DY(%u)\n",jointID);
  if (jointID<bvhTransformAtomic.numberOfJointsToTransform)
     { return (float) bvhTransformAtomic.joint[jointID].pos2D[1]/renderingAtomicConfiguration.height; }
  return 0.0;
}



int bvhConverter_isJointEndSite(int jointID)
{
  if (jointID<bvhAtomicMotion.jointHierarchySize)
     { return bvhAtomicMotion.jointHierarchy[jointID].isEndSite; }
  fprintf(stderr,RED "BVH library could not resolve if joint is an EndSite because joint is out bounds \"%u\" \n" NORMAL,jointID);
  return 0;
}

float bvhConverter_getBVHJointRotationXForFrame(int frameID,int jointID)
{
  //fprintf(stderr,"bvhConverter_get2DX(%u)\n",jointID);
  if (jointID<bvhTransformAtomic.numberOfJointsToTransform)
     { return (float) bvh_getJointRotationXAtFrame(&bvhAtomicMotion,jointID,frameID); }
  return 0.0;
}
float bvhConverter_getBVHJointRotationYForFrame(int frameID,int jointID)
{
  //fprintf(stderr,"bvhConverter_get2DX(%u)\n",jointID);
  if (jointID<bvhTransformAtomic.numberOfJointsToTransform)
     { return (float) bvh_getJointRotationYAtFrame(&bvhAtomicMotion,jointID,frameID); }
  return 0.0;
}
float bvhConverter_getBVHJointRotationZForFrame(int frameID,int jointID)
{
  //fprintf(stderr,"bvhConverter_get2DX(%u)\n",jointID);
  if (jointID<bvhTransformAtomic.numberOfJointsToTransform)
     { return (float) bvh_getJointRotationZAtFrame(&bvhAtomicMotion,jointID,frameID); }
  return 0.0;
}



int bvhConverter_modifySingleAtomic(const char * label,const float value,int frameID)
{
  if (label==0) { return 0; }
  if (frameID>=bvhAtomicMotion.numberOfFrames) { return 0; }
  //------------------------------------------------------------
  //fprintf(stderr,"bvhConverter_modifyAtomic received element %s with value %0.2f for frame %u\n",label,value,frameID);
  //------------------------------------------------------------
  int everythingOk = 1;
  char jointName[513]={0};
  snprintf(jointName,512,"%s",label);
  char * delimeter = strchr(jointName,'_');
  if (delimeter==0)
  {
      fprintf(stderr,"bvhConverter_modifyAtomic received element %s with value %0.2f for frame %u ",label,value,frameID);
      fprintf(stderr,"it doesn't have a degree of freedom associated so ignoring it..");
      return 0;
  }
  *delimeter = 0;
  char * dof = delimeter+1;
  //=======================================================
  lowercase(jointName);
  lowercase(dof);
  //=======================================================
  if (strstr(jointName,"endsite_")!=0)
  {
     fprintf(stderr,RED "Endsites can't be modified..!\n" NORMAL);
     return 0;
  }

  if (strcmp(jointName,"neck01")==0)
  {
    snprintf(jointName,512,"neck1"); //Fix ?
  }
  if (strcmp(jointName,"lthumbbase")==0)
  {
    snprintf(jointName,512,"__lthumb"); //Fix ?
  }
  if (strcmp(jointName,"rthumbbase")==0)
  {
    snprintf(jointName,512,"__rthumb"); //Fix ?
  }

  //fprintf(stderr," %u - %s->%0.2f ",i,label,value);
  //fprintf(stderr," Joint:%s Control:%s\n",jointName,dof);
  //=======================================================
  //int jointID = bvhConverter_getJointNameJointID(jointName);
  BVHJointID jointID=0;
  if  (
        bvh_getJointIDFromJointNameNocase(
                                          &bvhAtomicMotion,
                                          jointName,
                                          &jointID
                                         )
      )
      {
      // The next line is a debug message that spams a *lot*!
      //fprintf(stderr,"Joint ID %u / %s|%s => %0.2f \n",jointID,bvhAtomicMotion.jointHierarchy[jointID].jointName,dof,value);
      //==============================================================================================================
      if (strcmp(dof,"xposition")==0) { bvh_setJointPositionXAtFrame(&bvhAtomicMotion,jointID,frameID,value); } else
      if (strcmp(dof,"yposition")==0) { bvh_setJointPositionYAtFrame(&bvhAtomicMotion,jointID,frameID,value); } else
      if (strcmp(dof,"zposition")==0) { bvh_setJointPositionZAtFrame(&bvhAtomicMotion,jointID,frameID,value); } else
      if (strcmp(dof,"xrotation")==0) { bvh_setJointRotationXAtFrame(&bvhAtomicMotion,jointID,frameID,value); } else
      if (strcmp(dof,"yrotation")==0) { bvh_setJointRotationYAtFrame(&bvhAtomicMotion,jointID,frameID,value); } else
      if (strcmp(dof,"zrotation")==0) { bvh_setJointRotationZAtFrame(&bvhAtomicMotion,jointID,frameID,value); } else
      if (strcmp(dof,"wrotation")==0) { bvh_setJointRotationWAtFrame(&bvhAtomicMotion,jointID,frameID,value); } else
                                      {
                                         fprintf(stderr,RED "\n\n\nBVH library could not perform modification  \"%s\" for joint \"%s\" \n\n\n" NORMAL,dof,jointName);
                                         everythingOk=0;
                                      }
      //==============================================================================================================
      } else
      {
          fprintf(stderr,RED "\nBVH library modification could not resolve joint \"%s\" \n" NORMAL,jointName);
          everythingOk=0;
      }
  return everythingOk;
}



int bvhConverter_modifyAtomic(const char ** labels,const float * values,int numberOfElements,int frameID)
{
  //fprintf(stderr,"bvhConverter_modifyAtomic received %u elements\n",numberOfElements);
  int everythingOk = 1;
  for (int i=0; i<numberOfElements; i++)
  {
      if (!bvhConverter_modifySingleAtomic(labels[i],values[i],frameID))
      {
          fprintf(stderr,RED "\n\n\nBVH library modification failed resolving all joints\n\n\n" NORMAL);
          everythingOk=0;
      }
  }
  return everythingOk;
}


int bvhConverter_IKSetup(const char * bodyPart,const char ** labels,const float * values,int numberOfElements,int frameID)
{
    struct ikProblem * problem = 0;

    //Handle current/previous solution (assuming bvhConverter_modifyAtomic has been called)..
    //=======================================================================================
    if (atomicPenultimateSolution==0)
          { atomicPenultimateSolution = mallocNewMotionBuffer(&bvhAtomicMotion); }
    //=======================================================================================
    if (atomicPreviousSolution==0)
          { atomicPreviousSolution    = mallocNewMotionBuffer(&bvhAtomicMotion); }
     //====================================================================
    if (atomicSolution==0)
          { atomicSolution            = mallocNewMotionBuffer(&bvhAtomicMotion); }
     //====================================================================


    //Construct and/or select the correct problem to solve..!
    if (strcmp(bodyPart,"body")==0)
    {
       if (atomicBodyProblem==0)
           {
            fprintf(stderr,GREEN "Initializing Body Problem for the first time..\n" NORMAL);
            atomicBodyProblem = allocateEmptyIKProblem();
            prepareDefaultBodyProblem(
                                          atomicBodyProblem,
                                          &bvhAtomicMotion,
                                          &rendererAtomic,
                                          atomicPreviousSolution,
                                          atomicSolution,
                                          &bvhTransformAtomic
                                         );
            fprintf(stderr,GREEN "Done Initializing Body Problem ..\n" NORMAL);
           }
      problem = atomicBodyProblem;
    } else
    if (strcmp(bodyPart,"face")==0)
    {
      if (atomicFaceProblem==0)
           {
            fprintf(stderr,GREEN "Initializing Face Problem for the first time..\n" NORMAL);
              atomicFaceProblem = allocateEmptyIKProblem();
              prepareDefaultFaceProblem(
                                        atomicFaceProblem,
                                        &bvhAtomicMotion,
                                        &rendererAtomic,
                                        atomicPreviousSolution,
                                        atomicSolution,
                                        &bvhTransformAtomic,
                                        1
                                       );
            fprintf(stderr,GREEN "Done Initializing Face Problem ..\n" NORMAL);
           }
      problem = atomicFaceProblem;
    } else
    if (strcmp(bodyPart,"rhand")==0)
    {
       if (atomicRHandProblem==0)
           {
            fprintf(stderr,GREEN "Initializing RHand Problem for the first time..\n" NORMAL);
            atomicRHandProblem = allocateEmptyIKProblem();
            prepareDefaultRightHandProblem(
                                          atomicRHandProblem,
                                          &bvhAtomicMotion,
                                          &rendererAtomic,
                                          atomicPreviousSolution,
                                          atomicSolution,
                                          &bvhTransformAtomic,
                                          1
                                         );
            fprintf(stderr,GREEN "Done Initializing RHand Problem ..\n" NORMAL);
           }
       problem = atomicRHandProblem;
    } else
    if (strcmp(bodyPart,"lhand")==0)
    {
       if (atomicLHandProblem==0)
           {
            fprintf(stderr,GREEN "Initializing LHand Problem for the first time..\n" NORMAL);
            atomicLHandProblem = allocateEmptyIKProblem();
            prepareDefaultLeftHandProblem(
                                          atomicLHandProblem,
                                          &bvhAtomicMotion,
                                          &rendererAtomic,
                                          atomicPreviousSolution,
                                          atomicSolution,
                                          &bvhTransformAtomic,
                                          1
                                         );
            fprintf(stderr,GREEN "Done Initializing LHand Problem ..\n" NORMAL);
           }
       problem = atomicLHandProblem;
    }

    if (problem==0)
    {
      fprintf(stderr,"bvhConverter_IKSetup: Unrecognized body part `%s` \n",bodyPart);
      return 0;
    } else
    {
      return 1;
    }
}








float bvhConverter_IKFineTune(
                               const char * bodyPart,
                               const char ** labels,
                               const float * values,
                               int numberOfElements,
                               int frameID,
                               int iterations,
                               int epochs,
                               float lr,
                               float fSampling,
                               float fCutoff,
                               float langevinDynamics
                             )
{
  printf("bvhConverter_IKFineTune(Part %s,Elements %u, Frame %u)\n",bodyPart,numberOfElements,frameID);

  //-----------------------------
  float initialMAEInPixels = 0.0;
  float finalMAEInPixels   = 0.0;
  float initialMAEInMM     = 0.0;
  float finalMAEInMM       = 0.0;
  //-----------------------------
  //By default select no problem..
  struct ikProblem * selectedProblem  = 0;
  //-----------------------------
  int initializeIK = 0;
  if (strcmp(bodyPart,"body")==0)
    {
       selectedProblem = atomicBodyProblem;
       if (atomicBodyProblem==0)   { initializeIK=1; }
    } else
  if (strcmp(bodyPart,"rhand")==0)
    {
       selectedProblem = atomicRHandProblem;
       if (atomicRHandProblem==0)  { initializeIK=1; }
    } else
  if (strcmp(bodyPart,"lhand")==0)
    {
       selectedProblem = atomicLHandProblem;
       if (atomicLHandProblem==0)  { initializeIK=1; }
    } else
  if (strcmp(bodyPart,"face")==0)
    {
       selectedProblem = 0;//Just run Butterworth, no face IK
       if (atomicFaceProblem==0)   { initializeIK=1; }
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    if (initializeIK)
    {
     bvhConverter_IKSetup(bodyPart,labels,values,numberOfElements,frameID);
    }

    if (atomicSmoothingFilter==0)
    {
     fprintf(stderr,"Butterworth Smoothing filter initialized with fSampling:%0.2f and fCutoff:%0.2f \n",fSampling,fCutoff);
     atomicSmoothingFilter = butterWorth_allocate(bvhAtomicMotion.numberOfValuesPerFrame,fSampling,fCutoff);
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    //------------------------------------
         struct ikConfiguration ikConfig = {0};
         ikConfig.learningRate = lr;
         ikConfig.iterations = iterations;
         ikConfig.epochs = epochs;
         ikConfig.maximumAcceptableStartingLoss = 50000;//12000; //WARING < -  consider setting this to 0
         ikConfig.gradientExplosionThreshold    = 25; //50 default
         ikConfig.iterationEarlyStopping        = 1;  //<-
         ikConfig.iterationMinimumLossDelta     = 10; //<- losses seem to be numbers 2000 -> 300 so 10 is a good limit
         ikConfig.spring = 20;
         ikConfig.dumpScreenshots = 0; // Dont thrash disk
         ikConfig.verbose = 0; //Dont spam console
         ikConfig.tryMaintainingLocalOptima=1; //Less Jittery but can be stuck at local optima
         ikConfig.dontUseSolutionHistory=0;
         ikConfig.useLangevinDynamics = langevinDynamics;
         ikConfig.ikVersion = IK_VERSION;
         //------------------------------------

        int multiThreading = 0;

         //======================================================================================================
         //======================================================================================================
         //======================================================================================================
        if ( (strcmp(bodyPart,"body")==0) || (strcmp(bodyPart,"lhand")==0) || (strcmp(bodyPart,"rhand")==0) )
        {
         //Keep history..!
         copyMotionBuffer(atomicPenultimateSolution,atomicPreviousSolution);
         copyMotionBuffer(atomicPreviousSolution,atomicSolution);
         bvh_copyMotionFrameToMotionBuffer(
                                           &bvhAtomicMotion,
                                           atomicSolution,
                                           frameID
                                          );

         char jointName[512]={0};
         struct BVH_Transform bvhTargetTransform={0};
         int occlusions=1;
          performPointProjectionsForFrame(
                                           &bvhAtomicMotion,
                                           &bvhTargetTransform,
                                           frameID,
                                           &rendererAtomic,
                                           occlusions,
                                           renderingAtomicConfiguration.isDefined
                                          );

         for (int i=0; i<numberOfElements; i++)
         {
             //fprintf(stderr,"Number %u => %s with %0.2f \n",i,labels[i],values[i] );
             snprintf(jointName,512,"%s",labels[i]);
             char * delimeter = strchr(jointName,'_');

             if (delimeter!=0)
             {
             *delimeter = 0;
             char * coord = jointName;
             char * dof   = delimeter+1;
             //=======================================================
             lowercase(coord);
             lowercase(dof);

             if (  (coord[0]=='2') && (coord[1]=='d') && (  (coord[2]=='x') || (coord[2]=='y') ) )
             {
              BVHJointID jID=0;
              if ( bvh_getJointIDFromJointNameNocase(&bvhAtomicMotion,dof,&jID) )
                {
                  if (coord[2]=='x')
                  {
                   //fprintf(stderr,GREEN "%s/%s \n" NORMAL,coord,dof);
                   bvhTargetTransform.joint[jID].pos2D[0] = (float) values[i]*renderingAtomicConfiguration.width;
                  } else
                  if (coord[2]=='y')
                  {
                   //fprintf(stderr,GREEN "%s/%s \n" NORMAL,coord,dof);
                   bvhTargetTransform.joint[jID].pos2D[1] = (float) values[i]*renderingAtomicConfiguration.height;
                  }
                } else
                {
                  //fprintf(stderr,RED "IK: Could not resolve Joint %s for Number %u => %s with %0.2f \n" NORMAL,dof,i,labels[i],values[i] );
                }
            }//2DX/Y

           } //Tag has an _ and we process it

         }//Loop over received elements

         if (  approximateBodyFromMotionBufferUsingInverseKinematics(
                                                                     &bvhAtomicMotion,
                                                                     &rendererAtomic,
                                                                     selectedProblem,
                                                                     &ikConfig,
                                                                     //----------------
                                                                     atomicPenultimateSolution,
                                                                     atomicPreviousSolution,
                                                                     atomicSolution,
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

              /*  No longer automatically do this to avoid multiple smoothings per frame
              if ( (fSampling>0.0) && (fCutoff>0.0) )
              { //Only perform smoothing if sampling/cutoff is set..
               for (int mID=0; mID<atomicSolution->bufferSize; mID++)
               {
                   atomicSolution->motion[mID] = butterWorth_filterArrayElement(atomicSmoothingFilter,mID,atomicSolution->motion[mID]);
               }
              }*/



              if(!bvh_copyMotionBufferToMotionFrame(
                                                    &bvhAtomicMotion,
                                                    frameID,
                                                    atomicSolution
                                                   )
                )
                {
                    fprintf(stderr,RED "Failed bvh_copyMotionBufferToMotionFrame\n" NORMAL);
                }

                //Perform and update projections for new results..!
                bvhConverter_processFrame(frameID);
            } else
            {
              fprintf(stderr,RED "Failed approximateBodyFromMotionBufferUsingInverseKinematics\n" NORMAL);
            }
        }

  return finalMAEInPixels;
}


int bvhConverter_smooth(int frameID,float fSampling,float fCutoff)
{
 if (atomicSolution==0) { fprintf(stderr,RED "bvhConverter_smooth has no solution to work with..\n" NORMAL); return 0; }
 if (atomicSmoothingFilter==0) { fprintf(stderr,RED "bvhConverter_smooth has no initialized filter to work with..\n" NORMAL); return 0; }

 if ( (fSampling>0.0) && (fCutoff>0.0) )
   { //Only perform smoothing if sampling/cutoff is set..
    fprintf(stderr,GREEN "bvhConverter_smooth going through motions\n" NORMAL);
    for (int mID=0; mID<atomicSolution->bufferSize; mID++)
               {
                   atomicSolution->motion[mID] = butterWorth_filterArrayElement(atomicSmoothingFilter,mID,atomicSolution->motion[mID]);
               }



    fprintf(stderr,GREEN "copyback..\n" NORMAL);
    if(!bvh_copyMotionBufferToMotionFrame(
                                          &bvhAtomicMotion,
                                          frameID,
                                          atomicSolution
                                         )
                )
                {
                    fprintf(stderr,RED "Failed bvh_copyMotionBufferToMotionFrame\n" NORMAL);
                }

     //Perform and update projections for new results..!
     bvhConverter_processFrame(frameID);
     return 1;
   }
  return 0;
}



int bvhConverter(int argc,const char **argv)
{
    fprintf(stderr,RED "BVHConverter.c main is a stub please use the python code\n" NORMAL);
    return 0;
}

int main(int argc,const char **argv)
{
    fprintf(stderr,RED "BVHConverter.c main is a stub please use the python code\n" NORMAL);
    return 0;
}

