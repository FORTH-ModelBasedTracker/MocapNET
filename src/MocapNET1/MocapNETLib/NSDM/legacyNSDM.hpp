#pragma once

/** @file nsdm.hpp
 *  @brief Code that handles the NSDM conversion described in the MocapNET BMVC 2019 paper 
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>

#include "../../MocapNETLib/mocapnet.hpp"
#include "../../MocapNETLib/jsonCocoSkeleton.h"
#include <math.h>

 /**
 * @brief NSDM matrices need to be full to function properly, the more elements missing the worse the occlusions are and the worse the retrieved result is going to be
 * this is a limit to protect the output from noise when facing sever occlusions
*/
const int MAXIMUM_NUMBER_OF_NSDM_ELEMENTS_MISSING=110; // 102

static float get2DPointsDistance(float x1,float y1,float x2,float y2)
{
    return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}
 

 /**
 * @brief One of the most basic concepts of MocapNET is the NSDM or Normalized Signed Distance Matrix.
 * This call converts the raw 2D input to such a matrix and returns it as a vector.
 * @param Vector of 2D Input 
 * @param Switch to also add the synthetic points ( should be always 1 )
 * @param Switch to perform Scale Compensation a.k.a. Normalization this is the (N) of (N)SDM and should always be 1
 * @retval Vector with NSDM version of our input
 */ 
static std::vector<float> compressLegacyMocapNETInputToNSDM(std::vector<float> mocapnetInput,int addSyntheticPoints,int doScaleCompensation)
{
    if ( (MOCAPNET_UNCOMPRESSED_JOINT_PARTS * 3!=mocapnetInput.size())||(mocapnetInput.size()!=171) )
        {
            fprintf(stderr,"mocapNET: compressMocapNETInputToNSDM : wrong input size , received %lu expected 171\n" ,mocapnetInput.size());

            return mocapnetInput;
        }


    //---------------------------------------------------
    float rShoulderToHipDistance = get2DPointsDistance
                                   (
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_HIP*3+0],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_HIP*3+1],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_RSHOULDER*3+0],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_RSHOULDER*3+1]
                                   );
    //---------------------------------------------------
    float lShoulderToHipDistance = get2DPointsDistance
                                   (
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_HIP*3+0],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_HIP*3+1],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_LSHOULDER*3+0],
                                       mocapnetInput[MOCAPNET_UNCOMPRESSED_JOINT_LSHOULDER*3+1]
                                   );

    //std::cerr<<"rShoulderToHipDistance "<<rShoulderToHipDistance<<"\n";
    //std::cerr<<"lShoulderToHipDistance "<<lShoulderToHipDistance<<"\n";


    //---------------------------------------------------
    float scaleDistance=1.0;
    if ( (rShoulderToHipDistance!=0) && (lShoulderToHipDistance!=0) )
        {
            scaleDistance=(rShoulderToHipDistance+lShoulderToHipDistance)/2;
        }
    else if (rShoulderToHipDistance!=0)
        {
            scaleDistance=rShoulderToHipDistance;
        }
    else if (lShoulderToHipDistance!=0)
        {
            scaleDistance=lShoulderToHipDistance;
        }


    //std::cerr<<"mocapnetCompressed\n";

    std::vector<float>  mocapnetCompressed;
    for (unsigned int iI=0; iI<MocapNETInputCompressedArrayIndexesSize; iI++)
        {
            unsigned int i=MocapNETInputCompressedArrayIndexes[iI];
            for (unsigned int jJ=0; jJ<MocapNETInputCompressedArrayIndexesSize; jJ++)
                {
                    unsigned int j=MocapNETInputCompressedArrayIndexes[jJ];

                    if (i>=MOCAPNET_UNCOMPRESSED_JOINT_PARTS)
                        {
                            fprintf(stderr, "\nERROR at i=%u for EDM element  [%u,%u]\n",i,iI,jJ);
                            fprintf(stderr, "%s\n",MocapNETInputUncompressedArrayNames[i*3+0]);
                            fprintf(stderr, "%s\n",MocapNETInputUncompressedArrayNames[i*3+1]);
                            fprintf(stderr, "%s\n",MocapNETInputUncompressedArrayNames[i*3+2]);
                            exit(0);
                        }
                    if (j>=MOCAPNET_UNCOMPRESSED_JOINT_PARTS)
                        {
                            fprintf(stderr, "\nERROR at j=%u for EDM element [%u,%u]\n",j,iI,jJ);
                            fprintf(stderr, "%s\n",MocapNETInputUncompressedArrayNames[j*3+0]);
                            fprintf(stderr, "%s\n",MocapNETInputUncompressedArrayNames[j*3+1]);
                            fprintf(stderr, "%s\n",MocapNETInputUncompressedArrayNames[j*3+2]);
                            exit(0);
                        }

                    float iX = mocapnetInput[i*3+0];
                    float iY = mocapnetInput[i*3+1];
                    float jX = mocapnetInput[j*3+0];
                    float jY = mocapnetInput[j*3+1];

                    if ( (iX>1.0) || (iY>1.0) || (jX>1.0) || (jY>1.0) )
                        {
                            //This should never happen
                            fprintf(stderr, "\nBigger than 1.0 element at [%u,%u]\n",iI,jJ);
                            mocapnetCompressed.push_back(666.0);
                            mocapnetCompressed.push_back(666.0);
                        }
                    else if ( (iX==0) || (iY==0) || (jX==0) || (jY==0) )
                        {
                            mocapnetCompressed.push_back(0.0);
                            mocapnetCompressed.push_back(0.0);
                        }
                    else
                        {
                            //#--------------------------
                            //#     Synthetic Points
                            //#--------------------------
                            if (addSyntheticPoints)
                                {
                                    if (i==7)
                                        {
                                            iX=iX-0.3;
                                        }
                                    else if (i==8)
                                        {
                                            iX=iX+0.3;
                                        }
                            //#--------------------------
                                    if (j==7)
                                        {
                                            jX=jX-0.3;
                                        }
                                    else if (j==8)
                                        {
                                            jX=jX+0.3;
                                        }
                                }
                            //#--------------------------

                            float iXMinusjXPlus0_5=0.5+iX-jX;
                            float iYMinusjYPlus0_5=0.5+iY-jY;

                            if (iXMinusjXPlus0_5>10.0)
                                {
                                    fprintf(stderr, "\nERROR at (%0.2f,%0.2f)/(%0.2f,%0.2f)  for NSDM element [%u,%u]\n",iX,iY,jX,jY,iI,jJ);
                                    fprintf(stderr, "%s-%s\n",MocapNETInputUncompressedArrayNames[i*3+0],MocapNETInputUncompressedArrayNames[j*3+0]);
                                    fprintf(stderr, "%s-%s\n",MocapNETInputUncompressedArrayNames[i*3+1],MocapNETInputUncompressedArrayNames[j*3+1]);
                                }
                            if (iYMinusjYPlus0_5>10.0)
                                {
                                    fprintf(stderr, "\nERROR at (%0.2f,%0.2f)/(%0.2f,%0.2f)  for NSDM element [%u,%u]\n",iX,iY,jX,jY,iI,jJ);
                                    fprintf(stderr, "%s-%s\n",MocapNETInputUncompressedArrayNames[i*3+0],MocapNETInputUncompressedArrayNames[j*3+0]);
                                    fprintf(stderr, "%s-%s\n",MocapNETInputUncompressedArrayNames[i*3+1],MocapNETInputUncompressedArrayNames[j*3+1]);
                                }

                            if ( (doScaleCompensation) && (scaleDistance>0.0) )
                                {
                                    mocapnetCompressed.push_back( (float) iXMinusjXPlus0_5/scaleDistance );
                                    mocapnetCompressed.push_back( (float) iYMinusjYPlus0_5/scaleDistance );
                                }
                            else
                                {
                                    mocapnetCompressed.push_back(iXMinusjXPlus0_5);
                                    mocapnetCompressed.push_back(iYMinusjYPlus0_5);
                                }
                        }
                }
        }


    return  mocapnetCompressed;
}

 