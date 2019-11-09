#include "jointEstimator2D.h"


int loadJointEstimator2D(struct JointEstimator2D * jnet,float qualitySetting,int usePAFs,unsigned int forceCPU)
{



          if (
                loadTensorflowInstance(
                                       &net,
                                       networkPath,
                                       networkInputLayer,
                                       networkOutputLayer,
                                       forceCPU2DJointEstimation
                                      )
               )
                {
                    return 1;

                } 
  return 0;
}
