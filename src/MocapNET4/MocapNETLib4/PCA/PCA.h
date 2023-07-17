/** @file PCA.h
 *  @brief An implementation of a PCA data loader
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef PCA_H_INCLUDED
#define PCA_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include "../JSON/nxjson.h"
#include "../tools.h"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black  */
#define RED     "\033[31m"      /* Red    */
#define GREEN   "\033[32m"      /* Green  */
#define YELLOW  "\033[33m"      /* Yellow */

struct eigenVectorOpt
{
 float __attribute__((aligned(16)))  * value;
};

struct complexNumber
{
  float realPart;
  float imaginaryPart;
};

struct eigenVector
{
 struct complexNumber * value;
};

struct PCAData
{
   unsigned int numberOfSamplesUsedToCreatePCA;
   unsigned int numberOfEigenValues;
   float mean;
   float std;
   struct complexNumber * eigenValues;
   struct eigenVector   * eigenVectors;
   float * screeProportion;
   float * screeCumulative;
};

//Complex Numbers
//http://ebooks.edu.gr/ebooks/v/html/8547/2754/Mathimatika-B-Lykeiou-ThSp_html-apli/index5_2.html
static struct complexNumber addComplexNumbers(struct complexNumber a,struct complexNumber b)
{
  struct complexNumber result={0};
  result.realPart      = a.realPart + b.realPart;
  result.imaginaryPart = a.imaginaryPart + b.imaginaryPart;
  return result;
}

static struct complexNumber multiplyComplexNumbers(struct complexNumber a,struct complexNumber b)
{
  struct complexNumber result={0};
  // (A + B i)  * (C + D i) = (AC-BD) + (AD+BC) i  
  // (a.realPart + a.imaginaryPart i)  * (b.realPart + b.imaginaryPart i) = (a.realPart b.realPart - a.imaginaryPart b.imaginaryPart) + (a.realPart b.imaginaryPart + a.imaginaryPart b.realPart) i  
  result.realPart      = (a.realPart*b.realPart) - (a.imaginaryPart*b.imaginaryPart);
  result.imaginaryPart = (a.realPart*b.imaginaryPart) + (a.imaginaryPart*b.realPart);
  return result;
}

static char * recognizeComplexNumberStart(char * complexNumber)
{
  int l = strlen(complexNumber);
  char * res = complexNumber;
  //All Complex Numbers end with j
  //-------------------------------
  //We expect something like :
  // -4.1106844e-12-4.560428e-12j
  // or
  // -4.1106844e-12-4.560428e+12j
  // or
  // -4.1106844e-12-4.560428j
  // or
  // -4.1106844e-12+48j
  //-------------------------------
  if (l>2)
  {
  if (complexNumber[l-1]=='j')
      {
       l=l-2;
       while (l>0)
         {
            if ( 
                 (complexNumber[l]=='+') ||
                 (complexNumber[l]=='-')
               )
               {
                 //We have reache a +/- symbol there are three cases :
                 //Case A ) if the previous character is an e then it is a scientific notation complex number!
                 //Case B ) if not then we found the complex number start!
                 //Case C ) if l is 0 then we reached the start of the string!
                 if (l>0)
                 { 
                   if (complexNumber[l-1]!='e') 
                   {
                       //This is not a scientific notation so Case B we found our result!
                       res = complexNumber+l;
                       break;
                   } 
                 } 
               }
            l=l-1;
         }
      }
  }
  return res;
}







static struct complexNumber parseComplexNumber(const char * complexNumberString)
{
    struct complexNumber result={0};
    //---------------------------------------------------------------------------------------------
    if ( complexNumberString == 0 )   { return result; }
    if (complexNumberString[0]=='(')  { complexNumberString++; } //Point to second element to skip (
                                
    char localBuffer[128];
    snprintf(localBuffer,128,"%s",complexNumberString);
    

    float imaginaryPart = 0.0;
    char * seperator = strchr(localBuffer,')');
    if (seperator!=0) { *seperator = 0; } //Trim last )
    
    
    seperator = strstr(localBuffer,"+0j");
    if (seperator!=0)
    {
      //This is the easy case which means that the value is a clear float value and the +0j can be easily discarded!
      *seperator = 0;
    } else
    {
     unsigned int l = strlen(localBuffer); 
     if (localBuffer[l-1]=='j')
      {  
        fprintf(stderr,"Parsing complex number `%s`\n",localBuffer);
        seperator = recognizeComplexNumberStart(localBuffer);
        if (seperator!=0)
        {
            *seperator = 0; 
            fprintf(stderr,"Imaginary part `%s` / keeping real part `%s` \n",seperator+1,localBuffer);
            char * imaginaryPartString = seperator+1;
            int lastCharacter = strlen(imaginaryPartString);
            if (imaginaryPartString[lastCharacter-1]=='j')
                { imaginaryPartString[lastCharacter-1]=0; }
            imaginaryPart = strtof(imaginaryPartString,NULL);
        }
      }
    }
    
    float cleanedFloat = strtof(localBuffer,NULL);
    //fprintf(stderr,"Cleaned float is %f \n",cleanedFloat);
    if (cleanedFloat!=cleanedFloat)
    {
      //If we got a NaN value then clean it.. 
      cleanedFloat = 0.0; 
    }
    
    
    result.realPart      = cleanedFloat;
    result.imaginaryPart = imaginaryPart;
    
    return result;
}


static int unloadPCAData(struct PCAData* pca)
{
   if (pca!=0)
   {
       if (pca->screeProportion!=0) { free(pca->screeProportion); pca->screeProportion=0; }
       if (pca->screeCumulative!=0) { free(pca->screeCumulative); pca->screeCumulative=0; }
       if (pca->eigenValues!=0)     { free(pca->eigenValues);     pca->eigenValues=0; }
       if (pca->eigenVectors!=0)
       {
         for (int i=0; i<pca->numberOfEigenValues; i++)
          {
              free(pca->eigenVectors[i].value);
          }
         free(pca->eigenVectors);
       }
     return 1;
   }
 return 0;
}

static int loadPCADataFromJSON(struct PCAData* output, const char * jsonFilename)
{
  fprintf(stderr,"Loading PCA file %s ...\n",jsonFilename);
  unsigned int inputLength=0;
  char* input = readFileToMemory(jsonFilename,&inputLength);
  if (input!=0) 
  {
   //fprintf(stderr,"JSON DATA %s ...\n",input);
   fprintf(stderr,"Parsing %s ...\n",jsonFilename);
   const nx_json* json=nx_json_parse_utf8(input);
   
   //-------------------------------------------------------------------
   const nx_json* j  = nx_json_get(json,"numberOfSamplesFittedOn");
   fprintf(stderr,"key(%s)/type(%u)\n",j->key,j->type);
   output->numberOfSamplesUsedToCreatePCA = (unsigned int) atoi(j->text_value);
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"expectedInputs");
   fprintf(stderr,"key(%s)/type(%u)\n",j->key,j->type);
   output->numberOfEigenValues = (unsigned int) atoi(j->text_value);
   //-------------------------------------------------------------------
   j  = nx_json_get(json,"std");
   fprintf(stderr,"key(%s)/type(%u)\n",j->key,j->type);
   output->std = (float)  atof(j->text_value);
   j  = nx_json_get(json,"mean");
   fprintf(stderr,"key(%s)/type(%u)\n",j->key,j->type);
   output->mean = (float) atof(j->text_value);
   //-------------------------------------------------------------------

   //Our Summary..!   
   //----------------------------------------------------------------------------------------------------------------------------
   fprintf(stderr,"Number Of Samples %u\n",output->numberOfSamplesUsedToCreatePCA);
   fprintf(stderr,"Number Of Eigen Values %u\n",output->numberOfEigenValues);
   fprintf(stderr,"Mean %0.2f / Std %0.2f\n",output->mean,output->std );

   //We have now allocated enough space and are ready to parse all incoming values..
   //----------------------------------------------------------------------------------------------------------------------------
   output->eigenValues     = (struct complexNumber*) malloc(sizeof(struct complexNumber) * output->numberOfEigenValues);
   output->eigenVectors    = (struct eigenVector*) malloc(sizeof(struct eigenVector) * output->numberOfEigenValues);
   if (output->eigenVectors)
   {
      for (int i=0; i<output->numberOfEigenValues; i++)
      {
        output->eigenVectors[i].value = (struct complexNumber*) malloc(sizeof(struct complexNumber) * output->numberOfEigenValues);
      }
   }
   output->screeProportion = (float*) malloc(sizeof(float) * output->numberOfEigenValues);
   output->screeCumulative = (float*) malloc(sizeof(float) * output->numberOfEigenValues);
   //----------------------------------------------------------------------------------------------------------------------------
   

   j  = nx_json_get(json,"eigenvalues");
   fprintf(stderr,"We encountered %u eigenvalues (header says %u) \n",j->length,output->numberOfEigenValues);
   if (j->length == output->numberOfEigenValues)
   {
     //We have a correct number of eigenvalues so let's read them!
     for (int idx=0; idx<output->numberOfEigenValues; idx++)
     {
       const nx_json* item = nx_json_item(j,idx);  
       //fprintf(stderr,"key(%s)/index(%u)/type(%u)\n",j->key,idx,item->type);
      output->eigenValues[idx] = parseComplexNumber(item->text_value);
     }
   }
   
   
   const nx_json* jY  = nx_json_get(json,"eigenvectors");
   if (jY->length == output->numberOfEigenValues)
   {
    fprintf(stderr,"We encountered %u eigenvectors (header says %u) \n",jY->length,output->numberOfEigenValues);
    //We have a correct number of eigenvalues so let's read them!
    for (int idy=0; idy<output->numberOfEigenValues; idy++)
     { 
       const nx_json* itemY = nx_json_item(jY,idy);  
       if (itemY->length == output->numberOfEigenValues)
      {
       for (int idx=0; idx<output->numberOfEigenValues; idx++)
       {
        const nx_json* itemX = nx_json_item(itemY,idx);  
        //fprintf(stderr,"key(%s)/index(%u)/type(%u)\n",j->key,idx,item->type);
        output->eigenVectors[idx].value[idy]  = parseComplexNumber(itemX->text_value);
       }
      } else
      {
        fprintf(stderr,"Eigen Vector %d has an incorrect number of values (%u)\n",idy,itemY->length);
      }
     }
   }
     
     
   j  = nx_json_get(json,"scree_proportion");
   fprintf(stderr,"We encountered %u scree_proportions (header says %u) \n",j->length,output->numberOfEigenValues);
   if (j->length == output->numberOfEigenValues)
   {
     //We have a correct number of eigenvalues so let's read them!
     for (int idx=0; idx<output->numberOfEigenValues; idx++)
     {
       const nx_json* item = nx_json_item(j,idx);  
       //fprintf(stderr,"key(%s)/index(%u)/type(%u)\n",j->key,idx,item->type);
      output->screeProportion[idx] = strtof(item->text_value,NULL);
     }
   }
   
   
   j  = nx_json_get(json,"scree_cumulative");
   fprintf(stderr,"We encountered %u scree_cumulatives (header says %u) \n",j->length,output->numberOfEigenValues);
   if (j->length == output->numberOfEigenValues)
   {
     //We have a correct number of eigenvalues so let's read them!
     for (int idx=0; idx<output->numberOfEigenValues; idx++)
     {
       const nx_json* item = nx_json_item(j,idx);  
       //fprintf(stderr,"key(%s)/index(%u)/type(%u)\n",j->key,idx,item->type);
      output->screeCumulative[idx] = strtof(item->text_value,NULL);
     }
   }
   
   nx_json_free(json);
   free(input);
    
   return 1;
  } 
  return 0;
}


float dotProduct(float * vect_A, float * vect_B, int n)
{
    float product = 0.0;
 
    for (int i = 0; i < n; i++) 
        { product += vect_A[i] * vect_B[i]; }
        
    return product;
}



static int doPCATransform(float * output,int * outputSize,struct PCAData* pca,float * inputRaw,int inputSize,int selectedPCADimensions)
{
    if (pca==0) { return 0; }
    if (pca->numberOfEigenValues!=inputSize) { fprintf(stderr, RED "PCA: Shape given as input (%d,) is not aligned with PCA loaded (%u,%d) \n" NORMAL,inputSize,pca->numberOfEigenValues,*outputSize); return 0; }
    
    float mean = pca->mean;
    float std  = pca->std;
    if (std == 0.0 ) { std=1.0; } //Don't ever divide by zero
    
   fprintf(stderr,"  sample = list()\n");
   for (int i=0; i<inputSize; i++)
      {
        fprintf(stderr,"  sample.append(%f) #%u\n",inputRaw[i],i); 
      }
      
    //float __attribute__((aligned(16)))  * input = (float*) aligned_alloc(16, inputSize * sizeof(float));
    //float * input = (float*) malloc(inputSize * sizeof(float));
    struct complexNumber * inputComplex  = (struct complexNumber *) malloc(inputSize * sizeof(struct complexNumber));
    struct complexNumber * outputComplex = (struct complexNumber *) malloc(*outputSize * sizeof(struct complexNumber));
    //if (input!=0)
    if ((inputComplex!=0) && (outputComplex!=0))
    {
      for (int i=0; i<inputSize; i++)
      {
        //input[i] = (inputRaw[i]-mean)/std;
        inputComplex[i].realPart      = (inputRaw[i]-mean)/std;
        inputComplex[i].imaginaryPart = 0.0;
      }
    
     if (selectedPCADimensions>inputSize)
     {
         selectedPCADimensions = inputSize;
     }
     
      fprintf(stderr,"We want dot product of %u dimensions : \n",selectedPCADimensions);
      fprintf(stderr,"Input of size 0->%u\n",inputSize);
      fprintf(stderr,"EigenVector of size 0->%u\n",pca->numberOfEigenValues);
    
    /*
     * input is  1  x  458
       eigenvectors is  458  x  458
       result is  1  x  210
         
       data is  1  x  461
       eigenvectors is  461  x  461
       result is  1  x  210
    a = [[0, 1 , 2]]
b = [[4, 1], [3, 2], [10,10] ]
#[[23 22]]
print(np.dot(a,b))
 *   ./MocapNET4TestD 

 *   python3 DNN_Tensorflow2/principleComponentAnalysis.py
     */

      for (int i=0; i<selectedPCADimensions; i++)
      {
        output[i]                      = 0.0;
        outputComplex[i].realPart      = 0.0;
        outputComplex[i].imaginaryPart = 0.0;
      }
      
      for (int i=0; i<selectedPCADimensions; i++)
      {
        //---------------------------------
        for (int j=0; j<pca->numberOfEigenValues; j++)
         {
          //fprintf(stderr,"%0.2f * %0.2f \n",input[j],pca->eigenVectors[i].value[j]);
          //output[i] += input[j] * pca->eigenVectors[i].value[j];
          //output[i] += input[j] * pca->eigenVectors[i].value[j].realPart;
          struct complexNumber thisOutputComplex = multiplyComplexNumbers(inputComplex[j],pca->eigenVectors[i].value[j]);
          outputComplex[i].realPart      += thisOutputComplex.realPart;
          outputComplex[i].imaginaryPart += thisOutputComplex.imaginaryPart;
         } 
        //---------------------------------
       }
       
      //We have gone through all of the complex arithmetic, now we will keep only the real part
      for (int i=0; i<selectedPCADimensions; i++)
      {
        output[i] = outputComplex[i].realPart; 
      }
       
      //free(input);
      free(inputComplex);
      free(outputComplex);
    } else
    {
      fprintf(stderr,"Could not allocate aligned memory block..\n");
      return 0;
    }
   //------------------------------------------
   
    return 1;
}




#ifdef __cplusplus
}
#endif


#endif
