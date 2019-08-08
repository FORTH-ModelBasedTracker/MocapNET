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
            } else
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
    { //We should have the whole output.. since it is one line
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

      if (poseEnd!=0)      { *poseEnd=0; }
      if (handLeftEnd!=0)  { *handLeftEnd=0;  }
      if (handRightEnd!=0) { *handRightEnd=0; }

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
        if (value>1.0) { fprintf(stderr,"Warning : Too large value for accuracy\n"); }
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


