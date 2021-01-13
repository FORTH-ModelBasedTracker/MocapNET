#include "neuralNetworkAbstraction.hpp"

#include <stdio.h>

char * neuralNetworkGetPath(struct neuralNetworkModel * nn)
{
  #if USE_TENSORFLOW1
    return nn->model.modelPath;
  #elif USE_TENSORFLOW2
    return nn->model.modelPath;
  #else
    return 0;
  #endif
}

int neuralNetworkLoad(
                      struct neuralNetworkModel * nn,
                      const char * filename,
                      const char * inputTensor,
                      const char * outputTensor,
                      unsigned int numberOfInputElements,
                      unsigned int numberOfOutputElements,
                      unsigned int forceCPU
                     )
{
  #ifdef USE_TENSORFLOW1
    return  loadTensorflowInstance(&nn->model,filename,inputTensor,outputTensor,forceCPU);
  #elif USE_TENSORFLOW2 
    char checkForTF2Path[2048];
    snprintf(checkForTF2Path,2048,"%s/saved_model.pb",filename);
    
    if (tf2_fileExists(checkForTF2Path))
    {
    fprintf(stderr,"neuralNetworkLoad: attempting to load a TF2 style saved model (%s)..\n",checkForTF2Path);
     //This means we are talking about a TF2 "saved model" model
     return tf2_loadModel(&nn->model,filename,inputTensor,numberOfInputElements,numberOfOutputElements,forceCPU);
    } else
    if (tf2_fileExists(filename))
    {
     //This means we are talking about a TF1 style "frozen graph" model
     return tf2_loadFrozenGraph(&nn->model,filename,inputTensor,outputTensor,numberOfInputElements,numberOfOutputElements,forceCPU);     
    } else
    {
    fprintf(stderr,"Error: neuralNetworkLoad could not find a tf2/tf1 model to load..\n");
    }
  #endif 
  return 0;
}


int neuralNetworkUnload(
                        struct neuralNetworkModel * nn
                       )
{
  #ifdef USE_TENSORFLOW1
   return unloadTensorflow(&nn->model);
  #elif USE_TENSORFLOW2 
   return tf2_unloadModel(&nn->model); 
  #else
   return 0;
  #endif
}

std::vector<float> neuralNetworkExecute(struct neuralNetworkModel * nn,std::vector<float> input)
{
  std::vector<float> result;
  #ifdef USE_TENSORFLOW1
    result = predictTensorflow(&nn->model,input);
  #elif USE_TENSORFLOW2
    if (input.size() != nn->model.inputElements)
    {
      fprintf(stderr,"neuralNetworkExecute: Inconsistent input .. \n");
    }
 

    //Cast input vector in a regular C memory block and hold the dimension data..
    //-------------------------------------------------------------------------------------------
    int ndims = 2;
    int64_t dims[] = {1,nn->model.inputElements};
    float * data = (float*) malloc(sizeof(float) * nn->model.inputElements);
    if (data!=0)
    {
      for(int i=0; i< (1*nn->model.inputElements); i++)
        {
          data[i] = input[i];
        }
    int ndata = sizeof(float)*1*nn->model.inputElements; //number of bytes not number of element
    //-------------------------------------------------------------------------------------------
        
    //Actually run the neural network..
    //-------------------------------------------------------------------------------------------
      tf2_run(&nn->model,dims,ndims,data,ndata);
    //-------------------------------------------------------------------------------------------
      
    //Cast ouput from TF tensor data to a vector
    //-------------------------------------------------------------------------------------------
      void* buff = TF_TensorData(nn->model.outputValues[0]); 
      float* output = (float*)buff;
      
      for(int i=0;i< nn->model.outputElements;i++)
       { 
         result.push_back(output[i]);
       }  
    //-------------------------------------------------------------------------------------------
      
      free(data);
    }
  #endif
  
  return result;
}