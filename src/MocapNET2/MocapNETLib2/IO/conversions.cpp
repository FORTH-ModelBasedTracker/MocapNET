#include "conversions.hpp"

#include <string.h>
#include <vector>
#include "../tools.hpp"
#include "bvh.hpp"

#include "csvRead.hpp"
#include "../../../../dependencies/RGBDAcquisition/tools/AmMatrix/matrix4x4Tools.h"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


int appendVectorToFile(const char * filename, std::vector<float> vec)
{
  FILE * fp = fopen(filename,"a");
  if (fp!=0)
  {
      for (unsigned int i=0; i<vec.size(); i++)
      {
        if (i!=0) { fprintf(fp,","); }
        fprintf(fp,"%f",vec[i]); 
      }
      fprintf(fp,"\n");
      fclose(fp);
      return 1;
  }
  return 0;
}


int initializeAssociationsForSubsetOfSkeleton(unsigned int * targetIndexIsInitializedFlag,unsigned int * targetIndexes,unsigned int targetLength,const char * * targetLabels,struct skeletonSerialized * input)
{
    //int i=0,z=0;
    
    for (unsigned int i=0; i<targetLength; i++)
    {
      targetIndexes[i]=666;       
    }
    
    
    char targetLabelLowercase[513]={0};
    for (unsigned int i=0; i<targetLength; i++)
    {
       convertStringToLowercase(targetLabelLowercase,512,targetLabels[i]);
       
       for (unsigned int z=0; z<input->skeletonHeaderElements; z++)
        {
         if (input->skeletonHeader[z].str!=0)
         {
          if (strcmp(input->skeletonHeader[z].str,targetLabelLowercase)==0)
          {
             if (targetIndexes[i]==666)
             {
               targetIndexes[i]=z;
               fprintf(stderr,GREEN "%s==%s\n" NORMAL,input->skeletonHeader[z].str,targetLabelLowercase);
             } else
             {
               fprintf(stderr,YELLOW "%s==%s multiple times, only keeping first association..\n" NORMAL,input->skeletonHeader[z].str,targetLabelLowercase);  
             }  
          } //We have a matching string.. 
         } //There is a string to compare
        }
    }
    
    unsigned int unassociatedJoints=0;
    for (unsigned int i=0; i<targetLength; i++)
    {
       if ( targetIndexes[i]==666 ) { 
                                      ++unassociatedJoints; 
                                      fprintf(stderr,RED "Joint %s(%u) is not associated\n" NORMAL,targetLabels[i],i); 
                                    } else
                                    { fprintf(stderr,GREEN "Joint %s(%u) associated\n" NORMAL,targetLabels[i],i); } 
    }

    *targetIndexIsInitializedFlag=(unassociatedJoints==0);
    
    return (unassociatedJoints==0);
}











std::vector<float> deriveMocapNET2InputUsingAssociations(struct MocapNET2 * mnet,struct skeletonSerialized * input,unsigned int * targetIndexIsInitializedFlag,unsigned int * targetIndexes,unsigned int targetLength,const char * * targetLabels,int verbose)
{
    std::vector<float> result; 
    
    if (mnet->indexesPopulated)
    {
        
      if (! *targetIndexIsInitializedFlag)
      {
         fprintf(stderr,RED "deriveMocapNET2InputUsingAssociations called without initialization\n" NORMAL);  
          
         initializeAssociationsForSubsetOfSkeleton(
                                                     targetIndexIsInitializedFlag,
                                                     targetIndexes,
                                                     targetLength,
                                                     targetLabels,
                                                     input
                                                   );

      }
        
      for (int i=0; i<targetLength; i++)
       {
          if (verbose) { fprintf(stderr,"%s = %0.2f -> %s \n" , input->skeletonHeader[targetIndexes[i]].str , input->skeletonBody[targetIndexes[i]].value ,targetLabels[i]); } 
         result.push_back(input->skeletonBody[targetIndexes[i]].value);
       }
    } else
    {
      fprintf(stderr,RED "initializeAssociationsForSubsetOfSkeleton has to be run at initialization before deriveMocapNET2InputUsingAssociations\n");  
    }
    
    return result;
}

 
int convertSkeletons2DDetectedToSkeletonsSerialized( 
                                                     struct skeletonSerialized * output,
                                                     struct Skeletons2DDetected * input,
                                                     unsigned int frameNumber,
                                                     unsigned int width,
                                                     unsigned int height
                                                    )
{ 
    int check;
    if (output->skeletonHeaderElements==0)
    {
     //Brand new header that needs to be populated..!
     int elementsPopulated=3;
     output->skeletonHeader[0].strLength = 12; 
     output->skeletonHeader[0].str=(char *) malloc(sizeof(char) * output->skeletonHeader[0].strLength);
     snprintf(output->skeletonHeader[0].str,output->skeletonHeader[0].strLength,"frameNumber");

     output->skeletonHeader[1].strLength = 11; 
     output->skeletonHeader[1].str=(char *) malloc(sizeof(char) * output->skeletonHeader[1].strLength);
     snprintf(output->skeletonHeader[1].str,output->skeletonHeader[1].strLength,"skeletonID");

     output->skeletonHeader[2].strLength = 15; 
     output->skeletonHeader[2].str=(char *) malloc(sizeof(char) * output->skeletonHeader[2].strLength);
     snprintf(output->skeletonHeader[2].str,output->skeletonHeader[2].strLength,"totalSkeletons"); 
        
     char lowercaseName[512]={0};
     
     for (int i=0; i<BODY25_PARTS; i++)
                {
                    convertStringToLowercase(lowercaseName,512,Body25BodyNames[i]);
                    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+5;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "2dx_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }       
                    ++elementsPopulated;            
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+5;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "2dy_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }
                    ++elementsPopulated;             
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+9;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "visible_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }     
                    ++elementsPopulated;
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                }
            
            for (int i=0; i<COCO_HAND_PARTS; i++)
                { 
                    convertStringToLowercase(lowercaseName,512,COCOLeftHandNames[i]);
                    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+5;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "2dx_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }       
                    ++elementsPopulated;            
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+5;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "2dy_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }
                    ++elementsPopulated;             
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+9;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "visible_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }     
                    ++elementsPopulated;
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                }
                
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    convertStringToLowercase(lowercaseName,512,COCORightHandNames[i]);
                    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+5;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "2dx_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }       
                    ++elementsPopulated;            
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+5;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "2dy_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }
                    ++elementsPopulated;             
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+9;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "visible_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }     
                    ++elementsPopulated;
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                }
                
            for (int i=0; i<OP_HEAD_PARTS; i++)
                {
                    convertStringToLowercase(lowercaseName,512,HeadNames[i]);
                    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+5;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "2dx_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }       
                    ++elementsPopulated;            
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+5;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "2dy_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }
                    ++elementsPopulated;             
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                    output->skeletonHeader[elementsPopulated].strLength= strlen(lowercaseName)+9;
                    output->skeletonHeader[elementsPopulated].str = ( char* ) malloc( sizeof(char) * output->skeletonHeader[elementsPopulated].strLength);
                    if (output->skeletonHeader[elementsPopulated].str!=0)
                    {
                    check = snprintf( 
                                                         output->skeletonHeader[elementsPopulated].str ,
                                                         output->skeletonHeader[elementsPopulated].strLength,
                                                         "visible_%s",
                                                         lowercaseName
                                                       );
                    if (check<0) {  return 0; }        
                    } else { return 0; }     
                    ++elementsPopulated;
                    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                }
        output->skeletonHeaderElements = elementsPopulated;
    }
    // Reduce spam
    // else
    //{
    //    fprintf(stderr,"SkeletonSerialized appears to be already populated, skipping re allocating everything..\n");
    //}
    
    
    //=================================================================================
    //=================================================================================
    //=================================================================================
    //=================================================================================
    //=================================================================================
    //                                                                Now to convert the values.. 
    //=================================================================================
    //=================================================================================
    //=================================================================================
    //=================================================================================
     //=================================================================================
     
      output->skeletonBodyElements = output->skeletonHeaderElements;
      
      //Points will be normalized using normalize2DPointWhileAlsoMatchingTrainingAspectRatio
      output->width = 1.0;
      output->height = 1.0;   
      
     for (unsigned int skID=0; skID<input->numberOfSkeletonsDetected; skID++)
     { 
        int bodyElementsPopulated=3; 
       output->skeletonBody[0].value = frameNumber;
       output->skeletonBody[1].value = skID;
       output->skeletonBody[2].value = input->numberOfSkeletonsDetected;    
       
        for (int i=0; i<BODY25_PARTS; i++)
                {
                    //Normalize targets --------------------------------------------------------------
                    normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                                                                         &input->skeletons[skID].body.joint2D[i].x,
                                                                         &input->skeletons[skID].body.joint2D[i].y, 
                                                                         width,
                                                                         height,
                                                                         1,//Respect training aspect ratio
                                                                         MocapNETTrainingWidth,
                                                                         MocapNETTrainingHeight
                                                                       );
                    //--------------------------------------------------------------------------------
                    output->skeletonBody[bodyElementsPopulated].value = (float) input->skeletons[skID].body.joint2D[i].x;
                    ++bodyElementsPopulated;
                    
                    output->skeletonBody[bodyElementsPopulated].value = (float) input->skeletons[skID].body.joint2D[i].y;
                    ++bodyElementsPopulated;
                    
                    output->skeletonBody[bodyElementsPopulated].value = ( (input->skeletons[skID].body.joint2D[i].x!=0) && (input->skeletons[skID].body.joint2D[i].y!=0) ) ;
                    /*
                    if (!output->skeletonBody[bodyElementsPopulated].value)
                    {
                        fprintf(stderr,"%s not populated.. \n" ,Body25BodyNames[i]);
                    }*/
                    ++bodyElementsPopulated;
                    //--------------------------------------------------------------------------------
                }
                    
             
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    //Normalize targets --------------------------------------------------------------
                    normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                                                                         &input->skeletons[skID].leftHand.joint2D[i].x,
                                                                         &input->skeletons[skID].leftHand.joint2D[i].y,
                                                                         width,
                                                                         height,
                                                                         1,//Respect training aspect ratio
                                                                         MocapNETTrainingWidth,
                                                                         MocapNETTrainingHeight
                                                                       );
                    //--------------------------------------------------------------------------------
                     output->skeletonBody[bodyElementsPopulated].value = (float) input->skeletons[skID].leftHand.joint2D[i].x;
                    ++bodyElementsPopulated;
                     
                     output->skeletonBody[bodyElementsPopulated].value = (float) input->skeletons[skID].leftHand.joint2D[i].y;
                    ++bodyElementsPopulated;
                     
                     output->skeletonBody[bodyElementsPopulated].value =( (input->skeletons[skID].leftHand.joint2D[i].x!=0) && (input->skeletons[skID].leftHand.joint2D[i].y!=0) ) ;
                    ++bodyElementsPopulated;
                    //--------------------------------------------------------------------------------
                }
                
                
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    //Normalize targets --------------------------------------------------------------
                    normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                                                                         &input->skeletons[skID].rightHand.joint2D[i].x,
                                                                         &input->skeletons[skID].rightHand.joint2D[i].y,
                                                                         width,
                                                                         height,
                                                                         1,//Respect training aspect ratio
                                                                         MocapNETTrainingWidth,
                                                                         MocapNETTrainingHeight
                                                                       );
                    //--------------------------------------------------------------------------------
                    output->skeletonBody[bodyElementsPopulated].value = input->skeletons[skID].rightHand.joint2D[i].x;
                    ++bodyElementsPopulated;
                    
                    output->skeletonBody[bodyElementsPopulated].value = input->skeletons[skID].rightHand.joint2D[i].y;
                    ++bodyElementsPopulated;
                    
                    output->skeletonBody[bodyElementsPopulated].value =  ( (input->skeletons[skID].rightHand.joint2D[i].x!=0) && (input->skeletons[skID].rightHand.joint2D[i].y!=0) );
                    ++bodyElementsPopulated;
                    //--------------------------------------------------------------------------------
                }
                
            for (int i=0; i<OP_HEAD_PARTS; i++)
                {
                    //Normalize targets --------------------------------------------------------------
                    normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                                                                         &input->skeletons[skID].head.joint2D[i].x,
                                                                         &input->skeletons[skID].head.joint2D[i].y,
                                                                         width,
                                                                         height,
                                                                         1,//Respect training aspect ratio
                                                                         MocapNETTrainingWidth,
                                                                         MocapNETTrainingHeight
                                                                       );
                    //--------------------------------------------------------------------------------
                     output->skeletonBody[bodyElementsPopulated].value = (float) input->skeletons[skID].head.joint2D[i].x;
                    ++bodyElementsPopulated;
                    
                     output->skeletonBody[bodyElementsPopulated].value = (float) input->skeletons[skID].head.joint2D[i].y;
                    ++bodyElementsPopulated;
                    
                     output->skeletonBody[bodyElementsPopulated].value =  ( (input->skeletons[skID].head.joint2D[i].x!=0) && (input->skeletons[skID].head.joint2D[i].y!=0) ); 
                    ++bodyElementsPopulated;
                }            
                
     }
    
    
    return 1;
}





int convertMocapNET2OutputToSkeletonSerialized(
                                                struct MocapNET2 * mnet, 
                                                struct skeletonSerialized * output,
                                                std::vector<std::vector<float> > mocapNET2DPointsResult,
                                                unsigned int frameNumber,
                                                unsigned int width,
                                                unsigned int height
                                              )
{
    if (mocapNET2DPointsResult.size()==0)
    {
        fprintf(stderr,"convertMocapNET2OutputToSkeletonSerialized cannot work without 3D point results\n");
        return 0;
    }
    struct Skeletons2DDetected input={0};
    
    input.numberOfSkeletonsDetected=1;
    input.skeletons[0].body.joint2D[BODY25_Nose].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS05][0];
    input.skeletons[0].body.joint2D[BODY25_Nose].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS05][1];
    
    input.skeletons[0].body.joint2D[BODY25_Neck].x = mocapNET2DPointsResult[MOCAPNET_JOINT_NECK][0];
    input.skeletons[0].body.joint2D[BODY25_Neck].y = mocapNET2DPointsResult[MOCAPNET_JOINT_NECK][1];
    
    input.skeletons[0].body.joint2D[BODY25_RShoulder].x = mocapNET2DPointsResult[MOCAPNET_JOINT_RSHOULDER][0];
    input.skeletons[0].body.joint2D[BODY25_RShoulder].y = mocapNET2DPointsResult[MOCAPNET_JOINT_RSHOULDER][1];
    
    input.skeletons[0].body.joint2D[BODY25_RElbow].x = mocapNET2DPointsResult[MOCAPNET_JOINT_RELBOW][0];
    input.skeletons[0].body.joint2D[BODY25_RElbow].y = mocapNET2DPointsResult[MOCAPNET_JOINT_RELBOW][1];
    
    input.skeletons[0].body.joint2D[BODY25_RWrist].x = mocapNET2DPointsResult[MOCAPNET_JOINT_RHAND][0];
    input.skeletons[0].body.joint2D[BODY25_RWrist].y = mocapNET2DPointsResult[MOCAPNET_JOINT_RHAND][1];
    
    input.skeletons[0].body.joint2D[BODY25_LShoulder].x = mocapNET2DPointsResult[MOCAPNET_JOINT_LSHOULDER][0];
    input.skeletons[0].body.joint2D[BODY25_LShoulder].y = mocapNET2DPointsResult[MOCAPNET_JOINT_LSHOULDER][1];
    
    input.skeletons[0].body.joint2D[BODY25_LElbow].x = mocapNET2DPointsResult[MOCAPNET_JOINT_LELBOW][0];
    input.skeletons[0].body.joint2D[BODY25_LElbow].y = mocapNET2DPointsResult[MOCAPNET_JOINT_LELBOW][1];
    
    input.skeletons[0].body.joint2D[BODY25_LWrist].x = mocapNET2DPointsResult[MOCAPNET_JOINT_LHAND][0];
    input.skeletons[0].body.joint2D[BODY25_LWrist].y = mocapNET2DPointsResult[MOCAPNET_JOINT_LHAND][1];
    
    input.skeletons[0].body.joint2D[BODY25_MidHip].x = mocapNET2DPointsResult[MOCAPNET_JOINT_HIP][0];
    input.skeletons[0].body.joint2D[BODY25_MidHip].y = mocapNET2DPointsResult[MOCAPNET_JOINT_HIP][1];
     
    input.skeletons[0].body.joint2D[BODY25_RHip].x = mocapNET2DPointsResult[MOCAPNET_JOINT_RHIP][0];
    input.skeletons[0].body.joint2D[BODY25_RHip].y = mocapNET2DPointsResult[MOCAPNET_JOINT_RHIP][1];
    
    input.skeletons[0].body.joint2D[BODY25_RKnee].x = mocapNET2DPointsResult[MOCAPNET_JOINT_RKNEE][0];
    input.skeletons[0].body.joint2D[BODY25_RKnee].y = mocapNET2DPointsResult[MOCAPNET_JOINT_RKNEE][1];
    
    input.skeletons[0].body.joint2D[BODY25_RAnkle].x = mocapNET2DPointsResult[MOCAPNET_JOINT_RFOOT][0];
    input.skeletons[0].body.joint2D[BODY25_RAnkle].y = mocapNET2DPointsResult[MOCAPNET_JOINT_RFOOT][1];
    
    input.skeletons[0].body.joint2D[BODY25_LHip].x = mocapNET2DPointsResult[MOCAPNET_JOINT_LHIP][0];
    input.skeletons[0].body.joint2D[BODY25_LHip].y = mocapNET2DPointsResult[MOCAPNET_JOINT_LHIP][1];
    
    input.skeletons[0].body.joint2D[BODY25_LKnee].x = mocapNET2DPointsResult[MOCAPNET_JOINT_LKNEE][0];
    input.skeletons[0].body.joint2D[BODY25_LKnee].y = mocapNET2DPointsResult[MOCAPNET_JOINT_LKNEE][1];
    
    input.skeletons[0].body.joint2D[BODY25_LAnkle].x = mocapNET2DPointsResult[MOCAPNET_JOINT_LFOOT][0];
    input.skeletons[0].body.joint2D[BODY25_LAnkle].y = mocapNET2DPointsResult[MOCAPNET_JOINT_LFOOT][1];
    
    input.skeletons[0].body.joint2D[BODY25_REye].x = mocapNET2DPointsResult[MOCAPNET_JOINT_EYE_R][0];
    input.skeletons[0].body.joint2D[BODY25_REye].y = mocapNET2DPointsResult[MOCAPNET_JOINT_EYE_R][1];
    
    input.skeletons[0].body.joint2D[BODY25_LEye].x = mocapNET2DPointsResult[MOCAPNET_JOINT_EYE_L][0];
    input.skeletons[0].body.joint2D[BODY25_LEye].y = mocapNET2DPointsResult[MOCAPNET_JOINT_EYE_L][1];
    
    input.skeletons[0].body.joint2D[BODY25_REar].x = mocapNET2DPointsResult[MOCAPNET_JOINT_TEMPORALIS02_R][0];
    input.skeletons[0].body.joint2D[BODY25_REar].y = mocapNET2DPointsResult[MOCAPNET_JOINT_TEMPORALIS02_R][1];
    input.skeletons[0].body.joint2D[BODY25_LEar].x = mocapNET2DPointsResult[MOCAPNET_JOINT_TEMPORALIS02_L][0];
    input.skeletons[0].body.joint2D[BODY25_LEar].y = mocapNET2DPointsResult[MOCAPNET_JOINT_TEMPORALIS02_L][1];
    
    input.skeletons[0].body.joint2D[BODY25_LBigToe].x   = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_TOE1_2_L][0];
    input.skeletons[0].body.joint2D[BODY25_LBigToe].y   = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_TOE1_2_L][1];
    input.skeletons[0].body.joint2D[BODY25_LSmallToe].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_TOE5_3_L][0];
    input.skeletons[0].body.joint2D[BODY25_LSmallToe].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_TOE5_3_L][1];
    input.skeletons[0].body.joint2D[BODY25_LHeel].x     = mocapNET2DPointsResult[MOCAPNET_JOINT_LFOOT][0];
    input.skeletons[0].body.joint2D[BODY25_LHeel].y     = mocapNET2DPointsResult[MOCAPNET_JOINT_LFOOT][1];
    
    
    input.skeletons[0].body.joint2D[BODY25_RBigToe].x   = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_TOE1_2_R][0];
    input.skeletons[0].body.joint2D[BODY25_RBigToe].y   = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_TOE1_2_R][1];
    input.skeletons[0].body.joint2D[BODY25_RSmallToe].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_TOE5_3_R][0];
    input.skeletons[0].body.joint2D[BODY25_RSmallToe].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_TOE5_3_R][1];
    input.skeletons[0].body.joint2D[BODY25_RHeel].x     = mocapNET2DPointsResult[MOCAPNET_JOINT_RFOOT][0];
    input.skeletons[0].body.joint2D[BODY25_RHeel].y     = mocapNET2DPointsResult[MOCAPNET_JOINT_RFOOT][1];
    //Dont forget to update convertBodySkeletonSerializedToBVHTransform
  
 
    //Populate right hand
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Wrist].x = mocapNET2DPointsResult[MOCAPNET_JOINT_RHAND][0];                 //MOCAPNET_OUTPUT_JOINT_RHAND
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Wrist].y = mocapNET2DPointsResult[MOCAPNET_JOINT_RHAND][1];                 //MOCAPNET_OUTPUT_JOINT_RHAND
    //---------------------------------------------------------------------------------
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Thumb_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_RTHUMB][0];              //MOCAPNET_OUTPUT_JOINT_RTHUMB
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Thumb_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_RTHUMB][1];              //MOCAPNET_OUTPUT_JOINT_RTHUMB
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Thumb_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER1_2_R][0];         //MOCAPNET_OUTPUT_JOINT_RTHUMB
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Thumb_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER1_2_R][1];         //MOCAPNET_OUTPUT_JOINT_RTHUMB
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Thumb_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER1_3_R][0];         //MOCAPNET_OUTPUT_JOINT_FINGER1_2_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Thumb_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER1_3_R][1];         //MOCAPNET_OUTPUT_JOINT_FINGER1_2_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Thumb_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER1_3_R][0]; //MOCAPNET_OUTPUT_JOINT_FINGER1_3_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Thumb_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER1_3_R][1]; //MOCAPNET_OUTPUT_JOINT_FINGER1_3_R
    //---------------------------------------------------------------------------------
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Index_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_1_R][0];         //MOCAPNET_OUTPUT_JOINT_METACARPAL1_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Index_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_1_R][1];         //MOCAPNET_OUTPUT_JOINT_METACARPAL1_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Index_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_2_R][0];         //MOCAPNET_OUTPUT_JOINT_FINGER2_1_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Index_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_2_R][1];         //MOCAPNET_OUTPUT_JOINT_FINGER2_1_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Index_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_3_R][0];         //MOCAPNET_OUTPUT_JOINT_FINGER2_2_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Index_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_3_R][1];         //MOCAPNET_OUTPUT_JOINT_FINGER2_2_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Index_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER2_3_R][0]; //MOCAPNET_OUTPUT_JOINT_FINGER2_3_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Index_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER2_3_R][1]; //MOCAPNET_OUTPUT_JOINT_FINGER2_3_R
    //---------------------------------------------------------------------------------
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Middle_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_1_R][0];//MOCAPNET_OUTPUT_JOINT_METACARPAL2_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Middle_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_1_R][1];
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Middle_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_2_R][0];  //MOCAPNET_OUTPUT_JOINT_FINGER3_1_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Middle_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_2_R][1];
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Middle_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_3_R][0];  //MOCAPNET_OUTPUT_JOINT_FINGER3_2_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Middle_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_3_R][1];
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Middle_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER3_3_R][0];  //MOCAPNET_OUTPUT_JOINT_FINGER3_3_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Middle_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER3_3_R][1];  //MOCAPNET_OUTPUT_JOINT_FINGER3_3_R
    //---------------------------------------------------------------------------------
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Ring_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_1_R][0]; //MOCAPNET_OUTPUT_JOINT_METACARPAL3_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Ring_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_1_R][1];
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Ring_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_2_R][0];   //MOCAPNET_OUTPUT_JOINT_FINGER4_1_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Ring_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_2_R][1];
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Ring_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_3_R][0];   //MOCAPNET_OUTPUT_JOINT_FINGER4_2_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Ring_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_3_R][1];
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Ring_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER4_3_R][0];   //MOCAPNET_OUTPUT_JOINT_FINGER4_3_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Ring_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER4_3_R][1];   //MOCAPNET_OUTPUT_JOINT_FINGER4_3_R
    //---------------------------------------------------------------------------------
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Pinky_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_1_R][0]; //MOCAPNET_OUTPUT_JOINT_METACARPAL4_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Pinky_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_1_R][1];
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Pinky_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_2_R][0];   //MOCAPNET_OUTPUT_JOINT_FINGER5_1_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Pinky_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_2_R][1];
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Pinky_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_3_R][0];   //MOCAPNET_OUTPUT_JOINT_FINGER5_2_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Pinky_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_3_R][1];
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Pinky_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER5_3_R][0];   //MOCAPNET_OUTPUT_JOINT_FINGER5_3_R
    input.skeletons[0].rightHand.joint2D[COCO_Hand_Pinky_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER5_3_R][1];   //MOCAPNET_OUTPUT_JOINT_FINGER5_3_R
    //---------------------------------------------------------------------------------
     //Dont forget convertRHandSkeletonSerializedToBVHTransform needs to also be updated

 
 
 
 
    //Populate left hand
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Wrist].x = mocapNET2DPointsResult[MOCAPNET_JOINT_LHAND][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Wrist].y = mocapNET2DPointsResult[MOCAPNET_JOINT_LHAND][1];
    //---------------------------------------------------------------------------------
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Thumb_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_LTHUMB][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Thumb_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_LTHUMB][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Thumb_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER1_2_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Thumb_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER1_2_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Thumb_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER1_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Thumb_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER1_3_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Thumb_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER1_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Thumb_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER1_3_L][1];
    //---------------------------------------------------------------------------------
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Index_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_1_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Index_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_1_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Index_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_2_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Index_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_2_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Index_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Index_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER2_3_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Index_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER2_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Index_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER2_3_L][1]; //Maybe the end site is prefereable?
    //---------------------------------------------------------------------------------
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Middle_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_1_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Middle_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_1_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Middle_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_2_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Middle_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_2_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Middle_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Middle_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER3_3_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Middle_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER3_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Middle_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER3_3_L][1]; //Maybe the end site is prefereable?
    //---------------------------------------------------------------------------------
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Ring_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_1_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Ring_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_1_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Ring_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_2_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Ring_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_2_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Ring_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Ring_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER4_3_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Ring_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER4_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Ring_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER4_3_L][1]; //Maybe the end site is prefereable?
    //---------------------------------------------------------------------------------
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Pinky_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_1_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Pinky_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_1_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Pinky_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_2_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Pinky_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_2_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Pinky_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Pinky_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_FINGER5_3_L][1];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Pinky_4].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER5_3_L][0];
    input.skeletons[0].leftHand.joint2D[COCO_Hand_Pinky_4].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ENDSITE_FINGER5_3_L][1]; //Maybe the end site is prefereable?
    //---------------------------------------------------------------------------------
     //Dont forget convertLHandSkeletonSerializedToBVHTransform needs to also be updated

 
 
 
    //Populate face 
    input.skeletons[0].head.joint2D[OP_Head_REye_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORBICULARIS03_R][0];
    input.skeletons[0].head.joint2D[OP_Head_REye_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORBICULARIS03_R][1];
    input.skeletons[0].head.joint2D[OP_Head_REye].x = mocapNET2DPointsResult[MOCAPNET_JOINT_EYE_R][0];
    input.skeletons[0].head.joint2D[OP_Head_REye].y = mocapNET2DPointsResult[MOCAPNET_JOINT_EYE_R][1];
    input.skeletons[0].head.joint2D[OP_Head_REye_5].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORBICULARIS04_R][0];
    input.skeletons[0].head.joint2D[OP_Head_REye_5].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORBICULARIS04_R][1];
    //---------------------------------------------------------------------------------
    input.skeletons[0].head.joint2D[OP_Head_LEye_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORBICULARIS03_L][0];
    input.skeletons[0].head.joint2D[OP_Head_LEye_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORBICULARIS03_L][1];
    input.skeletons[0].head.joint2D[OP_Head_LEye].x = mocapNET2DPointsResult[MOCAPNET_JOINT_EYE_L][0];
    input.skeletons[0].head.joint2D[OP_Head_LEye].y = mocapNET2DPointsResult[MOCAPNET_JOINT_EYE_L][1];
    input.skeletons[0].head.joint2D[OP_Head_LEye_5].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORBICULARIS04_L][0];
    input.skeletons[0].head.joint2D[OP_Head_LEye_5].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORBICULARIS04_L][1];
    //---------------------------------------------------------------------------------
    input.skeletons[0].head.joint2D[OP_Head_Chin].x = mocapNET2DPointsResult[MOCAPNET_JOINT_SPECIAL04][0];
    input.skeletons[0].head.joint2D[OP_Head_Chin].y = mocapNET2DPointsResult[MOCAPNET_JOINT_SPECIAL04][1];
    //---------------------------------------------------------------------------------
    input.skeletons[0].head.joint2D[OP_Head_InMouth_1].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS03_R][0];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_1].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS03_R][1];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_2].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS05][0];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_2].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS05][1];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_3].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS03_L][0];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_3].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS03_L][1];
    //---------------------------------------------------------------------------------
    input.skeletons[0].head.joint2D[OP_Head_InMouth_7].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS07_R][0];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_7].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS07_R][1];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_6].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS01][0];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_6].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS01][1];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_5].x = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS07_L][0];
    input.skeletons[0].head.joint2D[OP_Head_InMouth_5].y = mocapNET2DPointsResult[MOCAPNET_JOINT_ORIS07_L][1];
    //---------------------------------------------------------------------------------
    // Dont forget to update convertFaceSkeletonSerializedToBVHTransform
 
    /*
    for (int jID=0; jID<getBVHNumberOfJoints(); jID++)
    {
      const char * jointName = getBVHJointName(jID);
      fprintf(stderr,"%u = %s \n",jID,jointName);
         
    }*/
    
     return convertSkeletons2DDetectedToSkeletonsSerialized( 
                                                            output,
                                                            &input,
                                                            frameNumber,
                                                            width,
                                                            height
                                                           );
}








int affineSkeletonRotation(struct skeletonSerialized * input,float degrees)
{
    if (degrees==0.0) { return 1; }
    //---------------------------------------------------------------------------
    float rad = degrees_to_radF(degrees);
    //---------------------------------------------------------------------------
    
    unsigned int hipJoint = 1 + BODY25_MidHip; //+1 because the first three parameters are for skeleton ids etc. 
    float hipX = input->skeletonBody[hipJoint*3+0].value;
    float hipY = input->skeletonBody[hipJoint*3+1].value;
    char  hipVisibility = (input->skeletonBody[hipJoint*3+2].value > 0.0);
    
    if (hipVisibility)
    {
     if (input!=0)
      {
       for (unsigned int jID = 0; jID< input->skeletonBodyElements/3; jID++)
       {
         float x=input->skeletonBody[jID*3+0].value;
         float y=input->skeletonBody[jID*3+1].value;
         char  visibility=(char) (input->skeletonBody[jID*3+2].value > 0.0);
         
         if (visibility)
         {
           //Subtract HIP from every joint and perform a 2D rotation using the rad variable
           float unrotatedX = x-hipX;
           //Subtract HIP from every joint and perform a 2D rotation using the rad variable
           float unrotatedY = y-hipY;
           
           input->skeletonBody[jID*3+0].value = hipX + (unrotatedX * cosf(rad)) - (unrotatedY * sinf(rad));
           input->skeletonBody[jID*3+1].value = hipY + (unrotatedX * sinf(rad)) + (unrotatedY * cosf(rad));
         }
       }
      return 1;   
      }
    }
  return 0;
}




 
float  rotationRequiredToMakeSkeletonCloserToTrainingDataset(struct skeletonSerialized * input)
{
    //--------------------------------------------------------------------
    unsigned int hipJoint = 1 + BODY25_MidHip; //+1 because the first three parameters are for skeleton ids etc. 
    float hipX = input->skeletonBody[hipJoint*3+0].value;
    float hipY = input->skeletonBody[hipJoint*3+1].value;
    char  hipVisibility = (input->skeletonBody[hipJoint*3+2].value > 0.0);
    //--------------------------------------------------------------------
    unsigned int neckJoint = 1 + BODY25_Neck; //+1 because the first three parameters are for skeleton ids etc. 
    float neckX = input->skeletonBody[neckJoint*3+0].value;
    float neckY = input->skeletonBody[neckJoint*3+1].value;
    char  neckVisibility = (input->skeletonBody[neckJoint*3+2].value > 0.0);
    //--------------------------------------------------------------------
    
    if ( (hipVisibility) && (neckVisibility) )
    { 
      float alignmentAngle=getAngleToAlignToZero_tools(hipX,hipY,neckX,neckY);
      return alignmentAngle;
    }
    return 0.0;
}


int makeSkeletonUpright(struct skeletonSerialized * input)
{
    float angleCorrectionNeededInDegrees = 180 + goFromRadToDegrees * rotationRequiredToMakeSkeletonCloserToTrainingDataset(input);
    
    if (angleCorrectionNeededInDegrees!=180.0)
    {
     return affineSkeletonRotation(input,angleCorrectionNeededInDegrees);
    }
    
  return 0;
}
