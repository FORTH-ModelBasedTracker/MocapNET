#pragma once
/** @file bvh.hpp
 *  @brief This is an interface to the BVH code. The BVH code ( https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader )
 *  might not be available. If this is the case then CMake will not declare the USE_BVH compilation flag and the whole code will not be included.
 *  @author Ammar Qammaz (AmmarkoV)
 */

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
 * @brief Get the jointID based on a string with the name of the joint
 * @param CString with the name we want to search for
 * @retval JointID that we want to get the name for, 0 means no result or the first joint
 */
unsigned int getBVHJointIDFromJointName(const char * jointName);

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
    unsigned int height,
    unsigned int dimensions
);
