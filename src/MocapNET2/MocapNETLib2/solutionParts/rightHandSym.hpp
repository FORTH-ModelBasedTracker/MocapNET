#pragma once
/** @file rightHandSym.hpp
 *  @brief Code that  handles getting 2D joints packed in their skeletonSerialized and vector formats and can run the tensorflow code retrieving a 3D BVH motion frame that estimates the human right hand based on the left hand (!)
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include "../mocapnet2.hpp"

#include "rightHand.hpp"
 
/**
 * @brief This call converts 2D input that is formatted on a skeletonSerialized structure to a 3D pose vector. You need to call mocapnetRightHand_initializeAssociations and mocapnetRightHand_initialize before calling this function 
 * and if you want to convert the output result to the final result vector you need to use the mocapnetRightHand_fillResultVector call.
 * @param MocapNET instance
 * @param a skeletonSerialized structure that holds our 2D input 
 * correct ensemble for the perceived orientation.
 * @retval 3D pose output that needs to be processed through mocapnetRightHand_fillResultVector to fill the final BVH buffer
 */ 
std::vector<float> mocapnetRightHandSym_evaluateInput(struct MocapNET2 * mnet,struct skeletonSerialized * input,float scaleInput);

