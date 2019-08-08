#include "jsonMocapNETHelpers.hpp"
#include <stdio.h>


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


int bothZero(float a,float b)
{
  return (a==0.0)&&(b==0.0);
}

int jointIsNotZero(struct skeletonCOCO * sk,int jointID)
{
  if ( bothZero(sk->joint2D[jointID].x,sk->joint2D[jointID].y) )
  {return 0;}
  return 1;
}

int bothJointAreNotZero(struct skeletonCOCO * sk,int jointIDA,int jointIDB)
{
  if (
       ( (sk->joint2D[jointIDA].x==0.0)&&(sk->joint2D[jointIDA].y==0.0) ) ||
       ( (sk->joint2D[jointIDB].x==0.0)&&(sk->joint2D[jointIDB].y==0.0) )
     )
  { return 0; }
  return 1;
}


void addSkeletonJointFromTwoJoints(
                                    struct skeletonCOCO * sk,
                                    std::vector<float> &result,
                                    int jointIDA,
                                    int jointIDB
                                   )

{
  float x=(float) (sk->joint2D[jointIDA].x + sk->joint2D[jointIDB].x)/2;
  float y=(float) (sk->joint2D[jointIDA].y + sk->joint2D[jointIDB].y)/2;
  float v=1.0;
  if (
       ( (sk->joint2D[jointIDA].x==0.0)&&(sk->joint2D[jointIDA].y==0.0) ) ||
       ( (sk->joint2D[jointIDB].x==0.0)&&(sk->joint2D[jointIDB].y==0.0) )
     )
  {
    x=0.0;
    y=0.0;
    v=0.0;
  }

  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
}

void addSkeletonJoint(
                       struct skeletonCOCO * sk,
                       std::vector<float> &result,
                       int jointID
                      )

{
  float x=(float) sk->joint2D[jointID].x;
  float y=(float) sk->joint2D[jointID].y;
  float v=1.0;
  if ( (x==0.0)&&(y==0.0) ) { v=0.0; }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
}

void addRightFinger(
                    struct skeletonCOCO * sk,
                    std::vector<float> &result,
                    int fingerJointA,
                    int fingerJointB,
                    int fingerJointC
                   )

{
  float x=(float) sk->rightHand.joint2D[fingerJointA].x;
  float y=(float) sk->rightHand.joint2D[fingerJointA].y;
  float v=1.0;
  if ( (x==0.0)&&(y==0.0) ) { v=0.0; }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
  //----------------------------------------------
  x=(float) sk->rightHand.joint2D[fingerJointB].x;
  y=(float) sk->rightHand.joint2D[fingerJointB].y;
  v=1.0;
  if ( (x==0.0)&&(y==0.0) ) { v=0.0; }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
  //----------------------------------------------
  x=(float) sk->rightHand.joint2D[fingerJointC].x;
  y=(float) sk->rightHand.joint2D[fingerJointC].y;
  v=1.0;
  if ( (x==0.0)&&(y==0.0) ) { v=0.0; }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
}



void addLeftFinger(
                    struct skeletonCOCO * sk,
                    std::vector<float> &result,
                    int fingerJointA,
                    int fingerJointB,
                    int fingerJointC
                   )

{
  float x=(float) sk->leftHand.joint2D[fingerJointA].x;
  float y=(float) sk->leftHand.joint2D[fingerJointA].y;
  float v=1.0;
  if ( (x==0.0)&&(y==0.0) ) { v=0.0; }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
  //----------------------------------------------
  x=(float) sk->leftHand.joint2D[fingerJointB].x;
  y=(float) sk->leftHand.joint2D[fingerJointB].y;
  v=1.0;
  if ( (x==0.0)&&(y==0.0) ) { v=0.0; }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
  //----------------------------------------------
  x=(float) sk->leftHand.joint2D[fingerJointC].x;
  y=(float) sk->leftHand.joint2D[fingerJointC].y;
  v=1.0;
  if ( (x==0.0)&&(y==0.0) ) { v=0.0; }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
}


std::vector<float> flattenskeletonCOCOToVector(struct skeletonCOCO * sk,unsigned int width ,unsigned int height)
{
  //Extra joints..
  float hipX=0.0;
  float hipY=0.0;
  int hipExists=0;

  float x,y,v;
  std::vector<float> result;
  //  MOCAPNET_UNCOMPRESSED_JOINT_HIP=0,        //0
  x=(float) (sk->joint2D[BODY25_LHip].x + sk->joint2D[BODY25_RHip].x)/2;
  y=(float) (sk->joint2D[BODY25_LHip].y + sk->joint2D[BODY25_RHip].y)/2;
  if (
       ( (sk->joint2D[BODY25_LHip].x==0.0)&&(sk->joint2D[BODY25_LHip].y==0.0) ) ||
       ( (sk->joint2D[BODY25_RHip].x==0.0)&&(sk->joint2D[BODY25_RHip].y==0.0) )
     )
  {
    v=0.0;
    x=0.0;
    y=0.0;
  } else
  {
    v=1.0;
    hipX=x;
    hipY=y;
    hipExists=1;
  }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
  //------------------------------------


  float chestX= (float) (sk->joint2D[BODY25_Neck].x + hipX)/2;
  float chestY= (float) (sk->joint2D[BODY25_Neck].y + hipY)/2;
  int chestNotVisible= (( (sk->joint2D[BODY25_Neck].x==0.0)&&(sk->joint2D[BODY25_Neck].y==0.0) ) || ( (hipX==0.0)&&(hipY==0.0) ) );



  //MOCAPNET_UNCOMPRESSED_JOINT_ABDOMEN,      //1
  x=(float) (sk->joint2D[BODY25_Neck].x + chestX)/2;
  y=(float) (sk->joint2D[BODY25_Neck].y + chestY)/2;
  if ( (chestNotVisible) || ((sk->joint2D[BODY25_Neck].x==0.0)&&(sk->joint2D[BODY25_Neck].y==0.0))  )
  {
    v=0.0;
    x=0.0;
    y=0.0;
  } else
  {
    v=1.0;
  }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
  //TODO:
  //------------------------------------

  //MOCAPNET_UNCOMPRESSED_JOINT_CHEST,        //2
  x=chestX;
  y=chestY;
  if ( chestNotVisible )
  {
    v=0.0;
    x=0.0;
    y=0.0;
  } else
  {
    v=1.0;
  }
  result.push_back(x);
  result.push_back(y);
  result.push_back(v);
  //------------------------------------


  //MOCAPNET_UNCOMPRESSED_JOINT_NECK,         //3
  addSkeletonJoint(sk,result,COCO_Neck);
  //------------------------------------

  //MOCAPNET_UNCOMPRESSED_JOINT_HEAD,         //4
  //
  int guessNose=1;
  if (guessNose==1)
     {
       if (bothJointAreNotZero(sk,BODY25_LEar,BODY25_REar))
        {
          addSkeletonJointFromTwoJoints(sk,result,BODY25_LEar,BODY25_REar);
        } else
       if (bothJointAreNotZero(sk,BODY25_LEye,BODY25_REye))
        {
          addSkeletonJointFromTwoJoints(sk,result,BODY25_LEye,BODY25_REye);
        }
     }  else
     { addSkeletonJoint(sk,result,BODY25_Nose); }
  //------------------------------------

  //MOCAPNET_UNCOMPRESSED_JOINT_LEFTEYE,      //5
  addSkeletonJoint(sk,result,BODY25_LEye);
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_LEFTEYE,   //6
  addSkeletonJoint(sk,result,BODY25_LEye);

  //MOCAPNET_UNCOMPRESSED_JOINT_RIGHTEYE,     //7
  addSkeletonJoint(sk,result,BODY25_REye);
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_RIGHTEYE,  //8
  addSkeletonJoint(sk,result,BODY25_REye);

  //MOCAPNET_UNCOMPRESSED_JOINT_RCOLLAR,      //9
  addSkeletonJointFromTwoJoints(sk,result,BODY25_Neck,BODY25_RShoulder);
  //------------------------------

  //MOCAPNET_UNCOMPRESSED_JOINT_RSHOULDER,    //10
  addSkeletonJoint(sk,result,BODY25_RShoulder);

  //MOCAPNET_UNCOMPRESSED_JOINT_RELBOW,       //11
  addSkeletonJoint(sk,result,BODY25_RElbow);

  //MOCAPNET_UNCOMPRESSED_JOINT_RHAND,        //12
  addSkeletonJoint(sk,result,BODY25_RWrist);

  //MOCAPNET_UNCOMPRESSED_JOINT_RTHUMB1,      //13
  //MOCAPNET_UNCOMPRESSED_JOINT_RTHUMB2,      //14
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_RTHUMB2,   //15
  addRightFinger(sk,result,COCO_Hand_Thumb_1,COCO_Hand_Thumb_2,COCO_Hand_Thumb_3);

  //MOCAPNET_UNCOMPRESSED_JOINT_RINDEX1,      //16
  //MOCAPNET_UNCOMPRESSED_JOINT_RINDEX2,      //17
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_RINDEX2,   //18
  addRightFinger(sk,result,COCO_Hand_Index_1,COCO_Hand_Index_2,COCO_Hand_Index_3);


  //MOCAPNET_UNCOMPRESSED_JOINT_RMID1,        //19
  //MOCAPNET_UNCOMPRESSED_JOINT_RMID2,        //20
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_RMID2,     //21
  addRightFinger(sk,result,COCO_Hand_Middle_1,COCO_Hand_Middle_2,COCO_Hand_Middle_3);


  //MOCAPNET_UNCOMPRESSED_JOINT_RRING1,       //22
  //MOCAPNET_UNCOMPRESSED_JOINT_RRING2,       //23
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_RRING2,    //24
  addRightFinger(sk,result,COCO_Hand_Ring_1,COCO_Hand_Ring_2,COCO_Hand_Ring_3);

  //MOCAPNET_UNCOMPRESSED_JOINT_RPINKY1,      //25
  //MOCAPNET_UNCOMPRESSED_JOINT_RPINKY2,      //26
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_RPINKY2,   //27
  addRightFinger(sk,result,COCO_Hand_Pinky_1,COCO_Hand_Pinky_2,COCO_Hand_Pinky_3);

  //MOCAPNET_UNCOMPRESSED_JOINT_LCOLLAR,      //28
  addSkeletonJointFromTwoJoints(sk,result,BODY25_Neck,BODY25_LShoulder);


  //MOCAPNET_UNCOMPRESSED_JOINT_LSHOULDER,    //29
  addSkeletonJoint(sk,result,BODY25_LShoulder);

  //MOCAPNET_UNCOMPRESSED_JOINT_LELBOW,       //30
  addSkeletonJoint(sk,result,BODY25_LElbow);

  //MOCAPNET_UNCOMPRESSED_JOINT_LHAND,        //31
  addSkeletonJoint(sk,result,BODY25_LWrist);

  //MOCAPNET_UNCOMPRESSED_JOINT_LTHUMB1,      //32
  //MOCAPNET_UNCOMPRESSED_JOINT_LTHUMB2,      //33
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_LTHUMB2,   //34
  addLeftFinger(sk,result,COCO_Hand_Thumb_1,COCO_Hand_Thumb_2,COCO_Hand_Thumb_3);

  //MOCAPNET_UNCOMPRESSED_JOINT_LINDEX1,      //35
  //MOCAPNET_UNCOMPRESSED_JOINT_LINDEX2,      //36
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_LINDEX2,   //37
  addLeftFinger(sk,result,COCO_Hand_Index_1,COCO_Hand_Index_2,COCO_Hand_Index_3);

  //MOCAPNET_UNCOMPRESSED_JOINT_LMID1,        //38
  //MOCAPNET_UNCOMPRESSED_JOINT_LMID2,        //39
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_LMID2,     //40
  addLeftFinger(sk,result,COCO_Hand_Middle_1,COCO_Hand_Middle_2,COCO_Hand_Middle_3);

  //MOCAPNET_UNCOMPRESSED_JOINT_LRING1,       //41
  //MOCAPNET_UNCOMPRESSED_JOINT_LRING2,       //42
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_LRING2,    //43
  addLeftFinger(sk,result,COCO_Hand_Ring_1,COCO_Hand_Ring_2,COCO_Hand_Ring_3);

  //MOCAPNET_UNCOMPRESSED_JOINT_LPINKY1,      //44
  //MOCAPNET_UNCOMPRESSED_JOINT_LPINKY2,      //45
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_LPINKY2,   //46
  addLeftFinger(sk,result,COCO_Hand_Pinky_1,COCO_Hand_Pinky_2,COCO_Hand_Pinky_3);


  //MOCAPNET_UNCOMPRESSED_JOINT_RBUTTOCK,     //47
  //MOCAPNET_UNCOMPRESSED_JOINT_RHIP,         //48
  addSkeletonJoint(sk,result,BODY25_RHip);
  addSkeletonJoint(sk,result,BODY25_RHip);

  //MOCAPNET_UNCOMPRESSED_JOINT_RKNEE,        //49
  addSkeletonJoint(sk,result,BODY25_RKnee);

  //MOCAPNET_UNCOMPRESSED_JOINT_RFOOT,        //50
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_RFOOT,     //51
  addSkeletonJoint(sk,result,BODY25_RAnkle);
  addSkeletonJoint(sk,result,BODY25_RAnkle);

  //MOCAPNET_UNCOMPRESSED_JOINT_LBUTTOCK,     //52
  //MOCAPNET_UNCOMPRESSED_JOINT_LHIP,         //53
  addSkeletonJoint(sk,result,BODY25_LHip);
  addSkeletonJoint(sk,result,BODY25_LHip);

  //MOCAPNET_UNCOMPRESSED_JOINT_LKNEE,        //54
  addSkeletonJoint(sk,result,BODY25_LKnee);

  //MOCAPNET_UNCOMPRESSED_JOINT_LFOOT,        //55
  //MOCAPNET_UNCOMPRESSED_JOINT_ES_LFOOT,     //56
  addSkeletonJoint(sk,result,BODY25_LAnkle);
  addSkeletonJoint(sk,result,BODY25_LAnkle);


  for (int i=0; i<result.size()/3; i++)
      {
        result[i*3+0]=(float) result[i*3+0]/width;
        result[i*3+1]=(float) result[i*3+1]/height;
      }

 if (result.size()==0)
 {
   fprintf(stderr,"Could not convert skeleton to vector..\n");
   return result;
 }
 return result;
}


