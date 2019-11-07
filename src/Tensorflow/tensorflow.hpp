#pragma once
/** @file tensorflow.hpp
 *  @brief The main wrapper around the tensorflow C API
 *
 *
 *  @author Ammar Qammaz (AmmarkoV)
 *  @bug The tensorflow C-API is volatile and subject to change
 */

#include <tensorflow/c/c_api.h> // TensorFlow C API header
#include <vector>


/**
 * @brief A structure that holds all of the relevant information for a tensorflow instance
 *
 * This is used to simplify context switching and reduce the complexity of the library
 *
 */
struct TensorflowInstance
{
  char modelPath[1024];
  char inputLayerName[512];
  char outputLayerName[512];

  TF_Graph* graph;
  TF_Session* session;
  TF_Operation* operation;
  TF_Tensor*  inputTensor;
  TF_Tensor*  outputTensor;
  TF_Output input_operation;
  TF_Output output_operation;

  TF_Status* status;
  TF_SessionOptions* options;
};

/**
 * @brief Get the number of Ticks in Microseconds, to be used as a performance counter
 * @ingroup tensorflow
 * @retval The number of microseconds since system boot
 */
unsigned long GetTickCountMicroseconds();


/**
 * @brief Get the number of Ticks in Milliseconds, to be used as a performance counter
 * @ingroup tensorflow
 * @retval The number of milliseconds since system boot
 */
unsigned long GetTickCountMilliseconds();


/**
 * @brief List nodes of the TF_Graph by printing them in stdout
 * @ingroup tensorflow
 * @param Label of printed output
 * @param Pointer to TF_Graph struct
 * @retval No return value
 */
void listNodes(const char * label , TF_Graph* graph);


/**
 * @brief Load a tensorflow instance from a .pb file
 * @ingroup tensorflow
 * @param Pointer to a struct TensorflowInstance that will hold the tensorflow instance on load.
 * @param Path to .pb file
 * @param Name of input tensor, i.e. input_1
 * @param Name of output tensor, i.e. output_1
 * @retval 1 = Success loading the file  , 0 = Failure
 */
int loadTensorflowInstance(
                            struct TensorflowInstance * net,
                            const char * filename,
                            const char * inputTensor,
                            const char * outputTensor,
                            unsigned int forceCPU
                          );

/**
 * @brief Evaluate an input vector through the neural network and return an output vector
 * @ingroup tensorflow
 * @param Pointer to a struct TensorflowInstance that holds a loaded tensorflow instance.
 * @param Input vector of floats
 * @retval Output vector of floats, Empty vector in case of failure
 */
std::vector<float> predictTensorflow(struct TensorflowInstance * net,std::vector<float> input);



/**
 * @brief Evaluate an input image through a network that outputs a vector of heatmaps
 * @ingroup tensorflow
 * @param Pointer to a struct TensorflowInstance that holds a loaded tensorflow instance.
 * @param Width of input image
 * @param Height of input image
 * @param Pixels of input image
 * @retval Output vector of vectors of floats, That correspond to the heatmaps
 * @bug TF_GraphGetTensorShape returns -1 for some networks.
 */
std::vector<std::vector<float> > predictTensorflowOnArrayOfHeatmaps(
                                                                     struct TensorflowInstance * net,
                                                                     unsigned int width ,
                                                                     unsigned int height ,
                                                                     float * data,
                                                                     unsigned int heatmapWidth,
                                                                     unsigned int heatmapHeight,
                                                                     unsigned int numberOfOutputTensors
                                                                   );


/**
 * @brief Clean tensorflow instance from memory and deallocate it
 * @ingroup tensorflow
 * @param Pointer to a struct TensorflowInstance that holds a loaded tensorflow instance.
 * @retval 1 = Success saving the file  , 0 = Failure
 */
int unloadTensorflow(struct TensorflowInstance * net);
