/*
 * Utility to extract BVH files straight from OpenPose JSON output
 * Sample usage ./MocapNETCSV  --from test.csv   --visualize
 */

#include "../MocapNETLib2/mocapnet2.hpp"
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "../MocapNETLib2/applicationLogic/parseCommandlineOptions.hpp"

#include "../MocapNETLib2/tools.hpp"
#include "../MocapNETLib2/visualization/map.hpp"
#include "../MocapNETLib2/visualization/drawHands.hpp"
#include "../MocapNETLib2/visualization/visualization.hpp"
#include "../MocapNETLib2/visualization/camera_ready.hpp"
//------------------------------------------
#include "../MocapNETLib2/IO/bvh.hpp"
#include "../MocapNETLib2/IO/csvRead.hpp"
#include "../MocapNETLib2/IO/csvWrite.hpp"
#include "../MocapNETLib2/IO/jsonRead.hpp"
#include "../MocapNETLib2/IO/jsonMocapNETHelpers.hpp"
#include "../MocapNETLib2/IO/conversions.hpp"
#include "../MocapNETLib2/IO/skeletonAbstraction.hpp"
//---------------------------------------------------

#include "../MocapNETLib2/solutionParts/leftHand.hpp"
#include "../MocapNETLib2/solutionParts/rightHand.hpp"

//--------------------------------------------------- 

#include "../../../dependencies/RGBDAcquisition/tools/Calibration/calibration.h"
#include "../../../dependencies/RGBDAcquisition/tools/AmMatrix/matrix4x4Tools.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/calculate/bvh_project.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/calculate/bvh_transform.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/ik/bvh_inverseKinematics.h"
#include "../../../dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/ik/hardcodedProblems_inverseKinematics.h"
 

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


unsigned int myMin(unsigned int a,unsigned int b)
{
  if (a<b) { return a; }
  return b;
}

//     ./HandOnlyTest --from /home/ammar/Documents/Datasets/RHD/test2D.csv --save --nofilter
//     ./HandOnlyTest --from /home/ammar/Documents/Datasets/STB/test2D.csv --save
//     ./HandOnlyTest --from /home/ammar/Documents/Programming/DNNTracker/DNNTracker/frames/GOPR3802.MP4-data/2dJoints_v1.4.csv 

int writeHTMLFile(char * path,unsigned int frameNumber)
{
    char filename[1025]={0};
     unsigned int batchSize = 100;
     unsigned int batches= frameNumber / batchSize;
     
     for (unsigned int batch=0; batch<batches; batch++)
     { 
      snprintf(filename,1024,"%s/results/index_%u.html",path,batch);
      FILE * html = fopen(filename,"w");
       if (html!=0)
       {
           unsigned int start=batch*batchSize;
           unsigned int finish = start + batchSize;
           if (finish>frameNumber) { finish=frameNumber; }
           
           fprintf(html,"<html><body>\n");

           if (batch>0) { fprintf(html,"<a href=\"index_%u.html\">Previous</a>",batch-1); }
           if (finish<frameNumber) { fprintf(html,"<a href=\"index_%u.html\">Next</a><br>",batch+1); }
             
           fprintf(html,"<table>\n");
            
           for (unsigned int i=start; i<finish; i++)
           {
               
            fprintf(html,"<tr>\n");
             fprintf(html,"<td>%u</td>",i);
             snprintf(filename,1024,"nsrm%u.png",i);
             fprintf(html,"<td><img width=128 src=\"%s\"></td>",filename);
             snprintf(filename,1024,"../images/im%u.png",i);
             fprintf(html,"<td><img width=128 src=\"%s\"></td>",filename);
             snprintf(filename,1024,"hand%u.png",i);
             fprintf(html,"<td><img width=128 src=\"%s\"></td>",filename);
             fprintf(html,"</tr>\n"); 
           }
           
           fprintf(html,"</table>\n");
           
           if (batch>0) { fprintf(html,"<br><br><a href=\"index_%u.html\">Previous</a>",batch-1); }
           if (finish<frameNumber) { fprintf(html,"<a href=\"index_%u.html\">Next</a><br>",batch+1); }
           
           fprintf(html,"</body></html>\n");
           fclose(html); 
           return 1;
       }
     }
  return 0;
}





 
int writeCSVOutput(const char * filename,struct BVH_MotionCapture * bvhMotion,unsigned int frameNumber,std::vector<float> points3D)
{
  FILE * fp;
  
  if (frameNumber==0)
  { 
      fp = fopen(filename,"w");
      if (fp!=0)
      {
        const char* jointName = bvhMotion->jointHierarchy[0].jointName;
        fprintf(fp,"3DX_%s,3DY_%s,3DZ_%s",jointName,jointName,jointName);
        for (unsigned int jID=1; jID<bvhMotion->jointHierarchySize; jID++)
        {
          jointName = bvhMotion->jointHierarchy[jID].jointName;
          fprintf(fp,",3DX_%s,3DY_%s,3DZ_%s",jointName,jointName,jointName); 
        }
        fprintf(fp,"\n");
        fclose(fp);
        return 1;
      }
  } else
  {
      fp = fopen(filename,"a");
      if (fp!=0)
      {
        if (points3D.size()>0)
        {
          fprintf(fp,"%0.2f",points3D[0]);
          for (unsigned int i=1; i<points3D.size(); i++)
          {
            fprintf(fp,",%0.2f",points3D[i]); 
          }
        fprintf(fp,"\n");
        }
        fclose(fp);
        return 1;
      }
  }
  return 0;
}


int writeSingleFloatInFile(char * path,float theNumberToWrite)
{
    char filename[1025]={0}; 
      snprintf(filename,1024,"%s/framerate",path);
      FILE * fp = fopen(filename,"w");
       if (fp!=0)
       {
        fprintf(fp,"%0.2f\n",theNumberToWrite);
        fclose(fp); 
        return 1;
       }
  return 0;
}



void drawLineIfPointsExist3(cv::Mat &outputMat,cv::Point start,cv::Point finish,cv::Scalar color,float thickness)
{
    if ( ( (start.x!=0) || (start.y!=0) ) && ( (finish.x!=0) || (finish.y!=0) ) )
        {
           cv::line(outputMat,start,finish,color,thickness); 
        }
   return;
}

void drawCircleIfPointsExist3(cv::Mat &outputMat,cv::Point point,float radious,cv::Scalar color,float thickness)
{
    if  ( (point.x!=0) || (point.y!=0) )
        {
           cv::circle(outputMat,point,radious,color,-thickness,8,0);
           
           if (color[0]<230 ) { color[0]+=25; }
           if (color[1]<230 ) { color[1]+=25; }
           if (color[2]<230 ) { color[2]+=25; }
           
           cv::circle(outputMat,point,radious/2,color,-thickness,8,0);
        }
   return;
}


void draw2DHandReprojectionError(const std::vector<float> points2DOriginal,const std::vector<std::vector<float> > points2DAchieved)
{
  cv::Mat image(1080,1920,CV_8UC3,cv::Scalar(0,0,0));
  
   float thickness=1.7;
   int fontUsed=cv::FONT_HERSHEY_SIMPLEX;
   cv::Scalar fontColor= cv::Scalar(255,255,255);
   char label[129]={0};
 
  for (unsigned int pointID=0; pointID<points2DOriginal.size()/3; pointID++)
  {
    float x = image.size().width  * points2DOriginal[pointID*3+0];
    float y = image.size().height * points2DOriginal[pointID*3+1];
    cv::Point pointToDraw(x,y);
    drawCircleIfPointsExist3(image,pointToDraw,5,cv::Scalar(255,0,255),1.0);
     
    snprintf(label,128,"%u(%s=%0.2f,%s=%0.2f)",pointID,
                       mocapNET_lhand[pointID*3+0],x,
                       mocapNET_lhand[pointID*3+1],y
                       //,mocapNET_lhand[pointID*3+2],points2DOriginal[pointID*3+2]
            ); //2D->3D 
    cv::Point  txtPosition(x+20,y);
    cv::putText(image,label,txtPosition,fontUsed,0.3,fontColor,thickness,8); 
  }
   
  
  for (unsigned int pointID=0; pointID<points2DAchieved.size(); pointID++)
  {
    float x = points2DAchieved[pointID][0];
    float y = points2DAchieved[pointID][1];
    cv::Point pointToDraw(x,y);
    drawCircleIfPointsExist3(image,pointToDraw,5,cv::Scalar(0,255,255),1.0);
    /*
    pointToDraw.x += 20;  
    snprintf(label,128,"%u(%s=%0.2f,%s=%0.2f)",pointID,
                       mocapNET_lhand[pointID*3+0],x,
                       mocapNET_lhand[pointID*3+1],y
                       //,mocapNET_lhand[pointID*3+2],points2DOriginal[pointID*3+2]
            ); //2D->3D
    cv::putText(image,label,pointToDraw,fontUsed,0.4,cv::Scalar(0,255,255),thickness,8);*/
  }


  cv::imshow("Disparity 2D",image);
}


enum STB_LOCATIONS
{
STB_frameNumber,   //0
STB_skeletonID,    //1 
STB_totalSkeletons,//2
STB_2DX_lhand,                   //3
STB_2DY_lhand,                   //4
STB_visible_lhand,               //5
STB_2DX_endsite_finger1_3_l,     //6
STB_2DY_endsite_finger1_3_l,     //7
STB_visible_endsite_finger1_3_l, //8
STB_2DX_finger1_3_l,             //9
STB_2DY_finger1_3_l,             //10
STB_visible_finger1_3_l,         //11
STB_2DX_finger1_2_l,             //12 
STB_2DY_finger1_2_l,             //13
STB_visible_finger1_2_l,         //14
STB_2DX_lthumb,                  //15
STB_2DY_lthumb,                  //16
STB_visible_lthumb,              //17 
STB_2DX_endsite_finger2_3_l,     //18
STB_2DY_endsite_finger2_3_l,     //19
STB_visible_endsite_finger2_3_l, //20
STB_2DX_finger2_3_l,         //
STB_2DY_finger2_3_l,         //
STB_visible_finger2_3_l,         //
STB_2DX_finger2_2_l,         //
STB_2DY_finger2_2_l,         //
STB_visible_finger2_2_l,         //
STB_2DX_finger2_1_l,         //
STB_2DY_finger2_1_l,         //
STB_visible_finger2_1_l,         //
STB_2DX_endsite_finger3_3_l,         //
STB_2DY_endsite_finger3_3_l,         //
STB_visible_endsite_finger3_3_l,         //
STB_2DX_finger3_3_l,         //
STB_2DY_finger3_3_l,         //
STB_visible_finger3_3_l,         //
STB_2DX_finger3_2_l,         //
STB_2DY_finger3_2_l,         //
STB_visible_finger3_2_l,         //
STB_2DX_finger3_1_l,         //
STB_2DY_finger3_1_l,         //
STB_visible_finger3_1_l,         //
STB_2DX_endsite_finger4_3_l,         //
STB_2DY_endsite_finger4_3_l,         //
STB_visible_endsite_finger4_3_l,         //
STB_2DX_finger4_3_l,         //
STB_2DY_finger4_3_l,         //
STB_visible_finger4_3_l,         //
STB_2DX_finger4_2_l,         //
STB_2DY_finger4_2_l,         //
STB_visible_finger4_2_l,         //
STB_2DX_finger4_1_l,         //
STB_2DY_finger4_1_l,         //
STB_visible_finger4_1_l,         //
STB_2DX_endsite_finger5_3_l,         //
STB_2DY_endsite_finger5_3_l,         //
STB_visible_endsite_finger5_3_l,         //
STB_2DX_finger5_3_l,         //
STB_2DY_finger5_3_l,         //
STB_visible_finger5_3_l,         //
STB_2DX_finger5_2_l,         //
STB_2DY_finger5_2_l,         //
STB_visible_finger5_2_l,         //
STB_2DX_finger5_1_l,         //
STB_2DY_finger5_1_l,         //
STB_visible_finger5_1_l         //
};



int extrapolateWristFromPalmAndFingerBases(struct skeletonSerialized * skeleton)
{
    float finger21X = skeleton->skeletonBody[STB_2DX_finger2_1_l].value;
    float finger21Y = skeleton->skeletonBody[STB_2DY_finger2_1_l].value;
    float finger31X = skeleton->skeletonBody[STB_2DX_finger3_1_l].value;
    float finger31Y = skeleton->skeletonBody[STB_2DY_finger3_1_l].value;
    float finger51X = skeleton->skeletonBody[STB_2DX_finger5_1_l].value;
    float finger51Y = skeleton->skeletonBody[STB_2DY_finger5_1_l].value;

    float palmX = skeleton->skeletonBody[STB_2DX_lhand].value;
    float palmY = skeleton->skeletonBody[STB_2DY_lhand].value;
    
    float fl2X = palmX + (palmX - finger21X);
    float fl2Y = palmY + (palmY - finger21Y);
    //---------------------------------------
    float fl3X = palmX + (palmX - finger31X);
    float fl3Y = palmY + (palmY - finger31Y);
    //---------------------------------------
    float fl5X = palmX + (palmX - finger51X);
    float fl5Y = palmY + (palmY - finger51Y);
    //---------------------------------------
    //x -0.01 Mean   :11.8946 
    //{-0.015,0.01}; // Mean 11.7829 
    //{-0.015,0.015}; Mean   :11.5735  
    //{-0.015,0.015}; ( 35 iterations ) Mean   :11.0499  
    //{-0.015,0.009}; ( 35 iterations ) Mean   :11.1  
    //{-0.012,-0.009};  Mean   :10.8625
    //{-0.012,-0.019}; Mean   :11.6612  
    //{0.012,-0.009}; Mean   :13.5712  
    //{-0.015,-0.009}; Mean   :10.8549
    //{-0.017,-0.009};Mean   :11.1785 
    //pointTuning[]={-0.017,-0.007}; Mean   :11.1785
    //{-0.013,-0.009}; Mean   :10.7158  
    // Change small finger..
    //={-0.013,-0.009} Mean   :10.50822 
    //Switch to 128 / 59B
    //{-0.014,-0.009}; Mean   :10.5741
    //{-0.014,0.009}; Mean   :10.727  
    //{-0.013,-0.009} Mean   :10.8166 
    //{-0.010,-0.009}; Mean   :10.99447 
    //{0.010,-0.009};Mean   :13.270 
    //{-0.014,-0.009}; Mean   :10.5741  

    
    float pointTuning[]={-0.014,-0.009};
    skeleton->skeletonBody[STB_2DX_lhand].value=pointTuning[0]+(fl2X+fl3X+fl5X)/3;
    skeleton->skeletonBody[STB_2DY_lhand].value=pointTuning[1]+(fl2Y+fl3Y+fl5Y)/3;
    return 1;
}

int doPalmWirstWorkaroundForSTB(struct skeletonSerialized * skeleton)
{
    /*
    for (unsigned int i=0; i<skeleton->skeletonHeaderElements; i++)
    {
      fprintf(stderr,"%u=%s ",i,skeleton->skeletonHeader[i].str);
    }
    */
    return extrapolateWristFromPalmAndFingerBases(skeleton);
}



int main(int argc, char *argv[])
{
    //----------------------------------------------
    struct MocapNET2Options options= {0};
    struct MocapNET2 mnet= {0};
    mnet.options = & options;
    //----------------------------------------------    
    
    //----------------------------------------------
    struct ikProblem * leftHandProblem = 0;
    struct ikProblem * rightHandProblem = 0;
    struct BVH_MotionCapture * bvhMotion;
    struct BVH_Transform bvhTransform  = {0};
    //Casting our solution/previous solution/penultimate solution in C arrays
    
    //----------------------------------------------
    
    //----------------------------------------------
    struct skeletonSerialized resultAsSkeletonSerialized= {0};
    //----------------------------------------------

    defaultMocapNET2Options(&options);
    options.GPUName[0]=0; //The CSV demo does not use the GPU so don't display it..
    
    
    std::vector<std::vector<float> > bvhFrames;
    std::vector<float> resultStandaloneBVH; 
    std::vector<float> penultimateResultStandaloneBVH;
    std::vector<float> previousResultStandaloneBVH;
    
    
    unsigned char STBWorkaroundNeeded=0;
    
    for (unsigned int i=0; i<argc; i++)
    {
        if (strcmp(argv[i],"--stb")==0)
        {
            fprintf(stderr,"Enabling STB palm workaround..\n");
            STBWorkaroundNeeded=1;
        }
    }
    
    unsigned int allHandsAreLeft=1;
    unsigned int allHandsAreRight=0;
    
   /*
    *  Force effortless IK configuration on CSV demo 
    */
    //Be unconstrained by default 
     options.constrainPositionRotation=0;
     //Use IK  ========
     options.useInverseKinematics=1;  //<-  CAREFUL this can be overriden later.>!
     options.learningRate=0.009;      //<-  CAREFUL this can be overriden later.>!
     options.iterations=35;//5;       //<-  CAREFUL this can be overriden later.>!
     options.epochs=30.0;             //<-  CAREFUL this can be overriden later.>!
     options.spring=0.0; //Deprecated ?
     //==============
    options.inputFramerate=60.0;
    options.filterCutoff=5.0;
    
    char enforceDepthLimit=0;
    float maximumDepth = -32;

    //640x480 should be a high framerate compatible resolution
    //for most webcams, you can change this using --size X Y commandline parameter
    options.width = 1920;
    options.height = 1080;

     float scaleInput=1.0;
     for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--scale2DInput")==0)
                {
                    if (argc>i+1)
                        {  
                          scaleInput=atof(argv[i+1]);  
                        }
                }
                
        }
     
    loadOptionsFromCommandlineOptions(&options,argc,argv);

    if (options.path==0)
        {
            fprintf(stderr,RED "No CSV dataset given\n " NORMAL);
            return 1;
        }


    if (options.label==0)
        {
            options. label="colorFrame_0_";
        }

    char filename[1025]={0};
    snprintf(filename,1024,"%s",options.path);




    struct CSVFileContext csv= {0};  
    if (!openCSVFile(&csv,filename))
                {
                    fprintf(stderr,RED "Unable to open CSV file %s \n" NORMAL,filename);
                    return 0;
                }
            else
                {
                    fprintf(stderr,GREEN "CSV file %s is now open\n" NORMAL,filename);
                    options.frameLimit=getBodyLinesOfCSVFIle(&csv,filename);
                    fprintf(stderr,GREEN "It has %u lines\n" NORMAL,options.frameLimit);
                }
     
     
            //-------------------------------------------------------
            int havePath=0;
            int positionOfSlash=0;
            for (int i=strlen(options.path); i>=0; i--)
                {
                    if (options.path[i]=='/')
                        {
                            positionOfSlash=i;
                            havePath=1;
                            break;
                        }
                }

            if (havePath)
                {
                    options.datasetPath=(char*) malloc(sizeof(char) * (positionOfSlash+2) );
                    if (options.datasetPath!=0)
                    {
                      for (int i=0; i<=positionOfSlash; i++)
                        {
                            options.datasetPath[i]=options.path[i];
                        }
                      options.datasetPath[positionOfSlash+1]=0; //Null termination
                    } 
                } 


    //----------------------------------------
    struct CSVFloatFileLine nextHandednessCSVLine;
    struct CSVFileContext handedness = {0};  
    char handednessFilename[1024]={0};
    snprintf(handednessFilename,1024,"%s/handSide.csv",options.datasetPath);
    fprintf(stderr,"hand Side file => %s \n",handednessFilename); 
    if (!openCSVFile(&handedness,handednessFilename))
                {
                    fprintf(stderr,RED "Unable to open CSV file %s \n" NORMAL,handednessFilename);
                    fprintf(stderr,YELLOW " allHandsAreLeft = %u \n" NORMAL,allHandsAreLeft);
                    fprintf(stderr,YELLOW " allHandsAreRight = %u \n" NORMAL,allHandsAreRight); 
                    //return 0;
                }
            else
                {
                    fprintf(stderr,GREEN "CSV file %s is now open\n" NORMAL,handednessFilename);
                    unsigned int numberOfHandednessResults = getBodyLinesOfCSVFIle(&csv,handednessFilename);
                    if (options.frameLimit!=numberOfHandednessResults) 
                    {
                        fprintf(stderr,RED "Mismatch of handedness file (%u lines) with 3D position file (%u lines)\n" NORMAL,numberOfHandednessResults,options.frameLimit);
                        exit(1);
                    } else
                    {
                        allHandsAreLeft=0;
                        allHandsAreRight=0;
                        fprintf(stderr,GREEN "It has %u lines\n" NORMAL,numberOfHandednessResults);
                    }
                } 
    //----------------------------------------





    loadCalibration(&options,options.datasetPath,"color.calib");


    if(options.visualize)
        {
          spawnVisualizationWindow("3D Points Output",options.visWidth,options.visHeight);
        }

   int useQuaternionHand=1;
   const char * handBVHFile = "dataset/lhand.qbvh"; 
   
   if (!initializeBVHConverter(handBVHFile,options.width,options.height,0) )
   {
       fprintf(stderr,"Could not open hand using %s \n",handBVHFile);
       return 0;
   }

   bvhMotion = (struct BVH_MotionCapture *) getBVHMotionHandle();

    struct simpleRenderer * renderer = (struct simpleRenderer *) getRendererHandle();

    //Try to replicate convertCSV3DToMocapNETInput
    //---------------------------------------------------
    float fX = 582.18394; //570.0
    float fY = 582.52915;  //570.0
    unsigned int width = 1920;
    unsigned int height = 1080;

    struct simpleRenderer localRenderer= {0};

    simpleRendererDefaults(
        &localRenderer,
        width,//1920
        height,//1080
        fX, //570.0
        fY  //570.0
    );

    simpleRendererInitialize(&localRenderer);
   //---------------------------------------------------
    renderer = &localRenderer;
    
    std::vector<float> resultRightHand;
    std::vector<float> resultLeftHand;
    
    
     struct MotionBuffer * solution              = mallocNewMotionBuffer((struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle());
     struct MotionBuffer * penultimateSolution   = mallocNewMotionBuffer((struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle());
     struct MotionBuffer * previousSolution      = mallocNewMotionBuffer((struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle());
                     
     rightHandProblem = allocateEmptyIKProblem();
     leftHandProblem  = allocateEmptyIKProblem();
     if ( (leftHandProblem==0) || (rightHandProblem==0) ) 
         { fprintf(stderr,"Failed to allocate memory for our problems..\n"); exit(0); }
    
    
    int ikFailures=0;
     if (!prepareDefaultRightHandProblem(
                                         rightHandProblem,
                                         (struct BVH_MotionCapture *) getBVHRHandQBVHMotionHandle(),
                                         renderer,
                                         previousSolution,
                                         solution,
                                         &bvhTransform,
                                         1//Standalone mode
                                        )
         )
         {
          fprintf(stderr,RED "MocapNET2/BVH: Could not initializeIK() for an IK solution for right hand\n" NORMAL);
          ++ikFailures; 
         }
    rightHandProblem->nearCutoffPlaneDeclared=1;
    rightHandProblem->nearCutoffPlane=5;  // cm 
         
     if (!prepareDefaultLeftHandProblem(
                                        leftHandProblem,
                                        (struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle(),
                                        renderer,
                                        previousSolution,
                                        solution,
                                        &bvhTransform,
                                        1//Standalone mode
                                       )
         )
         {
          fprintf(stderr,RED "MocapNET2/BVH: Could not initializeIK() for an IK solution for left hand\n" NORMAL);
          ++ikFailures; 
         }
    leftHandProblem->nearCutoffPlaneDeclared=1;
    leftHandProblem->nearCutoffPlane=5;   // cm 
    
   if (ikFailures>0)
   {
       fprintf(stderr,RED "Could not setup IK problems..\n" NORMAL);
       exit(0);
   }
   
   viewProblem(leftHandProblem);
   
   fprintf(stderr,"mocapNET hand initialization\n");
   //int result = mocapnetRightHand_initialize(&mnet," foo ",1,options.mocapNETMode,options.useCPUOnlyForMocapNET); 
   int result = mocapnetLeftHand_initialize(&mnet," foo ",1,options.mocapNETMode,options.useCPUOnlyForMocapNET); 
   //Just for the smoothing..
   commonMocapNETInitialization(&mnet);
   
   struct BVH_Transform bvhSkeletonTransform={0};
        
   long totalProcessingTime=0;
   unsigned int frameNumber = 0;
   if ( (result) && (bvh_allocateTransform(bvhMotion,&bvhSkeletonTransform)) )
   {
        struct skeletonSerialized skeleton = {0};
        
        while ( parseNextCSVCOCOSkeleton(&csv,&skeleton) )
                {
                    if (STBWorkaroundNeeded)
                    {
                      //Do STB Palm to wrist workaround..
                      doPalmWirstWorkaroundForSTB(&skeleton);
                    }
                    
                    if ( 
                         (options.addNormalizedPixelGaussianNoiseX!=0.0) ||
                         (options.addNormalizedPixelGaussianNoiseY!=0.0)
                       )
                         {
                             fprintf(stderr,RED "Adding Noise to input (%0.2f,%0.2f) assuming (%ux%u) frame..\n" NORMAL,options.addNormalizedPixelGaussianNoiseX,options.addNormalizedPixelGaussianNoiseY,width,height);
                             perturbSerializedSkeletonUsingGaussianNoise(
                                                                         &skeleton,
                                                                          (float) options.addNormalizedPixelGaussianNoiseX/width, 
                                                                          (float) options.addNormalizedPixelGaussianNoiseY/height
                                                                        );
                         }
                    
                    
                    unsigned int thisHandIsLeft=allHandsAreLeft;
                    if ( (!allHandsAreLeft) && (!allHandsAreRight) ) 
                         {
                             parseNextCSVFloatLine(&handedness,&nextHandednessCSVLine); 
                             thisHandIsLeft=nextHandednessCSVLine.field[0];
                             if (nextHandednessCSVLine.field[0] == nextHandednessCSVLine.field[1])
                             {
                                 fprintf(stderr,RED "Error inconsistend handedness..\n" NORMAL);
                                 exit(1);
                             }
                         }
                    
                    //Make sure associations are initialized on first frame..!
                    if (frameNumber==0)
                    {
                      if ( 
                           (!mocapnetLeftHand_initializeAssociations(&mnet,&skeleton)) ||
                           (!mocapnetRightHand_initializeUsingAssociationsOfLeftHand(&mnet,&skeleton))
                         )
                          {
                            fprintf(stderr,RED "\n");
                            fprintf(stderr,"___________________________________\n");
                            fprintf(stderr,"___________________________________\n");
                            fprintf(stderr,"\nFailed initializing associations..\n");
                            fprintf(stderr,"CSV: %s\n",filename);
                            fprintf(stderr,"___________________________________\n");
                            fprintf(stderr,"___________________________________\n" NORMAL);
                            //break;
                            exit(1);
                            //fprintf(stderr,RED "CONTINUING \n" NORMAL);
                          } else
                          {
                             //Signal that indexes are populated..
                             mnet.indexesPopulated=1;
                          }
                    }
                    //--------------------------------------------------------
                    
                    
                    long startTimeIK = GetTickCountMicrosecondsMN();
                    
                    mnet.orientation=MOCAPNET_ORIENTATION_FRONT;
                    
                    
                    
                    int executeNeuralNetworkOnThisFrame = ( 
                                                            (!mnet.options->skipNeuralNetworkIfItIsNotNeeded) || 
                                                            (frameNumber<2) ||
                                                            ( 
                                                               (mnet.options->skipNeuralNetworkIfItIsNotNeeded) &&
                                                               (frameNumber % mnet.options->maximumNeuralNetworkSkipFrames==0) 
                                                            )
                                                          );
                    
                    if (!executeNeuralNetworkOnThisFrame)
                    { 
                      fprintf(stderr,YELLOW "\n Neural Network execution skipped for frame %u ..\n" NORMAL ,frameNumber);
                    } else
                    {
                        if  (mnet.options->skipNeuralNetworkIfItIsNotNeeded)
                        {
                            fprintf(stderr,GREEN "\n Neural Network executed for frame %u ..\n" NORMAL ,frameNumber);
                        }
                    }
                        
                    if (thisHandIsLeft)
                    {
                     //std::vector<float> resultLeftHand = mocapnetLeftHand_evaluateInput(&mnet,&skeleton,scaleInput);
                     if (executeNeuralNetworkOnThisFrame)
                           { resultLeftHand = mocapnetLeftHand_evaluateInput(&mnet,&skeleton,scaleInput); }
                           
                     if (enforceDepthLimit)
                     {
                      if (resultLeftHand.size()>3)
                      {
                         fprintf(stderr,"LHand Depth %f\n",resultLeftHand[2]);
                         if(resultLeftHand[2]< maximumDepth )
                         {
                           resultLeftHand[2]= maximumDepth;
                           fprintf(stderr,"LHand FORCED %f\n",resultLeftHand[2]);
                         }
                      }  
                     }
                     
                     penultimateResultStandaloneBVH=previousResultStandaloneBVH;
                     previousResultStandaloneBVH=resultStandaloneBVH;
                       
                     mocapnetLeftHand_fillStandaloneResultVector(resultStandaloneBVH,resultLeftHand,useQuaternionHand);  
                    
                     convertStandaloneLHandSkeletonSerializedToBVHTransform(
                                                                           (struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle(),
                                                                           renderer,
                                                                           &bvhSkeletonTransform,
                                                                           &skeleton,
                                                                           0 //Already normalized..
                                                                          );
                    } else
                    {
                     //std::vector<float> resultRightHand = mocapnetRightHand_evaluateInput(&mnet,&skeleton,scaleInput);
                     if (executeNeuralNetworkOnThisFrame)
                           { resultRightHand = mocapnetRightHand_evaluateInput(&mnet,&skeleton,scaleInput); }
                           
                     if (enforceDepthLimit)
                     {
                       if (resultRightHand.size()>3)
                       {
                         fprintf(stderr,"RHand Depth %f\n",resultRightHand[2]);
                         if(resultRightHand[2]< maximumDepth)
                         {
                           resultRightHand[2]= maximumDepth;
                           fprintf(stderr,"RHand FORCED %f\n",resultRightHand[2]);
                         }
                       }
                     }
                    
                     penultimateResultStandaloneBVH=previousResultStandaloneBVH;
                     previousResultStandaloneBVH=resultStandaloneBVH;
                       
                     mocapnetRightHand_fillStandaloneResultVector(resultStandaloneBVH,resultRightHand,useQuaternionHand);  
                    
                     convertStandaloneRHandSkeletonSerializedToBVHTransform(
                                                                           (struct BVH_MotionCapture *) getBVHRHandQBVHMotionHandle(),
                                                                           renderer,
                                                                           &bvhSkeletonTransform,
                                                                           &skeleton,
                                                                           0 //Already normalized..
                                                                          );
                    } 
 

 // ------------------------------------------------------
 transferVectorToMotionBufferArray(solution,resultStandaloneBVH);
 // ------------------------------------------------------
 transferVectorToMotionBufferArray(previousSolution,previousResultStandaloneBVH);
 // ------------------------------------------------------
 transferVectorToMotionBufferArray(penultimateSolution,penultimateResultStandaloneBVH);
 // ------------------------------------------------------


                    struct ikConfiguration ikConfig = {0};
                    ikConfig.epochs=options.epochs;
                    ikConfig.learningRate=options.learningRate;
                    ikConfig.iterations=options.iterations;  
                    ikConfig.maximumAcceptableStartingLoss= 30000;//12000; //WARING < -  consider setting this to 0
                    ikConfig.gradientExplosionThreshold = 10;
                    ikConfig.spring=options.spring;
                    ikConfig.dontUseSolutionHistory = 0; // Dont thrash disk
                    ikConfig.dumpScreenshots = 0; // Dont thrash disk
                    ikConfig.verbose = 0; //Dont spam console
                    ikConfig.tryMaintainingLocalOptima=1; //Less Jittery but can be stuck at local optima
                    ikConfig.dontUseSolutionHistory=(mnet.options->doOutputFiltering==0);
                    ikConfig.ikVersion = IK_VERSION;

                    int multiThreading=0;
                    float initialMAEInPixels,finalMAEInPixels,initialMAEInMM,finalMAEInMM;
                    
                    if (options.useInverseKinematics)
                    {
                     //printIkConfiguration(&ikConfig);
                     unsigned int successfullyPerformedIKStep = 0;
                        
                        
                    if (thisHandIsLeft)
                    {
                        
                        if (  approximateBodyFromMotionBufferUsingInverseKinematics(
                                                                                    (struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle(),
                                                                                    renderer,
                                                                                    leftHandProblem,
                                                                                    &ikConfig,
                                                                                    //----------------
                                                                                    penultimateSolution,
                                                                                    previousSolution,
                                                                                    solution, 
                                                                                    0, //No ground truth..
                                                                                    //----------------
                                                                                    &bvhSkeletonTransform,
                                                                                   //----------------
                                                                                   multiThreading,// 0=single thread, 1=multi thread
                                                                                   //----------------
                                                                                   &initialMAEInPixels,
                                                                                   &finalMAEInPixels,
                                                                                   &initialMAEInMM,
                                                                                   &finalMAEInMM
                                                                                  )
                        )
                     { successfullyPerformedIKStep = 1; } 
                    } else
                    {
                       if (  approximateBodyFromMotionBufferUsingInverseKinematics(
                                                                                    (struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle(),
                                                                                    renderer,
                                                                                 rightHandProblem,
                                                                                 &ikConfig,
                                                                                 //----------------
                                                                                 penultimateSolution,
                                                                                 previousSolution,
                                                                                 solution, 
                                                                                 0, //No ground truth..
                                                                                 //----------------
                                                                                 &bvhSkeletonTransform,
                                                                                 //----------------
                                                                                 multiThreading,// 0=single thread, 1=multi thread
                                                                                 //----------------
                                                                                 &initialMAEInPixels,
                                                                                 &finalMAEInPixels,
                                                                                 &initialMAEInMM,
                                                                                 &finalMAEInMM
                                                                                )
                         )
                     { successfullyPerformedIKStep = 1; }
                    }
                        
                        
                    if ( successfullyPerformedIKStep )
                     {
                        if ( (solution!=0) && (solution->motion!=0) )
                        {
                          //If we performed inverse kinematics, then copy the output..
                          if (resultStandaloneBVH.size()>0)
                          {
                           if (resultStandaloneBVH.size()!=solution->bufferSize)
                            {
                             fprintf(stderr,RED "Mismatch on IK result (%u) vs standalone BVH result (%lu)..\n" NORMAL,solution->bufferSize,resultStandaloneBVH.size());
                            } 
                             else
                             { 
                                //We have just checked that resultStandaloneBVH and solution->motion have the same size..
                                for (unsigned int i=0; i<resultStandaloneBVH.size(); i++)
                                  { 
                                    resultStandaloneBVH[i] = solution->motion[i];
                                  }                                  
                             }
                          }
                        } else
                        {
                         fprintf(stderr,RED "Solution has no space for solution..\n" NORMAL);
                        }
                     } else
                     {
                      fprintf(stderr,RED "Failed executing IK..\n" NORMAL);
                     }
                        
                    }

 
            if (mnet.options->doOutputFiltering)
                {
                  //If we have a good fix we can use the results of the smoothing filter
                  if (resultStandaloneBVH.size()<MOCAPNET_OUTPUT_NUMBER)
                  {
                   for (unsigned int i=0; i<resultStandaloneBVH.size(); i++)
                                {
                                   resultStandaloneBVH[i] = filter(&mnet.outputSignals[i],resultStandaloneBVH[i]);
                                }
                  } else
                  {
                    fprintf(stderr,"Failed performing filtering..\n");
                  }
                }
 
                    bvhFrames.push_back(resultStandaloneBVH);
 
                    long endTimeIK = GetTickCountMicrosecondsMN();
                    
                    totalProcessingTime += endTimeIK - startTimeIK;
                    
                    //std::vector<std::vector<float> > points2D = convertStandaloneLHandBVHFrameTo2DPoints(resultStandaloneBVH);
                    //fprintf(stderr,"Draw hand should be expecting %u motion vectors\n",bvhMotion->numberOfValuesPerFrame);
                    //draw3DLhand(resultStandaloneBVH); 
                    
                    int foundImage = 0;
                    cv::Mat image;
                    cv::Mat croppedHand;
                    std::vector<std::vector<float> > points2D;
                    
                    if (thisHandIsLeft)
                    {
                      struct BVH_MotionCapture * bvhLHandMotion = (struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle(); 
                      points2D = convertStandaloneLHandBVHFrameTo2DPoints(resultStandaloneBVH); 
                      drawHand("3D Left Hand",bvhLHandMotion,points2D,resultStandaloneBVH,&croppedHand);
                    
                      std::vector<float> points3D = convertStandaloneLHandBVHFrameToFlat3DPoints(resultStandaloneBVH);
                      snprintf(filename,1024,"%s/mocapNETHand3DOutput.csv",options.datasetPath);
                      writeCSVOutput(filename,(struct BVH_MotionCapture *) getBVHLHandQBVHMotionHandle(),frameNumber,points3D);
                    } else
                    {
                      struct BVH_MotionCapture * bvhRHandMotion = (struct BVH_MotionCapture *) getBVHRHandQBVHMotionHandle(); 
                      points2D = convertStandaloneRHandBVHFrameTo2DPoints(resultStandaloneBVH); 
                      drawHand("3D Right Hand",bvhRHandMotion,points2D,resultStandaloneBVH,&croppedHand);
                    
                      std::vector<float> points3D = convertStandaloneRHandBVHFrameToFlat3DPoints(resultStandaloneBVH);
                      snprintf(filename,1024,"%s/mocapNETHand3DOutput.csv",options.datasetPath);
                      writeCSVOutput(filename,(struct BVH_MotionCapture *) getBVHRHandQBVHMotionHandle(),frameNumber,points3D);
                    }
                    
                    
                    snprintf(filename,1024,"%s/images/im%u.png",options.datasetPath,frameNumber);
                    if (fileExists(filename))
                       { 
                         image = imread(filename);
                         foundImage = 1;
                       } else
                       {
                         snprintf(filename,1024,"%s/colorFrame_0_%05u.jpg",options.datasetPath,frameNumber);
                         if (fileExists(filename))
                          { 
                           image = imread(filename);
                           foundImage = 1;
                          }
                       }
                       
                    if (!foundImage)
                    {
                        fprintf(stderr,"Could not find image (%s) ..\n",filename);
                        exit(0);
                    }
                    
                    
                    cv::Mat NSRMVisualization(200,200,CV_8UC3,cv::Scalar(0,0,0));
                    if (thisHandIsLeft)
                    { 
                     draw2DHandReprojectionError(mnet.leftHand.positionalInput,points2D);
                     draw2DHand(mnet.leftHand.positionalInput);  
                     visualizeNSDM(NSRMVisualization,"NSRM", mnet.leftHand.NSDM,1,0,0,200,200);
                     imshow("NSRM",NSRMVisualization);
                    } else
                    { 
                     draw2DHandReprojectionError(mnet.rightHand.positionalInput,points2D);
                     draw2DHand(mnet.rightHand.positionalInput);
                     visualizeNSDM(NSRMVisualization,"NSRM", mnet.rightHand.NSDM,1,0,0,200,200);
                     imshow("NSRM",NSRMVisualization);
                    }
                    
            if (mnet.options->saveVisualization)
            {
                    //Store Images.. 
                    snprintf(filename,1024,"%s/results/hand%u.png",options.datasetPath,frameNumber);
                    std::vector<int> compression_params;
                    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
                    compression_params.push_back(9);
                    cv::imwrite(filename,croppedHand,compression_params); 
                    
                    snprintf(filename,1024,"%s/results/nsrm%u.png",options.datasetPath,frameNumber);
                    cv::imwrite(filename,NSRMVisualization,compression_params); 
            }
          
                    imshow("Image",image);
                    cv::waitKey(10);
                    //fprintf(stderr,"Skeleton #%u received %u elements\n",frameNumber,skeleton.skeletonHeaderElements);
                    ++frameNumber;
                }
                
    
     bvh_freeTransform(&bvhSkeletonTransform);
     fprintf(stderr,"Done parsing CSV file..\n");
   } else
   {
     fprintf(stderr,"Failed initializing MocapNET\n");
   }
   
   
   
   //Spit an html file..
   if (mnet.options->saveVisualization)
   {
     writeHTMLFile(options.datasetPath,frameNumber); 
   }
   
   
    char targetBVHOutputFile[4096];
    snprintf(targetBVHOutputFile,4096,"%s/out.qbvh",options.datasetPath);
    int prependTPose=0;
    if ( writeBVHFile(targetBVHOutputFile,0,prependTPose,bvhFrames) )
                        {
                            fprintf(stderr,GREEN "Successfully wrote %lu frames to bvh file.. \n" NORMAL,bvhFrames.size());
                        }
                    else
                        {
                            fprintf(stderr,RED "Failed to write %lu frames to bvh file.. \n" NORMAL,bvhFrames.size());
                        }
   
   if (mnet.options->doOutputFiltering)
            {
                fprintf(stderr,"Output filtering was enabled..\n");
            } else
            {
                fprintf(stderr,"Output filtering was disabled..\n");
            }
            
   if (frameNumber>0)
   {
    float achievedFramerate = 1000000/(totalProcessingTime/frameNumber);  
    fprintf(stderr,"Achieved a %0.2f fps and %lu microseconds processing time for %u samples\n",achievedFramerate,totalProcessingTime,frameNumber);
    writeSingleFloatInFile(options.datasetPath,achievedFramerate);
   } else
   {
     fprintf(stderr,"No frames processed\n");
     return 1;
   }
 
   fprintf(stderr,"Done\n");
   exit(0);
   
   
   //These cause a corrupted size vs. prev_size 
   if (solution->motion!=0)            { free(solution->motion);             solution->motion=0;            solution->bufferSize = 0;            }
   if (previousSolution->motion!=0)    { free(previousSolution->motion);     previousSolution->motion=0;    previousSolution->bufferSize = 0;    }
   if (penultimateSolution->motion!=0) { free(penultimateSolution->motion);  penultimateSolution->motion=0; penultimateSolution->bufferSize = 0; }
   
   // bvh_freeTransform(&bvhTransform);
}
