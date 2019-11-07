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