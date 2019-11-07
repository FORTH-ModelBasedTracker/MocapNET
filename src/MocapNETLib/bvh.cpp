#include "bvh.hpp"
#include <string.h>

#if USE_BVH
#include "../../dependencies/RGBDAcquisition/tools/AmMatrix/matrix4x4Tools.h"
#include "../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
#include "../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_project.h"
#include "../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_transform.h"
struct BVH_MotionCapture bvhMotion= {0};
struct BVH_Transform bvhTransform= {0};
int haveBVHInit=0;
#else
#warning "BVH code not included.."
#endif // USE_BVH



const float fX = 582.18394; //570.0
const float fY = 582.52915;  //570.0



int writeBVHFile(
                                      const char * filename,
                                      const char * header,
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
            fprintf(fp,"Frames: %lu \n",bvhFrames.size());
            fprintf(fp,"Frame Time: 0.04\n");

            unsigned int i=0,j=0;
            for (i=0; i< bvhFrames.size(); i++)
                {
                    std::vector<float> frame = bvhFrames[i];
                    //fprintf(fp,"%lu joints",frame.size());

                    //fprintf(fp,"0.0 0.0 0.0 ");
                    for (j=0; j<frame.size(); j++)
                        {
                            fprintf(fp,"%0.4f ",frame[j]);
                        }
                    fprintf(fp,"\n");
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
    memset(newBVHLoadedFile,0,sizeof(struct BVH_MotionCapture));
    
    if (newBVHLoadedFile!=0)
    {
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



int initializeBVHConverter()
{
#if USE_BVH
    if ( bvh_loadBVH("dataset/headerAndOneMotion.bvh",&bvhMotion,1.0) )
        {
            haveBVHInit=1;
            return 1;
        }
    else
        {
            fprintf(stderr,"initializeBVHConverter: Failed to bvh_loadBVH(header.bvh)..\n");
        }
#else
    fprintf(stderr,"initializeBVHConverter:  BVH code not compiled in..\n");
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


std::vector<std::vector<float> > convertBVHFrameTo2DPoints(std::vector<float> bvhFrame,unsigned int width, unsigned int height)
{

    /*
        renderingConfiguration.width=1920;
    renderingConfiguration.height=1080;
    renderingConfiguration.cX=(float)renderingConfiguration.width/2;
    renderingConfiguration.cY=(float)renderingConfiguration.height/2;
    renderingConfiguration.fX=582.18394;
    renderingConfiguration.fY=582.52915;
    */

    std::vector<std::vector<float> > result;
#if USE_BVH
    struct simpleRenderer renderer= {0};
    simpleRendererDefaults(
        &renderer,
        width,
        height,
        fX, //570.0
        fY  //570.0
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
                        {
                            //-----------------
                            if (
                                   bvh_projectTo2D(
                                                                          &bvhMotion,
                                                                           &bvhTransform,
                                                                           &renderer,
                                                                           0,
                                                                           0
                                                                        )
                               )
                                {
                                    //-----------------
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




std::vector<std::vector<float> > convert3DGridTo2DPoints(float roll,float pitch,float yaw,unsigned int width, unsigned int height,unsigned int dimensions)
{
    std::vector<std::vector<float> > result;
#if USE_BVH
    struct simpleRenderer renderer= {0};
    simpleRendererDefaults(
        &renderer,
        width,
        height,
        fX,
        fY
    );
    simpleRendererInitialize(&renderer);


    std::vector<float> emptyPoint;
    emptyPoint.clear();
    emptyPoint.push_back((float) 0.0);
    emptyPoint.push_back((float) 0.0);


    float center[4]= {0};
    float rotation[3];

    rotation[0]=roll;
    rotation[1]=yaw;
    rotation[2]=pitch;


    float pos3D[4]= {0};

    signed int x,y,z;
    float position2DX,position2DY,position2DW;

    y=-5;
    unsigned int halfDimension = (unsigned int) dimensions / 2;
    signed int negativeStart=(signed int) -1 * halfDimension;
    signed int positiveEnd=(signed int) halfDimension;

    for (z=negativeStart; z<positiveEnd; z++)
        {
            //for (y=-negativeStart; y<positiveEnd; y++)
            {
                for (x=negativeStart; x<positiveEnd; x++)
                    {
                        pos3D[0]=x;
                        pos3D[1]=y;
                        pos3D[2]=z;

                        if (
                            simpleRendererRender(
                                &renderer ,
                                pos3D,
                                center,
                                rotation,
                                ROTATION_ORDER_RPY,
                                &center[0],
                                &center[1],
                                &center[2],
                                &position2DX,
                                &position2DY,
                                &position2DW
                            )
                        )
                            {
                                if (position2DW>0.0)
                                    {
                                        std::vector<float> point;
                                        point.clear();
                                        point.push_back((float) position2DX);
                                        point.push_back((float) position2DY);
                                        result.push_back(point);
                                    }
                                else
                                    {
                                        result.push_back(emptyPoint);
                                    }
                            }
                        else
                            {
                                result.push_back(emptyPoint);
                            }
                    }
            }
        }




#else
    fprintf(stderr,"BVH code is not compiled in this version of MocapNET\n");
#endif // USE_BVH
    return result;
}
