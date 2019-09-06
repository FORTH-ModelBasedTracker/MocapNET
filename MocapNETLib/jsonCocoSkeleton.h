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
    COCO_Hand_Wrist=0,
    COCO_Hand_Thumb_1,
    COCO_Hand_Thumb_2,
    COCO_Hand_Thumb_3,
    COCO_Hand_Index_1,
    COCO_Hand_Index_2,
    COCO_Hand_Index_3,
    COCO_Hand_Middle_1,
    COCO_Hand_Middle_2,
    COCO_Hand_Middle_3,
    COCO_Hand_Ring_1,
    COCO_Hand_Ring_2,
    COCO_Hand_Ring_3,
    COCO_Hand_Pinky_1,
    COCO_Hand_Pinky_2,
    COCO_Hand_Pinky_3,
    //---------------------
    COCO_HAND_PARTS
};



/**
 * @brief A C struct to hold a hand. It contains its 2D points, its 3D points and some flags that signal
 */
struct handCOCO
{
    int isLeft;
    int isRight;

    struct point2D joint2D[COCO_HAND_PARTS];
    float  jointAccuracy[COCO_HAND_PARTS];
    unsigned int active[COCO_HAND_PARTS];
    struct point3D joint[COCO_HAND_PARTS];
};


/**
 * @brief A C struct to hold a skeleton. It contains its 2D points, its 3D points and some flags that signal
 */
struct skeletonCOCO
{
    unsigned int observationNumber , observationTotal;
    unsigned int userID;

    struct point2D joint2D[BODY25_PARTS];
    float  jointAccuracy[BODY25_PARTS];
    unsigned int active[BODY25_PARTS];
    struct point3D joint[BODY25_PARTS];
    struct point3D bbox[8];

    struct handCOCO leftHand;
    struct handCOCO rightHand;
};


/**
 * @brief Parse a JSON file and retrieve a skeleton
 * @param Path to JSON file
 * @param Pointer to a struct skeletonCOCO that will hold the information loaded
 * @retval 1=Success/0=Failure
 */
int parseJsonCOCOSkeleton(
    const char * filename ,
    struct skeletonCOCO * skel
);


#ifdef __cplusplus
}
#endif

#endif // JSONCOCOSKELETON_H_INCLUDED
