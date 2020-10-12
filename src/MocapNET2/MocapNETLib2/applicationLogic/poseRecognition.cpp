#include "poseRecognition.hpp"
#include "../mocapnet2.hpp"
#include "../tools.hpp"
#include "../IO/bvh.hpp"
#include "../IO/csvRead.hpp"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */




int loadPoses(struct PoseDatabase * poseDB)
{
    unsigned  int poseID;
    char posePath[512]= {0};

    poseDB->poseChecksPerformed=0;

    for (poseID=0; poseID<hardcodedPoseNumber; poseID++)
        {
            snprintf(posePath,512,"dataset/poses/%s",hardcodedPoseName[poseID]);
            fprintf(stderr,"Loading Pose %u -> %s  : ",poseID,posePath);

          std::vector<std::vector<float> >  bvhFileLoaded = loadBVHFileMotionFrames(posePath); 
          
          if (bvhFileLoaded.size()==0)
          { 
            fprintf(stderr,RED "Pose %u -> %s  is corrupted\n" NORMAL,poseID,posePath);
          } else
          {
              poseDB->pose[poseID].pose=bvhFileLoaded[0]; 
             if (bvhFileLoaded.size()>1)
             {
                fprintf(stderr,YELLOW "Pose %u -> %s  has more than one frames\n" NORMAL,poseID,posePath); 
             }
          }
          
            
            if (poseDB->pose[poseID].pose.size()>0)
                {
                    fprintf(stderr,"Pose %s has %lu frames with %lu fields each\n",posePath,poseDB->pose[poseID].pose.size(),poseDB->pose[poseID].pose.size());
                    poseDB->pose[poseID].loaded=1;
                    poseDB->pose[poseID].poseCallback=0;
                    snprintf(poseDB->pose[poseID].label,128,"%s",hardcodedPoseName[poseID]);
                    poseDB->numberOfLoadedPoses +=1;
                    
                    fprintf(stderr,GREEN "Success , loaded a pose with %lu values\n" NORMAL,poseDB->pose[poseID].pose.size());
                }
            else
                {
                    fprintf(stderr,RED "Failure\n" NORMAL);
                }

        }

    return  (poseDB->numberOfLoadedPoses>0);
}


int areTwoBVHFramesCloseEnough(std::vector<float> vecA,std::vector<float> vecB,std::vector<char> active,float threshold)
{  
    if (vecA.size()!=vecB.size())
        {
            fprintf(stderr,"areTwoBVHFramesCloseEnough cannot compare different sized vectors..\n");
            return 0;
        }


    //Always skip X pos, Y pos, Z pos , and rotation
    for (int i=6; i<vecA.size(); i++)
        {
            float difference=abs(vecA[i]-vecB[i]);

            if (active.size()==0)
                {
                    //If there is no active vector consider all joints as active..
                    if (difference>threshold)
                        {
                            return 0;
                        }
                }
            else
                {
                    if (i>=active.size())
                        {
                            fprintf(stderr,RED "Failure comparing bvh frames due to short activeJoint vector\n" NORMAL);
                            return 0;
                        }
                    else
                        {
                            if ( (active[i]) && (difference>threshold) )
                                {
                                    return 0;
                                }
                        }
                }
        }


    return 1;
}



int getBVHFramesMSE(std::vector<float> vecA,std::vector<float> vecB,std::vector<char> active)
{
    if (vecA.size()!=vecB.size())
        {
            fprintf(stderr,"areTwoBVHFramesCloseEnough cannot compare different sized vectors..\n");
            return 0;
        }
    
    unsigned int samples=0;
    float total=0.0;

    //Always skip X pos, Y pos, Z pos , and rotation
    for (int i=6; i<vecA.size(); i++)
        {
            float difference=abs(vecA[i]-vecB[i]);

            if (active.size()==0)
                {
                    ++samples;
                    total += difference * difference;
                    //If there is no active vector consider all joints as active.. 
                }
            else
                {
                    if (i>=active.size())
                        {
                            fprintf(stderr,RED "Failure comparing bvh frames due to short activeJoint vector\n" NORMAL);
                            return 0;
                        }
                    else
                        if ( (active[i])  )
                                {
                                   ++samples;
                                    total += difference * difference;
                                } 
                }
        }
        
    return total/samples;
}



int isThisPoseFamiliar(
    struct PoseDatabase * poseDB,
    std::vector<float> currentPose,
    float percentageForDetection,
    float threshold
)
{
    std::vector<char> active;
    
    if (poseDB!=0)
    {
        for (unsigned int poseID=0; poseID<poseDB->numberOfLoadedPoses; poseID++)
        {
            
            float thisD = getBVHFramesMSE(currentPose,poseDB->pose[poseID].pose,active);
                fprintf(stderr,GREEN "Difference from pose %u ( %s  = %0.2f ) \n" NORMAL,poseID,hardcodedPoseName[poseID],thisD);
            
            if ( areTwoBVHFramesCloseEnough(currentPose,poseDB->pose[poseID].pose,active,threshold))
            {
                fprintf(stderr,GREEN "Detected pose %u ( %s ) \n" NORMAL,poseID,hardcodedPoseName[poseID]);
                return poseID;
            }     
        } 
    }
    
    fprintf(stderr,YELLOW "Not a familiar pose \n" NORMAL);
    return 0;
}