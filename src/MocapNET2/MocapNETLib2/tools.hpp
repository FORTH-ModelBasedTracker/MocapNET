#pragma once
/** @file tools.h
 *  @brief System Tools that don't fit somewhere else
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <vector>
#include <math.h>

const float goFromRadToDegrees=(float) 180.0 / M_PI; 
const float goFromDegreesToRad=(float) M_PI / 180.0; 


int vectorcmp(std::vector<float> vA,std::vector<float> vB, float maximumDistance);

int nsleep(long nanoseconds);

/**
 * @brief Get euclidean 2D distance between a pair of points
 * @ingroup tools
 * @param Point A, X coordinate
 * @param Point A, Y coordinate
 * @param Point B, X coordinate
 * @param Point B, Y coordinate
 * @retval Output rotation value
 */
 float getJoint2DDistance_tools(float aX,float aY,float bX,float bY);
 
/**
 * @brief This is a very crucial function that converts a pair of 2D points to an NSRM rotation
 * @ingroup tools
 * @param Point A, X coordinate
 * @param Point A, Y coordinate
 * @param Point B, X coordinate
 * @param Point B, Y coordinate
 * @retval Output rotation value angle in rads
 */
float getAngleToAlignToZero_tools(float aX,float aY,float bX,float bY) ;

/**
 * @brief This is a wrapper for the getAngleToAlignToZero_tools function that applies it in a vector of positions while also optionally centering everything using a reference joint.
 * @ingroup tools
 * @param Vector of 2D points 
 * @param Index to the 2D point to be treated as alignment center
 * @param Index to reference joint
 * @retval Output rotation value angle in rads
 */
float getAngleToAlignToZero(std::vector<float> &positions,unsigned int centerJoint,unsigned int referenceJoint);


/**
 * @brief This call aligns 2D points based on a single point that will be used as a pivot.
 * @ingroup tools
 * @param Vector of 2D points 
 * @param Angle to rotate all points at
 * @param Index to the 2D point to be treated as alignment center 
 * @retval 1=Success/0=Failure
 */
int rotate2DPointsBasedOnJointAsCenter(std::vector<float> &positions,float angle,unsigned int centerJoint);
 

void normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                                                          float * x,
                                                          float * y, 
                                                          unsigned int currentWidth,
                                                          unsigned int currentHeight,
                                                          unsigned int respectTrainingAspectRatio,
                                                          unsigned int trainingWidth,
                                                          unsigned int trainingHeight
                                                         );
 
int convertStringToLowercase(
                             char * target,
                             unsigned int targetLength,
                             const char * source
                            );
 
 
int  findFirstFormattedFileInDirectory(
                                        const char * path,
                                        const char * formatString,
                                        const char * label,
                                        unsigned int * frameIDOutput
                                      );

/**
 * @brief Count the number of empty ( zero ) elements in a vector
 * @ingroup tools
 * @param Vector
 * @retval Number of zeros encountered
 */
unsigned int getNumberOfEmptyElements(std::vector<float> mocapNETInput);

/**
 * @brief Execute a shell command
 * @ingroup tools
 * @param CString to execute
 * @param Pointer to array that will contain our result
 * @param Size of array that will contain our result
 * @param If we have more than one lines of output which one we want?
 * @bug This function executes a string in the shell and this means that it is dangerous
 * @retval 1 = Success loading the files  , 0 = Failure
 */
int executeCommandLineNum(
                           const char *  command ,
                           char * what2GetBack ,
                           unsigned int what2GetBackMaxSize,
                           unsigned int lineNumber
                         );

/**
 * @brief Get the CPU name 
 * @param CString where the CPU name should be stored
 * @param Size of array that will contain our result
 * @ingroup tools
 * @retval 1=Success/0=Failure
 */
int getCPUName(char * str,unsigned int maxLength);

/**
 * @brief Get the GPU name 
 * @param CString where the GPU name should be stored
 * @param Size of array that will contain our result
 * @ingroup tools
 * @retval 1=Success/0=Failure
 */
int getGPUName(char * str,unsigned int maxLength);

/**
 * @brief Get uptime in microseconds
 * @ingroup tools
 * @retval Microseconds of system uptime
 */
unsigned long GetTickCountMicrosecondsMN();

/**
 * @brief Get uptime in milliseconds
 * @ingroup tools
 * @retval Milliseconds of system uptime
 */
unsigned long GetTickCountMillisecondsMN();

/**
 * @brief Check if a directory exists on our filesystem
 * @ingroup tools
 * @retval 1=Exists/0=Doesnt Exist
 */
char directoryExists(const char * folder);

/**
 * @brief Check if a filename exists on our filesystem
 * @ingroup tools
 * @retval 1=Exists/0=Doesnt Exist
 */
char fileExists(const char * filename);

/**
 * @brief Convert start and end time to a framerate ( frames per second )
 * @ingroup demo
 * @retval Will return a framerate from two millisecond timestamps, if no time duration has been passed there is no division by zero.
 */
float convertStartEndTimeFromMicrosecondsToFPS(unsigned long startTime, unsigned long endTime);

/**
 * @brief Get the dimensions of an image by relying on the identify tool
 * @ingroup tools
 * @param CString with path to the image file to get dimensions for
 * @param Pointer to an unsigned int that will hold the width of the image file we specified
 * @param Pointer to an unsigned int that will hold the height of the image file we specified
 * @retval 1=Success/0=Failure
 */
int getImageWidthHeight(
                        const char * filename,
                        unsigned int * width ,
                        unsigned int * height
                       );


