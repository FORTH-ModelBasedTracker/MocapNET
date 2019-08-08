#pragma once


#include <iostream>
#include <vector>


int initializeBVHConverter();


unsigned int getBVHParentJoint(unsigned int currentJoint);

const char * getBVHJointName(unsigned int currentJoint);



std::vector<std::vector<float> > convertBVHFrameTo2DPoints(std::vector<float> bvhFrame,unsigned int width, unsigned int height);
//std::vector<std::vector<float> > convertBVHFrameTo2DPoints(std::vector<float> bvhFrame);
