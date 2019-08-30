#pragma once
/** @file tools.h
 *  @brief System Tools that don't fit somewhere else
 *  @author Ammar Qammaz (AmmarkoV)
 */

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


