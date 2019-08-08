/** @file main.c
 *  @brief  A minimal binary that parses BVH scenes
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/TrajectoryParserDataStructures.h"
#include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
#include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_to_tri_pose.h"
#include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_trajectoryParserTRI.h"
#include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_trajectoryParserPrimitives.h"
#include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_export.h"
#include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_svg.h"
#include "../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_csv.h"

void testPrintout(struct BVH_MotionCapture * bvhMotion,const char * jointName)
{
    //Test getting rotations for a joint..
    BVHFrameID frameID = 0;
    BVHJointID jID=0;
    if ( bvh_getJointIDFromJointName(bvhMotion ,jointName,&jID) )
    {
      fprintf(stderr,"\nJoint %s (#%u) \n",bvhMotion->jointHierarchy[jID].jointName,jID);

      fprintf(
              stderr,"Channels ( %u,%u,%u )\n",
              bvhMotion->jointToMotionLookup[jID].channelIDMotionOffset[BVH_ROTATION_X],
              bvhMotion->jointToMotionLookup[jID].channelIDMotionOffset[BVH_ROTATION_Y],
              bvhMotion->jointToMotionLookup[jID].channelIDMotionOffset[BVH_ROTATION_Z]
             );

       for (frameID=0; frameID<bvhMotion->numberOfFrames; frameID++)
       {

         fprintf(stderr,"Frame %u \n",frameID);
         fprintf(stderr,"XRotation:%0.2f " ,bvh_getJointRotationXAtFrame(bvhMotion , jID ,  frameID));
         fprintf(stderr,"YRotation:%0.2f " ,bvh_getJointRotationYAtFrame(bvhMotion , jID ,  frameID));
         fprintf(stderr,"ZRotation:%0.2f\n",bvh_getJointRotationZAtFrame(bvhMotion , jID ,  frameID));
       }
    }
}

void incorrectArguments()
{
  fprintf(stderr,"Incorrect number of arguments.. \n");
  exit(1);
}


int main(int argc, char **argv)
{
    const char * fromBVHFile="Motions/example.bvh";
    const char * toSVGDirectory="tmp/";
    unsigned int convertToSVG=0;
    unsigned int convertToCSV=0;
    unsigned int iterations=1;
    unsigned int useOriginalPosition=0;

    float cameraPositionOffset[3]={0};
    float cameraRotationOffset[3]={0};
    float objectRotationOffset[3]={0};

    //-------------------------------------
    unsigned int randomizePoses=0;
    float minimumObjectPositionValue[3]={0};
    float maximumObjectPositionValue[3]={0};
    float minimumObjectRotationValue[3]={0};
    float maximumObjectRotationValue[3]={0};
    //-------------------------------------


    unsigned int i=0;
    for (i=0; i<argc; i++)
    {
        if (strcmp(argv[i],"--useOriginalPosition")==0)
        {
          useOriginalPosition=1;
        } else
        if (strcmp(argv[i],"--iterations")==0)
        {
          if (i+1>=argc)  { incorrectArguments();}
          iterations=atoi(argv[i+1]);
        } else
        if (strcmp(argv[i],"--from")==0)
        {
          if (i+1>=argc)  { incorrectArguments();}
          fromBVHFile=argv[i+1];
        } else
        if (strcmp(argv[i],"--csv")==0)
        {
          if (i+1>=argc)  { incorrectArguments(); }
          toSVGDirectory=argv[i+1];
          convertToCSV=1;
        } else
        if (strcmp(argv[i],"--svg")==0)
        {
          if (i+1>=argc)  { incorrectArguments(); }
          toSVGDirectory=argv[i+1];

          char removeOldSVGFilesCommand[512];
          snprintf(removeOldSVGFilesCommand,512,"rm %s/*.svg",toSVGDirectory);
          int res = system(removeOldSVGFilesCommand);
          if (res!=0) { fprintf(stderr,"Could not clean svg files in %s",toSVGDirectory); }
          convertToSVG=1;
        } else
        if (strcmp(argv[i],"--cameraPosition")==0)
        {
          if (i+3>=argc)  { incorrectArguments(); }
          cameraPositionOffset[0]=-1*atof(argv[i+1])/10;
          cameraPositionOffset[1]=atof(argv[i+2])/10;
          cameraPositionOffset[2]=atof(argv[i+3])/10;
        } else
        if (strcmp(argv[i],"--cameraRotation")==0)
        {
          if (i+3>=argc)  { incorrectArguments(); }
          cameraRotationOffset[0]=atof(argv[i+1]);
          cameraRotationOffset[1]=atof(argv[i+2]);
          cameraRotationOffset[2]=atof(argv[i+3]);
        } else
        if (strcmp(argv[i],"--objectRotation")==0)
        {
          if (i+3>=argc)  { incorrectArguments(); }
          objectRotationOffset[0]=atof(argv[i+1]);
          objectRotationOffset[1]=atof(argv[i+2]);
          objectRotationOffset[2]=atof(argv[i+3]);
        } else
        if (strcmp(argv[i],"--randomize")==0)
        {
          if (i+1>=argc)  { incorrectArguments();}

          randomizePoses=1;
          srand(time(NULL)); // randomize seed…

          //Minimum Values
          minimumObjectPositionValue[0]=atof(argv[i+1])/10;
          minimumObjectPositionValue[1]=atof(argv[i+2])/10;
          minimumObjectPositionValue[2]=atof(argv[i+3])/10;
          minimumObjectRotationValue[0]=atof(argv[i+4]);
          minimumObjectRotationValue[1]=atof(argv[i+5]);
          minimumObjectRotationValue[2]=atof(argv[i+6]);

          //Maximum Values
          maximumObjectPositionValue[0]=atof(argv[i+7])/10;
          maximumObjectPositionValue[1]=atof(argv[i+8])/10;
          maximumObjectPositionValue[2]=atof(argv[i+9])/10;
          maximumObjectRotationValue[0]=atof(argv[i+10]);
          maximumObjectRotationValue[1]=atof(argv[i+11]);
          maximumObjectRotationValue[2]=atof(argv[i+12]);

          fprintf(stderr,"Randomizing poses : \n");
          fprintf(
                  stderr,"Minimum Position/Rotation : %0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f \n",
                  minimumObjectPositionValue[0]*10,
                  minimumObjectPositionValue[1]*10,
                  minimumObjectPositionValue[2]*10,
                  minimumObjectRotationValue[0],
                  minimumObjectRotationValue[1],
                  minimumObjectRotationValue[2]
                 );
          fprintf(
                  stderr,"Maximum Position/Rotation : %0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f \n",
                  maximumObjectPositionValue[0]*10,
                  maximumObjectPositionValue[1]*10,
                  maximumObjectPositionValue[2]*10,
                  maximumObjectRotationValue[0],
                  maximumObjectRotationValue[1],
                  maximumObjectRotationValue[2]
                 );
        }
    }

    struct BVH_MotionCapture bvhMotion={0};

    //First of all we need to load the BVH file
    bvh_loadBVH(fromBVHFile, &bvhMotion, 1.0);

    //Change joint names..
    bvh_renameJointsForCompatibility(&bvhMotion);

    //SVG or CSV output ..
    if ( (convertToSVG) || (convertToCSV) )
    {
     unsigned int i=0;
     for (i=0; i<iterations; i++)
     {
      fprintf(stderr,"BVHFile:%s - Frames:%u - Iteration %u/%u\n",fromBVHFile,bvhMotion.numberOfFrames,i+1,iterations);
      dumpBVHToSVGCSV(
                      toSVGDirectory,
                      convertToSVG,
                      convertToCSV,
                      &bvhMotion,
                      640,
                      480,
                      useOriginalPosition,
                      cameraPositionOffset,
                      cameraRotationOffset,
                      objectRotationOffset,
                      randomizePoses,
                      minimumObjectPositionValue,
                      maximumObjectPositionValue,
                      minimumObjectRotationValue,
                      maximumObjectRotationValue,
                      1,//Filter out points where even one joint is behind camera
                      1,//Filter out points out of frame..
                      0//We don't want to convert to radians
                  );
     }
      return 0;
    }

    bvh_free(&bvhMotion);

    return 0;
}
