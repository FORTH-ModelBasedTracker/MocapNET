#pragma once
/** @file bvh.hpp
 *  @brief This is an interface to the BVH code. The BVH code ( https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader )
 *  might not be available. If this is the case then CMake will not declare the USE_BVH compilation flag and the whole code will not be included.
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>


/**
 * @brief Initialize BVH code ( if it is present )
 * @retval 1=Success/0=Failure
 */
int initializeBVHConverter();


/**
 * @brief Get the joint ID of the parent of a joint
 * @param JointID that we want to get the parent for
 * @retval JointID of the parent/ 0 in the case of an error
 * @bug 0 can both be a valid joint and signal an error, so it needs special care
 */
unsigned int getBVHParentJoint(unsigned int currentJoint);


/**
 * @brief Get a string with the name of the joint
 * @param JointID that we want to get the name for
 * @retval CString with the name, null means no result
 */
const char * getBVHJointName(unsigned int currentJoint);



/**
 * @brief Initialize BVH code ( if it is present )
 * @param  A float corresponding to a BVH frame
 * @param  The width of the 2D frame that will hold the 2D Points
 * @param  The height of the 2D frame that will hold the 2D Points
 * @retval Vector of 2D points
 */
std::vector<std::vector<float> > convertBVHFrameTo2DPoints(
                                                            std::vector<float> bvhFrame,
                                                            unsigned int width,
                                                            unsigned int height
                                                          );



/**
 * @brief Generate 2D points for a grid
 * @param  The width of the 2D frame that will hold the 2D Points
 * @param  The height of the 2D frame that will hold the 2D Points
 * @retval Vector of 2D points
 */
std::vector<std::vector<float> > convert3DGridTo2DPoints(
                                                         float roll,
                                                         float pitch,
                                                         float yaw,
                                                         unsigned int width,
                                                         unsigned int height
                                                        );
