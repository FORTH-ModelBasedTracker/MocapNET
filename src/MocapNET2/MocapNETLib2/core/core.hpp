#pragma once


#include "../../MocapNETLib2/mocapnet2.hpp"


/**
 * @brief Given an orientation angle this function can decide on which orientation class it belong to corresponding to the MOCAPNET_Orientation enum
 * @param Angle in degrees
 * @retval MOCAPNET_Orientation enumeration value
 */
int  getMocapNETOrientationFromAngle(float direction);

int  getMocapNETOrientationFromOutputVector(std::vector<float> direction);


int localOrientationExtraction(struct MocapNET2SolutionPart * mnet,std::vector<float> mnetInput);



/**
 * @brief An internal function that handles local execution of a part of the final solution
 * @param Pointer to a valid and populated MocapNET2SolutionPart instance
 * @param The input to this MocapNET solution part
 * @param Orientation extracted from the localOrientationExtraction call
 * @param Some ensembles require an orientation change 
 * @retval 1=Success,0=Failure
 */
std::vector<float> localExecution(
                                   struct MocapNET2SolutionPart * mnet,
                                   std::vector<float> mnetInput,
                                   int orientation,
                                   int targetHasOrientationTrick
                                 );
                                 