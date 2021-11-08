#pragma once

#include "../../../Tensorflow/tf_utils.hpp"

#include <string.h>

//MOCAPNET2 ------------------------------------
#include "../../MocapNETLib2/mocapnet2.hpp"
#include "../../MocapNETLib2/tools.hpp"
#include "../../MocapNETLib2/IO/bvh.hpp"
#include "../../MocapNETLib2/IO/jsonRead.hpp"
#include "../../MocapNETLib2/IO/conversions.hpp"

#include "../../MocapNETLib2/remoteExecution.hpp"
//----------------------------------------------
#include "../../MocapNETLib2/solutionParts/body.hpp"
#include "../../MocapNETLib2/solutionParts/upperBody.hpp"
#include "../../MocapNETLib2/solutionParts/lowerBody.hpp"
#include "../../MocapNETLib2/solutionParts/rightHand.hpp"
#include "../../MocapNETLib2/solutionParts/leftHand.hpp"

std::vector<float> gatherResults(
    struct MocapNET2 * mnet,
    const std::vector<float> resultBody,
    const std::vector<float> resultUpperBody,
    const std::vector<float> resultLowerBody,
    const std::vector<float> resultLeftHand,
    const std::vector<float> resultRightHand,
    const std::vector<float> resultFace
);

std::vector<float> singleThreadedMocapNET(
                                           struct MocapNET2 * mnet,
                                           struct skeletonSerialized * input,
                                           int doLowerbody,
                                           int doHands,
                                           int doFace,
                                           int doGestureDetection,
                                           unsigned int useInverseKinematics,
                                           int doOutputFiltering
                                         );