#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import math
import sys
from enum import Enum 
from NSDM import getCompositeLabel,getCompositePoint,getJoint2DDistancePoints


def EigenPoseLabels(rules): 
    result=list()
    if ('eigenPoseData' in rules) and ('eigenPoses' in rules) and (int(rules['eigenPoses'])==1):
      numberOfEigenPoseRules=len(rules['eigenPoseData']['in'])  
      print("EigenPoses Rules Number ",numberOfEigenPoseRules)

      for i in range(0,numberOfEigenPoseRules):
        #-----------------------------------------------------------------
        result.append("EigenPose-%u"%(i))      
        #-----------------------------------------------------------------
    return result; 


def computeVectorSimilarity_SAD(vec1,vec2):
    loss=float(0.0)
    if (len(vec1)==len(vec2)):
       #If we are here the vectors have the same size
       for i in range(0,len(vec1)):
          loss = loss + abs(vec1[i]-vec2[i])
    return loss

def computeVectorSimilarity_MAD(vec1,vec2):
    loss=float(0.0)
    if (len(vec1)==len(vec2)):
       #If we are here the vectors have the same size
       for i in range(0,len(vec1)):
          loss = loss + abs(vec1[i]-vec2[i])
    return loss/len(vec1)

def computeVectorSimilarity(vec1,vec2,mode="MAD"):
    if (len(vec1)!=len(vec2)):
       print("EigenPoses.py: Asked for similarity on Vectors with Lengths",len(vec1)," vs ",len(vec2))
       sys.exit(0)
       return float("nan")
    #If we are here the vectors have the same size
    if (mode=="SAD"):
      return computeVectorSimilarity_SAD(vec1,vec2)
    if (mode=="MAD"):
      return computeVectorSimilarity_MAD(vec1,vec2)

def createEigenPosesUsingRules(rules,thisInput):
    result=list()
    if ('eigenPoseData' in rules) and ('eigenPoses' in rules) and (int(rules['eigenPoses'])==1):
      numberOfEigenPoseRules=len(rules['eigenPoseData']['in'])  
      for i in range(0,numberOfEigenPoseRules):
        result.append(computeVectorSimilarity(thisInput,rules['eigenPoseData']['in'][i])) 
    return result





if __name__ == '__main__':
    print("EigenPoses.py is a library it cannot be run standalone")
