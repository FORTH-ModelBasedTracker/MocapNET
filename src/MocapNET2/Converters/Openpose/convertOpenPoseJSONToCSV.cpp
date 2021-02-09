 /*
 * Export utility from OpenPose BODY25 + Hands + Face JSON format to a trivially parseable CSV file
 * Check https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/output.md on more information on OpenPose JSON files..
 * 
 * Sample call :   ./convertOpenPoseJSONToCSV --from frames/GOPR3223.MP4-data/ --label colorFrame_0_ -o .
 *
 *   This utility converts OpenPose json output to a CSV file that can be easily parsed and used as MocapNET input
 * 
 * My workflow when working with OpenPose is first dumping any video file that I want to process (let's call it video.mp4) to images using ffmpeg.
 * This ensures a well defined framerate and a well defined 1:1 correspondence of color/json/and my output frames regardless of video container.
 *
 *  I typically use this script -> https://github.com/AmmarkoV/RGBDAcquisition/blob/master/grabber/frames/dump_video.sh to dump the video to files
 *  but it is equivalent to running :
 *                       mkdir videoFiles && ffmpeg -i video.mp4 -r 30 -q:v 1  videoFiles/colorFrame_0_%05d.jpg && cp videoFiles/colorFrame_0_00001.jpg videoFiles/colorFrame_0_00000.jpg
 *  
 *  Having a well defined serialized version of the dataset ( with serial numbers that start from 0 instead of 1 ) we can be sure on what frame each openpose output will correspond
 *  
 *  Running : 
 *                openpose.bin -number_people_max 1 --hand --face --write_json videoFiles/ -image_dir videoFiles/
 *  
 *  The directory will be populated with colorFrame_0_xxxxx_keypoints.json files
 *  each of the colorFrame_0_xxxxx_keypoints.json will correspond to a  colorFrame_0_xxxxx.jpg
 *
 *  This can then be converted to a CSV file using : 
 *                 ./convertOpenPoseJSONToCSV --from videoFiles/ 
 *
 *
 *  If you want to use a custom json labeling scheme and not rely on images as OpenPose input you will need to :
 *
 *  1) provide the proper image resolution ( since the JSON files alone do not have this information ) 
 *     The image resolution is *CRUCIAL* for the software to correctly normalize input 2D points and respect the training aspect ratio
 *     if you don't supply the correct size a Full-HD(1920x1080p) input will be assumed. If this does not correspond to the original video resolution output will be adversly affected       
 *     You can do this with the --size WIDTH HEIGHT parameter
 *
 *  2) Depending on your datasets and filenames supply the proper number of characters for a valid serial frame number length
 *     i.e.   frame_xxxxx_keypoints.json needs --seriallength 5
 *            frame_xxxxxxx_keypoints.json needs --seriallength 7 
 *            etc  
 *
 *  3) Depending on your datasets and filenames supply the correct label for the dataset 
 *     i.e.   datasetNameA_xxxxx_keypoints.json needs --label datasetNameA
 *            AMoreComplexLabel_IsTHIS_0_xxxxxxx_keypoints.json needs --label  AMoreComplexLabel_IsTHIS_0 
 *            etc
 *
 *  4) Depending on the starting frame and ending frame of your dataset supply the --startAt X , --maxFrames X 
 *     i.e   dataset_00001_keypoints.json - dataset_00101_keypoints.json should need --startAt 1 --maxFrames 102   
 *     i.e   dataset_00000_keypoints.json - dataset_0099_keypoints.json should need --startAt 0 --maxFrames 100   
 *
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

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


int  findFirstJSONFileInDirectory(const char * path,const char * formatString, const char * label, unsigned int * frameIDOutput)
{
    char filePathOfJSONFile[1025]={0};
    const int maximumFilesToSearchFor=1000;
    unsigned int frameID=0;
    int found=0;
    while (!found)
    {
          snprintf(filePathOfJSONFile,1024,formatString,path,label,frameID);
        
         if( fileExists(filePathOfJSONFile) )
         {
             *frameIDOutput=frameID;
             return 1;
         }
        
         ++frameID;
         if (frameID==maximumFilesToSearchFor) 
               { 
                   fprintf(stderr,"Stopping search after %u checks ..\n",maximumFilesToSearchFor); 
                   break;
                   //return 0; 
               }
   }
  fprintf(stderr,"findFirstJSONFileInDirectory: failed to find any JSON files.. :(\n");
  fprintf(stderr,"Path : %s \n",path);
  fprintf(stderr,"Format : %s \n",formatString);
  fprintf(stderr,"Label : %s \n",label); 
  return 0;   
}


int main(int argc, char *argv[])
{
    unsigned int respectTrainingAspectRatio = 1;
    unsigned int width=1920 , height=1080 , frameLimit=100000 , processed = 0 , serialLength = 5 ,forcedSize=0;
    const char * path=0;
    const char * label=0;
    char outputPathFull[2048]={0};
    const char * outputPath=0;
    float version=1.4;
    int noLabel=0;
    float acceptableThreshold = 0.5;

    for (int i=0; i<argc; i++)
        {
            if (strcmp(argv[i],"--maxFrames")==0)
                {
                    frameLimit=atoi(argv[i+1]);
                }
            else if (strcmp(argv[i],"--from")==0)
                {
                    path = argv[i+1];
                }
            else if (strcmp(argv[i],"--threshold")==0)
                {
                    acceptableThreshold = atof(argv[i+1]);
                }
            else if (strcmp(argv[i],"-i")==0)
                {
                    path = argv[i+1];
                }
            else if (strcmp(argv[i],"--label")==0)
                {
                    label = argv[i+1];
                }
            else if (strcmp(argv[i],"--out")==0)
                {
                    outputPath = argv[i+1];
                }
            else if (strcmp(argv[i],"--ignoreTrainingAspectRatio")==0)
                {
                    fprintf(stderr,RED "Training aspect ratio is going to be ignored during training..!\n" NORMAL);
                    fprintf(stderr,RED "this is not a good idea unless you really know what you are doing\n" NORMAL);
                    respectTrainingAspectRatio = 0;
                }
            else if (strcmp(argv[i],"-o")==0)
                {
                    outputPath = argv[i+1];
                }
            else if (strcmp(argv[i],"--to")==0)
                {
                    outputPath = argv[i+1];
                }
            else if (strcmp(argv[i],"-v")==0)
                {
                    version = atof(argv[i+1]);
                }
            else if (strcmp(argv[i],"--seriallength")==0)
                {
                    serialLength = atoi(argv[i+1]);
                }
            else if (strcmp(argv[i],"--size")==0)
                {
                    width = atoi(argv[i+1]);
                    height = atoi(argv[i+2]);
                    forcedSize=1;
                }
            else if (strcmp(argv[i],"--nolabel")==0)
                {
                    //New datasets dont have a label in the filename..!
                     noLabel =1 ;
                }                
        }

    if (path==0)
        {
         path="./";
        }

    if (label==0)
        {
         label="colorFrame_0_";
        }
    if (noLabel)
        {
         label="";
        }

    if (outputPath==0)
        {
            snprintf(outputPathFull,2048,"%s/2dJoints_v%0.1f.csv",path,version);
        }
    else
        {
            if (strstr(outputPath,".csv")!=0)
            {
               fprintf(stderr,"Output path is actually a csv file so treating it as one..\n"); 
              snprintf(outputPathFull,2048,"%s",outputPath);            
            } else
            {
              snprintf(outputPathFull,2048,"%s/2dJoints_v%0.1f.csv",outputPath,version);               
            }
        }


    char formatString[256]= {0};
    char filePathOfJSONFile[2048]= {0};
    
    snprintf(formatString,256,"%%s/%%s%%0%uu.jpg",serialLength); 
    snprintf(filePathOfJSONFile,2048,formatString,path,label,1/*Frame ID*/); 
    snprintf(formatString,256,"%%s/%%s%%0%uu_keypoints.json",serialLength);
    
    if (!forcedSize)
    {
     if ( getImageWidthHeight(filePathOfJSONFile,&width,&height) )
         {
              fprintf(stderr,"Image dimensions changed from default to %ux%u",width,height);
         }
    else
         {
              fprintf(stderr,"Assuming default image dimensions %ux%u , you can change this using --size x y\n",width,height);
         }    
    } else
    {
        fprintf(stderr,"Forcing size due to --size %u %u commandline parameter\n",width,height); 
    }


    float totalTime=0.0;
    unsigned int totalSamples=0;

    struct skeletonStructure skeleton;
    memset(&skeleton,0,sizeof(struct skeletonStructure));

    unsigned int frameID=1;
    
    fprintf(stderr,"Threshold is set to %0.2f\n",acceptableThreshold);
    fprintf(stderr,"Processing : \n");
    
     if (  findFirstJSONFileInDirectory(path, formatString,label,&frameID) )
                                {
                                   frameLimit+=frameID;  
                                   fprintf(stderr,"First frame is %u, last will be %u \n",frameID,frameLimit);
                                } else
                                {
                                   fprintf(stderr,RED "Failed to find a JSON file..!\n " NORMAL);
                                   return 0;
                                }
    

    while (frameID<frameLimit)
        {
            snprintf(filePathOfJSONFile,1024,formatString,path,label,frameID);
            fprintf(stderr,"%s (%ux%u)\n",filePathOfJSONFile,width,height);

            if (parseJsonCOCOSkeleton(filePathOfJSONFile,&skeleton,acceptableThreshold,processed))
                {
                    if (processed==0)
                        {
                            writeOpenPoseCSVHeaderFromSkeleton(outputPathFull,&skeleton,width,height);
                        }
                        
                     if (skeleton.leftHand.isPopulated)    { fprintf(stderr,GREEN "Left Hand " NORMAL); }
                     if (skeleton.rightHand.isPopulated)  { fprintf(stderr,GREEN "Right Hand " NORMAL); }
                     if (skeleton.body.isPopulated)             { fprintf(stderr,GREEN "Body " NORMAL); }
                     if (skeleton.head.isPopulated)             { fprintf(stderr,GREEN "Head " NORMAL); }

                    writeOpenPoseCSVBodyFromSkeleton(outputPathFull,&skeleton,respectTrainingAspectRatio,width,height);
                    ++processed;
                }
            else
                {
                    fprintf(stderr,"Done processing %u frames..\n",frameID);
                    fprintf(stderr,"Output has been stored at %s \n",outputPathFull);
                    break;
                }

            ++frameID;
        }
        
        
    return 0;
}
