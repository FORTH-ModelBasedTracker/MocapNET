#include <stdlib.h>
#include <stdio.h> 
#include <vector>

#include "tensorflow2.h"


int main()
{
    const char* savedModelDirectory = "/home/ammar/Documents/Programming/DNNTracker/tensorflow2GPU/src/tmp.pb/";
    struct Tensorflow2Instance upperbodyFront={0};
    tf2_loadModel(&upperbodyFront,savedModelDirectory,322,42);
    
    int ndims = 2;
    int64_t dims[] = {1,322};
    
    float data[1*322] ;//= {1,1,1,1,1,1,1,1,1,1};
    for(int i=0; i< (1*322); i++)
    {
        data[i] = 1.00;
    }
    int ndata = sizeof(float)*1*322 ; //number of bytes not number of element

    tf2_run(&upperbodyFront,dims,ndims,data,ndata);

    void* buff = TF_TensorData(upperbodyFront.outputValues[0]);
    float* offsets = (float*)buff;
    printf("Result Tensor :\n");
    for(int i=0;i<upperbodyFront.outputElements;i++)
    {
        if(i>0) { printf(","); } 
        printf("%f",offsets[i]);
    } 
    printf("\n");
    
    
    tf2_unloadModel(&upperbodyFront);
}
