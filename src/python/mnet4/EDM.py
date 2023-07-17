#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import numpy as np
from enum import Enum 
from NSDM import getCompositeLabel,getCompositePoint,getJoint2DDistancePoints

def EDMLabels(rules): 
    result=list()
    numberOfNSDMRules=len(rules['NSDM'])  
    print("Rules Number ",numberOfNSDMRules)

    for i in range(0,numberOfNSDMRules):
        #-----------------------------------------------------------------
        labelI = getCompositeLabel(
                                   rules['NSDM'][i]['joint'],
                                   rules['NSDM'][i]['halfWayFromThisAnd'],
                                   rules['NSDM'][i]['xOffset'],
                                   rules['NSDM'][i]['yOffset'],
                                   rules['NSDM'][i]['isVirtual']
                                  )
        #-----------------------------------------------------------------
        for j in range(0,numberOfNSDMRules):
            if (i>j):
              #-----------------------------------------------------------------
              labelJ = getCompositeLabel(
                                          rules['NSDM'][j]['joint'],
                                          rules['NSDM'][j]['halfWayFromThisAnd'],
                                          rules['NSDM'][j]['xOffset'],
                                          rules['NSDM'][j]['yOffset'],
                                          rules['NSDM'][j]['isVirtual']
                                        ) 
              #-----------------------------------------------------------------
              result.append("EDM-%sY-%sY-Distance"%(labelI,labelJ))
              
    #print("EDM matrix will look like this ",result)               
    return result; 


def createEDMUsingRules(rules,thisInput):
    result=list()
    #-----------------------------------------------------------------------------------------------------
    if (len(thisInput)==0): 
             print("createNSDMUsingRules called with no input")
             return result

    if (not rules['inputJointMap'].checkJointListDimensions(thisInput)):
             print("createNSDMUsingRules called with incorrect input size ")
             return thisInput
    #-----------------------------------------------------------------------------------------------------
    numberOfNSDMRules=len(rules['NSDM'])
    for i in range(0,numberOfNSDMRules):
        iX,iY,iVisibility,iInvalidPoint = getCompositePoint(rules,i,thisInput)
        for j in range(0,numberOfNSDMRules):
            if (i>j):
             # Ensure that each distance is computed only once since the EDM is a symmetric matrix. 
             #---------------------------------------------------------------------------
             jX,jY,jVisibility,jInvalidPoint = getCompositePoint(rules,j,thisInput)
             if (iInvalidPoint or jInvalidPoint): #Changed to or 17/5/23 <- Why was this AND and not OR ? also C++ EDM.h code
                   result.append(np.float32(0.0))
             else: 
                   result.append(getJoint2DDistancePoints(iX,iY,jX,jY))
            #---------------------------------------------------------------------------
    return result




if __name__ == '__main__':
    print("EDM.py is a library it cannot be run standalone")
