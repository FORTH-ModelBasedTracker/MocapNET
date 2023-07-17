/** @file tools.h
 *  @brief Various Tools!
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef MNET4_TOOLS_H_INCLUDED
#define MNET4_TOOLS_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


#include <stdio.h>
#include <string.h>


static char * readFileToMemory(const char * filename,unsigned int *length)
{
    if (filename==0)
    { 
        fprintf(stderr,RED "No Path Given to readFileToMemory\n" NORMAL);
        return 0; 
    }
    
    *length = 0;
    FILE * pFile = fopen ( filename , "rb" );

    if (pFile==0)
    {
        fprintf(stderr,RED "readFileToMemory failed\n" NORMAL);
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
        fprintf(stderr,RED "Could not allocate enough memory for file %s \n" NORMAL,filename);
        fclose(pFile);
        return 0;
    }

    // copy the file into the buffer:
    size_t result = fread (buffer,1,lSize,pFile);
    if (result != lSize)
    {
        free(buffer);
        fprintf(stderr,RED "Could not read the whole file onto memory %s \n" NORMAL,filename);
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


#ifdef __cplusplus
}
#endif



#endif  