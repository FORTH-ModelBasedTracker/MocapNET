#include "tools.hpp"

#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h> //tolower
#include <string.h>
#include <stdlib.h>


unsigned long tickBaseMN = 0;


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
 

int vectorcmp(std::vector<float> vA,std::vector<float> vB, float maximumDistance)
{
    //fprintf(stderr,"vectorcmp %u vs %u elements \n",vA.size(), vB.size());
    if (vA.size()>vB.size()) { return -1; } 
      else
    if (vA.size()<vB.size()) { return  1; } 
      else
    {
        for (int i=0; i<vA.size(); i++)
        {
            //fprintf(stderr," %0.2f/%0.2f  ",vA[i],vB[i]);
            if (vA[i]<vB[i]) 
                 { 
                   if (vB[i]-vA[i]>maximumDistance) { return -1; } 
                 } else
            if (vA[i]>vB[i]) 
                 { 
                   if (vA[i]-vB[i]>maximumDistance) { return 1;  }
                 }
        }
    }
    return 0;
}


int nsleep(long nanoseconds)
{
   struct timespec req, rem;

   req.tv_sec = 0;              
   req.tv_nsec = nanoseconds; 

   return nanosleep(&req , &rem);
}


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
 


float getAngleToAlignToZero(std::vector<float> &positions,unsigned int centerJoint,unsigned int referenceJoint) 
{
  //We have points a, b and c and we want to calculate angle b
    float aX= positions[referenceJoint*3+0];
    float aY= positions[referenceJoint*3+1];
  
    float bX= positions[centerJoint*3+0];
    float bY= positions[centerJoint*3+1];
    
    return getAngleToAlignToZero_tools(aX,aY,bX,bY); 
}



int rotate2DPointsBasedOnJointAsCenter(std::vector<float> &positions,float angle,unsigned int centerJoint)
{
    if (positions.size()%3!=0)
        {
            fprintf(stderr,RED "rotate2DPointsBasedOnJointAsCenter: incorrect positions.. \n" NORMAL);
            return 0;
        }
 
    if (positions.size()<=centerJoint*3)
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
            fprintf(stderr,RED "rotate2DPointsBasedOnJointAsCenter: .. \n" NORMAL);
            return 0;
        }
 
    for (unsigned int jID=0; jID<positions.size()/3; jID++)
        {
            float jX=positions[jID*3+0];
            float jY=positions[jID*3+1];
            float jVisibility=positions[jID*3+2];
            //fprintf(stderr,"Rotating point %0.2f,%0.2f using pivot %0.2f,%0.2f by %0.2f deg  -> ",jX,jY,cx,cy,angle);
 
            // translate point back to origin:
            jX -= cx;
            jY -= cy;
 
            // rotate point
            float xnew = jX * c - jY * s;
            float ynew = jX * s + jY * c;
 
            // translate point back:
            positions[jID*3+0] = xnew + cx;
            positions[jID*3+1] = ynew + cy;
 
           //fprintf(stderr,"%0.2f,%0.2f\n",positions[jID*3+0],positions[jID*3+1]);
        }
 
 
    return 1;
}
 


void normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
    float * x,
    float * y,
    unsigned int currentWidth,
    unsigned int currentHeight,
    unsigned int respectTrainingAspectRatio,
    unsigned int trainingWidth,
    unsigned int trainingHeight
)
{
    if ( (*x==0) && (*y==0) )
        {
            //Only Fix Aspect Ratio on visible points to preserve 0,0,0 that are
            //important to the neural network..
            return;
        }
     
   if (!respectTrainingAspectRatio)
     {
         *x=(float) *x/currentWidth;
         *y=(float) *y/currentHeight;
         return; 
     }

    //fprintf(stderr,YELLOW "normalizeWhileAlsoMatchingTrainingAspectRatio\n" NORMAL);
    unsigned int addX=0,addY=0;
    unsigned int targetWidth=currentWidth,targetHeight=currentHeight;
    float currentAspectRatio  = (float) currentWidth/currentHeight;
    float trainingAspectRatio = (float) trainingWidth/trainingHeight;

    //fprintf(stderr,"Will try to correct aspect ratio from %0.2f(%ux%u) to %0.2f (%ux%u)\n",currentAspectRatio,currentWidth,currentHeight,trainingAspectRatio,trainingWidth,trainingHeight);


    if (currentHeight<currentWidth)
        {
            targetHeight=(unsigned int) currentWidth/trainingAspectRatio;
            if (targetHeight>=currentHeight)
                {
                    addY=(unsigned int) (targetHeight-currentHeight)/2;
                }
            else
                {
                    //Turns out we will have to enlarge X instead of englarging Y
                    addY=0;
                    targetHeight=currentHeight;
                    targetWidth=(unsigned int)currentHeight*trainingAspectRatio;
                    addX=(unsigned int) (targetWidth-currentWidth)/2;
                }
        }
    else if (currentWidth<=currentHeight)
        {
            targetWidth=(unsigned int)currentHeight*trainingAspectRatio;
            if (targetWidth>=currentWidth)
                {
                    addX=(unsigned int) (targetWidth-currentWidth)/2;
                }
            else
                {
                    //Turns out we will have to enlarge Y instead of englarging X
                    addX=0;
                    targetWidth=currentWidth;
                    targetHeight=(unsigned int) currentWidth/trainingAspectRatio;
                    addY=(unsigned int) (targetHeight-currentHeight)/2;
                }
        }

    //fprintf(stderr,"Target resolution is  %ux%u to Y\n",targetWidth,targetHeight);
    //fprintf(stderr,"Will add %u to X and %u to Y to achieve it\n",addX,addY);

    float targetAspectRatio=(float) targetWidth/targetHeight;
    if ((unsigned int) targetAspectRatio/100!= (unsigned int) trainingAspectRatio/100)
        {
            fprintf(stderr,"Failed to perfectly match training aspect ratio (%0.5f), managed to reach (%0.5f)\n",trainingAspectRatio,targetAspectRatio);
        }


    *x=(float) (*x+addX)/targetWidth;
    *y=(float) (*y+addY)/targetHeight;

}


int convertStringToLowercase(char * target,unsigned int targetLength,const char * source)
{
    if (source==0)
        {
            return 0;
        }
    unsigned int sourceLength = strlen(source);

    if (targetLength<=sourceLength)
        {
            return 0;
        }

    for (int i=0; i<sourceLength; i++)
        {
            target[i]=tolower(source[i]);
        }
    target[sourceLength]=0;

    return 1;
}

unsigned int getNumberOfEmptyElements(std::vector<float> mocapNETInput)
{
    unsigned int numberOfZeros=0;

    for (int i=0; i<mocapNETInput.size(); i++)
        {
            if (mocapNETInput[i]==0)
                {
                    ++numberOfZeros;
                }

        }
    return numberOfZeros;
}


int  findFirstFormattedFileInDirectory(const char * path,const char * formatString, const char * label, unsigned int * frameIDOutput)
{
    char filePathOfJSONFile[1025]= {0};

    unsigned int frameID=0;
    int found=0;
    
    while (!found)
        {
            snprintf(filePathOfJSONFile,1024,formatString,path,label,frameID);

            if( fileExists(filePathOfJSONFile) )
                {
                    *frameIDOutput=frameID;
                    found=1;
                    break;
                }

            ++frameID;
            if (frameID==10000)
                {
                    break;
                }
        }

    return found;
}


int executeCommandLineNum(const char *  command, char * what2GetBack, unsigned int what2GetBackMaxSize,unsigned int lineNumber)
{
    /* Open the command for reading. */
    FILE * fp = popen(command, "r");
    if (fp == 0 )
        {
            fprintf(stderr,"Failed to run command (%s) \n",command);
            return 0;
        }

    /* Read the output a line at a time - output it. */
    unsigned int i=0;
    while (fgets(what2GetBack, what2GetBackMaxSize, fp) != 0)
        {
            ++i;
            if (lineNumber==i)
                {
                    break;
                }
        }
    /* close */
    pclose(fp);
    return 1;
}

/**
 * @brief Simple way to get our CPU model name..
 * @retval 1=Success/0=Failure
 */
int getCPUName(char * str,unsigned int maxLength)
{
    if (!executeCommandLineNum("cat /proc/cpuinfo | grep \"model name\" | cut -d':' -f2 | tr -s ' '|  uniq",str,maxLength,0))
        {
            fprintf(stderr,"Could not get our CPU model name\n");
            return 0;
        }
        
    unsigned int length=strlen(str);
    for (unsigned int c=0; c<length; c++)
    {
      if (str[c]==10) { str[c]=0; } 
      if (str[c]==13) { str[c]=0; } 
    }
            
    return 1;
}

/**
 * @brief Simple way to get our GPU model name..
 * @retval 1=Success/0=Failure
 */
int getGPUName(char * str,unsigned int maxLength)
{
    if (!executeCommandLineNum("lspci | grep VGA  | cut -d':' -f3 | tr -s ' '|  uniq",str,maxLength,0))
        {
            fprintf(stderr,"Could not get our GPU model name\n");
            return 0;
        }
    
    unsigned int length=strlen(str);
    for (unsigned int c=0; c<length; c++)
    {
      if (str[c]==10) { str[c]=0; } 
      if (str[c]==13) { str[c]=0; } 
    }
    
    return 1;
}


unsigned long GetTickCountMicrosecondsMN()
{
    struct timespec ts;
    if ( clock_gettime(CLOCK_MONOTONIC,&ts) != 0)
        {
            return 0;
        }

    if (tickBaseMN==0)
        {
            tickBaseMN = ts.tv_sec*1000000 + ts.tv_nsec/1000;
            return 0;
        }

    return ( ts.tv_sec*1000000 + ts.tv_nsec/1000 ) - tickBaseMN;
}


unsigned long GetTickCountMillisecondsMN()
{
    return (unsigned long) GetTickCountMicrosecondsMN()/1000;
}


float convertStartEndTimeFromMicrosecondsToFPS(unsigned long startTime, unsigned long endTime)
{
    float timeInMilliseconds =  (float) (endTime-startTime)/1000;
    if (timeInMilliseconds ==0.0)
        {
            timeInMilliseconds=0.00001;    //Take care of division by null..
        }
    return (float) 1000/timeInMilliseconds;
}


char directoryExists(const char * folder)
{
 struct stat sb;
 if (stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode)) { return 1; } 
 return 0;
}


char fileExists(const char * filename)
{
    FILE *fp = fopen(filename,"r");
    if( fp )
        {
            /* exists */ 
            fclose(fp);
            return 1;
        }
    return 0;
}

int getImageWidthHeight(const char * filename,unsigned int * width, unsigned int * height)
{
    if (!fileExists(filename))
        {
            fprintf(stderr,"File %s does not exist, unable to get its dimensions..\n",filename);
            return 0;
        }

    unsigned int retrievedWidth=0;
    unsigned int retrievedHeight=0;

    char commandToExecute[1024]= {0};
    char result[1024]= {0};
    int  results=0;

    snprintf(commandToExecute,1024,"identify -format \"%%w\" %s",filename);
    if ( executeCommandLineNum(commandToExecute,result,1024,0) )
        {
            retrievedWidth = atoi(result);
            ++results;
        }

    snprintf(commandToExecute,1024,"identify -format \"%%h\" %s",filename);
    if ( executeCommandLineNum(commandToExecute,result,1024,0) )
        {
            retrievedHeight = atoi(result);
            ++results;
        }

    if (
        (retrievedWidth!=0) &&
        (retrievedHeight!=0)
       )
        {
            *width  = retrievedWidth;
            *height = retrievedHeight;
        }



    return (results==2);
}
