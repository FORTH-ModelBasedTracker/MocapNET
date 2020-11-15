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

#if USE_BVH
#include "../../../../dependencies/RGBDAcquisition/tools/PThreadWorkerPool/pthreadWorkerPool.h"
#endif


struct mocapNETContext
{
    struct MocapNET2 * mnet;
    struct skeletonSerialized * input;
    int doLowerbody;
    int doHands;
    int doFace;
    int doGestureDetection;
    unsigned int useInverseKinematics;
    int doOutputFiltering;
    int forceFront;
};

std::vector<float> multiThreadedMocapNET(
                                           struct MocapNET2 * mnet,
                                           struct skeletonSerialized * input,
                                           int doLowerbody,
                                           int doHands,
                                           int doFace,
                                           int doGestureDetection,
                                           unsigned int useInverseKinematics,
                                           int doOutputFiltering
                                         );
