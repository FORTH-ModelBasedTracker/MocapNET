#pragma once 
/** @file opengl.hpp
 *  @brief Code that handles the OpenGL visualization. Adding an OpenGL visualization on top of this OpenCV based demo would require a pretty big
 *  code base that handles all the context data, as well as model loading, rendering etc.
 *  Instead of all these this program uses this OpenGL Sandbox 
 *  https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/ 
 *  A text file stored in dataset/human3D.conf controls all of the visualization details for the OpenGL window
 *  this code just has to initialize the sandbox and update the OpenGL view with the current state of the human skeleton.
 *  Then for each visualization call we get back a pointer to an RGB frame which can be viewed as a cv::mat after casting it to one
 *  It is the easiest possible way to integrate the functionality
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include <iostream>
#include <vector>

  
/**
* @brief This is a list of joints from the BVH file to an OpenCOLLADA model created using makehuman and this target armature ->  ( http://www.makehumancommunity.org/content/cmu_plus_face.html )
* There is a 1:1 correspondance of this array and the BVH output joints  (see MocapNETOutputJointNames ). Joints on this array that are marked as `-` or start with underscore ( _ ) are not active and are ignored
 * This list must also be in sync with the human3D.conf file that holds the 3D scene that gets rendered..
 * To check it do : cat human3D.conf | grep Bone | cut -d':' -f2  | tr '\n' '~' | tr -d '[:blank:]'  | sed  -e 's/\~/"\n\"/g' 
 * To also check if the BVH header used is in sync as well with the names here you can do : cat dataset/headerWithHeadAndOneMotion.bvh | grep JOINT
*/
static const char * OpenCOLLADANames[]=
{
"Hips", // 0 "hip",
"Hips", // 3  "hip",
"Spine", // 6  "abdomen",
 "Spine1", // 9  "chest",
 "Neck", // 12  "neck",
 "Neck1", // 15  "neck1",
 "Head", // 18  "head",
 "-", // 21  "__jaw",
 "jaw", // 24  "jaw",
 "special04", // 27 "special04",
 "oris02", // 30 "oris02",
 "oris01", // 33 "oris01",
 "oris06_L", // 36  "oris06.l",
 "oris07_L", // 39  "oris07.l",
 "oris06_R", // 42 "oris06.r",
 "oris07_R", // 45  "oris07.r",
 "tongue00", // 48 "tongue00",
 "tongue01", // 51 "tongue01",
 "tongue02", // 54 "tongue02",
 "tongue03", // 57 "tongue03",
 "-", // 60  "__tongue04",
 "tongue04", // 63 "tongue04",
 "tongue07_L", // 66 "tongue07.l",
 "tongue07_R", // 69 "tongue07.r",
 "tongue06_L", // 72 "tongue06.l",
 "tongue06_R", // 75 "tongue06.r",
 "tongue05_L", // 78 "tongue05.l",
 "tongue05_R", // 81 "tongue05.r",
 "-", // 84 "__levator02.l",
 "levator02_L", // 87 "levator02.l",
 "levator03_L", // 90 "levator03.l",
 "levator04_L", // 93 "levator04.l",
 "levator05_L", // 96 "levator05.l",
 "-", // 99 "__levator02.r",
 "levator02_R", // 102 "levator02.r",
 "levator03_R", // 105 "levator03.r",
 "levator04_R", // 108 "levator04.r",
 "levator05_R", // 111 "levator05.r",
 "-", // 114 "__special01", 
 "special01", // 117 "special01",
 "oris04_L", // 120   "oris04.l",
 "oris03_L", // 123 "oris03.l",
 "oris04_R", // 126 "oris04.r",
 "oris03_R", // 129 "oris03.r",
 "oris06", // 132 "oris06",
 "oris05", // 135 "oris05",
 "-", // 138 "__special03",
 "special03", // 141 "special03",
 "-", // 144 "__levator06.l",
 "levator06_L", // 147 "levator06.l",
 "-", // 150 "__levator06.r",
 "levator06_R", // 153 "levator06.r",
 "special06_L", // 156 "special06.l",
 "special05_L", // 159 "special05.l",
 "eye_L", // 162 "eye.l",
 "orbicularis03_L", // 165  "orbicularis03.l",
 "orbicularis04_L", // 168 "orbicularis04.l",
 "special06_R", // 171 "special06.r",
 "special05_R", // 174 "special05.r",
 "eye_R", // 177 "eye.r",
 "orbicularis03_R", // 180 "orbicularis03.r",
 "orbicularis04_R", // 183 "orbicularis04.r",
 "-", // 186 "__temporalis01.l",
 "temporalis01_L", // 189 "temporalis01.l",
 "oculi02_L", // 192 "oculi02.l",
 "oculi01_L", // 195 "oculi01.l",
 "__temporalis01_R", // 198 "__temporalis01.r",
 "temporalis01_R", // 201 "temporalis01.r",
 "oculi02_R", // 204 "oculi02.r",
 "oculi01_R", // 207 "oculi01.r",
 "__temporalis02_L", // 210 "__temporalis02.l",
 "temporalis02_L", // 213 "temporalis02.l",
 "risorius02_L", // 216 "risorius02.l",
 "risorius03_L", // 219 "risorius03.l",
 "__temporalis02_R", // 222 "__temporalis02.r",
 "temporalis02_R", // 225 "temporalis02.r",
 "risorius02_R", // 228 "risorius02.r",
 "risorius03_R", // 231 "risorius03.r",
 "RightShoulder", // 234 "rCollar", 
 "RightArm", // 237 "rShldr",
 "RightForeArm", // 240  "rForeArm",
 "RightHand", // 243"rHand",
 "metacarpal1_R", // 246 "metacarpal1.r",
 "finger2-1_R", // 249 "finger2-1.r",
 "finger2-2_R", // 252  "finger2-2.r",
 "finger2-3_R", // 255  "finger2-3.r",
 "metacarpal2_R", // 258 "metacarpal2.r",
 "finger3-1_R", // 261 "finger3-1.r",
 "finger3-2_R", // 264 "finger3-2.r",
 "finger3-3_R", // 267 "finger3-3.r",
 "__metacarpal3_R", // 270  "__metacarpal3.r",
 "metacarpal3_R", // 273  "metacarpal3.r",
 "finger4-1_R", // 276  "finger4-1.r",
 "finger4-2_R", // 279 "finger4-2.r",
 "finger4-3_R", // 282  "finger4-3.r",
 "__metacarpal4_R", // 285  "__metacarpal4.r",
 "metacarpal4_R", // 288 "metacarpal4.r",
 "finger5-1_R", // 291 "finger5-1.r",
 "finger5-2_R", // 294 "finger5-2.r",
 "finger5-3_R", // 297  "finger5-3.r",
 "__rthumb", // 300 "__rthumb",
 "RThumb", // 303 "rthumb",
 "finger1-2_R", // 306 "finger1-2.r",
 "finger1-3_R", // 309 "finger1-3.r",
 "LeftShoulder", // 312 "lCollar", 
 "LeftArm", // 315 "lShldr",
 "LeftForeArm", // 318 "lForeArm",
 "LeftHand", // 321 "lHand",
 "metacarpal1_L", // 324  "metacarpal1.l",
 "finger2-1_L", // 327 "finger2-1.l",
 "finger2-2_L", // 330 "finger2-2.l",
 "finger2-3_L", // 333 "finger2-3.l",
 "metacarpal2_L", // 336 "metacarpal2.l",
 "finger3-1_L", // 339 "finger3-1.l",
 "finger3-2_L", // 342 "finger3-2.l",
 "finger3-3_L", // 345 "finger3-3.l",
 "__metacarpal3_L", // 348 "__metacarpal3.l",
 "metacarpal3_L", // 351 "metacarpal3.l",
 "finger4-1_L", // 354 "finger4-1.l",
 "finger4-2_L", // 357 "finger4-2.l",
 "finger4-3_L", // 360 "finger4-3.l",
 "__metacarpal4_L", // 363 "__metacarpal4.l",
 "metacarpal4_L", // 366 "metacarpal4.l",
 "finger5-1_L", // 369 "finger5-1.l",
 "finger5-2_L", // 372 "finger5-2.l",
 "finger5-3_L", // 375 "finger5-3.l",
 "__lthumb", // 378 "__lthumb",
 "LThumb", // 381 "lthumb",
 "finger1-2_L", // 384 "finger1-2.l",
 "finger1-3_L", // 387 "finger1-3.l",
 "RHipJoint", // 390 "rButtock",
 "RightUpLeg", // 393  "rThigh",
 "RightLeg", // 396 "rShin",
 "RightFoot", // 399 "rFoot",
 "_toe1-1_R", // 402 "toe1-1.R",  <- This does not currently exist in the makehuman model
 "_toe1-2_R", // 405 "toe1-2.R", <- This does not currently exist in the makehuman model
 "_toe2-1_R", // 408 "toe2-1.R", <- This does not currently exist in the makehuman model
 "_toe2-2_R", // 411 "toe2-2.R", <- This does not currently exist in the makehuman model
 "_toe2-3_R", // 414 "toe2-3.R", <- This does not currently exist in the makehuman model
 "_toe3-1_R", // 417 "toe3-1.R", <- This does not currently exist in the makehuman model
 "_toe3-2_R", // 420 "toe3-2.R", <- This does not currently exist in the makehuman model
 "_toe3-3_R", // 423 "toe3-3.R", <- This does not currently exist in the makehuman model
 "_toe4-1_R", // 426 "toe4-1.R", <- This does not currently exist in the makehuman model
 "_toe4-2_R", // 429 "toe4-2.R", <- This does not currently exist in the makehuman model
 "_toe4-3_R", // 432 "toe4-3.R", <- This does not currently exist in the makehuman model
 "_toe5-1_R", // 435 "toe5-1.R", <- This does not currently exist in the makehuman model
 "_toe5-2_R", // 438 "toe5-2.R", <- This does not currently exist in the makehuman model
 "_toe5-3_R", // 441 "toe5-3.R", <- This does not currently exist in the makehuman model
 "LHipJoint", // 444 "lButtock",
 "LeftUpLeg", // 447  "lThigh",
 "LeftLeg", // 450 "lShin",
 "LeftFoot", // 453 "lFoot",
 "_toe1-1_L", // 456 "toe1-1.L", <- This does not currently exist in the makehuman model
 "_toe1-2_L", // 459 "toe1-2.L",  <- This does not currently exist in the makehuman model
 "_toe2-1_L", // 462 "toe2-1.L", <- This does not currently exist in the makehuman model
 "_toe2-2_L", // 465 "toe2-2.L", <- This does not currently exist in the makehuman model
 "_toe2-3_L", // 468 "toe2-3.L", <- This does not currently exist in the makehuman model
 "_toe3-1_L", // 471 "toe3-1.L", <- This does not currently exist in the makehuman model
 "_toe3-2_L", // 474 "toe3-2.L", <- This does not currently exist in the makehuman model
 "_toe3-3_L", // 477 "toe3-3.L", <- This does not currently exist in the makehuman model
 "_toe4-1_L", // 480 "toe4-1.L", <- This does not currently exist in the makehuman model
 "_toe4-2_L", // 483 "toe4-2.L", <- This does not currently exist in the makehuman model
 "_toe4-3_L", // 486 "toe4-3.L", <- This does not currently exist in the makehuman model
 "_toe5-1_L", // 489 "toe5-1.L", <- This does not currently exist in the makehuman model
 "_toe5-2_L", // 492 "toe5-2.L", <- This does not currently exist in the makehuman model
 "_toe5-3_L", // 495 "toe5-3.L" <- This does not currently exist in the makehuman model
 };


#if USE_BVH
#include "../../../../dependencies/RGBDAcquisition/tools/Calibration/calibration.h"
int changeOpenGLCalibration(struct calibration * calib);
#endif


int clickOpenGLView(unsigned int x,unsigned int y,unsigned int state);


int initializeOpenGLStuff(unsigned int openGLFrameWidth,unsigned int openGLFrameHeight);

/**
 * @brief This function uses the file dataset/human3D.conf and the current scene state as updated using updateOpenGLView
 * @param Pointer to a value that is both input and output width of the OpenGL frame. The renderer will try to use the provided width but it might not be exactly the same.
 * @param Pointer to a value that is both input and output height of the OpenGL frame. The renderer will try to use the provided height but it might not be exactly the same.
 * @ingroup tools
 * @retval 0=Failure else a pointer to an RGB image that holds the OpenGL rendering
 */
char * visualizeOpenGL(unsigned int *openGLFrameWidth,unsigned int *openGLFrameHeight);


/**
 * @brief This function provides the 3D pose extracted to the OpenGL context so that the armature will be rendered accordingly
 * @param MocapNET BVH output
 * @ingroup tools
 * @retval 1=Success/0=Failure
 */
int updateOpenGLView(std::vector<float> bvhFrame);