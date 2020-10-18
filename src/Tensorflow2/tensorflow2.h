/** @file tensorflow2.h
 *  @brief  A header-only tensorflow 2.x wrapper automization to make your neuralnetwork-lives easier.
 *  Repository : https://github.com/AmmarkoV/PThreadWorkerPool
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef TENSORFLOW_2_H_INCLUDED
#define TENSORFLOW_2_H_INCLUDED

#include <tensorflow/c/c_api.h> // TensorFlow C API header

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief A structure that holds all of the relevant information for a tensorflow instance
 * This is used to simplify context switching and reduce the complexity of the library
 */
struct Tensorflow2Instance
{
    char modelPath[1024];

    //Network instance
    //-------------------------
    TF_Graph* graph;
    TF_Status* status;
    TF_Session* session;
    TF_SessionOptions* sessionOptions;

    //Input
    //-------------------------
    TF_Output t0;
    TF_Output* input;
    TF_Tensor*  inputTensor;
    TF_Tensor** inputValues;
    unsigned int inputElements;
    int numberOfInputTensors;

    //Output
    //-------------------------
    TF_Output t2;
    TF_Output* output;
    TF_Tensor*  outputTensor;
    TF_Tensor** outputValues;
    unsigned int outputElements;
    int numberOfOutputTensors;
    //-------------------------
};

void NoOpDeallocator(void* data, size_t a, void* b) {}

//saved_model_cli show --dir tmp.pb --tag_set serve --signature_def serving_default
/*
The given SavedModel SignatureDef contains the following input(s):
  inputs['input_front'] tensor_info:
      dtype: DT_FLOAT
      shape: (-1, 322)
      name: serving_default_input_front:0
The given SavedModel SignatureDef contains the following output(s):
  outputs['result_front'] tensor_info:
      dtype: DT_FLOAT
      shape: (-1, 42)
      name: StatefulPartitionedCall:0
Method name is: tensorflow/serving/predict
*/


static int tf2_loadModel(struct Tensorflow2Instance * tf2i,const char * path,unsigned int inputElements,unsigned int outputElements)
{
    //Tensorflow boilerplate names, fingers crossed that they don't change them..
    char inputTensorName[]= {"serving_default_input_front"}; //Default input layer
    char outputTensorName[]= {"StatefulPartitionedCall"};   //Default output layer
    const char* tags = "serve"; // default model serving tag; can change in future
    int ntags = 1;
    //--------------------------------------------------------------------------

    //Remember parameters..
    snprintf(tf2i->modelPath,1024,"%s",path);
    tf2i->inputElements=inputElements;
    tf2i->outputElements=outputElements;

    //Allocate stuff..
    tf2i->graph = TF_NewGraph();
    tf2i->status = TF_NewStatus();
    tf2i->sessionOptions = TF_NewSessionOptions();
    TF_Buffer* runOptions = NULL;

    //Actual loading from a saved model assuming :
    //https://www.tensorflow.org/api_docs/python/tf/saved_model/save
    tf2i->session = TF_LoadSessionFromSavedModel(
                        tf2i->sessionOptions,
                        runOptions,
                        path,
                        &tags,
                        ntags,
                        tf2i->graph,
                        NULL,
                        tf2i->status
                    );
    if(TF_GetCode(tf2i->status) != TF_OK)
    {
        fprintf(stderr,"Error loading model : %s \n",TF_Message(tf2i->status));
        return 0;
    }


    //Allocate input tensor
    //--------------------------------------------------------------------------------
    tf2i->numberOfInputTensors = 1;
    tf2i->input = (TF_Output*)malloc(sizeof(TF_Output) * tf2i->numberOfInputTensors);
    tf2i->t0 = {TF_GraphOperationByName(tf2i->graph,inputTensorName), 0};
    if(tf2i->t0.oper == NULL)
    {
        fprintf(stderr,"ERROR: Failed TF_GraphOperationByName %s\n",inputTensorName);
        return 0;
    }
    tf2i->input[0] = tf2i->t0;
    //--------------------------------------------------------------------------------


    //Allocate Output tensor
    //--------------------------------------------------------------------------------
    tf2i->numberOfOutputTensors = 1;
    tf2i->output = (TF_Output*)malloc(sizeof(TF_Output) * tf2i->numberOfOutputTensors);

    tf2i->t2 = {TF_GraphOperationByName(tf2i->graph,outputTensorName), 0};
    if(tf2i->t2.oper == NULL)
    {
        fprintf(stderr,"ERROR: Failed TF_GraphOperationByName %s\n",outputTensorName);
        return 0;
    }
    tf2i->output[0] = tf2i->t2;
    //--------------------------------------------------------------------------------


    //Allocate memory for input & output data
    tf2i->inputValues  = (TF_Tensor**) malloc(sizeof(TF_Tensor*)*tf2i->numberOfInputTensors);
    tf2i->outputValues = (TF_Tensor**) malloc(sizeof(TF_Tensor*)*tf2i->numberOfOutputTensors);
    return ( (tf2i->inputValues!=0) && (tf2i->outputValues!=0) );
}


static void tf2_unloadModel(struct Tensorflow2Instance * tf2i)
{
    // //Free memory
    TF_DeleteGraph(tf2i->graph);
    TF_DeleteSession(tf2i->session, tf2i->status);
    TF_DeleteSessionOptions(tf2i->sessionOptions);
    TF_DeleteStatus(tf2i->status);
}


static int tf2_run(struct Tensorflow2Instance * tf2i,int64_t * dimensions,unsigned int dimensionsNumber,float * data,unsigned int dataSizeBytes)
{
    TF_Tensor* int_tensor = TF_NewTensor(
                                TF_FLOAT,
                                dimensions,
                                dimensionsNumber,
                                data,
                                dataSizeBytes,
                                &NoOpDeallocator,
                                0
                            );
    if (int_tensor == NULL)
    {
        fprintf(stderr,"ERROR: Failed TF_NewTensor\n");
        return 0;
    }

    tf2i->inputValues[0] = int_tensor;

    //Run the Session
    TF_SessionRun(
        tf2i->session,
        NULL,
        tf2i->input,
        tf2i->inputValues,
        tf2i->numberOfInputTensors,
        tf2i->output,
        tf2i->outputValues,
        tf2i->numberOfOutputTensors,
        NULL,
        0,
        NULL,
        tf2i->status
    );

    TF_DeleteTensor(int_tensor);

    if(TF_GetCode(tf2i->status) != TF_OK)
    {
        fprintf(stderr,"Error running network : %s",TF_Message(tf2i->status));
        return 0;
    }

    return 1;
}


#ifdef __cplusplus
}
#endif

#endif

