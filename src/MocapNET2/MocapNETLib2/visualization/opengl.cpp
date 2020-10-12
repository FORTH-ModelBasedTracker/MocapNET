#include <stdio.h>
#include "opengl.hpp"
#include "../mocapnet2.hpp"

#if USE_OPENGL
 int openGLHasInitialization=0;
 int openGLHasFailed=0;
 #include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/OpenGLAcquisition.h"
#endif

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


int initializeOpenGLStuff(unsigned int openGLFrameWidth,unsigned int openGLFrameHeight)
{
 #if USE_OPENGL
    if (openGLHasFailed)
 {
    fprintf(stderr,"visualizeOpenGL won't do anything because it has previously failed.. \n");
    return 0;
 }
   //------------------------------------------------------------------
   if (!openGLHasInitialization)
   {
      openGLHasInitialization=1;
      openGLHasFailed=1;
      if (startOpenGLModule(1,"")) //safe
      {
         if (
              createOpenGLDevice(
                                  0,
                                  "dataset/human3D.conf",
                                  openGLFrameWidth,
                                  openGLFrameHeight,
                                  30
                                 )
              )
              {
                  fprintf(stderr,GREEN "Success creating an OpenGL context" NORMAL);
                  fprintf(stderr,GREEN "Requested rendering resolution was %ux%u\n" NORMAL,openGLFrameWidth,openGLFrameHeight);
                  openGLHasFailed=0;
                  return 1;
              }
               else
              {
                fprintf(stderr,"Failed creating an OpenGL context");
                return 0;
              }
      } else
      {
         fprintf(stderr,"Failed initializing OpenGL context");
         return 0;
      }
   }
   #endif // USE_OPENGL
   //------------------------------------------------------------------
   //------------------------------------------------------------------
   return 0;
}

#if USE_OPENGL
#if USE_BVH
int changeOpenGLCalibration(struct calibration * calib)
{
  initializeOpenGLStuff(calib->width,calib->height);
  return  ( (setOpenGLColorCalibration(0,calib)) && (setOpenGLDepthCalibration(0,calib) ) );
}
#endif
#endif

char * visualizeOpenGL(unsigned int *openGLFrameWidth,unsigned int *openGLFrameHeight)
{
 #if USE_OPENGL
 //fprintf(stderr,"visualizeOpenGL CALLED for %u x %u size \n",*openGLFrameWidth,*openGLFrameHeight);

 if (openGLHasFailed)
 {
    fprintf(stderr,"visualizeOpenGL won't do anything because it has previously failed.. \n");
    return 0;
 }
   //------------------------------------------------------------------
   initializeOpenGLStuff(*openGLFrameWidth,*openGLFrameHeight);
   //------------------------------------------------------------------
   //------------------------------------------------------------------

   fprintf(stderr,"Snap OpenGL frame..\n");
   snapOpenGLFrames(0);
   fprintf(stderr,"Returning OpenGL frame..\n");
   return  getOpenGLColorPixels(0);
 #else
  fprintf(stderr,"OpenGL code not included in this build..\n");
  return 0;
 #endif
 }


int clickOpenGLView(unsigned int x,unsigned int y,unsigned int state)
{
 #if USE_OPENGL
 fprintf(stderr,"clickOpenGLView @ %u,%u\n",x,y);
   return  passUserInputOpenGL(0,0,state,x,y);
 #else
  fprintf(stderr,"OpenGL code not included in this build..\n");
  return 0;
 #endif
}




int updateOpenGLView(std::vector<float> bvhFrame)
{
  if (bvhFrame.size()<MOCAPNET_OUTPUT_NUMBER)
  {
    fprintf(stderr,"Cannot update OpenGL view without a full bvh frame\n");
    return 0;
  }
  #if USE_OPENGL
      float xDirection=1.0;
      float yDirection=1.0;
      float zDirection=1.0;
    unsigned int rotationOrder = OPENGL_ACQUISITION_JOINT_ROTATION_NEGATIVE_XYZ; //Check



  /*
   *  Uncomment to check OpenGL/BVH associations....
   *
    fprintf(stderr,"Associtations BVH joint | OpenCollada  ");
    for (int i=1; i<(MOCAPNET_OUTPUT_NUMBER-1)/3; i++)
    {
        if ((OpenCOLLADANames[i][0]!='-') && (OpenCOLLADANames[i][0]!='_'))
        {
            fprintf(stderr,"%s | %s \n",MocapNETOutputArrayNames[i*3],OpenCOLLADANames[i]);
        }
    }
    exit(0);
    */

    //Position/Rotation is common for automatic/manual setting..
    controlOpenGLScene(
                       "human",
                       "position",
                       OPENGL_ACQUISITION_POSITION_XYZ,
                       -0.1*bvhFrame[MOCAPNET_OUTPUT_HIP_XPOSITION],//If the position of the OpenGL overlay is wrong
                       0.1*bvhFrame[MOCAPNET_OUTPUT_HIP_YPOSITION]-7.7,//          It means that you
                       -0.1*bvhFrame[MOCAPNET_OUTPUT_HIP_ZPOSITION]+2.4 //Need to supply correct intrinsics to OGL..
                      );

    controlOpenGLScene(
                       "human",
                       "Hips",
                       OPENGL_ACQUISITION_JOINT_ROTATION_ZYX, //OPENGL_ACQUISITION_JOINT_ROTATION_XYZ <- this uses a different rotation scheme..
                       -1.0*xDirection*bvhFrame[MOCAPNET_OUTPUT_HIP_ZROTATION],
                       -1.0*yDirection*bvhFrame[MOCAPNET_OUTPUT_HIP_YROTATION],
                       -1.0*zDirection*bvhFrame[MOCAPNET_OUTPUT_HIP_XROTATION]
                      );

    unsigned int iterationsToPerform = (unsigned int) (MOCAPNET_OUTPUT_NUMBER-1)/3;
    for (int i=2; i<iterationsToPerform; i++)
    {
        if ((OpenCOLLADANames[i][0]!='-') && (OpenCOLLADANames[i][0]!='_'))
        {
            //fprintf(stderr,"%s | %s \n",MocapNETOutputArrayNames[i*3],OpenCOLLADANames[i]);

            controlOpenGLScene(
                               "human",OpenCOLLADANames[i], rotationOrder,
                                xDirection*bvhFrame[(i*3)+1],
                                yDirection*bvhFrame[(i*3)+2],
                                zDirection*bvhFrame[(i*3)+0]
                               );
        }
    }
    return 1;
  #else
    return 0;
  #endif
}
