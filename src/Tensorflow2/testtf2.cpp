#include <stdlib.h>
#include <stdio.h> 
#include <vector>

#include "tensorflow2.h"


int main()
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
    






}
