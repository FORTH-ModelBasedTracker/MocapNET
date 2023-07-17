#include "calculations.h"

#include <stdio.h>
#include <stdlib.h>

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


const float goFromRadToDegrees=(float) 180.0 / M_PI; 
const float goFromDegreesToRad=(float) M_PI / 180.0; 


/** @brief This function returns the euclidean distance between two input 2D joints and zero if either of them is invalid*/
float getJoint2DDistance_tools(float aX,float aY,float bX,float bY)
{ 
    float xDistance=(float) bX-aX;
    float yDistance=(float) bY-aY;
    return (float) sqrt( (xDistance*xDistance) + (yDistance*yDistance) );
}


float getAngleToAlignToZero_tools(float aX,float aY,float bX,float bY) 
{
    if ( (aX==bX) && (aY==bY) ) { return 0; }
     
    
    //Bigger magnitudes..
    aX=100*aX;
    aY=100*aY;
    bX=100*bX;
    bY=100*bY;
    
    //We have points a, b and c and we want to calculate angle b  
    float lengthBetweenAAndB = getJoint2DDistance_tools(aX,aY,bX,bY);

    
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
    
  
    float dot   = (abX * cbX + abY * cbY); // dot product
    float cross = (abX * cbY - abY * cbX); // cross product

    float alpha = atan2(cross, dot);

    //fprintf(stderr,"Angle is %0.2f rad or %0.2f degrees \n",alpha,alpha*goFromRadToDegrees);
    return (float) alpha;// * goFromRadToDegrees ;
}
 


float getAngleToAlignToZero(float *positions,unsigned int centerJoint,unsigned int referenceJoint) 
{
  //We have points a, b and c and we want to calculate angle b
    float aX= positions[referenceJoint*3+0];
    float aY= positions[referenceJoint*3+1];
  
    float bX= positions[centerJoint*3+0];
    float bY= positions[centerJoint*3+1];
    
    return getAngleToAlignToZero_tools(aX,aY,bX,bY); 
}



int rotate2DPointsBasedOnJointAsCenter(float * positions,unsigned int positionsLength,float angle,unsigned int centerJoint)
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
            fprintf(stderr,RED "rotate2DPointsBasedOnJointAsCenter: cannot work without pivot joint.. \n" NORMAL);
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
 
