#include <stdlib.h>
#include <stdio.h>
#include <tensorflow/c/c_api.h>

void NoOpDeallocator(void* data, size_t a, void* b) {}

int main()
{
    //********* Read model
    TF_Graph* Graph = TF_NewGraph();
    TF_Status* Status = TF_NewStatus();

    TF_SessionOptions* SessionOpts = TF_NewSessionOptions();
    TF_Buffer* RunOpts = NULL;
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
    const char* saved_model_dir = "/home/ammar/Documents/Programming/DNNTracker/tensorflow2GPU/src/tmp.pb/";
    const char* tags = "serve"; // default model serving tag; can change in future
    int ntags = 1;

    TF_Session* Session = TF_LoadSessionFromSavedModel(SessionOpts, RunOpts, saved_model_dir, &tags, ntags, Graph, NULL, Status);
    if(TF_GetCode(Status) == TF_OK)
    {
        printf("TF_LoadSessionFromSavedModel OK\n");
    }
    else
    {
        printf("%s",TF_Message(Status));
    }
    
    char inputTensor[]={"serving_default_input_front"};
    char outputTensor[]={"StatefulPartitionedCall"};

    //****** Get input tensor
    //TODO : need to use saved_model_cli to read saved_model arch
    int NumInputs = 1;
    TF_Output* Input = (TF_Output*)malloc(sizeof(TF_Output) * NumInputs);

    TF_Output t0 = {TF_GraphOperationByName(Graph,inputTensor), 0};
    if(t0.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName %s\n",inputTensor);
    else
	printf("TF_GraphOperationByName %s is OK\n",inputTensor);
    
    Input[0] = t0;
    
    //********* Get Output tensor
    int NumOutputs = 1;
    TF_Output* Output = (TF_Output*)malloc(sizeof(TF_Output) * NumOutputs);

    TF_Output t2 = {TF_GraphOperationByName(Graph,outputTensor), 0};
    if(t2.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName %s\n",outputTensor);
    else	
	printf("TF_GraphOperationByName %s is OK\n",outputTensor);
    
    Output[0] = t2;

    //********* Allocate data for inputs & outputs
    TF_Tensor** InputValues  = (TF_Tensor**) malloc(sizeof(TF_Tensor*)*NumInputs);
    TF_Tensor** OutputValues = (TF_Tensor**) malloc(sizeof(TF_Tensor*)*NumOutputs);

    int ndims = 2;
    int64_t dims[] = {1,322};
    float data[1*322] ;//= {1,1,1,1,1,1,1,1,1,1};
    for(int i=0; i< (1*322); i++)
    {
        data[i] = 1.00;
    }
    int ndata = sizeof(float)*1*322 ;// This is tricky, it number of bytes not number of element

    TF_Tensor* int_tensor = TF_NewTensor(TF_FLOAT, dims, ndims, data, ndata, &NoOpDeallocator, 0);
    if (int_tensor != NULL)
    {
        printf("TF_NewTensor is OK\n");
    }
    else
	printf("ERROR: Failed TF_NewTensor\n");
    
    InputValues[0] = int_tensor;
    
    // //Run the Session
    TF_SessionRun(Session, NULL, Input, InputValues, NumInputs, Output, OutputValues, NumOutputs, NULL, 0,NULL , Status);

    if(TF_GetCode(Status) == TF_OK)
    {
        printf("Session is OK\n");
    }
    else
    {
        printf("%s",TF_Message(Status));
    }

    // //Free memory
    TF_DeleteGraph(Graph);
    TF_DeleteSession(Session, Status);
    TF_DeleteSessionOptions(SessionOpts);
    TF_DeleteStatus(Status);


    void* buff = TF_TensorData(OutputValues[0]);
    float* offsets = (float*)buff;
    printf("Result Tensor :\n");
    for(int i=0;i<42;i++)
    {
        if(i>0) { printf(","); } 
        printf("%f",offsets[i]);
    }
    
    printf("\n");
}
