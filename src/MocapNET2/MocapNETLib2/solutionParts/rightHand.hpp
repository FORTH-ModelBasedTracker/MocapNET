#pragma once
/** @file rightHand.hpp
 *  @brief Code that  handles getting 2D joints packed in their skeletonSerialized and vector formats and can run the tensorflow code retrieving a 3D BVH motion frame that estimates the human right hand
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include "../mocapnet2.hpp"



enum mocapNET_standalone_qbvh_rhand_joint_enum
{
    BVH_STANDALONE_RHAND_RHAND = 0,      //0
    //-------------------------------------------
    BVH_STANDALONE_RHAND_METACARPAL_1_R, //1
    BVH_STANDALONE_RHAND_FINGER2_1_R,    //2
    BVH_STANDALONE_RHAND_FINGER2_2_R,    //3
    BVH_STANDALONE_RHAND_FINGER2_3_R,    //4
    BVH_STANDALONE_ENDSITE_FINGER2_3_R,  //5
    //-------------------------------------------
    BVH_STANDALONE_RHAND_METACARPAL_2_R, //6
    BVH_STANDALONE_RHAND_FINGER3_1_R,    //7
    BVH_STANDALONE_RHAND_FINGER3_2_R,    //8
    BVH_STANDALONE_RHAND_FINGER3_3_R,    //9
    BVH_STANDALONE_ENDSITE_FINGER3_3_R,  //10
    //-------------------------------------------
    BVH_STANDALONE_RHAND__METACARPAL_3_R,//11
    BVH_STANDALONE_RHAND_METACARPAL_3_R, //12
    BVH_STANDALONE_RHAND_FINGER4_1_R,    //13
    BVH_STANDALONE_RHAND_FINGER4_2_R,    //14
    BVH_STANDALONE_RHAND_FINGER4_3_R,    //15
    BVH_STANDALONE_ENDSITE_FINGER4_3_R,  //16
    //-------------------------------------------
    BVH_STANDALONE_RHAND__METACARPAL_4_R,//17
    BVH_STANDALONE_RHAND_METACARPAL_4_R, //18
    BVH_STANDALONE_RHAND_FINGER5_1_R,    //19
    BVH_STANDALONE_RHAND_FINGER5_2_R,    //20
    BVH_STANDALONE_RHAND_FINGER5_3_R,    //21
    BVH_STANDALONE_ENDSITE_FINGER5_3_R,  //22 
    //-------------------------------------------
    BVH_STANDALONE_RHAND_RTHUMBBASE,        //23
    BVH_STANDALONE_RHAND_RTHUMB,         //24
    BVH_STANDALONE_RHAND_FINGER1_2_R,    //25
    BVH_STANDALONE_RHAND_FINGER1_3_R,    //26
    BVH_STANDALONE_ENDSITE_FINGER1_3_R,  //27
    //-------------------------------------------
    BVH_STANDALONE_RHAND_JOINT_NUMBER
};


enum mocapNET_standalone_qbvh_rhand_enum
{
    MNET_STANDALONE_RHAND_OUTPUT_RHAND_XPOSITION = 0,      //0
    MNET_STANDALONE_RHAND_OUTPUT_RHAND_YPOSITION,          //1
    MNET_STANDALONE_RHAND_OUTPUT_RHAND_ZPOSITION,          //2
    //-------------------------------------------
    //Assuming the .qbvh file..
    MNET_STANDALONE_RHAND_OUTPUT_RHAND_WROTATION,          //3
    MNET_STANDALONE_RHAND_OUTPUT_RHAND_XROTATION,          //4
    MNET_STANDALONE_RHAND_OUTPUT_RHAND_YROTATION,          //5
    MNET_STANDALONE_RHAND_OUTPUT_RHAND_ZROTATION,          //6
    //-------------------------------------------
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_1_R_ZROTATION, //7
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_1_R_XROTATION, //8
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_1_R_YROTATION, //9
    MNET_STANDALONE_RHAND_OUTPUT_FINGER2_1_R_ZROTATION,    //10
    MNET_STANDALONE_RHAND_OUTPUT_FINGER2_1_R_XROTATION,    //11
    MNET_STANDALONE_RHAND_OUTPUT_FINGER2_1_R_YROTATION,    //12
    MNET_STANDALONE_RHAND_OUTPUT_FINGER2_2_R_ZROTATION,    //13
    MNET_STANDALONE_RHAND_OUTPUT_FINGER2_2_R_XROTATION,    //14
    MNET_STANDALONE_RHAND_OUTPUT_FINGER2_2_R_YROTATION,    //15
    MNET_STANDALONE_RHAND_OUTPUT_FINGER2_3_R_ZROTATION,    //16
    MNET_STANDALONE_RHAND_OUTPUT_FINGER2_3_R_XROTATION,    //17
    MNET_STANDALONE_RHAND_OUTPUT_FINGER2_3_R_YROTATION,    //18
    //-------------------------------------------
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_2_R_ZROTATION, //19
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_2_R_XROTATION, //20
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_2_R_YROTATION, //21
    MNET_STANDALONE_RHAND_OUTPUT_FINGER3_1_R_ZROTATION,    //22
    MNET_STANDALONE_RHAND_OUTPUT_FINGER3_1_R_XROTATION,    //23
    MNET_STANDALONE_RHAND_OUTPUT_FINGER3_1_R_YROTATION,    //24
    MNET_STANDALONE_RHAND_OUTPUT_FINGER3_2_R_ZROTATION,    //25
    MNET_STANDALONE_RHAND_OUTPUT_FINGER3_2_R_XROTATION,    //26
    MNET_STANDALONE_RHAND_OUTPUT_FINGER3_2_R_YROTATION,    //27
    MNET_STANDALONE_RHAND_OUTPUT_FINGER3_3_R_ZROTATION,    //28
    MNET_STANDALONE_RHAND_OUTPUT_FINGER3_3_R_XROTATION,    //29
    MNET_STANDALONE_RHAND_OUTPUT_FINGER3_3_R_YROTATION,    //30
    //-------------------------------------------
    MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_3_R_ZROTATION,//31
    MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_3_R_XROTATION,//32
    MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_3_R_YROTATION,//33
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_3_R_ZROTATION, //34
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_3_R_XROTATION, //35
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_3_R_YROTATION, //36
    MNET_STANDALONE_RHAND_OUTPUT_FINGER4_1_R_ZROTATION,    //37
    MNET_STANDALONE_RHAND_OUTPUT_FINGER4_1_R_XROTATION,    //38
    MNET_STANDALONE_RHAND_OUTPUT_FINGER4_1_R_YROTATION,    //39
    MNET_STANDALONE_RHAND_OUTPUT_FINGER4_2_R_ZROTATION,    //40
    MNET_STANDALONE_RHAND_OUTPUT_FINGER4_2_R_XROTATION,    //41
    MNET_STANDALONE_RHAND_OUTPUT_FINGER4_2_R_YROTATION,    //42
    MNET_STANDALONE_RHAND_OUTPUT_FINGER4_3_R_ZROTATION,    //43
    MNET_STANDALONE_RHAND_OUTPUT_FINGER4_3_R_XROTATION,    //44
    MNET_STANDALONE_RHAND_OUTPUT_FINGER4_3_R_YROTATION,    //45
    //-------------------------------------------
    MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_4_R_ZROTATION,//46
    MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_4_R_XROTATION,//47
    MNET_STANDALONE_RHAND_OUTPUT__METACARPAL_4_R_YROTATION,//48
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_4_R_ZROTATION, //49
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_4_R_XROTATION, //50
    MNET_STANDALONE_RHAND_OUTPUT_METACARPAL_4_R_YROTATION, //51
    MNET_STANDALONE_RHAND_OUTPUT_FINGER5_1_R_ZROTATION,    //52
    MNET_STANDALONE_RHAND_OUTPUT_FINGER5_1_R_XROTATION,    //53
    MNET_STANDALONE_RHAND_OUTPUT_FINGER5_1_R_YROTATION,    //54
    MNET_STANDALONE_RHAND_OUTPUT_FINGER5_2_R_ZROTATION,    //55
    MNET_STANDALONE_RHAND_OUTPUT_FINGER5_2_R_XROTATION,    //56
    MNET_STANDALONE_RHAND_OUTPUT_FINGER5_2_R_YROTATION,    //57
    MNET_STANDALONE_RHAND_OUTPUT_FINGER5_3_R_ZROTATION,    //58
    MNET_STANDALONE_RHAND_OUTPUT_FINGER5_3_R_XROTATION,    //59
    MNET_STANDALONE_RHAND_OUTPUT_FINGER5_3_R_YROTATION,    //60
    //-------------------------------------------
    MNET_STANDALONE_RHAND_OUTPUT_RTHUMBBASE_ZROTATION,        //61
    MNET_STANDALONE_RHAND_OUTPUT_RTHUMBBASE_XROTATION,        //62
    MNET_STANDALONE_RHAND_OUTPUT_RTHUMBBASE_YROTATION,        //63
    MNET_STANDALONE_RHAND_OUTPUT_RTHUMB_ZROTATION,         //64
    MNET_STANDALONE_RHAND_OUTPUT_RTHUMB_XROTATION,         //65
    MNET_STANDALONE_RHAND_OUTPUT_RTHUMB_YROTATION,         //66
    MNET_STANDALONE_RHAND_OUTPUT_FINGER1_2_R_ZROTATION,    //67
    MNET_STANDALONE_RHAND_OUTPUT_FINGER1_2_R_XROTATION,    //68
    MNET_STANDALONE_RHAND_OUTPUT_FINGER1_2_R_YROTATION,    //69
    MNET_STANDALONE_RHAND_OUTPUT_FINGER1_3_R_ZROTATION,    //70
    MNET_STANDALONE_RHAND_OUTPUT_FINGER1_3_R_XROTATION,    //71
    MNET_STANDALONE_RHAND_OUTPUT_FINGER1_3_R_YROTATION,    //72
    //-------------------------------------------
    MNET_STANDALONE_RHAND_OUTPUT_NUMBER
};
 


/**
 * @brief Since the C/C++ code in this repository depends on a seperate python trainer, and the networks can differ depending on training parameters (and this is a research project) the series of 2D joints
 * can often change. It is also very expensive to do string matching on every frame, so before evaluating any input this call has to be executed in order to perform the correct array associations and from then on
 * we can pass 2D data without searching for labels on each frame. It needs to be called before any mocapnetRightHand_evaluateInput call
 * @param MocapNET instance
 * @param a skeletonSerialized structure that holds our 2D input
 * @retval 1=Success/0=Failure
 */ 
int mocapnetRightHand_initializeAssociations(struct MocapNET2 * mnet, struct skeletonSerialized * input);


int mocapnetRightHand_initializeUsingAssociationsOfLeftHand(struct MocapNET2 * mnet, struct skeletonSerialized * input);

/**
 * @brief This call loads and initializes the required Tensorflow models for the specific configuration requested, it needs to be called before any mocapnetRightHand_evaluateInput call
 * @param MocapNET instance
 * @param This parameter is currently omitted
 * @param MocapNET 1 supported multiple quality settings, however for MocapNET2 you should default to 1.0
 * @param MocapNET 1 used 3 ensembles, MocapNET 2 uses 5 ensembles, so mode should default to 5 
 * @param The network can be executed on the GPU or the CPU, if you supply 1 you will force CPU execution, if not MocapNET will try to run it on your GPU ( if tensorflow finds it )  
 * @retval 1=Success/0=Failure
 */ 
int mocapnetRightHand_initialize(struct MocapNET2 * mnet,const char * filename,float qualitySetting,unsigned int mode,unsigned int forceCPU);

/**
 * @brief This call deallocates the tensorflow models
 * @param MocapNET instance
 * @retval 1=Success/0=Failure
 */ 
int mocapnetRightHand_unload(struct MocapNET2 * mnet);

/**
 * @brief This call inserts the 3D pose extracted from tensorflow into the final resulting BVH vector
 * @param The final BVH vector that we want to populate with right hand data
 * @param The right hand result
 * @retval 1=Success/0=Failure
 */ 
int mocapnetRightHand_fillResultVector(std::vector<float> &finalResultVector,std::vector<float> resultBody);

int mocapnetRightHand_fillStandaloneResultVector(std::vector<float> &result ,std::vector<float> resultRHand,int usesQuaternion);

/**
 * @brief This call converts 2D input that is formatted on a skeletonSerialized structure to a 3D pose vector. You need to call mocapnetRightHand_initializeAssociations and mocapnetRightHand_initialize before calling this function 
 * and if you want to convert the output result to the final result vector you need to use the mocapnetRightHand_fillResultVector call.
 * @param MocapNET instance
 * @param a skeletonSerialized structure that holds our 2D input 
 * correct ensemble for the perceived orientation.
 * @retval 3D pose output that needs to be processed through mocapnetRightHand_fillResultVector to fill the final BVH buffer
 */ 
std::vector<float> mocapnetRightHand_evaluateInput(struct MocapNET2 * mnet,struct skeletonSerialized * input,float scaleInput);

