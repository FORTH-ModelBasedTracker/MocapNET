#include <stdio.h>
#include "opengl.hpp"
#include "mocapnet.hpp"

#if USE_OPENGL
 int openGLHasInitialization=0;
 int openGLHasFailed=0;
 #include "../RGBDAcquisition/opengl_acquisition_shared_library/OpenGLAcquisition.h"
#endif
 

char * visualizeOpenGL(unsigned int *openGLFrameWidth,unsigned int *openGLFrameHeight)
{
   
 #if USE_OPENGL
 fprintf(stderr,"visualizeOpenGL CALLED for %u x %u size \n",*openGLFrameWidth,*openGLFrameHeight);

 if (openGLHasFailed)
 {
    fprintf(stderr,"visualizeOpenGL won't do anything because it failed.. \n");
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
                                  *openGLFrameWidth,
                                  *openGLFrameHeight,
                                  30
                                 )
              )
              {
                  fprintf(stderr,"Success creating an OpenGL context");
                  openGLHasFailed=0;
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
   //------------------------------------------------------------------ 
   //------------------------------------------------------------------
   
   fprintf(stderr,"Snap OpenGL frame..\n");
   snapOpenGLFrames(0); 
   fprintf(stderr,"Returning OpenGL frame..\n");
   return  getOpenGLColorPixels(0);
 
 #endif
   
    
  return 0;  
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
      float xAddition=0.0;
      float yAddition=0.0;
      float zAddition=0.0;
      unsigned int rotationOrder = OPENGL_ACQUISITION_JOINT_ROTATION_TEST;//Until i fix this..
  
      //-------------------------------------------------------------
      /*
    controlOpenGLScene(
                       "human",
                       "Hips",
                       OPENGL_ACQUISITION_JOINT_ROTATION_XYZ,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_HIP_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_HIP_YROTATION]+yAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_HIP_ZROTATION]+zAddition
                      ); */
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "Spine", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_ABDOMEN_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_ABDOMEN_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_ABDOMEN_ZROTATION]+zAddition
                      );
    controlOpenGLScene(
                       "human", "Spine1", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_CHEST_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_CHEST_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_CHEST_ZROTATION]+zAddition
                      );               
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "LeftShoulder", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_LCOLLAR_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_LCOLLAR_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_LCOLLAR_ZROTATION]+zAddition
                      );
    controlOpenGLScene(
                       "human", "LeftArm", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_LSHOULDER_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_LSHOULDER_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_LSHOULDER_ZROTATION]+zAddition
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                        "human", "LeftForeArm", rotationOrder,
                        xDirection*bvhFrame[MOCAPNET_OUTPUT_LELBOW_XROTATION]+xAddition,
                        yDirection*bvhFrame[MOCAPNET_OUTPUT_LELBOW_YROTATION]+yAddition,
                        zDirection*bvhFrame[MOCAPNET_OUTPUT_LELBOW_ZROTATION]+zAddition
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                        "human", "LeftHand", rotationOrder,
                        xDirection*bvhFrame[MOCAPNET_OUTPUT_LHAND_XROTATION]+xAddition,
                        yDirection*bvhFrame[MOCAPNET_OUTPUT_LHAND_YROTATION]+yAddition,
                        zDirection*bvhFrame[MOCAPNET_OUTPUT_LHAND_ZROTATION]+zAddition
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "RightShoulder", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RCOLLAR_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RCOLLAR_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RCOLLAR_ZROTATION]+zAddition
                      );
    controlOpenGLScene(
                       "human", "RightArm", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RSHOULDER_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RSHOULDER_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RSHOULDER_ZROTATION]+zAddition
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "RightForeArm", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RELBOW_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RELBOW_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RELBOW_ZROTATION]+zAddition
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "RightHand", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RHAND_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RHAND_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RHAND_ZROTATION]+zAddition
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                        "human", "LeftUpLeg", rotationOrder,
                        xDirection*bvhFrame[MOCAPNET_OUTPUT_LHIP_XROTATION]+xAddition,
                        yDirection*bvhFrame[MOCAPNET_OUTPUT_LHIP_YROTATION]+yAddition,
                        zDirection*bvhFrame[MOCAPNET_OUTPUT_LHIP_ZROTATION]+zAddition
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "LeftLeg", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_LKNEE_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_LKNEE_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_LKNEE_ZROTATION]+zAddition
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "LeftFoot", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_LFOOT_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_LFOOT_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_LFOOT_ZROTATION]+zAddition
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "RightUpLeg", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RHIP_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RHIP_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RHIP_ZROTATION]+zAddition
                      ); 
    //-------------------------------------------------------------  
    controlOpenGLScene(
                       "human", "RightLeg", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RKNEE_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RKNEE_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RKNEE_ZROTATION]+zAddition
                      ); 
    //------------------------------------------------------------- 
    controlOpenGLScene(
                       "human", "RightFoot", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RFOOT_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RFOOT_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RFOOT_ZROTATION]+zAddition
                      ); 
    //------------------------------------------------------------- 
   
    return 1;
  #endif    
  return 0;  
}
