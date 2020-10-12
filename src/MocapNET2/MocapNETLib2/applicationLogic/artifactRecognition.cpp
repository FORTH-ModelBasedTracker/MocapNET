#include "artifactRecognition.hpp" 

#include "../../../../dependencies/InputParser/InputParser_C.h"
#include "../../../../dependencies/RGBDAcquisition/tools/AmMatrix/matrixCalculations.h"


 #include <iostream> 
 
 
#include <stdio.h>
 
#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
 
 
 int initializeArtifactsFromFile(struct sceneArtifacts * scene,const char * filename)
 {
     fprintf(stderr,"Initializing from %s .. \n",filename);
     
    FILE * fp = fopen(filename,"r");
    if (fp!=0)
        { 
            fprintf(stderr,GREEN "Successfully opened map file %s\n" NORMAL,filename);
 
            char * line = NULL;
            size_t len = 0;
            ssize_t read;
          
          scene->numberOfArtifacts=0;
          unsigned int id=0;
          
          struct InputParserC * ipc = InputParser_Create(8096,5);
          InputParser_SetDelimeter(ipc,0,',');
          InputParser_SetDelimeter(ipc,1,'(');
          InputParser_SetDelimeter(ipc,2,')');
          InputParser_SetDelimeter(ipc,3,10);
          InputParser_SetDelimeter(ipc,4,13); 
          
        do {
                 read = getline(&line, &len, fp);
                if (line!=0)
                {
                    int numberOfArguments = InputParser_SeperateWords(ipc,line,1);
                    if ( InputParser_WordCompareNoCaseAuto(ipc,0,"AREA2D") )
                    {
                        if (scene->numberOfArtifacts+1<NUMBER_OF_ARTIFACTS)
                        { 
                           ++scene->numberOfArtifacts;
                           if (scene->numberOfArtifacts>1)
                            {
                             ++id;
                           }
                        
                         InputParser_GetWord(ipc,1,scene->artifact[id].label,512);
                         fprintf(stderr,"New 2D Area declared ( %s )..!\n",scene->artifact[id].label);
                          scene->artifact[id].y1  =  InputParser_GetWordFloat(ipc,2); 
                          scene->artifact[id].x1  =  InputParser_GetWordFloat(ipc,3); 
                          scene->artifact[id].y2  =  InputParser_GetWordFloat(ipc,4);
                          scene->artifact[id].x2  =  InputParser_GetWordFloat(ipc,5);
                          scene->artifact[id].is3D=0;
                          scene->artifact[id].active=0; 
                          scene->artifact[id].hasAction=1; 
                        } 
                    } else
                    
                   if ( InputParser_WordCompareNoCaseAuto(ipc,0,"OBJECT3D") )
                    { 
                        if (scene->numberOfArtifacts+1<NUMBER_OF_ARTIFACTS)
                        { 
                           ++scene->numberOfArtifacts;
                           if (scene->numberOfArtifacts>1)
                            {
                             ++id;
                           }
                        
                        InputParser_GetWord(ipc,1,scene->artifact[id].label,512);
                        fprintf(stderr,"New 3D Object declared ( %s )..!\n",scene->artifact[id].label);
                          scene->artifact[id].y1  =  InputParser_GetWordFloat(ipc,2); 
                          scene->artifact[id].x1  =  InputParser_GetWordFloat(ipc,3); 
                          scene->artifact[id].z1  =  InputParser_GetWordFloat(ipc,4); 
                          scene->artifact[id].y2  =  InputParser_GetWordFloat(ipc,5);
                          scene->artifact[id].x2  =  InputParser_GetWordFloat(ipc,6);
                          scene->artifact[id].z2  =  InputParser_GetWordFloat(ipc,7); 
                          scene->artifact[id].is3D=1;
                          scene->artifact[id].active=0; 
                          scene->artifact[id].hasAction=1; 
                        } 
                    } else
                        
                        
                    if ( InputParser_WordCompareNoCaseAuto(ipc,0,"ACTIVATE_ON_POSITION") )
                    {
                        fprintf(stderr,"%s will activate on position\n",scene->artifact[id].label);
                        scene->artifact[id].activatesOnPosition=1;
                    } else
                    if ( InputParser_WordCompareNoCaseAuto(ipc,0,"ACTIVATE_ON_LOOK") )
                    {
                        fprintf(stderr,"%s will activate on look\n",scene->artifact[id].label);
                        scene->artifact[id].activatesOnLook=1;
                    } else
                    if ( InputParser_WordCompareNoCaseAuto(ipc,0,"ACTIVATE_ON_GESTURE") )
                    {
                        fprintf(stderr,"%s will activate on gesture\n",scene->artifact[id].label);
                        scene->artifact[id].activateOnGesture=1; 
                    }  else
                    if ( InputParser_WordCompareNoCaseAuto(ipc,0,"ACTIVATE_ACTION") )
                    {
                        InputParser_GetWord(ipc,1,scene->artifact[id].actionToExecuteOnActivation,512);
                        fprintf(stderr,"New Command for %s (%s) !\n",scene->artifact[id].label,scene->artifact[id].actionToExecuteOnActivation);
                        scene->artifact[id].hasAction=1;
                    }  else
                    if ( InputParser_WordCompareNoCaseAuto(ipc,0,"DEACTIVATE_ACTION") )
                    {
                        InputParser_GetWord(ipc,1,scene->artifact[id].actionToExecuteOnDeactivation,512);
                        fprintf(stderr,"New Command for %s (%s) !\n",scene->artifact[id].label,scene->artifact[id].actionToExecuteOnDeactivation);
                        scene->artifact[id].hasAction=1;
                    }  
                        
                }
             }
     while( (line!=0) && (read>0) );
            
             InputParser_Destroy(ipc);
             fclose(fp);
            return 1;
        }
      
     return 0;
 }
 

int check3DArtifactCollision(struct artifactData * artifact,float x1, float y1, float z1,float x2, float y2, float z2)
{
   if (artifact->is3D)
                { 
                      float initial[4]={x1,y1,z1,1.0};
                      float target[4]={x2,y2,z2,1.0};
                      //--------------------------------------------------------------
                      float v0[4] = { 
                                                 std::min(artifact->x1,artifact->x2),
                                                 std::min(artifact->y1,artifact->y2),
                                                  (float) (artifact->z1+artifact->z2)/2,
                                                 1.0
                                                };
                      //--------------------------------------------------------------
                      float v1[4] = { 
                                                 std::min(artifact->x1,artifact->x2),
                                                 std::max(artifact->y1,artifact->y2),
                                                  (float) (artifact->z1+artifact->z2)/2,
                                                 1.0
                                                };
                      //--------------------------------------------------------------
                      float v2[4] = { 
                                                 std::max(artifact->x1,artifact->x2),
                                                 std::max(artifact->y1,artifact->y2),
                                                  (float) (artifact->z1+artifact->z2)/2,
                                                 1.0
                                                };
                      //--------------------------------------------------------------
                      float v3[4] = { 
                                                 std::max(artifact->x1,artifact->x2),
                                                 std::min(artifact->y1,artifact->y2),
                                                  (float) (artifact->z1+artifact->z2)/2,
                                                 1.0
                                                };
                      //--------------------------------------------------------------
                       if (rayIntersectsRectangle(initial,target,v0,v1,v2,v3))
                       {
                           return 1;
                       }
                } 
    return 0;
}


int checkArtifactCollision(struct artifactData * artifact,float x, float y, float r)
 {
     if ( 
            (artifact->x1<x) && (artifact->y1<y) && (x<artifact->x2) && (y<artifact->y2)
         )
         {
               /*
                fprintf(stderr,"Artifact collision %0.2f %0.2f %0.2f %0.2f  -> %0.2f %0.2f \n",
                artifact->x1,artifact->y1,
                artifact->x2,artifact->y2,
                x,y); 
                */
         
                   return 1; 
         }
     /*
   fprintf(stderr,"NO collision %0.2f %0.2f %0.2f %0.2f  -> %0.2f %0.2f \n",
                artifact->x1,artifact->y1,
                artifact->x2,artifact->y2,
                x,y); */
     return 0;
 }
 
 
  
// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
int onSegment(float pX,float pY, float qX,float qY, float rX, float rY) 
{ 
    if (qX <= std::max(pX, rX) && qX >= std::min(pX, rX) && qY <= std::max(pY, rY) && qY >= std::min(pY, rY))  { return 1; } 
  
    return 0; 
} 
  
// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int orientation(float pX,float pY, float qX,float qY, float rX, float rY) 
{ 
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
    // for details of below formula. 
    int val = (qY - pY) * (rX - qX) - (qX - pX) * (rY - qY); 
  
    if (val == 0) return 0;  // colinear 
  
    return (val > 0)? 1: 2; // clock or counterclock wise 
} 
  
// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
int doIntersect(float p1X,float p1Y, float q1X, float q1Y,float p2X,float p2Y,float q2X, float q2Y) 
{
    // Find the four orientations needed for general and 
    // special cases 
    int o1 = orientation(p1X,p1Y, q1X,q1Y, p2X,p2Y); 
    int o2 = orientation(p1X,p1Y, q1X,q1Y, q2X,q2Y); 
    int o3 = orientation(p2X,p2Y, q2X,q2Y, p1X,p1Y); 
    int o4 = orientation(p2X,p2Y, q2X,q2Y, q1X,q1Y); 
  
    // General case 
    if (o1 != o2 && o3 != o4) 
        return 1; 
  
    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && onSegment(p1X,p1Y, p2X,p2Y, q1X,q1Y)) return 1; 
  
    // p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && onSegment(p1X,p1Y, q2X,q2Y, q1X,q1Y)) return 1; 
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && onSegment(p2X,p2Y, p1X,p1Y, q2X,q2Y)) return 1; 
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && onSegment(p2X,p2Y, q1X,q1Y, q2X,q2Y)) return 1; 
  
    return 0; // Doesn't fall in any of the above cases 
} 
  
  
  

int checkArtifactDirection(struct artifactData * artifact,float x1, float y1, float x2,float y2)
 {
     unsigned int check=0;
     
    check+=doIntersect(x1,y1,x2,y2,artifact->x1,artifact->y1,artifact->x2,artifact->y1) ; 
      if (check==0)      { check+=doIntersect(x1,y1,x2,y2,artifact->x1,artifact->y2,artifact->x2,artifact->y2) ; }
      if (check==0)      { check+=doIntersect(x1,y1,x2,y2,artifact->x1,artifact->y1,artifact->x1,artifact->y2) ; }
      if (check==0)      { check+=doIntersect(x1,y1,x2,y2,artifact->x2,artifact->y1,artifact->x2,artifact->y2) ; }
 
     
     return check;
 }
 
  