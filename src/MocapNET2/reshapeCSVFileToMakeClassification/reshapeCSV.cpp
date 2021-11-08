/*
 * Utility to extract BVH files straight from OpenPose JSON output
 * Sample usage ./MocapNETCSV  --from test.csv   --visualize 
 */
 
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include <unistd.h>
 
#include "../MocapNETLib2/mocapnet2.hpp" 
#include "../MocapNETLib2/core/core.hpp" 
#include "../MocapNETLib2/IO/csvRead.hpp" 


#include "../../../dependencies/RGBDAcquisition/tools/AmMatrix/matrix4x4Tools.h"
#include "../../../dependencies/RGBDAcquisition/tools/AmMatrix/simpleRenderer.h"
#include "../../../dependencies/RGBDAcquisition/tools/AmMatrix/solids.h"
#include "../../../dependencies/RGBDAcquisition/tools/AmMatrix/quaternions.h"
#include "../../../dependencies/RGBDAcquisition/tools/AmMatrix/matrixCalculations.h"

int verbose=0;

int generate4OrientationClassifier(const char * inputFilename)
{
     if (inputFilename==0)
     {
       fprintf(stderr,"Please provide an argument i.e. \n   ./reshapeCSV dataset/bvh_body_all.csv"); 
         return 0;
     }
    
     struct MocapNET2 mnet;
     struct CSVFileContext csvT={0};
     unsigned long processedLines=0; 

     fprintf(stderr,"Run : \n");
     fprintf(stderr,"./createRandomizedDataset.sh\n");
     fprintf(stderr,"to update the dataset/bvh_body_all.csv to the latest version..\n\n");
   
     fprintf(stdout,"Front,Back,Left,Right\n");  

     
     char csvFileInput[1024]; 
     snprintf(csvFileInput,1024,"%s",inputFilename);
     
     
     if (openCSVFile(&csvT,csvFileInput))
                {
                    fprintf(stderr,"CSV file had %u lines\n",csvT.lineNumber);
                    //----------------------------------------------------
                    struct skeletonSerialized skeletonS= {0};
                    while ( parseNextCSVCOCOSkeleton(&csvT,&skeletonS) )
                    //----------------------------------------------------
                                {
                                     if (processedLines==0)
                                         { initializeMocapNET2InputAssociation(&mnet,&skeletonS,1,1,1); }
                                         
                                     unsigned int correspondingClass = getMocapNETOrientationFromAngle(skeletonS.skeletonBody[4].value);
                                     
                                     if (processedLines%1000==0) { fprintf(stderr,"."); }
                                     //fprintf(stderr,".[%0.2f/%u]",skeletonS.skeletonBody[4].value,correspondingClass);
                                     
                                     /*
                                      enum MOCAPNET_Orientation
                                       {
                                          MOCAPNET_ORIENTATION_NONE=0,
                                          MOCAPNET_ORIENTATION_FRONT,
                                          MOCAPNET_ORIENTATION_BACK,
                                          MOCAPNET_ORIENTATION_LEFT,
                                          MOCAPNET_ORIENTATION_RIGHT,
                                          //----------------------------- 
                                          MOCAPNET_ORIENTATION_NUMBER
                                       };
                                      */
                                      
                                     switch(correspondingClass)
                                     {     
                                       //The series here is important for MocapNETLib2/core.cpp:
                                       case  MOCAPNET_ORIENTATION_NONE:  fprintf(stdout,"0,0,0,0\n");   break; //Add erroneous categories to catch bugs
                                       case  MOCAPNET_ORIENTATION_FRONT: fprintf(stdout,"1,0,0,0\n");   break; 
                                       case  MOCAPNET_ORIENTATION_BACK:  fprintf(stdout,"0,1,0,0\n");   break;
                                       case  MOCAPNET_ORIENTATION_LEFT:  fprintf(stdout,"0,0,1,0\n");   break;
                                       case  MOCAPNET_ORIENTATION_RIGHT: fprintf(stdout,"0,0,0,1\n");   break;
                                       default :                         fprintf(stdout,"0,0,0,0\n");   break; //Add erroneous categories to catch bugs
                                     };
                                     
                                     ++processedLines;
                                }
                    //----------------------------------------------------
                    closeCSVFile(&csvT);
                }


     fprintf(stderr,"\nDon't forget, Run : \n");
     fprintf(stderr,"./createRandomizedDataset.sh\n");
     fprintf(stderr,"to update the dataset/bvh_body_all.csv to the latest version..\n\n");

    exit(0);    
}

struct closestResultForVertice
{
   float quaternion[4];
   float distance;
   int checks;
};


int findClosestQuaternionToIcosahedronVertices()
{
    struct closestResultForVertice data[numberOfIcosahedronVertices]={0};

    float quaternion[] = {1.0,0.0,0.0,0.0};
    float euler[] = {0.0,0.0,0.0};
    struct Matrix4x4OfFloats m;

    struct Vector4x1OfFloats result;
    struct Vector4x1OfFloats unit;
    
    
    float unitLength =  2.8952;
                            /*sqrt(
                               ( polyhedraCoordinates[icosahedronVertices[0]*3 + 0] * polyhedraCoordinates[icosahedronVertices[0]*3 + 0] ) + 
                               ( polyhedraCoordinates[icosahedronVertices[0]*3 + 1] * polyhedraCoordinates[icosahedronVertices[0]*3 + 1] ) + 
                               ( polyhedraCoordinates[icosahedronVertices[0]*3 + 2] * polyhedraCoordinates[icosahedronVertices[0]*3 + 2] )   
                             );*/
    
     unit.m[0]=unitLength;
     unit.m[1]=unitLength;
     unit.m[2]=unitLength;
     unit.m[3]=1.0;
 
    for (float z=0.0; z<360; z+=1.0)
    {
     for (float y=0.0; y<360; y+=1.0)
     {
      for (float x=0.0; x<360; x+=1.0)
      { 
          euler[0]=x;
          euler[1]=y;
          euler[2]=z;
          euler2Quaternions(quaternion,euler,qWqXqYqZ);
          quaternion2Matrix4x4(m.m,quaternion,qWqXqYqZ); 
          transform3DPointFVectorUsing4x4FMatrix(&result,&m,&unit);
                   
                   
          //fprintf(stderr,"%0.2f,%0.2f,%0.2f -> %0.2f,%0.2f,%0.2f,%0.2f \n",euler[0],euler[1],euler[2],quaternion[0],quaternion[1],quaternion[2],quaternion[3]);
          
          
          for (unsigned int i=0; i<numberOfIcosahedronVertices; i++ )
          {
            float vertex[] =  {
                               polyhedraCoordinates[icosahedronVertices[i]*3 + 0],
                               polyhedraCoordinates[icosahedronVertices[i]*3 + 1],
                               polyhedraCoordinates[icosahedronVertices[i]*3 + 2]
                              };
           
            float currentDistance = distanceBetween3DPoints(result.m,vertex);
            
            if (data[i].checks!=0)
            {
                if (data[i].distance>currentDistance)
                {
                  data[i].quaternion[0]=quaternion[0];
                  data[i].quaternion[1]=quaternion[1];
                  data[i].quaternion[2]=quaternion[2];
                  data[i].quaternion[3]=quaternion[3];
                  data[i].distance = currentDistance;
                }
            } else
            {
              data[i].quaternion[0]=quaternion[0];
              data[i].quaternion[1]=quaternion[1];
              data[i].quaternion[2]=quaternion[2];
              data[i].quaternion[3]=quaternion[3];
              data[i].distance = currentDistance;
            }
            ++data[i].checks;
          }
          
      } 
     }
    }
    
    
          fprintf(stdout,"//autoconversion of icosahedron heatmap to quaternion..\n");
          fprintf(stdout,"int resolveIcosahedronHeatmapToQuaternion(float * quaternionOutput,float *heatmapInput)\n");
          fprintf(stdout,"{\n");
          
          fprintf(stdout,"float highestHeatmapValue=0.0;\n");
          fprintf(stdout,"unsigned int highestHeatmapIndex=0;\n");
          fprintf(stdout,"for (unsigned int i=0; i<%u; i++ )\n",numberOfIcosahedronVertices);
          fprintf(stdout,"{\n");
          fprintf(stdout,"  if (heatmapInput[i]>highestHeatmapValue)\n");
          fprintf(stdout,"     {\n");
          fprintf(stdout,"       highestHeatmapIndex = i; \n");
          fprintf(stdout,"       highestHeatmapValue = heatmapInput[i]; \n");
          fprintf(stdout,"     }\n");
          fprintf(stdout,"}\n");
          
            fprintf(stdout,"switch (highestHeatmapIndex)\n");
            fprintf(stdout,"{\n");
            for (unsigned int i=0; i<numberOfIcosahedronVertices; i++ )
              {
               fprintf(stdout," case %u:\n",i);
               fprintf(stdout," //Localised with distance %0.2f from truth\n",data[i].distance);
               fprintf(stdout," quaternionOutput[0]=%f; //qW\n",data[i].quaternion[0]);
               fprintf(stdout," quaternionOutput[1]=%f; //qX\n",data[i].quaternion[1]);
               fprintf(stdout," quaternionOutput[2]=%f; //qY\n",data[i].quaternion[2]);
               fprintf(stdout," quaternionOutput[3]=%f; //qZ\n",data[i].quaternion[3]);
               fprintf(stdout," break;\n");
              }
            fprintf(stdout,"}\n");

          fprintf(stdout,"return 1;\n"); 
          fprintf(stdout,"}\n"); 
          return 1;
}

 


int generateDodecahedronClassifier(const char * inputFilename)
{
     if (inputFilename==0)
     {
       fprintf(stderr,"Please provide an argument i.e. \n   ./reshapeCSV dataset/bvh_body_all.csv"); 
         return 0;
     }
    
     struct MocapNET2 mnet;
     struct CSVFileContext csvT={0};
     unsigned long processedLines=0; 

     fprintf(stderr,"Run : \n");
     fprintf(stderr,"./createRandomizedDataset.sh\n");
     fprintf(stderr,"to update the dataset/bvh_body_all.csv to the latest version..\n\n");
   
     char addComma=0;
     for (unsigned int i=0; i<numberOfIcosahedronVertices; i++)
                                     {
                                        if (addComma) { fprintf(stdout,","); }
                                        fprintf(stdout,"Vertex_%u",i);
                                        addComma=1;
                                     }
     fprintf(stdout,"\n");
     
     char csvFileInput[1024];
     snprintf(csvFileInput,1024,"%s",inputFilename);
     
     float unitLength =  2.8952;
                            /*sqrt(
                               ( polyhedraCoordinates[icosahedronVertices[0]*3 + 0] * polyhedraCoordinates[icosahedronVertices[0]*3 + 0] ) + 
                               ( polyhedraCoordinates[icosahedronVertices[0]*3 + 1] * polyhedraCoordinates[icosahedronVertices[0]*3 + 1] ) + 
                               ( polyhedraCoordinates[icosahedronVertices[0]*3 + 2] * polyhedraCoordinates[icosahedronVertices[0]*3 + 2] )   
                             );*/
     
     if (openCSVFile(&csvT,csvFileInput))
                {
                    fprintf(stderr,"CSV file had %u lines\n",csvT.lineNumber);
                    //----------------------------------------------------
                    struct skeletonSerialized skeletonS= {0};
                    while ( parseNextCSVCOCOSkeleton(&csvT,&skeletonS) )
                    //----------------------------------------------------
                                {
                                     if (processedLines==0)
                                         { initializeMocapNET2InputAssociation(&mnet,&skeletonS,1,1,1); }
                                         
                                     float quaternion[] = {
                                                           skeletonS.skeletonBody[3].value, //qW
                                                           skeletonS.skeletonBody[4].value, //qX
                                                           skeletonS.skeletonBody[5].value, //qY
                                                           skeletonS.skeletonBody[6].value  //qZ
                                                          };
                                     
                                     struct Matrix4x4OfFloats m;
                                     quaternion2Matrix4x4(m.m,quaternion,qWqXqYqZ);
                                     
                                     struct Vector4x1OfFloats unit;
                                     unit.m[0]=unitLength;
                                     unit.m[1]=unitLength;
                                     unit.m[2]=unitLength;
                                     unit.m[3]=1.0; 
                                     
                                     
                                     struct Vector4x1OfFloats result;
                                     transform3DPointFVectorUsing4x4FMatrix(&result,&m,&unit);
                                     
                                     
                                     float vertex[] =  {
                                                           polyhedraCoordinates[icosahedronVertices[0]*3 + 0],
                                                           polyhedraCoordinates[icosahedronVertices[0]*3 + 1],
                                                           polyhedraCoordinates[icosahedronVertices[0]*3 + 2]
                                                       };
                                     unsigned int closestPoint = 0; 
                                     float closestDistance = distanceBetween3DPoints(result.m,vertex);
                                        
                                     for (unsigned int i=0; i<numberOfIcosahedronVertices; i++ )
                                     {
                                        vertex[0]=polyhedraCoordinates[icosahedronVertices[i]*3 + 0];
                                        vertex[1]=polyhedraCoordinates[icosahedronVertices[i]*3 + 1];
                                        vertex[2]=polyhedraCoordinates[icosahedronVertices[i]*3 + 2];
                                        
                                        float distance = distanceBetween3DPoints(result.m,vertex);
                                        if (distance<closestDistance)
                                        {
                                          closestPoint = i;
                                          closestDistance = distance;
                                        }
                                     } 
                                     
                                     
                                     if (processedLines%1000==0) { fprintf(stderr,"."); }
                                     //fprintf(stderr,".[%0.2f/%u]",skeletonS.skeletonBody[4].value,correspondingClass);
                                      
                                      if (verbose)
                                      {
                                        fprintf(stderr,"%0.2f,%0.2f,%0.2f,%0.2f\n",quaternion[0],quaternion[1],quaternion[2],quaternion[3]); 
                                      }
                                      
                                     addComma=0;
                                     for (unsigned int i=0; i<numberOfIcosahedronVertices; i++)
                                     {
                                        if (addComma) { fprintf(stdout,","); }
                                        fprintf(stdout,"%u",(i==closestPoint));
                                        addComma=1;
                                     }
                                    fprintf(stdout,"\n");
                                    
                                     
                                   ++processedLines;
                                }
                    //----------------------------------------------------
                    closeCSVFile(&csvT);
                }


     fprintf(stderr,"\nDon't forget, Run : \n");
     fprintf(stderr,"./createRandomizedDataset.sh\n");
     fprintf(stderr,"to update the dataset/bvh_body_all.csv to the latest version..\n\n");

    exit(0);    
}


//autoconversion of icosahedron heatmap to quaternion..
int resolveIcosahedronHeatmapToQuaternion(float * quaternionOutput,float *heatmapInput)
{
float highestHeatmapValue=0.0;
unsigned int highestHeatmapIndex=0;
for (unsigned int i=0; i<12; i++ )
{
  if (heatmapInput[i]>highestHeatmapValue)
     {
       highestHeatmapIndex = i; 
       highestHeatmapValue = heatmapInput[i]; 
     }
}
switch (highestHeatmapIndex)
{
 case 0:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=0.211217;
 quaternionOutput[1]=-0.509923;
 quaternionOutput[2]=-0.319114;
 quaternionOutput[3]=-0.770410;
 break;
 case 1:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=-0.770410;
 quaternionOutput[1]=-0.319114;
 quaternionOutput[2]=0.509923;
 quaternionOutput[3]=-0.211217;
 break;
 case 2:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=0.694115;
 quaternionOutput[1]=0.134922;
 quaternionOutput[2]=0.395409;
 quaternionOutput[3]=0.586218;
 break;
 case 3:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=0.586218;
 quaternionOutput[1]=0.395409;
 quaternionOutput[2]=-0.134922;
 quaternionOutput[3]=-0.694115;
 break;
 case 4:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=0.694115;
 quaternionOutput[1]=0.395409;
 quaternionOutput[2]=0.586218;
 quaternionOutput[3]=0.134922;
 break;
 case 5:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=0.509923;
 quaternionOutput[1]=-0.319114;
 quaternionOutput[2]=0.211217;
 quaternionOutput[3]=0.770410;
 break;
 case 6:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=-0.134922;
 quaternionOutput[1]=-0.586218;
 quaternionOutput[2]=0.395409;
 quaternionOutput[3]=0.694115;
 break;
 case 7:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=-0.395409;
 quaternionOutput[1]=0.694115;
 quaternionOutput[2]=-0.134922;
 quaternionOutput[3]=0.586218;
 break;
 case 8:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=0.395409;
 quaternionOutput[1]=-0.586218;
 quaternionOutput[2]=-0.694115;
 quaternionOutput[3]=0.134922;
 break;
 case 9:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=-0.134922;
 quaternionOutput[1]=0.694115;
 quaternionOutput[2]=-0.586218;
 quaternionOutput[3]=0.395409;
 break;
 case 10:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=0.509923;
 quaternionOutput[1]=0.211217;
 quaternionOutput[2]=0.770410;
 quaternionOutput[3]=-0.319114;
 break;
 case 11:
 //Localised with distance 0.03 from truth
 quaternionOutput[0]=-0.319114;
 quaternionOutput[1]=0.770410;
 quaternionOutput[2]=-0.211217;
 quaternionOutput[3]=-0.509923;
 break;
}
return 1;
}



int main(int argc, char *argv[])
{
  const char * inputFilename = 0;
       
  for (int i=0; i<argc; i++)
  {
    if (strcmp(argv[i],"--from")==0)
    {
      inputFilename = argv[i+1];
    } else
    if (strcmp(argv[i],"--orientations")==0)
    {
        int numberOfOrientations = atoi(argv[i+1]);
        if (numberOfOrientations==4)
        {
            generate4OrientationClassifier(inputFilename);
        }
    } else
    if (strcmp(argv[i],"--faces")==0)
    {
        int numberOfOrientations = atoi(argv[i+1]);
        if (numberOfOrientations==20)
        {
            generateDodecahedronClassifier(inputFilename);
        }
    } else
    {
      //   ./ReshapeCSV --findClosestQuaternionToIcosahedronVertices
      if (strcmp(argv[i],"--findClosestQuaternionToIcosahedronVertices")==0)
      {
        findClosestQuaternionToIcosahedronVertices();
      }
    }
  }
}
