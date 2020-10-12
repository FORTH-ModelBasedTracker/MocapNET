#pragma once
/** @file conversions.hpp
 *  @brief Unfortunately due to the complexity of the problem and the different libraries used there cannot be a single skeleton representation.
 * This module handles conversions between std::vector<float> which is used by Tensorflow/NeuralNetwork layer , skeletonSerialized which is used by MocapNET and Skeletons2DDetected which is used by the 2D estimator
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
 
#include <vector>
#include "../mocapnet2.hpp"
#include "../../../JointEstimator2D/jointEstimator2D.hpp"


int appendVectorToFile(const char * filename, std::vector<float> vec);

int initializeAssociationsForSubsetOfSkeleton(
                                               unsigned int * targetIndexIsInitializedFlag,
                                               unsigned int * targetIndexes,
                                               unsigned int targetLength,
                                               const char * * targetLabels,
                                               struct skeletonSerialized * input
                                              );

 

std::vector<float> deriveMocapNET2InputUsingAssociations(
                                                          struct MocapNET2 * mnet,
                                                          struct skeletonSerialized * input,
                                                          unsigned int * targetIndexIsInitializedFlag,
                                                          unsigned int * targetIndexes,
                                                          unsigned int targetLength,
                                                          const char * * targetLabels,
                                                          int verbose
                                                        );



int convertSkeletons2DDetectedToSkeletonsSerialized(
    struct skeletonSerialized * output,
    struct Skeletons2DDetected * input,
    unsigned int frameNumber,
    unsigned int width,
    unsigned int height
);


int convertMocapNET2OutputToSkeletonSerialized(
                                                struct MocapNET2 * mnet , 
                                                struct skeletonSerialized * output , 
                                                std::vector<std::vector<float> > mocapNET2DPointsResult,
                                                unsigned int frameNumber,
                                                unsigned int width,
                                                unsigned int height
                                              );
                                              
                                              

int affineSkeletonRotation(struct skeletonSerialized * input,float degrees);

float  rotationRequiredToMakeSkeletonCloserToTrainingDataset(struct skeletonSerialized * input);

int makeSkeletonUpright(struct skeletonSerialized * input);
