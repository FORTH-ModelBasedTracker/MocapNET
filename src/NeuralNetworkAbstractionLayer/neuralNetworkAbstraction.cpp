#include "neuralNetworkAbstraction.hpp"

#include <stdio.h>


void neuralNetworkPrintVersion()
{
  #if USE_TENSORFLOW1
    //=========================
    //=========================
    fprintf(stderr,"TF1 wrapper / Tensorflow version %s\n",TF_Version());
    //=========================
    //=========================
  #elif USE_TENSORFLOW2
    //=========================
    //=========================
    fprintf(stderr,"TF2 wrapper / Tensorflow version %s\n",TF_Version());
    //=========================
    //=========================
  #else
    fprintf(stderr,"No Tensorflow support\n");
  #endif
}

char * neuralNetworkGetPath(struct neuralNetworkModel * nn)
{
  #if USE_TENSORFLOW1
    //=========================
    //=========================
    return nn->model.modelPath;
    //=========================
    //=========================
  #elif USE_TENSORFLOW2
    //=========================
    //=========================
    return nn->model.modelPath;
    //=========================
    //=========================
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
    //=========================
    //=========================
    return  loadTensorflowInstance(&nn->model,filename,inputTensor,outputTensor,forceCPU);
    //=========================
    //=========================
  #elif USE_TENSORFLOW2 
    //=========================
    //=========================
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
    //=========================
    //=========================
  #endif 
  return 0;
}


int neuralNetworkUnload(
                        struct neuralNetworkModel * nn
                       )
{
  #ifdef USE_TENSORFLOW1
    //=========================
    //=========================
    return unloadTensorflow(&nn->model);
    //=========================
    //=========================
  #elif USE_TENSORFLOW2 
    //=========================
    //=========================
    return tf2_unloadModel(&nn->model); 
    //=========================
    //=========================
 #else
   return 0;
  #endif
}

std::vector<float> neuralNetworkExecute(struct neuralNetworkModel * nn,std::vector<float> input)
{
  std::vector<float> result;
  result.clear();
  
  
  #ifdef USE_TENSORFLOW1
    //=========================
    //========================= 
    result = predictTensorflow(&nn->model,input);
    //=========================
    //========================= 
  #elif USE_TENSORFLOW2
    //=========================
    //=========================
    if (input.size() != nn->model.inputElements)
    {
      fprintf(stderr,"neuralNetworkExecute: Inconsistent input .. \n");
      return result;
    }
    
    //Cast input vector in a regular C memory block and hold the dimension data..
    //-------------------------------------------------------------------------------------------
    unsigned int ndata = sizeof(float)*1*nn->model.inputElements; //number of bytes not number of element
    int ndims = 2;
    int64_t dims[] = {1,nn->model.inputElements};
    
    
    unsigned int newBufferSize = ndata;
    if ( (nn->model.buffer!=0) && (nn->model.bufferSize < newBufferSize) )
    {
      //Do reallocation here.. 
      float * newBuffer = (float*) realloc (nn->model.buffer,newBufferSize);
      if (newBuffer!=nn->model.buffer)
      {
        nn->model.bufferSize = newBufferSize;
      }
    } else
    if (nn->model.buffer==0)
    {
        newBufferSize = ndata;
        nn->model.bufferSize = newBufferSize;
        nn->model.buffer = (float *) malloc(newBufferSize);
    }
     
    if (nn->model.buffer!=0)
    {
      for(int i=0; i<nn->model.inputElements; i++)
        {
          nn->model.buffer[i] = input[i];
        }
    //-------------------------------------------------------------------------------------------
        
    //Actually run the neural network..
    //-------------------------------------------------------------------------------------------
    if (
        tf2_run(
              &nn->model,
              dims,
              ndims,
              nn->model.buffer,
              nn->model.bufferSize
             )
         )
    //-------------------------------------------------------------------------------------------
    {
    //Cast ouput from TF tensor data to a vector 
    //-------------------------------------------------------------------------------------------
      void* buff = TF_TensorData(nn->model.outputValues[0]); 
      size_t tensorByteSize = TF_TensorByteSize(nn->model.outputValues[0]);
      unsigned long tensorElements = TF_TensorElementCount(nn->model.outputValues[0]);
      
      if (tensorElements<nn->model.outputElements)
      {
        fprintf(stderr,"neuralNetworkExecute: Incorrect number of output elements, expected %u found %lu \n",nn->model.outputElements,tensorElements);
      } else
      if (buff!=0)
      { //If we have a buffer
        if (nn->model.outputIsHalfFloats)
          {
           //Convert float16 back to float32
           float16* output = (float16*) buff; 
           for (unsigned int i=0; i<nn->model.outputElements; i++)
               {
                result.push_back(convertFloat16ToFloat32(output[i]));
               }
          } else
          {
           //Regular float32 output that can be immediately pushed back to the vector
           float* output = (float*) buff; 
           for (unsigned int i=0; i<nn->model.outputElements; i++)
               { 
                result.push_back(output[i]);
               }
          }
      }
      } else
      {
        fprintf(stderr,"Failed running the network..\n");
      }
    //------------------------------------------------------------------------------------------- 
    } //We have a data buffer.. 
     else
    {
      fprintf(stderr,"Could not allocate enough memory to run %s\n",nn->model.modelPath);
    }
    //=========================
    //=========================
  #endif
  
  return result;
}
