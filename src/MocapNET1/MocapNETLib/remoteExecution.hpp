#pragma once


#include <iostream>
#include <vector>

#include "mocapnet.hpp"

std::vector<float> remoteExecution(struct MocapNET * mnet,std::vector<float> mnetInput);


void * intializeRemoteExecution(const char * ip,unsigned int port,unsigned int socketTimeoutSeconds);
int stopRemoteExecution(void * instance);