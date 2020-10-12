#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string.h>

#include "../mocapnet2.hpp"
#include "../tools.hpp"
#include "csvWrite.hpp"


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
int writeCSVHeaderFromVector(const char * filename,const char ** labels,unsigned int numberOfLabels)
{ 
    FILE * fp = fopen(filename,"w");
    if (fp!=0)
        {
            for (int i=0; i<numberOfLabels; i++)
                {
                    fprintf(fp,"%s",labels[i]);
                    if (i<numberOfLabels-1)
                        {
                            fprintf(fp,",");
                        }
                }
            fprintf(fp,"\n");
            fclose(fp);
            return 1;
        }
    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
int writeCSVBodyFromVector(const char * filename,std::vector<float> inputValues)
{
    FILE * fp = fopen(filename,"a");
    if (fp!=0)
        { 
            if (inputValues.size()==0)
                {
                    fprintf(stderr,"Failed to read from JSON file..\n");
                }

            for (int i=0; i<inputValues.size(); i++)
                {
                    if (i%3==2)
                        {
                            fprintf(fp,"%0.1f",inputValues[i]);
                        }
                    else
                        {
                            fprintf(fp,"%f",inputValues[i]);
                        }

                    if (i<inputValues.size()-1)
                        {
                            fprintf(fp,",");
                        }
                }
            fprintf(fp,"\n");
            fclose(fp);
            return 1;
        }
    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
int writeCSVHeaderFromLabelsAndVectorOfVectors(const char * filename,const char ** labels,unsigned int numberOfLabels,std::vector<std::vector<float> > inputFrames)
{
  int totalWritesNeeded=0;  
  int totalWritesSucceeded=0;  
    
  if ( writeCSVHeaderFromVector(filename,labels,numberOfLabels) )
      {
        for (int i=0; i<inputFrames.size(); i++)
         {
           ++totalWritesNeeded;  
           if ( writeCSVBodyFromVector(filename,inputFrames[i]) )
           {
             ++totalWritesSucceeded;
           }
         }
        return (totalWritesSucceeded==totalWritesNeeded);
      }  
 return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------






int writeOpenPoseCSVHeaderFromSkeleton(const char * filename,struct skeletonStructure * skeleton,unsigned int width,unsigned int height)
{ 
    FILE * fp = fopen(filename,"w");
    if (fp!=0)
        { 
       
             fprintf(fp,"frameNumber,");
             fprintf(fp,"skeletonID,");
             fprintf(fp,"totalSkeletons,"); 
             
             char lowercaseName[512];
                    
            for (int i=0; i<BODY25_PARTS; i++)
                {
                    convertStringToLowercase(lowercaseName,512,Body25BodyNames[i]);
                    //----------------------------------
                    fprintf(fp,"2DX_%s,",lowercaseName);
                    fprintf(fp,"2DY_%s,",lowercaseName);
                    fprintf(fp,"visible_%s,",lowercaseName);
                }
            
            for (int i=0; i<COCO_HAND_PARTS; i++)
                { 
                    convertStringToLowercase(lowercaseName,512,COCOLeftHandNames[i]);
                    //----------------------------------
                    fprintf(fp,"2DX_%s,",lowercaseName);
                    fprintf(fp,"2DY_%s,",lowercaseName);
                    fprintf(fp,"visible_%s,",lowercaseName);
                }
                
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    convertStringToLowercase(lowercaseName,512,COCORightHandNames[i]);
                    //----------------------------------
                    fprintf(fp,"2DX_%s,",lowercaseName);
                    fprintf(fp,"2DY_%s,",lowercaseName);
                    fprintf(fp,"visible_%s,",lowercaseName);
                }
                
            for (int i=0; i<OP_HEAD_PARTS; i++)
                {
                    convertStringToLowercase(lowercaseName,512,HeadNames[i]);
                    //----------------------------------
                    fprintf(fp,"2DX_%s,",lowercaseName);
                    fprintf(fp,"2DY_%s,",lowercaseName);
                    fprintf(fp,"visible_%s",lowercaseName);
                    if (i<OP_HEAD_PARTS-1)
                        {
                           fprintf(fp,",");
                        } 
                }
                
            fprintf(fp,"\n");
            fclose(fp);
            return 1;
        }
    return 0;
}



int writeOpenPoseCSVBodyFromSkeleton(const char * filename,struct skeletonStructure * skeleton,unsigned int respectTrainingAspectRatio,unsigned int width,unsigned int height)
{ 
    FILE * fp = fopen(filename,"a");
    if (fp!=0)
        { 
             fprintf(fp,"%u,",skeleton->observationNumber);
             fprintf(fp,"%u,",skeleton->userID);
             fprintf(fp,"%u,",skeleton->totalUsersPresent);
                    
             
            
            
            for (int i=0; i<BODY25_PARTS; i++)
                {
                    //Normalize targets --------------------------------------------------------------
                    normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                                                                         &skeleton->body.joint2D[i].x,
                                                                         &skeleton->body.joint2D[i].y, 
                                                                         width,
                                                                         height,
                                                                         respectTrainingAspectRatio,
                                                                         MocapNETTrainingWidth,
                                                                         MocapNETTrainingHeight
                                                                       );
                    //--------------------------------------------------------------------------------
                    fprintf(fp,"%f,",(float) skeleton->body.joint2D[i].x);
                    fprintf(fp,"%f,",(float) skeleton->body.joint2D[i].y);
                    fprintf(fp,"%u,", ( (skeleton->body.joint2D[i].x!=0) && (skeleton->body.joint2D[i].y!=0) ) );
                }
            
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    //Normalize targets --------------------------------------------------------------
                    normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                                                                         &skeleton->leftHand.joint2D[i].x,
                                                                         &skeleton->leftHand.joint2D[i].y,
                                                                         width,
                                                                         height,
                                                                         respectTrainingAspectRatio,
                                                                         MocapNETTrainingWidth,
                                                                         MocapNETTrainingHeight
                                                                       );
                    //--------------------------------------------------------------------------------
                    fprintf(fp,"%f,",(float) skeleton->leftHand.joint2D[i].x);
                    fprintf(fp,"%f,",(float) skeleton->leftHand.joint2D[i].y);
                    fprintf(fp,"%u,", ( (skeleton->leftHand.joint2D[i].x!=0) && (skeleton->leftHand.joint2D[i].y!=0) ) );
                }
                
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    //Normalize targets --------------------------------------------------------------
                    normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                                                                         &skeleton->rightHand.joint2D[i].x,
                                                                         &skeleton->rightHand.joint2D[i].y,
                                                                         width,
                                                                         height,
                                                                         respectTrainingAspectRatio,
                                                                         MocapNETTrainingWidth,
                                                                         MocapNETTrainingHeight
                                                                       );
                    //--------------------------------------------------------------------------------
                    fprintf(fp,"%f,",(float) skeleton->rightHand.joint2D[i].x);
                    fprintf(fp,"%f,",(float) skeleton->rightHand.joint2D[i].y);
                    fprintf(fp,"%u,", ( (skeleton->rightHand.joint2D[i].x!=0) && (skeleton->rightHand.joint2D[i].y!=0) ) );
                }
                
            for (int i=0; i<OP_HEAD_PARTS; i++)
                {
                    //Normalize targets --------------------------------------------------------------
                    normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                                                                         &skeleton->head.joint2D[i].x,
                                                                         &skeleton->head.joint2D[i].y,
                                                                         width,
                                                                         height,
                                                                         respectTrainingAspectRatio,
                                                                         MocapNETTrainingWidth,
                                                                         MocapNETTrainingHeight
                                                                       );
                    //--------------------------------------------------------------------------------
                    fprintf(fp,"%f,",(float) skeleton->head.joint2D[i].x);
                    fprintf(fp,"%f,",(float) skeleton->head.joint2D[i].y);
                    fprintf(fp,"%u", ( (skeleton->head.joint2D[i].x!=0) && (skeleton->head.joint2D[i].y!=0) ) ); 
                    if (i<OP_HEAD_PARTS-1)
                        {
                           fprintf(fp,",");
                        } 
                }            
                
            fprintf(fp,"\n");
            fclose(fp);
            return 1;
        }
    return 0;
}

