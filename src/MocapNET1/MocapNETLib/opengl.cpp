#include <stdio.h>
#include "opengl.hpp"
#include "mocapnet.hpp"

#if USE_OPENGL
 int openGLHasInitialization=0;
 int openGLHasFailed=0;
 #include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/OpenGLAcquisition.h"
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
      float xAddition=0.0;
      float yAddition=0.0;
      float zAddition=0.0;
      unsigned int rotationOrder = OPENGL_ACQUISITION_JOINT_ROTATION_NEGATIVE_XYZ; 
  
  /* 
   *  TODO:  update OpenCOLLADANames to match the bvh joints found here..
   * 
    for (int i=3; i<(MOCAPNET_OUTPUT_NUMBER-1)/3; i++)
    {
        if (OpenCOLLADANames[i][0]!='-')
        {
            controlOpenGLScene(
                       "human",OpenCOLLADANames[i], rotationOrder,
                       xDirection*bvhFrame[3+i*3+0]+xAddition,
                       yDirection*bvhFrame[3+i*3+1]+yAddition,
                       zDirection*bvhFrame[3+i*3+2]+zAddition
                      );
            
        } 
    }*/
  
      //---------------------------------------------------------------------------------------
      // If you want Position/Rotation on your OpenGL virtual human uncomment next two lines..! 
      //---------------------------------------------------------------------------------------
      
      //Position/Rotation is common for automatic/manual setting..
       controlOpenGLScene(
                           "human",
                           "position",
                           OPENGL_ACQUISITION_POSITION_XYZ,
                           -0.1*bvhFrame[MOCAPNET_OUTPUT_HIP_XPOSITION],
                           -0.1*bvhFrame[MOCAPNET_OUTPUT_HIP_YPOSITION]-3.4, // I don't like this minus here..
                           -0.1*bvhFrame[MOCAPNET_OUTPUT_HIP_ZPOSITION]
                          );
  
    controlOpenGLScene(
                       "human",
                       "Hips",
                       OPENGL_ACQUISITION_JOINT_ROTATION_ZYX, //OPENGL_ACQUISITION_JOINT_ROTATION_XYZ <- this uses a different rotation scheme..
                       -1.0*xDirection*bvhFrame[MOCAPNET_OUTPUT_HIP_ZROTATION],
                       -1.0*yDirection*bvhFrame[MOCAPNET_OUTPUT_HIP_YROTATION],
                       -1.0*zDirection*bvhFrame[MOCAPNET_OUTPUT_HIP_XROTATION]
                      );                                          
     
    //-------------------------------------------------------------

    #define AUTOMATIC_SETTING 1
      
     #if AUTOMATIC_SETTING
     //Why doesn't this work..
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
    #else
    //------------------------------------------------------------- 
    controlOpenGLScene(
                       "human", "Spine", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_ABDOMEN_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_ABDOMEN_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_ABDOMEN_ZROTATION]
                      );
    controlOpenGLScene(
                       "human", "Spine1", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_CHEST_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_CHEST_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_CHEST_ZROTATION]
                      );               
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "LeftShoulder", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_LCOLLAR_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_LCOLLAR_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_LCOLLAR_ZROTATION]
                      );
    controlOpenGLScene(
                       "human", "LeftArm", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_LSHOULDER_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_LSHOULDER_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_LSHOULDER_ZROTATION]
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                        "human", "LeftForeArm", rotationOrder,
                        xDirection*bvhFrame[MOCAPNET_OUTPUT_LELBOW_XROTATION],
                        yDirection*bvhFrame[MOCAPNET_OUTPUT_LELBOW_YROTATION],
                        zDirection*bvhFrame[MOCAPNET_OUTPUT_LELBOW_ZROTATION]
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                        "human", "LeftHand", rotationOrder,
                        xDirection*bvhFrame[MOCAPNET_OUTPUT_LHAND_XROTATION],
                        yDirection*bvhFrame[MOCAPNET_OUTPUT_LHAND_YROTATION],
                        zDirection*bvhFrame[MOCAPNET_OUTPUT_LHAND_ZROTATION]
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "RightShoulder", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RCOLLAR_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RCOLLAR_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RCOLLAR_ZROTATION]
                      );
    controlOpenGLScene(
                       "human", "RightArm", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RSHOULDER_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RSHOULDER_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RSHOULDER_ZROTATION]
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "RightForeArm", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RELBOW_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RELBOW_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RELBOW_ZROTATION]
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "RightHand", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RHAND_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RHAND_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RHAND_ZROTATION]
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                        "human", "LeftUpLeg", rotationOrder,
                        xDirection*bvhFrame[MOCAPNET_OUTPUT_LHIP_XROTATION],
                        yDirection*bvhFrame[MOCAPNET_OUTPUT_LHIP_YROTATION],
                        zDirection*bvhFrame[MOCAPNET_OUTPUT_LHIP_ZROTATION]
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "LeftLeg", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_LKNEE_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_LKNEE_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_LKNEE_ZROTATION]
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "LeftFoot", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_LFOOT_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_LFOOT_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_LFOOT_ZROTATION]
                      );
    //-------------------------------------------------------------
    controlOpenGLScene(
                       "human", "RightUpLeg", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RHIP_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RHIP_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RHIP_ZROTATION]
                      ); 
    //-------------------------------------------------------------  
    controlOpenGLScene(
                       "human", "RightLeg", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RKNEE_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RKNEE_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RKNEE_ZROTATION]
                      ); 
    //------------------------------------------------------------- 
    controlOpenGLScene(
                       "human", "RightFoot", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_RFOOT_XROTATION],
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_RFOOT_YROTATION],
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_RFOOT_ZROTATION]
                      ); 
    #endif
   
    return 1;
  #else
    return 0; 
  #endif    
}
