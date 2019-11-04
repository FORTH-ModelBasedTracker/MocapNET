#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

#include "../AmmarServer/src/AmmServerlib/AmmServerlib.h"

#include "../MocapNETLib/InputParser_C.h"

#define DEFAULT_BINDING_PORT 8080  // <--- Change this to 80 if you want to bind to the default http port..!


#define WORKING_PATH "MocapNETServerHTTP/res/"
char webserver_root[MAX_FILE_PATH]=WORKING_PATH; // <- change this to the directory that contains your content if you dont want to use the default public_html dir..
char templates_root[MAX_FILE_PATH]="public_html/templates/";


#include <cstdlib>

#include "../MocapNETLib/mocapnet.hpp"

unsigned int requestsReceived=0;
unsigned int requestsServed=0;

//The declaration of some dynamic content resources..
struct AmmServer_Instance  * default_server=0;

struct AmmServer_RH_Context control={0};
struct AmmServer_RH_Context indexContext={0}; 
struct AmmServer_MemoryHandler * indexPage=0; 

volatile unsigned int numberOfInputFrames=0; //instead of using inputFrames.size() helps with mutex locks 
std::vector<std::vector<float> > inputFrames;
volatile unsigned int numberOfBVHFrames=0;   //instead of using bvhFrames.size() helps with mutex locks 
std::vector<std::vector<float> > bvhFrames; 


pthread_mutex_t lock; 
 

#define MAIN_THREAD_WAIT_TIME 10000
#define CLIENT_THREAD_WAIT_TIME 8000
 
void * control_callback(struct AmmServer_DynamicRequest  * rqst)
{ 
  char movementRequested[4096]={0}; 
  std::vector<float> output;
  if ( _GETcpy(rqst,"skeleton",movementRequested,4096) )
  {
     //fprintf(stderr,"Skeleton Received : %s\n",movementRequested);  
     
     char word_space[128]={0};  
     std::vector<float> input;
     struct InputParserC * ipc = InputParser_Create(4096,1); 
     InputParser_SetDelimeter(ipc,0,',');
      if (ipc!=0)
      {
        ++requestsReceived;  
          
        int numberOfArguments = InputParser_SeperateWords(ipc,movementRequested,1);
        //fprintf(stderr,"Skeleton Arguments : %u\n",numberOfArguments);  
        for (int argument=0; argument<numberOfArguments; argument++) //InputParser_GetNumberOfArguments(ipc)
          { 
            input.push_back(InputParser_GetWordFloat(ipc,argument));
          }
          
       
    pthread_mutex_lock(&lock); 
       unsigned int ourRequest = inputFrames.size();
       inputFrames.push_back(input);
       ++numberOfInputFrames;
    pthread_mutex_unlock(&lock);
    usleep(CLIENT_THREAD_WAIT_TIME);    
       
       //-----------------------------------
       fprintf(stderr,"Waiting : ");  
       int done=0;
       while (!done)
       { 
        //wait..
        usleep(CLIENT_THREAD_WAIT_TIME);
           
        pthread_mutex_lock(&lock); 
           //fprintf(stderr,".%u/%lu",ourRequest,bvhFrames.size());  
           done = (ourRequest<=numberOfBVHFrames); 
           if (done)
             { output=bvhFrames[ourRequest]; }
        pthread_mutex_unlock(&lock);    
       }
       fprintf(stderr,"\n");  
       //-----------------------------------
       
       rqst->content[0]=0;
       
       //TODO : strcat is not safe..
       if (ourRequest<=bvhFrames.size())
       { 
          for (int i=0; i<output.size(); i++)
          {
            if (output[i]==0) { snprintf(word_space,128,"0"); } else
                              { snprintf(word_space,128,"%0.4f",output[i]); }  
             
            strcat(rqst->content,word_space);  
            if (i<output.size()-1)          
                { strcat(rqst->content,","); }           
          }
       }
       
       
       InputParser_Destroy(ipc);
       ++requestsServed;
      }
  }  else
  {
     fprintf(stderr,"Incorrect Command\n");
  }

   rqst->contentSize=strlen(rqst->content);
   
   //fprintf(stderr,"Response : %s\n",rqst->content); 
   fprintf(stderr,"Responding with %lu bytes of %lu joints\n",rqst->contentSize,output.size());  
 
  return 0;
}


 
void * prepare_index_callback(struct AmmServer_DynamicRequest  * rqst)
{
  struct AmmServer_MemoryHandler * videoMH = AmmServer_CopyMemoryHandler(indexPage);


  char stringBuffer[30]={0};
  snprintf(stringBuffer,28,"%u",requestsReceived);
  AmmServer_ReplaceAllVarsInMemoryHandler(videoMH,1,"$TOTAL_REQUESTS$", stringBuffer);

  snprintf(stringBuffer,28,"%u",requestsServed);
  AmmServer_ReplaceAllVarsInMemoryHandler(videoMH,1,"$TOTAL_RESPONSES$", stringBuffer);

 
  memcpy (rqst->content , videoMH->content , videoMH->contentCurrentLength );
  rqst->contentSize=videoMH->contentCurrentLength ;

  AmmServer_FreeMemoryHandler(&videoMH);
  return 0;
}
 

//This function adds a Resource Handler for the pages stats.html and formtest.html and associates stats , form and their callback functions
void init_dynamic_content()
{
  fprintf(stderr,"Reading master index file..  ");
  indexPage=AmmServer_ReadFileToMemoryHandler(WORKING_PATH "/index.html"); 

  if (! AmmServer_AddResourceHandler(default_server,&control,"/control.html",16096,0,(void*) &control_callback,SAME_PAGE_FOR_ALL_CLIENTS) )
     { AmmServer_Warning("Failed adding index page\n"); } 

  AmmServer_AddResourceHandler(default_server,&indexContext,"/index.html",16096,0,(void*) &prepare_index_callback,SAME_PAGE_FOR_ALL_CLIENTS);
}

//This function destroys all Resource Handlers and free's all allocated memory..!
void close_dynamic_content()
{
    AmmServer_RemoveResourceHandler(default_server,&indexContext,1);
}
/*! Dynamic content code ..! END ------------------------*/




int main(int argc, char *argv[])
{
    printf("\nMocapNET Server %s starting up..\n",AmmServer_Version());
    //Check binary and header spec
    AmmServer_CheckIfHeaderBinaryAreTheSame(AMMAR_SERVER_HTTP_HEADER_SPEC);
    //Register termination signal for when we receive SIGKILL etc
    AmmServer_RegisterTerminationSignal((void*)&close_dynamic_content);

    char bindIP[MAX_IP_STRING_SIZE];
    strcpy(bindIP,"0.0.0.0");

    unsigned int port=DEFAULT_BINDING_PORT;

    float quality=1.0;
    int mocapNETMode=3;
    int useCPUOnly=0;

    if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    struct MocapNET mnet= {0};
    if ( loadMocapNET(&mnet,"test",quality,mocapNETMode,useCPUOnly) )
    {
            
    //Kick start AmmarServer , bind the ports , create the threads and get things going..!
    default_server = AmmServer_StartWithArgs(
                                             "MocapNET",
                                              argc,argv , //The internal server will use the arguments to change settings
                                              //If you don't want this look at the AmmServer_Start call
                                              bindIP,
                                              port,
                                              0, /*This means we don't want a specific configuration file*/
                                              webserver_root,
                                              templates_root
                                              );


    if (!default_server) { AmmServer_Error("Could not start server , shutting down everything.."); exit(1); }

    //Create dynamic content allocations and associate context to the correct files
    init_dynamic_content(); 
         int doAComputation=0;
         while ( (AmmServer_Running(default_server))  )
           {
             //Main thread should just sleep and let the background threads do the hard work..!
             //In other applications the programmer could use the main thread to do anything he likes..
             //The only caveat is that he would takeup more CPU time from the server and that he would have to poll
             //the AmmServer_Running() call once in a while to make sure everything is in order
             //usleep(60000);
     
             doAComputation= (numberOfInputFrames>numberOfBVHFrames);
             
             if (doAComputation) 
             {
               pthread_mutex_lock(&lock);        
                 fprintf(stderr,"executing ");    
                 std::vector<float> result = runMocapNET(&mnet,inputFrames[bvhFrames.size()],0/*gestures*/,0/*doOutputFiltering*/);
                 bvhFrames.push_back(result); 
                 ++numberOfBVHFrames;
                 usleep(1000);
               pthread_mutex_unlock(&lock); 
             }
           
             //fprintf(stderr,"!"); 
             usleep(MAIN_THREAD_WAIT_TIME);
           }

    //Delete dynamic content allocations  
    close_dynamic_content();

    //Stop the server and clean state
    AmmServer_Stop(default_server);
    AmmServer_Warning("Ammar Server stopped\n");
    
    }
    
    
    pthread_mutex_destroy(&lock); 
    
    AmmServer_Warning("MocapNETServer stopped\n");
        
    return 0;
}
