/** @file nsdm.hpp
 *  @brief Code that handles the NSDM conversion described in the MocapNET BMVC 2019 paper 
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>



 /**
 * @brief One of the most basic concepts of MocapNET is the NSDM or Normalized Signed Distance Matrix.
 * This call converts the raw 2D input to such a matrix and returns it as a vector.
 * @param Vector of 2D Input 
 * @param Switch to also add the synthetic points ( should be always 1 )
 * @param Switch to perform Scale Compensation a.k.a. Normalization this is the (N) of (N)SDM and should always be 1
 * @retval Vector with NSDM version of our input
 */ 
std::vector<float> compressMocapNETInputToNSDM(
                                                std::vector<float> mocapnetInput,
                                                int addSyntheticPoints,
                                                int doScaleCompensation
                                              );
