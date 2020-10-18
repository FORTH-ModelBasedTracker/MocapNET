#include <stdlib.h>
#include <stdio.h> 
#include <tensorflow/c/c_api.h> // TensorFlow C API header
#include <vector>


/**
 * @brief A structure that holds all of the relevant information for a tensorflow instance
 * This is used to simplify context switching and reduce the complexity of the library
 */
struct Tensorflow2Instance
{
  char modelPath[1024];
  char inputLayerName[512];
  char outputLayerName[512];

  TF_Graph* graph;
  TF_Session* session;
  unsigned int inputElements;
  TF_Tensor*  inputTensor;
  TF_Tensor*  outputTensor;
  unsigned int outputElements;
 
  TF_Output* input;
  TF_Output t0;
  
  int numberOfInputTensors;
  int numberOfOutputTensors;
 
  TF_Tensor** inputValues;
  TF_Tensor** outputValues;
  
  
  TF_Output* output;
  TF_Output t2;
 
  TF_Status* status;
  TF_SessionOptions* sessionOptions;
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


int tf2_loadModel(struct Tensorflow2Instance * tf2i,const char * path,unsigned int inputDimension,unsigned int outputDimension)
{
    //Tensorflow boilerplate names, fingers crossed that they don't change them..
    char inputTensorName[]={"serving_default_input_front"}; //Default input layer
    char outputTensorName[]={"StatefulPartitionedCall"};    //Default output layer 
    const char* tags = "serve"; // default model serving tag; can change in future
    int ntags = 1;
    
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
    

    //****** Get input tensor 
    tf2i->numberOfInputTensors = 1;
    tf2i->input = (TF_Output*)malloc(sizeof(TF_Output) * tf2i->numberOfInputTensors); 
    tf2i->t0 = {TF_GraphOperationByName(tf2i->graph,inputTensorName), 0};
    if(tf2i->t0.oper == NULL)
        { 
          fprintf(stderr,"ERROR: Failed TF_GraphOperationByName %s\n",inputTensorName); 
          return 0;
        }
    
    tf2i->input[0] = tf2i->t0;
    
    //********* Get Output tensor
    tf2i->numberOfOutputTensors = 1;
    tf2i->output = (TF_Output*)malloc(sizeof(TF_Output) * tf2i->numberOfOutputTensors);

    tf2i->t2 = {TF_GraphOperationByName(tf2i->graph,outputTensorName), 0};
    if(tf2i->t2.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName %s\n",outputTensorName);
    else	
	printf("TF_GraphOperationByName %s is OK\n",outputTensorName);
    
    tf2i->output[0] = tf2i->t2;

    //********* Allocate data for inputs & outputs
    tf2i->inputValues  = (TF_Tensor**) malloc(sizeof(TF_Tensor*)*tf2i->numberOfInputTensors);
    tf2i->outputValues = (TF_Tensor**) malloc(sizeof(TF_Tensor*)*tf2i->numberOfOutputTensors);
    return 1;
}


void tf2_unloadModel(struct Tensorflow2Instance * tf2i)
{
    // //Free memory
    TF_DeleteGraph(tf2i->graph);
    TF_DeleteSession(tf2i->session, tf2i->status);
    TF_DeleteSessionOptions(tf2i->sessionOptions);
    TF_DeleteStatus(tf2i->status); 
}



int tf2_run(struct Tensorflow2Instance * tf2i,int64_t * dimensions,unsigned int dimensionsNumber,float * data,unsigned int dataSize)
{
    
    TF_Tensor* int_tensor = TF_NewTensor(TF_FLOAT, dimensions, dimensionsNumber, data, dataSize, &NoOpDeallocator, 0);
    if (int_tensor != NULL)
    {
        printf("TF_NewTensor is OK\n");
    }
    else
	printf("ERROR: Failed TF_NewTensor\n");
    
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

    if(TF_GetCode(tf2i->status) == TF_OK)
    {
        printf("Session is OK\n");
        return 1;
    }
    else
    {
        printf("%s",TF_Message(tf2i->status));
    }
  return 0;
}


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
    int ndata = sizeof(float)*1*322 ;// This is tricky, it number of bytes not number of element

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
