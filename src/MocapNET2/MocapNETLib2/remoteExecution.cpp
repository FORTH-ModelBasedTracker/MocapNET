#include "remoteExecution.hpp"
#include "../../../dependencies/InputParser/InputParser_C.h"


#if USE_NETWORKING
 #include "../../../dependencies/AmmarServer/src/AmmClient/AmmClient.h"
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SEND_RECV_BUFFER_SIZE 16000

char * strstrDoubleNewlineLocal(char * request,unsigned int requestLength,unsigned int * endOfLine)
{
  if (request==0)             { return request; }
  if (requestLength==0)       { return request; }
  if (endOfLine==0)           { return request; }

  char * ptrA=request;
  char * ptrB=request+1;

  char * ptrEnd = request + requestLength;

  //fprintf(stderr,"\strstrDoubleNewline for 13 10 13 10 on a buffer with %u bytes of data : ",requestLength);
   while (ptrB<ptrEnd)
    {
      if ( ( (*ptrA==10) && (*ptrB==10) ) || (*ptrA==0) )
        {
         ++ptrB;

         *ptrA=0; //Also make null terminated string..
         *endOfLine = ptrA-request;

         //fprintf(stderr,"done\n");
         return ptrB;
        }
       //fprintf(stderr,"%c(%u) ",*ptrA,*ptrA);
      ++ptrA;   ++ptrB;
    }

  //fprintf(stderr,"not found\n");
 return request;
}


std::vector<float> remoteExecution(struct MocapNET2 * mnet,const std::vector<float> &mnetInput)
{
 std::vector<float> result; 
 
 #if USE_NETWORKING
 fprintf(stderr,"remoteExecution :\n"); 
 //-----------------------------------------------------
  char requestBuffer[SEND_RECV_BUFFER_SIZE+1]={0};
  char part[128]={0};
  
  snprintf(requestBuffer,SEND_RECV_BUFFER_SIZE,"control.html?skeleton="); 
  for (int i=0; i<mnetInput.size(); i++)
  {
    
    if (mnetInput[i]==0) { snprintf(part,128,"0"); } else
                         { snprintf(part,128,"%0.4f",mnetInput[i]); }   
    //TODO : unsafe strcat
    strcat(requestBuffer,part);  
    if (i<mnetInput.size()-1)         
              { strcat(requestBuffer,","); }    
  }
  
  //fprintf(stderr,"Request is %lu bytes long :  %s\n",strlen(requestBuffer),requestBuffer);

  struct AmmClient_Instance * instance = (struct AmmClient_Instance *) mnet->remoteContext;
  
  char resultBuffer[SEND_RECV_BUFFER_SIZE+1]={0};
  unsigned int filecontentSize=SEND_RECV_BUFFER_SIZE;
  
  if (
  AmmClient_RecvFile(
                     instance,
                     requestBuffer,
                     resultBuffer,
                     &filecontentSize,
                     1,//keepAlive,
                     0// reallyFastImplementation
                    )
     )
     {                 
  unsigned int startOfData=0;          
  char * onlyResults = strstrDoubleNewlineLocal(resultBuffer,filecontentSize,&startOfData);         
  if (onlyResults==0) {
                        fprintf(stderr,"Couldnt find body.. \n");
                        onlyResults=resultBuffer;
                      }       
                    
  //fprintf(stderr,"Got back:\n %s \n\n",resultBuffer);                    

  struct InputParserC * ipc = InputParser_Create(4096,1); 
  InputParser_SetDelimeter(ipc,0,',');
  
  if (ipc!=0)
      { 
        result.clear();  
        //fprintf(stderr,"Disregarding headers we have: %s \n\n",onlyResults);       
        int numberOfArguments = InputParser_SeperateWords(ipc,onlyResults,1);
        //fprintf(stderr,"Skeleton Arguments : %u\n",numberOfArguments);  
        for (int argument=0; argument<numberOfArguments; argument++) //InputParser_GetNumberOfArguments(ipc)
          {
            //fprintf(stderr,"%u - %0.2f \n",argument,InputParser_GetWordFloat(ipc,argument));
            result.push_back(InputParser_GetWordFloat(ipc,argument));
          }
       InputParser_Destroy(ipc);
       
       fprintf(stderr,"We got back %lu arguments from network\n",result.size()); 
       return result;
      }

         
     }   
 //-----------------------------------------------------
 #else
   fprintf(stderr,"remoteExecution cannot be used without it beeing compiled in.. Please run initialize.sh and opt in for AmmarServer installation\n"); 
 #endif
 return result;
}




void * intializeRemoteExecution(const char * ip,unsigned int port,unsigned int socketTimeoutSeconds)
{
 #if USE_NETWORKING
  return (void *) AmmClient_Initialize(ip,port,socketTimeoutSeconds);
 #endif
 return 0;
}


int stopRemoteExecution(void * instance)
{
 #if USE_NETWORKING
  return AmmClient_Close((struct AmmClient_Instance *) instance);
 #endif
 return 0;  
}
