
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string.h>


#include "csvRead.hpp"
#include "../../MocapNETLib2/mocapnet2.hpp"
#include "../../MocapNETLib2/IO/jsonMocapNETHelpers.hpp"

#include "../../../../dependencies/InputParser/InputParser_C.h"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */



unsigned int getBodyLinesOfCSVFIle(struct CSVFileContext * csv,const char * filename)
{
    FILE * fp = fopen(filename,"r");
    {
        // Extract characters from file and store in character c
        unsigned int count=0;
        char c;
        for (c = getc(fp); c != EOF; c = getc(fp))
            if (c == '\n') // Increment count if this character is newline
                {
                    count = count + 1;
                }

        fclose(fp);

        //The first line of a CSV is its header so we don't count this ..
        if (count>0)
            {
                --count;
            }
        return count;
    }
    return 0;
}


int openCSVFile(struct CSVFileContext * csv,const char * filename)
{
    csv->fp = fopen(filename,"r");
    if (csv->fp!=0)
        {
            csv->lineNumber=0;
            csv->numberOfHeaderFields=0;
            fprintf(stderr,GREEN "Successfully opened CSV file %s\n" NORMAL,filename);
            return 1;
        }
    fprintf(stderr,RED "Failed to open CSV file %s\n" NORMAL,filename);
    return 0;
}

int closeCSVFile(struct CSVFileContext * csv)
{
    if (csv==0)
        {
            fprintf(stderr,RED "Dont need to close empty CSV struct..\n" NORMAL);
            return 1;
        }
    if (csv->fp==0)
        {
            fprintf(stderr,RED "Dont need to close CSV header that has already been closed..\n" NORMAL);
            return 1;
        }

    if (csv->fp!=0)
        {
            //TODO FREE STUFF HERE..
            int i=0;
            for (i=0; i<csv->numberOfHeaderFields; i++)
                {
                    if (csv->field[i].str!=0)
                        {
                            free(csv->field[i].str);
                            csv->field[i].str=0;
                        }
                }

            fclose(csv->fp);
        }
    return 1;
}


int parseCSVHeader(struct CSVFileContext * csv)
{
    if (csv==0)
        {
            fprintf(stderr,RED "Cannot parse empty CSV header..\n" NORMAL);
            return 0;
        }
    if (csv->fp==0)
        {
            fprintf(stderr,RED "Cannot parse CSV header that hasn't been opened using openCSVFile..\n" NORMAL);
            return 0;
        }

    fprintf(stderr,"Parsing CSV header..\n");
    char whereToStoreItems[513]= {0};
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    if (csv->lineNumber==0)
        {
            read = getline(&line, &len, csv->fp);
            if (line!=0)
                {
                    struct InputParserC * ipc = InputParser_Create(8096,4);
                    InputParser_SetDelimeter(ipc,0,',');
                    InputParser_SetDelimeter(ipc,1,0);
                    InputParser_SetDelimeter(ipc,2,10);
                    InputParser_SetDelimeter(ipc,3,13);

                    int numberOfArguments = InputParser_SeperateWords(ipc,line,1);

                    if (numberOfArguments<MAX_CSV_HEADER_FIELDS)
                        {
                            int i=0;
                            for (i=0; i<numberOfArguments; i++)
                                {
                                    //InputParser_GetWord InputParser_GetLowercaseWord
                                    InputParser_GetLowercaseWord(ipc,i,whereToStoreItems,512);
                                    //fprintf(stderr,"CSV Header element %u => %s\n",i,whereToStoreItems);
                                    csv->field[i].strLength = strlen(whereToStoreItems);
                                    csv->field[i].str = (char*) malloc(sizeof(char) * (csv->field[i].strLength+2));
                                    memcpy(csv->field[i].str,whereToStoreItems,csv->field[i].strLength);
                                    csv->field[i].str[csv->field[i].strLength]=0;
                                    ++csv->numberOfHeaderFields;
                                }
                        }
                    else
                        {
                            fprintf(stderr,"Too many CSV header arguments (encountered %u, max %u)..\n",numberOfArguments,MAX_CSV_HEADER_FIELDS);
                            InputParser_Destroy(ipc);
                            if (line!=0)
                                {
                                    free(line);
                                    line=0;
                                }
                            return 0;
                        }




                    InputParser_Destroy(ipc);
                    csv->lineNumber = csv->lineNumber +1;
                    if (line!=0)
                        {
                            free(line);
                            line=0;
                        }

                    return 1;
                }
        }

    if (line!=0)
        {
            free(line);
            line=0;
        }
    return 0;
}





int parseNextCSVFloatLine(struct CSVFileContext * csv,struct CSVFloatFileLine * csvLine)
{
    if (csv==0)
        {
            return 0;
        }
    if (csvLine==0)
        {
            return 0;
        }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    if (csv->lineNumber==0)
        {
            if (!parseCSVHeader(csv))
                {
                    fprintf(stderr,RED "Could not read CSV header on first loop\n" NORMAL);
                    return 0;
                }
        }

    if ((read = getline(&line, &len, csv->fp)) != -1)
        {
            struct InputParserC * ipc = InputParser_Create(8096,4);
            InputParser_SetDelimeter(ipc,0,',');
            InputParser_SetDelimeter(ipc,1,0);
            InputParser_SetDelimeter(ipc,2,10);
            InputParser_SetDelimeter(ipc,3,13);

            csvLine->lineNumber=csv->lineNumber;
            csvLine->numberOfFields = InputParser_SeperateWords(ipc,line,1);

            //fprintf(stderr,"Line %u (%s) has %u arguments \n",csvLine->lineNumber,line,csvLine->numberOfFields );

            if ( csvLine->numberOfFields != csv->numberOfHeaderFields )
                {
                    fprintf(stderr,RED "Number of header fields %u does not correspond with number of body elements %u \n" NORMAL,csv->numberOfHeaderFields,csvLine->numberOfFields);
                }

            for (int i=0; i<csvLine->numberOfFields; i++)
                {
                    csvLine->field[i] = InputParser_GetWordFloat(ipc,i);
                    //fprintf(stderr,"%s(%u)=%0.2f ",csv->field[i].str,i,csvLine->field[i]);
                }


            InputParser_Destroy(ipc);

            csv->lineNumber = csv->lineNumber+1;
            if (line!=0)
                {
                    free(line);
                    line=0;
                }
            return 1;
        }
    if (line!=0)
        {
            free(line);
            line=0;
        }
    return 0;
}




int parseNextCSVCOCOSkeleton(struct CSVFileContext * csv, struct skeletonSerialized * skel)
{
    if (csv==0)
        {
            return 0;
        }
    if (skel==0)
        {
            return 0;
        }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    if (csv->lineNumber==0)
        {
            if (!parseCSVHeader(csv))
                {
                    fprintf(stderr,RED "Could not read CSV header on first loop\n" NORMAL);
                    return 0;
                }
            /*
             else
            {
               fprintf(stderr,"Header of skeleton parsed successfully..\n");
               for (unsigned int i=0; i<csv->numberOfHeaderFields; i++)
               {
                  fprintf(stderr,"CSV file header element %u => %s \n",i,csv->field[i].str);
               }
            }*/
        }

    if ((read = getline(&line, &len, csv->fp)) != -1)
        {
            //CSV files are always stored normalized..
            skel->width = 1.0;
            skel->height = 1.0;


            struct InputParserC * ipc = InputParser_Create(8096,4);
            InputParser_SetDelimeter(ipc,0,',');
            InputParser_SetDelimeter(ipc,1,0);
            InputParser_SetDelimeter(ipc,2,10);
            InputParser_SetDelimeter(ipc,3,13);

            skel->skeletonBodyElements = InputParser_SeperateWords(ipc,line,1);

            //fprintf(stderr,"Line %u (%s) has %u arguments \n",csv->lineNumber,line,skel->skeletonBodyElements);

            int typeOfData=0,jointID=0;
            float value=0.0;

            if ( skel->skeletonBodyElements != csv->numberOfHeaderFields )
                {
                    fprintf(stderr,"Number of header fields %u does not correspond with number of body elements %u \n",csv->numberOfHeaderFields,skel->skeletonBodyElements);
                }

            //Copy references to header ..
            skel->skeletonHeaderElements=csv->numberOfHeaderFields;
            for (int i=0; i<csv->numberOfHeaderFields; i++)
                {
                    skel->skeletonHeader[i].strLength = csv->field[i].strLength;
                    skel->skeletonHeader[i].str = csv->field[i].str;
                }

            for (int i=0; i<skel->skeletonBodyElements; i++)
                {
                    //fprintf(stderr,"CSV Header element %u => %s\n",i,whereToStoreItems);
                    skel->skeletonBody[i].value = InputParser_GetWordFloat(ipc,i);
                    //fprintf(stderr,"%s(%u)=%0.2f ",csv->field[i].str,i,skel->skeletonBody[i].value);
                }


            InputParser_Destroy(ipc);
            csv->lineNumber = csv->lineNumber+1;
            if (line!=0)
                {
                    free(line);
                    line=0;
                }
            return 1;
        }
    if (line!=0)
        {
            free(line);
            line=0;
        }
    return 0;
}



int uniformlyScaleSerializedSkeleton(struct skeletonSerialized * skel,float factor)
{
    if (skel==0)
        {
            return 0;
        }

    for (int i=0; i<skel->skeletonBodyElements; i++)
        {
            skel->skeletonBody[i].value *= factor;
        }
    return 1;
}



int scaleSerializedSkeletonX(struct skeletonSerialized * skel,float factorX)
{
    if (skel==0)
        {
            return 0;
        }
    if (skel->skeletonHeaderElements!=skel->skeletonBodyElements)
        {
            fprintf(stderr,"scaleSerializedSkeletonX: Inconsistent headers length\n");
            return 0;
        }

    int scaledJoints=0;
    for (int i=0; i<skel->skeletonHeaderElements; i++)
        {
            if
            (
                ( skel->skeletonHeader[i].strLength >4  ) &&
                ( skel->skeletonHeader[i].str!=0 )
            )
                {
                    if (
                        (skel->skeletonHeader[i].str[0]=='2') &&
                        ( (skel->skeletonHeader[i].str[1]=='D') || (skel->skeletonHeader[i].str[1]=='d') ) &&
                        ( (skel->skeletonHeader[i].str[2]=='X') || (skel->skeletonHeader[i].str[2]=='x') ) &&
                        (skel->skeletonHeader[i].str[3]=='_')
                    )
                        {
                            skel->skeletonBody[i].value *= factorX;
                            ++scaledJoints;
                        }
                }
        }

    return scaledJoints;
}


int scaleSerializedSkeletonY(struct skeletonSerialized * skel,float factorY)
{
    if (skel==0)
        {
            return 0;
        }
    if (skel->skeletonHeaderElements!=skel->skeletonBodyElements)
        {
            fprintf(stderr,"scaleSerializedSkeletonY: Inconsistent headers length\n");
            return 0;
        }

    int scaledJoints=0;
    for (int i=0; i<skel->skeletonHeaderElements; i++)
        {
            if
            (
                ( skel->skeletonHeader[i].strLength >4  ) &&
                ( skel->skeletonHeader[i].str!=0 )
            )
                {
                    if (
                        (skel->skeletonHeader[i].str[0]=='2') &&
                        ( (skel->skeletonHeader[i].str[1]=='D') || (skel->skeletonHeader[i].str[1]=='d') ) &&
                        ( (skel->skeletonHeader[i].str[2]=='Y') || (skel->skeletonHeader[i].str[2]=='y') ) &&
                        (skel->skeletonHeader[i].str[3]=='_')
                    )
                        {
                            skel->skeletonBody[i].value *= factorY;
                            ++scaledJoints;
                        }
                    ///else { fprintf(stderr,"`%s` does not match\n",skel->skeletonHeader[i].str); }
                }
        }

    return scaledJoints;
}



int scaleSerializedSkeletonFromCenter(struct skeletonSerialized * skel,float factorX,float factorY)
{
    if (skel==0)
        {
            return 0;
        }

//       Assuming we have a 2D point cloud with points 0..1,0..1
//       This is how we model the framebuffer
//        0,0 __________________________________________________________________________________ 1,0
//         |                                         /\                                            |
//         |                                          |  0.5                                       |
//         |                                          |                                            |
//         |                 0.5                      |                   0.5                      |
//         |           <----------------------------- * ----------------------------->             |
//         |                                          |                                            |
//         |                                          |                                            |
//         |                                          |  0.5                                       |
//         |                                         \/                                            |
//        0,1------------------------------------------------------------------------------------ 1,1
//  The * is the center of the image..
// We want to scale X,Y points based on factorX and factorY
//However the magnitued of the scaling will be linear to the distance from the
//Center of the image , Points 0.5 units away from the center will be scaled fully with the factor
//Points close to the center will be not scaled at all..
    if (skel->skeletonHeaderElements!=skel->skeletonBodyElements)
        {
            fprintf(stderr,"scaleSerializedSkeletonY: Inconsistent headers length\n");
            return 0;
        }

    float value,magnitude,maximumFactor,thisFactorY,relativeY,thisFactorX,relativeX;

    int scaledJoints=0;
    for (int i=0; i<skel->skeletonHeaderElements; i++)
        {
            if
            (
                ( skel->skeletonHeader[i].strLength >4  ) &&
                ( skel->skeletonHeader[i].str!=0 )
            )
                {
                    if (
                        (skel->skeletonHeader[i].str[0]=='2') &&
                        ( (skel->skeletonHeader[i].str[1]=='D') || (skel->skeletonHeader[i].str[1]=='d') ) &&
                        ( (skel->skeletonHeader[i].str[2]=='X') || (skel->skeletonHeader[i].str[2]=='x') ) &&
                        (skel->skeletonHeader[i].str[3]=='_')
                    )
                        {
                            if (factorX!=1.0)
                                {
                                    value = skel->skeletonBody[i].value;
                                    maximumFactor = factorX-1.0;
                                    magnitude = (float) fabs(value-0.5)/0.5;
                                    thisFactorX = (magnitude * maximumFactor) + 1.0;
                                    relativeX = value - 0.5;
                                    relativeX *= thisFactorX;
                                    skel->skeletonBody[i].value = relativeX + 0.5 ;
                                    ++scaledJoints;
                                }
                        }
                    else if (
                        (skel->skeletonHeader[i].str[0]=='2') &&
                        ( (skel->skeletonHeader[i].str[1]=='D') || (skel->skeletonHeader[i].str[1]=='d') ) &&
                        ( (skel->skeletonHeader[i].str[2]=='Y') || (skel->skeletonHeader[i].str[2]=='y') ) &&
                        (skel->skeletonHeader[i].str[3]=='_')
                    )
                        {
                            if (factorY!=1.0)
                                {
                                    value = skel->skeletonBody[i].value;
                                    maximumFactor = factorY-1.0;
                                    magnitude = (float) fabs(value-0.5)/0.5;
                                    thisFactorY = ( magnitude * maximumFactor ) + 1.0;
                                    relativeY = value - 0.5;
                                    relativeY *= thisFactorY;
                                    skel->skeletonBody[i].value = relativeY + 0.5 ;
                                    ++scaledJoints;
                                }
                        }
                } //Valid header
        } // For each joint

    return scaledJoints;
}



#ifdef __GNUC__ 
#  if __GNUC_PREREQ(5,5)
//      If  gcc_version >= RANDOM support
#include <random> //For generating random samples based on a normal distribution
//This function makes the life of the 3D pose estimator more difficult by adding a configurable level of gaussian noise on the 2D pixels estimated
int perturbSerializedSkeletonUsingGaussianNoise(struct skeletonSerialized * skel,float gaussianNoiseInNormalizedPixelsX,float gaussianNoiseInNormalizedPixelsY)
{
    if (skel->skeletonHeaderElements!=skel->skeletonBodyElements)
        {
            fprintf(stderr,"perturbSerializedSkeletonUsingGaussianNoise: Inconsistent headers length\n");
            return 0;
        }


    std::default_random_engine generator;
    std::normal_distribution<float> distributionX(0.0,gaussianNoiseInNormalizedPixelsX);
    std::normal_distribution<float> distributionY(0.0,gaussianNoiseInNormalizedPixelsY);


    float value,magnitude,maximumFactor,thisFactorY,relativeY,thisFactorX,relativeX;

    int perturbedCoordinates=0;
    for (int i=0; i<skel->skeletonHeaderElements; i++)
        {
            if
            (
                ( skel->skeletonHeader[i].strLength >4  ) &&
                ( skel->skeletonHeader[i].str!=0 )
            )
                {
                    if (
                        (skel->skeletonHeader[i].str[0]=='2') &&
                        ( (skel->skeletonHeader[i].str[1]=='D') || (skel->skeletonHeader[i].str[1]=='d') ) &&
                        ( (skel->skeletonHeader[i].str[2]=='X') || (skel->skeletonHeader[i].str[2]=='x') ) &&
                        (skel->skeletonHeader[i].str[3]=='_')
                    )
                        {
                            if (gaussianNoiseInNormalizedPixelsX>0.0)
                                {
                                    float perturbation = distributionX(generator);
                                    skel->skeletonBody[i].value += perturbation;

                                    ++perturbedCoordinates;
                                }
                        }
                    else if (
                        (skel->skeletonHeader[i].str[0]=='2') &&
                        ( (skel->skeletonHeader[i].str[1]=='D') || (skel->skeletonHeader[i].str[1]=='d') ) &&
                        ( (skel->skeletonHeader[i].str[2]=='Y') || (skel->skeletonHeader[i].str[2]=='y') ) &&
                        (skel->skeletonHeader[i].str[3]=='_')
                    )
                        {
                            if (gaussianNoiseInNormalizedPixelsY>0.0)
                                {
                                    float perturbation = distributionY(generator);
                                    skel->skeletonBody[i].value += perturbation;

                                    ++perturbedCoordinates;
                                }
                        }
                } //Valid header
        } // For each joint

    return perturbedCoordinates;
} 
#  else
#warning "Your compiler is too old and is missing normal random distributions"
int perturbSerializedSkeletonUsingGaussianNoise(struct skeletonSerialized * skel,float gaussianNoiseInNormalizedPixelsX,float gaussianNoiseInNormalizedPixelsY)
{
    fprintf(stderr,RED "\n\n\n\nYour compiler is too old and is missing normal random distributions\n\n\n\n" NORMAL );
    return 0;
}
//       Else
#  endif
#else
int perturbSerializedSkeletonUsingGaussianNoise(struct skeletonSerialized * skel,float gaussianNoiseInNormalizedPixelsX,float gaussianNoiseInNormalizedPixelsY)
{
    fprintf(stderr,RED "\n\n\n\nYour compiler is not GCC old and is missing(?) normal random distributions\n\n\n\n" NORMAL );
    return 0;
}
//    If not gcc
#endif


/*
float rand_FloatRange(float a, float b)
{
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}*/



std::vector<std::vector<float> > get2DPointsFromSkeleton(struct skeletonSerialized * skel)
{
    fprintf(stderr,"get2DPointsFromSkeleton:  ");
    std::vector<std::vector<float> > finalVector;
    if (skel==0)
        {
            fprintf(stderr,"get2DPointsFromSkeleton: No input\n");
            return finalVector;
        }
    if (skel->skeletonBodyElements!=skel->skeletonHeaderElements)
        {
            fprintf(stderr,"get2DPointsFromSkeleton: Mismatch of elements of body/header\n");
            return finalVector;
        }

    std::vector<float> points2D;
    for (int i=0; i<skel->skeletonBodyElements; i++)
        {
            if ( strstr(skel->skeletonHeader[i].str,"2DX_")!=0)
                {
                    points2D.push_back(skel->skeletonBody[i].value);
                }
            else if ( strstr(skel->skeletonHeader[i].str,"2DY_")!=0)
                {
                    points2D.push_back(skel->skeletonBody[i].value);
                }
            else if ( strstr(skel->skeletonHeader[i].str,"visible_")!=0)
                {
                    finalVector.push_back(points2D);
                    points2D.clear();
                }
        }

    fprintf(stderr,"survived..\n");
    return finalVector;
}
