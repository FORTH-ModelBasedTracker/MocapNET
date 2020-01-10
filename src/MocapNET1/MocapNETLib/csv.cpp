
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string.h>

#include "csv.hpp"
#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/jsonMocapNETHelpers.hpp"


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


















//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int writeCSVHeaderFromSkeleton(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height)
{
    std::vector<float> inputValues = flattenskeletonCOCOToVector(skeleton,width,height);

    FILE * fp = fopen(filename,"w");
    if (fp!=0)
        {
            for (int i=0; i<inputValues.size(); i++)
                {
                    fprintf(fp,"%s",MocapNETInputUncompressedArrayNames[i]);
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

int writeCSVBodyFromSkeleton(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height)
{
    std::vector<float> inputValues = flattenskeletonCOCOToVector(skeleton,width,height);
    return  writeCSVBodyFromVector(filename,inputValues);
    /*
    FILE * fp = fopen(filename,"a");
    if (fp!=0)
        {
            std::vector<float> inputValues = flattenskeletonCOCOToVector(skeleton,width,height);
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
        */
}
//----------------------------




int writeOpenPoseCSVHeaderFromSkeleton(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height)
{ 
    FILE * fp = fopen(filename,"w");
    if (fp!=0)
        { 
       
             fprintf(fp,"frameNumber,");
             fprintf(fp,"skeletonID,");
             fprintf(fp,"totalSkeletons,"); 
             
            for (int i=0; i<BODY25_PARTS; i++)
                {
                    fprintf(fp,"2D_X_%s,",Body25BodyNames[i]);
                    fprintf(fp,"2D_Y_%s,",Body25BodyNames[i]);
                    fprintf(fp,"2D_vis_%s,",Body25BodyNames[i]);
                }
            
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    fprintf(fp,"2D_X_%s,",COCOLeftHandNames[i]);
                    fprintf(fp,"2D_Y_%s,",COCOLeftHandNames[i]);
                    fprintf(fp,"2D_vis_%s,",COCOLeftHandNames[i]);
                }
                
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    fprintf(fp,"2D_X_%s,",COCORightHandNames[i]);
                    fprintf(fp,"2D_Y_%s,",COCORightHandNames[i]);
                    fprintf(fp,"2D_vis_%s,",COCORightHandNames[i]);
                }
                
            for (int i=0; i<OP_HEAD_PARTS; i++)
                {
                    fprintf(fp,"2D_X_%s,",HeadNames[i]);
                    fprintf(fp,"2D_Y_%s,",HeadNames[i]);
                    fprintf(fp,"2D_vis_%s",HeadNames[i]);
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



int writeOpenPoseCSVBodyFromSkeleton(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height)
{ 
    FILE * fp = fopen(filename,"a");
    if (fp!=0)
        { 
            
       
             fprintf(fp,"%u,",skeleton->observationNumber);
             fprintf(fp,"%u,",skeleton->userID);
             fprintf(fp,"%u,",skeleton->totalUsersPresent);
                    
            
            for (int i=0; i<BODY25_PARTS; i++)
                {
                    fprintf(fp,"%f,",(float) skeleton->body.joint2D[i].x/width);
                    fprintf(fp,"%f,",(float) skeleton->body.joint2D[i].y/height);
                    fprintf(fp,"%u,", ( (skeleton->body.joint2D[i].x!=0) && (skeleton->body.joint2D[i].y!=0) ) );
                }
            
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    fprintf(fp,"%f,",(float) skeleton->leftHand.joint2D[i].x/width);
                    fprintf(fp,"%f,",(float) skeleton->leftHand.joint2D[i].y/height);
                    fprintf(fp,"%u,", ( (skeleton->leftHand.joint2D[i].x!=0) && (skeleton->leftHand.joint2D[i].y!=0) ) );
                }
                
            for (int i=0; i<COCO_HAND_PARTS; i++)
                {
                    fprintf(fp,"%f,",(float) skeleton->rightHand.joint2D[i].x/width);
                    fprintf(fp,"%f,",(float) skeleton->rightHand.joint2D[i].y/height);
                    fprintf(fp,"%u,", ( (skeleton->rightHand.joint2D[i].x!=0) && (skeleton->rightHand.joint2D[i].y!=0) ) );
                }
                
            for (int i=0; i<OP_HEAD_PARTS; i++)
                {
                    fprintf(fp,"%f,",(float) skeleton->head.joint2D[i].x/width);
                    fprintf(fp,"%f,",(float) skeleton->head.joint2D[i].y/height);
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