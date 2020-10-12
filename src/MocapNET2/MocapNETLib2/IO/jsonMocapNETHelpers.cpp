#include "jsonMocapNETHelpers.hpp"
#include <stdio.h>
#include <string.h>

#include "../../MocapNETLib2/IO/bvh.hpp"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


int bothZero(float a,float b)
{
    return (a==0.0)&&(b==0.0);
}

int jointIsNotZero(struct skeletonStructure * sk,int jointID)
{
    if ( bothZero(sk->body.joint2D[jointID].x,sk->body.joint2D[jointID].y) )
        {
            return 0;
        }
    return 1;
}

int bothJointAreNotZero(struct skeletonStructure * sk,int jointIDA,int jointIDB)
{
    if (
        ( (sk->body.joint2D[jointIDA].x==0.0)&&(sk->body.joint2D[jointIDA].y==0.0) ) ||
        ( (sk->body.joint2D[jointIDB].x==0.0)&&(sk->body.joint2D[jointIDB].y==0.0) )
    )
        {
            return 0;
        }
    return 1;
}


void addSkeletonJointFromTwoJoints(
    struct skeletonStructure * sk,
    std::vector<float> &result,
    int jointIDA,
    int jointIDB
)
{
    float x=(float) (sk->body.joint2D[jointIDA].x + sk->body.joint2D[jointIDB].x)/2;
    float y=(float) (sk->body.joint2D[jointIDA].y + sk->body.joint2D[jointIDB].y)/2;
    float v=1.0;
    if (
        ( (sk->body.joint2D[jointIDA].x==0.0)&&(sk->body.joint2D[jointIDA].y==0.0) ) ||
        ( (sk->body.joint2D[jointIDB].x==0.0)&&(sk->body.joint2D[jointIDB].y==0.0) )
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
    struct skeletonStructure * sk,
    std::vector<float> &result,
    int jointID
)

{
    float x=(float) sk->body.joint2D[jointID].x;
    float y=(float) sk->body.joint2D[jointID].y;
    float v=1.0;
    if ( (x==0.0)&&(y==0.0) )
        {
            v=0.0;
        }
    result.push_back(x);
    result.push_back(y);
    result.push_back(v);
}

void addRightFinger(
    struct skeletonStructure * sk,
    std::vector<float> &result,
    int fingerJointA,
    int fingerJointB,
    int fingerJointC
)

{
    float x=(float) sk->rightHand.joint2D[fingerJointA].x;
    float y=(float) sk->rightHand.joint2D[fingerJointA].y;
    float v=1.0;
    if ( (x==0.0)&&(y==0.0) )
        {
            v=0.0;
        }
    result.push_back(x);
    result.push_back(y);
    result.push_back(v);
    //----------------------------------------------
    x=(float) sk->rightHand.joint2D[fingerJointB].x;
    y=(float) sk->rightHand.joint2D[fingerJointB].y;
    v=1.0;
    if ( (x==0.0)&&(y==0.0) )
        {
            v=0.0;
        }
    result.push_back(x);
    result.push_back(y);
    result.push_back(v);
    //----------------------------------------------
    x=(float) sk->rightHand.joint2D[fingerJointC].x;
    y=(float) sk->rightHand.joint2D[fingerJointC].y;
    v=1.0;
    if ( (x==0.0)&&(y==0.0) )
        {
            v=0.0;
        }
    result.push_back(x);
    result.push_back(y);
    result.push_back(v);
}



void addLeftFinger(
    struct skeletonStructure * sk,
    std::vector<float> &result,
    int fingerJointA,
    int fingerJointB,
    int fingerJointC
)

{
    float x=(float) sk->leftHand.joint2D[fingerJointA].x;
    float y=(float) sk->leftHand.joint2D[fingerJointA].y;
    float v=1.0;
    if ( (x==0.0)&&(y==0.0) )
        {
            v=0.0;
        }
    result.push_back(x);
    result.push_back(y);
    result.push_back(v);
    //----------------------------------------------
    x=(float) sk->leftHand.joint2D[fingerJointB].x;
    y=(float) sk->leftHand.joint2D[fingerJointB].y;
    v=1.0;
    if ( (x==0.0)&&(y==0.0) )
        {
            v=0.0;
        }
    result.push_back(x);
    result.push_back(y);
    result.push_back(v);
    //----------------------------------------------
    x=(float) sk->leftHand.joint2D[fingerJointC].x;
    y=(float) sk->leftHand.joint2D[fingerJointC].y;
    v=1.0;
    if ( (x==0.0)&&(y==0.0) )
        {
            v=0.0;
        }
    result.push_back(x);
    result.push_back(y);
    result.push_back(v);
}



std::vector<float> normalizeWhileAlsoMatchingTrainingAspectRatio(
                                                  std::vector<float> input,
                                                  unsigned int currentWidth,
                                                  unsigned int currentHeight,
                                                  unsigned int trainingWidth,
                                                  unsigned int trainingHeight
                                                )
{ 
    //fprintf(stderr,YELLOW "normalizeWhileAlsoMatchingTrainingAspectRatio\n" NORMAL);
    unsigned int addX=0,addY=0;
    unsigned int targetWidth=currentWidth,targetHeight=currentHeight;
    float currentAspectRatio = (float) currentWidth/currentHeight;
    float trainingAspectRatio = (float) trainingWidth/trainingHeight;
    
    //fprintf(stderr,"Will try to correct aspect ratio from %0.2f(%ux%u) to %0.2f (%ux%u)\n",currentAspectRatio,currentWidth,currentHeight,trainingAspectRatio,trainingWidth,trainingHeight);
   
      std::vector<float> fixedAspectRatio=input;
    
      if (currentHeight<currentWidth)
       {
         targetHeight=(unsigned int) currentWidth/trainingAspectRatio;   
         if (targetHeight>=currentHeight)
         {
           addY=(unsigned int) (targetHeight-currentHeight)/2;           
         } else
         {
           //Turns out we will have to enlarge X instead of englarging Y
           addY=0;
           targetHeight=currentHeight;
           targetWidth=(unsigned int)currentHeight*trainingAspectRatio;
           addX=(unsigned int) (targetWidth-currentWidth)/2; 
         }  
       } else   
     if (currentWidth<=currentHeight)
       {
         targetWidth=(unsigned int)currentHeight*trainingAspectRatio;
         if (targetWidth>=currentWidth)  
         {
           addX=(unsigned int) (targetWidth-currentWidth)/2;    
         } else
         {
           //Turns out we will have to enlarge Y instead of englarging X
           addX=0;
            targetWidth=currentWidth;
           targetHeight=(unsigned int) currentWidth/trainingAspectRatio;  
           addY=(unsigned int) (targetHeight-currentHeight)/2;
         }
       } 
           
    //fprintf(stderr,"Target resolution is  %ux%u to Y\n",targetWidth,targetHeight);
    //fprintf(stderr,"Will add %u to X and %u to Y to achieve it\n",addX,addY);
    
    float targetAspectRatio=(float) targetWidth/targetHeight;
    if ((unsigned int) targetAspectRatio/100!= (unsigned int) trainingAspectRatio/100)
    {
      fprintf(stderr,RED "Failed to perfectly match training aspect ratio (%0.5f), managed to reach (%0.5f)\n" NORMAL,trainingAspectRatio,targetAspectRatio);  
    }   
       
      for (int i=0; i<input.size()/3; i++)
        {
          if (input[i*3+2]!=0)    
          {  
            //Only Fix Aspect Ratio on visible points to preserve 0,0,0 that are 
            //important to the neural network..  
            fixedAspectRatio[i*3+0]=(float) (input[i*3+0]+addX)/targetWidth;
            fixedAspectRatio[i*3+1]=(float) (input[i*3+1]+addY)/targetHeight; 
          }
        }    
       
      return fixedAspectRatio;  
}





std::vector<float> flattenskeletonStructureToVector(struct skeletonStructure * sk,unsigned int width ,unsigned int height)
{
    //Extra joints..
    float hipX=0.0;
    float hipY=0.0;
    int hipExists=0;

    float x,y,v;
    std::vector<float> result;
    //  MOCAPNET_UNCOMPRESSED_JOINT_HIP=0,        //0
    x=(float) (sk->body.joint2D[BODY25_LHip].x + sk->body.joint2D[BODY25_RHip].x)/2;
    y=(float) (sk->body.joint2D[BODY25_LHip].y + sk->body.joint2D[BODY25_RHip].y)/2;
    if (
        ( (sk->body.joint2D[BODY25_LHip].x==0.0)&&(sk->body.joint2D[BODY25_LHip].y==0.0) ) ||
        ( (sk->body.joint2D[BODY25_RHip].x==0.0)&&(sk->body.joint2D[BODY25_RHip].y==0.0) )
    )
        {
            v=0.0;
            x=0.0;
            y=0.0;
        }
    else
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


    float chestX= (float) (sk->body.joint2D[BODY25_Neck].x + hipX)/2;
    float chestY= (float) (sk->body.joint2D[BODY25_Neck].y + hipY)/2;
    int chestNotVisible= (( (sk->body.joint2D[BODY25_Neck].x==0.0)&&(sk->body.joint2D[BODY25_Neck].y==0.0) ) || ( (hipX==0.0)&&(hipY==0.0) ) );



    //MOCAPNET_UNCOMPRESSED_JOINT_ABDOMEN,      //1
    x=(float) (sk->body.joint2D[BODY25_Neck].x + chestX)/2;
    y=(float) (sk->body.joint2D[BODY25_Neck].y + chestY)/2;
    if ( (chestNotVisible) || ((sk->body.joint2D[BODY25_Neck].x==0.0)&&(sk->body.joint2D[BODY25_Neck].y==0.0))  )
        {
            v=0.0;
            x=0.0;
            y=0.0;
        }
    else
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
        }
    else
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
            //TODO : The nose should only be visible in front 
            //Eyes the same.. I should also add this to the BVH -> 3D CSV conversions..!
        
            if (bothJointAreNotZero(sk,BODY25_LEar,BODY25_REar))
                {
                    addSkeletonJointFromTwoJoints(sk,result,BODY25_LEar,BODY25_REar);
                }
            else  
            if (bothJointAreNotZero(sk,BODY25_LEye,BODY25_REye))
                {
                  addSkeletonJointFromTwoJoints(sk,result,BODY25_LEye,BODY25_REye);
                } else
                { 
                  addSkeletonJoint(sk,result,BODY25_Nose);
                }
        }
    else
        {
            addSkeletonJoint(sk,result,BODY25_Nose);
        }
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





    
    //Last sanity check..!
    //-----------------------------------------------------------
     for (int i=0; i<result.size()/3; i++)
             {
               if (
                      ( ( result[i*3+0]<=0 )&&( result[i*3+1]<=0 ) ) || //Both x,y are zero
                      ( ( result[i*3+0]==0 )||( result[i*3+1]==0 ) )    //Any of x or y are zero to combat a weird bug..
                  ) 
               {
                 result[i*3+0]=0;  
                 result[i*3+1]=0;  
                 result[i*3+2]=0; 
               }
             }
    //-----------------------------------------------------------



    
    if ( (width==1) && (height==1) )
    {
       //Input is already normalized, nothing to do here.. 
       //fprintf(stderr,"Input is already normalized\n");
    } else
    { 
       //This is the training configuration of MocapNET 
       unsigned int trainingWidth=1920;
       unsigned int trainingHeight=1080;
 
       float currentAspectRatio  = (float) width/height;
       float trainingAspectRatio = (float) trainingWidth/trainingHeight;
    
       if (currentAspectRatio!=trainingAspectRatio)
        { 
             //fprintf(stderr,"Normalizing WITH aspect ratio change\n");
             //MocapNET has been trained on 1920x1080 frames, so all the received coordinates are normalized in the
             //0..1 range based on that. This means that the NN learns the X and Y variations. If a joint lies at  pixel 500,500 
             //it will be represented as 500/1920  , 500/1080.
             //Now if a user uses another configuration, let's say a vertical (portrait) feed where the resolution is 1080x1920
             //the 2D points will get normalized at 500/1080 , 500/1920 and the resulting 2D joint cloud won't work as well
             //This is why it is better to change the aspect ratio while normalizing      
             result = normalizeWhileAlsoMatchingTrainingAspectRatio(result,width,height,trainingWidth,trainingHeight);
        } else
        {   
          //If we don't want to correct aspect ratio just go on with normalization..  
          //fprintf(stderr,"Normalizing without aspect ratio change\n");
          for (int i=0; i<result.size()/3; i++)
             {
               result[i*3+0]=(float) result[i*3+0]/width;
               result[i*3+1]=(float) result[i*3+1]/height;
             }
        }
    }
    
    

    if (result.size()==0)
        {
            fprintf(stderr,"Could not convert skeleton to vector..\n");
            return result;
        }
    return result;
}






//       ./GroundTruthDumper --from dataset/gestures/comeleft.bvh --360 1 --bvh test.bvh
//       ./convertBVHToCSV --from test.bvh -o test.csv
//       ./MocapNETJSON --from test.csv --visualize



int convertBVHFrameToSkeletonCOCO(struct skeletonStructure * sk,std::vector<float>  bvhFrame,unsigned int width ,unsigned int height)
{ 
          std::vector<std::vector<float> > bvhFrame2DOutput = convertBVHFrameTo2DPoints(bvhFrame); //,width,height
           
           const int INVALID_JOINT=6666;
          fprintf(stderr,"Converting BVH frame to a SkeletonCOCO assuming a  %ux%u frame\n",width,height);
           for (unsigned int jointID=0; jointID<bvhFrame2DOutput.size();  jointID++)
           {
                   fprintf(stderr,"Joint %u - %s - %0.2f,%0.2f \n",jointID,getBVHJointName(jointID),bvhFrame2DOutput[jointID][0],bvhFrame2DOutput[jointID][1]);
                   
                  const  char * jointName = getBVHJointName(jointID);
                  unsigned int jointTargetID = INVALID_JOINT;
                   if (strcmp("hip",jointName)==0)     { jointTargetID=BODY25_MidHip; }  else
                   if (strcmp("neck",jointName)==0) { jointTargetID=BODY25_Neck;      }  else
                   if (strcmp("head",jointName)==0) { jointTargetID=BODY25_Nose;     }  else
                   if (strcmp("rShldr",jointName)==0)  { jointTargetID=BODY25_RShoulder;  }  else
                   if (strcmp("rForeArm",jointName)==0)  { jointTargetID=BODY25_RElbow; }    else
                   if (strcmp("rHand",jointName)==0)  { jointTargetID=BODY25_RWrist;  }  else
                   if (strcmp("lShldr",jointName)==0)  { jointTargetID=BODY25_LShoulder;  }  else
                   if (strcmp("lForeArm",jointName)==0)  { jointTargetID=BODY25_LElbow; }  else
                   if (strcmp("lHand",jointName)==0)  { jointTargetID=BODY25_LWrist;  }  else
                   if (strcmp("rThigh",jointName)==0)  { jointTargetID=BODY25_RHip;  }  else
                   if (strcmp("rShin",jointName)==0)  { jointTargetID=BODY25_RKnee;  }  else
                   if (strcmp("rFoot",jointName)==0)  { jointTargetID=BODY25_RAnkle; }  else
                   if (strcmp("lThigh",jointName)==0)  { jointTargetID=BODY25_LHip;  }  else
                   if (strcmp("lShin",jointName)==0)  { jointTargetID=BODY25_LKnee;  }  else
                   if (strcmp("lFoot",jointName)==0)  { jointTargetID=BODY25_LAnkle; }  else
                   if (strcmp("rightEye",jointName)==0)  { jointTargetID=BODY25_REye;  }  else
                   if (strcmp("leftEye",jointName)==0)  { jointTargetID=BODY25_LEye;  }   
                                                                                           
                   if (jointTargetID!=INVALID_JOINT)
                   {
                       sk->body.joint2D[jointTargetID].x=(float) bvhFrame2DOutput[jointID][0];
                       sk->body.joint2D[jointTargetID].y=(float) bvhFrame2DOutput[jointID][1]; 
                     fprintf(stderr,"-> %0.2f,%0.2f \n",sk->body.joint2D[jointTargetID].x,sk->body.joint2D[jointTargetID].y);
                   }                                                                                           
                   /*  
    ,                      //,
    ,                        //, 
    BODY25_Nose,                        //,
    BODY25_Nose,                        //,
    BODY25_REye,                        //BODY25_REar,
    BODY25_LEye,                        //BODY25_LEar,
    BODY25_LHeel,                       //BODY25_LBigToe,
    BODY25_LBigToe,                     //BODY25_LSmallToe,
    BODY25_LAnkle,                      //BODY25_LHeel,
    BODY25_RHeel,                       //BODY25_RBigToe,
    BODY25_RBigToe,                     //BODY25_RSmallToe,
    BODY25_RAnkle,                      // BODY25_RHeel,
    BODY25_Bkg                          //BODY25_Bkg
                   */
                   
                   
           }  
          
          return 0;
}