#pragma once
/** @file jsonMocapNETHelpers.hpp
 *  @brief This file contains helpers to facilitate conversion between different skeleton formats.
 *  @author Ammar Qammaz (AmmarkoV)
 */
#include "jsonCocoSkeleton.h"
#include <vector>


void addSkeletonJointFromTwoJoints(
    struct skeletonCOCO * sk,
    std::vector<float> &result,
    int jointIDA,
    int jointIDB
);


void addSkeletonJoint(
    struct skeletonCOCO * sk,
    std::vector<float> &result,
    int jointID
);

void addRightFinger(
    struct skeletonCOCO * sk,
    std::vector<float> &result,
    int fingerJointA,
    int fingerJointB,
    int fingerJointC
);



void addLeftFinger(
    struct skeletonCOCO * sk,
    std::vector<float> &result,
    int fingerJointA,
    int fingerJointB,
    int fingerJointC
);

std::vector<float> flattenskeletonCOCOToVector(struct skeletonCOCO * sk,unsigned int width ,unsigned int height);
