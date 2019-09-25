#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>


#include "jsonCocoSkeleton.h"
#include "InputParser_C.h"



#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


float checkSkeletonDistance(struct skeletonCOCO * PreviousSkelA,struct skeletonCOCO * skelB)
{
    float score=0.0f,xSq=0.0f,ySq=0.0f;
    for (int i=0; i<COCO_PARTS; i++)
        {
            if (
                (skelB->joint2D[i].x==0) &&
                (skelB->joint2D[i].y==0)
            )
                {
                    score+=1000;// Penalize holes..
                }
            else
                {
                    xSq = PreviousSkelA->joint2D[i].x - skelB->joint2D[i].x ;
                    xSq = xSq * xSq;
                    ySq = PreviousSkelA->joint2D[i].y - skelB->joint2D[i].y;
                    ySq = ySq * ySq;

                    score+=sqrt(xSq+ySq);
                }
        }

    return score;
}

int parseJsonCOCOSkeleton(const char * filename , struct skeletonCOCO * skel)
{
    //memset(skel,0,sizeof(struct skeletonCOCO));

    ssize_t read;

    FILE * fp = fopen(filename,"r");
    if (fp!=0)
        {
            fprintf(stderr,"Parsing COCO 2D skeleton from %s \n",filename);
            struct InputParserC * ipc = InputParser_Create(2048,3);
            InputParser_SetDelimeter(ipc,0,',');
            InputParser_SetDelimeter(ipc,1,',');

            char * line = NULL;
            size_t len = 0;

            /*
             {"version":1.2,"people":[{"pose_keypoints_2d":[789.327,185.923,0.877001,736.197,294.809,0.846233,645.14,297.87,0.778431,562.688,409.561,0.799173,600.938,503.878,0.865556,821.695,291.873,0.778329,892.325,406.56,0.7665,983.596,436.206,0.82181,742.157,577.328,0.639119,692.05,577.387,0.591021,692.179,786.364,0.706035,698.136,986.468,0.672194,792.319,574.466,0.611536,789.358,789.312,0.678118,777.511,986.492,0.684245,768.645,174.097,0.903252,789.408,173.995,0.664033,712.725,183.01,0.866464,0,0,0,786.353,1010.05,0.217574,804,1007.09,0.278136,762.876,1004.13,0.581561,703.932,1018.88,0.579733,689.16,1013.02,0.552471,709.862,998.327,0.553658],"face_keypoints_2d":[],"hand_left_keypoints_2d":[992.469,449.941,0.0195435,1007.79,454.882,0.0253094,1015.69,461.8,0.0479027,985.552,451.424,0.0311281,995.928,472.176,0.0117744,1017.67,455.377,0.0288021,989.01,435.118,0.0465278,1035.95,460.318,0.0662571,1038.92,459.329,0.0111233,1047.32,439.565,0.0165805,1041.39,441.047,0.0306216,1032.49,462.788,0.0548987,1035.46,458.341,0.0143203,1048.3,443.024,0.0181987,1043.36,444.012,0.0616555,1033.48,459.824,0.0298993,1034.96,452.906,0.0227293,1057.69,449.447,0.0381292,1050.77,461.306,0.0714897,1051.27,461.306,0.0691071,1043.86,460.318,0.0214702],"hand_right_keypoints_2d":[581.984,568.107,0.125156,590.642,552.828,0.0291986,618.143,573.2,0.0320249,602.865,525.327,0.0453639,596.244,540.605,0.0145001,623.745,569.635,0.0295879,621.708,565.56,0.0507096,643.098,557.921,0.049187,646.663,575.746,0.025924,647.682,548.245,0.0149331,619.671,544.68,0.0449555,618.143,545.189,0.0147493,594.207,547.226,0.0183433,633.931,513.104,0.0123248,635.459,532.457,0.0325759,623.236,547.226,0.0159314,660.414,557.412,0.0168346,626.801,512.595,0.0139896,647.682,533.985,0.0408492,652.775,542.133,0.0495017,658.886,556.393,0.0373437],"pose_keypoints_3d":[],"face_keypoints_3d":[],"hand_left_keypoints_3d":[],"hand_right_keypoints_3d":[]}]}
             */
            while ((read = getline(&line, &len, fp)) != -1)
                {
                    //We should have the whole output.. since it is one line
                    char * poseStart=0;
                    char * poseEnd=0;
                    //-----------------------------------------------
                    poseStart=strstr(line,"\"pose_keypoints_2d\":[");
                    if(poseStart!=0)
                        {
                            poseStart=strstr(poseStart,"[")+1;
                            poseEnd=strstr(poseStart,"]");
                        }
                    //-----------------------------------------------
                    char * handLeftStart=0;
                    char * handLeftEnd=0;
                    handLeftStart=strstr(line,"\"hand_left_keypoints_2d\":[");
                    if (handLeftStart!=0)
                        {
                            handLeftStart=strstr(handLeftStart,"[")+1;
                            handLeftEnd=strstr(handLeftStart,"]");
                        }
                    //-----------------------------------------------

                    char * handRightStart=0;
                    char * handRightEnd=0;
                    handRightStart=strstr(line,"\"hand_right_keypoints_2d\":[");
                    if (handRightStart!=0)
                        {
                            handRightStart=strstr(handRightStart,"[")+1;
                            handRightEnd=strstr(handRightStart,"]");
                        }

                    if (poseEnd!=0)
                        {
                            *poseEnd=0;
                        }
                    if (handLeftEnd!=0)
                        {
                            *handLeftEnd=0;
                        }
                    if (handRightEnd!=0)
                        {
                            *handRightEnd=0;
                        }

                    //fprintf(stderr,"RHand : %s\n",handLeftStart);
                    //fprintf(stderr,"LHand : %s\n",handRightStart);
                    //fprintf(stderr,"Pose : %s\n",poseStart);

                    float value;
                    int numberOfJoints = InputParser_SeperateWords(ipc,poseStart,1)/3;
                    if (numberOfJoints>=BODY25_PARTS)
                        {
                            fprintf(stderr,RED "The number of joints found in JSON file (%u) is more than our COCO internal structure (%u)\n" NORMAL,numberOfJoints,COCO_PARTS);
                            exit(0);
                        }
                    for (int poseNum=0; poseNum<numberOfJoints; poseNum++)
                        {
                            skel->joint2D[poseNum].x = InputParser_GetWordFloat(ipc,poseNum*3+0);
                            //fprintf(stderr,"Pose%u x ( %u ) = %0.2f\n",poseNum,poseNum*3+0, skel->joint2D[poseNum].x  );

                            skel->joint2D[poseNum].y = InputParser_GetWordFloat(ipc,poseNum*3+1);
                            //fprintf(stderr,"Pose%u y ( %u ) = %0.2f\n",poseNum,poseNum*3+1,skel->joint2D[poseNum].y);

                            value = InputParser_GetWordFloat(ipc,poseNum*3+2);
                            if (value>1.0)
                                {
                                    fprintf(stderr,"Warning : Too large value for accuracy\n");
                                }
                            skel->jointAccuracy[poseNum] = value;
                            skel->active[poseNum] = (value>0.5);
                            //fprintf(stderr,"Pose%u A ( %u ) = %0.2f\n",poseNum,poseNum*3+2,value);
                        }


                    numberOfJoints = InputParser_SeperateWords(ipc,handLeftStart,1)/3;
                    skel->leftHand.isRight=0;
                    skel->leftHand.isLeft=1;
                    for (int poseNum=0; poseNum<numberOfJoints; poseNum++)
                        {
                            value = InputParser_GetWordFloat(ipc,poseNum*3+0);
                            skel->leftHand.joint2D[poseNum].x = value;

                            value = InputParser_GetWordFloat(ipc,poseNum*3+1);
                            skel->leftHand.joint2D[poseNum].y = value;

                            value = InputParser_GetWordFloat(ipc,poseNum*3+2);
                            skel->leftHand.jointAccuracy[poseNum] = value;
                            skel->leftHand.active[poseNum] = (value>0.5);
                        }

                    numberOfJoints = InputParser_SeperateWords(ipc,handRightStart,1)/3;
                    skel->rightHand.isRight=1;
                    skel->rightHand.isLeft=0;
                    for (int poseNum=0; poseNum<numberOfJoints; poseNum++)
                        {
                            value = InputParser_GetWordFloat(ipc,poseNum*3+0);
                            skel->rightHand.joint2D[poseNum].x = value;

                            value = InputParser_GetWordFloat(ipc,poseNum*3+1);
                            skel->rightHand.joint2D[poseNum].y = value;

                            value = InputParser_GetWordFloat(ipc,poseNum*3+2);
                            skel->rightHand.jointAccuracy[poseNum] = value;
                            skel->rightHand.active[poseNum] = (value>0.5);
                        }
                }
            InputParser_Destroy(ipc);
            fclose(fp);
            return 1;
        }

    fprintf(stderr,"Could not find COCO 2D skeleton in %s \n",filename);
    return 0;
}



int resolveCSVHeader(const char * header,int * type,int * jointID)
{
    unsigned int headerLength = strlen(header);
    
    if (headerLength<5)
    {
      fprintf(stderr,"Unknown CSV header tag..\n");  
      return 0;  
    }
    
    *type = 0;
    unsigned int startOfJointName=0;
    if (headerLength>=5)
    {
       if  ( strncmp(header,"2D_X_",5)==0 )    { *type = 1; startOfJointName=5; } else
       if  ( strncmp(header,"2D_Y_",5)==0 )    { *type = 2; startOfJointName=5; } 
    }
    if (headerLength>=7)
    {
       if  ( strncmp(header,"2D_vis_",7)==0 ) { *type = 3; startOfJointName=7; }    
    }
    
    int found=1;
    
    if (strcmp(header+startOfJointName,"hip")==0)       { *jointID = BODY25_MidHip; } else
    if (strcmp(header+startOfJointName,"abdomen")==0)   {   } else
    if (strcmp(header+startOfJointName,"chest")==0)     {  /* *jointID = BODY25_Chest;*/ } else
    if (strcmp(header+startOfJointName,"neck")==0)      { *jointID = BODY25_Neck; } else
    if (strcmp(header+startOfJointName,"head")==0)      { *jointID = BODY25_Nose; } else
    if (strcmp(header+startOfJointName,"lefteye")==0)   { *jointID = BODY25_LEye; } else
    if (strcmp(header+startOfJointName,"righteye")==0)  { *jointID = BODY25_REye; } else
    if (strcmp(header+startOfJointName,"rcollar")==0)   {  } else
    if (strcmp(header+startOfJointName,"rshoulder")==0) { *jointID = BODY25_RShoulder;  } else
    if (strcmp(header+startOfJointName,"relbow")==0)    { *jointID = BODY25_RElbow;  } else
    if (strcmp(header+startOfJointName,"rhand")==0)     { *jointID = BODY25_RWrist;  } else
    if (strcmp(header+startOfJointName,"lcollar")==0)   {  } else
    if (strcmp(header+startOfJointName,"lshoulder")==0) { *jointID = BODY25_LShoulder;  } else
    if (strcmp(header+startOfJointName,"lelbow")==0)    { *jointID = BODY25_LElbow;  } else
    if (strcmp(header+startOfJointName,"lhand")==0)     { *jointID = BODY25_LWrist;  } else
    if (strcmp(header+startOfJointName,"rhip")==0)      { *jointID = BODY25_RHip;   } else
    if (strcmp(header+startOfJointName,"rknee")==0)     { *jointID = BODY25_RKnee;  } else
    if (strcmp(header+startOfJointName,"rfoot")==0)     { *jointID = BODY25_RAnkle;  } else
    if (strcmp(header+startOfJointName,"es_rfoot")==0)  { *jointID = BODY25_RBigToe; /**jointID = BODY25_RSmallToe;*/  } else
    if (strcmp(header+startOfJointName,"lhip")==0)      { *jointID = BODY25_LHip;   } else
    if (strcmp(header+startOfJointName,"lknee")==0)     { *jointID = BODY25_LKnee;  } else
    if (strcmp(header+startOfJointName,"lfoot")==0)     { *jointID = BODY25_LAnkle;  } else
    if (strcmp(header+startOfJointName,"es_lfoot")==0)  { *jointID = BODY25_LBigToe; /**jointID = BODY25_LSmallToe;*/  } else
                                                         { found=0; }
 
    
    
    return found;
}




int openCSVFile(struct CSVFileContext * csv,const char * filename)
{
  csv->fp = fopen(filename,"r");
  if (csv->fp!=0)
  {
    return 1;  
  }  
 return 0; 
}

int closeCSVFile(struct CSVFileContext * csv)
{
  if (csv->fp!=0)
  {
    //TODO FREE STUFF HERE..  
    int i=0;
    for (i=0; i<csv->numberOfHeaderFields; i++)
    {
        if (csv->field[i].str!=0)
        {
          free(csv->field[i].str);
          csv->field[i].str=0;  
        }
    }
      
    fclose(csv->fp);  
  }  
 return 1; 
}


int parseCSVHeader(struct CSVFileContext * csv)
{ 
    char whereToStoreItems[256];
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    if (csv->lineNumber==0)
    {
        read = getline(&line, &len, csv->fp);
        struct InputParserC * ipc = InputParser_Create(2048,3);
        InputParser_SetDelimeter(ipc,0,',');
        InputParser_SetDelimeter(ipc,1,',');  
            
        int numberOfArguments = InputParser_SeperateWords(ipc,line,1);
        
        if (numberOfArguments<MAX_CSV_HEADER_FIELDS)
        {
           int i=0;
           for (i=0; i<numberOfArguments; i++)
           {
               InputParser_GetWord(ipc,i,whereToStoreItems,256);
               csv->field[i].strLength = strlen(whereToStoreItems); 
               csv->field[i].str = (char*) malloc(sizeof(char) * (csv->field[i].strLength+2));
               memcpy(csv->field[i].str ,whereToStoreItems,csv->field[i].strLength);
               csv->field[i].str[csv->field[i].strLength]=0;
               csv->numberOfHeaderFields = csv->numberOfHeaderFields + 1; 
           }
        } else
        {
          fprintf(stderr,"Too many CSV header arguments..\n");  
          return 0;  
        }
        
        
        
        
        InputParser_Destroy(ipc);
        csv->lineNumber = csv->lineNumber +1;
        return 1;
    }
  return 0;
}

  
int parseNextCSVCOCOSkeleton(struct CSVFileContext * csv, struct skeletonCOCO * skel)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    if (csv->lineNumber==0)
    {
        if (!parseCSVHeader(csv))
        {
           fprintf(stderr,"Could not read CSV header"); 
           return 0;
        }
    }
    
    if ((read = getline(&line, &len, csv->fp)) != -1)
        { 
            struct InputParserC * ipc = InputParser_Create(2048,3);
            InputParser_SetDelimeter(ipc,0,',');
            InputParser_SetDelimeter(ipc,1,',');  
            InputParser_SetDelimeter(ipc,2,',');  
            
            int numberOfArguments = InputParser_SeperateWords(ipc,line,1);
            
            int typeOfData=0,jointID=0;
            float value=0.0;
            for (int i=0; i<numberOfArguments; i++)
            {
               if (resolveCSVHeader(csv->field[i].str,&typeOfData,&jointID))
               {
                   value = InputParser_GetWordFloat(ipc,i);
                   switch (typeOfData)
                   {
                     case 1:
                       //fprintf(stderr,"joint[%s].x=%0.2f\n",csv->field[i].str,value);
                       skel->joint2D[jointID].x=value;
                     break;
                     case 2:
                       //fprintf(stderr,"joint[%s].y=%0.2f\n",csv->field[i].str,value);
                       skel->joint2D[jointID].y=value;
                     break;
                     case 3:
                       //fprintf(stderr,"joint[%s].accuracy=%0.2f\n",csv->field[i].str,value);
                       skel->jointAccuracy[jointID] = value;
                       skel->active[jointID] = (value>0.5);
                     break;
                     default:
                       fprintf(stderr,"Could not resolve CSV entry %u ( should be %s )\n",i,csv->field[i].str);
                     break; 
                   };
                    
                       
                   if (jointID==BODY25_LBigToe) 
                       {
                          skel->joint2D[BODY25_LSmallToe].x=skel->joint2D[BODY25_LBigToe].x;
                          skel->joint2D[BODY25_LSmallToe].y=skel->joint2D[BODY25_LBigToe].y;
                          skel->jointAccuracy[BODY25_LSmallToe]=skel->jointAccuracy[BODY25_LBigToe];
                          skel->active[BODY25_LSmallToe]=skel->active[BODY25_LBigToe];
                       } else
                    if (jointID==BODY25_RBigToe) 
                       {
                          skel->joint2D[BODY25_RSmallToe].x=skel->joint2D[BODY25_RBigToe].x;
                          skel->joint2D[BODY25_RSmallToe].y=skel->joint2D[BODY25_RBigToe].y;
                          skel->jointAccuracy[BODY25_RSmallToe]=skel->jointAccuracy[BODY25_RBigToe];
                          skel->active[BODY25_RSmallToe]=skel->active[BODY25_RBigToe];
                       }
               }
                 
            }
             
            
            InputParser_Destroy(ipc);
            csv->lineNumber = csv->lineNumber +1;
          return 1;
        }
    return 0;
}
