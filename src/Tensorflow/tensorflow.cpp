/** @file tensorflow.cpp
 *  @brief This is a Tensorflow C-API wrapper.. Since Tensorflow is pretty volatile this whole file is kind of
 *  half-finished and will probably have to change to reflect changes to the tensorflow project. This code needs a serious cleanup when the Tensorflow C API is
 *  completely stable.
 *  If you are not familiar with Tensorflow for C check https://github.com/iwatake2222/CNN_NumberDetector/tree/master/03_Tensorflow_C
 *  @bug Tensorflow and the TF_GraphGetTensorShape ( https://github.com/tensorflow/tensorflow/blob/master/tensorflow/c/c_api.h#L239 ) returns -1,-1 as the dimensions of some tensors. In order to work around this the expected dimensions are given as variables
 *  from the MocapNET client applications
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include "tensorflow.hpp"
#include "tf_utils.hpp"
#include <stdio.h>

#include <vector>
#include <iostream>
#include <cstdint> // include this header for uint64_t
/*
std::vector<int> get_tensor_shape(tensorflow::Tensor& tensor)
{
    std::vector<int> shape;
    int num_dimensions = tensor.shape().dims()
    for(int ii_dim=0; ii_dim<num_dimensions; ii_dim++) {
        shape.push_back(tensor.shape().dim_size(ii_dim));
    }
    return shape;
}*/

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

#include <sys/time.h>
#include <unistd.h>
#include <time.h>


unsigned long tickBaseTF = 0;


unsigned long GetTickCountMicroseconds()
{
    struct timespec ts;
    if ( clock_gettime(CLOCK_MONOTONIC,&ts) != 0)
        {
            return 0;
        }

    if (tickBaseTF==0)
        {
            tickBaseTF = ts.tv_sec*1000000 + ts.tv_nsec/1000;
            return 0;
        }

    return ( ts.tv_sec*1000000 + ts.tv_nsec/1000 ) - tickBaseTF;
}



unsigned long GetTickCountMilliseconds()
{
    return (unsigned long) GetTickCountMicroseconds()/1000;
}






int checkAndDeallocate(TF_Status * s,const char * label)
{
    if (TF_GetCode(s) != TF_OK)
        {
            fprintf(stderr,RED "Error %s  \n" NORMAL,label );
            TF_DeleteStatus(s);
            return 0;
        }
    return 1;
}


void listNodes(const char * label , TF_Graph* graph)
{
    size_t pos = 0;
    TF_Operation* oper;
    std::cout << "Nodes list : \n";
    while ((oper = TF_GraphNextOperation(graph, &pos)) != nullptr)
        {
            std::cout << label<<" - "<<TF_OperationName(oper)<<" "<< std::endl;
        }
}



char tfGraphExists(const char * filename)
{
 FILE *fp = fopen(filename,"r");
 if( fp ) { /* exists */ fclose(fp); return 1; }
 return 0;
}

int loadTensorflowInstance(
    struct TensorflowInstance * net,
    const char * filename,
    const char * inputTensor,
    const char * outputTensor,
    unsigned int forceCPU
)
{
    if (!tfGraphExists(filename))
       {
            std::cout << "Graph "<<filename<<" could not be found "<< std::endl;
            return 0;
       }
    if (net==0)
       {
            std::cout << "TensorflowInstance structure not allocated "<< std::endl;
            return 0;
       }

    net->outputTensor=nullptr;

    //--------------------------------------------------------------------------------------------------------------
    net->graph   = tf_utils::LoadGraph(filename);
    if (net->graph == nullptr)
        {
            fprintf(stderr,RED "Can't load graph %s \n" NORMAL,filename); 
            return 0;
        }
    //tf_utils::PrintOp(net->graph);
    //--------------------------------------------------------------------------------------------------------------
    net->input_operation  = {TF_GraphOperationByName(net->graph, inputTensor), 0};
    if (net->input_operation.oper == nullptr)
        {
            listNodes(filename,net->graph);
            fprintf(stderr,RED "Can't init input for %s \n" NORMAL,filename);
            return 0;
        }
    std::cout << " Input Tensor for " <<filename << std::endl;
    tf_utils::PrintTensorInfo(net->graph,inputTensor,0,0);

    net->output_operation = {TF_GraphOperationByName(net->graph, outputTensor), 0};
    if (net->output_operation.oper == nullptr)
        {
            listNodes(filename,net->graph);
            fprintf(stderr,RED "Can't init output for %s \n" NORMAL,filename);
            return 0;
        }
    std::cout << " Output Tensor for " <<filename << std::endl;
    tf_utils::PrintTensorInfo(net->graph,outputTensor,0,0);
    //--------------------------------------------------------------------------------------------------------------

    snprintf(net->modelPath,1024,"%s",filename);
    snprintf(net->inputLayerName,512,"%s",inputTensor);
    snprintf(net->outputLayerName,512,"%s",outputTensor);

    //--------------------------------------------------------------------------------------------------------------
    net->status      = TF_NewStatus();
    net->options     = TF_NewSessionOptions();
    if (forceCPU)
        {
            //How do you end up with this byte array you might ask ?
            uint8_t config[] = { 0xa,0x7,0xa,0x3,0x43,0x50,0x55,0x10,0x1,0xa,0x7,0xa,0x3,0x47,0x50,0x55,0x10,0x0,0x38,0x1};
            //Good Question, you use the python code and extract the configuration bytes and copy paste them here..
            //https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/Tensorflow/createTensorflowConfigurationForC.py
            /*net->session = tf.ConfigProto(
                                             device_count={'CPU' : 1, 'GPU' : 0},
                                             allow_soft_placement=True,
                                             log_device_placement=False
                                           );*/

            TF_SetConfig(net->options, (void*)config,  20 , net->status);
        }

    net->session     = TF_NewSession(net->graph,net->options,net->status);

    TF_DeleteSessionOptions(net->options);
    if (TF_GetCode(net->status) != TF_OK)
        {
            TF_DeleteStatus(net->status);
            return 0;
        }
    //--------------------------------------------------------------------------------------------------------------

    return 1;
}

int unloadTensorflow(struct TensorflowInstance * net)
{
    //------------------------------------
    TF_CloseSession(net->session, net->status);
    if (TF_GetCode(net->status) != TF_OK)
        {
            std::cout << "Error closing all session";
            TF_DeleteStatus(net->status);
            return 0;
        }
    TF_DeleteSession(net->session, net->status);
    if (TF_GetCode(net->status) != TF_OK)
        {
            std::cout << "Error delete session";
            TF_DeleteStatus(net->status);
            return 0;
        }
    tf_utils::DeleteGraph(net->graph);
    tf_utils::DeleteTensor(net->inputTensor);
    tf_utils::DeleteTensor(net->outputTensor);
    //------------------------------------

    TF_DeleteStatus(net->status);
    return 1;
}





std::vector<float> predictTensorflow(struct TensorflowInstance * net,std::vector<float> input)
{
    const int printInput=0;
    const int printOutput=0;

    std::vector<float> result;
    result.clear();

    TF_Tensor* output_tensor = nullptr;

    unsigned int inputSize=input.size();
    std::vector<std::int64_t> input_dims = {1,inputSize};
    std::vector<float> input_vals = input;

    //----------------------------------------
    if (printInput)
        {
            //----------------------------------------
            std::cout << "Input vals: ";
            for (int i=0; i<inputSize; i++)
                {
                    std::cout <<  input_vals[i] << ", ";
                }
            std::cout  << std::endl;
        } //----------------------------------------
    //----------------------------------------

    TF_Tensor* input_tensor = tf_utils::CreateTensor(
                                  TF_FLOAT,
                                  input_dims.data(), input_dims.size(),
                                  input_vals.data(), input_vals.size() * sizeof(float)
                              );


    TF_SessionRun( net->session,
                   nullptr, // Run options.
                   &net->input_operation,     &input_tensor,  1, // Input tensors, input tensor values, number of inputs.
                   &net->output_operation,   &output_tensor,  1, // Output tensors, output tensor values, number of outputs.
                   nullptr, 0, // Target operations, number of targets.
                   nullptr, // Run metadata.
                   net->status // Output status.
                 );

    if (!checkAndDeallocate(net->status,"at predictTensorflow while running session"))
        { 
            fprintf(stderr,RED "Possibly a wrong number of arguments given ( %lu )\n" NORMAL,input.size());
            return result;
        }


    const auto data = static_cast<float*>(TF_TensorData(output_tensor));


    int64_t outputSizeA[4];
    TF_GraphGetTensorShape(
        net->graph,
        net->output_operation,
        outputSizeA, 2,
        net->status
    );


    if (!checkAndDeallocate(net->status,"TF_GraphGetTensorShape"))
        {
            return result;
        }




    int outputSize = outputSizeA[1];
    //----------------------------------------
    if (printOutput)
        {
            //----------------------------------------
            std::cout << "Output vals "<<outputSize<<" : ";
            for (int i=0; i<outputSize; i++)
                {
                    std::cout <<  data[i] << ", ";
                }
            std::cout  << std::endl;
        } //----------------------------------------
    //----------------------------------------




    for (int i=0; i<outputSize; i++)
        {
            result.push_back((float) data[i]);
        }

    tf_utils::DeleteTensor(input_tensor);
    tf_utils::DeleteTensor(output_tensor);
    
    return result;
}





std::vector<std::vector<float> > predictTensorflowOnArrayOfHeatmaps(
    struct TensorflowInstance * net,
    unsigned int width ,
    unsigned int height ,
    float * data,
    unsigned int heatmapWidth,
    unsigned int heatmapHeight,
    unsigned int numberOfOutputTensors
)
{
    std::vector<std::vector<float> > matrix; //This function output

    TF_Tensor* output_tensor = nullptr;
    std::vector<std::int64_t> input_dims = {1,height,width,3};


    TF_Tensor* input_tensor = tf_utils::CreateTensor(
                                  TF_FLOAT,
                                  input_dims.data(), 4,
                                  data , width * height * 3 * sizeof(float)
                              );

    TF_SessionRun( net->session,
                   nullptr, // Run options.
                   &net->input_operation,    &input_tensor,  1, // Input tensors,  input tensor values,  number of inputs.
                   &net->output_operation,   &output_tensor, 1, // Output tensors, output tensor values, number of outputs.
                   nullptr, 0, // Target operations, number of targets.
                   nullptr, // Run metadata.
                   net->status // Output status.
                 );

    if (TF_GetCode(net->status) != TF_OK)
        {
            fprintf(stderr,RED "predictTensorflowOnArrayOfHeatmaps: Error running session %u ( %s ) \n"  NORMAL, TF_GetCode(net->status),TF_Message(net->status));
            TF_DeleteStatus(net->status);
            tf_utils::DeleteTensor(input_tensor);
            return matrix;
        }

    if (output_tensor==nullptr)
        {
            fprintf(stderr,RED "Error retrieving output..\n"  NORMAL);
            TF_DeleteStatus(net->status);
            tf_utils::DeleteTensor(input_tensor);
            return matrix;
        }


    /*
      TF_Output outputO = TF_Output{net->operation, 0};
      TF_Status* s = TF_NewStatus();
      //TF_Output feed_out_0 = TF_Output{output_tensor, 0};
      int num_dims;

      // Fetch the shape, it should be completely unknown.
      num_dims = TF_GraphGetTensorNumDims(net->graph,outputO, s);
      fprintf(stderr,"Number of dimensions is %u \n",num_dims);
     */

    //===============================================================================
    int64_t outputSizeA[32]= {0};
    TF_GraphGetTensorShape(
        net->graph,
        net->output_operation,
        outputSizeA, numberOfOutputTensors,
        net->status
    );


    if (TF_GetCode(net->status) != TF_OK)
        {
            fprintf(stderr,RED "Error TF_GraphGetTensorShape for output, numberOfOutputTensors is probably wrong..! \n" NORMAL);
            TF_DeleteStatus(net->status);
            tf_utils::DeleteTensor(input_tensor);
            return matrix;
        }


    //===============================================================================

    //std::cout << "OutputA vals "<<outputSizeA[0]<<","<<outputSizeA[1]<<","<<outputSizeA[2]<<","<<outputSizeA[3]<<"\n";
    //This is -1 -1 -1 19 and so we need the values and we get them as input
    if ( (outputSizeA[1]==-1) && (outputSizeA[2]==-1) )
        {
            //std::cout<<"tensorflow not reporting correct output size..\n";
        }

    /*
     * Attempting to retreive the correct shape ..
      //===============================================================================
      TF_Operation *operation  = TF_GraphOperationByName(net->graph, net->outputLayerName);
      TF_Output operation_out = {operation, 0};
      int64_t outputSizeB[32];
      TF_OperationGetAttrShape(
                               operation,
                               "shape",
                               outputSizeB, 4,
                               net->status
                              );
      std::cout << "OutputB "<<net->outputLayerName<<" vals "<<outputSizeB[0]<<", cols = "<<outputSizeB[1]<<", rows = "<<outputSizeB[2]<<", hm = "<<outputSizeB[3]<<"\n";
      //===============================================================================
    */
//  const int num_outputs = TF_OperationNumOutputs(operation);
//  std::cout << "TF_OperationNumOutputs "<<num_outputs<<"\n";



//Retreive output..
    float * out_p = static_cast<float*>(TF_TensorData(output_tensor));
    if (out_p!=nullptr)
        {
            //Rows and columns should be automatically extracted,however
            //tensorflow and TF_GraphGetTensorShape returns -1,-1 as their dimensions
            //https://github.com/tensorflow/tensorflow/blob/master/tensorflow/c/c_api.h#L239
            unsigned int rows = heatmapWidth; //outputSizeA[numberOfOutputTensors-3];
            unsigned int cols = heatmapHeight; //outputSizeA[numberOfOutputTensors-2];
            unsigned int hm   = outputSizeA[numberOfOutputTensors-1];

            //For each of the output heatmaps
            for(int i=0; i<hm; ++i)
                {
                    std::vector<float> heatmap(rows*cols);
                    //For each of the rows of a particular heatmap
                    for(int r=0; r<rows; ++r)
                        {
                            //For each of the cols of a particular heatmap
                            for(int c=0; c<cols; ++c)
                                {
                                    int pos = (r*cols+c) * hm + i;
                                    heatmap[r*cols+c] = out_p[pos];
                                }
                        }
                    //Retreive the values in our matrix
                    matrix.push_back(heatmap);
                }
        } //We have output..
        
    tf_utils::DeleteTensor(input_tensor);
    tf_utils::DeleteTensor(output_tensor);
    return matrix;
}
