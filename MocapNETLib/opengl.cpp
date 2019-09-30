#include <stdio.h>
#include "opengl.hpp"

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
                                  "dataset/cube.conf",
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