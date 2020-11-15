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

std::vector<float> gatherResults(
    struct MocapNET2 * mnet,
    std::vector<float> resultBody,
    std::vector<float> resultUpperBody,
    std::vector<float> resultLowerBody,
    std::vector<float> resultLeftHand,
    std::vector<float> resultRightHand,
    std::vector<float> resultFace
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
