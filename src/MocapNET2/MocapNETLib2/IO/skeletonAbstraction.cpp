#include "skeletonAbstraction.hpp"

#include "csvRead.hpp"


#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string.h>
 

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

int areFeetMissing(struct skeletonSerialized * skel)
{
    unsigned int activeJoints=0;
    for (unsigned int i=0; i<skel->skeletonBodyElements; i++)
    {
                if ( strstr(skel->skeletonHeader[i].str,"visible_rfoot")!=0)  { if (skel->skeletonBody[i].value) { ++activeJoints; }  }   
                if ( strstr(skel->skeletonHeader[i].str,"visible_rknee")!=0) { if (skel->skeletonBody[i].value) { ++activeJoints; }  }   
                if ( strstr(skel->skeletonHeader[i].str,"visible_lfoot")!=0)   { if (skel->skeletonBody[i].value) { ++activeJoints; }  }   
                if ( strstr(skel->skeletonHeader[i].str,"visible_lknee")!=0)  { if (skel->skeletonBody[i].value) { ++activeJoints; }  } 
    }
    
    if (activeJoints<3) 
    {
 
      fprintf(stderr,YELLOW "Feet are missing, Only %u leg joints detected..\n" NORMAL,activeJoints);
      return 1; 
    }
    
    return 0;
}


int isLeftHardExtended(std::vector<float> result)
{
    //TODO:
    return 0;
}

int isRightHardExtended(std::vector<float> result)
{
    //TODO:
    return 0;
}


int  getPointOrientation(std::vector<float> result,float *x, float *y,float *r)
{
    //TODO:
    return 0;
}