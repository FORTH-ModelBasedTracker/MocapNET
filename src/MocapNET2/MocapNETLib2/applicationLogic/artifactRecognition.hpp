#pragma once
/** @file artifactRecognition.hpp
 *  @brief MocapNET Artifact recognition is implemented here. Artifacts are places in 3D space that can trigger specific events for applications that need human computer interaction
 *  @author Ammar Qammaz (AmmarkoV)
 */
 
#include <iostream>
#include <vector>

#define NUMBER_OF_ARTIFACTS 100

struct artifactData
{
    float x1;
    float y1;
    float z1;
    float x2;
    float y2;
    float z2;
    char is3D;
    char activatesOnOrientation;
    char activatesOnLocation;
    char activatesOnGestures;
    
    char hasAction;
    char active;
    char activatesOnLook;
    char activatesOnPosition;
    char activateOnGesture;
    char label[512];
    char actionToExecuteOnActivation[512];
    char actionToExecuteOnDeactivation[512];
};



/**
 * @brief  recorded gestures that can be used
 */
struct sceneArtifacts
{
    int numberOfArtifacts; 
    struct artifactData artifact[NUMBER_OF_ARTIFACTS]; 
};


//int initializeArtifacts(struct sceneArtifacts * scene);
 
int initializeArtifactsFromFile(struct sceneArtifacts * scene,const char * filename);

 
int check3DArtifactCollision(struct artifactData * artifact,float x1, float y1, float z1,float x2, float y2, float z2);

int checkArtifactCollision(struct artifactData * artifact,float x, float y, float r);

int checkArtifactDirection(struct artifactData * artifact,float x1, float y1, float x2,float y2);

int doIntersect(float p1X,float p1Y, float q1X, float q1Y,float p2X,float p2Y,float q2X, float q2Y) ;