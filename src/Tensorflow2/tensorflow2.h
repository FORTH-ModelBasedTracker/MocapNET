/** @file tensorflow2.h
 *  @brief  A header-only tensorflow 2.x wrapper automization to make your neuralnetwork-lives easier.
 *  Repository : https://github.com/AmmarkoV/PThreadWorkerPool
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef TENSORFLOW_2_H_INCLUDED
#define TENSORFLOW_2_H_INCLUDED

#include <tensorflow/c/c_api.h> // TensorFlow C API header
#include <stdlib.h>


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

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
    TF_Output input_operation;

    //Output
    //-------------------------
    TF_Output t2;
    TF_Output* output;
    TF_Tensor*  outputTensor;
    TF_Tensor** outputValues;
    unsigned int outputElements;
    int numberOfOutputTensors;
    TF_Output output_operation; 
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

static int tf2_setCPUExecutionSessionConfiguration(struct Tensorflow2Instance * tf2i)
{
  //How do you end up with this byte array you might ask ?
  unsigned char config[] = {0xa,0x7,0xa,0x3,0x43,0x50,0x55,0x10,0x1,0xa,0x7,0xa,0x3,0x47,0x50,0x55,0x10,0x0,0x38,0x1};
  //Good Question, you use the python code and extract the configuration bytes and copy paste them here..
  //https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/Tensorflow/createTensorflowConfigurationForC.py
  /*net->session = tf.ConfigProto(
                                  device_count={'CPU' : 1, 'GPU' : 0},
                                  allow_soft_placement=True,
                                  log_device_placement=False
                                 );*/

  TF_SetConfig(tf2i->sessionOptions, (void*)config,  20 , tf2i->status);
  return (TF_GetCode(tf2i->status) == TF_OK); 
}

//https://github.com/AmmarkoV/AmmarServer/blob/master/src/AmmServerlib/AString/AString.c#L235
static char * tf2_readFileToMem(const char * filename,unsigned int *length )
{
    *length = 0;
    FILE * pFile = fopen ( filename , "rb" );

    if (pFile==0)
    {
        fprintf(stderr,RED "tf2_readFileToMem: failed\n" NORMAL);
        fprintf(stderr,RED "Could not read file %s \n" NORMAL,filename);
        return 0;
    }

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    unsigned long lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    unsigned long bufferSize = sizeof(char)*(lSize+1);
    char * buffer = (char*) malloc (bufferSize);
    if (buffer == 0 )
    {
        fprintf(stderr,RED "tf2_readFileToMem: Could not allocate enough memory for file %s \n" NORMAL,filename);
        fclose(pFile);
        return 0;
    }

    // copy the file into the buffer:
    size_t result = fread (buffer,1,lSize,pFile);
    if (result != lSize)
    {
        free(buffer);
        fprintf(stderr,RED "tf2_readFileToMem: Could not read the whole file onto memory %s \n" NORMAL,filename);
        fclose(pFile);
        return 0;
    }

    /* the whole file is now loaded in the memory buffer. */

    // terminate
    fclose (pFile);

    buffer[lSize]=0; //Null Terminate Buffer!
    *length = (unsigned int) lSize;
    return buffer;
}



static void tf2_deallocateBuffer(void* data, size_t)
{
    free(data);
}




static int tf2_loadFrozenGraph( 
                         struct Tensorflow2Instance * tf2i,
                         const char* graphPath,
                         const char * inputTensor,
                         const char * outputTensor,
                         unsigned int forceCPU
                        )
{
    fprintf(stderr,"LoadGraph %s using TensorFlow Version: %s\n",graphPath,TF_Version());
    if (graphPath == 0)
        {
            fprintf(stderr,RED "Cannot load graph with null path..\n" NORMAL);
            return 0;
        }
    
    unsigned int graphInMemorySize=0;
    char * graphInMemory = tf2_readFileToMem(graphPath,&graphInMemorySize);

    if (graphInMemory == 0)
        {
            fprintf(stderr,RED "Cannot read buffer from file %s ..\n" NORMAL,graphPath);
            return 0;
        }

    tf2i->graph = TF_NewGraph();
    tf2i->status = TF_NewStatus();
    TF_ImportGraphDefOptions* opts = TF_NewImportGraphDefOptions();
    
    
    TF_Buffer* tfBufferWithGraph = TF_NewBuffer();
    tfBufferWithGraph->data = graphInMemory;
    tfBufferWithGraph->length = graphInMemorySize;
    tfBufferWithGraph->data_deallocator = tf2_deallocateBuffer;

    TF_GraphImportGraphDef(tf2i->graph,tfBufferWithGraph,opts,tf2i->status);
    TF_DeleteImportGraphDefOptions(opts);
    

    if (TF_GetCode(tf2i->status) != TF_OK)
        {
            fprintf(stderr,RED "Error importing graph definition..  (%u)\n" NORMAL,TF_GetCode(tf2i->status));
            switch  (TF_GetCode(tf2i->status))
                {
                case TF_INVALID_ARGUMENT :
                    fprintf(stderr,RED "Invalid Argument in graph..\n" NORMAL);
                break;
                };
            TF_DeleteGraph(tf2i->graph);
            tf2i->graph = 0;
        }

    TF_DeleteStatus(tf2i->status); 
    TF_DeleteBuffer(tfBufferWithGraph); 
    graphInMemorySize=0;
    
      //--------------------------------------------------------------------------------------------------------------
    tf2i->input_operation  = {TF_GraphOperationByName(tf2i->graph, inputTensor), 0};
    if (tf2i->input_operation.oper == nullptr)
        { 
            fprintf(stderr,RED "Can't init input for %s \n" NORMAL,inputTensor);
            return 0;
        } 

    tf2i->output_operation = {TF_GraphOperationByName(tf2i->graph, outputTensor), 0};
    if (tf2i->output_operation.oper == nullptr)
        { 
            fprintf(stderr,RED "Can't init output for %s \n" NORMAL,outputTensor);
            return 0;
        }
    //--------------------------------------------------------------------------------------------------------------
 

    //--------------------------------------------------------------------------------------------------------------
    tf2i->status             = TF_NewStatus();
    tf2i->sessionOptions     = TF_NewSessionOptions();
    if (forceCPU) { tf2_setCPUExecutionSessionConfiguration(tf2i); }
    
    
    tf2i->session = TF_NewSession(tf2i->graph,tf2i->sessionOptions,tf2i->status);

    TF_DeleteSessionOptions(tf2i->sessionOptions);
    if (TF_GetCode(tf2i->status) != TF_OK)
        {
            TF_DeleteStatus(tf2i->status);
            return 0;
        }
    //--------------------------------------------------------------------------------------------------------------


    return 1;
}



static int tf2_loadModel(struct Tensorflow2Instance * tf2i,const char * path,unsigned int inputElements,unsigned int outputElements,unsigned int forceCPU)
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
    if (forceCPU) { tf2_setCPUExecutionSessionConfiguration(tf2i); }
    
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
        fprintf(stderr,RED "Error loading model : %s \n" NORMAL,TF_Message(tf2i->status));
        return 0;
    }


    //Allocate input tensor
    //--------------------------------------------------------------------------------
    tf2i->numberOfInputTensors = 1;
    tf2i->input = (TF_Output*)malloc(sizeof(TF_Output) * tf2i->numberOfInputTensors);
    tf2i->t0 = {TF_GraphOperationByName(tf2i->graph,inputTensorName), 0};
    if(tf2i->t0.oper == NULL)
    {
        fprintf(stderr,RED "ERROR: Failed TF_GraphOperationByName %s\n" NORMAL,inputTensorName);
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
        fprintf(stderr,RED "ERROR: Failed TF_GraphOperationByName %s\n" NORMAL,outputTensorName);
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

