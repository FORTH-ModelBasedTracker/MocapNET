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
    char outputPathFull[2048]= {"./handToMnet.csv"};
    const char * outputPath=outputPathFull;

    float scale2DX=1.0,scale2DY=1.0;
    float offset2DX=0,offset2DY=0;

    float scale3DX=1.0,scale3DY=1.0,scale3DZ=-1.0;
    float offset3DX=0.0,offset3DY=0.0,offset3DZ=0.0;

    int flipX=0;
    int flipY=1;

    unsigned int thereIsALineLimit=0;
    unsigned int maxLinesToProcess=0;

    //  ./convertCSV3DToMocapNETInput --from /home/ammar/Documents/Datasets/STB/test.csv --to /home/ammar/Documents/Datasets/STB/test2D.csv --meters --flipY --offset3D 0 0 -100


    int i=0;
    while (i<argc)
        {
            if (strcmp(argv[i],"--from")==0)
                {
                    path = argv[i+1];
                    i+=1;
                }
            else if (strcmp(argv[i],"--to")==0)
                {
                    outputPath = argv[i+1];
                    i+=1;
                }
            else if (strcmp(argv[i],"--maxFrames")==0)
                {
                    thereIsALineLimit=1;
                    maxLinesToProcess=atoi(argv[i+1]);
                    i+=1;
                }
            else if (strcmp(argv[i],"--meters")==0)
                {
                    scale3DX = 1000.0;
                    scale3DY = 1000.0;
                    scale3DZ = 1000.0;
                }
            else if (strcmp(argv[i],"--scale3D")==0)
                {
                    scale3DX=atof(argv[i+1]);
                    scale3DY=atof(argv[i+2]);
                    scale3DZ=atof(argv[i+3]);
                    i+=3;
                }
            else if (strcmp(argv[i],"--offset3D")==0)
                {
                    offset3DX=atof(argv[i+1]);
                    offset3DY=atof(argv[i+2]);
                    offset3DZ=atof(argv[i+3]);
                    i+=3;
                }
            else if (strcmp(argv[i],"--scale2D")==0)
                {
                    scale2DX=atof(argv[i+1]);
                    scale2DY=atof(argv[i+2]);
                    i+=2;
                }
            else if (strcmp(argv[i],"--offset2D")==0)
                {
                    offset2DX=atof(argv[i+1]);
                    offset2DY=atof(argv[i+2]);
                    i+=2;
                }
            else if (strcmp(argv[i],"--flipX")==0)
                {
                    if (flipX)
                        {
                            flipX=0;
                        }
                    else
                        {
                            flipX=1;
                        }
                }
            else if (strcmp(argv[i],"--flipY")==0)
                {
                    if (flipY)
                        {
                            flipY=0;
                        }
                    else
                        {
                            flipY=1;
                        }
                }
            else
                {
                    fprintf(stderr,RED "Unknown parameter : %s \n" NORMAL,argv[i]);
                }
            ++i;
        }

    fprintf(stderr,"\n\n\n\nConversion from 3D CSV ground truth file configuration\n");
    fprintf(stderr,"_____________________________________________________________\n");
    fprintf(stderr,"3D input will have following modifiers : \n");
    fprintf(stderr," Scale  |  %0.2f | %0.2f | %0.2f \n",scale3DX,scale3DY,scale3DZ);
    fprintf(stderr," Offset |  %0.2f | %0.2f | %0.2f \n",offset3DX,offset3DY,offset3DZ);

    fprintf(stderr,"\nVirtual Camera : \n");
    fprintf(stderr," Width=%0.2f  Height=%0.2f\n",renderer.width,renderer.height);
    fprintf(stderr," fX=%0.2f  fY=%0.2f cX=%0.2f cY=%0.2f \n",renderer.fx,renderer.fy,renderer.cx,renderer.cy);

    fprintf(stderr,"\n2D output will have following modifiers : \n");
    fprintf(stderr," Scale  |  %0.2f | %0.2f  \n",scale2DX,scale2DY);
    fprintf(stderr," Offset |  %0.2f | %0.2f \n",offset2DX,offset2DY);

    fprintf(stderr,"\nFile I/O : \n");
    fprintf(stderr," 3D CSV ground truth file: `%s`\n",path);
    fprintf(stderr," 2D CSV output file: `%s`\n",outputPath);
    fprintf(stderr,"_____________________________________________________________\n");


    std::vector<std::vector<float> > result;
    std::vector<float> points2DOfASpecificFrame;

    struct CSVFileContext csv= {0};
    struct CSVFloatFileLine csvLine= {0};
    unsigned int linesProcessed=0;


    if ( openCSVFile(&csv,path) )
        {
            parseCSVHeader(&csv);

            fprintf(stderr,"%u columns of %u points to be outputted : ",csv.numberOfHeaderFields,(unsigned int) csv.numberOfHeaderFields/3);
            for (unsigned int pointID=0; pointID<csv.numberOfHeaderFields; pointID++)
                {
                    if (pointID!=0)
                        {
                            fprintf(stderr,",");
                        }

                    fprintf(stderr,"%s",csv.field[pointID].str);
                }
            fprintf(stderr,"\n_____________________________________________________________\n");

            while ( (parseNextCSVFloatLine(&csv,&csvLine)) && ( (!thereIsALineLimit) || (linesProcessed+1<maxLinesToProcess) ) )
                {
                    points2DOfASpecificFrame.clear();

                    float pos3D[4]= {0};
                    float position2DX=0.0,position2DY=0.0,position2DW=0.0;
                    float position2DOutputX=0.0,position2DOutputY=0.0;

                    float center[4]= {0};
                    float rotation[3]= {0};


                    for (unsigned int pointID=0; pointID<csv.numberOfHeaderFields/3; pointID++)
                        {
                            //--------------------------------------
                            pos3D[0]=(float) (scale3DX*csvLine.field[pointID * 3 + 0]) + offset3DX;
                            pos3D[1]=(float) (scale3DY*csvLine.field[pointID * 3 + 1]) + offset3DY;
                            pos3D[2]=(float) (scale3DZ*csvLine.field[pointID * 3 + 2]) + offset3DZ;
                            pos3D[3]=1.0;
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
                                    &position2DW)
                                )
                                {
                                    if (flipX) { position2DX = width - position2DX;  }
                                    if (flipY) { position2DY = height - position2DY; }

                                    position2DOutputX=0.0;
                                    position2DOutputY=0.0;

                                    if (position2DW>0.0)
                                        {
                                            //fprintf(stderr,"output is %0.2f,%0.2f \n",position2DX,position2DY);
                                            position2DOutputX = (scale2DX*position2DX) + offset2DX;
                                            position2DOutputY = (scale2DY*position2DY) + offset2DY;
                                        }
                                    else
                                        {
                                            //fprintf(stderr,"behind camera(%0.2f).. %0.2f,%0.2f \n",position2DW,position2DX,position2DY);
                                        }

                                    //We normalize our 2D output..!
                                    position2DOutputX=(float) position2DOutputX/renderer.width;
                                    position2DOutputY=(float) position2DOutputY/renderer.height;

                                    fprintf(stderr,"3D %0.2f,%0.2f,%0.2f => 2D %0.2f,%0.2f \n",pos3D[0],pos3D[1],pos3D[2],position2DOutputX,position2DOutputY);
                                    points2DOfASpecificFrame.push_back(position2DOutputX);
                                    points2DOfASpecificFrame.push_back(position2DOutputY);

                                }
                            else
                                {
                                    points2DOfASpecificFrame.push_back(0.0);
                                    points2DOfASpecificFrame.push_back(0.0);
                                }
                        } //For every point in this CSV line

                    result.push_back(points2DOfASpecificFrame);
                    ++linesProcessed;
                }//While we there are more lines to process


            if (result.size()>0)
                {
                    FILE * fp = fopen(outputPath,"w");
                    if (fp!=0)
                        {

                            fprintf(fp,"frameNumber,skeletonID,totalSkeletons");

                            for (unsigned int pointID=0; pointID<csv.numberOfHeaderFields/3; pointID++)
                                {
                                    char * pointLabel = csv.field[pointID*3].str + 4; //This is the fastest way to get rid of the `3D*_` part of the stream

                                    //Translate H36M labels to MNET labels..
                                    if ( strcmp(pointLabel,"nose")==0 )      { fprintf(fp,",2DX_head,2DY_head,visible_head");    } else 
                                    if ( strcmp(pointLabel,"rwrist")==0 )    { fprintf(fp,",2DX_rhand,2DY_rhand,visible_rhand"); } else 
                                    if ( strcmp(pointLabel,"lwrist")==0 )    { fprintf(fp,",2DX_lhand,2DY_lhand,visible_lhand"); } else
                                    if ( strcmp(pointLabel,"midhip")==0 )    { fprintf(fp,",2DX_hip,2DY_hip,visible_hip");       } else
                                    if ( strcmp(pointLabel,"rheel")==0 )     { fprintf(fp,",2DX_rfoot,2DY_rfoot,visible_rfoot"); } else //rankle/lankle instead..
                                    if ( strcmp(pointLabel,"lheel")==0 )     { fprintf(fp,",2DX_lfoot,2DY_lfoot,visible_lfoot"); } else 
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
                                            if ( pointID%2==0) { fprintf(fp,",%f",result[frameID][pointID]);   } else
                                                               { fprintf(fp,",%f,1",result[frameID][pointID]); }
                                        }
                                    fprintf(fp,"\n");
                                }
                            fclose(fp);
                        }
                }
            closeCSVFile(&csv);
        } //CSV file can be successfully opened
#else
    fprintf(stderr,"This utility is only available if you compile the BVH code in..\n");
#endif
    return 0;
}
