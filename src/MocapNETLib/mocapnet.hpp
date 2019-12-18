#pragma once
/** @file mocapnet.hpp
 *  @brief The MocapNET C library
 *  As seen in https://www.youtube.com/watch?v=fH5e-KMBvM0 , the MocapNET network requires two types of input.
 *  The first is an uncompressed list of (x,y,v) joints and the second an NSDM array. To add to those the output consists of BVH
 *  frames that must be accompanied by a header. This library internally handles all of these details.
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include "../Tensorflow/tensorflow.hpp"

#include "gestureRecognition.hpp"
#include "outputFiltering.hpp"

#include <iostream>
#include <vector>



/**
 * @brief This is an array of names for the input Joints expected from MocapNET.
 * Please notice that these 57 joints have actually result to three times the number of parameters since we require
 * x,y,v ( v for visibility ) information for each joint.
 * That gives us 57*3 = 171 input parameters. Values of this array can be accessed using the enumerator MOCAPNET_Uncompressed_Joints
 * For a full list of the 171 input value labels see MocapNETUncompressedArrayNames
 */
static const char * MocapNETInputUncompressedJointNames[] =
{
  "hip",          //0
  "abdomen",      //1
  "chest",        //2
  "neck",         //3
  "head",         //4
  "lefteye",      //5
  "es_lefteye",   //6
  "righteye",     //7
  "es_righteye",  //8
  "rcollar",      //9
  "rshoulder",    //10
  "relbow",       //11
  "rhand",        //12
  "rthumb1",      //13
  "rthumb2",      //14
  "es_rthumb2",   //15
  "rindex1",      //16
  "rindex2",      //17
  "es_rindex2",   //18
  "rmid1",        //19
  "rmid2",        //20
  "es_rmid2",     //21
  "rring1",       //22
  "rring2",       //23
  "es_rring2",    //24
  "rpinky1",      //25
  "rpinky2",      //26
  "es_rpinky2",   //27
  "lcollar",      //28
  "lshoulder",    //29
  "lelbow",       //30
  "lhand",        //31
  "lthumb1",      //32
  "lthumb2",      //33
  "es_lthumb2",   //34
  "lindex1",      //35
  "lindex2",      //36
  "es_lindex2",   //37
  "lmid1",        //38
  "lmid2",        //39
  "es_lmid2",     //40
  "lring1",       //41
  "lring2",       //42
  "es_lring2",    //43
  "lpinky1",      //44
  "lpinky2",      //45
  "es_lpinky2",   //46
  "rbuttock",     //47
  "rhip",         //48
  "rknee",        //49
  "rfoot",        //50
  "es_rfoot",     //51
  "lbuttock",     //52
  "lhip",         //53
  "lknee",        //54
  "lfoot",        //55
  "es_lfoot",     //56
  "End of MocapNETUncompressedJointNames"
};



/**
 * @brief This is a programmer friendly enumerator of joint names expected from MocapNET.
 * Please notice that these 57 joints have actually result to three times the number of parameters since we require
 * x,y,v ( v for visibility ) information for each joint.
 * That gives us 57*3 = 171 input parameters. String Labels can be accessed using the MocapNETUncompressedJointNames array
 * For a full list of the 171 input value labels see MocapNETUncompressedArrayNames
 */
enum MOCAPNET_Input_Uncompressed_Joints
{
  MOCAPNET_UNCOMPRESSED_JOINT_HIP=0,        //0
  MOCAPNET_UNCOMPRESSED_JOINT_ABDOMEN,      //1
  MOCAPNET_UNCOMPRESSED_JOINT_CHEST,        //2
  MOCAPNET_UNCOMPRESSED_JOINT_NECK,         //3
  MOCAPNET_UNCOMPRESSED_JOINT_HEAD,         //4
  MOCAPNET_UNCOMPRESSED_JOINT_LEFTEYE,      //5
  MOCAPNET_UNCOMPRESSED_JOINT_ES_LEFTEYE,   //6
  MOCAPNET_UNCOMPRESSED_JOINT_RIGHTEYE,     //7
  MOCAPNET_UNCOMPRESSED_JOINT_ES_RIGHTEYE,  //8
  MOCAPNET_UNCOMPRESSED_JOINT_RCOLLAR,      //9
  MOCAPNET_UNCOMPRESSED_JOINT_RSHOULDER,    //10
  MOCAPNET_UNCOMPRESSED_JOINT_RELBOW,       //11
  MOCAPNET_UNCOMPRESSED_JOINT_RHAND,        //12
  MOCAPNET_UNCOMPRESSED_JOINT_RTHUMB1,      //13
  MOCAPNET_UNCOMPRESSED_JOINT_RTHUMB2,      //14
  MOCAPNET_UNCOMPRESSED_JOINT_ES_RTHUMB2,   //15
  MOCAPNET_UNCOMPRESSED_JOINT_RINDEX1,      //16
  MOCAPNET_UNCOMPRESSED_JOINT_RINDEX2,      //17
  MOCAPNET_UNCOMPRESSED_JOINT_ES_RINDEX2,   //18
  MOCAPNET_UNCOMPRESSED_JOINT_RMID1,        //19
  MOCAPNET_UNCOMPRESSED_JOINT_RMID2,        //20
  MOCAPNET_UNCOMPRESSED_JOINT_ES_RMID2,     //21
  MOCAPNET_UNCOMPRESSED_JOINT_RRING1,       //22
  MOCAPNET_UNCOMPRESSED_JOINT_RRING2,       //23
  MOCAPNET_UNCOMPRESSED_JOINT_ES_RRING2,    //24
  MOCAPNET_UNCOMPRESSED_JOINT_RPINKY1,      //25
  MOCAPNET_UNCOMPRESSED_JOINT_RPINKY2,      //26
  MOCAPNET_UNCOMPRESSED_JOINT_ES_RPINKY2,   //27
  MOCAPNET_UNCOMPRESSED_JOINT_LCOLLAR,      //28
  MOCAPNET_UNCOMPRESSED_JOINT_LSHOULDER,    //29
  MOCAPNET_UNCOMPRESSED_JOINT_LELBOW,       //30
  MOCAPNET_UNCOMPRESSED_JOINT_LHAND,        //31
  MOCAPNET_UNCOMPRESSED_JOINT_LTHUMB1,      //32
  MOCAPNET_UNCOMPRESSED_JOINT_LTHUMB2,      //33
  MOCAPNET_UNCOMPRESSED_JOINT_ES_LTHUMB2,   //34
  MOCAPNET_UNCOMPRESSED_JOINT_LINDEX1,      //35
  MOCAPNET_UNCOMPRESSED_JOINT_LINDEX2,      //36
  MOCAPNET_UNCOMPRESSED_JOINT_ES_LINDEX2,   //37
  MOCAPNET_UNCOMPRESSED_JOINT_LMID1,        //38
  MOCAPNET_UNCOMPRESSED_JOINT_LMID2,        //39
  MOCAPNET_UNCOMPRESSED_JOINT_ES_LMID2,     //40
  MOCAPNET_UNCOMPRESSED_JOINT_LRING1,       //41
  MOCAPNET_UNCOMPRESSED_JOINT_LRING2,       //42
  MOCAPNET_UNCOMPRESSED_JOINT_ES_LRING2,    //43
  MOCAPNET_UNCOMPRESSED_JOINT_LPINKY1,      //44
  MOCAPNET_UNCOMPRESSED_JOINT_LPINKY2,      //45
  MOCAPNET_UNCOMPRESSED_JOINT_ES_LPINKY2,   //46
  MOCAPNET_UNCOMPRESSED_JOINT_RBUTTOCK,     //47
  MOCAPNET_UNCOMPRESSED_JOINT_RHIP,         //48
  MOCAPNET_UNCOMPRESSED_JOINT_RKNEE,        //49
  MOCAPNET_UNCOMPRESSED_JOINT_RFOOT,        //50
  MOCAPNET_UNCOMPRESSED_JOINT_ES_RFOOT,     //51
  MOCAPNET_UNCOMPRESSED_JOINT_LBUTTOCK,     //52
  MOCAPNET_UNCOMPRESSED_JOINT_LHIP,         //53
  MOCAPNET_UNCOMPRESSED_JOINT_LKNEE,        //54
  MOCAPNET_UNCOMPRESSED_JOINT_LFOOT,        //55
  MOCAPNET_UNCOMPRESSED_JOINT_ES_LFOOT,     //56
   //---------------------
  MOCAPNET_UNCOMPRESSED_JOINT_PARTS
};


/**
 * @brief This is an array of names for all uncompressed inputs expected from MocapNET.
 * Please notice that these 171 values correspond to triplets of 57 x,y,v ( v for visibility ) information for each joint.
 */
static const char * MocapNETInputUncompressedArrayNames[] =
{
  //Name               abs      jid
  //------------------------------
  "2D_X_hip",          //0   -   0
  "2D_Y_hip",          //1   -   0
  "2D_vis_hip",        //2   -   0
  //------------------------------
  "2D_X_abdomen",      //3   -   1
  "2D_Y_abdomen",      //4   -   1
  "2D_vis_abdomen",    //5   -   1
  //------------------------------
  "2D_X_chest",        //6   -   2
  "2D_Y_chest",        //7   -   2
  "2D_vis_chest",      //8   -   2
  //------------------------------
  "2D_X_neck",         //9   -   3
  "2D_Y_neck",         //10  -   3
  "2D_vis_neck",       //11  -   3
  //------------------------------
  "2D_X_head",         //12  -   4
  "2D_Y_head",         //13  -   4
  "2D_vis_head",       //14  -   4
  //------------------------------
  "2D_X_lefteye",      //15  -   5
  "2D_Y_lefteye",      //16  -   5
  "2D_vis_lefteye",    //17  -   5
  //------------------------------
  "2D_X_es_lefteye",   //18  -   6
  "2D_Y_es_lefteye",   //19  -   6
  "2D_vis_es_lefteye", //20  -   6
  //------------------------------
  "2D_X_righteye",     //21  -   7
  "2D_Y_righteye",     //22  -   7
  "2D_vis_righteye",   //23  -   7
  //------------------------------
  "2D_X_es_righteye",  //24  -   8
  "2D_Y_es_righteye",  //25  -   8
  "2D_vis_es_righteye",//26  -   8
  //------------------------------

  //------------------------------
  "2D_X_rcollar",      //27  -   9
  "2D_Y_rcollar",      //28  -   9
  "2D_vis_rcollar",    //29  -   9
  //------------------------------
  "2D_X_rshoulder",    //30  -   10
  "2D_Y_rshoulder",    //31  -   10
  "2D_vis_rshoulder",  //32  -   10
  //------------------------------
  "2D_X_relbow",       //33  -   11
  "2D_Y_relbow",       //34  -   11
  "2D_vis_relbow",     //35  -   11
  //------------------------------
  "2D_X_rhand",        //36  -   12
  "2D_Y_rhand",        //37  -   12
  "2D_vis_rhand",      //38  -   12
  //------------------------------
  //------------------------------
  "2D_X_rthumb1",      //39  -   13
  "2D_Y_rthumb1",      //40  -   13
  "2D_vis_rthumb1",    //41  -   13
  //------------------------------
  "2D_X_rthumb2",      //42  -   14
  "2D_Y_rthumb2",      //43  -   14
  "2D_vis_rthumb2",    //44  -   14
  //------------------------------
  "2D_X_es_rthumb2",   //45  -   15
  "2D_Y_es_rthumb2",   //46  -   15
  "2D_vis_es_rthumb2", //47  -   15
  //------------------------------
  "2D_X_rindex1",      //48  -   16
  "2D_Y_rindex1",      //49  -   16
  "2D_vis_rindex1",    //50  -   16
  //------------------------------
  "2D_X_rindex2",      //51  -   17
  "2D_Y_rindex2",      //52  -   17
  "2D_vis_rindex2",    //53  -   17
  //------------------------------
  "2D_X_es_rindex2",   //54  -   18
  "2D_Y_es_rindex2",   //55  -   18
  "2D_vis_es_rindex2", //56  -   18
  //------------------------------
  "2D_X_rmid1",        //57  -   19
  "2D_Y_rmid1",        //58  -   19
  "2D_vis_rmid1",      //59  -   19
  //------------------------------
  "2D_X_rmid2",        //60  -   20
  "2D_Y_rmid2",        //61  -   20
  "2D_vis_rmid2",      //62  -   20
  //------------------------------
  "2D_X_es_rmid2",     //63  -   21
  "2D_Y_es_rmid2",     //64  -   21
  "2D_vis_es_rmid2",   //65  -   21
  //------------------------------
  "2D_X_rring1",       //66  -   22
  "2D_Y_rring1",       //67  -   22
  "2D_vis_rring1",     //68  -   22
  //------------------------------
  "2D_X_rring2",       //69  -   23
  "2D_Y_rring2",       //70  -   23
  "2D_vis_rring2",     //71  -   23
  //------------------------------
  "2D_X_es_rring2",    //72  -   24
  "2D_Y_es_rring2",    //73  -   24
  "2D_vis_es_rring2",  //74  -   24
  //------------------------------
  "2D_X_rpinky1",      //75  -   25
  "2D_Y_rpinky1",      //76  -   25
  "2D_vis_rpinky1",    //77  -   25
  //------------------------------
  "2D_X_rpinky2",      //78  -   26
  "2D_Y_rpinky2",      //79  -   26
  "2D_vis_rpinky2",    //80  -   26
  //------------------------------
  "2D_X_es_rpinky2",   //81  -   27
  "2D_Y_es_rpinky2",   //82  -   27
  "2D_vis_es_rpinky2", //83  -   27
  //------------------------------
  //------------------------------


    //------------------------------
  "2D_X_lcollar",      //84  -   28
  "2D_Y_lcollar",      //85  -   28
  "2D_vis_lcollar",    //86  -   28
  //------------------------------
  "2D_X_lshoulder",    //87  -   29
  "2D_Y_lshoulder",    //88  -   29
  "2D_vis_lshoulder",  //89  -   29
  //------------------------------
  "2D_X_lelbow",       //90  -   30
  "2D_Y_lelbow",       //91  -   30
  "2D_vis_lelbow",     //92  -   30
  //------------------------------
  "2D_X_lhand",        //93  -   31
  "2D_Y_lhand",        //94  -   31
  "2D_vis_lhand",      //95  -   31
  //------------------------------

  //------------------------------
  "2D_X_lthumb1",      //96    - 32
  "2D_Y_lthumb1",      //97    - 32
  "2D_vis_lthumb1",    //98    - 32
  //------------------------------
  "2D_X_lthumb2",      //99    - 33
  "2D_Y_lthumb2",      //100   - 33
  "2D_vis_lthumb2",    //101   - 33
  //------------------------------
  "2D_X_es_lthumb2",   //102   - 34
  "2D_Y_es_lthumb2",   //103   - 34
  "2D_vis_es_lthumb2", //104   - 34
  //------------------------------
  "2D_X_lindex1",      //105   - 35
  "2D_Y_lindex1",      //106   - 35
  "2D_vis_lindex1",    //107   - 35
  //------------------------------
  "2D_X_lindex2",      //108   - 36
  "2D_Y_lindex2",      //109   - 36
  "2D_vis_lindex2",    //110   - 36
  //------------------------------
  "2D_X_es_lindex2",   //111   - 37
  "2D_Y_es_lindex2",   //112   - 37
  "2D_vis_es_lindex2", //113   - 37
  //------------------------------
  "2D_X_lmid1",        //114   - 38
  "2D_Y_lmid1",        //115   - 38
  "2D_vis_lmid1",      //116   - 38
  //------------------------------
  "2D_X_lmid2",        //117   - 39
  "2D_Y_lmid2",        //118   - 39
  "2D_vis_lmid2",      //119   - 39
  //------------------------------
  "2D_X_es_lmid2",     //120   - 40
  "2D_Y_es_lmid2",     //121   - 40
  "2D_vis_es_lmid2",   //122   - 40
  //------------------------------
  "2D_X_lring1",       //123   - 41
  "2D_Y_lring1",       //124   - 41
  "2D_vis_lring1",     //125   - 41
  //------------------------------
  "2D_X_lring2",       //126   - 42
  "2D_Y_lring2",       //127   - 42
  "2D_vis_lring2",     //128   - 42
  //------------------------------
  "2D_X_es_lring2",    //129   - 43
  "2D_Y_es_lring2",    //130   - 43
  "2D_vis_es_lring2",  //131   - 43
  //------------------------------
  "2D_X_lpinky1",      //132   - 44
  "2D_Y_lpinky1",      //133   - 44
  "2D_vis_lpinky1",    //134   - 44
  //------------------------------
  "2D_X_lpinky2",      //135   - 45
  "2D_Y_lpinky2",      //136   - 45
  "2D_vis_lpinky2",    //137   - 45
  //------------------------------
  "2D_X_es_lpinky2",   //138   - 46
  "2D_Y_es_lpinky2",   //139   - 46
  "2D_vis_es_lpinky2", //140   - 46
  //------------------------------
  //------------------------------

  //------------------------------
  //------------------------------
  "2D_X_rbuttock",      //141  - 47
  "2D_Y_rbuttock",      //142  - 47
  "2D_vis_rbuttock",    //143  - 47
  //------------------------------
  "2D_X_rhip",          //144  - 48
  "2D_Y_rhip",          //145  - 48
  "2D_vis_rhip",        //146  - 48
  //------------------------------
  "2D_X_rknee",         //147  - 49
  "2D_Y_rknee",         //148  - 49
  "2D_vis_rknee",       //149  - 49
  //------------------------------
  "2D_X_rfoot",         //150  - 50
  "2D_Y_rfoot",         //151  - 50
  "2D_vis_rfoot",       //152  - 50
  //------------------------------
  "2D_X_es_rfoot",      //153  - 51
  "2D_Y_es_rfoot",      //154  - 51
  "2D_vis_es_rfoot",    //155  - 51
  //------------------------------

  //------------------------------
  "2D_X_lbuttock",      //156  - 52
  "2D_Y_lbuttock",      //157  - 52
  "2D_vis_lbuttock",    //158  - 52
  //------------------------------
  "2D_X_lhip",          //159  - 53
  "2D_Y_lhip",          //160  - 53
  "2D_vis_lhip",        //161  - 53
  //------------------------------
  "2D_X_lknee",         //162  - 54
  "2D_Y_lknee",         //163  - 54
  "2D_vis_lknee",       //164  - 54
  //------------------------------
  "2D_X_lfoot",         //165  - 55
  "2D_Y_lfoot",         //166  - 55
  "2D_vis_lfoot",       //167  - 55
  //------------------------------
  "2D_X_es_lfoot",      //168  - 56
  "2D_Y_es_lfoot",      //169  - 56
  "2D_vis_es_lfoot",    //170  - 56
  //------------------------------
//=================
    "End of MocapNETUncompressedNames Names"
};




/**
 * @brief This is the size of one dimension of the NSDM matrix. Please note that we create
 * one NSDM matrix for X coordinates, one for Y coordinates and each matrix is 17x17 so we get
 * a total number of 17x17x2 = 578 elements.
 */
static const unsigned int MocapNETInputCompressedArrayIndexesSize = 17;


/**
 * @brief An array of indexes for the construction of the NSDM matrices
 */
static const unsigned int MocapNETInputCompressedArrayIndexes[] =
{
  MOCAPNET_UNCOMPRESSED_JOINT_HIP,                        // element 0
  MOCAPNET_UNCOMPRESSED_JOINT_NECK,                       // element 1
  MOCAPNET_UNCOMPRESSED_JOINT_HEAD,                       // element 2
  //--
  MOCAPNET_UNCOMPRESSED_JOINT_RSHOULDER,                  // element 3
  MOCAPNET_UNCOMPRESSED_JOINT_RELBOW,                     // element 4
  MOCAPNET_UNCOMPRESSED_JOINT_RHAND,                      // element 5
  //--
  MOCAPNET_UNCOMPRESSED_JOINT_HIP, //SYNTHETIC POINT      // element 6
  MOCAPNET_UNCOMPRESSED_JOINT_HIP, //SYNTHETIC POINT      // element 7
  //--
  MOCAPNET_UNCOMPRESSED_JOINT_LSHOULDER,                  // element 8
  MOCAPNET_UNCOMPRESSED_JOINT_LELBOW,                     // element 9
  MOCAPNET_UNCOMPRESSED_JOINT_LHAND,                      // element 10
  //--
  MOCAPNET_UNCOMPRESSED_JOINT_RHIP,                       // element 11
  MOCAPNET_UNCOMPRESSED_JOINT_RKNEE,                      // element 12
  MOCAPNET_UNCOMPRESSED_JOINT_RFOOT,                      // element 13
  //--
  MOCAPNET_UNCOMPRESSED_JOINT_LHIP,                       // element 14
  MOCAPNET_UNCOMPRESSED_JOINT_LKNEE,                      // element 15
  MOCAPNET_UNCOMPRESSED_JOINT_LFOOT                       // element 16

};




/**
 * @brief An array with string labels for what each element of an input should be after concatenating uncompressed and compressed input.
 */
static const char * MocapNETInputUncompressedAndCompressedArrayNames[] =
{
 "2DX_hip",
 "2DY_hip",
 "visible_hip",
 "2DX_abdomen",
 "2DY_abdomen",
 "visible_abdomen",
 "2DX_chest",
 "2DY_chest",
 "visible_chest",
 "2DX_neck",
 "2DY_neck",
 "visible_neck",
 "2DX_head",
 "2DY_head",
 "visible_head",
 "2DX_lefteye",
 "2DY_lefteye",
 "visible_lefteye",
 "2DX_EndSite_lefteye",
 "2DY_EndSite_lefteye",
 "visible_EndSite_lefteye",
 "2DX_righteye",
 "2DY_righteye",
 "visible_righteye",
 "2DX_EndSite_righteye",
 "2DY_EndSite_righteye",
 "visible_EndSite_righteye",
 "2DX_rcollar",
 "2DY_rcollar",
 "visible_rcollar",
 "2DX_rshoulder",
 "2DY_rshoulder",
 "visible_rshoulder",
 "2DX_relbow",
 "2DY_relbow",
 "visible_relbow",
 "2DX_rhand",
 "2DY_rhand",
 "visible_rhand",
 "2DX_rthumb1",
 "2DY_rthumb1",
 "visible_rthumb1",
 "2DX_rthumb2",
 "2DY_rthumb2",
 "visible_rthumb2",
 "2DX_EndSite_rthumb2",
 "2DY_EndSite_rthumb2",
 "visible_EndSite_rthumb2",
 "2DX_rindex1",
 "2DY_rindex1",
 "visible_rindex1",
 "2DX_rindex2",
 "2DY_rindex2",
 "visible_rindex2",
 "2DX_EndSite_rindex2",
 "2DY_EndSite_rindex2",
 "visible_EndSite_rindex2",
 "2DX_rmid1",
 "2DY_rmid1",
 "visible_rmid1",
 "2DX_rmid2",
 "2DY_rmid2",
 "visible_rmid2",
 "2DX_EndSite_rmid2",
 "2DY_EndSite_rmid2",
 "visible_EndSite_rmid2",
 "2DX_rring1",
 "2DY_rring1",
 "visible_rring1",
 "2DX_rring2",
 "2DY_rring2",
 "visible_rring2",
 "2DX_EndSite_rring2",
 "2DY_EndSite_rring2",
 "visible_EndSite_rring2",
 "2DX_rpinky1",
 "2DY_rpinky1",
 "visible_rpinky1",
 "2DX_rpinky2",
 "2DY_rpinky2",
 "visible_rpinky2",
 "2DX_EndSite_rpinky2",
 "2DY_EndSite_rpinky2",
 "visible_EndSite_rpinky2",
 "2DX_lcollar",
 "2DY_lcollar",
 "visible_lcollar",
 "2DX_lshoulder",
 "2DY_lshoulder",
 "visible_lshoulder",
 "2DX_lelbow",
 "2DY_lelbow",
 "visible_lelbow",
 "2DX_lhand",
 "2DY_lhand",
 "visible_lhand",
 "2DX_lthumb1",
 "2DY_lthumb1",
 "visible_lthumb1",
 "2DX_lthumb2",
 "2DY_lthumb2",
 "visible_lthumb2",
 "2DX_EndSite_lthumb2",
 "2DY_EndSite_lthumb2",
 "visible_EndSite_lthumb2",
 "2DX_lindex1",
 "2DY_lindex1",
 "visible_lindex1",
 "2DX_lindex2",
 "2DY_lindex2",
 "visible_lindex2",
 "2DX_EndSite_lindex2",
 "2DY_EndSite_lindex2",
 "visible_EndSite_lindex2",
 "2DX_lmid1",
 "2DY_lmid1",
 "visible_lmid1",
 "2DX_lmid2",
 "2DY_lmid2",
 "visible_lmid2",
 "2DX_EndSite_lmid2",
 "2DY_EndSite_lmid2",
 "visible_EndSite_lmid2",
 "2DX_lring1",
 "2DY_lring1",
 "visible_lring1",
 "2DX_lring2",
 "2DY_lring2",
 "visible_lring2",
 "2DX_EndSite_lring2",
 "2DY_EndSite_lring2",
 "visible_EndSite_lring2",
 "2DX_lpinky1",
 "2DY_lpinky1",
 "visible_lpinky1",
 "2DX_lpinky2",
 "2DY_lpinky2",
 "visible_lpinky2",
 "2DX_EndSite_lpinky2",
 "2DY_EndSite_lpinky2",
 "visible_EndSite_lpinky2",
 "2DX_rbuttock",
 "2DY_rbuttock",
 "visible_rbuttock",
 "2DX_rhip",
 "2DY_rhip",
 "visible_rhip",
 "2DX_rknee",
 "2DY_rknee",
 "visible_rknee",
 "2DX_rfoot",
 "2DY_rfoot",
 "visible_rfoot",
 "2DX_EndSite_rfoot",
 "2DY_EndSite_rfoot",
 "visible_EndSite_rfoot",
 "2DX_lbuttock",
 "2DY_lbuttock",
 "visible_lbuttock",
 "2DX_lhip",
 "2DY_lhip",
 "visible_lhip",
 "2DX_lknee",
 "2DY_lknee",
 "visible_lknee",
 "2DX_lfoot",
 "2DY_lfoot",
 "visible_lfoot",
 "2DX_EndSite_lfoot",
 "2DY_EndSite_lfoot",
 "visible_EndSite_lfoot",
 "2DX_hip-2DX_hip",
 "2DY_hip-2DY_hip",
 "2DX_hip-2DX_neck",
 "2DY_hip-2DY_neck",
 "2DX_hip-2DX_head",
 "2DY_hip-2DY_head",
 "2DX_hip-2DX_rshoulder",
 "2DY_hip-2DY_rshoulder",
 "2DX_hip-2DX_relbow",
 "2DY_hip-2DY_relbow",
 "2DX_hip-2DX_rhand",
 "2DY_hip-2DY_rhand",
 "2DX_hip-2DX_hip",
 "2DY_hip-2DY_hip",
 "2DX_hip-2DX_hip",
 "2DY_hip-2DY_hip",
 "2DX_hip-2DX_lshoulder",
 "2DY_hip-2DY_lshoulder",
 "2DX_hip-2DX_lelbow",
 "2DY_hip-2DY_lelbow",
 "2DX_hip-2DX_lhand",
 "2DY_hip-2DY_lhand",
 "2DX_hip-2DX_rhip",
 "2DY_hip-2DY_rhip",
 "2DX_hip-2DX_rknee",
 "2DY_hip-2DY_rknee",
 "2DX_hip-2DX_rfoot",
 "2DY_hip-2DY_rfoot",
 "2DX_hip-2DX_lhip",
 "2DY_hip-2DY_lhip",
 "2DX_hip-2DX_lknee",
 "2DY_hip-2DY_lknee",
 "2DX_hip-2DX_lfoot",
 "2DY_hip-2DY_lfoot",
 "2DX_neck-2DX_hip",
 "2DY_neck-2DY_hip",
 "2DX_neck-2DX_neck",
 "2DY_neck-2DY_neck",
 "2DX_neck-2DX_head",
 "2DY_neck-2DY_head",
 "2DX_neck-2DX_rshoulder",
 "2DY_neck-2DY_rshoulder",
 "2DX_neck-2DX_relbow",
 "2DY_neck-2DY_relbow",
 "2DX_neck-2DX_rhand",
 "2DY_neck-2DY_rhand",
 "2DX_neck-2DX_hip",
 "2DY_neck-2DY_hip",
 "2DX_neck-2DX_hip",
 "2DY_neck-2DY_hip",
 "2DX_neck-2DX_lshoulder",
 "2DY_neck-2DY_lshoulder",
 "2DX_neck-2DX_lelbow",
 "2DY_neck-2DY_lelbow",
 "2DX_neck-2DX_lhand",
 "2DY_neck-2DY_lhand",
 "2DX_neck-2DX_rhip",
 "2DY_neck-2DY_rhip",
 "2DX_neck-2DX_rknee",
 "2DY_neck-2DY_rknee",
 "2DX_neck-2DX_rfoot",
 "2DY_neck-2DY_rfoot",
 "2DX_neck-2DX_lhip",
 "2DY_neck-2DY_lhip",
 "2DX_neck-2DX_lknee",
 "2DY_neck-2DY_lknee",
 "2DX_neck-2DX_lfoot",
 "2DY_neck-2DY_lfoot",
 "2DX_head-2DX_hip",
 "2DY_head-2DY_hip",
 "2DX_head-2DX_neck",
 "2DY_head-2DY_neck",
 "2DX_head-2DX_head",
 "2DY_head-2DY_head",
 "2DX_head-2DX_rshoulder",
 "2DY_head-2DY_rshoulder",
 "2DX_head-2DX_relbow",
 "2DY_head-2DY_relbow",
 "2DX_head-2DX_rhand",
 "2DY_head-2DY_rhand",
 "2DX_head-2DX_hip",
 "2DY_head-2DY_hip",
 "2DX_head-2DX_hip",
 "2DY_head-2DY_hip",
 "2DX_head-2DX_lshoulder",
 "2DY_head-2DY_lshoulder",
 "2DX_head-2DX_lelbow",
 "2DY_head-2DY_lelbow",
 "2DX_head-2DX_lhand",
 "2DY_head-2DY_lhand",
 "2DX_head-2DX_rhip",
 "2DY_head-2DY_rhip",
 "2DX_head-2DX_rknee",
 "2DY_head-2DY_rknee",
 "2DX_head-2DX_rfoot",
 "2DY_head-2DY_rfoot",
 "2DX_head-2DX_lhip",
 "2DY_head-2DY_lhip",
 "2DX_head-2DX_lknee",
 "2DY_head-2DY_lknee",
 "2DX_head-2DX_lfoot",
 "2DY_head-2DY_lfoot",
 "2DX_rshoulder-2DX_hip",
 "2DY_rshoulder-2DY_hip",
 "2DX_rshoulder-2DX_neck",
 "2DY_rshoulder-2DY_neck",
 "2DX_rshoulder-2DX_head",
 "2DY_rshoulder-2DY_head",
 "2DX_rshoulder-2DX_rshoulder",
 "2DY_rshoulder-2DY_rshoulder",
 "2DX_rshoulder-2DX_relbow",
 "2DY_rshoulder-2DY_relbow",
 "2DX_rshoulder-2DX_rhand",
 "2DY_rshoulder-2DY_rhand",
 "2DX_rshoulder-2DX_hip",
 "2DY_rshoulder-2DY_hip",
 "2DX_rshoulder-2DX_hip",
 "2DY_rshoulder-2DY_hip",
 "2DX_rshoulder-2DX_lshoulder",
 "2DY_rshoulder-2DY_lshoulder",
 "2DX_rshoulder-2DX_lelbow",
 "2DY_rshoulder-2DY_lelbow",
 "2DX_rshoulder-2DX_lhand",
 "2DY_rshoulder-2DY_lhand",
 "2DX_rshoulder-2DX_rhip",
 "2DY_rshoulder-2DY_rhip",
 "2DX_rshoulder-2DX_rknee",
 "2DY_rshoulder-2DY_rknee",
 "2DX_rshoulder-2DX_rfoot",
 "2DY_rshoulder-2DY_rfoot",
 "2DX_rshoulder-2DX_lhip",
 "2DY_rshoulder-2DY_lhip",
 "2DX_rshoulder-2DX_lknee",
 "2DY_rshoulder-2DY_lknee",
 "2DX_rshoulder-2DX_lfoot",
 "2DY_rshoulder-2DY_lfoot",
 "2DX_relbow-2DX_hip",
 "2DY_relbow-2DY_hip",
 "2DX_relbow-2DX_neck",
 "2DY_relbow-2DY_neck",
 "2DX_relbow-2DX_head",
 "2DY_relbow-2DY_head",
 "2DX_relbow-2DX_rshoulder",
 "2DY_relbow-2DY_rshoulder",
 "2DX_relbow-2DX_relbow",
 "2DY_relbow-2DY_relbow",
 "2DX_relbow-2DX_rhand",
 "2DY_relbow-2DY_rhand",
 "2DX_relbow-2DX_hip",
 "2DY_relbow-2DY_hip",
 "2DX_relbow-2DX_hip",
 "2DY_relbow-2DY_hip",
 "2DX_relbow-2DX_lshoulder",
 "2DY_relbow-2DY_lshoulder",
 "2DX_relbow-2DX_lelbow",
 "2DY_relbow-2DY_lelbow",
 "2DX_relbow-2DX_lhand",
 "2DY_relbow-2DY_lhand",
 "2DX_relbow-2DX_rhip",
 "2DY_relbow-2DY_rhip",
 "2DX_relbow-2DX_rknee",
 "2DY_relbow-2DY_rknee",
 "2DX_relbow-2DX_rfoot",
 "2DY_relbow-2DY_rfoot",
 "2DX_relbow-2DX_lhip",
 "2DY_relbow-2DY_lhip",
 "2DX_relbow-2DX_lknee",
 "2DY_relbow-2DY_lknee",
 "2DX_relbow-2DX_lfoot",
 "2DY_relbow-2DY_lfoot",
 "2DX_rhand-2DX_hip",
 "2DY_rhand-2DY_hip",
 "2DX_rhand-2DX_neck",
 "2DY_rhand-2DY_neck",
 "2DX_rhand-2DX_head",
 "2DY_rhand-2DY_head",
 "2DX_rhand-2DX_rshoulder",
 "2DY_rhand-2DY_rshoulder",
 "2DX_rhand-2DX_relbow",
 "2DY_rhand-2DY_relbow",
 "2DX_rhand-2DX_rhand",
 "2DY_rhand-2DY_rhand",
 "2DX_rhand-2DX_hip",
 "2DY_rhand-2DY_hip",
 "2DX_rhand-2DX_hip",
 "2DY_rhand-2DY_hip",
 "2DX_rhand-2DX_lshoulder",
 "2DY_rhand-2DY_lshoulder",
 "2DX_rhand-2DX_lelbow",
 "2DY_rhand-2DY_lelbow",
 "2DX_rhand-2DX_lhand",
 "2DY_rhand-2DY_lhand",
 "2DX_rhand-2DX_rhip",
 "2DY_rhand-2DY_rhip",
 "2DX_rhand-2DX_rknee",
 "2DY_rhand-2DY_rknee",
 "2DX_rhand-2DX_rfoot",
 "2DY_rhand-2DY_rfoot",
 "2DX_rhand-2DX_lhip",
 "2DY_rhand-2DY_lhip",
 "2DX_rhand-2DX_lknee",
 "2DY_rhand-2DY_lknee",
 "2DX_rhand-2DX_lfoot",
 "2DY_rhand-2DY_lfoot",
 "2DX_hip-2DX_hip",
 "2DY_hip-2DY_hip",
 "2DX_hip-2DX_neck",
 "2DY_hip-2DY_neck",
 "2DX_hip-2DX_head",
 "2DY_hip-2DY_head",
 "2DX_hip-2DX_rshoulder",
 "2DY_hip-2DY_rshoulder",
 "2DX_hip-2DX_relbow",
 "2DY_hip-2DY_relbow",
 "2DX_hip-2DX_rhand",
 "2DY_hip-2DY_rhand",
 "2DX_hip-2DX_hip",
 "2DY_hip-2DY_hip",
 "2DX_hip-2DX_hip",
 "2DY_hip-2DY_hip",
 "2DX_hip-2DX_lshoulder",
 "2DY_hip-2DY_lshoulder",
 "2DX_hip-2DX_lelbow",
 "2DY_hip-2DY_lelbow",
 "2DX_hip-2DX_lhand",
 "2DY_hip-2DY_lhand",
 "2DX_hip-2DX_rhip",
 "2DY_hip-2DY_rhip",
 "2DX_hip-2DX_rknee",
 "2DY_hip-2DY_rknee",
 "2DX_hip-2DX_rfoot",
 "2DY_hip-2DY_rfoot",
 "2DX_hip-2DX_lhip",
 "2DY_hip-2DY_lhip",
 "2DX_hip-2DX_lknee",
 "2DY_hip-2DY_lknee",
 "2DX_hip-2DX_lfoot",
 "2DY_hip-2DY_lfoot",
 "2DX_hip-2DX_hip",
 "2DY_hip-2DY_hip",
 "2DX_hip-2DX_neck",
 "2DY_hip-2DY_neck",
 "2DX_hip-2DX_head",
 "2DY_hip-2DY_head",
 "2DX_hip-2DX_rshoulder",
 "2DY_hip-2DY_rshoulder",
 "2DX_hip-2DX_relbow",
 "2DY_hip-2DY_relbow",
 "2DX_hip-2DX_rhand",
 "2DY_hip-2DY_rhand",
 "2DX_hip-2DX_hip",
 "2DY_hip-2DY_hip",
 "2DX_hip-2DX_hip",
 "2DY_hip-2DY_hip",
 "2DX_hip-2DX_lshoulder",
 "2DY_hip-2DY_lshoulder",
 "2DX_hip-2DX_lelbow",
 "2DY_hip-2DY_lelbow",
 "2DX_hip-2DX_lhand",
 "2DY_hip-2DY_lhand",
 "2DX_hip-2DX_rhip",
 "2DY_hip-2DY_rhip",
 "2DX_hip-2DX_rknee",
 "2DY_hip-2DY_rknee",
 "2DX_hip-2DX_rfoot",
 "2DY_hip-2DY_rfoot",
 "2DX_hip-2DX_lhip",
 "2DY_hip-2DY_lhip",
 "2DX_hip-2DX_lknee",
 "2DY_hip-2DY_lknee",
 "2DX_hip-2DX_lfoot",
 "2DY_hip-2DY_lfoot",
 "2DX_lshoulder-2DX_hip",
 "2DY_lshoulder-2DY_hip",
 "2DX_lshoulder-2DX_neck",
 "2DY_lshoulder-2DY_neck",
 "2DX_lshoulder-2DX_head",
 "2DY_lshoulder-2DY_head",
 "2DX_lshoulder-2DX_rshoulder",
 "2DY_lshoulder-2DY_rshoulder",
 "2DX_lshoulder-2DX_relbow",
 "2DY_lshoulder-2DY_relbow",
 "2DX_lshoulder-2DX_rhand",
 "2DY_lshoulder-2DY_rhand",
 "2DX_lshoulder-2DX_hip",
 "2DY_lshoulder-2DY_hip",
 "2DX_lshoulder-2DX_hip",
 "2DY_lshoulder-2DY_hip",
 "2DX_lshoulder-2DX_lshoulder",
 "2DY_lshoulder-2DY_lshoulder",
 "2DX_lshoulder-2DX_lelbow",
 "2DY_lshoulder-2DY_lelbow",
 "2DX_lshoulder-2DX_lhand",
 "2DY_lshoulder-2DY_lhand",
 "2DX_lshoulder-2DX_rhip",
 "2DY_lshoulder-2DY_rhip",
 "2DX_lshoulder-2DX_rknee",
 "2DY_lshoulder-2DY_rknee",
 "2DX_lshoulder-2DX_rfoot",
 "2DY_lshoulder-2DY_rfoot",
 "2DX_lshoulder-2DX_lhip",
 "2DY_lshoulder-2DY_lhip",
 "2DX_lshoulder-2DX_lknee",
 "2DY_lshoulder-2DY_lknee",
 "2DX_lshoulder-2DX_lfoot",
 "2DY_lshoulder-2DY_lfoot",
 "2DX_lelbow-2DX_hip",
 "2DY_lelbow-2DY_hip",
 "2DX_lelbow-2DX_neck",
 "2DY_lelbow-2DY_neck",
 "2DX_lelbow-2DX_head",
 "2DY_lelbow-2DY_head",
 "2DX_lelbow-2DX_rshoulder",
 "2DY_lelbow-2DY_rshoulder",
 "2DX_lelbow-2DX_relbow",
 "2DY_lelbow-2DY_relbow",
 "2DX_lelbow-2DX_rhand",
 "2DY_lelbow-2DY_rhand",
 "2DX_lelbow-2DX_hip",
 "2DY_lelbow-2DY_hip",
 "2DX_lelbow-2DX_hip",
 "2DY_lelbow-2DY_hip",
 "2DX_lelbow-2DX_lshoulder",
 "2DY_lelbow-2DY_lshoulder",
 "2DX_lelbow-2DX_lelbow",
 "2DY_lelbow-2DY_lelbow",
 "2DX_lelbow-2DX_lhand",
 "2DY_lelbow-2DY_lhand",
 "2DX_lelbow-2DX_rhip",
 "2DY_lelbow-2DY_rhip",
 "2DX_lelbow-2DX_rknee",
 "2DY_lelbow-2DY_rknee",
 "2DX_lelbow-2DX_rfoot",
 "2DY_lelbow-2DY_rfoot",
 "2DX_lelbow-2DX_lhip",
 "2DY_lelbow-2DY_lhip",
 "2DX_lelbow-2DX_lknee",
 "2DY_lelbow-2DY_lknee",
 "2DX_lelbow-2DX_lfoot",
 "2DY_lelbow-2DY_lfoot",
 "2DX_lhand-2DX_hip",
 "2DY_lhand-2DY_hip",
 "2DX_lhand-2DX_neck",
 "2DY_lhand-2DY_neck",
 "2DX_lhand-2DX_head",
 "2DY_lhand-2DY_head",
 "2DX_lhand-2DX_rshoulder",
 "2DY_lhand-2DY_rshoulder",
 "2DX_lhand-2DX_relbow",
 "2DY_lhand-2DY_relbow",
 "2DX_lhand-2DX_rhand",
 "2DY_lhand-2DY_rhand",
 "2DX_lhand-2DX_hip",
 "2DY_lhand-2DY_hip",
 "2DX_lhand-2DX_hip",
 "2DY_lhand-2DY_hip",
 "2DX_lhand-2DX_lshoulder",
 "2DY_lhand-2DY_lshoulder",
 "2DX_lhand-2DX_lelbow",
 "2DY_lhand-2DY_lelbow",
 "2DX_lhand-2DX_lhand",
 "2DY_lhand-2DY_lhand",
 "2DX_lhand-2DX_rhip",
 "2DY_lhand-2DY_rhip",
 "2DX_lhand-2DX_rknee",
 "2DY_lhand-2DY_rknee",
 "2DX_lhand-2DX_rfoot",
 "2DY_lhand-2DY_rfoot",
 "2DX_lhand-2DX_lhip",
 "2DY_lhand-2DY_lhip",
 "2DX_lhand-2DX_lknee",
 "2DY_lhand-2DY_lknee",
 "2DX_lhand-2DX_lfoot",
 "2DY_lhand-2DY_lfoot",
 "2DX_rhip-2DX_hip",
 "2DY_rhip-2DY_hip",
 "2DX_rhip-2DX_neck",
 "2DY_rhip-2DY_neck",
 "2DX_rhip-2DX_head",
 "2DY_rhip-2DY_head",
 "2DX_rhip-2DX_rshoulder",
 "2DY_rhip-2DY_rshoulder",
 "2DX_rhip-2DX_relbow",
 "2DY_rhip-2DY_relbow",
 "2DX_rhip-2DX_rhand",
 "2DY_rhip-2DY_rhand",
 "2DX_rhip-2DX_hip",
 "2DY_rhip-2DY_hip",
 "2DX_rhip-2DX_hip",
 "2DY_rhip-2DY_hip",
 "2DX_rhip-2DX_lshoulder",
 "2DY_rhip-2DY_lshoulder",
 "2DX_rhip-2DX_lelbow",
 "2DY_rhip-2DY_lelbow",
 "2DX_rhip-2DX_lhand",
 "2DY_rhip-2DY_lhand",
 "2DX_rhip-2DX_rhip",
 "2DY_rhip-2DY_rhip",
 "2DX_rhip-2DX_rknee",
 "2DY_rhip-2DY_rknee",
 "2DX_rhip-2DX_rfoot",
 "2DY_rhip-2DY_rfoot",
 "2DX_rhip-2DX_lhip",
 "2DY_rhip-2DY_lhip",
 "2DX_rhip-2DX_lknee",
 "2DY_rhip-2DY_lknee",
 "2DX_rhip-2DX_lfoot",
 "2DY_rhip-2DY_lfoot",
 "2DX_rknee-2DX_hip",
 "2DY_rknee-2DY_hip",
 "2DX_rknee-2DX_neck",
 "2DY_rknee-2DY_neck",
 "2DX_rknee-2DX_head",
 "2DY_rknee-2DY_head",
 "2DX_rknee-2DX_rshoulder",
 "2DY_rknee-2DY_rshoulder",
 "2DX_rknee-2DX_relbow",
 "2DY_rknee-2DY_relbow",
 "2DX_rknee-2DX_rhand",
 "2DY_rknee-2DY_rhand",
 "2DX_rknee-2DX_hip",
 "2DY_rknee-2DY_hip",
 "2DX_rknee-2DX_hip",
 "2DY_rknee-2DY_hip",
 "2DX_rknee-2DX_lshoulder",
 "2DY_rknee-2DY_lshoulder",
 "2DX_rknee-2DX_lelbow",
 "2DY_rknee-2DY_lelbow",
 "2DX_rknee-2DX_lhand",
 "2DY_rknee-2DY_lhand",
 "2DX_rknee-2DX_rhip",
 "2DY_rknee-2DY_rhip",
 "2DX_rknee-2DX_rknee",
 "2DY_rknee-2DY_rknee",
 "2DX_rknee-2DX_rfoot",
 "2DY_rknee-2DY_rfoot",
 "2DX_rknee-2DX_lhip",
 "2DY_rknee-2DY_lhip",
 "2DX_rknee-2DX_lknee",
 "2DY_rknee-2DY_lknee",
 "2DX_rknee-2DX_lfoot",
 "2DY_rknee-2DY_lfoot",
 "2DX_rfoot-2DX_hip",
 "2DY_rfoot-2DY_hip",
 "2DX_rfoot-2DX_neck",
 "2DY_rfoot-2DY_neck",
 "2DX_rfoot-2DX_head",
 "2DY_rfoot-2DY_head",
 "2DX_rfoot-2DX_rshoulder",
 "2DY_rfoot-2DY_rshoulder",
 "2DX_rfoot-2DX_relbow",
 "2DY_rfoot-2DY_relbow",
 "2DX_rfoot-2DX_rhand",
 "2DY_rfoot-2DY_rhand",
 "2DX_rfoot-2DX_hip",
 "2DY_rfoot-2DY_hip",
 "2DX_rfoot-2DX_hip",
 "2DY_rfoot-2DY_hip",
 "2DX_rfoot-2DX_lshoulder",
 "2DY_rfoot-2DY_lshoulder",
 "2DX_rfoot-2DX_lelbow",
 "2DY_rfoot-2DY_lelbow",
 "2DX_rfoot-2DX_lhand",
 "2DY_rfoot-2DY_lhand",
 "2DX_rfoot-2DX_rhip",
 "2DY_rfoot-2DY_rhip",
 "2DX_rfoot-2DX_rknee",
 "2DY_rfoot-2DY_rknee",
 "2DX_rfoot-2DX_rfoot",
 "2DY_rfoot-2DY_rfoot",
 "2DX_rfoot-2DX_lhip",
 "2DY_rfoot-2DY_lhip",
 "2DX_rfoot-2DX_lknee",
 "2DY_rfoot-2DY_lknee",
 "2DX_rfoot-2DX_lfoot",
 "2DY_rfoot-2DY_lfoot",
 "2DX_lhip-2DX_hip",
 "2DY_lhip-2DY_hip",
 "2DX_lhip-2DX_neck",
 "2DY_lhip-2DY_neck",
 "2DX_lhip-2DX_head",
 "2DY_lhip-2DY_head",
 "2DX_lhip-2DX_rshoulder",
 "2DY_lhip-2DY_rshoulder",
 "2DX_lhip-2DX_relbow",
 "2DY_lhip-2DY_relbow",
 "2DX_lhip-2DX_rhand",
 "2DY_lhip-2DY_rhand",
 "2DX_lhip-2DX_hip",
 "2DY_lhip-2DY_hip",
 "2DX_lhip-2DX_hip",
 "2DY_lhip-2DY_hip",
 "2DX_lhip-2DX_lshoulder",
 "2DY_lhip-2DY_lshoulder",
 "2DX_lhip-2DX_lelbow",
 "2DY_lhip-2DY_lelbow",
 "2DX_lhip-2DX_lhand",
 "2DY_lhip-2DY_lhand",
 "2DX_lhip-2DX_rhip",
 "2DY_lhip-2DY_rhip",
 "2DX_lhip-2DX_rknee",
 "2DY_lhip-2DY_rknee",
 "2DX_lhip-2DX_rfoot",
 "2DY_lhip-2DY_rfoot",
 "2DX_lhip-2DX_lhip",
 "2DY_lhip-2DY_lhip",
 "2DX_lhip-2DX_lknee",
 "2DY_lhip-2DY_lknee",
 "2DX_lhip-2DX_lfoot",
 "2DY_lhip-2DY_lfoot",
 "2DX_lknee-2DX_hip",
 "2DY_lknee-2DY_hip",
 "2DX_lknee-2DX_neck",
 "2DY_lknee-2DY_neck",
 "2DX_lknee-2DX_head",
 "2DY_lknee-2DY_head",
 "2DX_lknee-2DX_rshoulder",
 "2DY_lknee-2DY_rshoulder",
 "2DX_lknee-2DX_relbow",
 "2DY_lknee-2DY_relbow",
 "2DX_lknee-2DX_rhand",
 "2DY_lknee-2DY_rhand",
 "2DX_lknee-2DX_hip",
 "2DY_lknee-2DY_hip",
 "2DX_lknee-2DX_hip",
 "2DY_lknee-2DY_hip",
 "2DX_lknee-2DX_lshoulder",
 "2DY_lknee-2DY_lshoulder",
 "2DX_lknee-2DX_lelbow",
 "2DY_lknee-2DY_lelbow",
 "2DX_lknee-2DX_lhand",
 "2DY_lknee-2DY_lhand",
 "2DX_lknee-2DX_rhip",
 "2DY_lknee-2DY_rhip",
 "2DX_lknee-2DX_rknee",
 "2DY_lknee-2DY_rknee",
 "2DX_lknee-2DX_rfoot",
 "2DY_lknee-2DY_rfoot",
 "2DX_lknee-2DX_lhip",
 "2DY_lknee-2DY_lhip",
 "2DX_lknee-2DX_lknee",
 "2DY_lknee-2DY_lknee",
 "2DX_lknee-2DX_lfoot",
 "2DY_lknee-2DY_lfoot",
 "2DX_lfoot-2DX_hip",
 "2DY_lfoot-2DY_hip",
 "2DX_lfoot-2DX_neck",
 "2DY_lfoot-2DY_neck",
 "2DX_lfoot-2DX_head",
 "2DY_lfoot-2DY_head",
 "2DX_lfoot-2DX_rshoulder",
 "2DY_lfoot-2DY_rshoulder",
 "2DX_lfoot-2DX_relbow",
 "2DY_lfoot-2DY_relbow",
 "2DX_lfoot-2DX_rhand",
 "2DY_lfoot-2DY_rhand",
 "2DX_lfoot-2DX_hip",
 "2DY_lfoot-2DY_hip",
 "2DX_lfoot-2DX_hip",
 "2DY_lfoot-2DY_hip",
 "2DX_lfoot-2DX_lshoulder",
 "2DY_lfoot-2DY_lshoulder",
 "2DX_lfoot-2DX_lelbow",
 "2DY_lfoot-2DY_lelbow",
 "2DX_lfoot-2DX_lhand",
 "2DY_lfoot-2DY_lhand",
 "2DX_lfoot-2DX_rhip",
 "2DY_lfoot-2DY_rhip",
 "2DX_lfoot-2DX_rknee",
 "2DY_lfoot-2DY_rknee",
 "2DX_lfoot-2DX_rfoot",
 "2DY_lfoot-2DY_rfoot",
 "2DX_lfoot-2DX_lhip",
 "2DY_lfoot-2DY_lhip",
 "2DX_lfoot-2DX_lknee",
 "2DY_lfoot-2DY_lknee",
 "2DX_lfoot-2DX_lfoot",
 "2DY_lfoot-2DY_lfoot"
 };







/**
 * @brief An array with string labels for what each element of an input should be after concatenating uncompressed and compressed input.
 * Use ./GroundTruthDumper --from dataset/headerWithHeadAndOneMotion.bvh --printc 
 * to extract this automatically
 */
static const char * MocapNETOutputArrayNames[] =
{
"hip_Xposition", // 0
"hip_Yposition", // 1
"hip_Zposition", // 2
"hip_Zrotation", // 3
"hip_Yrotation", // 4
"hip_Xrotation", // 5
"abdomen_Zrotation", // 6
 "abdomen_Xrotation", // 7
 "abdomen_Yrotation", // 8
 "chest_Zrotation", // 9
 "chest_Xrotation", // 10
 "chest_Yrotation", // 11
 "neck_Zrotation", // 12
 "neck_Xrotation", // 13
 "neck_Yrotation", // 14
 "neck1_Zrotation", // 15
 "neck1_Xrotation", // 16
 "neck1_Yrotation", // 17
 "head_Zrotation", // 18
 "head_Xrotation", // 19
 "head_Yrotation", // 20
 "__jaw_Zrotation", // 21
 "__jaw_Xrotation", // 22
 "__jaw_Yrotation", // 23
 "jaw_Zrotation", // 24
 "jaw_Xrotation", // 25
 "jaw_Yrotation", // 26
 "special04_Zrotation", // 27
 "special04_Xrotation", // 28
 "special04_Yrotation", // 29
 "oris02_Zrotation", // 30
 "oris02_Xrotation", // 31
 "oris02_Yrotation", // 32
 "oris01_Zrotation", // 33
 "oris01_Xrotation", // 34
 "oris01_Yrotation", // 35
 "oris06.l_Zrotation", // 36
 "oris06.l_Xrotation", // 37
 "oris06.l_Yrotation", // 38
 "oris07.l_Zrotation", // 39
 "oris07.l_Xrotation", // 40
 "oris07.l_Yrotation", // 41
 "oris06.r_Zrotation", // 42
 "oris06.r_Xrotation", // 43
 "oris06.r_Yrotation", // 44
 "oris07.r_Zrotation", // 45
 "oris07.r_Xrotation", // 46
 "oris07.r_Yrotation", // 47
 "tongue00_Zrotation", // 48
 "tongue00_Xrotation", // 49
 "tongue00_Yrotation", // 50
 "tongue01_Zrotation", // 51
 "tongue01_Xrotation", // 52
 "tongue01_Yrotation", // 53
 "tongue02_Zrotation", // 54
 "tongue02_Xrotation", // 55
 "tongue02_Yrotation", // 56
 "tongue03_Zrotation", // 57
 "tongue03_Xrotation", // 58
 "tongue03_Yrotation", // 59
 "__tongue04_Zrotation", // 60
 "__tongue04_Xrotation", // 61
 "__tongue04_Yrotation", // 62
 "tongue04_Zrotation", // 63
 "tongue04_Xrotation", // 64
 "tongue04_Yrotation", // 65
 "tongue07.l_Zrotation", // 66
 "tongue07.l_Xrotation", // 67
 "tongue07.l_Yrotation", // 68
 "tongue07.r_Zrotation", // 69
 "tongue07.r_Xrotation", // 70
 "tongue07.r_Yrotation", // 71
 "tongue06.l_Zrotation", // 72
 "tongue06.l_Xrotation", // 73
 "tongue06.l_Yrotation", // 74
 "tongue06.r_Zrotation", // 75
 "tongue06.r_Xrotation", // 76
 "tongue06.r_Yrotation", // 77
 "tongue05.l_Zrotation", // 78
 "tongue05.l_Xrotation", // 79
 "tongue05.l_Yrotation", // 80
 "tongue05.r_Zrotation", // 81
 "tongue05.r_Xrotation", // 82
 "tongue05.r_Yrotation", // 83
 "__levator02.l_Zrotation", // 84
 "__levator02.l_Xrotation", // 85
 "__levator02.l_Yrotation", // 86
 "levator02.l_Zrotation", // 87
 "levator02.l_Xrotation", // 88
 "levator02.l_Yrotation", // 89
 "levator03.l_Zrotation", // 90
 "levator03.l_Xrotation", // 91
 "levator03.l_Yrotation", // 92
 "levator04.l_Zrotation", // 93
 "levator04.l_Xrotation", // 94
 "levator04.l_Yrotation", // 95
 "levator05.l_Zrotation", // 96
 "levator05.l_Xrotation", // 97
 "levator05.l_Yrotation", // 98
 "__levator02.r_Zrotation", // 99
 "__levator02.r_Xrotation", // 100
 "__levator02.r_Yrotation", // 101
 "levator02.r_Zrotation", // 102
 "levator02.r_Xrotation", // 103
 "levator02.r_Yrotation", // 104
 "levator03.r_Zrotation", // 105
 "levator03.r_Xrotation", // 106
 "levator03.r_Yrotation", // 107
 "levator04.r_Zrotation", // 108
 "levator04.r_Xrotation", // 109
 "levator04.r_Yrotation", // 110
 "levator05.r_Zrotation", // 111
 "levator05.r_Xrotation", // 112
 "levator05.r_Yrotation", // 113
 "__special01_Zrotation", // 114
 "__special01_Xrotation", // 115
 "__special01_Yrotation", // 116
 "special01_Zrotation", // 117
 "special01_Xrotation", // 118
 "special01_Yrotation", // 119
 "oris04.l_Zrotation", // 120
 "oris04.l_Xrotation", // 121
 "oris04.l_Yrotation", // 122
 "oris03.l_Zrotation", // 123
 "oris03.l_Xrotation", // 124
 "oris03.l_Yrotation", // 125
 "oris04.r_Zrotation", // 126
 "oris04.r_Xrotation", // 127
 "oris04.r_Yrotation", // 128
 "oris03.r_Zrotation", // 129
 "oris03.r_Xrotation", // 130
 "oris03.r_Yrotation", // 131
 "oris06_Zrotation", // 132
 "oris06_Xrotation", // 133
 "oris06_Yrotation", // 134
 "oris05_Zrotation", // 135
 "oris05_Xrotation", // 136
 "oris05_Yrotation", // 137
 "__special03_Zrotation", // 138
 "__special03_Xrotation", // 139
 "__special03_Yrotation", // 140
 "special03_Zrotation", // 141
 "special03_Xrotation", // 142
 "special03_Yrotation", // 143
 "__levator06.l_Zrotation", // 144
 "__levator06.l_Xrotation", // 145
 "__levator06.l_Yrotation", // 146
 "levator06.l_Zrotation", // 147
 "levator06.l_Xrotation", // 148
 "levator06.l_Yrotation", // 149
 "__levator06.r_Zrotation", // 150
 "__levator06.r_Xrotation", // 151
 "__levator06.r_Yrotation", // 152
 "levator06.r_Zrotation", // 153
 "levator06.r_Xrotation", // 154
 "levator06.r_Yrotation", // 155
 "special06.l_Zrotation", // 156
 "special06.l_Xrotation", // 157
 "special06.l_Yrotation", // 158
 "special05.l_Zrotation", // 159
 "special05.l_Xrotation", // 160
 "special05.l_Yrotation", // 161
 "eye.l_Zrotation", // 162
 "eye.l_Xrotation", // 163
 "eye.l_Yrotation", // 164
 "orbicularis03.l_Zrotation", // 165
 "orbicularis03.l_Xrotation", // 166
 "orbicularis03.l_Yrotation", // 167
 "orbicularis04.l_Zrotation", // 168
 "orbicularis04.l_Xrotation", // 169
 "orbicularis04.l_Yrotation", // 170
 "special06.r_Zrotation", // 171
 "special06.r_Xrotation", // 172
 "special06.r_Yrotation", // 173
 "special05.r_Zrotation", // 174
 "special05.r_Xrotation", // 175
 "special05.r_Yrotation", // 176
 "eye.r_Zrotation", // 177
 "eye.r_Xrotation", // 178
 "eye.r_Yrotation", // 179
 "orbicularis03.r_Zrotation", // 180
 "orbicularis03.r_Xrotation", // 181
 "orbicularis03.r_Yrotation", // 182
 "orbicularis04.r_Zrotation", // 183
 "orbicularis04.r_Xrotation", // 184
 "orbicularis04.r_Yrotation", // 185
 "__temporalis01.l_Zrotation", // 186
 "__temporalis01.l_Xrotation", // 187
 "__temporalis01.l_Yrotation", // 188
 "temporalis01.l_Zrotation", // 189
 "temporalis01.l_Xrotation", // 190
 "temporalis01.l_Yrotation", // 191
 "oculi02.l_Zrotation", // 192
 "oculi02.l_Xrotation", // 193
 "oculi02.l_Yrotation", // 194
 "oculi01.l_Zrotation", // 195
 "oculi01.l_Xrotation", // 196
 "oculi01.l_Yrotation", // 197
 "__temporalis01.r_Zrotation", // 198
 "__temporalis01.r_Xrotation", // 199
 "__temporalis01.r_Yrotation", // 200
 "temporalis01.r_Zrotation", // 201
 "temporalis01.r_Xrotation", // 202
 "temporalis01.r_Yrotation", // 203
 "oculi02.r_Zrotation", // 204
 "oculi02.r_Xrotation", // 205
 "oculi02.r_Yrotation", // 206
 "oculi01.r_Zrotation", // 207
 "oculi01.r_Xrotation", // 208
 "oculi01.r_Yrotation", // 209
 "__temporalis02.l_Zrotation", // 210
 "__temporalis02.l_Xrotation", // 211
 "__temporalis02.l_Yrotation", // 212
 "temporalis02.l_Zrotation", // 213
 "temporalis02.l_Xrotation", // 214
 "temporalis02.l_Yrotation", // 215
 "risorius02.l_Zrotation", // 216
 "risorius02.l_Xrotation", // 217
 "risorius02.l_Yrotation", // 218
 "risorius03.l_Zrotation", // 219
 "risorius03.l_Xrotation", // 220
 "risorius03.l_Yrotation", // 221
 "__temporalis02.r_Zrotation", // 222
 "__temporalis02.r_Xrotation", // 223
 "__temporalis02.r_Yrotation", // 224
 "temporalis02.r_Zrotation", // 225
 "temporalis02.r_Xrotation", // 226
 "temporalis02.r_Yrotation", // 227
 "risorius02.r_Zrotation", // 228
 "risorius02.r_Xrotation", // 229
 "risorius02.r_Yrotation", // 230
 "risorius03.r_Zrotation", // 231
 "risorius03.r_Xrotation", // 232
 "risorius03.r_Yrotation", // 233
 "rcollar_Zrotation", // 234
 "rcollar_Xrotation", // 235
 "rcollar_Yrotation", // 236
 "rshoulder_Zrotation", // 237
 "rshoulder_Xrotation", // 238
 "rshoulder_Yrotation", // 239
 "relbow_Zrotation", // 240
 "relbow_Xrotation", // 241
 "relbow_Yrotation", // 242
 "rhand_Zrotation", // 243
 "rhand_Xrotation", // 244
 "rhand_Yrotation", // 245
 "metacarpal1.r_Zrotation", // 246
 "metacarpal1.r_Xrotation", // 247
 "metacarpal1.r_Yrotation", // 248
 "finger2-1.r_Zrotation", // 249
 "finger2-1.r_Xrotation", // 250
 "finger2-1.r_Yrotation", // 251
 "finger2-2.r_Zrotation", // 252
 "finger2-2.r_Xrotation", // 253
 "finger2-2.r_Yrotation", // 254
 "finger2-3.r_Zrotation", // 255
 "finger2-3.r_Xrotation", // 256
 "finger2-3.r_Yrotation", // 257
 "metacarpal2.r_Zrotation", // 258
 "metacarpal2.r_Xrotation", // 259
 "metacarpal2.r_Yrotation", // 260
 "finger3-1.r_Zrotation", // 261
 "finger3-1.r_Xrotation", // 262
 "finger3-1.r_Yrotation", // 263
 "finger3-2.r_Zrotation", // 264
 "finger3-2.r_Xrotation", // 265
 "finger3-2.r_Yrotation", // 266
 "finger3-3.r_Zrotation", // 267
 "finger3-3.r_Xrotation", // 268
 "finger3-3.r_Yrotation", // 269
 "__metacarpal3.r_Zrotation", // 270
 "__metacarpal3.r_Xrotation", // 271
 "__metacarpal3.r_Yrotation", // 272
 "metacarpal3.r_Zrotation", // 273
 "metacarpal3.r_Xrotation", // 274
 "metacarpal3.r_Yrotation", // 275
 "finger4-1.r_Zrotation", // 276
 "finger4-1.r_Xrotation", // 277
 "finger4-1.r_Yrotation", // 278
 "finger4-2.r_Zrotation", // 279
 "finger4-2.r_Xrotation", // 280
 "finger4-2.r_Yrotation", // 281
 "finger4-3.r_Zrotation", // 282
 "finger4-3.r_Xrotation", // 283
 "finger4-3.r_Yrotation", // 284
 "__metacarpal4.r_Zrotation", // 285
 "__metacarpal4.r_Xrotation", // 286
 "__metacarpal4.r_Yrotation", // 287
 "metacarpal4.r_Zrotation", // 288
 "metacarpal4.r_Xrotation", // 289
 "metacarpal4.r_Yrotation", // 290
 "finger5-1.r_Zrotation", // 291
 "finger5-1.r_Xrotation", // 292
 "finger5-1.r_Yrotation", // 293
 "finger5-2.r_Zrotation", // 294
 "finger5-2.r_Xrotation", // 295
 "finger5-2.r_Yrotation", // 296
 "finger5-3.r_Zrotation", // 297
 "finger5-3.r_Xrotation", // 298
 "finger5-3.r_Yrotation", // 299
 "__rthumb_Zrotation", // 300
 "__rthumb_Xrotation", // 301
 "__rthumb_Yrotation", // 302
 "rthumb_Zrotation", // 303
 "rthumb_Xrotation", // 304
 "rthumb_Yrotation", // 305
 "finger1-2.r_Zrotation", // 306
 "finger1-2.r_Xrotation", // 307
 "finger1-2.r_Yrotation", // 308
 "finger1-3.r_Zrotation", // 309
 "finger1-3.r_Xrotation", // 310
 "finger1-3.r_Yrotation", // 311
 "lcollar_Zrotation", // 312
 "lcollar_Xrotation", // 313
 "lcollar_Yrotation", // 314
 "lshoulder_Zrotation", // 315
 "lshoulder_Xrotation", // 316
 "lshoulder_Yrotation", // 317
 "lelbow_Zrotation", // 318
 "lelbow_Xrotation", // 319
 "lelbow_Yrotation", // 320
 "lhand_Zrotation", // 321
 "lhand_Xrotation", // 322
 "lhand_Yrotation", // 323
 "metacarpal1.l_Zrotation", // 324
 "metacarpal1.l_Xrotation", // 325
 "metacarpal1.l_Yrotation", // 326
 "finger2-1.l_Zrotation", // 327
 "finger2-1.l_Xrotation", // 328
 "finger2-1.l_Yrotation", // 329
 "finger2-2.l_Zrotation", // 330
 "finger2-2.l_Xrotation", // 331
 "finger2-2.l_Yrotation", // 332
 "finger2-3.l_Zrotation", // 333
 "finger2-3.l_Xrotation", // 334
 "finger2-3.l_Yrotation", // 335
 "metacarpal2.l_Zrotation", // 336
 "metacarpal2.l_Xrotation", // 337
 "metacarpal2.l_Yrotation", // 338
 "finger3-1.l_Zrotation", // 339
 "finger3-1.l_Xrotation", // 340
 "finger3-1.l_Yrotation", // 341
 "finger3-2.l_Zrotation", // 342
 "finger3-2.l_Xrotation", // 343
 "finger3-2.l_Yrotation", // 344
 "finger3-3.l_Zrotation", // 345
 "finger3-3.l_Xrotation", // 346
 "finger3-3.l_Yrotation", // 347
 "__metacarpal3.l_Zrotation", // 348
 "__metacarpal3.l_Xrotation", // 349
 "__metacarpal3.l_Yrotation", // 350
 "metacarpal3.l_Zrotation", // 351
 "metacarpal3.l_Xrotation", // 352
 "metacarpal3.l_Yrotation", // 353
 "finger4-1.l_Zrotation", // 354
 "finger4-1.l_Xrotation", // 355
 "finger4-1.l_Yrotation", // 356
 "finger4-2.l_Zrotation", // 357
 "finger4-2.l_Xrotation", // 358
 "finger4-2.l_Yrotation", // 359
 "finger4-3.l_Zrotation", // 360
 "finger4-3.l_Xrotation", // 361
 "finger4-3.l_Yrotation", // 362
 "__metacarpal4.l_Zrotation", // 363
 "__metacarpal4.l_Xrotation", // 364
 "__metacarpal4.l_Yrotation", // 365
 "metacarpal4.l_Zrotation", // 366
 "metacarpal4.l_Xrotation", // 367
 "metacarpal4.l_Yrotation", // 368
 "finger5-1.l_Zrotation", // 369
 "finger5-1.l_Xrotation", // 370
 "finger5-1.l_Yrotation", // 371
 "finger5-2.l_Zrotation", // 372
 "finger5-2.l_Xrotation", // 373
 "finger5-2.l_Yrotation", // 374
 "finger5-3.l_Zrotation", // 375
 "finger5-3.l_Xrotation", // 376
 "finger5-3.l_Yrotation", // 377
 "__lthumb_Zrotation", // 378
 "__lthumb_Xrotation", // 379
 "__lthumb_Yrotation", // 380
 "lthumb_Zrotation", // 381
 "lthumb_Xrotation", // 382
 "lthumb_Yrotation", // 383
 "finger1-2.l_Zrotation", // 384
 "finger1-2.l_Xrotation", // 385
 "finger1-2.l_Yrotation", // 386
 "finger1-3.l_Zrotation", // 387
 "finger1-3.l_Xrotation", // 388
 "finger1-3.l_Yrotation", // 389
 "rbuttock_Zrotation", // 390
 "rbuttock_Xrotation", // 391
 "rbuttock_Yrotation", // 392
 "rhip_Zrotation", // 393
 "rhip_Xrotation", // 394
 "rhip_Yrotation", // 395
 "rknee_Zrotation", // 396
 "rknee_Xrotation", // 397
 "rknee_Yrotation", // 398
 "rfoot_Zrotation", // 399
 "rfoot_Xrotation", // 400
 "rfoot_Yrotation", // 401
 "toe1-1.r_Zrotation", // 402
 "toe1-1.r_Xrotation", // 403
 "toe1-1.r_Yrotation", // 404
 "toe1-2.r_Zrotation", // 405
 "toe1-2.r_Xrotation", // 406
 "toe1-2.r_Yrotation", // 407
 "toe2-1.r_Zrotation", // 408
 "toe2-1.r_Xrotation", // 409
 "toe2-1.r_Yrotation", // 410
 "toe2-2.r_Zrotation", // 411
 "toe2-2.r_Xrotation", // 412
 "toe2-2.r_Yrotation", // 413
 "toe2-3.r_Zrotation", // 414
 "toe2-3.r_Xrotation", // 415
 "toe2-3.r_Yrotation", // 416
 "toe3-1.r_Zrotation", // 417
 "toe3-1.r_Xrotation", // 418
 "toe3-1.r_Yrotation", // 419
 "toe3-2.r_Zrotation", // 420
 "toe3-2.r_Xrotation", // 421
 "toe3-2.r_Yrotation", // 422
 "toe3-3.r_Zrotation", // 423
 "toe3-3.r_Xrotation", // 424
 "toe3-3.r_Yrotation", // 425
 "toe4-1.r_Zrotation", // 426
 "toe4-1.r_Xrotation", // 427
 "toe4-1.r_Yrotation", // 428
 "toe4-2.r_Zrotation", // 429
 "toe4-2.r_Xrotation", // 430
 "toe4-2.r_Yrotation", // 431
 "toe4-3.r_Zrotation", // 432
 "toe4-3.r_Xrotation", // 433
 "toe4-3.r_Yrotation", // 434
 "toe5-1.r_Zrotation", // 435
 "toe5-1.r_Xrotation", // 436
 "toe5-1.r_Yrotation", // 437
 "toe5-2.r_Zrotation", // 438
 "toe5-2.r_Xrotation", // 439
 "toe5-2.r_Yrotation", // 440
 "toe5-3.r_Zrotation", // 441
 "toe5-3.r_Xrotation", // 442
 "toe5-3.r_Yrotation", // 443
 "lbuttock_Zrotation", // 444
 "lbuttock_Xrotation", // 445
 "lbuttock_Yrotation", // 446
 "lhip_Zrotation", // 447
 "lhip_Xrotation", // 448
 "lhip_Yrotation", // 449
 "lknee_Zrotation", // 450
 "lknee_Xrotation", // 451
 "lknee_Yrotation", // 452
 "lfoot_Zrotation", // 453
 "lfoot_Xrotation", // 454
 "lfoot_Yrotation", // 455
 "toe1-1.l_Zrotation", // 456
 "toe1-1.l_Xrotation", // 457
 "toe1-1.l_Yrotation", // 458
 "toe1-2.l_Zrotation", // 459
 "toe1-2.l_Xrotation", // 460
 "toe1-2.l_Yrotation", // 461
 "toe2-1.l_Zrotation", // 462
 "toe2-1.l_Xrotation", // 463
 "toe2-1.l_Yrotation", // 464
 "toe2-2.l_Zrotation", // 465
 "toe2-2.l_Xrotation", // 466
 "toe2-2.l_Yrotation", // 467
 "toe2-3.l_Zrotation", // 468
 "toe2-3.l_Xrotation", // 469
 "toe2-3.l_Yrotation", // 470
 "toe3-1.l_Zrotation", // 471
 "toe3-1.l_Xrotation", // 472
 "toe3-1.l_Yrotation", // 473
 "toe3-2.l_Zrotation", // 474
 "toe3-2.l_Xrotation", // 475
 "toe3-2.l_Yrotation", // 476
 "toe3-3.l_Zrotation", // 477
 "toe3-3.l_Xrotation", // 478
 "toe3-3.l_Yrotation", // 479
 "toe4-1.l_Zrotation", // 480
 "toe4-1.l_Xrotation", // 481
 "toe4-1.l_Yrotation", // 482
 "toe4-2.l_Zrotation", // 483
 "toe4-2.l_Xrotation", // 484
 "toe4-2.l_Yrotation", // 485
 "toe4-3.l_Zrotation", // 486
 "toe4-3.l_Xrotation", // 487
 "toe4-3.l_Yrotation", // 488
 "toe5-1.l_Zrotation", // 489
 "toe5-1.l_Xrotation", // 490
 "toe5-1.l_Yrotation", // 491
 "toe5-2.l_Zrotation", // 492
 "toe5-2.l_Xrotation", // 493
 "toe5-2.l_Yrotation", // 494
 "toe5-3.l_Zrotation", // 495
 "toe5-3.l_Xrotation", // 496
 "toe5-3.l_Yrotation"  // 497
};



/**
 * @brief This is a programmer friendly enumerator of joint output extracted from MocapNET.
 * Use ./GroundTruthDumper --from dataset/headerWithHeadAndOneMotion.bvh --printc 
 * to extract this automatically
 */
enum MOCAPNET_Output_Joints
{
MOCAPNET_OUTPUT_HIP_XPOSITION = 0,
MOCAPNET_OUTPUT_HIP_YPOSITION,//1 
MOCAPNET_OUTPUT_HIP_ZPOSITION,//2 
MOCAPNET_OUTPUT_HIP_ZROTATION,//3 
MOCAPNET_OUTPUT_HIP_YROTATION,//4 
MOCAPNET_OUTPUT_HIP_XROTATION,//5 
MOCAPNET_OUTPUT_ABDOMEN_ZROTATION,//6 
MOCAPNET_OUTPUT_ABDOMEN_XROTATION,//7 
MOCAPNET_OUTPUT_ABDOMEN_YROTATION,//8 
MOCAPNET_OUTPUT_CHEST_ZROTATION,//9 
MOCAPNET_OUTPUT_CHEST_XROTATION,//10 
MOCAPNET_OUTPUT_CHEST_YROTATION,//11 
MOCAPNET_OUTPUT_NECK_ZROTATION,//12 
MOCAPNET_OUTPUT_NECK_XROTATION,//13 
MOCAPNET_OUTPUT_NECK_YROTATION,//14 
MOCAPNET_OUTPUT_NECK1_ZROTATION,//15 
MOCAPNET_OUTPUT_NECK1_XROTATION,//16 
MOCAPNET_OUTPUT_NECK1_YROTATION,//17 
MOCAPNET_OUTPUT_HEAD_ZROTATION,//18 
MOCAPNET_OUTPUT_HEAD_XROTATION,//19 
MOCAPNET_OUTPUT_HEAD_YROTATION,//20 
MOCAPNET_OUTPUT___JAW_ZROTATION,//21 
MOCAPNET_OUTPUT___JAW_XROTATION,//22 
MOCAPNET_OUTPUT___JAW_YROTATION,//23 
MOCAPNET_OUTPUT_JAW_ZROTATION,//24 
MOCAPNET_OUTPUT_JAW_XROTATION,//25 
MOCAPNET_OUTPUT_JAW_YROTATION,//26 
MOCAPNET_OUTPUT_SPECIAL04_ZROTATION,//27 
MOCAPNET_OUTPUT_SPECIAL04_XROTATION,//28 
MOCAPNET_OUTPUT_SPECIAL04_YROTATION,//29 
MOCAPNET_OUTPUT_ORIS02_ZROTATION,//30 
MOCAPNET_OUTPUT_ORIS02_XROTATION,//31 
MOCAPNET_OUTPUT_ORIS02_YROTATION,//32 
MOCAPNET_OUTPUT_ORIS01_ZROTATION,//33 
MOCAPNET_OUTPUT_ORIS01_XROTATION,//34 
MOCAPNET_OUTPUT_ORIS01_YROTATION,//35 
MOCAPNET_OUTPUT_ORIS06_L_ZROTATION,//36 
MOCAPNET_OUTPUT_ORIS06_L_XROTATION,//37 
MOCAPNET_OUTPUT_ORIS06_L_YROTATION,//38 
MOCAPNET_OUTPUT_ORIS07_L_ZROTATION,//39 
MOCAPNET_OUTPUT_ORIS07_L_XROTATION,//40 
MOCAPNET_OUTPUT_ORIS07_L_YROTATION,//41 
MOCAPNET_OUTPUT_ORIS06_R_ZROTATION,//42 
MOCAPNET_OUTPUT_ORIS06_R_XROTATION,//43 
MOCAPNET_OUTPUT_ORIS06_R_YROTATION,//44 
MOCAPNET_OUTPUT_ORIS07_R_ZROTATION,//45 
MOCAPNET_OUTPUT_ORIS07_R_XROTATION,//46 
MOCAPNET_OUTPUT_ORIS07_R_YROTATION,//47 
MOCAPNET_OUTPUT_TONGUE00_ZROTATION,//48 
MOCAPNET_OUTPUT_TONGUE00_XROTATION,//49 
MOCAPNET_OUTPUT_TONGUE00_YROTATION,//50 
MOCAPNET_OUTPUT_TONGUE01_ZROTATION,//51 
MOCAPNET_OUTPUT_TONGUE01_XROTATION,//52 
MOCAPNET_OUTPUT_TONGUE01_YROTATION,//53 
MOCAPNET_OUTPUT_TONGUE02_ZROTATION,//54 
MOCAPNET_OUTPUT_TONGUE02_XROTATION,//55 
MOCAPNET_OUTPUT_TONGUE02_YROTATION,//56 
MOCAPNET_OUTPUT_TONGUE03_ZROTATION,//57 
MOCAPNET_OUTPUT_TONGUE03_XROTATION,//58 
MOCAPNET_OUTPUT_TONGUE03_YROTATION,//59 
MOCAPNET_OUTPUT___TONGUE04_ZROTATION,//60 
MOCAPNET_OUTPUT___TONGUE04_XROTATION,//61 
MOCAPNET_OUTPUT___TONGUE04_YROTATION,//62 
MOCAPNET_OUTPUT_TONGUE04_ZROTATION,//63 
MOCAPNET_OUTPUT_TONGUE04_XROTATION,//64 
MOCAPNET_OUTPUT_TONGUE04_YROTATION,//65 
MOCAPNET_OUTPUT_TONGUE07_L_ZROTATION,//66 
MOCAPNET_OUTPUT_TONGUE07_L_XROTATION,//67 
MOCAPNET_OUTPUT_TONGUE07_L_YROTATION,//68 
MOCAPNET_OUTPUT_TONGUE07_R_ZROTATION,//69 
MOCAPNET_OUTPUT_TONGUE07_R_XROTATION,//70 
MOCAPNET_OUTPUT_TONGUE07_R_YROTATION,//71 
MOCAPNET_OUTPUT_TONGUE06_L_ZROTATION,//72 
MOCAPNET_OUTPUT_TONGUE06_L_XROTATION,//73 
MOCAPNET_OUTPUT_TONGUE06_L_YROTATION,//74 
MOCAPNET_OUTPUT_TONGUE06_R_ZROTATION,//75 
MOCAPNET_OUTPUT_TONGUE06_R_XROTATION,//76 
MOCAPNET_OUTPUT_TONGUE06_R_YROTATION,//77 
MOCAPNET_OUTPUT_TONGUE05_L_ZROTATION,//78 
MOCAPNET_OUTPUT_TONGUE05_L_XROTATION,//79 
MOCAPNET_OUTPUT_TONGUE05_L_YROTATION,//80 
MOCAPNET_OUTPUT_TONGUE05_R_ZROTATION,//81 
MOCAPNET_OUTPUT_TONGUE05_R_XROTATION,//82 
MOCAPNET_OUTPUT_TONGUE05_R_YROTATION,//83 
MOCAPNET_OUTPUT___LEVATOR02_L_ZROTATION,//84 
MOCAPNET_OUTPUT___LEVATOR02_L_XROTATION,//85 
MOCAPNET_OUTPUT___LEVATOR02_L_YROTATION,//86 
MOCAPNET_OUTPUT_LEVATOR02_L_ZROTATION,//87 
MOCAPNET_OUTPUT_LEVATOR02_L_XROTATION,//88 
MOCAPNET_OUTPUT_LEVATOR02_L_YROTATION,//89 
MOCAPNET_OUTPUT_LEVATOR03_L_ZROTATION,//90 
MOCAPNET_OUTPUT_LEVATOR03_L_XROTATION,//91 
MOCAPNET_OUTPUT_LEVATOR03_L_YROTATION,//92 
MOCAPNET_OUTPUT_LEVATOR04_L_ZROTATION,//93 
MOCAPNET_OUTPUT_LEVATOR04_L_XROTATION,//94 
MOCAPNET_OUTPUT_LEVATOR04_L_YROTATION,//95 
MOCAPNET_OUTPUT_LEVATOR05_L_ZROTATION,//96 
MOCAPNET_OUTPUT_LEVATOR05_L_XROTATION,//97 
MOCAPNET_OUTPUT_LEVATOR05_L_YROTATION,//98 
MOCAPNET_OUTPUT___LEVATOR02_R_ZROTATION,//99 
MOCAPNET_OUTPUT___LEVATOR02_R_XROTATION,//100 
MOCAPNET_OUTPUT___LEVATOR02_R_YROTATION,//101 
MOCAPNET_OUTPUT_LEVATOR02_R_ZROTATION,//102 
MOCAPNET_OUTPUT_LEVATOR02_R_XROTATION,//103 
MOCAPNET_OUTPUT_LEVATOR02_R_YROTATION,//104 
MOCAPNET_OUTPUT_LEVATOR03_R_ZROTATION,//105 
MOCAPNET_OUTPUT_LEVATOR03_R_XROTATION,//106 
MOCAPNET_OUTPUT_LEVATOR03_R_YROTATION,//107 
MOCAPNET_OUTPUT_LEVATOR04_R_ZROTATION,//108 
MOCAPNET_OUTPUT_LEVATOR04_R_XROTATION,//109 
MOCAPNET_OUTPUT_LEVATOR04_R_YROTATION,//110 
MOCAPNET_OUTPUT_LEVATOR05_R_ZROTATION,//111 
MOCAPNET_OUTPUT_LEVATOR05_R_XROTATION,//112 
MOCAPNET_OUTPUT_LEVATOR05_R_YROTATION,//113 
MOCAPNET_OUTPUT___SPECIAL01_ZROTATION,//114 
MOCAPNET_OUTPUT___SPECIAL01_XROTATION,//115 
MOCAPNET_OUTPUT___SPECIAL01_YROTATION,//116 
MOCAPNET_OUTPUT_SPECIAL01_ZROTATION,//117 
MOCAPNET_OUTPUT_SPECIAL01_XROTATION,//118 
MOCAPNET_OUTPUT_SPECIAL01_YROTATION,//119 
MOCAPNET_OUTPUT_ORIS04_L_ZROTATION,//120 
MOCAPNET_OUTPUT_ORIS04_L_XROTATION,//121 
MOCAPNET_OUTPUT_ORIS04_L_YROTATION,//122 
MOCAPNET_OUTPUT_ORIS03_L_ZROTATION,//123 
MOCAPNET_OUTPUT_ORIS03_L_XROTATION,//124 
MOCAPNET_OUTPUT_ORIS03_L_YROTATION,//125 
MOCAPNET_OUTPUT_ORIS04_R_ZROTATION,//126 
MOCAPNET_OUTPUT_ORIS04_R_XROTATION,//127 
MOCAPNET_OUTPUT_ORIS04_R_YROTATION,//128 
MOCAPNET_OUTPUT_ORIS03_R_ZROTATION,//129 
MOCAPNET_OUTPUT_ORIS03_R_XROTATION,//130 
MOCAPNET_OUTPUT_ORIS03_R_YROTATION,//131 
MOCAPNET_OUTPUT_ORIS06_ZROTATION,//132 
MOCAPNET_OUTPUT_ORIS06_XROTATION,//133 
MOCAPNET_OUTPUT_ORIS06_YROTATION,//134 
MOCAPNET_OUTPUT_ORIS05_ZROTATION,//135 
MOCAPNET_OUTPUT_ORIS05_XROTATION,//136 
MOCAPNET_OUTPUT_ORIS05_YROTATION,//137 
MOCAPNET_OUTPUT___SPECIAL03_ZROTATION,//138 
MOCAPNET_OUTPUT___SPECIAL03_XROTATION,//139 
MOCAPNET_OUTPUT___SPECIAL03_YROTATION,//140 
MOCAPNET_OUTPUT_SPECIAL03_ZROTATION,//141 
MOCAPNET_OUTPUT_SPECIAL03_XROTATION,//142 
MOCAPNET_OUTPUT_SPECIAL03_YROTATION,//143 
MOCAPNET_OUTPUT___LEVATOR06_L_ZROTATION,//144 
MOCAPNET_OUTPUT___LEVATOR06_L_XROTATION,//145 
MOCAPNET_OUTPUT___LEVATOR06_L_YROTATION,//146 
MOCAPNET_OUTPUT_LEVATOR06_L_ZROTATION,//147 
MOCAPNET_OUTPUT_LEVATOR06_L_XROTATION,//148 
MOCAPNET_OUTPUT_LEVATOR06_L_YROTATION,//149 
MOCAPNET_OUTPUT___LEVATOR06_R_ZROTATION,//150 
MOCAPNET_OUTPUT___LEVATOR06_R_XROTATION,//151 
MOCAPNET_OUTPUT___LEVATOR06_R_YROTATION,//152 
MOCAPNET_OUTPUT_LEVATOR06_R_ZROTATION,//153 
MOCAPNET_OUTPUT_LEVATOR06_R_XROTATION,//154 
MOCAPNET_OUTPUT_LEVATOR06_R_YROTATION,//155 
MOCAPNET_OUTPUT_SPECIAL06_L_ZROTATION,//156 
MOCAPNET_OUTPUT_SPECIAL06_L_XROTATION,//157 
MOCAPNET_OUTPUT_SPECIAL06_L_YROTATION,//158 
MOCAPNET_OUTPUT_SPECIAL05_L_ZROTATION,//159 
MOCAPNET_OUTPUT_SPECIAL05_L_XROTATION,//160 
MOCAPNET_OUTPUT_SPECIAL05_L_YROTATION,//161 
MOCAPNET_OUTPUT_EYE_L_ZROTATION,//162 
MOCAPNET_OUTPUT_EYE_L_XROTATION,//163 
MOCAPNET_OUTPUT_EYE_L_YROTATION,//164 
MOCAPNET_OUTPUT_ORBICULARIS03_L_ZROTATION,//165 
MOCAPNET_OUTPUT_ORBICULARIS03_L_XROTATION,//166 
MOCAPNET_OUTPUT_ORBICULARIS03_L_YROTATION,//167 
MOCAPNET_OUTPUT_ORBICULARIS04_L_ZROTATION,//168 
MOCAPNET_OUTPUT_ORBICULARIS04_L_XROTATION,//169 
MOCAPNET_OUTPUT_ORBICULARIS04_L_YROTATION,//170 
MOCAPNET_OUTPUT_SPECIAL06_R_ZROTATION,//171 
MOCAPNET_OUTPUT_SPECIAL06_R_XROTATION,//172 
MOCAPNET_OUTPUT_SPECIAL06_R_YROTATION,//173 
MOCAPNET_OUTPUT_SPECIAL05_R_ZROTATION,//174 
MOCAPNET_OUTPUT_SPECIAL05_R_XROTATION,//175 
MOCAPNET_OUTPUT_SPECIAL05_R_YROTATION,//176 
MOCAPNET_OUTPUT_EYE_R_ZROTATION,//177 
MOCAPNET_OUTPUT_EYE_R_XROTATION,//178 
MOCAPNET_OUTPUT_EYE_R_YROTATION,//179 
MOCAPNET_OUTPUT_ORBICULARIS03_R_ZROTATION,//180 
MOCAPNET_OUTPUT_ORBICULARIS03_R_XROTATION,//181 
MOCAPNET_OUTPUT_ORBICULARIS03_R_YROTATION,//182 
MOCAPNET_OUTPUT_ORBICULARIS04_R_ZROTATION,//183 
MOCAPNET_OUTPUT_ORBICULARIS04_R_XROTATION,//184 
MOCAPNET_OUTPUT_ORBICULARIS04_R_YROTATION,//185 
MOCAPNET_OUTPUT___TEMPORALIS01_L_ZROTATION,//186 
MOCAPNET_OUTPUT___TEMPORALIS01_L_XROTATION,//187 
MOCAPNET_OUTPUT___TEMPORALIS01_L_YROTATION,//188 
MOCAPNET_OUTPUT_TEMPORALIS01_L_ZROTATION,//189 
MOCAPNET_OUTPUT_TEMPORALIS01_L_XROTATION,//190 
MOCAPNET_OUTPUT_TEMPORALIS01_L_YROTATION,//191 
MOCAPNET_OUTPUT_OCULI02_L_ZROTATION,//192 
MOCAPNET_OUTPUT_OCULI02_L_XROTATION,//193 
MOCAPNET_OUTPUT_OCULI02_L_YROTATION,//194 
MOCAPNET_OUTPUT_OCULI01_L_ZROTATION,//195 
MOCAPNET_OUTPUT_OCULI01_L_XROTATION,//196 
MOCAPNET_OUTPUT_OCULI01_L_YROTATION,//197 
MOCAPNET_OUTPUT___TEMPORALIS01_R_ZROTATION,//198 
MOCAPNET_OUTPUT___TEMPORALIS01_R_XROTATION,//199 
MOCAPNET_OUTPUT___TEMPORALIS01_R_YROTATION,//200 
MOCAPNET_OUTPUT_TEMPORALIS01_R_ZROTATION,//201 
MOCAPNET_OUTPUT_TEMPORALIS01_R_XROTATION,//202 
MOCAPNET_OUTPUT_TEMPORALIS01_R_YROTATION,//203 
MOCAPNET_OUTPUT_OCULI02_R_ZROTATION,//204 
MOCAPNET_OUTPUT_OCULI02_R_XROTATION,//205 
MOCAPNET_OUTPUT_OCULI02_R_YROTATION,//206 
MOCAPNET_OUTPUT_OCULI01_R_ZROTATION,//207 
MOCAPNET_OUTPUT_OCULI01_R_XROTATION,//208 
MOCAPNET_OUTPUT_OCULI01_R_YROTATION,//209 
MOCAPNET_OUTPUT___TEMPORALIS02_L_ZROTATION,//210 
MOCAPNET_OUTPUT___TEMPORALIS02_L_XROTATION,//211 
MOCAPNET_OUTPUT___TEMPORALIS02_L_YROTATION,//212 
MOCAPNET_OUTPUT_TEMPORALIS02_L_ZROTATION,//213 
MOCAPNET_OUTPUT_TEMPORALIS02_L_XROTATION,//214 
MOCAPNET_OUTPUT_TEMPORALIS02_L_YROTATION,//215 
MOCAPNET_OUTPUT_RISORIUS02_L_ZROTATION,//216 
MOCAPNET_OUTPUT_RISORIUS02_L_XROTATION,//217 
MOCAPNET_OUTPUT_RISORIUS02_L_YROTATION,//218 
MOCAPNET_OUTPUT_RISORIUS03_L_ZROTATION,//219 
MOCAPNET_OUTPUT_RISORIUS03_L_XROTATION,//220 
MOCAPNET_OUTPUT_RISORIUS03_L_YROTATION,//221 
MOCAPNET_OUTPUT___TEMPORALIS02_R_ZROTATION,//222 
MOCAPNET_OUTPUT___TEMPORALIS02_R_XROTATION,//223 
MOCAPNET_OUTPUT___TEMPORALIS02_R_YROTATION,//224 
MOCAPNET_OUTPUT_TEMPORALIS02_R_ZROTATION,//225 
MOCAPNET_OUTPUT_TEMPORALIS02_R_XROTATION,//226 
MOCAPNET_OUTPUT_TEMPORALIS02_R_YROTATION,//227 
MOCAPNET_OUTPUT_RISORIUS02_R_ZROTATION,//228 
MOCAPNET_OUTPUT_RISORIUS02_R_XROTATION,//229 
MOCAPNET_OUTPUT_RISORIUS02_R_YROTATION,//230 
MOCAPNET_OUTPUT_RISORIUS03_R_ZROTATION,//231 
MOCAPNET_OUTPUT_RISORIUS03_R_XROTATION,//232 
MOCAPNET_OUTPUT_RISORIUS03_R_YROTATION,//233 
MOCAPNET_OUTPUT_RCOLLAR_ZROTATION,//234 
MOCAPNET_OUTPUT_RCOLLAR_XROTATION,//235 
MOCAPNET_OUTPUT_RCOLLAR_YROTATION,//236 
MOCAPNET_OUTPUT_RSHOULDER_ZROTATION,//237 
MOCAPNET_OUTPUT_RSHOULDER_XROTATION,//238 
MOCAPNET_OUTPUT_RSHOULDER_YROTATION,//239 
MOCAPNET_OUTPUT_RELBOW_ZROTATION,//240 
MOCAPNET_OUTPUT_RELBOW_XROTATION,//241 
MOCAPNET_OUTPUT_RELBOW_YROTATION,//242 
MOCAPNET_OUTPUT_RHAND_ZROTATION,//243 
MOCAPNET_OUTPUT_RHAND_XROTATION,//244 
MOCAPNET_OUTPUT_RHAND_YROTATION,//245 
MOCAPNET_OUTPUT_METACARPAL1_R_ZROTATION,//246 
MOCAPNET_OUTPUT_METACARPAL1_R_XROTATION,//247 
MOCAPNET_OUTPUT_METACARPAL1_R_YROTATION,//248 
MOCAPNET_OUTPUT_FINGER2_1_R_ZROTATION,//249 
MOCAPNET_OUTPUT_FINGER2_1_R_XROTATION,//250 
MOCAPNET_OUTPUT_FINGER2_1_R_YROTATION,//251 
MOCAPNET_OUTPUT_FINGER2_2_R_ZROTATION,//252 
MOCAPNET_OUTPUT_FINGER2_2_R_XROTATION,//253 
MOCAPNET_OUTPUT_FINGER2_2_R_YROTATION,//254 
MOCAPNET_OUTPUT_FINGER2_3_R_ZROTATION,//255 
MOCAPNET_OUTPUT_FINGER2_3_R_XROTATION,//256 
MOCAPNET_OUTPUT_FINGER2_3_R_YROTATION,//257 
MOCAPNET_OUTPUT_METACARPAL2_R_ZROTATION,//258 
MOCAPNET_OUTPUT_METACARPAL2_R_XROTATION,//259 
MOCAPNET_OUTPUT_METACARPAL2_R_YROTATION,//260 
MOCAPNET_OUTPUT_FINGER3_1_R_ZROTATION,//261 
MOCAPNET_OUTPUT_FINGER3_1_R_XROTATION,//262 
MOCAPNET_OUTPUT_FINGER3_1_R_YROTATION,//263 
MOCAPNET_OUTPUT_FINGER3_2_R_ZROTATION,//264 
MOCAPNET_OUTPUT_FINGER3_2_R_XROTATION,//265 
MOCAPNET_OUTPUT_FINGER3_2_R_YROTATION,//266 
MOCAPNET_OUTPUT_FINGER3_3_R_ZROTATION,//267 
MOCAPNET_OUTPUT_FINGER3_3_R_XROTATION,//268 
MOCAPNET_OUTPUT_FINGER3_3_R_YROTATION,//269 
MOCAPNET_OUTPUT___METACARPAL3_R_ZROTATION,//270 
MOCAPNET_OUTPUT___METACARPAL3_R_XROTATION,//271 
MOCAPNET_OUTPUT___METACARPAL3_R_YROTATION,//272 
MOCAPNET_OUTPUT_METACARPAL3_R_ZROTATION,//273 
MOCAPNET_OUTPUT_METACARPAL3_R_XROTATION,//274 
MOCAPNET_OUTPUT_METACARPAL3_R_YROTATION,//275 
MOCAPNET_OUTPUT_FINGER4_1_R_ZROTATION,//276 
MOCAPNET_OUTPUT_FINGER4_1_R_XROTATION,//277 
MOCAPNET_OUTPUT_FINGER4_1_R_YROTATION,//278 
MOCAPNET_OUTPUT_FINGER4_2_R_ZROTATION,//279 
MOCAPNET_OUTPUT_FINGER4_2_R_XROTATION,//280 
MOCAPNET_OUTPUT_FINGER4_2_R_YROTATION,//281 
MOCAPNET_OUTPUT_FINGER4_3_R_ZROTATION,//282 
MOCAPNET_OUTPUT_FINGER4_3_R_XROTATION,//283 
MOCAPNET_OUTPUT_FINGER4_3_R_YROTATION,//284 
MOCAPNET_OUTPUT___METACARPAL4_R_ZROTATION,//285 
MOCAPNET_OUTPUT___METACARPAL4_R_XROTATION,//286 
MOCAPNET_OUTPUT___METACARPAL4_R_YROTATION,//287 
MOCAPNET_OUTPUT_METACARPAL4_R_ZROTATION,//288 
MOCAPNET_OUTPUT_METACARPAL4_R_XROTATION,//289 
MOCAPNET_OUTPUT_METACARPAL4_R_YROTATION,//290 
MOCAPNET_OUTPUT_FINGER5_1_R_ZROTATION,//291 
MOCAPNET_OUTPUT_FINGER5_1_R_XROTATION,//292 
MOCAPNET_OUTPUT_FINGER5_1_R_YROTATION,//293 
MOCAPNET_OUTPUT_FINGER5_2_R_ZROTATION,//294 
MOCAPNET_OUTPUT_FINGER5_2_R_XROTATION,//295 
MOCAPNET_OUTPUT_FINGER5_2_R_YROTATION,//296 
MOCAPNET_OUTPUT_FINGER5_3_R_ZROTATION,//297 
MOCAPNET_OUTPUT_FINGER5_3_R_XROTATION,//298 
MOCAPNET_OUTPUT_FINGER5_3_R_YROTATION,//299 
MOCAPNET_OUTPUT___RTHUMB_ZROTATION,//300 
MOCAPNET_OUTPUT___RTHUMB_XROTATION,//301 
MOCAPNET_OUTPUT___RTHUMB_YROTATION,//302 
MOCAPNET_OUTPUT_RTHUMB_ZROTATION,//303 
MOCAPNET_OUTPUT_RTHUMB_XROTATION,//304 
MOCAPNET_OUTPUT_RTHUMB_YROTATION,//305 
MOCAPNET_OUTPUT_FINGER1_2_R_ZROTATION,//306 
MOCAPNET_OUTPUT_FINGER1_2_R_XROTATION,//307 
MOCAPNET_OUTPUT_FINGER1_2_R_YROTATION,//308 
MOCAPNET_OUTPUT_FINGER1_3_R_ZROTATION,//309 
MOCAPNET_OUTPUT_FINGER1_3_R_XROTATION,//310 
MOCAPNET_OUTPUT_FINGER1_3_R_YROTATION,//311 
MOCAPNET_OUTPUT_LCOLLAR_ZROTATION,//312 
MOCAPNET_OUTPUT_LCOLLAR_XROTATION,//313 
MOCAPNET_OUTPUT_LCOLLAR_YROTATION,//314 
MOCAPNET_OUTPUT_LSHOULDER_ZROTATION,//315 
MOCAPNET_OUTPUT_LSHOULDER_XROTATION,//316 
MOCAPNET_OUTPUT_LSHOULDER_YROTATION,//317 
MOCAPNET_OUTPUT_LELBOW_ZROTATION,//318 
MOCAPNET_OUTPUT_LELBOW_XROTATION,//319 
MOCAPNET_OUTPUT_LELBOW_YROTATION,//320 
MOCAPNET_OUTPUT_LHAND_ZROTATION,//321 
MOCAPNET_OUTPUT_LHAND_XROTATION,//322 
MOCAPNET_OUTPUT_LHAND_YROTATION,//323 
MOCAPNET_OUTPUT_METACARPAL1_L_ZROTATION,//324 
MOCAPNET_OUTPUT_METACARPAL1_L_XROTATION,//325 
MOCAPNET_OUTPUT_METACARPAL1_L_YROTATION,//326 
MOCAPNET_OUTPUT_FINGER2_1_L_ZROTATION,//327 
MOCAPNET_OUTPUT_FINGER2_1_L_XROTATION,//328 
MOCAPNET_OUTPUT_FINGER2_1_L_YROTATION,//329 
MOCAPNET_OUTPUT_FINGER2_2_L_ZROTATION,//330 
MOCAPNET_OUTPUT_FINGER2_2_L_XROTATION,//331 
MOCAPNET_OUTPUT_FINGER2_2_L_YROTATION,//332 
MOCAPNET_OUTPUT_FINGER2_3_L_ZROTATION,//333 
MOCAPNET_OUTPUT_FINGER2_3_L_XROTATION,//334 
MOCAPNET_OUTPUT_FINGER2_3_L_YROTATION,//335 
MOCAPNET_OUTPUT_METACARPAL2_L_ZROTATION,//336 
MOCAPNET_OUTPUT_METACARPAL2_L_XROTATION,//337 
MOCAPNET_OUTPUT_METACARPAL2_L_YROTATION,//338 
MOCAPNET_OUTPUT_FINGER3_1_L_ZROTATION,//339 
MOCAPNET_OUTPUT_FINGER3_1_L_XROTATION,//340 
MOCAPNET_OUTPUT_FINGER3_1_L_YROTATION,//341 
MOCAPNET_OUTPUT_FINGER3_2_L_ZROTATION,//342 
MOCAPNET_OUTPUT_FINGER3_2_L_XROTATION,//343 
MOCAPNET_OUTPUT_FINGER3_2_L_YROTATION,//344 
MOCAPNET_OUTPUT_FINGER3_3_L_ZROTATION,//345 
MOCAPNET_OUTPUT_FINGER3_3_L_XROTATION,//346 
MOCAPNET_OUTPUT_FINGER3_3_L_YROTATION,//347 
MOCAPNET_OUTPUT___METACARPAL3_L_ZROTATION,//348 
MOCAPNET_OUTPUT___METACARPAL3_L_XROTATION,//349 
MOCAPNET_OUTPUT___METACARPAL3_L_YROTATION,//350 
MOCAPNET_OUTPUT_METACARPAL3_L_ZROTATION,//351 
MOCAPNET_OUTPUT_METACARPAL3_L_XROTATION,//352 
MOCAPNET_OUTPUT_METACARPAL3_L_YROTATION,//353 
MOCAPNET_OUTPUT_FINGER4_1_L_ZROTATION,//354 
MOCAPNET_OUTPUT_FINGER4_1_L_XROTATION,//355 
MOCAPNET_OUTPUT_FINGER4_1_L_YROTATION,//356 
MOCAPNET_OUTPUT_FINGER4_2_L_ZROTATION,//357 
MOCAPNET_OUTPUT_FINGER4_2_L_XROTATION,//358 
MOCAPNET_OUTPUT_FINGER4_2_L_YROTATION,//359 
MOCAPNET_OUTPUT_FINGER4_3_L_ZROTATION,//360 
MOCAPNET_OUTPUT_FINGER4_3_L_XROTATION,//361 
MOCAPNET_OUTPUT_FINGER4_3_L_YROTATION,//362 
MOCAPNET_OUTPUT___METACARPAL4_L_ZROTATION,//363 
MOCAPNET_OUTPUT___METACARPAL4_L_XROTATION,//364 
MOCAPNET_OUTPUT___METACARPAL4_L_YROTATION,//365 
MOCAPNET_OUTPUT_METACARPAL4_L_ZROTATION,//366 
MOCAPNET_OUTPUT_METACARPAL4_L_XROTATION,//367 
MOCAPNET_OUTPUT_METACARPAL4_L_YROTATION,//368 
MOCAPNET_OUTPUT_FINGER5_1_L_ZROTATION,//369 
MOCAPNET_OUTPUT_FINGER5_1_L_XROTATION,//370 
MOCAPNET_OUTPUT_FINGER5_1_L_YROTATION,//371 
MOCAPNET_OUTPUT_FINGER5_2_L_ZROTATION,//372 
MOCAPNET_OUTPUT_FINGER5_2_L_XROTATION,//373 
MOCAPNET_OUTPUT_FINGER5_2_L_YROTATION,//374 
MOCAPNET_OUTPUT_FINGER5_3_L_ZROTATION,//375 
MOCAPNET_OUTPUT_FINGER5_3_L_XROTATION,//376 
MOCAPNET_OUTPUT_FINGER5_3_L_YROTATION,//377 
MOCAPNET_OUTPUT___LTHUMB_ZROTATION,//378 
MOCAPNET_OUTPUT___LTHUMB_XROTATION,//379 
MOCAPNET_OUTPUT___LTHUMB_YROTATION,//380 
MOCAPNET_OUTPUT_LTHUMB_ZROTATION,//381 
MOCAPNET_OUTPUT_LTHUMB_XROTATION,//382 
MOCAPNET_OUTPUT_LTHUMB_YROTATION,//383 
MOCAPNET_OUTPUT_FINGER1_2_L_ZROTATION,//384 
MOCAPNET_OUTPUT_FINGER1_2_L_XROTATION,//385 
MOCAPNET_OUTPUT_FINGER1_2_L_YROTATION,//386 
MOCAPNET_OUTPUT_FINGER1_3_L_ZROTATION,//387 
MOCAPNET_OUTPUT_FINGER1_3_L_XROTATION,//388 
MOCAPNET_OUTPUT_FINGER1_3_L_YROTATION,//389 
MOCAPNET_OUTPUT_RBUTTOCK_ZROTATION,//390 
MOCAPNET_OUTPUT_RBUTTOCK_XROTATION,//391 
MOCAPNET_OUTPUT_RBUTTOCK_YROTATION,//392 
MOCAPNET_OUTPUT_RHIP_ZROTATION,//393 
MOCAPNET_OUTPUT_RHIP_XROTATION,//394 
MOCAPNET_OUTPUT_RHIP_YROTATION,//395 
MOCAPNET_OUTPUT_RKNEE_ZROTATION,//396 
MOCAPNET_OUTPUT_RKNEE_XROTATION,//397 
MOCAPNET_OUTPUT_RKNEE_YROTATION,//398 
MOCAPNET_OUTPUT_RFOOT_ZROTATION,//399 
MOCAPNET_OUTPUT_RFOOT_XROTATION,//400 
MOCAPNET_OUTPUT_RFOOT_YROTATION,//401 
MOCAPNET_OUTPUT_TOE1_1_R_ZROTATION,//402 
MOCAPNET_OUTPUT_TOE1_1_R_XROTATION,//403 
MOCAPNET_OUTPUT_TOE1_1_R_YROTATION,//404 
MOCAPNET_OUTPUT_TOE1_2_R_ZROTATION,//405 
MOCAPNET_OUTPUT_TOE1_2_R_XROTATION,//406 
MOCAPNET_OUTPUT_TOE1_2_R_YROTATION,//407 
MOCAPNET_OUTPUT_TOE2_1_R_ZROTATION,//408 
MOCAPNET_OUTPUT_TOE2_1_R_XROTATION,//409 
MOCAPNET_OUTPUT_TOE2_1_R_YROTATION,//410 
MOCAPNET_OUTPUT_TOE2_2_R_ZROTATION,//411 
MOCAPNET_OUTPUT_TOE2_2_R_XROTATION,//412 
MOCAPNET_OUTPUT_TOE2_2_R_YROTATION,//413 
MOCAPNET_OUTPUT_TOE2_3_R_ZROTATION,//414 
MOCAPNET_OUTPUT_TOE2_3_R_XROTATION,//415 
MOCAPNET_OUTPUT_TOE2_3_R_YROTATION,//416 
MOCAPNET_OUTPUT_TOE3_1_R_ZROTATION,//417 
MOCAPNET_OUTPUT_TOE3_1_R_XROTATION,//418 
MOCAPNET_OUTPUT_TOE3_1_R_YROTATION,//419 
MOCAPNET_OUTPUT_TOE3_2_R_ZROTATION,//420 
MOCAPNET_OUTPUT_TOE3_2_R_XROTATION,//421 
MOCAPNET_OUTPUT_TOE3_2_R_YROTATION,//422 
MOCAPNET_OUTPUT_TOE3_3_R_ZROTATION,//423 
MOCAPNET_OUTPUT_TOE3_3_R_XROTATION,//424 
MOCAPNET_OUTPUT_TOE3_3_R_YROTATION,//425 
MOCAPNET_OUTPUT_TOE4_1_R_ZROTATION,//426 
MOCAPNET_OUTPUT_TOE4_1_R_XROTATION,//427 
MOCAPNET_OUTPUT_TOE4_1_R_YROTATION,//428 
MOCAPNET_OUTPUT_TOE4_2_R_ZROTATION,//429 
MOCAPNET_OUTPUT_TOE4_2_R_XROTATION,//430 
MOCAPNET_OUTPUT_TOE4_2_R_YROTATION,//431 
MOCAPNET_OUTPUT_TOE4_3_R_ZROTATION,//432 
MOCAPNET_OUTPUT_TOE4_3_R_XROTATION,//433 
MOCAPNET_OUTPUT_TOE4_3_R_YROTATION,//434 
MOCAPNET_OUTPUT_TOE5_1_R_ZROTATION,//435 
MOCAPNET_OUTPUT_TOE5_1_R_XROTATION,//436 
MOCAPNET_OUTPUT_TOE5_1_R_YROTATION,//437 
MOCAPNET_OUTPUT_TOE5_2_R_ZROTATION,//438 
MOCAPNET_OUTPUT_TOE5_2_R_XROTATION,//439 
MOCAPNET_OUTPUT_TOE5_2_R_YROTATION,//440 
MOCAPNET_OUTPUT_TOE5_3_R_ZROTATION,//441 
MOCAPNET_OUTPUT_TOE5_3_R_XROTATION,//442 
MOCAPNET_OUTPUT_TOE5_3_R_YROTATION,//443 
MOCAPNET_OUTPUT_LBUTTOCK_ZROTATION,//444 
MOCAPNET_OUTPUT_LBUTTOCK_XROTATION,//445 
MOCAPNET_OUTPUT_LBUTTOCK_YROTATION,//446 
MOCAPNET_OUTPUT_LHIP_ZROTATION,//447 
MOCAPNET_OUTPUT_LHIP_XROTATION,//448 
MOCAPNET_OUTPUT_LHIP_YROTATION,//449 
MOCAPNET_OUTPUT_LKNEE_ZROTATION,//450 
MOCAPNET_OUTPUT_LKNEE_XROTATION,//451 
MOCAPNET_OUTPUT_LKNEE_YROTATION,//452 
MOCAPNET_OUTPUT_LFOOT_ZROTATION,//453 
MOCAPNET_OUTPUT_LFOOT_XROTATION,//454 
MOCAPNET_OUTPUT_LFOOT_YROTATION,//455 
MOCAPNET_OUTPUT_TOE1_1_L_ZROTATION,//456 
MOCAPNET_OUTPUT_TOE1_1_L_XROTATION,//457 
MOCAPNET_OUTPUT_TOE1_1_L_YROTATION,//458 
MOCAPNET_OUTPUT_TOE1_2_L_ZROTATION,//459 
MOCAPNET_OUTPUT_TOE1_2_L_XROTATION,//460 
MOCAPNET_OUTPUT_TOE1_2_L_YROTATION,//461 
MOCAPNET_OUTPUT_TOE2_1_L_ZROTATION,//462 
MOCAPNET_OUTPUT_TOE2_1_L_XROTATION,//463 
MOCAPNET_OUTPUT_TOE2_1_L_YROTATION,//464 
MOCAPNET_OUTPUT_TOE2_2_L_ZROTATION,//465 
MOCAPNET_OUTPUT_TOE2_2_L_XROTATION,//466 
MOCAPNET_OUTPUT_TOE2_2_L_YROTATION,//467 
MOCAPNET_OUTPUT_TOE2_3_L_ZROTATION,//468 
MOCAPNET_OUTPUT_TOE2_3_L_XROTATION,//469 
MOCAPNET_OUTPUT_TOE2_3_L_YROTATION,//470 
MOCAPNET_OUTPUT_TOE3_1_L_ZROTATION,//471 
MOCAPNET_OUTPUT_TOE3_1_L_XROTATION,//472 
MOCAPNET_OUTPUT_TOE3_1_L_YROTATION,//473 
MOCAPNET_OUTPUT_TOE3_2_L_ZROTATION,//474 
MOCAPNET_OUTPUT_TOE3_2_L_XROTATION,//475 
MOCAPNET_OUTPUT_TOE3_2_L_YROTATION,//476 
MOCAPNET_OUTPUT_TOE3_3_L_ZROTATION,//477 
MOCAPNET_OUTPUT_TOE3_3_L_XROTATION,//478 
MOCAPNET_OUTPUT_TOE3_3_L_YROTATION,//479 
MOCAPNET_OUTPUT_TOE4_1_L_ZROTATION,//480 
MOCAPNET_OUTPUT_TOE4_1_L_XROTATION,//481 
MOCAPNET_OUTPUT_TOE4_1_L_YROTATION,//482 
MOCAPNET_OUTPUT_TOE4_2_L_ZROTATION,//483 
MOCAPNET_OUTPUT_TOE4_2_L_XROTATION,//484 
MOCAPNET_OUTPUT_TOE4_2_L_YROTATION,//485 
MOCAPNET_OUTPUT_TOE4_3_L_ZROTATION,//486 
MOCAPNET_OUTPUT_TOE4_3_L_XROTATION,//487 
MOCAPNET_OUTPUT_TOE4_3_L_YROTATION,//488 
MOCAPNET_OUTPUT_TOE5_1_L_ZROTATION,//489 
MOCAPNET_OUTPUT_TOE5_1_L_XROTATION,//490 
MOCAPNET_OUTPUT_TOE5_1_L_YROTATION,//491 
MOCAPNET_OUTPUT_TOE5_2_L_ZROTATION,//492 
MOCAPNET_OUTPUT_TOE5_2_L_XROTATION,//493 
MOCAPNET_OUTPUT_TOE5_2_L_YROTATION,//494 
MOCAPNET_OUTPUT_TOE5_3_L_ZROTATION,//495 
MOCAPNET_OUTPUT_TOE5_3_L_XROTATION,//496 
MOCAPNET_OUTPUT_TOE5_3_L_YROTATION,//497 
//----------------------------- 
MOCAPNET_OUTPUT_NUMBER
};

enum LEGACY_Output_Joints
{
 LEGACY_OUTPUT_HIP_XPOSITION=0,
 LEGACY_OUTPUT_HIP_YPOSITION,
 LEGACY_OUTPUT_HIP_ZPOSITION,
 LEGACY_OUTPUT_HIP_ZROTATION,
 LEGACY_OUTPUT_HIP_YROTATION,
 LEGACY_OUTPUT_HIP_XROTATION,
 LEGACY_OUTPUT_ABDOMEN_ZROTATION,
 LEGACY_OUTPUT_ABDOMEN_XROTATION,
 LEGACY_OUTPUT_ABDOMEN_YROTATION,
 LEGACY_OUTPUT_CHEST_ZROTATION,
 LEGACY_OUTPUT_CHEST_XROTATION,
 LEGACY_OUTPUT_CHEST_YROTATION,
 LEGACY_OUTPUT_NECK_ZROTATION,
 LEGACY_OUTPUT_NECK_XROTATION,
 LEGACY_OUTPUT_NECK_YROTATION,
 LEGACY_OUTPUT_HEAD_ZROTATION,
 LEGACY_OUTPUT_HEAD_XROTATION,
 LEGACY_OUTPUT_HEAD_YROTATION,
 LEGACY_OUTPUT_LEFTEYE_ZROTATION,
 LEGACY_OUTPUT_LEFTEYE_XROTATION,
 LEGACY_OUTPUT_LEFTEYE_YROTATION,
 LEGACY_OUTPUT_RIGHTEYE_ZROTATION,
 LEGACY_OUTPUT_RIGHTEYE_XROTATION,
 LEGACY_OUTPUT_RIGHTEYE_YROTATION,
 LEGACY_OUTPUT_RCOLLAR_ZROTATION,
 LEGACY_OUTPUT_RCOLLAR_XROTATION,
 LEGACY_OUTPUT_RCOLLAR_YROTATION,
 LEGACY_OUTPUT_RSHOULDER_ZROTATION,
 LEGACY_OUTPUT_RSHOULDER_XROTATION,
 LEGACY_OUTPUT_RSHOULDER_YROTATION,
 LEGACY_OUTPUT_RELBOW_ZROTATION,
 LEGACY_OUTPUT_RELBOW_XROTATION,
 LEGACY_OUTPUT_RELBOW_YROTATION,
 LEGACY_OUTPUT_RHAND_ZROTATION,
 LEGACY_OUTPUT_RHAND_XROTATION,
 LEGACY_OUTPUT_RHAND_YROTATION,
 LEGACY_OUTPUT_RTHUMB1_ZROTATION,
 LEGACY_OUTPUT_RTHUMB1_XROTATION,
 LEGACY_OUTPUT_RTHUMB1_YROTATION,
 LEGACY_OUTPUT_RTHUMB2_ZROTATION,
 LEGACY_OUTPUT_RTHUMB2_XROTATION,
 LEGACY_OUTPUT_RTHUMB2_YROTATION,
 LEGACY_OUTPUT_RINDEX1_ZROTATION,
 LEGACY_OUTPUT_RINDEX1_XROTATION,
 LEGACY_OUTPUT_RINDEX1_YROTATION,
 LEGACY_OUTPUT_RINDEX2_ZROTATION,
 LEGACY_OUTPUT_RINDEX2_XROTATION,
 LEGACY_OUTPUT_RINDEX2_YROTATION,
 LEGACY_OUTPUT_RMID1_ZROTATION,
 LEGACY_OUTPUT_RMID1_XROTATION,
 LEGACY_OUTPUT_RMID1_YROTATION,
 LEGACY_OUTPUT_RMID2_ZROTATION,
 LEGACY_OUTPUT_RMID2_XROTATION,
 LEGACY_OUTPUT_RMID2_YROTATION,
 LEGACY_OUTPUT_RRING1_ZROTATION,
 LEGACY_OUTPUT_RRING1_XROTATION,
 LEGACY_OUTPUT_RRING1_YROTATION,
 LEGACY_OUTPUT_RRING2_ZROTATION,
 LEGACY_OUTPUT_RRING2_XROTATION,
 LEGACY_OUTPUT_RRING2_YROTATION,
 LEGACY_OUTPUT_RPINKY1_ZROTATION,
 LEGACY_OUTPUT_RPINKY1_XROTATION,
 LEGACY_OUTPUT_RPINKY1_YROTATION,
 LEGACY_OUTPUT_RPINKY2_ZROTATION,
 LEGACY_OUTPUT_RPINKY2_XROTATION,
 LEGACY_OUTPUT_RPINKY2_YROTATION,
 LEGACY_OUTPUT_LCOLLAR_ZROTATION,
 LEGACY_OUTPUT_LCOLLAR_XROTATION,
 LEGACY_OUTPUT_LCOLLAR_YROTATION,
 LEGACY_OUTPUT_LSHOULDER_ZROTATION,
 LEGACY_OUTPUT_LSHOULDER_XROTATION,
 LEGACY_OUTPUT_LSHOULDER_YROTATION,
 LEGACY_OUTPUT_LELBOW_ZROTATION,
 LEGACY_OUTPUT_LELBOW_XROTATION,
 LEGACY_OUTPUT_LELBOW_YROTATION,
 LEGACY_OUTPUT_LHAND_ZROTATION,
 LEGACY_OUTPUT_LHAND_XROTATION,
 LEGACY_OUTPUT_LHAND_YROTATION,
 LEGACY_OUTPUT_LTHUMB1_ZROTATION,
 LEGACY_OUTPUT_LTHUMB1_XROTATION,
 LEGACY_OUTPUT_LTHUMB1_YROTATION,
 LEGACY_OUTPUT_LTHUMB2_ZROTATION,
 LEGACY_OUTPUT_LTHUMB2_XROTATION,
 LEGACY_OUTPUT_LTHUMB2_YROTATION,
 LEGACY_OUTPUT_LINDEX1_ZROTATION,
 LEGACY_OUTPUT_LINDEX1_XROTATION,
 LEGACY_OUTPUT_LINDEX1_YROTATION,
 LEGACY_OUTPUT_LINDEX2_ZROTATION,
 LEGACY_OUTPUT_LINDEX2_XROTATION,
 LEGACY_OUTPUT_LINDEX2_YROTATION,
 LEGACY_OUTPUT_LMID1_ZROTATION,
 LEGACY_OUTPUT_LMID1_XROTATION,
 LEGACY_OUTPUT_LMID1_YROTATION,
 LEGACY_OUTPUT_LMID2_ZROTATION,
 LEGACY_OUTPUT_LMID2_XROTATION,
 LEGACY_OUTPUT_LMID2_YROTATION,
 LEGACY_OUTPUT_LRING1_ZROTATION,
 LEGACY_OUTPUT_LRING1_XROTATION,
 LEGACY_OUTPUT_LRING1_YROTATION,
 LEGACY_OUTPUT_LRING2_ZROTATION,
 LEGACY_OUTPUT_LRING2_XROTATION,
 LEGACY_OUTPUT_LRING2_YROTATION,
 LEGACY_OUTPUT_LPINKY1_ZROTATION,
 LEGACY_OUTPUT_LPINKY1_XROTATION,
 LEGACY_OUTPUT_LPINKY1_YROTATION,
 LEGACY_OUTPUT_LPINKY2_ZROTATION,
 LEGACY_OUTPUT_LPINKY2_XROTATION,
 LEGACY_OUTPUT_LPINKY2_YROTATION,
 LEGACY_OUTPUT_RBUTTOCK_ZROTATION,
 LEGACY_OUTPUT_RBUTTOCK_XROTATION,
 LEGACY_OUTPUT_RBUTTOCK_YROTATION,
 LEGACY_OUTPUT_RHIP_ZROTATION,
 LEGACY_OUTPUT_RHIP_XROTATION,
 LEGACY_OUTPUT_RHIP_YROTATION,
 LEGACY_OUTPUT_RKNEE_ZROTATION,
 LEGACY_OUTPUT_RKNEE_XROTATION,
 LEGACY_OUTPUT_RKNEE_YROTATION,
 LEGACY_OUTPUT_RFOOT_ZROTATION,
 LEGACY_OUTPUT_RFOOT_XROTATION,
 LEGACY_OUTPUT_RFOOT_YROTATION,
 LEGACY_OUTPUT_LBUTTOCK_ZROTATION,
 LEGACY_OUTPUT_LBUTTOCK_XROTATION,
 LEGACY_OUTPUT_LBUTTOCK_YROTATION,
 LEGACY_OUTPUT_LHIP_ZROTATION,
 LEGACY_OUTPUT_LHIP_XROTATION,
 LEGACY_OUTPUT_LHIP_YROTATION,
 LEGACY_OUTPUT_LKNEE_ZROTATION,
 LEGACY_OUTPUT_LKNEE_XROTATION,
 LEGACY_OUTPUT_LKNEE_YROTATION,
 LEGACY_OUTPUT_LFOOT_ZROTATION,
 LEGACY_OUTPUT_LFOOT_XROTATION,
 LEGACY_OUTPUT_LFOOT_YROTATION,
 //----------------------------- 
 LEGACY_OUTPUT_NUMBER
};

struct MocapNETModelLimits
{
    int numberOfLimits;
    float minimumYaw1;
    float maximumYaw1;
    float minimumYaw2;
    float maximumYaw2;
    int isFlipped;
};

 

/**
 * @brief This is a MocapNET orientation.
 */
enum MOCAPNET_Orientation
{
 MOCAPNET_ORIENTATION_NONE=0,
 MOCAPNET_ORIENTATION_FRONT,
 MOCAPNET_ORIENTATION_BACK,
 MOCAPNET_ORIENTATION_LEFT,
 MOCAPNET_ORIENTATION_RIGHT,
 //----------------------------- 
 MOCAPNET_ORIENTATION_NUMBER
};


/**
 * @brief MocapNET consists of separate classes/ensembles that are invoked for particular orientations.
 * This structure holds the required tensorflow instances to make MocapNET work.
 */
struct MocapNET
{
   char useRemoteMocapNETServer;
   char remoteMocapNETServerURL[128];  
   int  remoteMocapNETServerPort;
   void * remoteContext;
   //--------------------------------
   
    
   struct TensorflowInstance models[16]; 
   unsigned int mode;
   unsigned int loadedModels;
   unsigned int framesReceived;
   struct MocapNETModelLimits modelLimits[16];  
   
   //--------------------------------
   struct ButterWorth directionSignal;
   struct ButterWorth outputSignals[MOCAPNET_OUTPUT_NUMBER];
   //--------------------------------
   
   unsigned int lastActivatedGesture;
   unsigned int gestureTimestamp;
   
   float lastSkeletonOrientation;
   
   struct PoseHistory poseHistoryStorage;
   struct GestureDatabase recognizedGestures;
   void * newGestureEventCallback;
};
 
 
int registerGestureEventCallbackWithMocapNET(struct MocapNET * mnet,void * callback);

std::vector <float> fillInTheBlanks(std::vector <float> previousVector,std::vector <float> currentVector);

std::vector <float> smoothVector(std::vector <float> previousVector,std::vector <float> currentVector,float magnitude);



int connectToMocapNETServer(struct MocapNET * mnet,const char * url,int port);

/**
 * @brief Load a MocapNET from .pb files on disk
 * @ingroup mocapnet
 * @param Pointer to a struct MocapNET that will hold the tensorflow instances on load.
 * @param Path to .pb files that are needed
 * @param Quality setting, can currently be 1.0 ( highest quality ), 1.5  or 2.0 ( highest performance )
 * @param Mode setting, can currently be 2 (default ) or 4 (experimental) 
 * @param Force the usage of CPU for MocapNET ( should be 1 as MocapNET is designed for CPU while GPU handles 2D ) 
 * @retval 1 = Success loading the files  , 0 = Failure
 */
int loadMocapNET(struct MocapNET * mnet,const char * filename,float qualitySetting,int mode,unsigned int forceCPU);


 


/**
 * @brief Convert a Vector Of floats encoded in the COCO format to the MocapNET format
 * @ingroup mocapnet
 * @param Input vector of floats
 * @retval Output vector of floats, Empty vector in case of failure
 */
std::vector<float> prepareMocapNETInputFromUncompressedInput(std::vector<float> input);

//we expect input to have the COCO skeleton order as seen in jsonCocoSkeleton.hpp enum COCOSkeletonJoints

/**
 * @brief run MocapNET on an input vector that has the correct formatting. If getting data from an external source
 * the prepareMocapNETInputFromUncompressedInput function could be used to prepare the input for this function.
 * @param Pointer to a valid and populated MocapNET instance
 * @param Vector of input values according to MocapNETUncompressedAndCompressedArrayNames
 * @retval 1=Success,0=Failure
 */
std::vector<float> runMocapNET(struct MocapNET * mnet,std::vector<float> input,int doGestureDetection,int doOutputFiltering) ;





/**
 * @brief Deallocate tensorflow instances and free memory
 * @param Pointer to a valid and populated MocapNET instance
 * @retval 1=Success,0=Failure
 */
int unloadMocapNET(struct MocapNET * mnet);
