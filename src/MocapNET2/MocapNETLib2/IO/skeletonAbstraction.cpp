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


#include <ctype.h> //toupper
int strcasecmp_sk(const char * input1,const char * input2)
{
  #if CASE_SENSITIVE_OBJECT_NAMES
    return strcmp(input1,input2);
  #endif

    if ( (input1==0) || (input2==0) )
     {
         fprintf(stderr,"Error , calling strcasecmp_internal with null parameters \n");
         return 1;
     }
    unsigned int len1 = strlen(input1);
    unsigned int len2 = strlen(input2);
    if (len1!=len2)
     {
         //mismatched lengths of strings , they can't be equal..!
         return 1;
     }

   char A; //<- character buffer for input1
   char B; //<- character buffer for input2
   unsigned int i=0;
   while (i<len1) //len1 and len2 are equal
    {
       A = toupper(input1[i]);
       B = toupper(input2[i]);
       if (A!=B) { return 1; }
       ++i;
    }
  //if we reached this point , there where no reasons
  //why input1 and input2 could not be equal..
  return 0;
}


int areFeetMissing(struct skeletonSerialized * skel)
{
    unsigned int activeJoints=0;
    for (unsigned int i=0; i<skel->skeletonBodyElements; i++)
    {
                if ( strcasecmp_sk(skel->skeletonHeader[i].str,"visible_rfoot")!=0)  { if (skel->skeletonBody[i].value) { ++activeJoints; }  }   
                if ( strcasecmp_sk(skel->skeletonHeader[i].str,"visible_rknee")!=0)  { if (skel->skeletonBody[i].value) { ++activeJoints; }  }   
                if ( strcasecmp_sk(skel->skeletonHeader[i].str,"visible_lfoot")!=0)  { if (skel->skeletonBody[i].value) { ++activeJoints; }  }   
                if ( strcasecmp_sk(skel->skeletonHeader[i].str,"visible_lknee")!=0)  { if (skel->skeletonBody[i].value) { ++activeJoints; }  } 
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
