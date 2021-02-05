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

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */



int main(int argc, char *argv[])
{
    struct MocapNET2Options options= {0};
    struct MocapNET2 mnet= {0};
    mnet.options = & options;

    struct skeletonSerialized resultAsSkeletonSerialized= {0};

    defaultMocapNET2Options(&options);
    options.GPUName[0]=0; //The CSV demo does not use the GPU so don't display it..


   /*
    *  Force effortless IK configuration on CSV demo
    */
    //Be unconstrained by default
     options.constrainPositionRotation=0;
     //Use IK  ========
     options.useInverseKinematics=1;
     options.learningRate=0.01;
     options.iterations=5;//5;
     options.epochs=30.0;
     options.spring=0.0; //Deprecated ?
     //==============

    //640x480 should be a high framerate compatible resolution
    //for most webcams, you can change this using --size X Y commandline parameter
    options.width = 1920;
    options.height = 1080;

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


    struct CSVFileContext csv= {0};
    if (!options.isCSVFile)
        {
            fprintf(stderr,RED "The path %s doesn't look like a CSV file.. \n" NORMAL,options.path);
            return 0;
        }
    else if (options.isCSVFile)
        {
            if (!openCSVFile(&csv,options.path))
                {
                    fprintf(stderr,RED "Unable to open CSV file %s \n" NORMAL,options.path);
                    return 0;
                }
            else
                {
                    fprintf(stderr,GREEN "CSV file %s is now open\n" NORMAL,options.path);
                    options.frameLimit=getBodyLinesOfCSVFIle(&csv,options.path);
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
        }


    loadCalibration(&options,options.datasetPath,"color.calib");


    if(options.visualize)
        {
          spawnVisualizationWindow("3D Points Output",options.visWidth,options.visHeight);
        }



    if ( loadMocapNET2(&mnet,"CSV Demo") )
        {
            //------------------------------
            mnet.learningRate=options.learningRate;
            mnet.iterations=options.iterations;
            mnet.epochs=options.epochs;
            mnet.spring=options.spring;
            //------------------------------


            char filePathOfImageFile[1024]= {0};
            if (options.datasetPath)
                {
                    char formatString[1024]= {0};
                    snprintf(formatString,1024,"%%s/%%s%%0%uu.jpg",options.serialLength);
                    snprintf(filePathOfImageFile,1024,formatString,options.datasetPath,options.label,1/*Frame ID*/);
                    // snprintf(filePathOfImageFile,1024,"%s/colorFrame_0_00001.jpg",datasetPath);

                    if ( getImageWidthHeight(filePathOfImageFile,&options.width,&options.height) )
                        {
                            fprintf(stderr,"Image dimensions changed from default to %ux%u\n",options.width,options.height);
                            //Force visualization size to the same as image dimensions autodetected..
                            options.visWidth  = options.width;
                            options.visHeight = options.height;
                        }
                    else
                        {
                            fprintf(stderr,"Assuming default image dimensions %ux%u , you can change this using --size x y\n",options.width,options.height);
                        }
                }


            //We might want to load a special bvh file based on our options..!
            loadOptionsAfterBVHLoadFromCommandlineOptions(&options,argc,argv);

            //If the initialization didnt happen inside the previous call lets do it now
            if (!options.hasInit)
            {
               if (initializeBVHConverter(0,options.visWidth,options.visHeight,0))
                 {
                   fprintf(stderr,"BVH code initalization successfull..\n");
                   options.hasInit=1;
                 }
            }
            //--------------------------------------------------------------------------

            std::vector<float>  result;
            std::vector<float>  previousResult;
            std::vector<std::vector<float> > output3DPositions;
            std::vector<float> points3DFlatOutput;

            float totalTime=0.0;
            unsigned int totalSamples=0;

            std::vector<std::vector<float> > bvhFrames;
            struct skeletonSerialized skeleton= {0};

            char formatString[1024]= {0};
            snprintf(formatString,1024,"%%s/%%s%%0%uu_keypoints.json",options.serialLength);

            unsigned int frameID=0;
            while ( (options.frameLimit==0) || (frameID<options.frameLimit) )
                {
                    if ( parseNextCSVCOCOSkeleton(&csv,&skeleton) )
                        {
                            takeCareOfScalingInputAndAddingNoiseAccordingToOptions(&options,&skeleton);

                            unsigned int feetAreMissing=areFeetMissing(&skeleton);

                            long startTime = GetTickCountMicrosecondsMN();
                            //--------------------------------------------------------
                            previousResult = result;
                            result = runMocapNET2(
                                         &mnet,
                                         &skeleton,
                                         ( (options.doLowerBody) && (!feetAreMissing) ),
                                         options.doHands,
                                         options.doFace,
                                         options.doGestureDetection,
                                         options.useInverseKinematics,
                                         options.doOutputFiltering);
                            bvhFrames.push_back(result);
                            //--------------------------------------------------------
                            long endTime = GetTickCountMicrosecondsMN();

                            //Convert BVH frame to 2D points to show on screen
                            std::vector<std::vector<float> >   exactMocapNET2DOutput = convertBVHFrameTo2DPoints(result);//,MocapNETTrainingWidth,MocapNETTrainingHeight);


                            if (options.saveCSV3DFile)
                                {
                                    //Convert BVH frame to 3D points to output on a file
                                    points3DFlatOutput=convertBVHFrameToFlat3DPoints(result);//,MocapNETTrainingWidth,MocapNETTrainingHeight);
                                    output3DPositions.push_back(points3DFlatOutput); //3d Input
                                }

                            resultAsSkeletonSerialized.skeletonHeaderElements = skeleton.skeletonHeaderElements;
                            resultAsSkeletonSerialized.skeletonBodyElements   = skeleton.skeletonBodyElements;
                            //exactMocapNET2DOutput = convertBVHFrameTo2DPoints(result,MocapNETTrainingWidth,MocapNETTrainingHeight);
                            if (
                                convertMocapNET2OutputToSkeletonSerialized(
                                    &mnet,
                                    &resultAsSkeletonSerialized,
                                    exactMocapNET2DOutput,
                                    frameID,
                                    MocapNETTrainingWidth,
                                    MocapNETTrainingHeight
                                )
                            )
                                {
                                    //TODO : Compare resultAsSkeletonSerialized and skeleton
                                    doReprojectionCheck(&skeleton,&resultAsSkeletonSerialized);
                                }




                            float sampleTime = (float) (endTime-startTime)/1000;
                            if (sampleTime==0.0)
                                {
                                    sampleTime=1.0;    //Take care of division by null..
                                }

                            options.totalLoopFPS = (float) 1000/sampleTime;
                            fprintf(stderr,"Sample %u - %0.4fms - %0.4f fps\n",frameID,sampleTime,options.totalLoopFPS);


                            if (options.visualize)
                                {
                                      visualizationCommander(
                                                             &mnet,
                                                             &options,
                                                             &skeleton,
                                                             0,
                                                             result,
                                                             exactMocapNET2DOutput,
                                                             frameID,
                                                             0 //Visualization code must handle messages..
                                                            );

                                }


                            totalTime+=sampleTime;
                            ++totalSamples;

                            if (options.delay!=0)
                                {
                                    fprintf(stderr,"Sleeping for %u milliseconds\n",options.delay);
                                    nsleep(options.delay*1000);
                                }

                        }
                    else
                        {
                            fprintf(stderr,"Done.. \n");
                            break;
                        }

                    ++frameID;
                }

            fprintf(stderr,"Finished with %u/%u frames\n",frameID,options.frameLimit);

            if (totalSamples>0)
                {
                    char * bvhHeaderToWrite=0;

                    if (options.bvhCenter)
                        {
                            for (unsigned int i=0; i<bvhFrames.size(); i++)
                                {
                                    bvhFrames[i][0]=0;
                                    bvhFrames[i][1]=0;
                                    bvhFrames[i][2]=0;
                                }
                        }

                    if (options.dontBend)
                       {
                         for (unsigned int i=0; i<bvhFrames.size(); i++)
                                {
                                   if (bvhFrames[i][3]>10)  { bvhFrames[i][3]=10; } else
                                   if (bvhFrames[i][3]<-10) { bvhFrames[i][3]=-10; }
                                }
                       }

                    //fix https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/35
                    fixBVHHip(bvhFrames);

                    if ( writeBVHFile(options.outputPath,bvhHeaderToWrite,options.prependTPose,bvhFrames) )
                        {
                            fprintf(stderr,"Successfully wrote %lu frames to bvh file %s.. \n",bvhFrames.size(),options.outputPath);
                        }
                    else
                        {
                            fprintf(stderr,"Failed to write %lu frames to bvh file %s .. \n",bvhFrames.size(),options.outputPath);
                        }

                    if (options.saveCSV3DFile)
                        {
                            fprintf(stderr,"Will now write 3D output to out3DP.csv.. \n");
                            //----------------------------------------------------------------------------------------------------------------------------------
                            if ( writeCSVHeaderFromLabelsAndVectorOfVectors("out3DP.csv",MocapNET3DPositionalOutputArrayNames,MOCAPNET_3DPOINT_NUMBER,output3DPositions) )
                                {
                                    fprintf(stderr,GREEN "Successfully wrote %lu frames to csv file.. \n" NORMAL,output3DPositions.size());
                                }
                            else
                                {
                                    fprintf(stderr,RED "Failed to write %lu frames to bvh file.. \n" NORMAL,output3DPositions.size());
                                }
                        }


                    float averageTime=(float) totalTime/totalSamples;
                    fprintf(stderr,"\n\nMocapNET v%s execution summary :\n",MocapNETVersion);
                    fprintf(stderr,"__________________________________________\n");
                    //neuralNetworkPrintVersion();
                    printBVHCodeVersion();
                    fprintf(stderr,"CPU : %s \n",options.CPUName);
                    fprintf(stderr,"GPU : %s \n",options.GPUName);
                    fprintf(stderr,"Total %0.2f ms for %u samples - Average %0.2f ms - %0.2f fps\n",totalTime,totalSamples,averageTime,(float) 1000/averageTime);
                    //-----------------------------------------------
                    if (options.doMultiThreadedIK)
                         { fprintf(stderr,"Multi-threading was on\n"); }
                    if (codeOptimizationsForIKEnabled())
                         { fprintf(stderr,"Code optimizations where on\n"); }
                }

            if (options.isCSVFile)
                {
                    closeCSVFile(&csv);
                }



            if (options.saveVisualization)
                {
                    int i;
                    //Low-Res video encoding
                    //int i=system("ffmpeg -framerate 25 -i vis%05d.jpg -y -r 30 -threads 8 -crf 9 -pix_fmt yuv420p  lastRun3D.webm");
                    //High-Res video encoding
                    snprintf(formatString,1024,"ffmpeg -framerate %f -i vis%%05d.jpg -s 1200x720 -y -r %f -pix_fmt yuv420p -threads 8 %s_lastRun3DHiRes.mp4 && rm ./vis*.jpg",options.inputFramerate,options.inputFramerate,options.path); //
                    i=system(formatString);
                    if (i==0)
                        {
                            fprintf(stderr,"Successfully wrote video file to %s_lastRun3DHiRes.mp4 .. \n",options.path);
                        }
                    else
                        {
                            fprintf(stderr,"Failed to write a video file.. \n");
                        }
                }

            unloadMocapNET2(&mnet);
        }
    else
        {
            fprintf(stderr,RED "MocapNET2 failed to load properly and will now exit..\n");
        }
}
