#include "bvh.hpp"

#if USE_BVH
 #include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
 #include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_project.h"
 #include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_transform.h"
 struct BVH_MotionCapture bvhMotion={0};
 struct BVH_Transform bvhTransform={0};
 int haveBVHInit=0;
#else
 #warning "BVH code not included.."
#endif // USE_BVH

int initializeBVHConverter()
{
 #if USE_BVH
   if ( bvh_loadBVH("dataset/headerAndOneMotion.bvh",&bvhMotion,1.0) )
   {
     haveBVHInit=1;
     return 1;
   } else
   {
      std::cerr<<"initializeBVHConverter: Failed to bvh_loadBVH(header.bvh)..\n";
   }
 #else
   std::cerr<<"initializeBVHConverter:  BVH code not compiled in..\n";
 #endif // USE_BVH
 return 0;
}

float * mallocVector(std::vector<float> bvhFrame)
{
  float * newVector = (float*) malloc(sizeof(float) * bvhFrame.size());
  if (newVector!=0)
  {
  for (int i=0; i<bvhFrame.size(); i++)
    {
      newVector[i]=bvhFrame[i];
    }
  }
  return newVector;
}


unsigned int getBVHParentJoint(unsigned int currentJoint)
{
 #if USE_BVH
  return bhv_getJointParent(&bvhMotion,currentJoint);
 #endif
 return 0;
}


const char * getBVHJointName(unsigned int currentJoint)
{
 #if USE_BVH
 if (bvhMotion.jointHierarchySize>currentJoint)
     {
       return bvhMotion.jointHierarchy[currentJoint].jointName;
     }
 #endif
   return 0;
}

std::vector<std::vector<float> > convertBVHFrameTo2DPoints(std::vector<float> bvhFrame,unsigned int width, unsigned int height)
{
 std::vector<std::vector<float> > result;
 #if USE_BVH
  struct simpleRenderer renderer={0};
  simpleRendererDefaults(
                          &renderer,
                           width,
                           height,
                           570.0,
                           570.0
                          );
  simpleRendererInitialize(&renderer);
  if (!haveBVHInit)
   {
     initializeBVHConverter();
   }


  if (haveBVHInit)
  {
    float * motionBuffer= mallocVector(bvhFrame);

    if (motionBuffer!=0)
    {
      if (
           bvh_loadTransformForMotionBuffer(
                                            &bvhMotion,
                                            motionBuffer,
                                            &bvhTransform
                                            )
          )
          { //-----------------
            if (
                 bvh_projectTo2D(
                                   &bvhMotion,
                                   &bvhTransform,
                                   &renderer,
                                   0,
                                   0
                                )
               )
               {//-----------------
                  for (unsigned int jID=0; jID<bvhMotion.jointHierarchySize; jID++)
                     {
                       std::vector<float> point;
                       point.clear();
                       point.push_back((float) bvhTransform.joint[jID].pos2D[0]);
                       point.push_back((float) bvhTransform.joint[jID].pos2D[1]);
                       result.push_back(point);
                     }
               } //-----------------
          } //-----------------
          else
          {
             std::cerr<<"bvh_loadTransformForMotionBuffer failed..\n";
          }
     free(motionBuffer);
    } else
    {
     std::cerr<<"Could not allocate enough memory..\n";
    }
  } else
  {
     std::cerr<<"Could not initialize BVH subsystem..\n";
  }


 #else
   std::cerr<<"BVH code is not compiled in this version of MocapNET\n";

 #endif // USE_BVH
 return result;
}
