#pragma once

#include <stdio.h>

/**
 * @brief A C struct to hold a 2D coordinate
 */
struct point2D
{
    float x,y;
};

/**
 * @brief A C struct to hold a 3D coordinate
 */
struct point3D
{
    float x,y,z;
};


/**
 * @brief A structure to hold a bounding box
 */
struct boundingBox
{
    char populated;
    float minimumX;
    float maximumX;
    float minimumY;
    float maximumY;
};


/**
 * @brief This is an array of names for all the old COCO body parts.
 */
static const char * COCOOldBodyNames[] =
{
    "Nose",        //0
    "Neck",        //1
    "RShoulder",   //2
    "RElbow",      //3
    "RWrist",      //4
    "LShoulder",   //5
    "LElbow",      //6
    "LWrist",      //7
    "RHip",        //8
    "RKnee",       //9
    "RAnkle",      //10
    "LHip",        //11
    "LKnee",       //12
    "LAnkle",      //13
    "REye",        //14
    "LEye",        //15
    "REar",        //16
    "LEar",        //17
    "Bkg",         //18
//=================
    "End of Joint Names"
};



/**
 * @brief An enumerator of coco skeleton joints
 */
//https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/media/keypoints_pose_18.png
enum COCOOldSkeletonJoints
{
    COCO_Nose=0,
    COCO_Neck,
    COCO_RShoulder,
    COCO_RElbow,
    COCO_RWrist,
    COCO_LShoulder,
    COCO_LElbow,
    COCO_LWrist,
    COCO_RHip,
    COCO_RKnee,
    COCO_RAnkle,
    COCO_LHip,
    COCO_LKnee,
    COCO_LAnkle,
    COCO_REye,
    COCO_LEye,
    COCO_REar,
    COCO_LEar,
    COCO_Bkg,
    //---------------------
    COCO_PARTS
};


/**
 * @brief An array of indexes to the parents of coco skeleton joints
 */
static const int COCOSkeletonJointsParentRelationMap[] =
{
    // Parent                        Joint
    COCO_Nose,                        //COCO_Nose,
    COCO_Nose,                        //COCO_Neck,
    COCO_Neck,                        //COCO_RShoulder,
    COCO_RShoulder,                   //COCO_RElbow,
    COCO_RElbow,                      //COCO_RWrist,
    COCO_Neck,                        //COCO_LShoulder,
    COCO_LShoulder,                   //COCO_LElbow,
    COCO_LElbow,                      //COCO_LWrist,
    COCO_Neck,                        //COCO_RHip,
    COCO_RHip,                        //COCO_RKnee,
    COCO_RKnee,                       //COCO_RAnkle,
    COCO_Neck,                        //COCO_LHip,
    COCO_LHip,                        //COCO_LKnee,
    COCO_LKnee,                       //COCO_LAnkle,
    COCO_Nose,                        //COCO_REye,
    COCO_Nose,                        //COCO_LEye,
    COCO_REye,                        //COCO_REar,
    COCO_LEye,                        //COCO_LEar,
    COCO_Bkg                          //COCO_Bkg
};



/**
 * @brief This is an array of names for all the new BODY25 body parts.
 * This is a "friendly" array, meaning it is meant to be easy to understand and does not need to exactly correspond to the BVH file..
 */
static const char * Body25BodyFriendlyNames[] =
{
    "Nose",        //0
    "Neck",        //1
    "RShoulder",   //2
    "RElbow",      //3
    "RWrist",      //4
    "LShoulder",   //5
    "LElbow",      //6
    "LWrist",      //7
    "MidHip",      //8
    "RHip",        //9
    "RKnee",       //10
    "RAnkle",      //11
    "LHip",        //12
    "LKnee",       //13
    "LAnkle",      //14
    "REye",        //15
    "LEye",        //16
    "REar",        //17
    "LEar",        //18
    "LBigToe",     //19
    "LSmallToe",   //20
    "LHeel",       //21
    "RBigToe",     //22
    "RSmallToe",   //23
    "RHeel",       //24
    "Bkg",         //25
//=================
    "End of Joint Names"
};

/**
 * @brief This is an array of names for all the new BODY25 body parts.
 * This tries to mirror body_configuration.json and everything used is lowercase exactly for this reason..
 * it also has to be the same with the bvh file headerWithHeadAndOneMotion.bvh
 */
static const char * Body25BodyNames[] =
{
    "head",            //0
    "neck",            //1
    "rshoulder",       //2
    "relbow",          //3
    "rhand",           //4
    "lshoulder",       //5
    "lelbow",          //6
    "lhand",           //7
    "hip",             //8
    "rhip",            //9
    "rknee",           //10
    "rfoot",           //11
    "lhip",            //12
    "lknee",           //13
    "lfoot",           //14
    "endsite_eye.r",   //15
    "endsite_eye.l",   //16
    "rear",            //17 ========= No correspondance
    "lear",            //18 ========= No correspondance
    "endsite_toe1-2.l",//19
    "endsite_toe5-3.l",//20
    "lheel",           //21 ========= No correspondance
    "endsite_toe1-2.r",//22
    "endsite_toe5-3.r",//23
    "rheel",           //24 ========= No correspondance
    "bkg",             //25 ========= No correspondance
    //==================
    "End of Joint Names"
};

/**
 * @brief An enumerator of BODY 25 skeleton joints
 */
enum Body25SkeletonJoints
{
    BODY25_Nose=0,
    BODY25_Neck,
    BODY25_RShoulder,
    BODY25_RElbow,
    BODY25_RWrist,
    BODY25_LShoulder,
    BODY25_LElbow,
    BODY25_LWrist,
    BODY25_MidHip,
    BODY25_RHip,
    BODY25_RKnee,
    BODY25_RAnkle,
    BODY25_LHip,
    BODY25_LKnee,
    BODY25_LAnkle,
    BODY25_REye,
    BODY25_LEye,
    BODY25_REar,
    BODY25_LEar,
    BODY25_LBigToe,
    BODY25_LSmallToe,
    BODY25_LHeel,
    BODY25_RBigToe,
    BODY25_RSmallToe,
    BODY25_RHeel,
    BODY25_Bkg,
    //---------------------
    BODY25_PARTS
};



/**
 * @brief An array of indexes to the parents of BODY25 skeleton joints
 */
static const int Body25SkeletonJointsParentRelationMap[] =
{
    // Parent                             Joint
    BODY25_Nose,                        //BODY25_Nose,
    BODY25_Nose,                        //BODY25_Neck,
    BODY25_Neck,                        //BODY25_RShoulder,
    BODY25_RShoulder,                   //BODY25_RElbow,
    BODY25_RElbow,                      //BODY25_RWrist,
    BODY25_Neck,                        //BODY25_LShoulder,
    BODY25_LShoulder,                   //BODY25_LElbow,
    BODY25_LElbow,                      //BODY25_LWrist,
    BODY25_Neck,                        //BODY25_MidHip
    BODY25_MidHip,                      //BODY25_RHip,
    BODY25_RHip,                        //BODY25_RKnee,
    BODY25_RKnee,                       //BODY25_RAnkle,
    BODY25_MidHip,                      //BODY25_LHip,
    BODY25_LHip,                        //BODY25_LKnee,
    BODY25_LKnee,                       //BODY25_LAnkle,
    BODY25_Nose,                        //BODY25_REye,
    BODY25_Nose,                        //BODY25_LEye,
    BODY25_REye,                        //BODY25_REar,
    BODY25_LEye,                        //BODY25_LEar,
    BODY25_LHeel,                       //BODY25_LBigToe,
    BODY25_LBigToe,                     //BODY25_LSmallToe,
    BODY25_LAnkle,                      //BODY25_LHeel,
    BODY25_RHeel,                       //BODY25_RBigToe,
    BODY25_RBigToe,                     //BODY25_RSmallToe,
    BODY25_RAnkle,                      // BODY25_RHeel,
    BODY25_Bkg                          //BODY25_Bkg
};





/**
 * @brief An enumerator of COCO Hand joints
 */
//https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/media/keypoints_hand.png
enum COCOHandJoints
{
    COCO_Hand_Wrist=0,//0
    COCO_Hand_Thumb_1,//1
    COCO_Hand_Thumb_2, //2
    COCO_Hand_Thumb_3, //3
    COCO_Hand_Thumb_4,//4
    COCO_Hand_Index_1,//5
    COCO_Hand_Index_2,//6
    COCO_Hand_Index_3,//7
    COCO_Hand_Index_4,//8
    COCO_Hand_Middle_1,//9
    COCO_Hand_Middle_2,//10
    COCO_Hand_Middle_3,//11
    COCO_Hand_Middle_4,//12
    COCO_Hand_Ring_1,//13
    COCO_Hand_Ring_2,//14
    COCO_Hand_Ring_3,//15
    COCO_Hand_Ring_4,//16
    COCO_Hand_Pinky_1,//17
    COCO_Hand_Pinky_2,//18
    COCO_Hand_Pinky_3,//19
    COCO_Hand_Pinky_4,//20
    //---------------------
    COCO_HAND_PARTS
};


/**
 * @brief This is an array of names for the left hand.
 * This is a "friendly" array, meaning it is meant to be easy to understand and does not need to exactly correspond to the BVH file..
 */
static const char * COCOLeftHandFriendlyNames[] =
{
     "Left_Hand_Wrist",//0
     "Left_Hand_Thumb_1",//1
     "Left_Hand_Thumb_2", //2
     "Left_Hand_Thumb_3", //3
     "Left_Hand_Thumb_4",//4
     "Left_Hand_Index_1",//5
     "Left_Hand_Index_2",//6
     "Left_Hand_Index_3",//7
     "Left_Hand_Index_4",//8
     "Left_Hand_Middle_1",//9
     "Left_Hand_Middle_2",//10
     "Left_Hand_Middle_3",//11
     "Left_Hand_Middle_4",//12
     "Left_Hand_Ring_1",//13
     "Left_Hand_Ring_2",//14
     "Left_Hand_Ring_3",//15
     "Left_Hand_Ring_4",//16
     "Left_Hand_Pinky_1",//17
     "Left_Hand_Pinky_2",//18
     "Left_Hand_Pinky_3",//19
     "Left_Hand_Pinky_4",//20
     //--------------------
     "End of Left Hand Names"
};

/**
 * @brief This is an array of names for the left hand.
 * This tries to mirror lhand_configuration.json and everything used is lowercase exactly for this reason..
 * it also has to be the same with the bvh file headerWithHeadAndOneMotion.bvh
 */
static const char * COCOLeftHandNames[] =
{
     "lhand",//0
     "lthumb",//1
     "finger1-2.l", //2
     "finger1-3.l", //3
     "endsite_finger1-3.l",//4
     "finger2-1.l",//5
     "finger2-2.l",//6
     "finger2-3.l",//7
     "endsite_finger2-3.l",//8
     "finger3-1.l",//9
     "finger3-2.l",//10
     "finger3-3.l",//11
     "endsite_finger3-3.l",//12
     "finger4-1.l",//13
     "finger4-2.l",//14
     "finger4-3.l",//15
     "endsite_finger4-3.l",//16 
     "finger5-1.l",//17
     "finger5-2.l",//18
     "finger5-3.l",//19
     "endsite_finger5-3.l",//20
     //--------------------
     "End of Left Hand Names"
};



/**
 * @brief This is an array of names for the right hand.
 * This is a "friendly" array, meaning it is meant to be easy to understand and does not need to exactly correspond to the BVH file..
 */

static const char * COCORightHandFriendlyNames[] =
{
     "Right_Hand_Wrist",//0
     "Right_Hand_Thumb_1",//1
     "Right_Hand_Thumb_2", //2
     "Right_Hand_Thumb_3", //3
     "Right_Hand_Thumb_4",//4
     "Right_Hand_Index_1",//5
     "Right_Hand_Index_2",//6
     "Right_Hand_Index_3",//7
     "Right_Hand_Index_4",//8
     "Right_Hand_Middle_1",//9
     "Right_Hand_Middle_2",//10
     "Right_Hand_Middle_3",//11
     "Right_Hand_Middle_4",//12
     "Right_Hand_Ring_1",//13
     "Right_Hand_Ring_2",//14
     "Right_Hand_Ring_3",//15
     "Right_Hand_Ring_4",//16
     "Right_Hand_Pinky_1",//17
     "Right_Hand_Pinky_2",//18
     "Right_Hand_Pinky_3",//19
     "Right_Hand_Pinky_4",//20
     //--------------------
     "End of Right Hand Names"

};


/**
 * @brief This is an array of names for the right hand.
 * This tries to mirror rhand_configuration.json and everything used is lowercase exactly for this reason..
 * it also has to be the same with the bvh file headerWithHeadAndOneMotion.bvh
 */
static const char * COCORightHandNames[] =
{
     "rhand",//0
     "rthumb",//1
     "finger1-2.r", //2
     "finger1-3.r", //3
     "endsite_finger1-3.r",//4
     "finger2-1.r",//5
     "finger2-2.r",//6
     "finger2-3.r",//7
     "endsite_finger2-3.r",//8
     "finger3-1.r",//9
     "finger3-2.r",//10
     "finger3-3.r",//11
     "endsite_finger3-3.r",//12
     "finger4-1.r",//13
     "finger4-2.r",//14
     "finger4-3.r",//15
     "endsite_finger4-3.r",//16 
     "finger5-1.r",//17
     "finger5-2.r",//18
     "finger5-3.r",//19
     "endsite_finger5-3.r",//20
     //--------------------
     "End of Right Hand Names"
};



/**
 * @brief An enumerator of COCO Hand joints
 */
//https://raw.githubusercontent.com/CMU-Perceptual-Computing-Lab/openpose/master/doc/media/keypoints_face.png
enum OP_HeadJoints
{
    OP_Head_RChin_0=0, //0
    OP_Head_RChin_1,      //1
    OP_Head_RChin_2,      //2     
    OP_Head_RChin_3,      //3
    OP_Head_RChin_4,      //4
    OP_Head_RChin_5,      //5
    OP_Head_RChin_6,      //6
    OP_Head_RChin_7,      //7
    OP_Head_Chin,             //8
    OP_Head_LChin_7     , //9
    OP_Head_LChin_6,      //10
    OP_Head_LChin_5,      //11
    OP_Head_LChin_4,      //12
    OP_Head_LChin_3,      //13
    OP_Head_LChin_2,      //14
    OP_Head_LChin_1,      //15
    OP_Head_LChin_0,      //16
    OP_Head_REyebrow_0, //17
    OP_Head_REyebrow_1, //18
    OP_Head_REyebrow_2, //19
    OP_Head_REyebrow_3, //20
    OP_Head_REyebrow_4, //21
    OP_Head_LEyebrow_4, //22
    OP_Head_LEyebrow_3, //23
    OP_Head_LEyebrow_2, //24
    OP_Head_LEyebrow_1, //25
    OP_Head_LEyebrow_0, //26
    OP_Head_NoseBone_0,//27 
    OP_Head_NoseBone_1,//28
    OP_Head_NoseBone_2,//29
    OP_Head_NoseBone_3,//30
    OP_Head_Nostrills_0,//31 
    OP_Head_Nostrills_1,//32 
    OP_Head_Nostrills_2,//33 
    OP_Head_Nostrills_3,//34 
    OP_Head_Nostrills_4,//35 
    OP_Head_REye_0,//36 
    OP_Head_REye_1,//37 
    OP_Head_REye_2,//38 
    OP_Head_REye_3,//39 
    OP_Head_REye_4,//40 
    OP_Head_REye_5,//41 
    OP_Head_LEye_0,//42 
    OP_Head_LEye_1,//43 
    OP_Head_LEye_2,//44 
    OP_Head_LEye_3,//45 
    OP_Head_LEye_4,//46 
    OP_Head_LEye_5,//47 
    OP_Head_OutMouth_0,//48 
    OP_Head_OutMouth_1,//49 
    OP_Head_OutMouth_2,//50 
    OP_Head_OutMouth_3,//51 
    OP_Head_OutMouth_4,//52 
    OP_Head_OutMouth_5,//53 
    OP_Head_OutMouth_6,//54 
    OP_Head_OutMouth_7,//55 
    OP_Head_OutMouth_8,//56 
    OP_Head_OutMouth_9,//57 
    OP_Head_OutMouth_10,//58 
    OP_Head_OutMouth_11,//59 
    OP_Head_InMouth_0,//60
    OP_Head_InMouth_1,//61
    OP_Head_InMouth_2,//62
    OP_Head_InMouth_3,//63
    OP_Head_InMouth_4,//64
    OP_Head_InMouth_5,//65
    OP_Head_InMouth_6,//66
    OP_Head_InMouth_7,//67
    OP_Head_REye,//68 
    OP_Head_LEye,//69  
    //---------------------
    OP_HEAD_PARTS
};


static const char * HeadNames[] =
{
    "Head_RChin_0", //0
    "Head_RChin_1",      //1
    "Head_RChin_2",      //2     
    "Head_RChin_3",      //3
    "Head_RChin_4",      //4
    "Head_RChin_5",      //5
    "Head_RChin_6",      //6
    "Head_RChin_7",      //7
    "Head_Chin",             //8
    "Head_LChin_7",      //9
    "Head_LChin_6",      //10
    "Head_LChin_5",      //11
    "Head_LChin_4",      //12
    "Head_LChin_3",      //13
    "Head_LChin_2",      //14
    "Head_LChin_1",      //15
    "Head_LChin_0",      //16
    "Head_REyebrow_0",//17
    "Head_REyebrow_1",//18
    "Head_REyebrow_2",//19
    "Head_REyebrow_3",//20
    "Head_REyebrow_4",//21
    "Head_LEyebrow_4",//22
    "Head_LEyebrow_3",//23
    "Head_LEyebrow_2",//24
    "Head_LEyebrow_1",//25
    "Head_LEyebrow_0",//26
    "Head_NoseBone_0",//27 
    "Head_NoseBone_1",//28
    "Head_NoseBone_2",//29
    "Head_NoseBone_3",//30
    "Head_Nostrills_0",//31 
    "Head_Nostrills_1",//32 
    "Head_Nostrills_2",//33 
    "Head_Nostrills_3",//34 
    "Head_Nostrills_4",//35 
    "Head_REye_0",//36 
    "Head_REye_1",//37 
    "Head_REye_2",//38 
    "Head_REye_3",//39 
    "Head_REye_4",//40 
    "Head_REye_5",//41 
    "Head_LEye_0",//42 
    "Head_LEye_1",//43 
    "Head_LEye_2",//44 
    "Head_LEye_3",//45 
    "Head_LEye_4",//46 
    "Head_LEye_5",//47 
    "Head_OutMouth_0",//48 
    "Head_OutMouth_1",//49 
    "Head_OutMouth_2",//50 
    "Head_OutMouth_3",//51 
    "Head_OutMouth_4",//52 
    "Head_OutMouth_5",//53 
    "Head_OutMouth_6",//54 
    "Head_OutMouth_7",//55 
    "Head_OutMouth_8",//56 
    "Head_OutMouth_9",//57 
    "Head_OutMouth_10",//58 
    "Head_OutMouth_11",//59 
    "Head_InMouth_0",//60
    "Head_InMouth_1",//61
    "Head_InMouth_2",//62
    "Head_InMouth_3",//63
    "Head_InMouth_4",//64
    "Head_InMouth_5",//65
    "Head_InMouth_6",//66
    "Head_InMouth_7",//67
    "Head_REye",//68 
    "Head_LEye"//69  
};


/**
 * @brief A C struct to hold a hand. It contains its 2D points, its 3D points and some flags that signal
 */
struct handCOCO
{
    int isPopulated;
    int isLeft;
    int isRight;

    struct point2D joint2D[COCO_HAND_PARTS];
    float  jointAccuracy[COCO_HAND_PARTS];
    unsigned int active[COCO_HAND_PARTS];
    struct point3D joint[COCO_HAND_PARTS];
};

/**
 * @brief A C struct to hold a hand. It contains its 2D points, its 3D points and some flags that signal
 */
struct headOP
{  
    int isPopulated;
    struct point2D joint2D[OP_HEAD_PARTS];
    float  jointAccuracy[OP_HEAD_PARTS];
    unsigned int active[OP_HEAD_PARTS];
    struct point3D joint[OP_HEAD_PARTS];
};


/**
 * @brief A C struct to hold a body. It contains its 2D points, its 3D points and some flags that signal
 */
struct body25OP
{  
    int isPopulated;
    struct point2D joint2D[BODY25_PARTS]; 
    struct point2D bbox2D[2];
    
    float  jointAccuracy[BODY25_PARTS];
    unsigned int active[BODY25_PARTS];
    struct point3D joint[BODY25_PARTS];
    struct point3D bbox[8];
};



/**
 * @brief A C struct to hold a skeleton. It contains its 2D points, its 3D points and some flags that signal
 */
struct skeletonStructure
{
    unsigned int observationNumber , observationTotal;
    unsigned int userID;
    unsigned int totalUsersPresent; 
    //------------------------
    struct body25OP body;
    struct handCOCO leftHand;
    struct handCOCO rightHand;
    struct headOP head;
    //------------------------
    struct boundingBox  bbox2D;
};


struct headerField
{
  unsigned int strLength;
  char * str;  
};


struct bodyField
{ 
  float value;  
};


struct skeletonSerialized
{
    unsigned int skeletonHeaderElements;
    struct headerField skeletonHeader[512];

    unsigned int skeletonBodyElements;
    struct bodyField skeletonBody[512]; 
    
    float width;
    float height;
};


static void printSkeletonSerialized(const char * msg, struct skeletonSerialized * sk )
{
    if (msg!=0)
    {
        fprintf(stderr,"printSkeletonSerialized (%s)\n",msg);
    }
    
    if (sk==0) 
    {
        fprintf(stderr,"struct skeletonSerialized that was given as input is not allocated\n");
        return; 
    }
    
    if (sk->skeletonHeaderElements != sk->skeletonBodyElements)
    {
        fprintf(stderr,"struct skeletonSerialized is corrupted since it has a different number of head and body records..\n");
        return; 
    }
    
     fprintf(stderr,"Serialization has %u elements\n",sk->skeletonHeaderElements);
    for (unsigned int skID = 0; skID<sk->skeletonHeaderElements; skID++) 
    {
        if (sk->skeletonHeader[skID].str==0)
        {
         fprintf(stderr," Serial #%u has an empty label => % 0.2f \n" ,skID , sk->skeletonBody[skID].value);     
        } else
        {
         fprintf(stderr," %s => % 0.2f \n" ,sk->skeletonHeader[skID].str , sk->skeletonBody[skID].value);
        }
    }
    
    
}