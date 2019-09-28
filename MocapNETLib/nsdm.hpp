/** @file nsdm.hpp
 *  @brief Code that handles the NSDM conversion described in the MocapNET BMVC 2019 paper 
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>

 
 
std::vector<float> compressMocapNETInputToNSDM(std::vector<float> mocapnetInput,int addSyntheticPoints,int doScaleCompensation);
