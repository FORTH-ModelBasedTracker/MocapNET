/*
 * Export utility from OpenPose BODY 25 JSON format to a more regular CSV file
 * Sample call :   ./convertBody25JSONToCSV --from frames/GOPR3223.MP4-data/ --label colorFrame_0_ -o .
 * */

#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>

#include "../../../MocapNET2/MocapNETLib2/tools.hpp"
#include "../../../MocapNET2/MocapNETLib2/mocapnet2.hpp"
#include "../../../MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp"
#include "../../../MocapNET2/MocapNETLib2/IO/jsonRead.hpp"
#include "../../../MocapNET2/MocapNETLib2/IO/csvRead.hpp"
#include "../../../MocapNET2/MocapNETLib2/IO/csvWrite.hpp"


#if USE_BVH
#include "../../../../dependencies/RGBDAcquisition/tools/AmMatrix/matrix4x4Tools.h"
#include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
#include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/calculate/bvh_project.h"
#include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/calculate/bvh_transform.h"
#include "../../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/ik/bvh_inverseKinematics.h"
#else
#warning "BVH code not included.."
#endif // USE_BVH


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */



int main(int argc, char *argv[])
{
#if USE_BVH

    float fX = 582.18394; //570.0
    float fY = 582.52915;  //570.0 
    unsigned int width = 1920;
    unsigned int height = 1080;
    
    struct simpleRenderer renderer= {0};

    simpleRendererDefaults(
        &renderer,
        width,//1920
        height,//1080
        fX, //570.0
        fY  //570.0
    );
    
    simpleRendererInitialize(&renderer);

    const char * path=0;
    const char * label=0;
    char outputPathFull[2048]= {"./h36ToMnet.csv"};
    const char * outputPath=outputPathFull;

    float offsetX=0.0,offsetY=0.0,offsetZ=0.0;
    
    int flipX=0;
    int flipY=1;
    
    for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--from")==0)
                {
                    path = argv[i+1];
                }
            else if (strcmp(argv[i],"--to")==0)
                {
                    outputPath = argv[i+1];
                }
            else if (strcmp(argv[i],"--offset")==0)
                {
                    offsetX=atof(argv[i+1]);
                    offsetY=atof(argv[i+2]);
                    offsetZ=atof(argv[i+3]);
                }
            else if (strcmp(argv[i],"--flipX")==0)
                {
                    offsetX=atof(argv[i+1]);
                    offsetY=atof(argv[i+2]);
                    offsetZ=atof(argv[i+3]);
                }
        }

    std::vector<std::vector<float> > result;
    std::vector<float> points2DOfASpecificFrame;
    
    struct CSVFileContext csv= {0};
    struct CSVFloatFileLine csvLine= {0};
    if ( openCSVFile(&csv,path) )
        {
            parseCSVHeader(&csv);
            while (parseNextCSVFloatLine(&csv,&csvLine))
                {
                    points2DOfASpecificFrame.clear();
                     
                    float pos3D[4]= {0};
                    //pos3D[3]=1.0;
                    
                    float position2DX=0,position2DY=0,position2DW=0;

                    float center[4]= {0};
                    float rotation[3]= {0};
                    
                    float hipX = csvLine.field[9 * 3 + 0];
                    float hipY = csvLine.field[9 * 3 + 1];
                    float hipZ = csvLine.field[9 * 3 + 2];
                    
                    
                    for (unsigned int pointID=0; pointID<csv.numberOfHeaderFields/3; pointID++)
                        {
                            //--------------------------------------
                            pos3D[0]=(float) (csvLine.field[pointID * 3 + 0]) + offsetX;
                            pos3D[1]=(float) (csvLine.field[pointID * 3 + 1]) + offsetY;
                            pos3D[2]=(float) -1 * (csvLine.field[pointID * 3 + 2]) + offsetZ;
                            //fprintf(stderr,"Projecting %0.2f,%0.2f,%0.2f : ",pos3D[0],pos3D[1],pos3D[2]);
                            //--------------------------------------
                            if (
                                simpleRendererRender(
                                    &renderer,
                                    pos3D,
                                    center,
                                    rotation,
                                    ROTATION_ORDER_RPY,
                                    &position2DX,
                                    &position2DY,
                                    &position2DW
                                )
                               )
                                {
                                    if (flipX)
                                    {
                                       position2DX = width - position2DX; 
                                    }
                                    
                                    if (flipY)
                                    {
                                       position2DY = height - position2DY; 
                                    }
                                    
                                    
                                    if (position2DW>0.0)
                                        {
                                            //fprintf(stderr,"output is %0.2f,%0.2f \n",position2DX,position2DY);
                                            points2DOfASpecificFrame.push_back((float) position2DX);
                                            points2DOfASpecificFrame.push_back((float) position2DY);
                                        }
                                    else
                                        {
                                            //fprintf(stderr,"behind camera(%0.2f).. %0.2f,%0.2f \n",position2DW,position2DX,position2DY);
                                            points2DOfASpecificFrame.push_back(0.0);
                                            points2DOfASpecificFrame.push_back(0.0);
                                        }
                                } else
                                {
                                  points2DOfASpecificFrame.push_back(0.0);
                                  points2DOfASpecificFrame.push_back(0.0);
                                }
                        }
                    
                   result.push_back(points2DOfASpecificFrame);
                }
           
            
            if (result.size()>0)
             {
                 FILE * fp = fopen(outputPath,"w");
                 if (fp!=0)
                 {
                     
                     fprintf(fp,"frameNumber,skeletonID,totalSkeletons"); 
                              
                     for (unsigned int pointID=0; pointID<csv.numberOfHeaderFields/3; pointID++)
                        {
                            char * pointLabel = csv.field[pointID*3].str + 4;
                            
                            //Translate H36M labels to MNET labels..
                            if ( strcmp(pointLabel,"nose")==0 )      { fprintf(fp,",2DX_head,2DY_head,visible_head"); } else
                            if ( strcmp(pointLabel,"rwrist")==0 )    { fprintf(fp,",2DX_rhand,2DY_rhand,visible_rhand"); } else
                            if ( strcmp(pointLabel,"lwrist")==0 )    { fprintf(fp,",2DX_lhand,2DY_lhand,visible_lhand"); } else
                            if ( strcmp(pointLabel,"midhip")==0 )    { fprintf(fp,",2DX_hip,2DY_hip,visible_hip"); } else
                                //rankle/lankle instead..
                            if ( strcmp(pointLabel,"rheel")==0 )    { fprintf(fp,",2DX_rfoot,2DY_rfoot,visible_rfoot"); } else
                            if ( strcmp(pointLabel,"lheel")==0 )    { fprintf(fp,",2DX_lfoot,2DY_lfoot,visible_lfoot"); } else
                            if ( strcmp(pointLabel,"reye")==0 )      { fprintf(fp,",2DX_endsite_eye.r,2DY_endsite_eye.r,visible_endsite_eye.r"); } else
                            if ( strcmp(pointLabel,"leye")==0 )      { fprintf(fp,",2DX_endsite_eye.l,2DY_endsite_eye.l,visible_endsite_eye.l"); } else 
                            if ( strcmp(pointLabel,"rbigtoe")==0 )   { fprintf(fp,",2DX_endsite_toe1-2.r,2DY_endsite_toe1-2.r,visible_endsite_toe1-2.r"); } else
                            if ( strcmp(pointLabel,"lbigtoe")==0 )   { fprintf(fp,",2DX_endsite_toe1-2.l,2DY_endsite_toe1-2.l,visible_endsite_toe1-2.l"); } else 
                            if ( strcmp(pointLabel,"rsmalltoe")==0 ) { fprintf(fp,",2DX_endsite_toe5-3.r,2DY_endsite_toe5-3.r,visible_endsite_toe5-3.r"); } else
                            if ( strcmp(pointLabel,"lsmalltoe")==0 ) { fprintf(fp,",2DX_endsite_toe5-3.l,2DY_endsite_toe5-3.l,visible_endsite_toe5-3.l"); } else
                                                                     { fprintf(fp,",2DX_%s,2DY_%s,visible_%s",pointLabel,pointLabel,pointLabel); }
                                
                            
                        }
                     fprintf(fp,"\n"); 
                     
                     for (unsigned int frameID=0; frameID<result.size(); frameID++)
                     {
                        
                       fprintf(fp,"%u,0,0",frameID); 
                       for (unsigned int pointID=0; pointID<result[frameID].size(); pointID++)
                        { 
                           float normalizedPoint = result[frameID][pointID];
                           
                           unsigned int element = pointID%2;
                           
                           if (element==0) { normalizedPoint/= width;   fprintf(fp,",%f",normalizedPoint);  } else 
                                           { normalizedPoint/= height;  fprintf(fp,",%f,1",normalizedPoint);  } 
                                              
                        }
                        fprintf(fp,"\n");    
                     }
                     
                     
                     fclose(fp);
                 }
            
            
             } 
           
           
            closeCSVFile(&csv);
        }
        
        
        
        

#else
    fprintf(stderr,"This utility is only available if you compile the BVH code in..\n");
#endif
    return 0;
}
