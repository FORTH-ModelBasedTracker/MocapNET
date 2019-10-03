#include "gestureRecognition.hpp"


int addToMotionHistory(struct PoseHistory * poseHistoryStorage,std::vector<float> pose)
{
    poseHistoryStorage->history.push_back(pose);
     if (poseHistoryStorage->history.size() > poseHistoryStorage->maxPoseHistory)
                {
                    poseHistoryStorage->history.erase(poseHistoryStorage->history.begin());
                }
    return 1;
}