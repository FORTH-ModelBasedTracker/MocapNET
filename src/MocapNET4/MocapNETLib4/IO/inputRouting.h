/** @file inputRouting.h
 *  @brief An implementation of input routing for the input joints
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef INPUT_ROUTING_H_INCLUDED
#define INPUT_ROUTING_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif


#include "../JSON/readModelConfiguration.h"
#include "../NSxM/NSxM.h"
#include <math.h>


/**
 * @brief This is an array of names for all the new BODY25 body parts.
 * This tries to mirror body_configuration.json and everything used is lowercase exactly for this reason..
 * it also has to be the same with the bvh file headerWithHeadAndOneMotion.bvh
 */
static const char * Body25Labels[] =
{
    "head",            //0
    "neck",            //1
    "rshoulder",       //2
    "relbow",          //3
    "rhand",           //4
    "lshoulder",       //5
    "lelbow",          //6
    "lhand",           //7
    "hip",             //8
    "rhip",            //9
    "rknee",           //10
    "rfoot",           //11
    "lhip",            //12
    "lknee",           //13
    "lfoot",           //14
    "endsite_eye.r",   //15
    "endsite_eye.l",   //16
    "rear",            //17 ========= No correspondance
    "lear",            //18 ========= No correspondance
    "endsite_toe1-2.l",//19
    "endsite_toe5-3.l",//20
    "lheel",           //21 ========= No correspondance
    "endsite_toe1-2.r",//22
    "endsite_toe5-3.r",//23
    "rheel",           //24 ========= No correspondance
    "bkg",             //25 ========= No correspondance
    //==================
    "End of Joint Names",
    0
};




#include <ctype.h> //toupper
static int strcasecmp_route(const char * input1,const char * input2)
{
    if ( (input1==0) || (input2==0) )
     {
         fprintf(stderr,"Error , calling strcasecmp_route with null parameters \n");
         return 1;
     }
     
  #if CASE_SENSITIVE_OBJECT_NAMES
    return strcmp(input1,input2);
  #endif

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




struct inputRouting
{
  char    resolved;
  float * routedValues;
  int  *  routingRules;
  int     numberOfRoutingRules;
};



static int destroyRoute(struct inputRouting * route)
{
  if (route==0)  { return 0; }
  if (route->routedValues!=0) { free(route->routedValues); route->routedValues=0; }
  if (route->routingRules!=0) { free(route->routingRules); route->routingRules=0; }
  route->numberOfRoutingRules=0;
  route->resolved=0;
  return 1;
}
                                  
                                  
                                  
/**
 * @brief generate Route from Labels
 * @param Pointer to a model configuration 
 * @param Pointer to the route structure
 * @param Labels to route
 * @param Number of Labels to route
 * @retval 1=Success/0=Failure
 */
static int generateRouteFromLabels(
                                    struct ModelConfigurationData * config,
                                    struct inputRouting * route,
                                    const char * * incomingLabels,
                                    unsigned int incomingLabelsLength
                                  )
{
  fprintf(stderr,GREEN "\ngenerateRouteFromLabels for %u labels and %u hierarchy elements\n" NORMAL,incomingLabelsLength,config->numberOfHierarchyElements);
  
  if (config==0)         { return 0; }
  if (incomingLabels==0) { return 0; }
  if (route==0)          { return 0; }
   

  destroyRoute(route);
  
  route->numberOfRoutingRules = config->numberOfHierarchyElements;
  
  route->routedValues = (float*) malloc(sizeof(float) * 3 * route->numberOfRoutingRules);
  route->routingRules = (int*)   malloc(sizeof(int)   * route->numberOfRoutingRules);
  
  if ( 
       (route->routedValues==0) ||
       (route->routingRules==0)
     ) //Failed allocating memory.. 
     { 
        destroyRoute(route);
        return 0; 
     }
  

  memset(route->routedValues,0,sizeof(float) * 3 * route->numberOfRoutingRules);
  memset(route->routingRules,0,sizeof(int) * route->numberOfRoutingRules);
  
  
  int routingFailures = 0;
  for (int trg=0; trg<config->numberOfHierarchyElements; trg++)
  {
        //fprintf(stderr,"trg %u/%u\n",trg,config->numberOfHierarchyElements);
        int trgJointResolved = 0;
        
        for (int src=0; src<incomingLabelsLength; src++)
        {
          //fprintf(stderr,"src %u/%u\n",src,incomingLabelsLength);
          
          if (strcasecmp_route(incomingLabels[src],config->hierarchyElements[trg].joint)==0)
            {
                fprintf(stderr,GREEN "MATCH %s (%u) to %s (%u) \n" NORMAL,incomingLabels[src],src,config->hierarchyElements[trg].joint,trg);
                trgJointResolved = 1;
                route->routedValues[trg*3+0] = 0.0;//Set everything to zero initially..
                route->routedValues[trg*3+1] = 0.0;//Set everything to zero initially..
                route->routedValues[trg*3+2] = 0.0;//Set everything to zero initially..
                route->routingRules[trg] = src;
            }
        }


        if (!trgJointResolved)
        {
          fprintf(stderr,YELLOW "Could not match %s \n" NORMAL,config->hierarchyElements[trg].joint);
          routingFailures+=1;
        }
  }
 
   if (routingFailures==0)
   {
          fprintf(stderr,GREEN "Successfully routed all %u input rules\n" NORMAL,route->numberOfRoutingRules);
   } else
   {
          fprintf(stderr,RED "Failed routing %u out of %u input rules\n" NORMAL, routingFailures,route->numberOfRoutingRules);
   }
   
  route->resolved = (routingFailures==0);
  
  return (routingFailures==0);
}



static int routeInput(
                       float * preexistingOutput2DJoints,
                       int * output2DJointsLength,
                       struct ModelConfigurationData * config,
                       struct inputRouting * route,
                       float * raw2DPoints,
                       int raw2DPointsLength
                     )
{
  if (route->resolved)
  {
   //if (raw2DPointsLength==route->numberOfRoutingRules)
   {
      float * output = preexistingOutput2DJoints;

      int val = 0;
      for (int i=0; i<route->numberOfRoutingRules; i++)
       {
         fprintf(stderr,"%s ",config->hierarchyElements[i].joint);
         //Each rule has 3 values 2DX, 2DY, 2DVisibility 
         //--------------------------------------------------------------------------
         output[val]=raw2DPoints[(route->routingRules[i]*3) + 0]; 
         fprintf(stderr,"2DX=%0.2f ",output[val]);
         val+=1;
         //--------------------------------------------------------------------------
         output[val]=raw2DPoints[(route->routingRules[i]*3) + 1]; 
         fprintf(stderr,"2DY=%0.2f ",output[val]);
         val+=1;
         //--------------------------------------------------------------------------
         output[val]=raw2DPoints[(route->routingRules[i]*3) + 2]; 
         fprintf(stderr,"2DVisibility=%0.2f \n",output[val]);
         val+=1;
         //--------------------------------------------------------------------------
       }
     return 1;
   }
  }
 return 0;
}



#ifdef __cplusplus
}
#endif




#endif
