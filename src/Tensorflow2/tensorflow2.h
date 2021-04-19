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


typedef unsigned short float16;


/**
 * @brief A structure that holds all of the relevant information for a tensorflow instance
 * This is used to simplify context switching and reduce the complexity of the library
 */
struct Tensorflow2Instance
{
    char modelPath[1025];

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
    char inputIsHalfFloats;
    unsigned int inputElements;
    int numberOfInputTensors; 

    //Output
    //-------------------------
    TF_Output output_operation;
    TF_Output* output;
    TF_Tensor*  outputTensor;
    TF_Tensor** outputValues;
    char outputIsHalfFloats;
    unsigned int outputElements;
    int numberOfOutputTensors; 
    
    //Buffer for itermediate data
    //-------------------------
    unsigned int bufferSize;
    float * buffer;
};

static void tf2_noOpDeallocator(void* data, size_t a, void* b) {}


static void tf2_deallocateBuffer(void* data, size_t)
{
    if (data!=0)
       { 
         fprintf(stderr,"tf2_deallocateBuffer\n");
         free(data); 
       }
}


#if INTEL_OPTIMIZATIONS
#include <immintrin.h>
#include <emmintrin.h>

//https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/intrinsics/intrinsics-for-converting-half-floats/intrinsics-for-converting-half-floats-1.html
//AVX 
//This intrinsic takes a 32-bit float value, x , and converts it to a half-float value, which is returned.
//__m128 _mm_cvtph_ps(__m128i x);
//This intrinsic takes four packed half-float values and converts them to four 32-bit float values, which are returned. The upper 64-bits of x are ignored. The lower 64-bits are taken as four 16-bit float values for conversion.
//__m128i _mm_cvtps_ph(_m128 x, int imm);

static float16 convertFloat32ToFloat16(float f)
{ 
    /* 
      (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC) // round to nearest, and suppress exceptions
      (_MM_FROUND_TO_NEG_INF |_MM_FROUND_NO_EXC)     // round down, and suppress exceptions
      (_MM_FROUND_TO_POS_INF |_MM_FROUND_NO_EXC)     // round up, and suppress exceptions
      (_MM_FROUND_TO_ZERO |_MM_FROUND_NO_EXC)        // truncate, and suppress exceptions
       _MM_FROUND_CUR_DIRECTION // use MXCSR.RC; see _MM_SET_ROUNDING_MODE
     * */
    int imm = _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC;
    //https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_cvtss_sh&expand=4979,5019,4979,1898
    return _cvtss_sh(f,imm); 
}
 
static float convertFloat16ToFloat32(float16 h)
{
    //https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_cvtsh_ss&expand=4979,5019,4979,1898,1873
    return _cvtsh_ss(h);
}
#else
static float16 convertFloat32ToFloat16(float f)
{
  //https://en.wikipedia.org/wiki/Half-precision_floating-point_format
  //https://en.wikipedia.org/wiki/IEEE_754-2008_revision
  //https://stackoverflow.com/questions/3026441/float32-to-float16/3026505
  #warning "convertFloat32ToFloat16 is not correctly implemented on non SSE equipped builds"
  unsigned int * fltInt32 = ( unsigned int * ) &f;
  float16 h;

  h = (*fltInt32 >> 31) << 5;
  unsigned short tmp = (*fltInt32 >> 23) & 0xff;
  tmp = (tmp - 0x70) & ((unsigned int)((int)(0x70 - tmp) >> 4) >> 27);
  h = (h | tmp) << 10;
  h |= (*fltInt32 >> 13) & 0x3ff;  
  
  return h;
}

static float convertFloat16ToFloat32(float16 h)
{
  #warning "convertFloat16ToFloat32 is not properly implemented on non SSE equipped builds"
  fprintf(stderr,"convertFloat16ToFloat32 not working.. ");
  //http://www.fox-toolkit.org/ftp/fasthalffloatconversion.pdf
  float f = ((h&0x8000)<<16) | (((h&0x7c00)+0x1C000)<<13) | ((h&0x03FF)<<13);
  return f;
}
#endif



static int tf2_checkModelUsingExternalTool(const char * path)
{
  char commandToCheck[4096];
  snprintf(commandToCheck,2000,"saved_model_cli show --dir %s --tag_set serve --signature_def serving_default",path);
  int i = system(commandToCheck);
 
  return (i==0);
}





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
    if (length==0)   { return 0; }
    if (filename==0) { return 0; }
    //-----------------------------------------

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
    tf2i->input = (TF_Output*) malloc(sizeof(TF_Output) * tf2i->numberOfInputTensors);
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
    tf2i->output = (TF_Output*) malloc(sizeof(TF_Output) * tf2i->numberOfOutputTensors);

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
    if (graphPath == 0)
        {
            fprintf(stderr,"Cannot load graph with null path..\n");
            return 0;
        }
    fprintf(stderr,"LoadGraph %s using TensorFlow Version: %s\n",graphPath,TF_Version());
    
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
    tf2i->sessionOptions = TF_NewSessionOptions();
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
    char inputTensorName[1025]={0}; //= {"serving_default_input_front"}; //Default input layer
    snprintf(inputTensorName,1024,"serving_default_%s",inputTensor);
    //snprintf(inputTensorName,1024,"%s",inputTensor);
    
    char outputTensorName[1025]={0}; //= {"StatefulPartitionedCall"};   //Default output layer
    snprintf(outputTensorName,1024,"StatefulPartitionedCall");
    
    //--------------------------------------------------------------------------
    int ntags = 1;
    
    //char tags[1025]={0}; //= {"StatefulPartitionedCall"};   //Default output layer
    //snprintf(tags,1024,"serve");
    //const char* tags = "serve"; // default model serving tag; can change in future
    const char* tags[] = {"serve"};
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
                        tags,
                        ntags,
                        tf2i->graph,
                        NULL,
                        tf2i->status
                    );
    if(TF_GetCode(tf2i->status) != TF_OK)
    {
        fprintf(stderr,"Error loading model : %s \n",TF_Message(tf2i->status));
        
        
        tf2_checkModelUsingExternalTool(tf2i->modelPath);
        
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
     
     
    if (tf2i->buffer!=0)
    {
        free(tf2i->buffer);
        tf2i->buffer=0;
        tf2i->bufferSize=0;
    }
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
    int result = 0;
    TF_Tensor* int_tensor = NULL; 
    
    
    if (tf2i->inputIsHalfFloats)
    {
      //In an attempt not to perform unneccessary reallocations 
      //I rewrite the float array with halfs..
      unsigned int dataSizeElements = dataSizeBytes / sizeof(float);
      unsigned int convertedDataSizeBytes = sizeof(float16) * dataSizeElements;
       
       float16 * dataAsFloat16 = (float16*) data;
       float buffer=0.0;
       
       for (int i=0; i<dataSizeElements; i++)
        {
          //Have an intermediate location..
          buffer = data[i];
          dataAsFloat16[i] = convertFloat32ToFloat16(buffer);
        }
       int_tensor = TF_NewTensor(
                                  TF_HALF,
                                  dimensions,
                                  dimensionsNumber,
                                  dataAsFloat16,
                                  convertedDataSizeBytes,
                                  &tf2_noOpDeallocator,
                                  0
                                 ); 
    } else
    {
      //Regular floats ready to be transported to the tensor
      int_tensor = TF_NewTensor(
                                TF_FLOAT,
                                dimensions,
                                dimensionsNumber,
                                data,
                                dataSizeBytes,
                                &tf2_noOpDeallocator,
                                0
                               );
    }
     
    //----------------------------------------------------------------
    
    if (int_tensor == NULL)
    {
        fprintf(stderr,"ERROR tf2_run: Failed TF_NewTensor\n");
        fprintf(stderr,"Trying to create a tensor with %u dimensions : ",dimensionsNumber);
        fprintf(stderr,"{ ");
        for (int i=0; i<dimensionsNumber; i++)
        {
             fprintf(stderr,"%ld ",dimensions[i]); 
        }
        fprintf(stderr,"}\n");
        
        fprintf(stderr,"Data output size : %u \n",dataSizeBytes); 
        return 0;
    } 
      else
    {
    tf2i->inputValues[0] = int_tensor;
     
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
        fprintf(stderr,"tf2_run: Error running %s network, %s\n",tf2i->modelPath,TF_Message(tf2i->status));

        fprintf(stderr,"dimensionsNumber=%u\n",dimensionsNumber);
        fprintf(stderr,"dataSizeBytes=%u\n",dataSizeBytes); 
        tf2_checkModelUsingExternalTool(tf2i->modelPath);

        result = 0;
    }

     TF_DeleteTensor(int_tensor);
     tf2i->inputValues[0] =0;
     int_tensor=0; 
     result=1;
    }

    return result;
}


#ifdef __cplusplus
}
#endif

#endif