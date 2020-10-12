#pragma once
/** @file remoteExecution.hpp
 *  @brief This is a deprecated abstraction layer that allowed remote execution of the NeuralNetwork of MocapNET 1 , it is currently not working..
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>

#include "mocapnet2.hpp"

std::vector<float> remoteExecution(struct MocapNET2 * mnet,const std::vector<float> &mnetInput);


void * intializeRemoteExecution(const char * ip,unsigned int port,unsigned int socketTimeoutSeconds);
int stopRemoteExecution(void * instance);