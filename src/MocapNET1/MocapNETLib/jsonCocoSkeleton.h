#ifndef JSONCOCOSKELETON_H_INCLUDED
#define JSONCOCOSKELETON_H_INCLUDED
/** @file jsonCocoSkeleton.hpp
 *  @brief This is the code needed to hold a skeleton.
 *  @author Ammar Qammaz (AmmarkoV)
 */


#ifdef __cplusplus
extern "C"
{
#endif

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
 */
static const char * Body25BodyNames[] =
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


static const char * COCOLeftHandNames[] =
{
     "Left_Hand_Wrist=0",//0
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
     "Left_Hand_Pinky_4"//20
};


static const char * COCORightHandNames[] =
{
     "Right_Hand_Wrist=0",//0
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
     "Right_Hand_Pinky_4"//20
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
    float  jointAccuracy[BODY25_PARTS];
    unsigned int active[BODY25_PARTS];
    struct point3D joint[BODY25_PARTS];
    struct point3D bbox[8];
};



/**
 * @brief A C struct to hold a skeleton. It contains its 2D points, its 3D points and some flags that signal
 */
struct skeletonCOCO
{
    unsigned int observationNumber , observationTotal;
    unsigned int userID;
    unsigned int totalUsersPresent;
 
    struct body25OP body;
    struct handCOCO leftHand;
    struct handCOCO rightHand;
    struct headOP head;
};


/**
 * @brief Parse a JSON file and retrieve a skeleton
 * @param Path to JSON file
 * @param Pointer to a struct skeletonCOCO that will hold the information loaded
 * @param Threshold to set a joint to active ( 0.4-0.5 is a good value )
 * @retval 1=Success/0=Failure
 */ 
int parseJsonCOCOSkeleton(const char * filename , struct skeletonCOCO * skel,float acceptableThreshold,unsigned int frameID);

#define MAX_CSV_HEADER_FIELDS 512

struct CSVHeaderField
{
  unsigned int strLength;
  char * str;  
};

struct CSVFileContext
{
    FILE * fp;
    unsigned int lineNumber;
    unsigned int numberOfHeaderFields;
    struct CSVHeaderField field[512];
};

int openCSVFile(struct CSVFileContext * csv,const char * filename);
int closeCSVFile(struct CSVFileContext * csv);
int parseCSVHeader(struct CSVFileContext * csv);
int parseNextCSVCOCOSkeleton(struct CSVFileContext * csv, struct skeletonCOCO * skel);


#ifdef __cplusplus
}
#endif

#endif // JSONCOCOSKELETON_H_INCLUDED
