#include "gestureRecognition.hpp"
#include "mocapnet.hpp"
#include "bvh.hpp"
#include "csv.hpp"

int addToMotionHistory(struct PoseHistory * poseHistoryStorage,std::vector<float> pose)
{
    poseHistoryStorage->history.push_back(pose);
     if (poseHistoryStorage->history.size() > poseHistoryStorage->maxPoseHistory)
                {
                    poseHistoryStorage->history.erase(poseHistoryStorage->history.begin());
                }
    return 1;
}



int dumpMotionHistory(const char * filename,struct PoseHistory * poseHistoryStorage)
{
  return writeBVHFile(filename,0,poseHistoryStorage->history);  
}