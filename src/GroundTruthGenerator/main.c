/** @file main.c
 *  @brief  A minimal binary that parses BVH scenes using OGLRendererSandbox s
 *          X86 compilation: gcc -o -L/usr/X11/lib   main main.c -lGL -lX11 -lpng -ljpeg
 *          X64 compilation: gcc -o -L/usr/X11/lib64 main main.c -lGL -lX11 -lpng -ljpeg
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../../Library/TrajectoryParser/TrajectoryParserDataStructures.h"
#include "../../Library/MotionCaptureLoader/bvh_loader.h"
#include "../../Library/MotionCaptureLoader/bvh_to_tri_pose.h"

#include "../../Library/MotionCaptureLoader/export/bvh_to_trajectoryParserTRI.h"
#include "../../Library/MotionCaptureLoader/export/bvh_to_trajectoryParserPrimitives.h"
#include "../../Library/MotionCaptureLoader/export/bvh_export.h"
#include "../../Library/MotionCaptureLoader/export/bvh_to_bvh.h"

#include "../../Library/MotionCaptureLoader/edit/bvh_cut_paste.h"
#include "../../Library/MotionCaptureLoader/edit/bvh_randomize.h"
#include "../../Library/MotionCaptureLoader/edit/bvh_rename.h"
#include "../../Library/MotionCaptureLoader/edit/bvh_inverseKinematics.h"

#include  "../../../../../tools/AmMatrix/matrix4x4Tools.h"
#include  "../../../../../tools/AmMatrix/matrixOpenGL.h"


void prepareHuman36MRotationMatrix(float * rotationMatrix,float rX,float rY,float rZ)
{
    double rXM[9],rYM[9],rZM[9];
    double intermediateR[9];

    //R1x=np.matrix([[1,0,0],    [0,np.cos(Rx),-np.sin(Rx)], [0,np.sin(Rx),np.cos(Rx)] ]) #[1 0 0; 0 cos(obj.Params(1)) -sin(obj.Params(1)); 0 sin(obj.Params(1)) cos(obj.Params(1))]
    rXM[0]=1.0; rXM[1]=0.0;     rXM[2]=0.0;
    rXM[3]=0.0; rXM[4]=cos(rX); rXM[5]=-sin(rX);
    rXM[6]=0.0; rXM[7]=sin(rX); rXM[8]=cos(rX);

    //R1y=np.matrix([[np.cos(Ry),0,np.sin(Ry)], [0,1,0], [-np.sin(Ry),0,np.cos(Ry)]])     #[cos(obj.Params(2)) 0 sin(obj.Params(2)); 0 1 0; -sin(obj.Params(2)) 0 cos(obj.Params(2))]
    rYM[0]=cos(rY);  rYM[1]=0.0; rYM[2]=sin(rY);
    rYM[3]=0.0;      rYM[4]=1.0; rYM[5]=0.0;
    rYM[6]=-sin(rY); rYM[7]=0.0; rYM[8]=cos(rY);

    //R1z=np.matrix([[np.cos(Rz),-np.sin(Rz),0], [np.sin(Rz),np.cos(Rz),0], [0,0,1]])     #[cos(obj.Params(3)) -sin(obj.Params(3)) 0; sin(obj.Params(3)) cos(obj.Params(3)) 0; 0 0 1]
    rZM[0]=cos(rZ); rZM[1]=-sin(rZ); rZM[2]=0.0;
    rZM[3]=sin(rZ); rZM[4]=cos(rZ);  rZM[5]=0.0;
    rZM[6]=0.0;     rZM[7]=0.0;      rZM[8]=1.0;

    multiplyThree4x4Matrices(
                              intermediateR ,
                              rXM ,
                              rYM,
                              rZM
                            );

     copy4x4DMatrixToF(rotationMatrix,intermediateR);
}


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
    const char * toBVHFile="Motions/bvh.bvh";
    const char * toSceneFile="Scenes/bvh.conf";
    const char * toSceneFileTRI="Scenes/bvhTRI.conf";
    const char * toSVGDirectory="tmp/";
    const char * toCSVFilename="data.csv";
    unsigned int convertToSVG=0;
    unsigned int convertToCSV=0;
    unsigned int useCSV_2D_Output=1,useCSV_3D_Output=1,useCSV_BVH_Output=1;
    unsigned int maxFrames = 0;
    unsigned int occlusions = 0;
    float scaleWorld=1.0;
    unsigned int flipOrientation = 0;
    unsigned int flipRandomizationOrientation = 0;

    unsigned int filterBehindCamera=1;
    unsigned int filterIfAnyJointOutsideof2DFrame=1;
    unsigned int filterTopWeirdRandomSkeletons=1;

    struct BVH_MotionCapture bvhMotion={0};

    struct BVH_RendererConfiguration renderingConfiguration={0};

    // Emulate GoPro by default..
    // https://gopro.com/help/articles/Question_Answer/HERO4-Field-of-View-FOV-Information
    renderingConfiguration.width=1920;
    renderingConfiguration.height=1080;
    renderingConfiguration.cX=(float)renderingConfiguration.width/2;
    renderingConfiguration.cY=(float)renderingConfiguration.height/2;
    renderingConfiguration.fX=582.18394;
    renderingConfiguration.fY=582.52915;
    //640,480 , 575.57 , 575.57, //Kinect
    //-------------------------------------------------------------------------------------

    unsigned int i=0;
    for (i=0; i<argc; i++)
    {
        //-----------------------------------------------------
        if (strcmp(argv[i],"--nofilter")==0)
        {
          filterBehindCamera=0;
          filterIfAnyJointOutsideof2DFrame=0;
          filterTopWeirdRandomSkeletons=0;
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--renderingConfiguration")==0)
        {
          if (i+17>=argc)  { incorrectArguments(); }
          //-----------------------------------------
          float rX=atof(argv[i+1]);
          float rY=atof(argv[i+2]);
          float rZ=atof(argv[i+3]);
          renderingConfiguration.T[0]=atof(argv[i+4]);
          renderingConfiguration.T[1]=atof(argv[i+5]);
          renderingConfiguration.T[2]=atof(argv[i+6]);
          renderingConfiguration.fX=atof(argv[i+7]);
          renderingConfiguration.fY=atof(argv[i+8]);
          renderingConfiguration.cX=atof(argv[i+9]);
          renderingConfiguration.cY=atof(argv[i+10]);
          renderingConfiguration.k1=atof(argv[i+11]);
          renderingConfiguration.k2=atof(argv[i+12]);
          renderingConfiguration.k3=atof(argv[i+13]);
          renderingConfiguration.p1=atof(argv[i+14]);
          renderingConfiguration.p2=atof(argv[i+15]);
          unsigned int width=atoi(argv[i+16]);
          unsigned int height=atoi(argv[i+17]);
          //-------------------------------------------------------------------------------
          prepareHuman36MRotationMatrix(renderingConfiguration.R,rX,rY,rZ);
          copy3x3FMatrixTo4x4F(renderingConfiguration.viewMatrix,renderingConfiguration.R);

          // 0  1  2  3
          // 4  5  6  7
          // 8  9  10 11
          // 12 13 14 15
          //----------------------------------------------------------------
          renderingConfiguration.viewMatrix[3] =renderingConfiguration.T[0];
          renderingConfiguration.viewMatrix[7] =renderingConfiguration.T[1];
          renderingConfiguration.viewMatrix[11]=renderingConfiguration.T[2];
          //----------------------------------------
          renderingConfiguration.viewport[0]=0;
          renderingConfiguration.viewport[1]=0;
          renderingConfiguration.viewport[2]=width;
          renderingConfiguration.viewport[3]=height;
          renderingConfiguration.width=width;
          renderingConfiguration.height=height;
          //----------------------------------------
          buildOpenGLProjectionForIntrinsics(
                                             renderingConfiguration.projection ,
                                             renderingConfiguration.viewport ,
                                             renderingConfiguration.fX,
                                             renderingConfiguration.fY,
                                             1.0,//sr->skew,
                                             renderingConfiguration.cX,
                                             renderingConfiguration.cY,
                                             width,
                                             height,
                                             1.0, //Near
                                             10000.0 //Far
                                            );
          //----------------------------------------
          renderingConfiguration.isDefined=1;
          exit(0);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--test")==0)
        {
          bvh_testConstrainRotations();
          exit(0);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--occlusions")==0)
        {
          occlusions=1;
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--print")==0)
        {
          bvh_printBVH(&bvhMotion);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--scale")==0)
        {
          if (i+1>=argc)  { incorrectArguments(); }
          scaleWorld=atof(argv[i+1]);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--onlyFirstFrame")==0)
        {
          bvh_copyMotionFrame(&bvhMotion, 0, 1 );
          bvhMotion.numberOfFrames=2; //Just Render one frame..
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--maxFrames")==0)
        {
          if (i+1>=argc)  { incorrectArguments();}
          maxFrames=atoi(argv[i+1]);
          //We can limit the number of frames
          if (maxFrames!=0)
            {
             //Only reducing number of frames
             if (bvhMotion.numberOfFrames>maxFrames)
               {
                 bvhMotion.numberOfFrames = maxFrames;
               }
            }
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--from")==0)
        {
          if (i+1>=argc)  { incorrectArguments();}
          fromBVHFile=argv[i+1];
          //First of all we need to load the BVH file
          bvh_loadBVH(fromBVHFile, &bvhMotion, scaleWorld);
          //Change joint names..
          bvh_renameJointsForCompatibility(&bvhMotion);
          bvh_ConstrainRotations(&bvhMotion,flipOrientation);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--to")==0)
        {
          if (i+1>=argc)  { incorrectArguments(); }
          toSceneFile=argv[i+1];
           struct bvhToTRI bvhtri={0};
           bvh_loadBVHToTRI("Motions/cmu.profile",&bvhtri,&bvhMotion);
           dumpBVHToTrajectoryParserTRI(toSceneFileTRI,&bvhMotion,&bvhtri,1/*USE Irugubak oisutuib*/,0);
           dumpBVHToTrajectoryParserPrimitives(toSceneFile,&bvhMotion);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--bvh")==0)
        {
          if (i+1>=argc)  { incorrectArguments(); }
          toBVHFile=argv[i+1];
          dumpBVHToBVH(
                        toBVHFile,
                        &bvhMotion
                      );
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--csv")==0)
        {
          if (i+3>=argc)  { incorrectArguments(); }
          toSVGDirectory=argv[i+1];
          toCSVFilename=argv[i+2];
          convertToCSV=1;
          if (strcmp(argv[i+3],"2d+bvh")==0 ) { useCSV_2D_Output=1; useCSV_3D_Output=0; useCSV_BVH_Output=1; } else
          if (strcmp(argv[i+3],"2d")==0 )     { useCSV_2D_Output=1; useCSV_3D_Output=0; useCSV_BVH_Output=0; } else
          if (strcmp(argv[i+3],"3d")==0 )     { useCSV_2D_Output=0; useCSV_3D_Output=1; useCSV_BVH_Output=0; } else
          if (strcmp(argv[i+3],"bvh")==0 )    { useCSV_2D_Output=0; useCSV_3D_Output=0; useCSV_BVH_Output=1; } else
                                              { useCSV_2D_Output=1; useCSV_3D_Output=1; useCSV_BVH_Output=1; }
        } else
        //-----------------------------------------------------
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

        //-----------------------------------------------------
        if (strcmp(argv[i],"--swap")==0)
        {
          if (i+2>=argc)  { incorrectArguments(); }
          bvh_GrowMocapFileBySwappingJointAndItsChildren(
                                                          &bvhMotion,
                                                          argv[i+1],
                                                          argv[i+2],
                                                          0
                                                         );
        } else
        //-----------------------------------------------------


        //-----------------------------------------------------
        if (strcmp(argv[i],"--mirror")==0)
        {
          if (i+2>=argc)  { incorrectArguments(); }
          bvh_MirrorJointsThroughIK(
                                    &bvhMotion,
                                    argv[i+1],
                                    argv[i+2]
                                  );
        } else
        //-----------------------------------------------------

        //-----------------------------------------------------
        if (strcmp(argv[i],"--interpolate")==0)
        {
          bvh_InterpolateMotion(
                                &bvhMotion
                               );
        } else
        //-----------------------------------------------------

        //-----------------------------------------------------
        if (strcmp(argv[i],"--repeat")==0)
        {
          if (i+1>=argc)  { incorrectArguments(); }
          bvh_GrowMocapFileByCopyingExistingMotions(
                                                     &bvhMotion,
                                                     atoi(argv[i+1])
                                                   );
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--setPositionRotation")==0)
        {
          if (i+6>=argc)  { incorrectArguments(); }
          float cameraPositionOffset[3];
          float cameraRotationOffset[3];

          cameraPositionOffset[0]=-1*atof(argv[i+1])/10;
          cameraPositionOffset[1]=-1*atof(argv[i+2])/10;
          cameraPositionOffset[2]=-1*atof(argv[i+3])/10;
          cameraRotationOffset[0]=atof(argv[i+4]);
          cameraRotationOffset[1]=atof(argv[i+5]);
          cameraRotationOffset[2]=atof(argv[i+6]);
          bvh_SetPositionRotation(
                                  &bvhMotion,
                                  cameraPositionOffset,
                                  cameraRotationOffset
                                 );
          bvh_ConstrainRotations(&bvhMotion,flipOrientation);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--offsetPositionRotation")==0)
        {
          if (i+6>=argc)  { incorrectArguments(); }
          float cameraPositionOffset[3];
          float cameraRotationOffset[3];

          cameraPositionOffset[0]=-1*atof(argv[i+1])/10;
          cameraPositionOffset[1]=-1*atof(argv[i+2])/10;
          cameraPositionOffset[2]=-1*atof(argv[i+3])/10;
          cameraRotationOffset[0]=atof(argv[i+4]);
          cameraRotationOffset[1]=atof(argv[i+5]);
          cameraRotationOffset[2]=atof(argv[i+6]);
          bvh_OffsetPositionRotation(
                                     &bvhMotion,
                                     cameraPositionOffset,
                                     cameraRotationOffset
                                    );
          bvh_ConstrainRotations(&bvhMotion,flipOrientation);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--flipRandomizationOrientation")==0)
        {
             flipRandomizationOrientation=1;
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--perturbJointAngles")==0)
        {
          if (i+2>=argc)  { incorrectArguments(); }
          unsigned int numberOfValues=atoi(argv[i+1]);
          float deviation=atof(argv[i+2]);
          srand(time(NULL));
          if (i+2+numberOfValues>=argc)  { incorrectArguments(); } else
            {
              bvh_PerturbJointAngles(
                                     &bvhMotion,
                                     numberOfValues,
                                     deviation,
                                     argv,
                                     i+2
                                    );
            }
            //exit(0);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--testRandomizationLimits")==0)
        {
          // ./BVHTester --from Motions/02_03.bvh --testRandomizationLimits -1400 -300 1000 1400 300 5000 --svg tmp/
          if (i+6>=argc)  { incorrectArguments(); }
          float minimumPositionTest[3];
          float maximumPositionTest[3];
          //----
          minimumPositionTest[0]=-1*atof(argv[i+1])/10;
          minimumPositionTest[1]=-1*atof(argv[i+2])/10;
          minimumPositionTest[2]=-1*atof(argv[i+3])/10;
          //----
          maximumPositionTest[0]=-1*atof(argv[i+4])/10;
          maximumPositionTest[1]=-1*atof(argv[i+5])/10;
          maximumPositionTest[2]=-1*atof(argv[i+6])/10;

          bvh_TestRandomizationLimitsXYZ(
                                         &bvhMotion,
                                         minimumPositionTest,
                                         maximumPositionTest
                                        );

          bvh_ConstrainRotations(&bvhMotion,flipRandomizationOrientation);
        } else
         //-----------------------------------------------------
        if (strcmp(argv[i],"--randomize2D")==0)
        {
          // ./BVHTester --from Motions/02_03.bvh --randomize2D 1400 5000 -35 -90 -35 35 90 35 --occlusions --svg tmp/
          if (i+8>=argc)  { incorrectArguments(); }
          float minimumRotation[3];
          float maximumRotation[3];
          float minimumDepth=0.0;
          float maximumDepth=0.0;

          minimumDepth=-1*atof(argv[i+1])/10;
          maximumDepth=-1*atof(argv[i+2])/10;
          //----
          minimumRotation[0]=atof(argv[i+3]);
          minimumRotation[1]=atof(argv[i+4]);
          minimumRotation[2]=atof(argv[i+5]);
          //----
          maximumRotation[0]=atof(argv[i+6]);
          maximumRotation[1]=atof(argv[i+7]);
          maximumRotation[2]=atof(argv[i+8]);
          //----

          bvh_RandomizePositionFrom2D(
                                      &bvhMotion,
                                      minimumRotation,
                                      maximumRotation,
                                      minimumDepth,
                                      maximumDepth,
                                      renderingConfiguration.fX,
                                      renderingConfiguration.fY,
                                      renderingConfiguration.cX,
                                      renderingConfiguration.cY,
                                      renderingConfiguration.width,
                                      renderingConfiguration.height
                                     );

          bvh_ConstrainRotations(&bvhMotion,flipRandomizationOrientation);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--randomize")==0)
        {
          if (i+12>=argc)  { incorrectArguments(); }
          srand(time(NULL));

          float minimumPosition[3];
          float minimumRotation[3];
          float maximumPosition[3];
          float maximumRotation[3];

          //----
          minimumPosition[0]=-1*atof(argv[i+1])/10;
          minimumPosition[1]=-1*atof(argv[i+2])/10;
          minimumPosition[2]=-1*atof(argv[i+3])/10;
          //----
          minimumRotation[0]=atof(argv[i+4]);
          minimumRotation[1]=atof(argv[i+5]);
          minimumRotation[2]=atof(argv[i+6]);
          //----
          maximumPosition[0]=-1*atof(argv[i+7])/10;
          maximumPosition[1]=-1*atof(argv[i+8])/10;
          maximumPosition[2]=-1*atof(argv[i+9])/10;
          //----
          maximumRotation[0]=atof(argv[i+10]);
          maximumRotation[1]=atof(argv[i+11]);
          maximumRotation[2]=atof(argv[i+12]);


          bvh_RandomizePositionRotation(
                                         &bvhMotion,
                                         minimumPosition,
                                         minimumRotation,
                                         maximumPosition,
                                         maximumRotation
                                       );

          bvh_ConstrainRotations(&bvhMotion,flipRandomizationOrientation);
        } else
         //-----------------------------------------------------
        if (strcmp(argv[i],"--randomize2Dranges")==0)
        {
          // ./BVHTester --from Motions/02_03.bvh --randomize2Dranges 1400 5000 -35 -179.999999 -35 35 -90 35 -35 90 -35 35 180 35 --occlusions --svg tmp/
          if (i+14>=argc)  { incorrectArguments(); }
          float minimumRotationRangeA[3];
          float maximumRotationRangeA[3];
          float minimumRotationRangeB[3];
          float maximumRotationRangeB[3];
          float minimumDepth=0.0;
          float maximumDepth=0.0;

          minimumDepth=-1*atof(argv[i+1])/10;
          maximumDepth=-1*atof(argv[i+2])/10;
          //----
          minimumRotationRangeA[0]=atof(argv[i+3]);
          minimumRotationRangeA[1]=atof(argv[i+4]);
          minimumRotationRangeA[2]=atof(argv[i+5]);
          //----
          maximumRotationRangeA[0]=atof(argv[i+6]);
          maximumRotationRangeA[1]=atof(argv[i+7]);
          maximumRotationRangeA[2]=atof(argv[i+8]);
          //----
          minimumRotationRangeB[0]=atof(argv[i+9]);
          minimumRotationRangeB[1]=atof(argv[i+10]);
          minimumRotationRangeB[2]=atof(argv[i+11]);
          //----
          maximumRotationRangeB[0]=atof(argv[i+12]);
          maximumRotationRangeB[1]=atof(argv[i+13]);
          maximumRotationRangeB[2]=atof(argv[i+14]);
          //----

          bvh_RandomizePositionFrom2DRotation2Ranges(
                                      &bvhMotion,
                                      minimumRotationRangeA,
                                      maximumRotationRangeA,
                                      minimumRotationRangeB,
                                      maximumRotationRangeB,
                                      minimumDepth,
                                      maximumDepth,
                                      renderingConfiguration.fX,
                                      renderingConfiguration.fY,
                                      renderingConfiguration.cX,
                                      renderingConfiguration.cY,
                                      renderingConfiguration.width,
                                      renderingConfiguration.height
                                     );

          bvh_ConstrainRotations(&bvhMotion,flipRandomizationOrientation);
        } else
        //-----------------------------------------------------
        if (strcmp(argv[i],"--randomizeranges")==0)
        {
          if (i+24>=argc)  { incorrectArguments(); }
          srand(time(NULL));

          float minimumPositionRangeA[3];
          float minimumRotationRangeA[3];
          float maximumPositionRangeA[3];
          float maximumRotationRangeA[3];

          float minimumPositionRangeB[3];
          float minimumRotationRangeB[3];
          float maximumPositionRangeB[3];
          float maximumRotationRangeB[3];

          //----
          minimumPositionRangeA[0]=-1*atof(argv[i+1])/10;
          minimumPositionRangeA[1]=-1*atof(argv[i+2])/10;
          minimumPositionRangeA[2]=-1*atof(argv[i+3])/10;
          //----
          minimumRotationRangeA[0]=atof(argv[i+4]);
          minimumRotationRangeA[1]=atof(argv[i+5]);
          minimumRotationRangeA[2]=atof(argv[i+6]);
          //----
          maximumPositionRangeA[0]=-1*atof(argv[i+7])/10;
          maximumPositionRangeA[1]=-1*atof(argv[i+8])/10;
          maximumPositionRangeA[2]=-1*atof(argv[i+9])/10;
          //----
          maximumRotationRangeA[0]=atof(argv[i+10]);
          maximumRotationRangeA[1]=atof(argv[i+11]);
          maximumRotationRangeA[2]=atof(argv[i+12]);

          //----
          minimumPositionRangeB[0]=-1*atof(argv[i+13])/10;
          minimumPositionRangeB[1]=-1*atof(argv[i+14])/10;
          minimumPositionRangeB[2]=-1*atof(argv[i+15])/10;
          //----
          minimumRotationRangeB[0]=atof(argv[i+16]);
          minimumRotationRangeB[1]=atof(argv[i+17]);
          minimumRotationRangeB[2]=atof(argv[i+18]);
          //----
          maximumPositionRangeB[0]=-1*atof(argv[i+19])/10;
          maximumPositionRangeB[1]=-1*atof(argv[i+20])/10;
          maximumPositionRangeB[2]=-1*atof(argv[i+21])/10;
          //----
          maximumRotationRangeB[0]=atof(argv[i+22]);
          maximumRotationRangeB[1]=atof(argv[i+23]);
          maximumRotationRangeB[2]=atof(argv[i+24]);

          bvh_RandomizePositionRotation2Ranges(
                                                &bvhMotion,
                                                minimumPositionRangeA,
                                                minimumRotationRangeA,
                                                maximumPositionRangeA,
                                                maximumRotationRangeA,
                                                minimumPositionRangeB,
                                                minimumRotationRangeB,
                                                maximumPositionRangeB,
                                                maximumRotationRangeB
                                              );

          bvh_ConstrainRotations(&bvhMotion,flipRandomizationOrientation);
        }
    }



    //SVG or CSV output ..
    if ( (convertToSVG) || (convertToCSV) )
    {
     dumpBVHToSVGCSV(
                     toSVGDirectory,
                     toCSVFilename,
                     convertToSVG,
                     convertToCSV,useCSV_2D_Output,useCSV_3D_Output,useCSV_BVH_Output,
                     &bvhMotion,
                     &renderingConfiguration,
                     occlusions,
                     filterBehindCamera,//Filter out all poses where even one joint is behind camera
                     filterIfAnyJointOutsideof2DFrame,//Filter out all poses where even one joint is outside of 2D frame
                     filterTopWeirdRandomSkeletons,//Filter top left weird random skelingtons ( skeletons )
                     0//We don't want to convert to radians
                 );
      return 0;
    }


    bvh_free(&bvhMotion);

    return 0;
}
