#pragma once

#include <vector>


#ifdef USE_TENSORFLOW1
  #include "../Tensorflow/tensorflow.hpp"
#elif USE_TENSORFLOW2
  #include "../Tensorflow2/tensorflow2.h"
#endif

struct neuralNetworkModel
{
#ifdef USE_TENSORFLOW1
    struct TensorflowInstance model; 
#elif USE_TENSORFLOW2
    struct Tensorflow2Instance model; 
#else
    #warning "No tensorflow support signaled through a C define.."
    void * model;
#endif
};


void neuralNetworkPrintVersion();

char * neuralNetworkGetPath(struct neuralNetworkModel * nn);

int neuralNetworkLoad(
                      struct neuralNetworkModel * nn,
                      const char * filename,
                      const char * inputTensor,
                      const char * outputTensor,
                      unsigned int numberOfInputElements,
                      unsigned int numberOfOutputElements,
                      unsigned int forceCPU
                     );




int neuralNetworkUnload(
                        struct neuralNetworkModel * nn
                       );


std::vector<float> neuralNetworkExecute(
                                         struct neuralNetworkModel * nn,
                                         std::vector<float> input
                                       );
