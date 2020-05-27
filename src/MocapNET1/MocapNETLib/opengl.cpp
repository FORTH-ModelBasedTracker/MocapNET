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
 
 
 
 /*
  * cat dataset/headerWithHeadAndOneMotion.bvh | grep JOINT
  * 
    JOINT abdomen
    JOINT chest
      JOINT neck
          JOINT neck1
            JOINT head
              JOINT __jaw
                JOINT jaw
                  JOINT special04
                    JOINT oris02
                      JOINT oris01
                    JOINT oris06.l
                      JOINT oris07.l
                    JOINT oris06.r
                      JOINT oris07.r
                  JOINT tongue00
                    JOINT tongue01
                      JOINT tongue02
                        JOINT tongue03
                          JOINT __tongue04
                            JOINT tongue04
                          JOINT tongue07.l
                          JOINT tongue07.r
                        JOINT tongue06.l
                        JOINT tongue06.r
                      JOINT tongue05.l
                      JOINT tongue05.r
              JOINT __levator02.l
                JOINT levator02.l
                  JOINT levator03.l
                    JOINT levator04.l
                      JOINT levator05.l
              JOINT __levator02.r
                JOINT levator02.r
                  JOINT levator03.r
                    JOINT levator04.r
                      JOINT levator05.r
              JOINT __special01
                JOINT special01
                  JOINT oris04.l
                    JOINT oris03.l
                  JOINT oris04.r
                    JOINT oris03.r
                  JOINT oris06
                    JOINT oris05
              JOINT __special03
                JOINT special03
                  JOINT __levator06.l
                    JOINT levator06.l
                  JOINT __levator06.r
                    JOINT levator06.r
              JOINT special06.l
                JOINT special05.l
                  JOINT eye.l
                  JOINT orbicularis03.l
                  JOINT orbicularis04.l
              JOINT special06.r
                JOINT special05.r
                  JOINT eye.r
                  JOINT orbicularis03.r
                  JOINT orbicularis04.r
              JOINT __temporalis01.l
                JOINT temporalis01.l
                  JOINT oculi02.l
                    JOINT oculi01.l
              JOINT __temporalis01.r
                JOINT temporalis01.r
                  JOINT oculi02.r
                    JOINT oculi01.r
              JOINT __temporalis02.l
                JOINT temporalis02.l
                  JOINT risorius02.l
                    JOINT risorius03.l
              JOINT __temporalis02.r
                JOINT temporalis02.r
                  JOINT risorius02.r
                    JOINT risorius03.r
      JOINT rCollar
        JOINT rShldr
          JOINT rForeArm
            JOINT rHand
                  JOINT metacarpal1.r
                    JOINT finger2-1.r
                      JOINT finger2-2.r
                        JOINT finger2-3.r
                  JOINT metacarpal2.r
                    JOINT finger3-1.r
                      JOINT finger3-2.r
                        JOINT finger3-3.r
                  JOINT __metacarpal3.r
                    JOINT metacarpal3.r
                      JOINT finger4-1.r
                        JOINT finger4-2.r
                          JOINT finger4-3.r
                  JOINT __metacarpal4.r
                    JOINT metacarpal4.r
                      JOINT finger5-1.r
                        JOINT finger5-2.r
                          JOINT finger5-3.r
                  JOINT __rthumb
                    JOINT rthumb
                      JOINT finger1-2.r
                        JOINT finger1-3.r
      JOINT lCollar
        JOINT lShldr
          JOINT lForeArm
            JOINT lHand
                  JOINT metacarpal1.l
                    JOINT finger2-1.l
                      JOINT finger2-2.l
                        JOINT finger2-3.l
                  JOINT metacarpal2.l
                    JOINT finger3-1.l
                      JOINT finger3-2.l
                        JOINT finger3-3.l
                  JOINT __metacarpal3.l
                    JOINT metacarpal3.l
                      JOINT finger4-1.l
                        JOINT finger4-2.l
                          JOINT finger4-3.l
                  JOINT __metacarpal4.l
                    JOINT metacarpal4.l
                      JOINT finger5-1.l
                        JOINT finger5-2.l
                          JOINT finger5-3.l
                  JOINT __lthumb
                    JOINT lthumb
                      JOINT finger1-2.l
                        JOINT finger1-3.l
  JOINT rButtock
    JOINT rThigh
      JOINT rShin
        JOINT rFoot
							JOINT toe1-1.R
								JOINT toe1-2.R
							JOINT toe2-1.R
								JOINT toe2-2.R
									JOINT toe2-3.R
							JOINT toe3-1.R
								JOINT toe3-2.R
									JOINT toe3-3.R
							JOINT toe4-1.R
								JOINT toe4-2.R
									JOINT toe4-3.R
							JOINT toe5-1.R
								JOINT toe5-2.R
									JOINT toe5-3.R
  JOINT lButtock
    JOINT lThigh
      JOINT lShin
        JOINT lFoot
							JOINT toe1-1.L
								JOINT toe1-2.L
							JOINT toe2-1.L
								JOINT toe2-2.L
									JOINT toe2-3.L
							JOINT toe3-1.L
								JOINT toe3-2.L
									JOINT toe3-3.L
							JOINT toe4-1.L
								JOINT toe4-2.L
									JOINT toe4-3.L
							JOINT toe5-1.L
								JOINT toe5-2.L
									JOINT toe5-3.L

  * */

//cat human3D.conf | grep Bone | cut -d':' -f2  | tr '\n' '~' | tr -d '[:blank:]'  | sed  -e 's/\~/"\n\"/g' 
static const char * OpenCOLLADANames[] =
{
"Hips",
"Spine",
"Spine1",
"Neck",
"Neck1",
"Head",
"-",
"jaw",
"special04",
"oris02",
"oris01",
"oris06_L",
"oris07_L",
"oris06_R",
"oris07_R",
"tongue00",
"tongue01",
"tongue02",
"tongue03",
"tongue04",
"tongue07_L",
"tongue07_R",
"tongue06_L",
"tongue06_R",
"tongue05_L",
"tongue05_R",
"-",
"levator02_L",
"levator03_L",
"levator04_L",
"levator05_L",
"-",
"levator02_R",
"levator03_R",
"levator04_R",
"levator05_R",
"-",
"special01",
"oris04_L",
"oris03_L",
"oris04_R",
"oris03_R",
"oris06",
"oris05",
"-",
"special03",
"levator06_L",
"levator06_R",
"special06_L",
"special05_L",
"eye_L",
"orbicularis03_L",
"orbicularis04_L",
"special06_R",
"special05_R",
"eye_R",
"orbicularis03_R",
"orbicularis04_R",
"-",
"temporalis01_L",
"oculi02_L",
"oculi01_L",
"-",
"temporalis01_R",
"oculi02_R",
"oculi01_R",
"-",
"temporalis02_L",
"risorius02_L",
"risorius03_L",
"-",
"temporalis02_R",
"risorius02_R",
"risorius03_R",
"RightShoulder",
"RightArm",
"RightForeArm",
"RightHand",

"RThumb",
"finger1-2_R",
//"finger1-3_R",
//"metacarpal1_R",
"finger2-1_R",
"finger2-2_R",
//"finger2-3_R",
//"metacarpal2_R",
"finger3-1_R",
"finger3-2_R",
//"finger3-3_R",
//"metacarpal3_R",
"finger4-1_R",
"finger4-2_R",
//"finger4-3_R",
//"metacarpal4_R",
"finger5-1_R",
"finger5-2_R",
//"finger5-3_R",

"LeftShoulder",
"LeftArm",
"LeftForeArm",
"LeftHand",
"LThumb",
"finger1-2_L",
//"finger1-3_L",
//"metacarpal1_L",
"finger2-1_L",
"finger2-2_L",
//"finger2-3_L",
//"metacarpal2_L",
"finger3-1_L",
"finger3-2_L",
//"finger3-3_L",
//"metacarpal3_L",
"finger4-1_L",
"finger4-2_L",
//"finger4-3_L",
//"metacarpal4_L",
"finger5-1_L",
"finger5-2_L",
//"finger5-3_L",

//"LowerBack",

"-",
"RHipJoint",
"RightUpLeg",
"RightLeg",
"RightFoot",
//"RightToeBase",

"-",
"LHipJoint",
"LeftUpLeg",
"LeftLeg",
"LeftFoot",
//"LeftToeBase",
"END"
};



 
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
      /*
       controlOpenGLScene(
                           "human",
                           "position",
                           OPENGL_ACQUISITION_POSITION_XYZ,
                           0.1*bvhFrame[MOCAPNET_OUTPUT_HIP_XPOSITION],
                           -0.1*bvhFrame[MOCAPNET_OUTPUT_HIP_YPOSITION]-3.4, // I don't like this minus here..
                           -0.1*bvhFrame[MOCAPNET_OUTPUT_HIP_ZPOSITION]
                          );
  
       controlOpenGLScene(
                       "human",
                       "Hips",
                       OPENGL_ACQUISITION_JOINT_ROTATION_ZYX, //OPENGL_ACQUISITION_JOINT_ROTATION_XYZ <- this uses a different rotation scheme..
                       -1.0*bvhFrame[MOCAPNET_OUTPUT_HIP_XROTATION],
                       -1.0*bvhFrame[MOCAPNET_OUTPUT_HIP_YROTATION],
                       -1.0*bvhFrame[MOCAPNET_OUTPUT_HIP_ZROTATION]
                      );                            
       */
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
   
   
   //HEAD
    //------------------------------------------------------------- 
    controlOpenGLScene(
                       "oris01", "oris01", rotationOrder,
                       xDirection*bvhFrame[MOCAPNET_OUTPUT_ORIS01_XROTATION]+xAddition,
                       yDirection*bvhFrame[MOCAPNET_OUTPUT_ORIS01_YROTATION]+yAddition,
                       zDirection*bvhFrame[MOCAPNET_OUTPUT_ORIS01_ZROTATION]+zAddition
                      ); 
   
   
    return 1;
  #else
    return 0; 
  #endif    
}
