/** @file NSRM.h
 *  @brief An implementation of an NSRM descriptor
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef NSRM_H_INCLUDED
#define NSRM_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif


#include <math.h>


const float goFromRadToDegrees=(float) 180.0 / M_PI; 
const float goFromDegreesToRad=(float) M_PI / 180.0; 

/** @brief This function returns the euclidean distance between two input 2D joints and zero if either of them is invalid*/
static float getJoint2DDistance_NSRM(float aX,float aY,float bX,float bY)
{ 
    float xDistance=(float) bX-aX;
    float yDistance=(float) bY-aY;
    return (float) sqrt( (xDistance*xDistance) + (yDistance*yDistance) );
}


static float getAngleToAlignToZero_NSRM(float aX,float aY,float bX,float bY) 
{
    if ( (aX==bX) && (aY==bY) ) { return 0; }
     
    
    //Bigger magnitudes..
    aX=100*aX;
    aY=100*aY;
    bX=100*bX;
    bY=100*bY;
    
    //We have points a, b and c and we want to calculate angle b  
    float lengthBetweenAAndB = getJoint2DDistance_NSRM(aX,aY,bX,bY);

    
    //We align vertically.. , Point C is B offset in Y direction
    float cX = bX;
    float cY = bY - lengthBetweenAAndB;
    
    //fprintf(stderr,"We want to align A(%0.2f,%0.2f) to C(%0.2f,%0.2f) with pivot B(%0.2f,%0.2f)\n",aX,aY,cX,cY,bX,bY); 
    //fprintf(stderr,"length  AB = %0.2f\n",lengthBetweenAAndB);    
    //fprintf(stderr,"bY = %0.2f\n",bY);
    //fprintf(stderr,"cY = %0.2f = %0.2f - %0.2f\n",cY,bY,lengthBetweenAAndB);
    
    
    //Calulate vector a->b
    float abX = bX - aX;
    float abY = bY - aY;

    //calculate vector c->b
    float cbX = bX - cX;
    float cbY = bY - cY;
    
  
    float dot   = ((abX * cbX) + (abY * cbY)); // dot product
    float cross = ((abX * cbY) - (abY * cbX)); // cross product

    float alpha = atan2(cross,dot);

    //fprintf(stderr,"Angle is %0.2f rad or %0.2f degrees \n",alpha,alpha*goFromRadToDegrees);
    return (float) alpha;// * goFromRadToDegrees ;
}
 


static float getAngleToAlignToZeroJoints_NSRM(float * positions,unsigned int centerJoint,unsigned int referenceJoint) 
{
  //We have points a, b and c and we want to calculate angle b
    float aX= positions[referenceJoint*3+0];
    float aY= positions[referenceJoint*3+1];
  
    float bX= positions[centerJoint*3+0];
    float bY= positions[centerJoint*3+1];
    
    return getAngleToAlignToZero_NSRM(aX,aY,bX,bY); 
}


/*
static int rotate2DPointsBasedOnJointAsCenter_NSRM(float * positions,int positionsSize,float angle,unsigned int centerJoint)
{
    if (positionsSize%3!=0)
        {
            fprintf(stderr,RED "rotate2DPointsBasedOnJointAsCenter: incorrect positions.. \n" NORMAL);
            return 0;
        }
 
    if (positionsSize<=centerJoint*3)
        {
            fprintf(stderr,RED "rotate2DPointsBasedOnJointAsCenter: centerJoint out of bounds.. \n" NORMAL);
            return 0;
        }
 
    float s = sin((float) angle * goFromDegreesToRad);
    float c = cos((float) angle * goFromDegreesToRad);
    //=================================================
    float cx          = positions[centerJoint*3+0];
    float cy          = positions[centerJoint*3+1];
    float cVisibility = positions[centerJoint*3+2];
    //=================================================
 
    if (cVisibility==0.0)
        {
            fprintf(stderr,RED "rotate2DPointsBasedOnJointAsCenter: cannot work without pivot joint.. \n" NORMAL);
            return 0;
        }
 
    for (unsigned int jID=0; jID<positionsSize/3; jID++)
        {
            float jX=positions[jID*3+0];
            float jY=positions[jID*3+1];
            float jVisibility=positions[jID*3+2];
            //fprintf(stderr,"Rotating point %0.2f,%0.2f using pivot %0.2f,%0.2f by %0.2f deg  -> ",jX,jY,cx,cy,angle);
 
            //Translate point back to origin:
            jX -= cx;
            jY -= cy;
 
            //Rotate point
            float xnew = jX * c - jY * s;
            float ynew = jX * s + jY * c;
 
            //Translate point back:
            positions[jID*3+0] = xnew + cx;
            positions[jID*3+1] = ynew + cy;
 
           //fprintf(stderr,"%0.2f,%0.2f\n",positions[jID*3+0],positions[jID*3+1]);
        }
 
 
    return 1;
}*/
 


static int countNSRMElements(int numberOfJointRules)
{
  return numberOfJointRules*numberOfJointRules;
}




static int rotate2DPointsBasedOnJointAsCenter_NSRM(float * positions,unsigned int positionsLength,float angle,unsigned int centerJoint)
{
    if (positionsLength%3!=0)
        {
            fprintf(stderr,RED "rotate2DPointsBasedOnJointAsCenter: incorrect positions.. \n" NORMAL);
            return 0;
        }
 
    if (positionsLength<=centerJoint*3)
        {
            fprintf(stderr,RED "rotate2DPointsBasedOnJointAsCenter: centerJoint out of bounds.. \n" NORMAL);
            return 0;
        }
 
    float s = sin((float) angle * goFromDegreesToRad );
    float c = cos((float) angle * goFromDegreesToRad );
            
    float cx=positions[centerJoint*3+0];
    float cy=positions[centerJoint*3+1];
    float cVisibility=positions[centerJoint*3+2];
 
    if (cVisibility==0.0)
        {
            fprintf(stderr,RED "rotate2DPointsBasedOnJointAsCenter: cannot work with invisible pivot joint.. \n" NORMAL);
            return 0;
        }
 
    for (unsigned int jID=0; jID<positionsLength/3; jID++)
        {
            float jX=positions[jID*3+0];
            float jY=positions[jID*3+1];
            float jVisibility=positions[jID*3+2];
            //fprintf(stderr,"Rotating point %0.2f,%0.2f using pivot %0.2f,%0.2f by %0.2f deg  -> ",jX,jY,cx,cy,angle);
 
            //Translate point back to origin:
            jX -= cx;
            jY -= cy;
 
            //Rotate point
            float xnew = jX * c - jY * s;
            float ynew = jX * s + jY * c;
 
            //Translate point back:
            positions[jID*3+0] = xnew + cx;
            positions[jID*3+1] = ynew + cy;
 
           //fprintf(stderr,"%0.2f,%0.2f\n",positions[jID*3+0],positions[jID*3+1]);
        }
 
 
    return 1;
}



static float performNSRMAlignment(float * input2DJoints,
                                  unsigned int input2DJointsLength, 
                                  struct ModelConfigurationData* rules)
{
  //Enforce alignment rule..
  //------------------------------------------------------------
  int pivotPoint            = rules->alignment[0].jointStartID;// rules['Alignment'][0]['jointStartID']
  int referencePoint        = rules->alignment[0].jointEndID;  // rules['Alignment'][0]['jointEndID']
  //------------------------------------------------------------
  float pivotX              = input2DJoints[pivotPoint*3+0];
  float pivotY              = input2DJoints[pivotPoint*3+1];
  float pivotVisibility     = input2DJoints[pivotPoint*3+2];
  
  float referenceX          = input2DJoints[referencePoint*3+0];
  float referenceY          = input2DJoints[referencePoint*3+1];
  float referenceVisibility = input2DJoints[referencePoint*3+2];
  //------------------------------------------------------------
  
  float angleToRotate = 0.0;
  if ((pivotVisibility!=0) && (referenceVisibility!=0))
  {
    angleToRotate = getAngleToAlignToZero_NSRM(pivotX,pivotY,referenceX,referenceY); 
    rotate2DPointsBasedOnJointAsCenter_NSRM(input2DJoints,input2DJointsLength,angleToRotate,pivotPoint);
  } 

  return angleToRotate;
}


static int appendNSRMElements(
                              float * input2DJoints,
                              unsigned int input2DJointsLength,
                              float * output,
                              struct ModelConfigurationData* rules,
                              float angleUsedToRotateInput
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
  
  //-----------------------------------------------------------------------------------------------------
  //                                     ..Main NSRM parameters ..
  //----------------------------------------------------------------------------------------------------- 
  int count = 0;
  for (int i=0; i<numberOfJointRules; i++)
  {
   getCompositePoint(&iX,&iY,&iVisibility,&iInvalidPoint,rules,i,input2DJoints,input2DJointsLength);
   for (int j=0; j<numberOfJointRules; j++)
    {
         getCompositePoint(&jX,&jY,&jVisibility,&jInvalidPoint,rules,j,input2DJoints,input2DJointsLength);
         if ( (iInvalidPoint) || (jInvalidPoint) )
         {
            output[count] = 0.0;
         } else
         {
            output[count] =  getAngleToAlignToZero_NSRM(iX,iY,jX,jY);// getJoint2DDistanceEDM(input2DJoints,i,j);
         }
         count+=1;
    }
  }
  
  //-----------------------------------------------------------------------------------------------------
  //                                     ..New eNSRM diagonal parameters ..
  //-----------------------------------------------------------------------------------------------------
  if (rules->eNSRM)
  {
   count=0;
   int iJointID = rules->descriptorElements[0].jointID;
   iX = input2DJoints[iJointID*3+0];
   iY = input2DJoints[iJointID*3+1];
   //getCompositePoint(&iX,&iY,&iVisibility,&iInvalidPoint,rules,i,input2DJoints,input2DJointsLength);
   for (int i=0; i<numberOfJointRules; i++)
   {
    for (int j=0; j<numberOfJointRules; j++)
    {
      if (i==j)
       {
        output[count] == angleUsedToRotateInput;
        if  (i>0)
        {
         int jJointID = rules->descriptorElements[j].jointID;
         jX = input2DJoints[jJointID*3+0];
         jY = input2DJoints[jJointID*3+1];            
         //getCompositePoint(&jX,&jY,&jVisibility,&jInvalidPoint,rules,j,input2DJoints,input2DJointsLength);
         output[count] =  getJoint2DDistance_NSRM(iX,iY,jX,jY);
        }
       }
       count+=1;
    }
   }
  }
  
   
  //NSDM Normalization rule does not apply on NSRM
  //SKIPPED
  //-----------------------------------------------------------------------------------------------------
  
  //Enforce alignment rule..
  //------------------------------------------------------------
  int iJointID         = rules->alignment[0].jointStartID;// rules['Alignment'][0]['jointStartID']
  int jJointID         = rules->alignment[0].jointEndID;  // rules['Alignment'][0]['jointEndID']
  //------------------------------------------------------------
  float aX             = input2DJoints[iJointID*3+0];
  float aY             = input2DJoints[iJointID*3+1];
  float bX             = input2DJoints[jJointID*3+0];
  float bY             = input2DJoints[jJointID*3+1];
  //------------------------------------------------------------
  float alignmentAngle = getAngleToAlignToZero_NSRM(aX,aY,bX,bY);
  //fprintf(stderr,"ALIGNMENT %u %u %f",iJointID,jJointID,alignmentAngle);
  for (int i=0; i<count; i++)
      {
          if (output[i]!=0.0)
                 { output[i]=(float) alignmentAngle+output[i]; }
      }
    
  return count;
}




#ifdef __cplusplus
}
#endif




#endif  
