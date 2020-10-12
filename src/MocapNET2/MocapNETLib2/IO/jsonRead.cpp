#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>


#include "jsonRead.hpp"
#include "../../../../dependencies/InputParser/InputParser_C.h"



#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


float checkSkeletonDistance(struct skeletonStructure * PreviousSkelA,struct skeletonStructure * skelB)
{
    float score=0.0f,xSq=0.0f,ySq=0.0f;
    for (int i=0; i<COCO_PARTS; i++)
        {
            if (
                (skelB->body.joint2D[i].x==0) &&
                (skelB->body.joint2D[i].y==0)
            )
                {
                    score+=1000;// Penalize holes..
                }
            else
                {
                    xSq = PreviousSkelA->body.joint2D[i].x - skelB->body.joint2D[i].x ;
                    xSq = xSq * xSq;
                    ySq = PreviousSkelA->body.joint2D[i].y - skelB->body.joint2D[i].y;
                    ySq = ySq * ySq;

                    score+=sqrt(xSq+ySq);
                }
        }

    return score;
}

int parseJsonCOCOSkeleton(const char * filename , struct skeletonStructure * skel,float acceptableThreshold,unsigned int frameID)
{
    //memset(skel,0,sizeof(struct skeletonStructure));

    ssize_t read;

    FILE * fp = fopen(filename,"r");
    if (fp!=0)
        {
            fprintf(stderr,"Parsing 2D skeleton from %s \n",filename);
            struct InputParserC * ipc = InputParser_Create(2048,3);
            InputParser_SetDelimeter(ipc,0,',');
            InputParser_SetDelimeter(ipc,1,',');

            char * line = NULL;
            size_t len = 0;

            /*
             {"version":1.2,"people":[{"pose_keypoints_2d":[1051.33,389.167,0.90388,1054.25,474.35,0.834839,995.279,477.271,0.840843,977.646,592.078,0.843254,974.836,692.082,0.925783,1119.04,465.536,0.857748,1148.35,574.318,0.82055,1163.13,671.663,0.926104,1065.89,668.604,0.768351,1021.83,668.697,0.789775,998.248,833.448,0.891052,954.023,974.726,0.895817,1104.23,668.623,0.725152,1092.4,833.417,0.892817,1068.87,954.203,0.908514,1042.39,380.222,0.879022,1069.03,380.207,0.905886,1021.77,388.906,0.874647,1092.43,388.924,0.880871,1095.43,1001.27,0.875549,1110.14,995.311,0.873654,1048.35,968.843,0.832852,954.222,1024.87,0.826879,933.585,1021.78,0.818463,951.109,980.601,0.70936],"face_keypoints_2d":[1021.85,379.364,0.312111,1022.56,385.04,0.388846,1022.92,394.265,0.400321,1025.04,404.553,0.559545,1027.88,413.423,0.583821,1031.79,421.938,0.61052,1037.46,429.389,0.609643,1044.2,434.001,0.630441,1051.3,435.775,0.729202,1059.81,435.065,0.657737,1067.97,430.098,0.685271,1074.36,423.003,0.685615,1078.97,415.197,0.731754,1081.81,406.327,0.712712,1082.17,397.103,0.697536,1082.88,388.233,0.6253,1083.94,380.073,0.518587,1022.56,373.332,0.47997,1027.88,370.494,0.637251,1033.91,369.784,0.717338,1041.72,370.139,0.853094,1047.75,372.977,0.90591,1056.27,373.332,0.889469,1062.3,370.494,0.892274,1068.33,370.494,0.831293,1074.36,371.558,0.73095,1078.97,374.751,0.598676,1051.65,380.073,0.765338,1050.94,386.459,0.77752,1050.94,392.491,0.767245,1050.94,398.167,0.696292,1044.56,401.006,0.712432,1047.75,401.715,0.848516,1050.94,403.134,0.929941,1054.85,402.07,0.957662,1058.75,401.36,0.981191,1030.01,378.299,0.420897,1033.91,378.654,0.380504,1037.11,378.654,0.393703,1039.95,379.009,0.464231,1036.75,379.009,0.458442,1033.56,379.009,0.404015,1062.3,381.847,0.749875,1065.49,381.492,0.641693,1069.39,381.137,0.495781,1072.94,381.492,0.501538,1069.75,381.847,0.518428,1065.49,382.202,0.611457,1038.53,408.456,0.72183,1042.78,407.747,0.864191,1048.11,407.392,0.85783,1051.3,408.456,0.80366,1054.85,407.392,0.833569,1060.17,408.456,0.880244,1066.2,410.23,0.902783,1061.94,417.326,0.928561,1056.27,420.519,0.899355,1051.3,420.874,0.923864,1048.11,420.519,0.927453,1042.07,418.035,0.798128,1039.95,409.875,0.841352,1048.11,410.23,0.881547,1051.65,410.23,0.85485,1055.2,410.23,0.805752,1064.43,410.94,0.870964,1055.91,415.907,0.938425,1051.65,416.262,0.891374,1048.11,416.262,0.926013,1034.98,378.654,0.312223,1067.62,381.492,0.513061],"hand_left_keypoints_2d":[1165.94,680.642,0.466609,1156.44,688.082,0.629248,1153.13,701.72,0.761934,1153.13,715.359,0.819062,1151.89,726.518,0.775558,1168.84,711.64,0.671397,1168.84,728.172,0.799059,1163.88,737.264,0.725134,1160.16,742.224,0.736829,1172.97,711.64,0.702249,1171.73,728.172,0.808444,1166.77,738.091,0.642235,1161.4,742.224,0.604293,1173.38,710.813,0.666696,1172.97,724.865,0.61383,1166.77,732.718,0.536511,1163.88,740.157,0.497097,1172.56,708.747,0.597281,1172.14,721.559,0.59746,1167.18,727.345,0.381854,1163.88,729.411,0.33763],"hand_right_keypoints_2d":[976.039,702.5,0.411615,985.84,702.926,0.655772,996.066,713.152,0.692949,1002.03,726.362,0.772721,1009.7,733.605,0.864994,984.561,730.197,0.731554,988.822,745.536,0.812263,990.953,756.615,0.808444,991.805,765.137,0.836125,979.022,737.014,0.686607,982.005,749.798,0.65217,981.579,760.876,0.754628,979.448,767.694,0.727902,976.039,738.719,0.634819,977.317,749.798,0.69985,976.465,758.32,0.664762,975.187,765.137,0.690298,975.187,739.571,0.592209,975.187,748.093,0.492786,975.613,754.059,0.522497,974.761,760.45,0.43477],"pose_keypoints_3d":[],"face_keypoints_3d":[],"hand_left_keypoints_3d":[],"hand_right_keypoints_3d":[]}]}
            */
            while ((read = getline(&line, &len, fp)) != -1)
                {
                    skel->observationNumber=frameID;
                    
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
                    //----------------------------------------------- 
                    char * headStart=0;
                    char * headEnd=0;
                    headStart=strstr(line,"\"face_keypoints_2d\":[");
                    if (headStart!=0)
                        {
                            headStart=strstr(headStart,"[")+1;
                            headEnd=strstr(headStart,"]");
                        }
                    //-----------------------------------------------
                        
                    //Null Terminate strings..    
                    //----------------------------------------------------------------------------------------------------------------
                    if (poseEnd!=0)               { *poseEnd=0; }
                    if (handLeftEnd!=0)     { *handLeftEnd=0; }
                    if (handRightEnd!=0)  { *handRightEnd=0; }
                    if (headEnd!=0)              { *headEnd=0; }
                    //----------------------------------------------------------------------------------------------------------------


                    //fprintf(stderr,"RHand : %s\n",handLeftStart);
                    //fprintf(stderr,"LHand : %s\n",handRightStart);
                    //fprintf(stderr,"Pose : %s\n",poseStart);
                    //fprintf(stderr,"Head : %s\n",headStart);

                    int numberOfJoints; 
                    float value;
                    
                    
                        
                    //----------------------------------------------------------------------------------------------------------------         
                    if (poseStart!=0)
                    {
                     numberOfJoints = InputParser_SeperateWords(ipc,poseStart,1)/3;
                     if (numberOfJoints>=BODY25_PARTS)
                        {
                            fprintf(stderr,RED "The number of joints found in JSON file (%u) is more than our BODY25 internal structure (%u)\n" NORMAL,numberOfJoints,BODY25_PARTS);
                         } else
                         {
                           if (numberOfJoints>0) {   skel->body.isPopulated=1; }  
                           for (int poseNum=0; poseNum<numberOfJoints; poseNum++)
                             {
                               skel->body.joint2D[poseNum].x = InputParser_GetWordFloat(ipc,poseNum*3+0);
                               //fprintf(stderr,"Pose%u x ( %u ) = %0.2f\n",poseNum,poseNum*3+0, skel->joint2D[poseNum].x  );

                               skel->body.joint2D[poseNum].y = InputParser_GetWordFloat(ipc,poseNum*3+1);
                               //fprintf(stderr,"Pose%u y ( %u ) = %0.2f\n",poseNum,poseNum*3+1,skel->joint2D[poseNum].y);

                               value = InputParser_GetWordFloat(ipc,poseNum*3+2);
                               if (value>1.0)
                                  {
                                    fprintf(stderr,"Warning : Too large value for accuracy\n");
                                  }
                               skel->body.jointAccuracy[poseNum] = value;
                               skel->body.active[poseNum] = (value>acceptableThreshold);
                              //fprintf(stderr,"Pose%u A ( %u ) = %0.2f\n",poseNum,poseNum*3+2,value);
                             }
                         }
                    }
                     //----------------------------------------------------------------------------------------------------------------   

                    // Left Hand Parser
                    //----------------------------------------------------------------------------------------------------------------
                    if (handLeftStart!=0)
                    {
                      numberOfJoints = InputParser_SeperateWords(ipc,handLeftStart,1)/3;
                       if (numberOfJoints>COCO_HAND_PARTS)
                        {
                            fprintf(stderr,RED "The number of left hand joints found in JSON file (%u) is more than our internal structure (%u)\n" NORMAL,numberOfJoints,COCO_HAND_PARTS);
                         } else
                         { 
                           skel->leftHand.isRight=0;
                           skel->leftHand.isLeft=1;
                           if (numberOfJoints>0) {   skel->leftHand.isPopulated=1; }  
                           for (int poseNum=0; poseNum<numberOfJoints; poseNum++)
                               {
                                   value = InputParser_GetWordFloat(ipc,poseNum*3+0);
                                   skel->leftHand.joint2D[poseNum].x = value;

                                   value = InputParser_GetWordFloat(ipc,poseNum*3+1);
                                   skel->leftHand.joint2D[poseNum].y = value;

                                   value = InputParser_GetWordFloat(ipc,poseNum*3+2);
                                   skel->leftHand.jointAccuracy[poseNum] = value;
                                   skel->leftHand.active[poseNum] = (value>acceptableThreshold);
                               }
                         }
                    }
                    
                    
                    // Right Hand Parser 
                    //----------------------------------------------------------------------------------------------------------------
                    if (handRightStart!=0)
                    {
                     numberOfJoints = InputParser_SeperateWords(ipc,handRightStart,1)/3;
                      if (numberOfJoints>COCO_HAND_PARTS)
                        {
                            fprintf(stderr,RED "The number of right hand joints found in JSON file (%u) is more than our internal structure (%u)\n" NORMAL,numberOfJoints,COCO_HAND_PARTS);
                         } else
                         { 
                           skel->rightHand.isRight=1;
                           skel->rightHand.isLeft=0;
                           if (numberOfJoints>0) {   skel->rightHand.isPopulated=1; }  
                           for (int poseNum=0; poseNum<numberOfJoints; poseNum++)
                               {
                                   value = InputParser_GetWordFloat(ipc,poseNum*3+0);
                                   skel->rightHand.joint2D[poseNum].x = value;

                                   value = InputParser_GetWordFloat(ipc,poseNum*3+1);
                                   skel->rightHand.joint2D[poseNum].y = value;

                                   value = InputParser_GetWordFloat(ipc,poseNum*3+2);
                                   skel->rightHand.jointAccuracy[poseNum] = value;
                                   skel->rightHand.active[poseNum] = (value>acceptableThreshold);
                               }
                         }
                    }
                    //----------------------------------------------------------------------------------------------------------------
    
    
                      // Head Parser 
                    //----------------------------------------------------------------------------------------------------------------
                    if (headStart!=0)
                    {
                    numberOfJoints = InputParser_SeperateWords(ipc,headStart,1)/3; 
                    if (numberOfJoints>OP_HEAD_PARTS)
                        {
                            fprintf(stderr,RED "The number of head joints found in JSON file (%u) is more than our internal structure (%u)\n" NORMAL,numberOfJoints,OP_HEAD_PARTS);
                         } else
                         { 
                           if (numberOfJoints>0) {   skel->head.isPopulated=1; }  
                            for (int poseNum=0; poseNum<numberOfJoints; poseNum++)
                                  {
                                      value = InputParser_GetWordFloat(ipc,poseNum*3+0);
                                      skel->head.joint2D[poseNum].x = value;

                                      value = InputParser_GetWordFloat(ipc,poseNum*3+1);
                                      skel->head.joint2D[poseNum].y = value;

                                      value = InputParser_GetWordFloat(ipc,poseNum*3+2);
                                      skel->head.jointAccuracy[poseNum] = value;
                                      skel->head.active[poseNum] = (value>acceptableThreshold);
                                  }
                         }
                    }
                    //---------------------------------------------------------------------------------------------------------------- 
                    
                }

            InputParser_Destroy(ipc);
            fclose(fp);
            return 1;
        } 
         else
        {
          fprintf(stderr,"Could not open 2D skeleton file %s \n",filename);      
         return 0; 
        }
        

    fprintf(stderr,"Could not find COCO 2D skeleton in %s \n",filename);
    return 0;
}


