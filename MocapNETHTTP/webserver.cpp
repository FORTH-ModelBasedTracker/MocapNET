#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../AmmarServer/src/AmmServerlib/AmmServerlib.h"

#define DEFAULT_BINDING_PORT 8080  // <--- Change this to 80 if you want to bind to the default http port..!


#define WORKING_PATH "MocapNETHTTP/res/"
char webserver_root[MAX_FILE_PATH]=WORKING_PATH; // <- change this to the directory that contains your content if you dont want to use the default public_html dir..
char templates_root[MAX_FILE_PATH]="public_html/templates/";


#include "opencv2/opencv.hpp"
using namespace cv;

#include <cstdlib>

#include "../MocapNETLib/jsonCocoSkeleton.h"
#include "../MocapNETLib/jsonMocapNETHelpers.cpp"

#include "../Tensorflow/tensorflow.hpp"
#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/bvh.hpp"
#include "../MocapNETLib/visualization.hpp"


const unsigned long maximumDiskUsageAllowed=3.5 /*GB*/ *1024 *1024 *1024;
const unsigned int bufferPageSize=32 /*KB*/ *1024;
unsigned int maxUploadFileSizeAllowedMB=4; /*MB*/

unsigned int uploadsFilesSize=0;
unsigned int uploadsDataSize=0;

//The decleration of some dynamic content resources..
struct AmmServer_Instance  * default_server=0;
struct AmmServer_RH_Context stats={0};


struct AmmServer_RH_Context uploadProcessor={0};
struct AmmServer_RH_Context indexProcessor={0};


struct AmmServer_MemoryHandler * indexPage=0;
struct AmmServer_MemoryHandler * errorPage=0;

//This function prepares the content of  stats context , ( stats.content )
void * prepare_stats_content_callback(struct AmmServer_DynamicRequest  * rqst)
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  fprintf(stderr,"Trying to write dynamic request to %p , max size %lu \n",rqst->content , rqst->MAXcontentSize);

  //No range check but since everything here is static max_stats_size should be big enough not to segfault with the strcat calls!
  snprintf(rqst->content,rqst->MAXcontentSize,
           "<html><head><title>Dynamic Content Enabled</title><meta http-equiv=\"refresh\" content=\"1\"></head>\
            <body>The date and time in AmmarServer is<br><h2>%02d-%02d-%02d %02d:%02d:%02d\n</h2>\
            The string you see is updated dynamically every time you get a fresh copy of this file!<br><br>\n\
            To include your own content see the <a href=\"https://github.com/AmmarkoV/AmmarServer/blob/master/src/main.c#L46\">\
            Dynamic content code label in ammarserver main.c</a><br>\n\
            If you dont need dynamic content at all consider disabling it from ammServ.conf or by setting DYNAMIC_CONTENT_RESOURCE_MAPPING_ENABLED=0; in \
            <a href=\"https://github.com/AmmarkoV/AmmarServer/blob/master/src/AmmServerlib/file_caching.c\">file_caching.c</a> and recompiling.!</body></html>",
           tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,   tm.tm_hour, tm.tm_min, tm.tm_sec);

  rqst->contentSize=strlen(rqst->content);
  return 0;
}




//This function prepares the content of  stats context , ( stats.content )
void * prepare_index_callback_message(struct AmmServer_DynamicRequest  * rqst , const char * errorMessage)
{
  struct AmmServer_MemoryHandler * videoMH = AmmServer_CopyMemoryHandler(indexPage);

  unsigned int linkID=1+rand()%6;

  char stringBuffer[29]={0};
  snprintf(stringBuffer,28,"banner_%u",linkID);

  AmmServer_ReplaceAllVarsInMemoryHandler(videoMH,2,"$NAME_OF_THIS_MYLOADER_SERVER$","AmmarServer");
  AmmServer_ReplaceAllVarsInMemoryHandler(videoMH,1,"$BANNERLINK$",stringBuffer);


  if (errorMessage==0)
  {
  AmmServer_ReplaceAllVarsInMemoryHandler(videoMH,1,"$UPLOAD_HTML_CODE$",
     "<form enctype=\"multipart/form-data\" action=\"upload.html\" method=\"POST\">\
       <input type=\"hidden\" name=\"rawresponse\" value=\"NO\" />\
       File to upload: <input name=\"uploadedfile\" type=\"file\" />\
       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\
       <input type=\"submit\" value=\"Upload File\" name=\"submit\" />&nbsp;&nbsp;\
       <input type=\"button\" value=\"R\" name=\"submit\" onclick=\"window.location.href='random.html';return false\" />\
   </form>");
  } else
  {
    AmmServer_ReplaceAllVarsInMemoryHandler(videoMH,1,"$UPLOAD_HTML_CODE$",errorMessage);
  }


  snprintf(stringBuffer,28,"%u",maxUploadFileSizeAllowedMB);
  AmmServer_ReplaceAllVarsInMemoryHandler(videoMH,1,"$MAXIMUM_UPLOAD_SIZE$", stringBuffer);

  snprintf(stringBuffer,28,"%0.2f MB",(float) 0.0);
  AmmServer_ReplaceAllVarsInMemoryHandler(videoMH,1,"$TOTAL_SHARED_DATA$", stringBuffer);


  snprintf(stringBuffer,28,"%0.2f MB",(float) 0.0);
  AmmServer_ReplaceAllVarsInMemoryHandler(videoMH,1,"$TOTAL_UPLOAD_SIZE$", stringBuffer);

  memcpy (rqst->content , videoMH->content , videoMH->contentCurrentLength );
  rqst->contentSize=videoMH->contentCurrentLength ;

  AmmServer_FreeMemoryHandler(&videoMH);
  return 0;
}

//This function prepares the content of  stats context , ( stats.content )
void * prepare_index_callback(struct AmmServer_DynamicRequest  * rqst)
{
  return prepare_index_callback_message(rqst,0);
}

//This function adds a Resource Handler for the pages stats.html and formtest.html and associates stats , form and their callback functions
void init_dynamic_content()
{


  fprintf(stderr,"Reading master index file..  ");
  indexPage=AmmServer_ReadFileToMemoryHandler(WORKING_PATH "/index.html");
  errorPage=AmmServer_ReadFileToMemoryHandler(WORKING_PATH "/error.html");

  AmmServer_AddResourceHandler(default_server,&stats,"/index.html",16096,0,(void*) &prepare_index_callback,SAME_PAGE_FOR_ALL_CLIENTS);
}

//This function destroys all Resource Handlers and free's all allocated memory..!
void close_dynamic_content()
{
    AmmServer_RemoveResourceHandler(default_server,&stats,1);
}
/*! Dynamic content code ..! END ------------------------*/




int main(int argc, char *argv[])
{
    printf("\nAmmar Server %s starting up..\n",AmmServer_Version());
    //Check binary and header spec
    AmmServer_CheckIfHeaderBinaryAreTheSame(AMMAR_SERVER_HTTP_HEADER_SPEC);
    //Register termination signal for when we receive SIGKILL etc
    AmmServer_RegisterTerminationSignal((void*)&close_dynamic_content);

    char bindIP[MAX_IP_STRING_SIZE];
    strcpy(bindIP,"0.0.0.0");

    unsigned int port=DEFAULT_BINDING_PORT;

    //Kick start AmmarServer , bind the ports , create the threads and get things going..!
    default_server = AmmServer_StartWithArgs(
                                             "SimpleTemplate",
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
    //stats.html and formtest.html should be availiable from now on..!

         while ( (AmmServer_Running(default_server))  )
           {
             //Main thread should just sleep and let the background threads do the hard work..!
             //In other applications the programmer could use the main thread to do anything he likes..
             //The only caveat is that he would takeup more CPU time from the server and that he would have to poll
             //the AmmServer_Running() call once in a while to make sure everything is in order
             //usleep(60000);
             sleep(1);
           }

    //Delete dynamic content allocations and remove stats.html and formtest.html from the server
    close_dynamic_content();

    //Stop the server and clean state
    AmmServer_Stop(default_server);
    AmmServer_Warning("Ammar Server stopped\n");
    return 0;
}
