/** @file EDM.h
 *  @brief An implementation of an EDM descriptor
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef EDM_H_INCLUDED
#define EDM_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

#include "../JSON/readModelConfiguration.h"
#include "NSxM.h"
#include <math.h>


static int countEDMElements(int numberOfJointRules)
{
  int count = 0;
  for (int i=0; i<numberOfJointRules; i++)
  { 
   for (int j=0; j<numberOfJointRules; j++)
    {
      if (i>j)
       {
         count+=1;
       } 
    }
  }
  return count;
}

/** @brief This function returns the euclidean distance between two input 2D joints and zero if either of them is invalid*/
static float getJoint2DDistanceEDM(float aX,float aY,float bX,float bY)
{
    if ( ((aX==0) && (aY==0)) || ((bX==0) && (bY==0)) ) 
        {
         return 0.0;
        }
    //-------------------------
    float xDistance=(float) bX-aX;
    float yDistance=(float) bY-aY;
    float result = sqrt( (xDistance*xDistance) + (yDistance*yDistance) );
    
    if (result!=result)
    { 
        fprintf(stderr,"getJoint2DDistanceEDM yielded NaN..\n");
        return 0.0; 
    }
    
    return result;
}

static int appendEDMElements(
                             float * input2DJoints,
                             unsigned int input2DJointsLength,
                             float * output,
                             struct ModelConfigurationData* rules 
                            )
{
  int numberOfJointRules = rules->numberOfDescriptorElements;
  //----------------------
  float iX=0.0,iY=0.0,iVisibility=0.0;
  int iInvalidPoint=0; 
  //----------------------
  float jX=0.0,jY=0.0,jVisibility=0.0;
  int jInvalidPoint=0;
  //----------------------  
  int count = 0;
  for (int i=0; i<numberOfJointRules; i++)
  {
   getCompositePoint(&iX,&iY,&iVisibility,&iInvalidPoint,rules,i,input2DJoints,input2DJointsLength);
   for (int j=0; j<numberOfJointRules; j++)
    {
      if (i>j)
       {
         getCompositePoint(&jX,&jY,&jVisibility,&jInvalidPoint,rules,j,input2DJoints,input2DJointsLength);
         if ( (iInvalidPoint) && (jInvalidPoint) ) //<- Why is this AND and not OR ? also in EDM.py code
         {
            output[count] = 0.0;
         } else
         { 
            output[count] = getJoint2DDistanceEDM(iX,iY,jX,jY);
         }
         count+=1;
       } 
    }
  }
  
  return count;
}

#ifdef __cplusplus
}
#endif




#endif  
