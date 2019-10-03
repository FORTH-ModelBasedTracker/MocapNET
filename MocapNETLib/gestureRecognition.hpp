#pragma once

#include <iostream>
#include <vector>



/**
 * @brief MocapNET consists of separate classes/ensembles that are invoked for particular orientations.
 * This structure holds the required tensorflow instances to make MocapNET work.
 */
struct PoseHistory
{ 
    unsigned int maxPoseHistory;
    std::vector<std::vector<float> > history;
};

int addToMotionHistory(struct PoseHistory * poseHistoryStorage,std::vector<float> pose);