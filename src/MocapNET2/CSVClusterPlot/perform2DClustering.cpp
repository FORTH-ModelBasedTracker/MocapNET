#include "perform2DClustering.hpp"


#include "../MocapNETLib2/IO/bvh.hpp"

#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_filter.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_rename.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_randomize.h"



std::vector<std::vector<float> > collectPoses(
    struct applicationState * state,
    std::vector<char>   &activeJoints,
    struct BVH_MotionCapture * bvhMotion,
    float distance,
    unsigned int width,
    unsigned int height
)
{
    
    std::vector<std::vector<float> > bvh2DPoints; 
    
    if (bvhMotion==0)
        {
            return bvh2DPoints;
        } 
    
    
    std::vector<float> bvhConfiguration;

    bvhConfiguration.clear();
    for (int i=0; i<bvhMotion->numberOfValuesPerFrame; i++)
        {
            bvhConfiguration.push_back(0.0);
        }
    bvhConfiguration[2]=(float) -150.0;



    if ( bvhConfiguration.size() < bvhMotion->numberOfValuesPerFrame)
        {
            return bvh2DPoints;
        }

    unsigned int mID=0;

    fprintf(stderr,"Collecting %u poses\n",bvhMotion->numberOfFrames);

   // state->maxAccumulatedSample= 0;

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
                    bvhConfiguration[4]=state->rotation;
                    bvhConfiguration[5]=0;

                    bvh2DPoints = convertBVHFrameTo2DPoints(bvhConfiguration); //,width, height
                                  
                                  

                    if (bvh2DPoints.size()>0)
                        {
                            ++state->accumulatedSamples;
                            for (int i=0; i<bvh2DPoints.size(); i++)
                                {
                                    //We want to exclude Hip joint because it is stuck in the same place for every frame..!   
                                     if  (
                                             ( activeJoints[i] ) &&
                                             ( (bvh2DPoints[i][0]>0) && (bvh2DPoints[i][1]>0) && (bvh2DPoints[i][0]<width) && (bvh2DPoints[i][1]<height) )
                                           )
                                        {
                                            unsigned int x=(unsigned int) bvh2DPoints[i][0];
                                            unsigned int y=(unsigned int) bvh2DPoints[i][1];

                                            if ( (x<width) && (y<height) )
                                                {
                                                    unsigned long newValue = state->accumulatedImage[y*width+x] + 1;

                                                     if (newValue<=state->accumulatedSamples)
                                                        {
                                                            state->accumulatedImage[y*width+x] = newValue;

                                                            if (state->maxAccumulatedSample<newValue)
                                                                {
                                                                    state->maxAccumulatedSample=newValue;
                                                                    //fprintf(stderr,"Pixel %u,%u has largest value %lu\n",x,y,newValue);
                                                                }
                                                        }
                                                }
                                            else
                                                {
                                                    fprintf(stderr,"%0.2f,%0.2f wrongly casted to %u,%u",bvh2DPoints[i][0],bvh2DPoints[i][1],x,y);
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

    return bvh2DPoints;
}