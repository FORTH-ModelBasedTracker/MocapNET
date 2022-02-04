#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <math.h> 
#include <vector>

#include "tensorflow2.h"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


float rand_FloatRange(float a, float b)
{
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}


int testFloat16()
{  
    unsigned int maxErrors = 10000;
    unsigned int errors=0;
    for (int i=0; i<maxErrors; i++)
    {
        float r1 = rand_FloatRange(-1000,1000);
        float r2 = convertFloat16ToFloat32(convertFloat32ToFloat16(r1));
        float distance = sqrt( (r1-r2) * (r1-r2) );
        if (distance>0.4)
        {
            fprintf(stderr,"%f ",distance);
            ++errors;
        }
    }
    
  fprintf(stderr,"\n\n%u errors / %0.2f %% \n",errors,(float) errors*100/maxErrors);
  return (errors==0);
}


int testUpperBody()
{
    const char* savedModelDirectory = "/home/ammar/Documents/Programming/DNNTracker/tensorflow2GPU/src/tmp.pb/";
    
    int runOnCPU=1;

    int ndims = 2;
    int64_t dims[] = {1,322};
    //-----------------------------
    float data[1*322];
    for(int i=0; i< (1*322); i++)
    {
        data[i] = 1.00;
    }
    int ndata = sizeof(float)*1*322 ; //number of bytes not number of element
    //-----------------------------
    
    
    unsigned int numberOfInputElements = 322;
    unsigned int numberOfOutputElements = 42;
    
    char inputTensor[]={"input_front"};
    char outputTensor[]={"result_front/concat"};
    
    //Tensorflow v2 test..
    struct Tensorflow2Instance upperbodyFrontv2={0};
    if ( tf2_loadModel(&upperbodyFrontv2,savedModelDirectory,inputTensor,numberOfInputElements,numberOfOutputElements,runOnCPU) )
    {
      if ( tf2_run(&upperbodyFrontv2,dims,ndims,data,ndata) )
      {
 
      void* buff = TF_TensorData(upperbodyFrontv2.outputValues[0]); 
      float* offsets = (float*)buff;
      printf("Result Tensor v2:\n");
      for(int i=0;i<upperbodyFrontv2.outputElements;i++)
       {
        if(i>0) { printf(","); } 
        printf("%f",offsets[i]);
       } 
       printf("\n");
          
      } else
      {
        fprintf(stderr,"Failed tf2_run\n");
      }
     
      printf("Deallocating v2 \n");
      tf2_unloadModel(&upperbodyFrontv2);       
    } else
    {
        fprintf(stderr,"Failed tf2_loadModel\n");
    }
    
    
    
    
    
    
    //Tensorflow v1 test..
    struct Tensorflow2Instance upperbodyFrontv1={0};
    if ( tf2_loadFrozenGraph(&upperbodyFrontv1,"dataset/combinedModel/mocapnet2/mode5/1.0/upperbody_front.pb",inputTensor,outputTensor,numberOfInputElements,numberOfOutputElements,runOnCPU) )
    {
      if ( tf2_run(&upperbodyFrontv1,dims,ndims,data,ndata) )
      {
       fprintf(stderr,"Ready to run..!\n");
       tf2_run(&upperbodyFrontv1,dims,ndims,data,ndata);
       
       void* buff = TF_TensorData(upperbodyFrontv1.outputValues[0]); 
       float* offsets = (float*)buff;
       printf("Result Tensor v1:\n");
       for(int i=0;i<upperbodyFrontv1.outputElements;i++)
       {
        if(i>0) { printf(","); } 
        printf("%f",offsets[i]);
       } 
       printf("\n");
      }
      
      
      printf("Deallocating v1 \n");
      tf2_unloadModel(&upperbodyFrontv1);     
    }
  

    
       printf("Done\n");
    




       return 0;
}




#include <unistd.h>
int executeCommandLineAndRetreiveAllResults(const char *  command , char * what2GetBack , unsigned int what2GetBackMaxSize, unsigned long * what2GetBackSize)
{
 /* Open the command for reading. */
 FILE * f = popen(command, "r");
 if (f == 0)
       {
         fprintf(stderr,"Failed to run command (%s) \n",command);
         return 0;
       }


  size_t contentSize = fread(what2GetBack, 1 , what2GetBackMaxSize, f);
  *what2GetBackSize = contentSize;

  /* close */
  pclose(f);
  return 1;
}

int getInfoFast(const char * field,char * buffer,unsigned int bufferSize,char terminatorCharacter)
{
   int fieldOffset = strlen(field);
   char * foundAt = strstr(buffer,field);

   if (foundAt!=0)
   {
     foundAt += fieldOffset;
     char * termination = strchr(foundAt,terminatorCharacter);
     if (termination!=0)
     {
        char retainOldTerminator = *termination;
        *termination = 0;
        fprintf(stderr,"Field `%s` => `%s` \n",field,foundAt);
        *termination =retainOldTerminator;
     }
   }
   
 return 0;
}




int testHands()
{
//    const char* savedModelDirectory = "/home/ammar/Documents/Programming/DNNTracker/DNNTracker/dataset/combinedModel/mocapnet2/mode1/1.0/rhand_half_all.pb";
    const char* savedModelDirectory = "/home/ammar/Documents/Programming/DNNTracker/DNNTracker/dataset/combinedModel/mocapnet2/mode1/1.0/lhand_all.pb";
    
    #define NUMBER_OF_OUTPUTS 52
    #define NUMBER_OF_INPUTS 319
    int runOnCPU=1;

    int ndims = 2;
    int64_t dims[] = {1,NUMBER_OF_INPUTS};
    //-----------------------------
    float data[1*NUMBER_OF_INPUTS];
    for(int i=0; i< (1*NUMBER_OF_INPUTS); i++)
    {
        data[i] = 1.00;
    }
    int ndata = sizeof(float)*1*NUMBER_OF_INPUTS ; //number of bytes not number of element
    //-----------------------------
    
    
    //Check using external tool and a parser
    //-------------------------------------
    tf2_checkModelUsingExternalTool(savedModelDirectory);
    /*
    
    static int maxOutputLength=4096;
    unsigned long outputSize=0;
    char output[maxOutputLength]={0};
    executeCommandLineAndRetreiveAllResults(commandToCheck,output,maxOutputLength,&outputSize);
    fprintf(stderr,"Retrieved.. %s\n",output);
    
    getInfoFast("inputs['",output,outputSize,'\'');
    getInfoFast("shape: ",output,outputSize,10);
    getInfoFast("name: ",output,outputSize,10);

    char * outputTensorsStart = strstr(output,"output(s):");
    if (outputTensorsStart!=0)
        {
          getInfoFast("outputs['",outputTensorsStart,outputSize,'\'');
          getInfoFast("shape: ",outputTensorsStart,outputSize,10); 
          getInfoFast("name: ",output,outputSize,10);
        }
    //-------------------------------------
    */
    unsigned int numberOfInputElements = NUMBER_OF_INPUTS;
    unsigned int numberOfOutputElements = NUMBER_OF_OUTPUTS;
    
    //https://medium.com/analytics-vidhya/deploying-tensorflow-2-1-as-c-c-executable-1d090845055c
    char inputTensor[]={"input_all"}; //{"input_front"};
    char outputTensor[]={"result_all/concat"}; //{"result_front/concat"};
    
    //Tensorflow v2 test..
    struct Tensorflow2Instance upperbodyFrontv2={0};
    if ( tf2_loadModel(&upperbodyFrontv2,savedModelDirectory,inputTensor,numberOfInputElements,numberOfOutputElements,runOnCPU) )
    {
      upperbodyFrontv2.inputIsHalfFloats=1;
      if ( tf2_run(&upperbodyFrontv2,dims,ndims,data,ndata) )
      {
 
      void* buff = TF_TensorData(upperbodyFrontv2.outputValues[0]); 
      float* offsets = (float*)buff;
      printf("Result Tensor v2:\n");
      for(int i=0; i<upperbodyFrontv2.outputElements; i++)
       {
        if(i>0) { printf(","); } 
        printf("%f",offsets[i]);
       } 
       printf("\n");
          
      } else
      {
        fprintf(stderr,"Failed tf2_run\n");
      }
     
      printf("Deallocating v2 \n");
      tf2_unloadModel(&upperbodyFrontv2);       
    } else
    {
        fprintf(stderr,RED "Failed tf2_loadModel\n" NORMAL);
    }
    
    
    
    
    //Tensorflow v1 test..
    struct Tensorflow2Instance upperbodyFrontv1={0};
    if ( tf2_loadFrozenGraph(&upperbodyFrontv1,"dataset/combinedModel/mocapnet2/mode1/1.0/_rhand_all.pb",inputTensor,outputTensor,numberOfInputElements,numberOfOutputElements,runOnCPU) )
    { 
      if ( tf2_run(&upperbodyFrontv1,dims,ndims,data,ndata) )
      {
       fprintf(stderr,"Ready to run..!\n");
       tf2_run(&upperbodyFrontv1,dims,ndims,data,ndata);
       
       void* buff = TF_TensorData(upperbodyFrontv1.outputValues[0]); 
       float* offsets = (float*)buff;
       printf("Result Tensor v1:\n");
       for(int i=0; i<upperbodyFrontv1.outputElements; i++)
       {
        if(i>0) { printf(","); } 
        printf("%f",offsets[i]);
       } 
       printf("\n");
      }
      
      
      printf("Deallocating v1 \n");
      tf2_unloadModel(&upperbodyFrontv1);     
    }
  

    
       printf("Done\n");
    




       return 0;
}



int main()
{
  testFloat16(); 
    
  return testHands();

  //return testUpperBody();
}
