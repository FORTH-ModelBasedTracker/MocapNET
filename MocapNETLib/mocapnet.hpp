#pragma once
/** @file mocapnet.hpp
 *  @brief The MocapNET C library
 *  As seen in https://www.youtube.com/watch?v=fH5e-KMBvM0 , the MocapNET network requires two types of input.
 *  The first is an uncompressed list of (x,y,v) joints and the second an NSDM array. To add to those the output consists of BVH
 *  frames that must be accompanied by a header. This library internally handles all of these details.
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include "../Tensorflow/tensorflow.hpp"
#include <iostream>
#include <vector>



/**
 * @brief This is a BVH header that can be easily injected in a file ( using just an fprintf call )
 */
// replace \n with \\n"\n" in header.bvh
static const char * bvhHeader="HIERARCHY\n"
"ROOT hip\n"
"{\n"
"  OFFSET 0 0 0\n"
"  CHANNELS 6 Xposition Yposition Zposition Zrotation Yrotation Xrotation\n"
"  JOINT abdomen\n"
"  {\n"
"    OFFSET 0 20.6881 -0.73152\n"
"    CHANNELS 3 Zrotation Xrotation Yrotation\n"
"    JOINT chest\n"
"    {\n"
"      OFFSET 0 11.7043 -0.48768\n"
"      CHANNELS 3 Zrotation Xrotation Yrotation\n"
"      JOINT neck\n"
"      {\n"
"        OFFSET 0 22.1894 -2.19456\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT head\n"
"        {\n"
"          OFFSET -0.24384 7.07133 1.2192\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          JOINT leftEye\n"
"          {\n"
"            OFFSET 4.14528 8.04674 8.04672\n"
"            CHANNELS 3 Zrotation Xrotation Yrotation\n"
"            End Site\n"
"            {\n"
"              OFFSET 1 0 0\n"
"            }\n"
"          }\n"
"          JOINT rightEye\n"
"          {\n"
"            OFFSET -3.6576 8.04674 8.04672\n"
"            CHANNELS 3 Zrotation Xrotation Yrotation\n"
"            End Site\n"
"            {\n"
"              OFFSET 1 0 0\n"
"            }\n"
"          }\n"
"        }\n"
"      }\n"
"      JOINT rCollar\n"
"      {\n"
"        OFFSET -2.68224 19.2634 -4.8768\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT rShldr\n"
"        {\n"
"          OFFSET -8.77824 -1.95073 1.46304\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          JOINT rForeArm\n"
"          {\n"
"            OFFSET -28.1742 -1.7115 0.48768\n"
"            CHANNELS 3 Zrotation Xrotation Yrotation\n"
"            JOINT rHand\n"
"            {\n"
"              OFFSET -22.5879 0.773209 7.07136\n"
"              CHANNELS 3 Zrotation Xrotation Yrotation\n"
"              JOINT rThumb1\n"
"              {\n"
"                OFFSET -1.2192 -0.487915 3.41376\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT rThumb2\n"
"                {\n"
"                  OFFSET -3.37035 -0.52449 3.41376\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET -1.78271 -1.18214 1.43049\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT rIndex1\n"
"              {\n"
"                OFFSET -7.75947 0.938293 5.60832\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT rIndex2\n"
"                {\n"
"                  OFFSET -2.54057 -0.884171 1.56538\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET -1.62519 -0.234802 1.16502\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT rMid1\n"
"              {\n"
"                OFFSET -8.24714 1.18213 3.41376\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT rMid2\n"
"                {\n"
"                  OFFSET -3.10165 -0.590103 1.0647\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET -2.48547 -0.328903 0.83742\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT rRing1\n"
"              {\n"
"                OFFSET -8.82822 0.546677 1.51678\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT rRing2\n"
"                {\n"
"                  OFFSET -2.60934 -0.819778 -0.0198488\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET -2.33842 -0.294052 0.168128\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT rPinky1\n"
"              {\n"
"                OFFSET -8.27202 -0.0477905 -0.4584\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT rPinky2\n"
"                {\n"
"                  OFFSET -1.82734 -0.647385 -0.700984\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET -1.69225 -0.51767 -0.607171\n"
"                  }\n"
"                }\n"
"              }\n"
"            }\n"
"          }\n"
"        }\n"
"      }\n"
"      JOINT lCollar\n"
"      {\n"
"        OFFSET 2.68224 19.2634 -4.8768\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT lShldr\n"
"        {\n"
"          OFFSET 8.77824 -1.95073 1.46304\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          JOINT lForeArm\n"
"          {\n"
"            OFFSET 28.1742 -1.7115 0.48768\n"
"            CHANNELS 3 Zrotation Xrotation Yrotation\n"
"            JOINT lHand\n"
"            {\n"
"              OFFSET 22.5879 0.773209 7.07136\n"
"              CHANNELS 3 Zrotation Xrotation Yrotation\n"
"              JOINT lThumb1\n"
"              {\n"
"                OFFSET 1.2192 -0.487915 3.41376\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT lThumb2\n"
"                {\n"
"                  OFFSET 3.37035 -0.52449 3.41376\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET 1.78271 -1.18214 1.43049\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT lIndex1\n"
"              {\n"
"                OFFSET 7.75947 0.938293 5.60832\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT lIndex2\n"
"                {\n"
"                  OFFSET 2.54057 -0.884171 1.56538\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET 1.62519 -0.234802 1.16502\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT lMid1\n"
"              {\n"
"                OFFSET 8.24714 1.18213 3.41376\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT lMid2\n"
"                {\n"
"                  OFFSET 3.10165 -0.590103 1.0647\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET 2.48547 -0.328903 0.83742\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT lRing1\n"
"              {\n"
"                OFFSET 8.82822 0.546677 1.51678\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT lRing2\n"
"                {\n"
"                  OFFSET 2.60934 -0.819778 -0.0198488\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET 2.33842 -0.294052 0.168128\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT lPinky1\n"
"              {\n"
"                OFFSET 8.27202 -0.0477905 -0.4584\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                JOINT lPinky2\n"
"                {\n"
"                  OFFSET 1.82734 -0.647385 -0.700984\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation\n"
"                  End Site\n"
"                  {\n"
"                    OFFSET 1.69225 -0.51767 -0.607171\n"
"                  }\n"
"                }\n"
"              }\n"
"            }\n"
"          }\n"
"        }\n"
"      }\n"
"    }\n"
"  }\n"
"  JOINT rButtock\n"
"  {\n"
"    OFFSET -8.77824 4.35084 1.2192\n"
"    CHANNELS 3 Zrotation Xrotation Yrotation\n"
"    JOINT rThigh\n"
"    {\n"
"      OFFSET 0 -1.70687 -2.19456\n"
"      CHANNELS 3 Zrotation Xrotation Yrotation\n"
"      JOINT rShin\n"
"      {\n"
"        OFFSET 0 -36.8199 0.73152\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT rFoot\n"
"        {\n"
"          OFFSET 0.73152 -45.1104 -5.12064\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"            OFFSET -1.1221 -3.69964 12.103\n"
"          }\n"
"        }\n"
"      }\n"
"    }\n"
"  }\n"
"  JOINT lButtock\n"
"  {\n"
"    OFFSET 8.77824 4.35084 1.2192\n"
"    CHANNELS 3 Zrotation Xrotation Yrotation\n"
"    JOINT lThigh\n"
"    {\n"
"      OFFSET 0 -1.70687 -2.19456\n"
"      CHANNELS 3 Zrotation Xrotation Yrotation\n"
"      JOINT lShin\n"
"      {\n"
"        OFFSET 0 -36.8199 0.73152\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT lFoot\n"
"        {\n"
"          OFFSET -0.73152 -45.1104 -5.12064\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"            OFFSET 1.1221 -3.69964 12.103\n"
"          }\n"
"        }\n"
"      }\n"
"    }\n"
"  }\n"
"}";



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
 */
static const char * MocapNETOutputArrayNames[] =
{
 "hip_Xposition",
 "hip_Yposition",
 "hip_Zposition",
 "hip_Zrotation",
 "hip_Yrotation",
 "hip_Xrotation",
 "abdomen_Zrotation",
 "abdomen_Xrotation",
 "abdomen_Yrotation",
 "chest_Zrotation",
 "chest_Xrotation",
 "chest_Yrotation",
 "neck_Zrotation",
 "neck_Xrotation",
 "neck_Yrotation",
 "head_Zrotation",
 "head_Xrotation",
 "head_Yrotation",
 "lefteye_Zrotation",
 "lefteye_Xrotation",
 "lefteye_Yrotation",
 "righteye_Zrotation",
 "righteye_Xrotation",
 "righteye_Yrotation",
 "rcollar_Zrotation",
 "rcollar_Xrotation",
 "rcollar_Yrotation",
 "rshoulder_Zrotation",
 "rshoulder_Xrotation",
 "rshoulder_Yrotation",
 "relbow_Zrotation",
 "relbow_Xrotation",
 "relbow_Yrotation",
 "rhand_Zrotation",
 "rhand_Xrotation",
 "rhand_Yrotation",
 "rthumb1_Zrotation",
 "rthumb1_Xrotation",
 "rthumb1_Yrotation",
 "rthumb2_Zrotation",
 "rthumb2_Xrotation",
 "rthumb2_Yrotation",
 "rindex1_Zrotation",
 "rindex1_Xrotation",
 "rindex1_Yrotation",
 "rindex2_Zrotation",
 "rindex2_Xrotation",
 "rindex2_Yrotation",
 "rmid1_Zrotation",
 "rmid1_Xrotation",
 "rmid1_Yrotation",
 "rmid2_Zrotation",
 "rmid2_Xrotation",
 "rmid2_Yrotation",
 "rring1_Zrotation",
 "rring1_Xrotation",
 "rring1_Yrotation",
 "rring2_Zrotation",
 "rring2_Xrotation",
 "rring2_Yrotation",
 "rpinky1_Zrotation",
 "rpinky1_Xrotation",
 "rpinky1_Yrotation",
 "rpinky2_Zrotation",
 "rpinky2_Xrotation",
 "rpinky2_Yrotation",
 "lcollar_Zrotation",
 "lcollar_Xrotation",
 "lcollar_Yrotation",
 "lshoulder_Zrotation",
 "lshoulder_Xrotation",
 "lshoulder_Yrotation",
 "lelbow_Zrotation",
 "lelbow_Xrotation",
 "lelbow_Yrotation",
 "lhand_Zrotation",
 "lhand_Xrotation",
 "lhand_Yrotation",
 "lthumb1_Zrotation",
 "lthumb1_Xrotation",
 "lthumb1_Yrotation",
 "lthumb2_Zrotation",
 "lthumb2_Xrotation",
 "lthumb2_Yrotation",
 "lindex1_Zrotation",
 "lindex1_Xrotation",
 "lindex1_Yrotation",
 "lindex2_Zrotation",
 "lindex2_Xrotation",
 "lindex2_Yrotation",
 "lmid1_Zrotation",
 "lmid1_Xrotation",
 "lmid1_Yrotation",
 "lmid2_Zrotation",
 "lmid2_Xrotation",
 "lmid2_Yrotation",
 "lring1_Zrotation",
 "lring1_Xrotation",
 "lring1_Yrotation",
 "lring2_Zrotation",
 "lring2_Xrotation",
 "lring2_Yrotation",
 "lpinky1_Zrotation",
 "lpinky1_Xrotation",
 "lpinky1_Yrotation",
 "lpinky2_Zrotation",
 "lpinky2_Xrotation",
 "lpinky2_Yrotation",
 "rbuttock_Zrotation",
 "rbuttock_Xrotation",
 "rbuttock_Yrotation",
 "rhip_Zrotation",
 "rhip_Xrotation",
 "rhip_Yrotation",
 "rknee_Zrotation",
 "rknee_Xrotation",
 "rknee_Yrotation",
 "rfoot_Zrotation",
 "rfoot_Xrotation",
 "rfoot_Yrotation",
 "lbuttock_Zrotation",
 "lbuttock_Xrotation",
 "lbuttock_Yrotation",
 "lhip_Zrotation",
 "lhip_Xrotation",
 "lhip_Yrotation",
 "lknee_Zrotation",
 "lknee_Xrotation",
 "lknee_Yrotation",
 "lfoot_Zrotation",
 "lfoot_Xrotation",
 "lfoot_Yrotation"
};



/**
 * @brief This is a programmer friendly enumerator of joint output extracted from MocapNET.
 */
enum MOCAPNET_Output_Joints
{
 MOCAPNET_OUTPUT_HIP_XPOSITION=0,
 MOCAPNET_OUTPUT_HIP_YPOSITION,
 MOCAPNET_OUTPUT_HIP_ZPOSITION,
 MOCAPNET_OUTPUT_HIP_ZROTATION,
 MOCAPNET_OUTPUT_HIP_YROTATION,
 MOCAPNET_OUTPUT_HIP_XROTATION,
 MOCAPNET_OUTPUT_ABDOMEN_ZROTATION,
 MOCAPNET_OUTPUT_ABDOMEN_XROTATION,
 MOCAPNET_OUTPUT_ABDOMEN_YROTATION,
 MOCAPNET_OUTPUT_CHEST_ZROTATION,
 MOCAPNET_OUTPUT_CHEST_XROTATION,
 MOCAPNET_OUTPUT_CHEST_YROTATION,
 MOCAPNET_OUTPUT_NECK_ZROTATION,
 MOCAPNET_OUTPUT_NECK_XROTATION,
 MOCAPNET_OUTPUT_NECK_YROTATION,
 MOCAPNET_OUTPUT_HEAD_ZROTATION,
 MOCAPNET_OUTPUT_HEAD_XROTATION,
 MOCAPNET_OUTPUT_HEAD_YROTATION,
 MOCAPNET_OUTPUT_LEFTEYE_ZROTATION,
 MOCAPNET_OUTPUT_LEFTEYE_XROTATION,
 MOCAPNET_OUTPUT_LEFTEYE_YROTATION,
 MOCAPNET_OUTPUT_RIGHTEYE_ZROTATION,
 MOCAPNET_OUTPUT_RIGHTEYE_XROTATION,
 MOCAPNET_OUTPUT_RIGHTEYE_YROTATION,
 MOCAPNET_OUTPUT_RCOLLAR_ZROTATION,
 MOCAPNET_OUTPUT_RCOLLAR_XROTATION,
 MOCAPNET_OUTPUT_RCOLLAR_YROTATION,
 MOCAPNET_OUTPUT_RSHOULDER_ZROTATION,
 MOCAPNET_OUTPUT_RSHOULDER_XROTATION,
 MOCAPNET_OUTPUT_RSHOULDER_YROTATION,
 MOCAPNET_OUTPUT_RELBOW_ZROTATION,
 MOCAPNET_OUTPUT_RELBOW_XROTATION,
 MOCAPNET_OUTPUT_RELBOW_YROTATION,
 MOCAPNET_OUTPUT_RHAND_ZROTATION,
 MOCAPNET_OUTPUT_RHAND_XROTATION,
 MOCAPNET_OUTPUT_RHAND_YROTATION,
 MOCAPNET_OUTPUT_RTHUMB1_ZROTATION,
 MOCAPNET_OUTPUT_RTHUMB1_XROTATION,
 MOCAPNET_OUTPUT_RTHUMB1_YROTATION,
 MOCAPNET_OUTPUT_RTHUMB2_ZROTATION,
 MOCAPNET_OUTPUT_RTHUMB2_XROTATION,
 MOCAPNET_OUTPUT_RTHUMB2_YROTATION,
 MOCAPNET_OUTPUT_RINDEX1_ZROTATION,
 MOCAPNET_OUTPUT_RINDEX1_XROTATION,
 MOCAPNET_OUTPUT_RINDEX1_YROTATION,
 MOCAPNET_OUTPUT_RINDEX2_ZROTATION,
 MOCAPNET_OUTPUT_RINDEX2_XROTATION,
 MOCAPNET_OUTPUT_RINDEX2_YROTATION,
 MOCAPNET_OUTPUT_RMID1_ZROTATION,
 MOCAPNET_OUTPUT_RMID1_XROTATION,
 MOCAPNET_OUTPUT_RMID1_YROTATION,
 MOCAPNET_OUTPUT_RMID2_ZROTATION,
 MOCAPNET_OUTPUT_RMID2_XROTATION,
 MOCAPNET_OUTPUT_RMID2_YROTATION,
 MOCAPNET_OUTPUT_RRING1_ZROTATION,
 MOCAPNET_OUTPUT_RRING1_XROTATION,
 MOCAPNET_OUTPUT_RRING1_YROTATION,
 MOCAPNET_OUTPUT_RRING2_ZROTATION,
 MOCAPNET_OUTPUT_RRING2_XROTATION,
 MOCAPNET_OUTPUT_RRING2_YROTATION,
 MOCAPNET_OUTPUT_RPINKY1_ZROTATION,
 MOCAPNET_OUTPUT_RPINKY1_XROTATION,
 MOCAPNET_OUTPUT_RPINKY1_YROTATION,
 MOCAPNET_OUTPUT_RPINKY2_ZROTATION,
 MOCAPNET_OUTPUT_RPINKY2_XROTATION,
 MOCAPNET_OUTPUT_RPINKY2_YROTATION,
 MOCAPNET_OUTPUT_LCOLLAR_ZROTATION,
 MOCAPNET_OUTPUT_LCOLLAR_XROTATION,
 MOCAPNET_OUTPUT_LCOLLAR_YROTATION,
 MOCAPNET_OUTPUT_LSHOULDER_ZROTATION,
 MOCAPNET_OUTPUT_LSHOULDER_XROTATION,
 MOCAPNET_OUTPUT_LSHOULDER_YROTATION,
 MOCAPNET_OUTPUT_LELBOW_ZROTATION,
 MOCAPNET_OUTPUT_LELBOW_XROTATION,
 MOCAPNET_OUTPUT_LELBOW_YROTATION,
 MOCAPNET_OUTPUT_LHAND_ZROTATION,
 MOCAPNET_OUTPUT_LHAND_XROTATION,
 MOCAPNET_OUTPUT_LHAND_YROTATION,
 MOCAPNET_OUTPUT_LTHUMB1_ZROTATION,
 MOCAPNET_OUTPUT_LTHUMB1_XROTATION,
 MOCAPNET_OUTPUT_LTHUMB1_YROTATION,
 MOCAPNET_OUTPUT_LTHUMB2_ZROTATION,
 MOCAPNET_OUTPUT_LTHUMB2_XROTATION,
 MOCAPNET_OUTPUT_LTHUMB2_YROTATION,
 MOCAPNET_OUTPUT_LINDEX1_ZROTATION,
 MOCAPNET_OUTPUT_LINDEX1_XROTATION,
 MOCAPNET_OUTPUT_LINDEX1_YROTATION,
 MOCAPNET_OUTPUT_LINDEX2_ZROTATION,
 MOCAPNET_OUTPUT_LINDEX2_XROTATION,
 MOCAPNET_OUTPUT_LINDEX2_YROTATION,
 MOCAPNET_OUTPUT_LMID1_ZROTATION,
 MOCAPNET_OUTPUT_LMID1_XROTATION,
 MOCAPNET_OUTPUT_LMID1_YROTATION,
 MOCAPNET_OUTPUT_LMID2_ZROTATION,
 MOCAPNET_OUTPUT_LMID2_XROTATION,
 MOCAPNET_OUTPUT_LMID2_YROTATION,
 MOCAPNET_OUTPUT_LRING1_ZROTATION,
 MOCAPNET_OUTPUT_LRING1_XROTATION,
 MOCAPNET_OUTPUT_LRING1_YROTATION,
 MOCAPNET_OUTPUT_LRING2_ZROTATION,
 MOCAPNET_OUTPUT_LRING2_XROTATION,
 MOCAPNET_OUTPUT_LRING2_YROTATION,
 MOCAPNET_OUTPUT_LPINKY1_ZROTATION,
 MOCAPNET_OUTPUT_LPINKY1_XROTATION,
 MOCAPNET_OUTPUT_LPINKY1_YROTATION,
 MOCAPNET_OUTPUT_LPINKY2_ZROTATION,
 MOCAPNET_OUTPUT_LPINKY2_XROTATION,
 MOCAPNET_OUTPUT_LPINKY2_YROTATION,
 MOCAPNET_OUTPUT_RBUTTOCK_ZROTATION,
 MOCAPNET_OUTPUT_RBUTTOCK_XROTATION,
 MOCAPNET_OUTPUT_RBUTTOCK_YROTATION,
 MOCAPNET_OUTPUT_RHIP_ZROTATION,
 MOCAPNET_OUTPUT_RHIP_XROTATION,
 MOCAPNET_OUTPUT_RHIP_YROTATION,
 MOCAPNET_OUTPUT_RKNEE_ZROTATION,
 MOCAPNET_OUTPUT_RKNEE_XROTATION,
 MOCAPNET_OUTPUT_RKNEE_YROTATION,
 MOCAPNET_OUTPUT_RFOOT_ZROTATION,
 MOCAPNET_OUTPUT_RFOOT_XROTATION,
 MOCAPNET_OUTPUT_RFOOT_YROTATION,
 MOCAPNET_OUTPUT_LBUTTOCK_ZROTATION,
 MOCAPNET_OUTPUT_LBUTTOCK_XROTATION,
 MOCAPNET_OUTPUT_LBUTTOCK_YROTATION,
 MOCAPNET_OUTPUT_LHIP_ZROTATION,
 MOCAPNET_OUTPUT_LHIP_XROTATION,
 MOCAPNET_OUTPUT_LHIP_YROTATION,
 MOCAPNET_OUTPUT_LKNEE_ZROTATION,
 MOCAPNET_OUTPUT_LKNEE_XROTATION,
 MOCAPNET_OUTPUT_LKNEE_YROTATION,
 MOCAPNET_OUTPUT_LFOOT_ZROTATION,
 MOCAPNET_OUTPUT_LFOOT_XROTATION,
 MOCAPNET_OUTPUT_LFOOT_YROTATION
};

/**
 * @brief MocapNET consists of separate classes/ensembles that are invoked for particular orientations.
 * This structure holds the required tensorflow instances to make MocapNET work.
 */
struct MocapNET
{
   struct TensorflowInstance allModel;
   struct TensorflowInstance frontModel;
   struct TensorflowInstance backModel;
};



/**
 * @brief After collecting a vector of BVH output vectors this call can write them to disk in BVH format
 * to make them accessible by third party 3D animation software like blender etc.
 * @param Path to output file i.e. "output.bvh"
 * @param Pointer to BVH header string, if set to null it will default to the bvhHeader found in this header file.
 * @param Vector of BVH frame vectors.
 * @retval 1=Success,0=Failure
 */
int writeBVHFile(
                  const char * filename,
                  const char * header,
                   std::vector<std::vector<float> > bvhFrames
                );


/**
 * @brief Load a MocapNET from .pb files on disk
 * @ingroup mocapnet
 * @param Pointer to a struct MocapNET that will hold the tensorflow instances on load.
 * @param Path to .pb files that are needed
 * @retval 1 = Success loading the files  , 0 = Failure
 */
int loadMocapNET(struct MocapNET * mnet,const char * filename,unsigned int forceCPU);

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
std::vector<float> runMocapNET(struct MocapNET * mnet,std::vector<float> input) ;





/**
 * @brief Deallocate tensorflow instances and free memory
 * @param Pointer to a valid and populated MocapNET instance
 * @retval 1=Success,0=Failure
 */
int unloadMocapNET(struct MocapNET * mnet);
