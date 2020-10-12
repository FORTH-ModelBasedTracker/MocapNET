#pragma once
/** @file jsonMocapNETHelpers.hpp
 *  @brief This file contains helpers to facilitate conversion between different skeleton formats.
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include "jsonRead.hpp"
#include <vector>


void addSkeletonJointFromTwoJoints(
    struct skeletonStructure * sk,
    std::vector<float> &result,
    int jointIDA,
    int jointIDB
);


void addSkeletonJoint(
    struct skeletonStructure * sk,
    std::vector<float> &result,
    int jointID
);

void addRightFinger(
    struct skeletonStructure * sk,
    std::vector<float> &result,
    int fingerJointA,
    int fingerJointB,
    int fingerJointC
);



void addLeftFinger(
    struct skeletonStructure * sk,
    std::vector<float> &result,
    int fingerJointA,
    int fingerJointB,
    int fingerJointC
);

std::vector<float> flattenskeletonCOCOToVector(struct skeletonStructure * sk,unsigned int width ,unsigned int height);



int convertBVHFrameToSkeletonCOCO(struct skeletonStructure * sk,std::vector<float>  bvhFrame,unsigned int width ,unsigned int height);