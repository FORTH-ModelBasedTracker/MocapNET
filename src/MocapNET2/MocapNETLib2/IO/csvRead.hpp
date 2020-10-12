#pragma once
/** @file csvRead.hpp
 *  @brief To simplify dataset parsing the very simple CSV ( comma seperated value ) format is used by MocapNET2 
 *  This module facilitates opening,parsing and performing some trivial processing of CSV input files..
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <vector>


#include "commonSkeleton.hpp"
#define MAX_CSV_HEADER_FIELDS 1024


/**
 * @brief Each CSV file needs a context to be parsed, this contains the file discriptor the current line number, the number of fields in the header as well as header fields 
 */
struct CSVFileContext
{
    FILE * fp;
    unsigned int lineNumber;
    unsigned int numberOfHeaderFields;
    struct headerField field[MAX_CSV_HEADER_FIELDS];
};

/**
 * @brief We may choose to parse a CSV file that only consists of floats, for this specific type there is an optimized CSV context
 */
 struct CSVFloatFileLine
{ 
    unsigned int lineNumber;
    unsigned int numberOfFields;
    float field[MAX_CSV_HEADER_FIELDS];
};


/**
 * @brief Write a skeleton to a CSV. This is the first call that prepares the header 
 * @param CSV context
 * @param filename of CSV file 
 * @retval 0=Failure/No lines, Otherwise the number of body lines is returned
 */
unsigned int getBodyLinesOfCSVFIle(struct CSVFileContext * csv,const char * filename);
 
 
 
/**
 * @brief This is the initial call to open a CSV file, you need to have a preallocated CSVFileContext strcture as well as a filename. Also don't forget to call closeCSVFile after you are done parsing it 
 * @param CSV context
 * @param filename of CSV file 
 * @retval 0=Failure/1=Success
 */
int openCSVFile(struct CSVFileContext * csv,const char * filename);

/**
 * @brief This call should be executed once after opening a CSV file to parse it's header
 * @param CSV context 
 * @retval 0=Failure/1=Success
 */
int parseCSVHeader(struct CSVFileContext * csv);


/**
 * @brief This is the final call to close a CSV file and release its file descriptor after you are done parsing it 
 * @param CSV context 
 * @retval 0=Failure/1=Success
 */
int closeCSVFile(struct CSVFileContext * csv);


int parseNextCSVFloatLine(struct CSVFileContext * csv,struct CSVFloatFileLine * csvLine);


int parseNextCSVCOCOSkeleton(struct CSVFileContext * csv, struct skeletonSerialized * skel);



int uniformlyScaleSerializedSkeleton(struct skeletonSerialized * skel,float factor);


int scaleSerializedSkeletonX(struct skeletonSerialized * skel,float factorX);
int scaleSerializedSkeletonY(struct skeletonSerialized * skel,float factorY);

int scaleSerializedSkeletonFromCenter(struct skeletonSerialized * skel,float factorX,float factorY);


int perturbSerializedSkeletonUsingGaussianNoise(struct skeletonSerialized * skel,float gaussianNoiseInNormalizedPixelsX,float gaussianNoiseInNormalizedPixelsY);



std::vector<std::vector<float> > get2DPointsFromSkeleton(struct skeletonSerialized * skel);
