/** @file readListFile.h
 *  @brief An implementation of reading a list from a text file
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef READ_LIST_FILE_H_INCLUDED
#define READ_LIST_FILE_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>

struct listFileEntry
{
  int strLength;
  char * str;
};

struct listFileData
{
  unsigned int numberOfEntries;
  struct listFileEntry * entry;
};


static int slowLineCounter(const char * filename)
{
   char ch;
   int linesCount=0;
   //open file in read more
   FILE * fp=fopen(filename,"r");
   if(fp==NULL) {
      printf("File \"%s\" does not exist!!!\n",filename);
      return -1;
   }
   //read character by character and check for new line
   while((ch=fgetc(fp))!=EOF) 
   {
      if(ch=='\n')
         linesCount++;
   }
   //close the file
   fclose(fp);
   
   return linesCount;
}

static int destroyListFile(struct listFileData * listOutput)
{
    fprintf(stderr,"destroying List File..! \n");
    if (listOutput!=0)
    {
     if (listOutput->entry!=0)
      {
        for (int i=0; i<listOutput->numberOfEntries; i++)
        {
            if (listOutput->entry[i].str!=0)
            {
               free(listOutput->entry[i].str);
               listOutput->entry[i].strLength = 0;
            }
        }
        //------------------------------------------------
        free(listOutput->entry);
        listOutput->entry=0;
      }
    }
    fprintf(stderr,"destroyed List File..! \n");
    return 1;
}



static int printListFile(struct listFileData * listOutput,const char * label)
{
    if (listOutput==0) { return 0; }
    if (listOutput->entry!=0)
    {
     printf("Listing %s\n",label); 
     printf("_______________________\n"); 
     for (int i=0; i<listOutput->numberOfEntries; i++)
     {
          printf("Line %u === `%s`\n",i,listOutput->entry[i].str); 
     }
     printf("\n\n\n"); 
    return 1;
    }
    
    
  return 0;
}

static int readListFile(struct listFileData * listOutput,const char * filename)
{
    if (listOutput==0) { return 0; }
    
    //We now know the number of entries
    listOutput->numberOfEntries = slowLineCounter(filename);
    
    if (listOutput!=0)
    {
        //Clean up everything!
        destroyListFile(listOutput);
    }
    
    
    listOutput->entry = (struct listFileEntry *) malloc(sizeof(struct listFileEntry) * listOutput->numberOfEntries );
    if (listOutput->entry==0) { return 0; }
    
    
    char *  line = NULL;
    size_t  len  = 0;
    ssize_t read = 0;

    FILE * fp = fopen(filename, "r");
    if (fp == NULL)
        { return 0; }
    
    unsigned int entryNumber = 0;
    int i=0;
    while ((read=getline(&line, &len, fp)) != -1) 
        {
          int stringLength = strlen(line);
          
          int stringLengthWithoutNull = stringLength-1;
          while ( (stringLengthWithoutNull>0) && (  (line[stringLengthWithoutNull]==10) || (line[stringLengthWithoutNull]==13) ) )
          { 
              line[stringLengthWithoutNull]=0;
              stringLengthWithoutNull-=1;
              stringLength-=1;
          }

          
          //fprintf(stderr,"reading line %s (%u)..! \n",line,stringLength);
          if (entryNumber<listOutput->numberOfEntries)
          {
             listOutput->entry[i].str = (char *) malloc(sizeof(char) * (stringLength+2));
             if (listOutput->entry[i].str!=0)
             {
              listOutput->entry[i].strLength = stringLength;
              strncpy(listOutput->entry[i].str,line,stringLength);
              listOutput->entry[i].str[stringLength]=0; //Null termination
              //snprintf(listOutput->entry[i].str,stringLength+1,"%s",line);
             }
             
             ++entryNumber;
          }
          
          
          i+=1;
        }

    fclose(fp);
    if (line)
        { free(line); }
   return 1;
}


#ifdef __cplusplus
}
#endif




#endif  

