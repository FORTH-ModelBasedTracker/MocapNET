/** @file NSDM.h
 *  @brief An implementation of an NSRM descriptor
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef NSDM_H_INCLUDED
#define NSDM_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif


#include "../JSON/readModelConfiguration.h"
#include "NSxM.h"
#include <math.h>

static int countNSDMElements(int numberOfJointRules)
{
  return 2*numberOfJointRules*numberOfJointRules;
}



static int appendNSDMElements(
                              float * input2DJoints,
                              struct descriptor * output,
                              int numberOfJointRules
                             )
{
  return 0;
}



#ifdef __cplusplus
}
#endif




#endif  
