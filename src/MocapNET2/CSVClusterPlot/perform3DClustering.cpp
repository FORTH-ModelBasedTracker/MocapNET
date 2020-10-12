#include "perform3DClustering.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../MocapNETLib2/IO/bvh.hpp"

#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_filter.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_rename.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_randomize.h"



#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

int appendBVHVectorToFile(FILE * fp, std::vector<float> vec)
{
  if ( (fp!=0) && (vec.size()>0) )
  {
      for (unsigned int i=0; i<vec.size(); i++)
      {
        if (i!=0) { fprintf(fp," "); }
        fprintf(fp,"%0.2f",vec[i]); 
      }
      fprintf(fp,"\n"); 
      return 1;
  }
  return 0;
}


std::vector<std::vector<float> > collect3DPoses(
    struct  clusteringOf3DPoses * state,
    std::vector<char>   &activeJoints,
    struct BVH_MotionCapture * bvhMotion,
    float distance,
    unsigned int width,
    unsigned int height,
    unsigned int depth
)
{
    std::vector<std::vector<float> > bvh2DPointsFront;
    std::vector<std::vector<float> > bvh2DPointsSide;  
    
    if (bvhMotion==0)
    {
        //Return an empty vector of vectors..
        return  bvh2DPointsFront;
    }
    
    if (state->fp==0)
    {
        state->fp = fopen("filteredPoses.bvhm","w"); 
    }
    
    if (state->space==0)
    {
        state->allocatedSpaceMemorySize = width * height * depth * sizeof(struct voxelElement); 
        fprintf(stderr,"Will now  try to allocate a huge chunk of memory (%lu bytes)\n",state->allocatedSpaceMemorySize);
        state->space = (struct voxelElement * ) malloc(state->allocatedSpaceMemorySize);
        if (state->space!=0)
        {
           fprintf(stderr,"Will now  try to cleanup the huge chunk of memory (%lu bytes)\n",state->allocatedSpaceMemorySize);
            memset(state->space,0,state->allocatedSpaceMemorySize);
           fprintf(stderr,"Survived..\n");
        } else
        {   
            state->allocatedSpaceMemorySize=0;
            fprintf(stderr,"Error allocating a huge chunk of memory..!\n");
            return bvh2DPointsFront;
        }
    }  
    
    std::vector<float> bvhConfiguration;

    bvhConfiguration.clear();
    for (int i=0; i<bvhMotion->numberOfValuesPerFrame; i++)
        {
            bvhConfiguration.push_back(0.0);
        }
    bvhConfiguration[2]=(float) -150.0;


    
    
    

    if (bvhMotion==0)
        {
            return bvh2DPointsFront;
        } 
        
    if ( bvhConfiguration.size() < bvhMotion->numberOfValuesPerFrame)
        {
            return bvh2DPointsFront;
        }

    unsigned int mID=0;

    fprintf(stderr,"Collecting %u poses\n",bvhMotion->numberOfFrames);

   // state->maxAccumulatedSample= 0;
   unsigned int negativesAtThisCall=0;
   unsigned int hitsAtThisCall=0;

    for (int frameID=0; frameID<bvhMotion->numberOfFrames; frameID++)
        {
            //fprintf(stderr,".%u",frameID);

            for (int i=0; i<bvhMotion->numberOfValuesPerFrame; i++)
                {
                    //fprintf(stderr,"%u ",i);
                    int motionValueID  = mID % (bvhMotion->numberOfValuesPerFrame * bvhMotion->numberOfFrames);
                    bvhConfiguration[i]=bvhMotion->motionValues[motionValueID];
                    ++mID;
                }
            //fprintf(stderr,"!");
               
                

            if (bvhConfiguration.size()>5)
                {
                    bvhConfiguration[0]=0;
                    bvhConfiguration[1]=0;
                    bvhConfiguration[2]=distance;
                    bvhConfiguration[3]=0;
                    bvhConfiguration[4]=0;
                    bvhConfiguration[5]=0;

                   
                    bvhConfiguration[4]=0;
                    bvh2DPointsFront = convertBVHFrameTo2DPoints(bvhConfiguration); //,width,height
                    bvhConfiguration[4]=90;
                    bvh2DPointsSide = convertBVHFrameTo2DPoints(bvhConfiguration); //,width,height
                    bvhConfiguration[4]=0; // So the configuration we save is ok 

                    if (  (bvh2DPointsFront.size()>0) && (bvh2DPointsSide.size()>0) && (bvh2DPointsFront.size()==bvh2DPointsSide.size()) )
                        { 
                            for (int i=0; i<bvh2DPointsFront.size(); i++)
                                {
                                    //We want to exclude Hip joint because it is stuck in the same place for every frame..! 
                                     if  ( 
                                             (activeJoints[i]) &&
                                             ( (bvh2DPointsFront[i][0]>0) && (bvh2DPointsFront[i][1]>0) && (bvh2DPointsFront[i][0]<width) && (bvh2DPointsFront[i][1]<height) ) &&
                                             ( (bvh2DPointsSide[i][0]>0) && (bvh2DPointsSide[i][1]>0) && (bvh2DPointsSide[i][0]<depth) && (bvh2DPointsSide[i][1]<height) ) 
                                           )
                                        {
                                            unsigned int x=(unsigned int) bvh2DPointsFront[i][0];
                                            unsigned int y=(unsigned int) bvh2DPointsFront[i][1];
                                            unsigned int z=(unsigned int) bvh2DPointsSide[i][0];
                                            
                                            //fprintf(stderr,"x=%u,y=%u,z=%u\n",x,y,z);
                                            
                                            if ( (x<width) && (y<height) && (z<depth) )
                                                {
                                                    unsigned long memoryLocation = (unsigned long) (z*width*height) + (y*width)+ x;
                                                    
                                                    if (state->allocatedSpaceMemorySize>memoryLocation)
                                                    {
                                                     if (state->space[memoryLocation].element==0)
                                                     {
                                                         ++state->accumulatedSamples;
                                                         appendBVHVectorToFile(state->fp, bvhConfiguration);
                                                         ++hitsAtThisCall;
                                                         state->space[memoryLocation].element=1;
                                                     } else
                                                     {
                                                         ++negativesAtThisCall;
                                                         if (state->space[memoryLocation].element<254)
                                                         {
                                                            state->space[memoryLocation].element+=1;
                                                         }
                                                     }
                                                     
                                                     
                                                     /*
                                                     if (newValue<=state->accumulatedSamples)
                                                        {
                                                            state->space[memoryLocation].element = newValue;

                                                            if (state->maxAccumulatedSample<newValue)
                                                                {
                                                                    state->maxAccumulatedSample=newValue;
                                                                    //fprintf(stderr,"Pixel %u,%u has largest value %lu\n",x,y,newValue);
                                                                }
                                                        } */
                                                    } else
                                                    {
                                                      fprintf(stderr,"Bad memory location for %u,%u,%u \n",x,y,z);
                                                    }
                                                        
                                                }
                                            else
                                                {
                                                    fprintf(stderr,"%0.2f,%0.2f wrongly casted to %u,%u",bvh2DPointsFront[i][0],bvh2DPointsFront[i][1],x,y);
                                                }

                                        }
                                }
                        }
                    else
                        {
                            fprintf(stderr,"Could not project BVH 2D points\n");
                        }
                }
        } //For every frame loop..
    
    if (hitsAtThisCall>0)
    {
        fprintf(stderr,GREEN "Added %u more poses (%0.2f%% of the dataset is novel)..\n" NORMAL,hitsAtThisCall,(float) (100*hitsAtThisCall)/bvhMotion->numberOfFrames);
    } else
    {
        fprintf(stderr,YELLOW "No new poses observed ( %u/%u negatives )..\n" NORMAL,negativesAtThisCall,bvhMotion->numberOfFrames);     
    }
   
    return bvh2DPointsFront;
}