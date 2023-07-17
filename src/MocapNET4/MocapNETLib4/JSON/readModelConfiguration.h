/** @file readModelConfiguration.h
 *  @brief Parsing the JSON files accompanying models..!
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef READ_JSON_CONFIGURATION_H_INCLUDED
#define READ_JSON_CONFIGURATION_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

 
#include <stdio.h>
#include <stdlib.h>
#include "nxjson.h"
#include "../tools.h"

#define SMALL_STR 32
#define BIG_STR 128
#define MAX_JOINT_NAME 32
#define MAX_DESCRIPTOR_ELEMENTS 64
#define MAX_HIERARCHY_ELEMENTS 64
#define MAX_BANNED_ELEMENTS 16

//--------------------------------------
struct jointName
{
  char         joint[MAX_JOINT_NAME];
};
//--------------------------------------
struct jointPair
{
  char         jointStart[MAX_JOINT_NAME];
  unsigned int jointStartID;
  char         jointEnd[MAX_JOINT_NAME];
  unsigned int jointEndID;
};
//--------------------------------------
struct jointDescriptorItem
{
  char         joint[MAX_JOINT_NAME];
  unsigned int jointID;
  char         isVirtual;
  float        xOffset;
  float        yOffset;
  char         halfwayFromJointAndThis[MAX_JOINT_NAME];
  unsigned int secondTargetJointID;
};
//--------------------------------------
struct jointHierarchyItem
{
  char         joint[MAX_JOINT_NAME];
  char         inheritNetwork[MAX_JOINT_NAME];
  char         parent[MAX_JOINT_NAME];
  unsigned int parentID;
  unsigned int importance;
  char         immuneToSelfOcclusions;
};
//--------------------------------------

struct ModelConfigurationData
{
  float version;
  //--------------------------------------
  char backend[SMALL_STR];
  char precision[SMALL_STR];
  char BVH[BIG_STR];
  char outputDirectory[BIG_STR];
  unsigned int veryHighNumberOfEpochs;
  unsigned int highNumberOfEpochs;
  unsigned int defaultNumberOfEpochs;
  unsigned int defaultBatchSize;
  float learningRate;
  float minEarlyStoppingDelta;
  char activationFunction[SMALL_STR];
  float dropoutRate;
  float lamda; 
  char useQuadLoss;
  char useSquaredLoss;
  unsigned int earlyStoppingPatience; 
  char rememberWeights;
  char rememberConsecutiveWeights;
  char useOnlineHardExampleMining;
  unsigned int hardMiningEpochs;
  unsigned int normalMiningEpochs;
  unsigned int groupOutputs;
  char ignoreOcclusions;
  char NSDMNormalizationMasterSwitch;
  char NSDMAlsoUseAlignmentAngles;
  unsigned int neuralNetworkDepth;
  char EDM;
  char eNSRM;
  char doPCA[BIG_STR];
  unsigned int PCADimensionsKept;
  unsigned int padEnsembleInput;
  //--------------------------------------
  struct jointPair normalizedBasedOn[3];
  unsigned int numberOfNormalizedBasedOnRules;
  //-------------------------------------- 
  struct jointPair alignment[3];
  unsigned int numberOfAlignmentRules;
  //-------------------------------------- 
  struct jointDescriptorItem descriptorElements[MAX_DESCRIPTOR_ELEMENTS];
  unsigned int numberOfDescriptorElements;
  //-------------------------------------- 
  struct jointHierarchyItem hierarchyElements[MAX_HIERARCHY_ELEMENTS];
  unsigned int numberOfHierarchyElements;
  //-------------------------------------- 
  struct jointName bannedJoints[MAX_BANNED_ELEMENTS];
  unsigned int numberOfBannedJoints;
};


static void printModelConfigurationData(struct ModelConfigurationData* out)
{
  fprintf(stderr,"Version : %0.2f\n",                   out->version);
  fprintf(stderr,"Backend : %s\n",                      out->backend);
  fprintf(stderr,"Precision : %s\n",                    out->precision);
  fprintf(stderr,"BVH : %s\n",                          out->BVH);
  fprintf(stderr,"outputDirectory : %s\n",              out->outputDirectory);
  fprintf(stderr,"veryHighNumberOfEpochs : %u\n",       out->veryHighNumberOfEpochs);
  fprintf(stderr,"highNumberOfEpochs : %u\n",           out->highNumberOfEpochs);
  fprintf(stderr,"defaultNumberOfEpochs : %u\n",        out->defaultNumberOfEpochs);
  fprintf(stderr,"defaultBatchSize : %u\n",             out->defaultBatchSize);
  fprintf(stderr,"learningRate : %f\n",                 out->learningRate);
  fprintf(stderr,"minEarlyStoppingDelta : %f\n",        out->minEarlyStoppingDelta);
  fprintf(stderr,"activationFunction : %s\n",           out->activationFunction);
  fprintf(stderr,"dropoutRate : %f\n",                  out->dropoutRate);
  fprintf(stderr,"lamda : %f\n",                        out->lamda); 
  fprintf(stderr,"useQuadLoss : %u\n",                  out->useQuadLoss);
  fprintf(stderr,"useSquaredLoss : %u\n",               out->useSquaredLoss);
  fprintf(stderr,"earlyStoppingPatience : %u\n",        out->earlyStoppingPatience); 
  fprintf(stderr,"rememberWeights : %u\n",              out->rememberWeights);
  fprintf(stderr,"rememberConsecutiveWeights : %u\n",   out->rememberConsecutiveWeights);
  fprintf(stderr,"useOnlineHardExampleMining : %u\n",   out->useOnlineHardExampleMining);
  fprintf(stderr,"hardMiningEpochs : %u\n",             out->hardMiningEpochs);
  fprintf(stderr,"normalMiningEpochs : %u\n",           out->normalMiningEpochs);
  fprintf(stderr,"groupOutputs : %u\n",                 out->groupOutputs);
  fprintf(stderr,"ignoreOcclusions : %u\n",             out->ignoreOcclusions);
  fprintf(stderr,"NSDMNormalizationMasterSwitch : %u\n",out->NSDMNormalizationMasterSwitch);
  fprintf(stderr,"NSDMAlsoUseAlignmentAngles : %u\n",   out->NSDMAlsoUseAlignmentAngles);
  fprintf(stderr,"neuralNetworkDepth : %u\n",           out->neuralNetworkDepth);
  fprintf(stderr,"EDM : %u\n",                          out->EDM);
  fprintf(stderr,"eNSRM : %u\n",                        out->eNSRM);
  fprintf(stderr,"BVH : %s\n",                          out->doPCA);
  fprintf(stderr,"PCADimensionsKept : %u\n",            out->PCADimensionsKept);
  fprintf(stderr,"padEnsembleInput : %u\n",             out->padEnsembleInput);

  fprintf(stderr,"Normalization Rules : %u\n",out->numberOfNormalizedBasedOnRules);
  for (int i=0; i<out->numberOfNormalizedBasedOnRules; i++)
  {
    fprintf(stderr,"Rule %u : \n",i);
    fprintf(stderr,"    jointStart:%s\n"  ,out->normalizedBasedOn[i].jointStart);
    fprintf(stderr,"    jointStartID:%u\n",out->normalizedBasedOn[i].jointStartID);
    fprintf(stderr,"    jointEnd:%s\n"    ,out->normalizedBasedOn[i].jointEnd);
    fprintf(stderr,"    jointEndID:%u\n"  ,out->normalizedBasedOn[i].jointEndID);
  }

  fprintf(stderr,"Alignment Rules : %u\n",out->numberOfAlignmentRules);
  for (int i=0; i<out->numberOfAlignmentRules; i++)
  {
    fprintf(stderr,"Rule %u : \n",i);
    fprintf(stderr,"    jointStart:%s\n"  ,out->alignment[i].jointStart);
    fprintf(stderr,"    jointStartID:%u\n",out->alignment[i].jointStartID);
    fprintf(stderr,"    jointEnd:%s\n"    ,out->alignment[i].jointEnd);
    fprintf(stderr,"    jointEndID:%u\n"  ,out->alignment[i].jointEndID);
  }

  fprintf(stderr,"Descriptor Rules : %u\n",out->numberOfDescriptorElements);
  for (int i=0; i<out->numberOfDescriptorElements; i++)
  {
    fprintf(stderr,"Rule %u : \n",i);
    fprintf(stderr,"    joint:%s\n"                  ,out->descriptorElements[i].joint);
    fprintf(stderr,"    jointID:%u\n"                ,out->descriptorElements[i].jointID);
    fprintf(stderr,"    isVirtual:%u\n"              ,out->descriptorElements[i].isVirtual);
    fprintf(stderr,"    xOffset:%f\n"                ,out->descriptorElements[i].xOffset);
    fprintf(stderr,"    yOffset:%f\n"                ,out->descriptorElements[i].yOffset);
    fprintf(stderr,"    halfwayFromJointAndThis:%s\n",out->descriptorElements[i].halfwayFromJointAndThis);
    fprintf(stderr,"    secondTargetJointID:%u\n"    ,out->descriptorElements[i].secondTargetJointID);
  } 

  //TODO POPULATE JOINT ID!

  fprintf(stderr,"Hierarchy Rules : %u\n",out->numberOfHierarchyElements);
  for (int i=0; i<out->numberOfHierarchyElements; i++)
  {
    fprintf(stderr,"Rule %u : \n",i);
    fprintf(stderr,"    joint:%s\n"                  ,out->hierarchyElements[i].joint);
    fprintf(stderr,"    inheritNetwork:%s\n"         ,out->hierarchyElements[i].inheritNetwork);
    fprintf(stderr,"    parent:%s\n"                 ,out->hierarchyElements[i].parent);
    fprintf(stderr,"    parentID:%u\n"               ,out->hierarchyElements[i].parentID);
    fprintf(stderr,"    importance:%u\n"             ,out->hierarchyElements[i].importance);
    fprintf(stderr,"    immuneToSelfOcclusions:%u\n" ,out->hierarchyElements[i].immuneToSelfOcclusions);
  } 


  fprintf(stderr,"Banned Encoders Rules : %u\n",out->numberOfBannedJoints);
  for (int i=0; i<out->numberOfBannedJoints; i++)
  {
    fprintf(stderr,"Rule %u : \n",i);
    fprintf(stderr,"    joint:%s\n"                  ,out->bannedJoints[i].joint); 
  }
}




static int resolveConfigurationData(struct ModelConfigurationData* config)
{
  if (config!=0) 
  { //Found a configuration to resolve..
  
  printModelConfigurationData(config);

  fprintf(stderr,"Resolving %u NSxM matrix elements..\n",config->numberOfDescriptorElements);
  fprintf(stderr,"Using %u hierarchy elements..\n",config->numberOfHierarchyElements);
  
  //---------------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------------
  
  for (unsigned int descID=0; descID<config->numberOfDescriptorElements; descID++)
  {
    //Resolve NSxM to Joint ID 
    for (unsigned int jointID=0; jointID<config->numberOfHierarchyElements; jointID++)
    {
        fprintf(stderr,"Trying to match %s(%u/%u) with %s(%u/%u)\n",config->descriptorElements[descID].joint,descID,config->numberOfDescriptorElements,config->hierarchyElements[jointID].joint,jointID,config->numberOfHierarchyElements);
        
        if (strcmp(config->descriptorElements[descID].joint,config->hierarchyElements[jointID].joint)==0)
        {
            fprintf(stderr,"Found\n");
            config->descriptorElements[descID].jointID = jointID;
            break;
        }
    }
    
    //Resolve NSxM to Joint ID for halfway 
    for (int jointID=0; jointID<config->numberOfHierarchyElements; jointID++)
    {
        if (config->descriptorElements[descID].isVirtual==2)
        {
         fprintf(stderr,"Trying to match %s(%u) with %s(%u)\n",config->descriptorElements[descID].halfwayFromJointAndThis,descID,config->hierarchyElements[jointID].joint,jointID);
        
         if (strcmp(config->descriptorElements[descID].halfwayFromJointAndThis,config->hierarchyElements[jointID].joint)==0)
          {
            fprintf(stderr,"Found\n");
            config->descriptorElements[descID].secondTargetJointID = jointID;
            break;
          }        
        }
    }
  }
  
  //---------------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------------
  
  
  
  
  const int INVALID_VALUE=666;
  
  //Resolve normalized based on order..
  //---------------------------------------------------------------------------------------------------------------------------
  for (unsigned int descID=0; descID<config->numberOfNormalizedBasedOnRules; descID++)
  {
      config->normalizedBasedOn[descID].jointStartID=INVALID_VALUE;
      config->normalizedBasedOn[descID].jointEndID  =INVALID_VALUE;
  }
  //---------------------------------------------------------------------------------------------------------------------------
  for (unsigned int descID=0; descID<config->numberOfDescriptorElements; descID++)
  {
    for (int jointID=0; jointID<config->numberOfHierarchyElements; jointID++)
    {
        fprintf(stderr,"Trying to match norm rule %s(%u/%u) with %s(%u/%u)\n",config->descriptorElements[descID].joint,descID,config->numberOfNormalizedBasedOnRules,config->hierarchyElements[jointID].joint,jointID,config->numberOfHierarchyElements);
        
        if (strcmp(config->normalizedBasedOn[descID].jointStart,config->hierarchyElements[jointID].joint)==0)
          {
            fprintf(stderr,"Found\n");
            config->normalizedBasedOn[descID].jointStartID = jointID;
          }
        if (strcmp(config->normalizedBasedOn[descID].jointEnd,config->hierarchyElements[jointID].joint)==0)
          {
            fprintf(stderr,"Found\n");
            config->normalizedBasedOn[descID].jointEndID = jointID;
          }
    }
  }
  //---------------------------------------------------------------------------------------------------------------------------
  for (unsigned int descID=0; descID<config->numberOfNormalizedBasedOnRules; descID++)
  {
      if ( config->normalizedBasedOn[descID].jointStartID == INVALID_VALUE ) 
      {
          fprintf(stderr,"Normalization Rule %u/%u for start of joint is unresolved, stopping..\n",descID,config->numberOfNormalizedBasedOnRules);
          return 0;
      }
      if ( config->normalizedBasedOn[descID].jointEndID == INVALID_VALUE ) 
      {
          fprintf(stderr,"Normalization Rule %u/%u for end of joint is unresolved, stopping..\n",descID,config->numberOfNormalizedBasedOnRules);
          return 0;
      } 
  }
  //---------------------------------------------------------------------------------------------------------------------------
  
  
  // Resolve joint alignment..
  //---------------------------------------------------------------------------------------------------------------------------
  for (unsigned int descID=0; descID<config->numberOfAlignmentRules; descID++)
  {
      config->alignment[descID].jointStartID=INVALID_VALUE;
      config->alignment[descID].jointEndID  =INVALID_VALUE;
  }
  //---------------------------------------------------------------------------------------------------------------------------
  for (unsigned int descID=0; descID<config->numberOfAlignmentRules; descID++)
  {
    for (int jointID=0; jointID<config->numberOfHierarchyElements; jointID++)
    {
        fprintf(stderr,"Trying to match norm rule %s(%u/%u) with %s(%u/%u)\n",config->descriptorElements[descID].joint,descID,config->numberOfNormalizedBasedOnRules,config->hierarchyElements[jointID].joint,jointID,config->numberOfHierarchyElements);
        
        if (strcmp(config->alignment[descID].jointStart,config->hierarchyElements[jointID].joint)==0)
          {
            fprintf(stderr,"Found\n");
            config->alignment[descID].jointStartID = jointID;
          }
        if (strcmp(config->alignment[descID].jointEnd,config->hierarchyElements[jointID].joint)==0)
          {
            fprintf(stderr,"Found\n");
            config->alignment[descID].jointEndID = jointID;
          }
    }
  }
  //---------------------------------------------------------------------------------------------------------------------------
  for (unsigned int descID=0; descID<config->numberOfAlignmentRules; descID++)
  {
      if ( config->alignment[descID].jointStartID == INVALID_VALUE ) 
      {
          fprintf(stderr,"Normalization Rule %u/%u for start of joint is unresolved, stopping..\n",descID,config->numberOfNormalizedBasedOnRules);
          return 0;
      }
      if ( config->alignment[descID].jointEndID == INVALID_VALUE ) 
      {
          fprintf(stderr,"Normalization Rule %u/%u for end of joint is unresolved, stopping..\n",descID,config->numberOfNormalizedBasedOnRules);
          return 0;
      } 
  }
  //---------------------------------------------------------------------------------------------------------------------------
  
  
  
  //Resolve parent order for OpenCV drawing..
  for (unsigned int jointID=0; jointID<config->numberOfHierarchyElements; jointID++)
  {
    //Resolve NSxM to Joint ID 
    for (unsigned int parentID=0; parentID<config->numberOfHierarchyElements; parentID++)
    { 
        if (strcmp(config->hierarchyElements[parentID].joint,config->hierarchyElements[jointID].joint)==0)
        {
            fprintf(stderr,"Found\n");
            config->hierarchyElements[jointID].parentID = jointID;
            break;
        }
    }
  }
  
  
   return 1;
  }
  return 0;
}





static int loadModelConfigurationData(struct ModelConfigurationData* out,const char * jsonFilename)
{
  fprintf(stderr,"Loading Configuration file %s ...\n",jsonFilename);
  unsigned int inputLength=0;
  char* input = readFileToMemory(jsonFilename,&inputLength);
  if (input!=0) 
  {
   fprintf(stderr,"Parsing %s ...\n",jsonFilename);
   const nx_json* json=nx_json_parse_utf8(input);
   const nx_json* rule=0;
   const nx_json* item=0;
   
   //-------------------------------------------------------------------
   const nx_json* j  = nx_json_get(json,"version");
   if ((j!=0) && (j->type==NX_JSON_STRING)) { out->version = atof(j->text_value); }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"backend");
   if ((j!=0) && (j->type==NX_JSON_STRING)) { snprintf(out->backend,SMALL_STR,"%s",j->text_value); }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"precision");
   if ((j!=0) && (j->type==NX_JSON_STRING)) { snprintf(out->precision,SMALL_STR,"%s",j->text_value); }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"BVH");
   if ((j!=0) && (j->type==NX_JSON_STRING)) { snprintf(out->BVH,BIG_STR,"%s",j->text_value); }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"OutputDirectory");
   if ((j!=0) && (j->type==NX_JSON_STRING)) { snprintf(out->outputDirectory,BIG_STR,"%s",j->text_value); }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"veryHighNumberOfEpochs");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->veryHighNumberOfEpochs = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"highNumberOfEpochs");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->highNumberOfEpochs = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"defaultNumberOfEpochs");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->defaultNumberOfEpochs = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"defaultBatchSize");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->defaultBatchSize = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"learningRate");
   if ((j!=0) && (j->type==NX_JSON_DOUBLE)) { out->learningRate = (float) j->dbl_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"minEarlyStoppingDelta");
   if ((j!=0) && (j->type==NX_JSON_DOUBLE)) { out->minEarlyStoppingDelta = (float) j->dbl_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"activationFunction");
   if ((j!=0) && (j->type==NX_JSON_STRING)) { snprintf(out->activationFunction,SMALL_STR,"%s",j->text_value); }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"dropoutRate");
   if ((j!=0) && (j->type==NX_JSON_DOUBLE)) { out->dropoutRate = (float) j->dbl_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"lamda");
   if ((j!=0) && (j->type==NX_JSON_DOUBLE)) { out->lamda = (float) j->dbl_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"useQuadLoss");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->useQuadLoss = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"useSquaredLoss");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->useSquaredLoss = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"earlyStoppingPatience");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->earlyStoppingPatience = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"rememberWeights");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->rememberWeights = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"rememberConsecutiveWeights");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->rememberConsecutiveWeights = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"useOnlineHardExampleMining");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->useOnlineHardExampleMining = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"hardMiningEpochs");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->hardMiningEpochs = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"normalMiningEpochs");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->normalMiningEpochs = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"groupOutputs");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->groupOutputs = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"ignoreOcclusions");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->ignoreOcclusions = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"NSDMNormalizationMasterSwitch");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->NSDMNormalizationMasterSwitch = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"NSDMAlsoUseAlignmentAngles");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->NSDMAlsoUseAlignmentAngles = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"neuralNetworkDepth");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->neuralNetworkDepth = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"EDM");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->EDM = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"eNSRM");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->eNSRM = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"doPCA");
   if ((j!=0) && (j->type==NX_JSON_STRING)) { snprintf(out->doPCA,BIG_STR,"%s",j->text_value); }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"PCADimensionsKept");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->PCADimensionsKept = j->int_value; }
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"padEnsembleInput");
   if ((j!=0) && (j->type==NX_JSON_INTEGER)) { out->padEnsembleInput = j->int_value; }
   //-------------------------------------------------------------------
   //-------------------------------------------------------------------
   //-------------------------------------------------------------------
   fprintf(stderr,"Parsed Initial Variables...\n");

   //Now to parse Normalization elements..
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"NormalizeNSDMBasedOn");
   if (j!=0) { 
               out->numberOfNormalizedBasedOnRules = j->length; 
               if (out->numberOfNormalizedBasedOnRules>3)
               {
                 fprintf(stderr,"Maximum Accepted Normalization rules are 3!");
                 out->numberOfNormalizedBasedOnRules = 3;
               }
               
               for (int i=0; i<out->numberOfNormalizedBasedOnRules; i++)
               {
                   rule = nx_json_item(j,i);
                   //---------------------------------------------------------------------------------------------------
                   item  = nx_json_get(rule,"jointStart");
                   if ((item!=0) && (item->type==NX_JSON_STRING))  { snprintf(out->normalizedBasedOn[i].jointStart,MAX_JOINT_NAME,"%s",item->text_value); }
                   item  = nx_json_get(rule,"jointStartID");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->normalizedBasedOn[i].jointStartID = item->int_value; }
                   item  = nx_json_get(rule,"jointEnd"); 
                   if ((item!=0) && (item->type==NX_JSON_STRING))  { snprintf(out->normalizedBasedOn[i].jointEnd,MAX_JOINT_NAME,"%s",item->text_value); }
                   item  = nx_json_get(rule,"jointEndID");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->normalizedBasedOn[i].jointEndID = item->int_value; }
               }
             }
   //-------------------------------------------------------------------
   
   
   //Now to parse Alignment elements..
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"Alignment");
   if (j!=0) { 
               out->numberOfAlignmentRules = j->length; 
               if (out->numberOfAlignmentRules>3)
               {
                 fprintf(stderr,"Maximum Accepted Alignment rules are 3!");
                 out->numberOfAlignmentRules = 3;
               }
               
               for (int i=0; i<out->numberOfAlignmentRules; i++)
               {
                   rule = nx_json_item(j,i);
                   //---------------------------------------------------------------------------------------------------
                   item  = nx_json_get(rule,"jointStart"); 
                   if ((item!=0) && (item->type==NX_JSON_STRING))  { snprintf(out->alignment[i].jointStart,MAX_JOINT_NAME,"%s",item->text_value); }
                   item  = nx_json_get(rule,"jointStartID");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->alignment[i].jointStartID = item->int_value; }
                   item  = nx_json_get(rule,"jointEnd"); 
                   if ((item!=0) && (item->type==NX_JSON_STRING))  { snprintf(out->alignment[i].jointEnd,MAX_JOINT_NAME,"%s",item->text_value); }
                   item  = nx_json_get(rule,"jointEndID");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->alignment[i].jointEndID = item->int_value; }
               }
             }
   //-------------------------------------------------------------------
   
   
   //Now to parse NSxM elements..
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"NSDM");
   if (j!=0) { 
               out->numberOfDescriptorElements = j->length; 
               if (out->numberOfDescriptorElements>MAX_DESCRIPTOR_ELEMENTS)
               {
                 fprintf(stderr,"Maximum Accepted NSxM rules are %u!",MAX_DESCRIPTOR_ELEMENTS);
                 out->numberOfDescriptorElements = MAX_DESCRIPTOR_ELEMENTS;
               }
               
               for (int i=0; i<out->numberOfDescriptorElements; i++)
               {
                   rule = nx_json_item(j,i);
                   //---------------------------------------------------------------------------------------------------
                   item  = nx_json_get(rule,"joint"); 
                   if ((item!=0) && (item->type==NX_JSON_STRING)) { snprintf(out->descriptorElements[i].joint,MAX_JOINT_NAME,"%s",item->text_value); }
                   item  = nx_json_get(rule,"jointID");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->descriptorElements[i].jointID = item->int_value; }
                   item  = nx_json_get(rule,"isVirtual");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->descriptorElements[i].isVirtual = item->int_value; }
                   item  = nx_json_get(rule,"xOffset");
                   if ((item!=0) && (item->type==NX_JSON_DOUBLE)) { out->descriptorElements[i].xOffset = (float) item->dbl_value; }
                   item  = nx_json_get(rule,"yOffset");
                   if ((item!=0) && (item->type==NX_JSON_DOUBLE)) { out->descriptorElements[i].yOffset = (float) item->dbl_value; }
                   item  = nx_json_get(rule,"halfWayFromThisAnd"); 
                   if ((item!=0) && (item->type==NX_JSON_STRING)) { snprintf(out->descriptorElements[i].halfwayFromJointAndThis,MAX_JOINT_NAME,"%s",item->text_value); }
                   item  = nx_json_get(rule,"secondTargetJointID");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->descriptorElements[i].secondTargetJointID = item->int_value; }
               }
             }
   //-------------------------------------------------------------------



   
   //Now to parse Hierarchy elements..
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"hierarchy");
   if (j!=0) { 
               out->numberOfHierarchyElements = j->length; 
               if (out->numberOfHierarchyElements>MAX_HIERARCHY_ELEMENTS)
               {
                 fprintf(stderr,"Maximum Accepted Hierarchy rules are %u!",MAX_HIERARCHY_ELEMENTS);
                 out->numberOfHierarchyElements = MAX_HIERARCHY_ELEMENTS;
               }
               
               for (int i=0; i<out->numberOfHierarchyElements; i++)
               {
                   rule = nx_json_item(j,i);
                   //---------------------------------------------------------------------------------------------------
                   item  = nx_json_get(rule,"joint"); 
                   if ((item!=0) && (item->type==NX_JSON_STRING)) { snprintf(out->hierarchyElements[i].joint,MAX_JOINT_NAME,"%s",item->text_value); }
                   item  = nx_json_get(rule,"inheritNetwork"); 
                   if ((item!=0) && (item->type==NX_JSON_STRING)) { snprintf(out->hierarchyElements[i].inheritNetwork,MAX_JOINT_NAME,"%s",item->text_value); }
                   item  = nx_json_get(rule,"parent"); 
                   if ((item!=0) && (item->type==NX_JSON_STRING)) { snprintf(out->hierarchyElements[i].parent,MAX_JOINT_NAME,"%s",item->text_value); }
                   item  = nx_json_get(rule,"parentID");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->hierarchyElements[i].parentID = item->int_value; }
                   item  = nx_json_get(rule,"importance");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->hierarchyElements[i].importance = item->int_value; }
                   item  = nx_json_get(rule,"immuneToSelfOcclusions");
                   if ((item!=0) && (item->type==NX_JSON_INTEGER)) { out->hierarchyElements[i].immuneToSelfOcclusions = item->int_value; }
               }
             }
   //-------------------------------------------------------------------



   
   //Now to parse Hierarchy elements..
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"banned");
   if (j!=0) { 
               out->numberOfBannedJoints = j->length; 
               if (out->numberOfBannedJoints>MAX_BANNED_ELEMENTS)
               {
                 fprintf(stderr,"Maximum Accepted Banned rules are %u!",MAX_BANNED_ELEMENTS);
                 out->numberOfBannedJoints = MAX_BANNED_ELEMENTS;
               }
               
               for (int i=0; i<out->numberOfBannedJoints; i++)
               {
                   rule = nx_json_item(j,i);
                   //---------------------------------------------------------------------------------------------------
                   item  = nx_json_get(rule,"output"); 
                   if ((item!=0) && (item->type==NX_JSON_STRING)) { snprintf(out->bannedJoints[i].joint,MAX_JOINT_NAME,"%s",item->text_value); }
               }
             }
   //-------------------------------------------------------------------

   return resolveConfigurationData(out);
  }
 return 0;
}







static int getCompositePoint(
                             float * iXOut,
                             float * iYOut,
                             float * iVisibilityOut,
                             int * invalidPointOut,
                             struct ModelConfigurationData* rules,
                             int i,
                             float * points2D,
                             unsigned int points2DLength
                            )
{
   if (i > rules->numberOfDescriptorElements)
   {
       fprintf(stderr,"Point %u out of bounds for descriptor elements\n",i);
       return 0;
   }
   
   int invalidPoint      = 0;
   int numberOfNSDMRules = rules->numberOfDescriptorElements; 
   int iJointID          = rules->descriptorElements[i].jointID;
   
   if (iJointID*3 > points2DLength)
   {
       fprintf(stderr,"Unable to get composite point %u\n",i);
       return 0;
   }
   
   float iX              = points2D[iJointID*3+0];
   float iY              = points2D[iJointID*3+1];
   float iVisibility     = points2D[iJointID*3+2];
   //---------------------------------------------------------------------------
   //In case we fall through..
   *iXOut           = iX;
   *iYOut           = iY;
   *iVisibilityOut  = iVisibility;
   *invalidPointOut = invalidPoint;
   //---------------------------------------------------------------------------
   
   
   if ((iX!=0) || (iY!=0))
   {
         //---------------------------------------------------------------------------
         //                          Synthetic Points
         //---------------------------------------------------------------------------
         
         if (rules->descriptorElements[i].isVirtual==1)
         {
             
                    iX=iX+rules->descriptorElements[i].xOffset;
                    iY=iY+rules->descriptorElements[i].yOffset;
         }
         else
         if (rules->descriptorElements[i].isVirtual==2)
         {
                    int secondTargetJointID = rules->descriptorElements[i].secondTargetJointID;// rules['NSDM'][i]['secondTargetJointID']
                    float secondTargetX     = points2D[secondTargetJointID*3+0];
                    float secondTargetY     = points2D[secondTargetJointID*3+1];
                    if ((secondTargetX==0) && (secondTargetY==0))
                    {
                            invalidPoint=1;
                            iX=0;
                            iY=0;
                    }
                    else
                    {
                            iX=(float) (iX+secondTargetX)/2;
                            iY=(float) (iY+secondTargetY)/2;    
                    }
         }
         //---------------------------------------------------------------------------
      
      //Return values..
      *iXOut           = iX;
      *iYOut           = iY;
      *iVisibilityOut  = iVisibility;
      *invalidPointOut = invalidPoint;
      return 1; 
   }
 return 0;
}












 
#ifdef __cplusplus
}
#endif




#endif  
