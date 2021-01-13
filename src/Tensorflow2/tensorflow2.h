/** @file tensorflow2.h
 *  @brief  A header-only tensorflow 2.x wrapper automization to make your neuralnetwork-lives easier.
 *  Repository : https://github.com/AmmarkoV/PThreadWorkerPool
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef TENSORFLOW2_H_INCLUDED
#define TENSORFLOW2_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <tensorflow/c/c_api.h> // TensorFlow C API header

#include <stdio.h>
#include <stdlib.h>


//#define NORMAL  "\033[0m"
//#define BLACK   "\033[30m"      /* Black */
//#define RED     "\033[31m"      /* Red */
//#define GREEN   "\033[32m"      /* Green */
//#define YELLOW  "\033[33m"      /* Yellow */
  

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
    TF_Output input_operation;
    TF_Output* input;
    TF_Tensor*  inputTensor;
    TF_Tensor** inputValues;
    unsigned int inputElements;
    int numberOfInputTensors; 

    //Output
    //-------------------------
    TF_Output output_operation;
    TF_Output* output;
    TF_Tensor*  outputTensor;
    TF_Tensor** outputValues;
    unsigned int outputElements;
    int numberOfOutputTensors; 
};

static void tf2_noOpDeallocator(void* data, size_t a, void* b) {}


static void tf2_deallocateBuffer(void* data, size_t)
{
    free(data);
}







/*
#include <tensorflow/lite/c/c_api.h>
#include <stdio.h>
#include <stdlib.h>

int tflitetest (int argc, char* argv[]) {

  TfLiteModel* model = TfLiteModelCreateFromFile("model.tflite");
  TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
  TfLiteInterpreterOptionsSetNumThreads(options, 2);
  TfLiteInterpreter* interpreter = TfLiteInterpreterCreate(model, options);
  TfLiteInterpreterAllocateTensors(interpreter);
  for(int i = 0; i < 3; i++)
  {
    printf("Iteration: %d\n", i);

    float input[49] = { 0.0 };

    TfLiteTensor* input_tensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
    TfLiteTensorCopyFromBuffer(input_tensor, input, 49 * sizeof(float));

    TfLiteInterpreterInvoke(interpreter);

    const TfLiteTensor* output_tensor = TfLiteInterpreterGetOutputTensor(interpreter, 14);

    float output[49];
    TfLiteTensorCopyToBuffer(output_tensor, output, 49 * sizeof(float));

    printf("Output: \n\n");
    for (int j = 0; j < 49; j++) {
      printf("%d: %f\n", j, output[j]);
    }

  }
  TfLiteInterpreterDelete(interpreter);
  TfLiteInterpreterOptionsDelete(options);
  TfLiteModelDelete(model);
  return 0;
}
*/




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


static char tf2_fileExists(const char * filename)
{
 FILE *fp = fopen(filename,"r");
 if( fp ) { /* exists */ fclose(fp); return 1; }
 return 0;
}

//https://github.com/AmmarkoV/AmmarServer/blob/master/src/AmmServerlib/AString/AString.c#L235
static char * tf2_readFileToMem(const char * filename,unsigned int *length )
{
    *length = 0;
    FILE * pFile = fopen ( filename , "rb" );

    if (pFile==0)
    {
        fprintf(stderr,"ERROR tf2_readFileToMem: failed\n");
        fprintf(stderr,"Could not read file %s \n",filename);
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
        fprintf(stderr,"ERROR tf2_readFileToMem: Could not allocate enough memory for file %s \n",filename);
        fclose(pFile);
        return 0;
    }

    // copy the file into the buffer:
    size_t result = fread (buffer,1,lSize,pFile);
    if (result != lSize)
    {
        free(buffer);
        fprintf(stderr,"ERROR tf2_readFileToMem: Could not read the whole file onto memory %s \n",filename);
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




static int tf2_allocateIOTensors( 
                                  struct Tensorflow2Instance * tf2i,
                                  const char * inputTensorName,
                                  const char * outputTensorName,
                                  unsigned int inputElements,
                                  unsigned int outputElements,
                                  unsigned int forceCPU
                                 )
{ 
    //Allocate input tensor
    //--------------------------------------------------------------------------------
    tf2i->numberOfInputTensors = 1;
    tf2i->input = (TF_Output*)malloc(sizeof(TF_Output) * tf2i->numberOfInputTensors);
    tf2i->input_operation = {TF_GraphOperationByName(tf2i->graph,inputTensorName), 0};
    if(tf2i->input_operation.oper == NULL)
    {
        fprintf(stderr,"ERROR: Failed TF_GraphOperationByName %s\n",inputTensorName);
        return 0;
    }
    tf2i->input[0] = tf2i->input_operation;
    //--------------------------------------------------------------------------------


    //Allocate Output tensor
    //--------------------------------------------------------------------------------
    tf2i->numberOfOutputTensors = 1;
    tf2i->output = (TF_Output*)malloc(sizeof(TF_Output) * tf2i->numberOfOutputTensors);

    tf2i->output_operation = {TF_GraphOperationByName(tf2i->graph,outputTensorName), 0};
    if(tf2i->output_operation.oper == NULL)
    {
        fprintf(stderr,"ERROR: Failed TF_GraphOperationByName %s\n",outputTensorName);
        return 0;
    }
    tf2i->output[0] = tf2i->output_operation;
    //--------------------------------------------------------------------------------


    //Allocate memory for input & output data
    tf2i->inputValues  = (TF_Tensor**) malloc(sizeof(TF_Tensor*)*tf2i->numberOfInputTensors);
    tf2i->outputValues = (TF_Tensor**) malloc(sizeof(TF_Tensor*)*tf2i->numberOfOutputTensors);
    return ( (tf2i->inputValues!=0) && (tf2i->outputValues!=0) );
}



static int tf2_loadFrozenGraph( 
                               struct Tensorflow2Instance * tf2i,
                               const char* graphPath,
                               const char * inputTensorName,
                               const char * outputTensorName,
                               unsigned int inputElements,
                               unsigned int outputElements,
                               unsigned int forceCPU
                              )
{
    fprintf(stderr,"LoadGraph %s using TensorFlow Version: %s\n",graphPath,TF_Version());
    if (graphPath == 0)
        {
            fprintf(stderr,"Cannot load graph with null path..\n");
            return 0;
        }
    
    unsigned int graphInMemorySize=0;
    char * graphInMemory = tf2_readFileToMem(graphPath,&graphInMemorySize);

    if (graphInMemory == 0)
        {
            fprintf(stderr,"Cannot read buffer from file %s ..\n",graphPath);
            return 0;
        }
        
    
    //Remember parameters..
    snprintf(tf2i->modelPath,1024,"%s",graphPath);
    tf2i->inputElements=inputElements;
    tf2i->outputElements=outputElements;

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
            fprintf(stderr,"ERROR importing graph definition..  (%u)\n",TF_GetCode(tf2i->status));
            switch  (TF_GetCode(tf2i->status))
                {
                case TF_INVALID_ARGUMENT :
                    fprintf(stderr,"Invalid Argument in graph..\n");
                break;
                };
            TF_DeleteGraph(tf2i->graph);
            tf2i->graph = 0;
        }

    TF_DeleteBuffer(tfBufferWithGraph); 
    graphInMemorySize=0;
    

    //--------------------------------------------------------------------------------------------------------------
    tf2i->sessionOptions     = TF_NewSessionOptions();
    if (forceCPU) { tf2_setCPUExecutionSessionConfiguration(tf2i); }


    tf2i->session = TF_NewSession(tf2i->graph,tf2i->sessionOptions,tf2i->status);
    //--------------------------------------------------------------------------------------------------------------
    
    return tf2_allocateIOTensors( 
                                  tf2i,
                                  inputTensorName,
                                  outputTensorName,
                                  inputElements,
                                  outputElements,
                                  forceCPU
                                 );
}



static int tf2_loadModel(
                          struct Tensorflow2Instance * tf2i,
                          const char * path,
                          const char * inputTensor,
                          unsigned int inputElements,
                          unsigned int outputElements,
                          unsigned int forceCPU
                         )
{
    //https://medium.com/analytics-vidhya/deploying-tensorflow-2-1-as-c-c-executable-1d090845055c
    //Tensorflow boilerplate names, fingers crossed that they don't change them..
    char inputTensorName[1024]; //= {"serving_default_input_front"}; //Default input layer
    snprintf(inputTensorName,1024,"serving_default_%s",inputTensor);
    //snprintf(inputTensorName,1024,"%s",inputTensor);
    
    char outputTensorName[1024]; //= {"StatefulPartitionedCall"};   //Default output layer
    snprintf(outputTensorName,1024,"StatefulPartitionedCall");
    
    //--------------------------------------------------------------------------
    int ntags = 1;
    const char* tags = "serve"; // default model serving tag; can change in future
    //const char* tags = "tensorflow/serving/predict"; // default model serving tag; can change in future
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    fprintf(stderr,"Loading model : %s | Input : %s | Output : %s\n",path,inputTensorName,outputTensorName);
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
        fprintf(stderr,"Error loading model : %s \n",TF_Message(tf2i->status));
        return 0;
    }

    return tf2_allocateIOTensors( 
                                  tf2i,
                                  inputTensorName,
                                  outputTensorName,
                                  inputElements,
                                  outputElements,
                                  forceCPU
                                 );
}


static int tf2_unloadModel(struct Tensorflow2Instance * tf2i)
{
    // //Free memory
    TF_DeleteGraph(tf2i->graph);
    if(TF_GetCode(tf2i->status) != TF_OK)
    {
        fprintf(stderr,"tf2_unloadModel: Error deleting graph (%s) \n",TF_Message(tf2i->status));
        return 0;
    }
    
    
    TF_DeleteSession(tf2i->session, tf2i->status);
    if(TF_GetCode(tf2i->status) != TF_OK)
    {
        fprintf(stderr,"tf2_unloadModel: Error deleting session (%s) \n",TF_Message(tf2i->status));
        return 0;
    }
    
    
    TF_DeleteSessionOptions(tf2i->sessionOptions);
    if(TF_GetCode(tf2i->status) != TF_OK)
    {
        fprintf(stderr,"tf2_unloadModel: Error deleting session options (%s) \n",TF_Message(tf2i->status));
        return 0;
    }
    
    
    TF_DeleteStatus(tf2i->status);
    //Can't check status having deleted the status ..
     
    return 1;
}


static int tf2_run(
                   struct Tensorflow2Instance * tf2i,
                   int64_t * dimensions,
                   unsigned int dimensionsNumber,
                   float * data,
                   unsigned int dataSizeBytes
                   )
{
    TF_Tensor* int_tensor = TF_NewTensor(
                                          TF_FLOAT,
                                          dimensions,
                                          dimensionsNumber,
                                          data,
                                          dataSizeBytes,
                                          &tf2_noOpDeallocator,
                                          0
                                         );
    if (int_tensor == NULL)
    {
        fprintf(stderr,"ERROR tf2_run: Failed TF_NewTensor\n");
        fprintf(stderr,"Trying to create a tensor with %u dimensions : ",dimensionsNumber);
        fprintf(stderr,"{ ");
        for (int i=0; i<dimensionsNumber; i++)
        {
             fprintf(stderr,"%lu ",dimensions[i]); 
        }
        fprintf(stderr,"}\n");
        
        fprintf(stderr,"Data output size : %u \n",dataSizeBytes);
        
        return 0;
    }

    tf2i->inputValues[0] = int_tensor;
    
    int result = 1;
    //Run the Session
    TF_SessionRun(
                   tf2i->session,
                   NULL, //run options
                   tf2i->input,  tf2i->inputValues,  tf2i->numberOfInputTensors,
                   tf2i->output, tf2i->outputValues, tf2i->numberOfOutputTensors,
                   NULL,  0,  
                   NULL, //Metadata
                   tf2i->status
                 );

    if(TF_GetCode(tf2i->status) != TF_OK)
    {
        fprintf(stderr,"tf2_run: Error running network, %s",TF_Message(tf2i->status));
        
        fprintf(stderr,"dimensionsNumber=%u\n",dimensionsNumber);
        fprintf(stderr,"dataSizeBytes=%u\n",dataSizeBytes);
         
        result = 0;
    }

    TF_DeleteTensor(int_tensor);


    return result;
}


#ifdef __cplusplus
}
#endif

#endif

