/** @file NSxM.h
 *  @brief A Description the upperbody input of a Tensorflow network required for MocapNET
 *  @author Ammar Qammaz (AmmarkoV)
 *  Automatically generated @ 2021-01-31 23:41:28.767309 using :
 *     python3 exportCPPCodeFromJSONConfiguration.py --front upperbody --config dataset/upperbody_configuration.json
 *     please note that since the names of the labels are both affected by the dataset/upperbody_configuration.json configuration
 *     as well as the ground truth, if you have made any weird additions you might consider running the ./createRandomizedDataset.sh and ./createTestDataset.sh scripts
 */

#pragma once

#include <iostream>
#include <math.h>
#include <vector>
#include <string.h>

#include "../JSON/readListFile.h"
#include "../JSON/readModelConfiguration.h"

#include "../PCA/PCA.h"

#include "../IO/inputRouting.h"

#include "EDM.h"
#include "NSDM.h"
#include "NSRM.h"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black  */
#define RED     "\033[31m"      /* Red    */
#define GREEN   "\033[32m"      /* Green  */
#define YELLOW  "\033[33m"      /* Yellow */

struct label
{
  char * str;
  unsigned int length;
};

struct descriptor
{
 float * routedInput;
 int routedInputLength; 
 unsigned int numberOfElements;
 unsigned int maxNumberOfElements;
 struct label * labels;
 float * values;
};


static int destroyDescriptor( 
                             struct descriptor * output
                            )
{
 //-----------------------------------------------
 if (output->values!=0) 
    { free(output->values); }
 //-----------------------------------------------
 for (int i=0; i<output->maxNumberOfElements; i++)
  {
    if (output->labels[i].str!=0)
       {
         free(output->labels[i].str);
         output->labels[i].str = 0;
         output->labels[i].length = 0;
       }
  }
 //-----------------------------------------------
  return 1;
}




static int createDescriptor( 
                             struct descriptor * output,
                             float * data2DRaw,
                             unsigned int data2DRawLength,
                             struct inputRouting * route,
                             struct ModelConfigurationData * config,
                             struct PCAData * pca,
                             struct listFileData * listInputJoints,
                             struct listFileData * listOutput
                           )
{
  fprintf(stderr,GREEN "createDescriptor..\n" NORMAL);
  if (output==0) { return 0; }
   //---------------------------------------------------------
   int useEDM             = config->EDM;   
   int useNSRM            = config->eNSRM;
   int alignPoints        = config->NSDMAlsoUseAlignmentAngles;
   int usePCADimensions   = config->PCADimensionsKept;
   int numberOfJointRules = config->numberOfDescriptorElements;
   //---------------------------------------------------------

 
  if (output->routedInput==0)
  {
     output->routedInput       = (float *) malloc(sizeof(float) * route->numberOfRoutingRules * 3);
     output->routedInputLength = route->numberOfRoutingRules * 3;
  }
   
   //--------------------------------------
   if (
       !routeInput(
                    output->routedInput,
                    &output->routedInputLength,
                    config,
                    route,
                    data2DRaw,
                    data2DRawLength
                   ) 
      )
      {
        fprintf(stderr,RED "Unable to route 2D input..\n" NORMAL);
        return 0;
      }
   //--------------------------------------
   float * data2D            = output->routedInput;
   unsigned int data2DLength = output->routedInputLength;
   //--------------------------------------
                             
   if (listInputJoints->numberOfEntries!=data2DLength)
   {
       fprintf(stderr,RED "Mismatch of Input 2D Points Vs Neural Network 2D Joints..\n" NORMAL);
   }
                                
  for (int i=0; i<data2DLength; i++)
  {
      fprintf(stderr,"%0.2f ",data2D[i]);
  }
  fprintf(stderr,"\n\n");


  int neededSpace = data2DLength; //We will append 2D Data (x,y,v)
  fprintf(stderr,"2D Points : %u \n",neededSpace);
  neededSpace += useEDM  * countEDMElements(numberOfJointRules);   // If EDM  is set to 0 this is 0
  fprintf(stderr,"2D Points + EDM : %u \n",neededSpace);
  neededSpace += useNSRM * countNSRMElements(numberOfJointRules);  // If NSRM is set to 0 this is 0
  fprintf(stderr,"2D Points + NSRM : %u \n",neededSpace);

  //---------------------------------------------------------
  if (neededSpace>output->maxNumberOfElements)
    {
     //Space already allocated but not enough,
     //Destroy anything previously allocated
     destroyDescriptor(output);
    }
  //---------------------------------------------------------
  if (output->maxNumberOfElements==0)
    { //If operating on a newly allocated descriptor
      output->maxNumberOfElements = neededSpace;
      //---------------------------------------------------------
      output->values = (float*) malloc( sizeof(float) * output->maxNumberOfElements );
      if (output->values!=0)
        { memset(output->values,0,sizeof(float) * output->maxNumberOfElements); }
      //--------------------------------------------------------- 
      output->labels = (struct label *) malloc( sizeof(struct label) * output->maxNumberOfElements );
      if (output->labels!=0)
        { memset(output->labels,0,sizeof(struct label) * output->maxNumberOfElements); }
      //We have a clean output descriptor 
    } 
  //---------------------------------------------------------
    
    
   float * outputInPosition = output->values;
    
   //Copy all 2D Data this must happen before the alignment!.. 
   fprintf(stderr,"Copying %u 2D coordinates \n",data2DLength);
   for (int i=0; i<data2DLength; i++)
    {
        *outputInPosition = data2D[i];
        ++outputInPosition;
    }
    
    
  float angleToRotate = 0.0;
  if (config->eNSRM)//alignPoints)
   {
     fprintf(stderr,"Aligning Points\n");
     //Do point alignment here..
     angleToRotate = performNSRMAlignment(data2D,data2DLength,config);
     fprintf(stderr,YELLOW "Correcting skeleton by rotating it %0.2f degrees\n" NORMAL,angleToRotate);
   }
      
   /*
   fprintf(stderr,"  2d = list()\n");
   for (int i=0; i<data2DLength; i++)
      {
        fprintf(stderr,"  2d.append(%f) #%u\n",output->values[i],i); 
      }*/
    
    
    if (useEDM)
    {
      fprintf(stderr,"Using EDM\n");
      //---------------------------------------------------------
       int EDMElements = appendEDMElements(
                                           data2D,
                                           data2DLength,
                                           outputInPosition,
                                           config
                                          );
        outputInPosition += EDMElements;
      //---------------------------------------------------------
      /*
     fprintf(stderr,"  EDM = list()\n");
     for (int i=0; i<neededSpace; i++)
      {
        fprintf(stderr,"  EDM.append(%f) #%u\n",output->values[i],i); 
      }*/ 
    }
      
      
    if (useNSRM)
    {
      fprintf(stderr,"Using NSRM\n");
      //---------------------------------------------------------
       int NSRMElements = appendNSRMElements(
                                             data2D,
                                             data2DLength,
                                             outputInPosition,
                                             config,
                                             angleToRotate
                                            );
        outputInPosition += NSRMElements;
      //---------------------------------------------------------
    /*
     fprintf(stderr,"  NSRM = list()\n");
     for (int i=0; i<neededSpace; i++)
      {
        fprintf(stderr,"  NSRM.append(%f) #%u\n",output->values[i],i); 
      }*/
    }
    output->values[169] += angleToRotate;
     
    fprintf(stderr,"Descriptor yielded %u elements : ",neededSpace);
    output->numberOfElements = neededSpace;
    for (int i=0; i<neededSpace; i++)
      { 
         if (output->values[i]>10.0) { fprintf(stderr,RED); }
         fprintf(stderr,"%0.2f(#%u) ",output->values[i],i); 
         if (output->values[i]>10.0) { fprintf(stderr,NORMAL); }
      }
    fprintf(stderr,"\n"); 

    if (config->doPCA)
    {
     /*
     fprintf(stderr,"  PCA = list()\n");
     for (int i=0; i<neededSpace; i++)
      {
        fprintf(stderr,"  PCA.append(%f) #%u\n",output->values[i],i); 
      }*/
     int finalOutputSize = config->PCADimensionsKept;
     doPCATransform(
                     output->values,
                     &finalOutputSize,
                     pca,
                     output->values,
                     neededSpace,
                     config->PCADimensionsKept
                   );
    
     fprintf(stderr,"PCA (mean %0.2f/std %0.2f) packed descriptor yielded %u elements : \n",pca->mean,pca->std,finalOutputSize);
     for (int i=0; i<finalOutputSize; i++)
     {
        fprintf(stderr,"%u = %f \n",i,output->values[i]);
     }
     output->numberOfElements = finalOutputSize;
    }

   return 1;
}

/** @brief This function returns the euclidean distance between two input 2D joints and zero if either of them is invalid*/
static float getJoint2DDistanceNSxM(float* in,int jointA,int jointB)
{
    float aX=in[jointA*3+0];
    float aY=in[jointA*3+1];
    float bX=in[jointB*3+0];
    float bY=in[jointB*3+1];
    if ( ((aX==0) && (aY==0)) || ((bX==0) && (bY==0)) ) {
        return 0.0;
    }


    float xDistance=(float) bX-aX;
    float yDistance=(float) bY-aY;
    return sqrt( (xDistance*xDistance) + (yDistance*yDistance) );
}




/** @brief This is an array of names for all uncompressed 2D inputs expected. */
static const unsigned int mocapNET_InputLength_WithoutNSDM_upperbody = 33;

/** @brief Use rich diagonal, part of networks after 31-01-2021 */
static const unsigned int richDiagonal_upperbody = 1;

/** @brief An array of strings that contains the label for each expected input. */
static const char * mocapNET_upperbody[] =
{
    "2DX_hip", //0
    "2DY_hip", //1
    "visible_hip", //2
    "2DX_neck", //3
    "2DY_neck", //4
    "visible_neck", //5
    "2DX_head", //6
    "2DY_head", //7
    "visible_head", //8
    "2DX_EndSite_eye.l", //9
    "2DY_EndSite_eye.l", //10
    "visible_EndSite_eye.l", //11
    "2DX_EndSite_eye.r", //12
    "2DY_EndSite_eye.r", //13
    "visible_EndSite_eye.r", //14
    "2DX_rshoulder", //15
    "2DY_rshoulder", //16
    "visible_rshoulder", //17
    "2DX_relbow", //18
    "2DY_relbow", //19
    "visible_relbow", //20
    "2DX_rhand", //21
    "2DY_rhand", //22
    "visible_rhand", //23
    "2DX_lshoulder", //24
    "2DY_lshoulder", //25
    "visible_lshoulder", //26
    "2DX_lelbow", //27
    "2DY_lelbow", //28
    "visible_lelbow", //29
    "2DX_lhand", //30
    "2DY_lhand", //31
    "visible_lhand", //32
//This is where regular input ends and the NSDM data kicks in..
    "angleUsedFor2DRotation_0", //33
    "hipY-EndSite_eye.rY-Angle", //34
    "hipY-EndSite_eye.lY-Angle", //35
    "hipY-neckY-Angle", //36
    "hipY-rshoulderY-Angle", //37
    "hipY-halfway_rshoulder_and_relbowY-Angle", //38
    "hipY-relbowY-Angle", //39
    "hipY-halfway_relbow_and_rhandY-Angle", //40
    "hipY-rhandY-Angle", //41
    "hipY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //42
    "hipY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //43
    "hipY-lshoulderY-Angle", //44
    "hipY-halfway_lshoulder_and_lelbowY-Angle", //45
    "hipY-lelbowY-Angle", //46
    "hipY-halfway_lelbow_and_lhandY-Angle", //47
    "hipY-lhandY-Angle", //48
    "hipY-halfway_neck_and_hipY-Angle", //49
    "EndSite_eye.rY-hipY-Angle", //50
    "angleUsedFor2DRotation_1", //51
    "EndSite_eye.rY-EndSite_eye.lY-Angle", //52
    "EndSite_eye.rY-neckY-Angle", //53
    "EndSite_eye.rY-rshoulderY-Angle", //54
    "EndSite_eye.rY-halfway_rshoulder_and_relbowY-Angle", //55
    "EndSite_eye.rY-relbowY-Angle", //56
    "EndSite_eye.rY-halfway_relbow_and_rhandY-Angle", //57
    "EndSite_eye.rY-rhandY-Angle", //58
    "EndSite_eye.rY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //59
    "EndSite_eye.rY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //60
    "EndSite_eye.rY-lshoulderY-Angle", //61
    "EndSite_eye.rY-halfway_lshoulder_and_lelbowY-Angle", //62
    "EndSite_eye.rY-lelbowY-Angle", //63
    "EndSite_eye.rY-halfway_lelbow_and_lhandY-Angle", //64
    "EndSite_eye.rY-lhandY-Angle", //65
    "EndSite_eye.rY-halfway_neck_and_hipY-Angle", //66
    "EndSite_eye.lY-hipY-Angle", //67
    "EndSite_eye.lY-EndSite_eye.rY-Angle", //68
    "angleUsedFor2DRotation_2", //69
    "EndSite_eye.lY-neckY-Angle", //70
    "EndSite_eye.lY-rshoulderY-Angle", //71
    "EndSite_eye.lY-halfway_rshoulder_and_relbowY-Angle", //72
    "EndSite_eye.lY-relbowY-Angle", //73
    "EndSite_eye.lY-halfway_relbow_and_rhandY-Angle", //74
    "EndSite_eye.lY-rhandY-Angle", //75
    "EndSite_eye.lY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //76
    "EndSite_eye.lY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //77
    "EndSite_eye.lY-lshoulderY-Angle", //78
    "EndSite_eye.lY-halfway_lshoulder_and_lelbowY-Angle", //79
    "EndSite_eye.lY-lelbowY-Angle", //80
    "EndSite_eye.lY-halfway_lelbow_and_lhandY-Angle", //81
    "EndSite_eye.lY-lhandY-Angle", //82
    "EndSite_eye.lY-halfway_neck_and_hipY-Angle", //83
    "neckY-hipY-Angle", //84
    "neckY-EndSite_eye.rY-Angle", //85
    "neckY-EndSite_eye.lY-Angle", //86
    "angleUsedFor2DRotation_3", //87
    "neckY-rshoulderY-Angle", //88
    "neckY-halfway_rshoulder_and_relbowY-Angle", //89
    "neckY-relbowY-Angle", //90
    "neckY-halfway_relbow_and_rhandY-Angle", //91
    "neckY-rhandY-Angle", //92
    "neckY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //93
    "neckY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //94
    "neckY-lshoulderY-Angle", //95
    "neckY-halfway_lshoulder_and_lelbowY-Angle", //96
    "neckY-lelbowY-Angle", //97
    "neckY-halfway_lelbow_and_lhandY-Angle", //98
    "neckY-lhandY-Angle", //99
    "neckY-halfway_neck_and_hipY-Angle", //100
    "rshoulderY-hipY-Angle", //101
    "rshoulderY-EndSite_eye.rY-Angle", //102
    "rshoulderY-EndSite_eye.lY-Angle", //103
    "rshoulderY-neckY-Angle", //104
    "angleUsedFor2DRotation_4", //105
    "rshoulderY-halfway_rshoulder_and_relbowY-Angle", //106
    "rshoulderY-relbowY-Angle", //107
    "rshoulderY-halfway_relbow_and_rhandY-Angle", //108
    "rshoulderY-rhandY-Angle", //109
    "rshoulderY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //110
    "rshoulderY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //111
    "rshoulderY-lshoulderY-Angle", //112
    "rshoulderY-halfway_lshoulder_and_lelbowY-Angle", //113
    "rshoulderY-lelbowY-Angle", //114
    "rshoulderY-halfway_lelbow_and_lhandY-Angle", //115
    "rshoulderY-lhandY-Angle", //116
    "rshoulderY-halfway_neck_and_hipY-Angle", //117
    "halfway_rshoulder_and_relbowY-hipY-Angle", //118
    "halfway_rshoulder_and_relbowY-EndSite_eye.rY-Angle", //119
    "halfway_rshoulder_and_relbowY-EndSite_eye.lY-Angle", //120
    "halfway_rshoulder_and_relbowY-neckY-Angle", //121
    "halfway_rshoulder_and_relbowY-rshoulderY-Angle", //122
    "angleUsedFor2DRotation_5", //123
    "halfway_rshoulder_and_relbowY-relbowY-Angle", //124
    "halfway_rshoulder_and_relbowY-halfway_relbow_and_rhandY-Angle", //125
    "halfway_rshoulder_and_relbowY-rhandY-Angle", //126
    "halfway_rshoulder_and_relbowY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //127
    "halfway_rshoulder_and_relbowY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //128
    "halfway_rshoulder_and_relbowY-lshoulderY-Angle", //129
    "halfway_rshoulder_and_relbowY-halfway_lshoulder_and_lelbowY-Angle", //130
    "halfway_rshoulder_and_relbowY-lelbowY-Angle", //131
    "halfway_rshoulder_and_relbowY-halfway_lelbow_and_lhandY-Angle", //132
    "halfway_rshoulder_and_relbowY-lhandY-Angle", //133
    "halfway_rshoulder_and_relbowY-halfway_neck_and_hipY-Angle", //134
    "relbowY-hipY-Angle", //135
    "relbowY-EndSite_eye.rY-Angle", //136
    "relbowY-EndSite_eye.lY-Angle", //137
    "relbowY-neckY-Angle", //138
    "relbowY-rshoulderY-Angle", //139
    "relbowY-halfway_rshoulder_and_relbowY-Angle", //140
    "angleUsedFor2DRotation_6", //141
    "relbowY-halfway_relbow_and_rhandY-Angle", //142
    "relbowY-rhandY-Angle", //143
    "relbowY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //144
    "relbowY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //145
    "relbowY-lshoulderY-Angle", //146
    "relbowY-halfway_lshoulder_and_lelbowY-Angle", //147
    "relbowY-lelbowY-Angle", //148
    "relbowY-halfway_lelbow_and_lhandY-Angle", //149
    "relbowY-lhandY-Angle", //150
    "relbowY-halfway_neck_and_hipY-Angle", //151
    "halfway_relbow_and_rhandY-hipY-Angle", //152
    "halfway_relbow_and_rhandY-EndSite_eye.rY-Angle", //153
    "halfway_relbow_and_rhandY-EndSite_eye.lY-Angle", //154
    "halfway_relbow_and_rhandY-neckY-Angle", //155
    "halfway_relbow_and_rhandY-rshoulderY-Angle", //156
    "halfway_relbow_and_rhandY-halfway_rshoulder_and_relbowY-Angle", //157
    "halfway_relbow_and_rhandY-relbowY-Angle", //158
    "angleUsedFor2DRotation_7", //159
    "halfway_relbow_and_rhandY-rhandY-Angle", //160
    "halfway_relbow_and_rhandY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //161
    "halfway_relbow_and_rhandY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //162
    "halfway_relbow_and_rhandY-lshoulderY-Angle", //163
    "halfway_relbow_and_rhandY-halfway_lshoulder_and_lelbowY-Angle", //164
    "halfway_relbow_and_rhandY-lelbowY-Angle", //165
    "halfway_relbow_and_rhandY-halfway_lelbow_and_lhandY-Angle", //166
    "halfway_relbow_and_rhandY-lhandY-Angle", //167
    "halfway_relbow_and_rhandY-halfway_neck_and_hipY-Angle", //168
    "rhandY-hipY-Angle", //169
    "rhandY-EndSite_eye.rY-Angle", //170
    "rhandY-EndSite_eye.lY-Angle", //171
    "rhandY-neckY-Angle", //172
    "rhandY-rshoulderY-Angle", //173
    "rhandY-halfway_rshoulder_and_relbowY-Angle", //174
    "rhandY-relbowY-Angle", //175
    "rhandY-halfway_relbow_and_rhandY-Angle", //176
    "angleUsedFor2DRotation_8", //177
    "rhandY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //178
    "rhandY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //179
    "rhandY-lshoulderY-Angle", //180
    "rhandY-halfway_lshoulder_and_lelbowY-Angle", //181
    "rhandY-lelbowY-Angle", //182
    "rhandY-halfway_lelbow_and_lhandY-Angle", //183
    "rhandY-lhandY-Angle", //184
    "rhandY-halfway_neck_and_hipY-Angle", //185
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-hipY-Angle", //186
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-EndSite_eye.rY-Angle", //187
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-EndSite_eye.lY-Angle", //188
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-neckY-Angle", //189
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-rshoulderY-Angle", //190
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-halfway_rshoulder_and_relbowY-Angle", //191
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-relbowY-Angle", //192
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-halfway_relbow_and_rhandY-Angle", //193
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-rhandY-Angle", //194
    "angleUsedFor2DRotation_9", //195
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //196
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-lshoulderY-Angle", //197
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-halfway_lshoulder_and_lelbowY-Angle", //198
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-lelbowY-Angle", //199
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-halfway_lelbow_and_lhandY-Angle", //200
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-lhandY-Angle", //201
    "virtual_hip_x_minus_0_15_y_minus_0_15Y-halfway_neck_and_hipY-Angle", //202
    "virtual_hip_x_plus0_15_y_minus_0_15Y-hipY-Angle", //203
    "virtual_hip_x_plus0_15_y_minus_0_15Y-EndSite_eye.rY-Angle", //204
    "virtual_hip_x_plus0_15_y_minus_0_15Y-EndSite_eye.lY-Angle", //205
    "virtual_hip_x_plus0_15_y_minus_0_15Y-neckY-Angle", //206
    "virtual_hip_x_plus0_15_y_minus_0_15Y-rshoulderY-Angle", //207
    "virtual_hip_x_plus0_15_y_minus_0_15Y-halfway_rshoulder_and_relbowY-Angle", //208
    "virtual_hip_x_plus0_15_y_minus_0_15Y-relbowY-Angle", //209
    "virtual_hip_x_plus0_15_y_minus_0_15Y-halfway_relbow_and_rhandY-Angle", //210
    "virtual_hip_x_plus0_15_y_minus_0_15Y-rhandY-Angle", //211
    "virtual_hip_x_plus0_15_y_minus_0_15Y-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //212
    "angleUsedFor2DRotation_10", //213
    "virtual_hip_x_plus0_15_y_minus_0_15Y-lshoulderY-Angle", //214
    "virtual_hip_x_plus0_15_y_minus_0_15Y-halfway_lshoulder_and_lelbowY-Angle", //215
    "virtual_hip_x_plus0_15_y_minus_0_15Y-lelbowY-Angle", //216
    "virtual_hip_x_plus0_15_y_minus_0_15Y-halfway_lelbow_and_lhandY-Angle", //217
    "virtual_hip_x_plus0_15_y_minus_0_15Y-lhandY-Angle", //218
    "virtual_hip_x_plus0_15_y_minus_0_15Y-halfway_neck_and_hipY-Angle", //219
    "lshoulderY-hipY-Angle", //220
    "lshoulderY-EndSite_eye.rY-Angle", //221
    "lshoulderY-EndSite_eye.lY-Angle", //222
    "lshoulderY-neckY-Angle", //223
    "lshoulderY-rshoulderY-Angle", //224
    "lshoulderY-halfway_rshoulder_and_relbowY-Angle", //225
    "lshoulderY-relbowY-Angle", //226
    "lshoulderY-halfway_relbow_and_rhandY-Angle", //227
    "lshoulderY-rhandY-Angle", //228
    "lshoulderY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //229
    "lshoulderY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //230
    "angleUsedFor2DRotation_11", //231
    "lshoulderY-halfway_lshoulder_and_lelbowY-Angle", //232
    "lshoulderY-lelbowY-Angle", //233
    "lshoulderY-halfway_lelbow_and_lhandY-Angle", //234
    "lshoulderY-lhandY-Angle", //235
    "lshoulderY-halfway_neck_and_hipY-Angle", //236
    "halfway_lshoulder_and_lelbowY-hipY-Angle", //237
    "halfway_lshoulder_and_lelbowY-EndSite_eye.rY-Angle", //238
    "halfway_lshoulder_and_lelbowY-EndSite_eye.lY-Angle", //239
    "halfway_lshoulder_and_lelbowY-neckY-Angle", //240
    "halfway_lshoulder_and_lelbowY-rshoulderY-Angle", //241
    "halfway_lshoulder_and_lelbowY-halfway_rshoulder_and_relbowY-Angle", //242
    "halfway_lshoulder_and_lelbowY-relbowY-Angle", //243
    "halfway_lshoulder_and_lelbowY-halfway_relbow_and_rhandY-Angle", //244
    "halfway_lshoulder_and_lelbowY-rhandY-Angle", //245
    "halfway_lshoulder_and_lelbowY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //246
    "halfway_lshoulder_and_lelbowY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //247
    "halfway_lshoulder_and_lelbowY-lshoulderY-Angle", //248
    "angleUsedFor2DRotation_12", //249
    "halfway_lshoulder_and_lelbowY-lelbowY-Angle", //250
    "halfway_lshoulder_and_lelbowY-halfway_lelbow_and_lhandY-Angle", //251
    "halfway_lshoulder_and_lelbowY-lhandY-Angle", //252
    "halfway_lshoulder_and_lelbowY-halfway_neck_and_hipY-Angle", //253
    "lelbowY-hipY-Angle", //254
    "lelbowY-EndSite_eye.rY-Angle", //255
    "lelbowY-EndSite_eye.lY-Angle", //256
    "lelbowY-neckY-Angle", //257
    "lelbowY-rshoulderY-Angle", //258
    "lelbowY-halfway_rshoulder_and_relbowY-Angle", //259
    "lelbowY-relbowY-Angle", //260
    "lelbowY-halfway_relbow_and_rhandY-Angle", //261
    "lelbowY-rhandY-Angle", //262
    "lelbowY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //263
    "lelbowY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //264
    "lelbowY-lshoulderY-Angle", //265
    "lelbowY-halfway_lshoulder_and_lelbowY-Angle", //266
    "angleUsedFor2DRotation_13", //267
    "lelbowY-halfway_lelbow_and_lhandY-Angle", //268
    "lelbowY-lhandY-Angle", //269
    "lelbowY-halfway_neck_and_hipY-Angle", //270
    "halfway_lelbow_and_lhandY-hipY-Angle", //271
    "halfway_lelbow_and_lhandY-EndSite_eye.rY-Angle", //272
    "halfway_lelbow_and_lhandY-EndSite_eye.lY-Angle", //273
    "halfway_lelbow_and_lhandY-neckY-Angle", //274
    "halfway_lelbow_and_lhandY-rshoulderY-Angle", //275
    "halfway_lelbow_and_lhandY-halfway_rshoulder_and_relbowY-Angle", //276
    "halfway_lelbow_and_lhandY-relbowY-Angle", //277
    "halfway_lelbow_and_lhandY-halfway_relbow_and_rhandY-Angle", //278
    "halfway_lelbow_and_lhandY-rhandY-Angle", //279
    "halfway_lelbow_and_lhandY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //280
    "halfway_lelbow_and_lhandY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //281
    "halfway_lelbow_and_lhandY-lshoulderY-Angle", //282
    "halfway_lelbow_and_lhandY-halfway_lshoulder_and_lelbowY-Angle", //283
    "halfway_lelbow_and_lhandY-lelbowY-Angle", //284
    "angleUsedFor2DRotation_14", //285
    "halfway_lelbow_and_lhandY-lhandY-Angle", //286
    "halfway_lelbow_and_lhandY-halfway_neck_and_hipY-Angle", //287
    "lhandY-hipY-Angle", //288
    "lhandY-EndSite_eye.rY-Angle", //289
    "lhandY-EndSite_eye.lY-Angle", //290
    "lhandY-neckY-Angle", //291
    "lhandY-rshoulderY-Angle", //292
    "lhandY-halfway_rshoulder_and_relbowY-Angle", //293
    "lhandY-relbowY-Angle", //294
    "lhandY-halfway_relbow_and_rhandY-Angle", //295
    "lhandY-rhandY-Angle", //296
    "lhandY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //297
    "lhandY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //298
    "lhandY-lshoulderY-Angle", //299
    "lhandY-halfway_lshoulder_and_lelbowY-Angle", //300
    "lhandY-lelbowY-Angle", //301
    "lhandY-halfway_lelbow_and_lhandY-Angle", //302
    "angleUsedFor2DRotation_15", //303
    "lhandY-halfway_neck_and_hipY-Angle", //304
    "halfway_neck_and_hipY-hipY-Angle", //305
    "halfway_neck_and_hipY-EndSite_eye.rY-Angle", //306
    "halfway_neck_and_hipY-EndSite_eye.lY-Angle", //307
    "halfway_neck_and_hipY-neckY-Angle", //308
    "halfway_neck_and_hipY-rshoulderY-Angle", //309
    "halfway_neck_and_hipY-halfway_rshoulder_and_relbowY-Angle", //310
    "halfway_neck_and_hipY-relbowY-Angle", //311
    "halfway_neck_and_hipY-halfway_relbow_and_rhandY-Angle", //312
    "halfway_neck_and_hipY-rhandY-Angle", //313
    "halfway_neck_and_hipY-virtual_hip_x_minus_0_15_y_minus_0_15Y-Angle", //314
    "halfway_neck_and_hipY-virtual_hip_x_plus0_15_y_minus_0_15Y-Angle", //315
    "halfway_neck_and_hipY-lshoulderY-Angle", //316
    "halfway_neck_and_hipY-halfway_lshoulder_and_lelbowY-Angle", //317
    "halfway_neck_and_hipY-lelbowY-Angle", //318
    "halfway_neck_and_hipY-halfway_lelbow_and_lhandY-Angle", //319
    "halfway_neck_and_hipY-lhandY-Angle", //320
    "angleUsedFor2DRotation_16", //321
    "end"
};
/** @brief Programmer friendly enumerator of expected inputs*/
enum mocapNET_upperbody_enum
{
    MNET_UPPERBODY_IN_2DX_HIP = 0, //0
    MNET_UPPERBODY_IN_2DY_HIP, //1
    MNET_UPPERBODY_IN_VISIBLE_HIP, //2
    MNET_UPPERBODY_IN_2DX_NECK, //3
    MNET_UPPERBODY_IN_2DY_NECK, //4
    MNET_UPPERBODY_IN_VISIBLE_NECK, //5
    MNET_UPPERBODY_IN_2DX_HEAD, //6
    MNET_UPPERBODY_IN_2DY_HEAD, //7
    MNET_UPPERBODY_IN_VISIBLE_HEAD, //8
    MNET_UPPERBODY_IN_2DX_ENDSITE_EYE_L, //9
    MNET_UPPERBODY_IN_2DY_ENDSITE_EYE_L, //10
    MNET_UPPERBODY_IN_VISIBLE_ENDSITE_EYE_L, //11
    MNET_UPPERBODY_IN_2DX_ENDSITE_EYE_R, //12
    MNET_UPPERBODY_IN_2DY_ENDSITE_EYE_R, //13
    MNET_UPPERBODY_IN_VISIBLE_ENDSITE_EYE_R, //14
    MNET_UPPERBODY_IN_2DX_RSHOULDER, //15
    MNET_UPPERBODY_IN_2DY_RSHOULDER, //16
    MNET_UPPERBODY_IN_VISIBLE_RSHOULDER, //17
    MNET_UPPERBODY_IN_2DX_RELBOW, //18
    MNET_UPPERBODY_IN_2DY_RELBOW, //19
    MNET_UPPERBODY_IN_VISIBLE_RELBOW, //20
    MNET_UPPERBODY_IN_2DX_RHAND, //21
    MNET_UPPERBODY_IN_2DY_RHAND, //22
    MNET_UPPERBODY_IN_VISIBLE_RHAND, //23
    MNET_UPPERBODY_IN_2DX_LSHOULDER, //24
    MNET_UPPERBODY_IN_2DY_LSHOULDER, //25
    MNET_UPPERBODY_IN_VISIBLE_LSHOULDER, //26
    MNET_UPPERBODY_IN_2DX_LELBOW, //27
    MNET_UPPERBODY_IN_2DY_LELBOW, //28
    MNET_UPPERBODY_IN_VISIBLE_LELBOW, //29
    MNET_UPPERBODY_IN_2DX_LHAND, //30
    MNET_UPPERBODY_IN_2DY_LHAND, //31
    MNET_UPPERBODY_IN_VISIBLE_LHAND, //32
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_0, //33
    MNET_UPPERBODY_IN_HIPY_ENDSITE_EYE_RY_ANGLE, //34
    MNET_UPPERBODY_IN_HIPY_ENDSITE_EYE_LY_ANGLE, //35
    MNET_UPPERBODY_IN_HIPY_NECKY_ANGLE, //36
    MNET_UPPERBODY_IN_HIPY_RSHOULDERY_ANGLE, //37
    MNET_UPPERBODY_IN_HIPY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //38
    MNET_UPPERBODY_IN_HIPY_RELBOWY_ANGLE, //39
    MNET_UPPERBODY_IN_HIPY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //40
    MNET_UPPERBODY_IN_HIPY_RHANDY_ANGLE, //41
    MNET_UPPERBODY_IN_HIPY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //42
    MNET_UPPERBODY_IN_HIPY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //43
    MNET_UPPERBODY_IN_HIPY_LSHOULDERY_ANGLE, //44
    MNET_UPPERBODY_IN_HIPY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //45
    MNET_UPPERBODY_IN_HIPY_LELBOWY_ANGLE, //46
    MNET_UPPERBODY_IN_HIPY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //47
    MNET_UPPERBODY_IN_HIPY_LHANDY_ANGLE, //48
    MNET_UPPERBODY_IN_HIPY_HALFWAY_NECK_AND_HIPY_ANGLE, //49
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_HIPY_ANGLE, //50
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_1, //51
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_ENDSITE_EYE_LY_ANGLE, //52
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_NECKY_ANGLE, //53
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_RSHOULDERY_ANGLE, //54
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //55
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_RELBOWY_ANGLE, //56
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //57
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_RHANDY_ANGLE, //58
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //59
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //60
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_LSHOULDERY_ANGLE, //61
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //62
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_LELBOWY_ANGLE, //63
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //64
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_LHANDY_ANGLE, //65
    MNET_UPPERBODY_IN_ENDSITE_EYE_RY_HALFWAY_NECK_AND_HIPY_ANGLE, //66
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_HIPY_ANGLE, //67
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_ENDSITE_EYE_RY_ANGLE, //68
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_2, //69
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_NECKY_ANGLE, //70
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_RSHOULDERY_ANGLE, //71
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //72
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_RELBOWY_ANGLE, //73
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //74
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_RHANDY_ANGLE, //75
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //76
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //77
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_LSHOULDERY_ANGLE, //78
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //79
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_LELBOWY_ANGLE, //80
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //81
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_LHANDY_ANGLE, //82
    MNET_UPPERBODY_IN_ENDSITE_EYE_LY_HALFWAY_NECK_AND_HIPY_ANGLE, //83
    MNET_UPPERBODY_IN_NECKY_HIPY_ANGLE, //84
    MNET_UPPERBODY_IN_NECKY_ENDSITE_EYE_RY_ANGLE, //85
    MNET_UPPERBODY_IN_NECKY_ENDSITE_EYE_LY_ANGLE, //86
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_3, //87
    MNET_UPPERBODY_IN_NECKY_RSHOULDERY_ANGLE, //88
    MNET_UPPERBODY_IN_NECKY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //89
    MNET_UPPERBODY_IN_NECKY_RELBOWY_ANGLE, //90
    MNET_UPPERBODY_IN_NECKY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //91
    MNET_UPPERBODY_IN_NECKY_RHANDY_ANGLE, //92
    MNET_UPPERBODY_IN_NECKY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //93
    MNET_UPPERBODY_IN_NECKY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //94
    MNET_UPPERBODY_IN_NECKY_LSHOULDERY_ANGLE, //95
    MNET_UPPERBODY_IN_NECKY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //96
    MNET_UPPERBODY_IN_NECKY_LELBOWY_ANGLE, //97
    MNET_UPPERBODY_IN_NECKY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //98
    MNET_UPPERBODY_IN_NECKY_LHANDY_ANGLE, //99
    MNET_UPPERBODY_IN_NECKY_HALFWAY_NECK_AND_HIPY_ANGLE, //100
    MNET_UPPERBODY_IN_RSHOULDERY_HIPY_ANGLE, //101
    MNET_UPPERBODY_IN_RSHOULDERY_ENDSITE_EYE_RY_ANGLE, //102
    MNET_UPPERBODY_IN_RSHOULDERY_ENDSITE_EYE_LY_ANGLE, //103
    MNET_UPPERBODY_IN_RSHOULDERY_NECKY_ANGLE, //104
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_4, //105
    MNET_UPPERBODY_IN_RSHOULDERY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //106
    MNET_UPPERBODY_IN_RSHOULDERY_RELBOWY_ANGLE, //107
    MNET_UPPERBODY_IN_RSHOULDERY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //108
    MNET_UPPERBODY_IN_RSHOULDERY_RHANDY_ANGLE, //109
    MNET_UPPERBODY_IN_RSHOULDERY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //110
    MNET_UPPERBODY_IN_RSHOULDERY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //111
    MNET_UPPERBODY_IN_RSHOULDERY_LSHOULDERY_ANGLE, //112
    MNET_UPPERBODY_IN_RSHOULDERY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //113
    MNET_UPPERBODY_IN_RSHOULDERY_LELBOWY_ANGLE, //114
    MNET_UPPERBODY_IN_RSHOULDERY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //115
    MNET_UPPERBODY_IN_RSHOULDERY_LHANDY_ANGLE, //116
    MNET_UPPERBODY_IN_RSHOULDERY_HALFWAY_NECK_AND_HIPY_ANGLE, //117
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_HIPY_ANGLE, //118
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_ENDSITE_EYE_RY_ANGLE, //119
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_ENDSITE_EYE_LY_ANGLE, //120
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_NECKY_ANGLE, //121
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_RSHOULDERY_ANGLE, //122
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_5, //123
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_RELBOWY_ANGLE, //124
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //125
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_RHANDY_ANGLE, //126
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //127
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //128
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_LSHOULDERY_ANGLE, //129
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //130
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_LELBOWY_ANGLE, //131
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //132
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_LHANDY_ANGLE, //133
    MNET_UPPERBODY_IN_HALFWAY_RSHOULDER_AND_RELBOWY_HALFWAY_NECK_AND_HIPY_ANGLE, //134
    MNET_UPPERBODY_IN_RELBOWY_HIPY_ANGLE, //135
    MNET_UPPERBODY_IN_RELBOWY_ENDSITE_EYE_RY_ANGLE, //136
    MNET_UPPERBODY_IN_RELBOWY_ENDSITE_EYE_LY_ANGLE, //137
    MNET_UPPERBODY_IN_RELBOWY_NECKY_ANGLE, //138
    MNET_UPPERBODY_IN_RELBOWY_RSHOULDERY_ANGLE, //139
    MNET_UPPERBODY_IN_RELBOWY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //140
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_6, //141
    MNET_UPPERBODY_IN_RELBOWY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //142
    MNET_UPPERBODY_IN_RELBOWY_RHANDY_ANGLE, //143
    MNET_UPPERBODY_IN_RELBOWY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //144
    MNET_UPPERBODY_IN_RELBOWY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //145
    MNET_UPPERBODY_IN_RELBOWY_LSHOULDERY_ANGLE, //146
    MNET_UPPERBODY_IN_RELBOWY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //147
    MNET_UPPERBODY_IN_RELBOWY_LELBOWY_ANGLE, //148
    MNET_UPPERBODY_IN_RELBOWY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //149
    MNET_UPPERBODY_IN_RELBOWY_LHANDY_ANGLE, //150
    MNET_UPPERBODY_IN_RELBOWY_HALFWAY_NECK_AND_HIPY_ANGLE, //151
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_HIPY_ANGLE, //152
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_ENDSITE_EYE_RY_ANGLE, //153
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_ENDSITE_EYE_LY_ANGLE, //154
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_NECKY_ANGLE, //155
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_RSHOULDERY_ANGLE, //156
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //157
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_RELBOWY_ANGLE, //158
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_7, //159
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_RHANDY_ANGLE, //160
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //161
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //162
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_LSHOULDERY_ANGLE, //163
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //164
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_LELBOWY_ANGLE, //165
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //166
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_LHANDY_ANGLE, //167
    MNET_UPPERBODY_IN_HALFWAY_RELBOW_AND_RHANDY_HALFWAY_NECK_AND_HIPY_ANGLE, //168
    MNET_UPPERBODY_IN_RHANDY_HIPY_ANGLE, //169
    MNET_UPPERBODY_IN_RHANDY_ENDSITE_EYE_RY_ANGLE, //170
    MNET_UPPERBODY_IN_RHANDY_ENDSITE_EYE_LY_ANGLE, //171
    MNET_UPPERBODY_IN_RHANDY_NECKY_ANGLE, //172
    MNET_UPPERBODY_IN_RHANDY_RSHOULDERY_ANGLE, //173
    MNET_UPPERBODY_IN_RHANDY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //174
    MNET_UPPERBODY_IN_RHANDY_RELBOWY_ANGLE, //175
    MNET_UPPERBODY_IN_RHANDY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //176
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_8, //177
    MNET_UPPERBODY_IN_RHANDY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //178
    MNET_UPPERBODY_IN_RHANDY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //179
    MNET_UPPERBODY_IN_RHANDY_LSHOULDERY_ANGLE, //180
    MNET_UPPERBODY_IN_RHANDY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //181
    MNET_UPPERBODY_IN_RHANDY_LELBOWY_ANGLE, //182
    MNET_UPPERBODY_IN_RHANDY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //183
    MNET_UPPERBODY_IN_RHANDY_LHANDY_ANGLE, //184
    MNET_UPPERBODY_IN_RHANDY_HALFWAY_NECK_AND_HIPY_ANGLE, //185
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_HIPY_ANGLE, //186
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ENDSITE_EYE_RY_ANGLE, //187
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ENDSITE_EYE_LY_ANGLE, //188
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_NECKY_ANGLE, //189
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_RSHOULDERY_ANGLE, //190
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //191
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_RELBOWY_ANGLE, //192
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //193
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_RHANDY_ANGLE, //194
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_9, //195
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //196
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_LSHOULDERY_ANGLE, //197
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //198
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_LELBOWY_ANGLE, //199
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //200
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_LHANDY_ANGLE, //201
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_HALFWAY_NECK_AND_HIPY_ANGLE, //202
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_HIPY_ANGLE, //203
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ENDSITE_EYE_RY_ANGLE, //204
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ENDSITE_EYE_LY_ANGLE, //205
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_NECKY_ANGLE, //206
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_RSHOULDERY_ANGLE, //207
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //208
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_RELBOWY_ANGLE, //209
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //210
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_RHANDY_ANGLE, //211
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //212
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_10, //213
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_LSHOULDERY_ANGLE, //214
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //215
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_LELBOWY_ANGLE, //216
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //217
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_LHANDY_ANGLE, //218
    MNET_UPPERBODY_IN_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_HALFWAY_NECK_AND_HIPY_ANGLE, //219
    MNET_UPPERBODY_IN_LSHOULDERY_HIPY_ANGLE, //220
    MNET_UPPERBODY_IN_LSHOULDERY_ENDSITE_EYE_RY_ANGLE, //221
    MNET_UPPERBODY_IN_LSHOULDERY_ENDSITE_EYE_LY_ANGLE, //222
    MNET_UPPERBODY_IN_LSHOULDERY_NECKY_ANGLE, //223
    MNET_UPPERBODY_IN_LSHOULDERY_RSHOULDERY_ANGLE, //224
    MNET_UPPERBODY_IN_LSHOULDERY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //225
    MNET_UPPERBODY_IN_LSHOULDERY_RELBOWY_ANGLE, //226
    MNET_UPPERBODY_IN_LSHOULDERY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //227
    MNET_UPPERBODY_IN_LSHOULDERY_RHANDY_ANGLE, //228
    MNET_UPPERBODY_IN_LSHOULDERY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //229
    MNET_UPPERBODY_IN_LSHOULDERY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //230
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_11, //231
    MNET_UPPERBODY_IN_LSHOULDERY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //232
    MNET_UPPERBODY_IN_LSHOULDERY_LELBOWY_ANGLE, //233
    MNET_UPPERBODY_IN_LSHOULDERY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //234
    MNET_UPPERBODY_IN_LSHOULDERY_LHANDY_ANGLE, //235
    MNET_UPPERBODY_IN_LSHOULDERY_HALFWAY_NECK_AND_HIPY_ANGLE, //236
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_HIPY_ANGLE, //237
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_ENDSITE_EYE_RY_ANGLE, //238
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_ENDSITE_EYE_LY_ANGLE, //239
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_NECKY_ANGLE, //240
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_RSHOULDERY_ANGLE, //241
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //242
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_RELBOWY_ANGLE, //243
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //244
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_RHANDY_ANGLE, //245
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //246
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //247
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_LSHOULDERY_ANGLE, //248
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_12, //249
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_LELBOWY_ANGLE, //250
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //251
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_LHANDY_ANGLE, //252
    MNET_UPPERBODY_IN_HALFWAY_LSHOULDER_AND_LELBOWY_HALFWAY_NECK_AND_HIPY_ANGLE, //253
    MNET_UPPERBODY_IN_LELBOWY_HIPY_ANGLE, //254
    MNET_UPPERBODY_IN_LELBOWY_ENDSITE_EYE_RY_ANGLE, //255
    MNET_UPPERBODY_IN_LELBOWY_ENDSITE_EYE_LY_ANGLE, //256
    MNET_UPPERBODY_IN_LELBOWY_NECKY_ANGLE, //257
    MNET_UPPERBODY_IN_LELBOWY_RSHOULDERY_ANGLE, //258
    MNET_UPPERBODY_IN_LELBOWY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //259
    MNET_UPPERBODY_IN_LELBOWY_RELBOWY_ANGLE, //260
    MNET_UPPERBODY_IN_LELBOWY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //261
    MNET_UPPERBODY_IN_LELBOWY_RHANDY_ANGLE, //262
    MNET_UPPERBODY_IN_LELBOWY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //263
    MNET_UPPERBODY_IN_LELBOWY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //264
    MNET_UPPERBODY_IN_LELBOWY_LSHOULDERY_ANGLE, //265
    MNET_UPPERBODY_IN_LELBOWY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //266
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_13, //267
    MNET_UPPERBODY_IN_LELBOWY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //268
    MNET_UPPERBODY_IN_LELBOWY_LHANDY_ANGLE, //269
    MNET_UPPERBODY_IN_LELBOWY_HALFWAY_NECK_AND_HIPY_ANGLE, //270
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_HIPY_ANGLE, //271
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_ENDSITE_EYE_RY_ANGLE, //272
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_ENDSITE_EYE_LY_ANGLE, //273
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_NECKY_ANGLE, //274
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_RSHOULDERY_ANGLE, //275
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //276
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_RELBOWY_ANGLE, //277
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //278
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_RHANDY_ANGLE, //279
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //280
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //281
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_LSHOULDERY_ANGLE, //282
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //283
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_LELBOWY_ANGLE, //284
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_14, //285
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_LHANDY_ANGLE, //286
    MNET_UPPERBODY_IN_HALFWAY_LELBOW_AND_LHANDY_HALFWAY_NECK_AND_HIPY_ANGLE, //287
    MNET_UPPERBODY_IN_LHANDY_HIPY_ANGLE, //288
    MNET_UPPERBODY_IN_LHANDY_ENDSITE_EYE_RY_ANGLE, //289
    MNET_UPPERBODY_IN_LHANDY_ENDSITE_EYE_LY_ANGLE, //290
    MNET_UPPERBODY_IN_LHANDY_NECKY_ANGLE, //291
    MNET_UPPERBODY_IN_LHANDY_RSHOULDERY_ANGLE, //292
    MNET_UPPERBODY_IN_LHANDY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //293
    MNET_UPPERBODY_IN_LHANDY_RELBOWY_ANGLE, //294
    MNET_UPPERBODY_IN_LHANDY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //295
    MNET_UPPERBODY_IN_LHANDY_RHANDY_ANGLE, //296
    MNET_UPPERBODY_IN_LHANDY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //297
    MNET_UPPERBODY_IN_LHANDY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //298
    MNET_UPPERBODY_IN_LHANDY_LSHOULDERY_ANGLE, //299
    MNET_UPPERBODY_IN_LHANDY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //300
    MNET_UPPERBODY_IN_LHANDY_LELBOWY_ANGLE, //301
    MNET_UPPERBODY_IN_LHANDY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //302
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_15, //303
    MNET_UPPERBODY_IN_LHANDY_HALFWAY_NECK_AND_HIPY_ANGLE, //304
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_HIPY_ANGLE, //305
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_ENDSITE_EYE_RY_ANGLE, //306
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_ENDSITE_EYE_LY_ANGLE, //307
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_NECKY_ANGLE, //308
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_RSHOULDERY_ANGLE, //309
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_HALFWAY_RSHOULDER_AND_RELBOWY_ANGLE, //310
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_RELBOWY_ANGLE, //311
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_HALFWAY_RELBOW_AND_RHANDY_ANGLE, //312
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_RHANDY_ANGLE, //313
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15Y_ANGLE, //314
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15Y_ANGLE, //315
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_LSHOULDERY_ANGLE, //316
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_HALFWAY_LSHOULDER_AND_LELBOWY_ANGLE, //317
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_LELBOWY_ANGLE, //318
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_HALFWAY_LELBOW_AND_LHANDY_ANGLE, //319
    MNET_UPPERBODY_IN_HALFWAY_NECK_AND_HIPY_LHANDY_ANGLE, //320
    MNET_UPPERBODY_IN_ANGLEUSEDFOR2DROTATION_16, //321
    MNET_UPPERBODY_IN_NUMBER
};

/** @brief Programmer friendly enumerator of expected outputs
 TODO: CAREFULL!*/
enum mocapNET_Output_upperbody_enum
{
    MOCAPNET_UPPERBODY_OUTPUT_HIP_XPOSITION = 0, //0
    MOCAPNET_UPPERBODY_OUTPUT_HIP_YPOSITION, //1
    MOCAPNET_UPPERBODY_OUTPUT_HIP_ZPOSITION, //2
    MOCAPNET_UPPERBODY_OUTPUT_HIP_ZROTATION, //3
    MOCAPNET_UPPERBODY_OUTPUT_HIP_YROTATION, //4
    MOCAPNET_UPPERBODY_OUTPUT_HIP_XROTATION, //5
    MOCAPNET_UPPERBODY_OUTPUT_ABDOMEN_ZROTATION, //6
    MOCAPNET_UPPERBODY_OUTPUT_ABDOMEN_XROTATION, //7
    MOCAPNET_UPPERBODY_OUTPUT_ABDOMEN_YROTATION, //8
    MOCAPNET_UPPERBODY_OUTPUT_CHEST_ZROTATION, //9
    MOCAPNET_UPPERBODY_OUTPUT_CHEST_XROTATION, //10
    MOCAPNET_UPPERBODY_OUTPUT_CHEST_YROTATION, //11
    MOCAPNET_UPPERBODY_OUTPUT_NECK_ZROTATION, //12
    MOCAPNET_UPPERBODY_OUTPUT_NECK_XROTATION, //13
    MOCAPNET_UPPERBODY_OUTPUT_NECK_YROTATION, //14
    MOCAPNET_UPPERBODY_OUTPUT_HEAD_ZROTATION, //15
    MOCAPNET_UPPERBODY_OUTPUT_HEAD_XROTATION, //16
    MOCAPNET_UPPERBODY_OUTPUT_HEAD_YROTATION, //17
    MOCAPNET_UPPERBODY_OUTPUT_EYE_L_ZROTATION, //18
    MOCAPNET_UPPERBODY_OUTPUT_EYE_L_XROTATION, //19
    MOCAPNET_UPPERBODY_OUTPUT_EYE_L_YROTATION, //20
    MOCAPNET_UPPERBODY_OUTPUT_EYE_R_ZROTATION, //21
    MOCAPNET_UPPERBODY_OUTPUT_EYE_R_XROTATION, //22
    MOCAPNET_UPPERBODY_OUTPUT_EYE_R_YROTATION, //23
    MOCAPNET_UPPERBODY_OUTPUT_RSHOULDER_ZROTATION, //24
    MOCAPNET_UPPERBODY_OUTPUT_RSHOULDER_XROTATION, //25
    MOCAPNET_UPPERBODY_OUTPUT_RSHOULDER_YROTATION, //26
    MOCAPNET_UPPERBODY_OUTPUT_RELBOW_ZROTATION, //27
    MOCAPNET_UPPERBODY_OUTPUT_RELBOW_XROTATION, //28
    MOCAPNET_UPPERBODY_OUTPUT_RELBOW_YROTATION, //29
    MOCAPNET_UPPERBODY_OUTPUT_RHAND_ZROTATION, //30
    MOCAPNET_UPPERBODY_OUTPUT_RHAND_XROTATION, //31
    MOCAPNET_UPPERBODY_OUTPUT_RHAND_YROTATION, //32
    MOCAPNET_UPPERBODY_OUTPUT_LSHOULDER_ZROTATION, //33
    MOCAPNET_UPPERBODY_OUTPUT_LSHOULDER_XROTATION, //34
    MOCAPNET_UPPERBODY_OUTPUT_LSHOULDER_YROTATION, //35
    MOCAPNET_UPPERBODY_OUTPUT_LELBOW_ZROTATION, //36
    MOCAPNET_UPPERBODY_OUTPUT_LELBOW_XROTATION, //37
    MOCAPNET_UPPERBODY_OUTPUT_LELBOW_YROTATION, //38
    MOCAPNET_UPPERBODY_OUTPUT_LHAND_ZROTATION, //39
    MOCAPNET_UPPERBODY_OUTPUT_LHAND_XROTATION, //40
    MOCAPNET_UPPERBODY_OUTPUT_LHAND_YROTATION, //41
    MOCAPNET_UPPERBODY_OUTPUT_NUMBER
};

/** @brief Programmer friendly enumerator of NSDM elments*/
enum mocapNET_NSDM_upperbody_enum
{
    MNET_NSDM_UPPERBODY_HIP = 0, //0
    MNET_NSDM_UPPERBODY_ENDSITE_EYE_R, //1
    MNET_NSDM_UPPERBODY_ENDSITE_EYE_L, //2
    MNET_NSDM_UPPERBODY_NECK, //3
    MNET_NSDM_UPPERBODY_RSHOULDER, //4
    MNET_NSDM_UPPERBODY_VIRTUAL_HALFWAY_BETWEEN_RSHOULDER_AND_RELBOW, //5
    MNET_NSDM_UPPERBODY_RELBOW, //6
    MNET_NSDM_UPPERBODY_VIRTUAL_HALFWAY_BETWEEN_RELBOW_AND_RHAND, //7
    MNET_NSDM_UPPERBODY_RHAND, //8
    MNET_NSDM_UPPERBODY_VIRTUAL_HIP_X_MINUS_0_15_Y_MINUS_0_15, //9
    MNET_NSDM_UPPERBODY_VIRTUAL_HIP_X_PLUS0_15_Y_MINUS_0_15, //10
    MNET_NSDM_UPPERBODY_LSHOULDER, //11
    MNET_NSDM_UPPERBODY_VIRTUAL_HALFWAY_BETWEEN_LSHOULDER_AND_LELBOW, //12
    MNET_NSDM_UPPERBODY_LELBOW, //13
    MNET_NSDM_UPPERBODY_VIRTUAL_HALFWAY_BETWEEN_LELBOW_AND_LHAND, //14
    MNET_NSDM_UPPERBODY_LHAND, //15
    MNET_NSDM_UPPERBODY_VIRTUAL_HALFWAY_BETWEEN_NECK_AND_HIP, //16
    MNET_NSDM_UPPERBODY_NUMBER
};

/** @brief This is a lookup table to immediately resolve referred Joints*/
static const int mocapNET_ResolveJoint_upperbody[] =
{
    0, //0
    4, //1
    3, //2
    1, //3
    5, //4
    5, //5
    6, //6
    6, //7
    7, //8
    0, //9
    0, //10
    8, //11
    8, //12
    9, //13
    9, //14
    10, //15
    1, //16
    0//end of array
};

/** @brief This is a lookup table to immediately resolve referred Joints of second targets*/
static const int mocapNET_ResolveSecondTargetJoint_upperbody[] =
{
    0, //0
    0, //1
    0, //2
    0, //3
    0, //4
    6, //5
    0, //6
    7, //7
    0, //8
    0, //9
    0, //10
    0, //11
    9, //12
    0, //13
    10, //14
    0, //15
    0, //16
    0//end of array
};

/** @brief This is the configuration of NSDM elements :
 *  A value of 0 is a normal 2D point
 *  A value of 1 is a 2D point plus some offset
 *  A value of 2 is a virtual point between two 2D points */
static const int mocapNET_ArtificialJoint_upperbody[] =
{
    0, //0
    0, //1
    0, //2
    0, //3
    0, //4
    2, //5
    0, //6
    2, //7
    0, //8
    1, //9
    1, //10
    0, //11
    2, //12
    0, //13
    2, //14
    0, //15
    2, //16
    0//end of array
};

/** @brief These are X offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_upperbody )*/
static const float mocapNET_ArtificialJointXOffset_upperbody[] =
{
    0, //0
    0, //1
    0, //2
    0, //3
    0, //4
    0, //5
    0, //6
    0, //7
    0, //8
    -0.15, //9
    0.15, //10
    0, //11
    0, //12
    0, //13
    0, //14
    0, //15
    0, //16
    0//end of array
};

/** @brief These are Y offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_upperbody )*/
static const float mocapNET_ArtificialJointYOffset_upperbody[] =
{
    0, //0
    0, //1
    0, //2
    0, //3
    0, //4
    0, //5
    0, //6
    0, //7
    0, //8
    -0.15, //9
    -0.15, //10
    0, //11
    0, //12
    0, //13
    0, //14
    0, //15
    0, //16
    0//end of array
};

/** @brief These are 2D Joints that are used as starting points for scaling vectors*/
static const int mocapNET_ScalingStart_upperbody[] =
{
    0, //0
    0, //1
    0//end of array
};

/** @brief These are 2D Joints that are used as ending points for scaling vectors*/
static const int mocapNET_ScalingEnd_upperbody[] =
{
    5, //0
    8, //1
    0//end of array
};

/** @brief These is a 2D Joints that is used as alignment for the skeleton*/
static const int mocapNET_AlignmentStart_upperbody[] =
{
    0, //0
    0//end of array
};

/** @brief These is a 2D Joints that is used as alignment for the skeleton*/
static const int mocapNET_AlignmentEnd_upperbody[] =
{
    1, //0
    0//end of array
};

/** @brief This function can be used to debug NSDM input and find in a user friendly what is missing..!*/
static int upperbodyCountMissingNSDMElements(std::vector<float> mocapNETInput,int verbose)
{
    unsigned int numberOfZeros=0;
    for (int i=0; i<mocapNETInput.size(); i++)
    {
        if (mocapNETInput[i]==0)
        {
            if (verbose) {
                fprintf(stderr,"upperbody NSDM %s missing\n",mocapNET_upperbody[i]);
            }
            ++numberOfZeros;
        }
    }
    return numberOfZeros;
}

/** @brief Another debug function to make sure the joint order is correctly resolved..!*/
static void upperbodyDebugNSDMJointOrder(char ** labels)
{
    for (int i=0; i<MNET_NSDM_UPPERBODY_NUMBER; i++)
    {
        fprintf(stderr,"NSDM upperbody %s => skeletonSerialized %s\n ",mocapNET_upperbody[i],labels[i]);
    }
}

/** @brief This function returns the euclidean distance between two input 2D joints and zero if either of them is invalid*/
static float getJoint2DDistance_UPPERBODY(std::vector<float> in,int jointA,int jointB)
{
    float aX=in[jointA*3+0];
    float aY=in[jointA*3+1];
    float bX=in[jointB*3+0];
    float bY=in[jointB*3+1];
    if ( ((aX==0) && (aY==0)) || ((bX==0) && (bY==0)) ) {
        return 0.0;
    }


    float xDistance=(float) bX-aX;
    float yDistance=(float) bY-aY;
    return sqrt( (xDistance*xDistance) + (yDistance*yDistance) );
}
/*
static std::vector<float> upperbodyCreateNDSM(std::vector<float> in,float alignmentAngle2D,int havePositionalElements,int haveAngularElements,int doNormalization)
{
    std::vector<float> result;
    int secondTargetJointID;
    float sIX,sIY,sJX,sJY;
    for (int i=0; i<MNET_NSDM_UPPERBODY_NUMBER; i++)
    {
        //------------------------------------
        unsigned int iJointID=mocapNET_ResolveJoint_upperbody[i];
        float iX=in[iJointID*3+0];
        float iY=in[iJointID*3+1];
        float iVisibility=in[iJointID*3+2];
        int invalidPointI=0;

        //------------------------------------
        unsigned int jJointID;
        float jX=0.0;
        float jY=0.0;
        float jVisibility=0.0;
        int invalidPointJ=0;

        //------------------------------------

        if ((iX==0) && (iY==0))
        {
            for (int j=0; j<MNET_NSDM_UPPERBODY_NUMBER; j++)
            {
                if (havePositionalElements) {
                    result.push_back(0.0);
                    result.push_back(0.0);
                }
                if (haveAngularElements)    {
                    result.push_back(0.0);
                }
            }
            invalidPointI=1;
        } else
        {
            switch (mocapNET_ArtificialJoint_upperbody[i])
            {
            case 1 :
                iX+=mocapNET_ArtificialJointXOffset_upperbody[i];
                iY+=mocapNET_ArtificialJointYOffset_upperbody[i];
                break;

            case 2 :
                secondTargetJointID=mocapNET_ResolveSecondTargetJoint_upperbody[i];
                sIX=in[secondTargetJointID*3+0];
                sIY=in[secondTargetJointID*3+1];
                invalidPointI=((sIX==0) && (sIY==0));
                if (!invalidPointI)
                {
                    iX=(float) (iX+sIX)/2;
                    iY=(float) (iY+sIY)/2;
                }
                break;

            default :
                //Normal NSDM element ( not virtual )
                break;
            };
            for (int j=0; j<MNET_NSDM_UPPERBODY_NUMBER; j++)
            {
                jJointID=mocapNET_ResolveJoint_upperbody[j];
                jX=in[jJointID*3+0];
                jY=in[jJointID*3+1];
                jVisibility=in[jJointID*3+2];
                invalidPointJ=0;

                if ((jX==0) && (jY==0))
                {
                    if (havePositionalElements) {
                        result.push_back(0.0);
                        result.push_back(0.0);
                    }
                    if (haveAngularElements)    {
                        result.push_back(0.0);
                    }
                    invalidPointJ=1;

                }
                else
                {
                    switch (mocapNET_ArtificialJoint_upperbody[j])
                    {
                    case 1 :
                        jX+=mocapNET_ArtificialJointXOffset_upperbody[j];
                        jY+=mocapNET_ArtificialJointYOffset_upperbody[j];
                        break;

                    case 2 :
                        secondTargetJointID=mocapNET_ResolveSecondTargetJoint_upperbody[j];
                        sJX=in[secondTargetJointID*3+0];
                        sJY=in[secondTargetJointID*3+1];
                        invalidPointJ=((sJX==0) && (sJY==0));
                        if (!invalidPointJ)
                        {
                            jX=(float) (jX+sJX)/2;
                            jY=(float) (jY+sJY)/2;
                        }
                        break;

                    default :
                        //Normal NSDM element ( not virtual )
                        break;
                    };
                    //---------------------------------
                    if (invalidPointJ||invalidPointI)
                    {
                        if (havePositionalElements) {
                            result.push_back(0.0);
                            result.push_back(0.0);
                        }
                        if (haveAngularElements)    {
                            result.push_back(0.0);
                        }
                    } else
                    {
                        if (havePositionalElements)
                        {
                            result.push_back(iX-jX);
                            result.push_back(iY-jY);
                        }
                        if (haveAngularElements)
                        {
                            result.push_back(getAngleToAlignToZero_tools(iX,iY,jX,jY));
                        }
                    }
                    //---------------------------------
                }  // proper j Joint

            }   // j for loop end
        } // proper i Joint
    }  // i for loop end


    if (doNormalization)
    {
        if (havePositionalElements)
        {
            //Normalization-------------------------------------------------------------------------------
            unsigned int numberOfNSDMScalingRules=2;
            if (numberOfNSDMScalingRules>0)
            {
                unsigned int numberOfDistanceSamples=0;
                float sumOfDistanceSamples=0.0;
                for ( int i=0; i<numberOfNSDMScalingRules; i++)
                {
                    unsigned int iJointID=mocapNET_ScalingStart_upperbody[i];
                    unsigned int jJointID=mocapNET_ScalingEnd_upperbody[i];
                    float distance = getJoint2DDistance_UPPERBODY(in,iJointID,jJointID);
                    if (distance>0.0)
                    {
                        numberOfDistanceSamples=numberOfDistanceSamples+1;
                        sumOfDistanceSamples=sumOfDistanceSamples+distance;
                    }
                }
//-------------------------------------------------------------------------------------------------
                float scaleDistance=1.0;
//-------------------------------------------------------------------------------------------------
                if (numberOfDistanceSamples>0)
                {
                    scaleDistance=(float) sumOfDistanceSamples/numberOfDistanceSamples;
                }
//-------------------------------------------------------------------------------------------------
                if (scaleDistance!=1.0)
                {
                    for (int i=0; i<result.size(); i++)
                    {
                        result[i]=result[i]/scaleDistance;
                    }
                }
            } //If there are normalization rules..
//-------------------------------------------------------------------------------------------------
            for (int i=0; i<result.size(); i++)
            {
                if (result[i]!=0.0)
                {
                    result[i]=0.5+result[i];
                }
            }
//-------------------------------------------------------------------------------------------------
        }



//-------------------------------------------------------------------------------------------------
        if ( (haveAngularElements) && (!havePositionalElements) )
        {
            float minValue=result[0], maxValue=result[0];
            for (int i=0; i<result.size(); i++)
            {
                if (result[i]<minValue) {
                    minValue=result[i];
                }
                if (result[i]>maxValue) {
                    maxValue=result[i];
                }
            }
            fprintf(stderr,"Original Min Value %0.2f, Max Value %0.2f \n",minValue,maxValue);


            unsigned int iJointID=mocapNET_AlignmentStart_upperbody[0];
            unsigned int jJointID=mocapNET_AlignmentEnd_upperbody[0];
            float aX=in[iJointID*3+0];
            float aY=in[iJointID*3+1];
            float bX=in[jJointID*3+0];
            float bY=in[jJointID*3+1];
            float alignmentAngle=getAngleToAlignToZero_tools(aX,aY,bX,bY);
            for (int i=0; i<result.size(); i++)
            {
                if (result[i]!=0.0)
                {
                    result[i]= alignmentAngle + result[i];
                }
            }
            minValue=result[0], maxValue=result[0];
            for (int i=0; i<result.size(); i++)
            {
                if (result[i]<minValue) {
                    minValue=result[i];
                }
                if (result[i]>maxValue) {
                    maxValue=result[i];
                }
            }
            fprintf(stderr,"Aligned Min Value %0.2f, Max Value %0.2f \n",minValue,maxValue);


        }
//-------------------------------------------------------------------------------------------------


    } //If normalization is enabled..


//New normalization code that overrides diagonal of Matrix
    unsigned int elementID=0;
    unsigned int firstJointID=mocapNET_ResolveJoint_upperbody[0];
    for (unsigned int i=0; i<MNET_NSDM_UPPERBODY_NUMBER; i++)
    {
        for (unsigned int j=0; j<MNET_NSDM_UPPERBODY_NUMBER; j++)
        {
            if (i==j)
            {
                result[elementID]=alignmentAngle2D;
                //Rest diagonal elements will encode euclidean distances from base joint
                //if richDiagonal_upperbody flag is set..
                if ( (i>0) && (richDiagonal_upperbody) )
                {
                    unsigned int jJointID=mocapNET_ResolveJoint_upperbody[j];
                    result[elementID]=getJoint2DDistance_UPPERBODY(in,firstJointID,jJointID);
                }
            }
            elementID+=1;
        }
    }
    return result;
}

*/