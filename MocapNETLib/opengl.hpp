#pragma once 

/** @file opengl.hpp
 *  @brief Code that handles the OpenGL visualization 
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include <iostream>
#include <vector>

 
char * visualizeOpenGL(unsigned int *openGLFrameWidth,unsigned int *openGLFrameHeight);


int updateOpenGLView(std::vector<float> bvhFrame);