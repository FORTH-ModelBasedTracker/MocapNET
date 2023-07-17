#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import numpy as np
from enum import Enum

goFromDegreesToRad=np.float32(np.pi/180.0)
goFromRadToDegrees=np.float32(180.0/np.pi)

def getJoint2DDistancePoints(aX,aY,bX,bY): 
    xDistance=np.float32(bX-aX)
    yDistance=np.float32(bY-aY)
    return np.float32(np.sqrt( (xDistance*xDistance) + (yDistance*yDistance) ))

def getNumberOfSquaredCompressedSquaredJoints():
    countList=len(getBodyNoHandsList())
    numberOfSquaredCompressedJoints=countList*countList*2
    return numberOfSquaredCompressedJoints


def getAngleToAlignToZero(iX,iY,jX,jY,NSRMNormalizeAngles=0):
    #---------------------------------------------    
    if ( (iX==jX) and (iY==jY) ):
             return np.float32(0.0)
    #---------------------------------------------
    #We have points a, b and c and we want to calculate angle b
    aX= iX*100
    aY= iY*100
    #---------------------------------------------  
    bX= jX*100
    bY= jY*100
    #---------------------------------------------
    lengthBetweenAAndB = getJoint2DDistancePoints(aX,aY,bX,bY)
    #---------------------------------------------
    cX= bX
    cY= bY - lengthBetweenAAndB
    #---------------------------------------------
    #fprintf(stderr,"We want to align A(%0.2f,%0.2f) to C(%0.2f,%0.2f) with pivot B(%0.2f,%0.2f)\n",aX,aY,cX,cY,bX,bY)
    #fprintf(stderr,"length  AB = %0.2f\n",lengthBetweenAAndB);    
    #fprintf(stderr,"bY = %0.2f\n",bY);
    #fprintf(stderr,"cY = %0.2f = %0.2f - %0.2f\n",cY,bY,lengthBetweenAAndB);
    #Calulate vector a->b
    abX = bX - aX
    abY = bY - aY
    #calculate vector c->b
    cbX = bX - cX
    cbY = bY - cY
    #---------------------------------------------
    dot   = np.float32( (abX * cbX) + (abY * cbY) ) # dot product
    cross = np.float32( (abX * cbY) - (abY * cbX) ) # cross product
    #---------------------------------------------
    alpha = np.arctan2(cross,dot) #arctan2 returns a value in the range [-pi, pi]
    #---------------------------------------------
    #fprintf(stderr,"Angle is %0.2f rad or %0.2f degrees \n",alpha,alpha*goFromRadToDegrees);
    if (NSRMNormalizeAngles==2):
       return np.float32( (2.0*alpha) / np.pi) # Normalize output in range [-1..1]
    if (NSRMNormalizeAngles==1):
       #This should actually be (2*alpha) / np.pi
       #arctan(R) -> [ -pi/2 , pi/2] 
       return np.float32(alpha / np.pi) # Normalize output in range [-1/2..1/2]
    else:
       return np.float32(alpha) # Output in range [-pi..pi]


"""
 This is the new function to make resolving 2D coordinates on a vector easier and safer (but slower :P) 
"""
def getJoint2DXYV(rules,positions,jointName):
    #------------------------------------------------------------------------------------------
    x          = np.float32(0.0)
    y          = np.float32(0.0)
    visibility = np.float32(0.0) 
    #------------------------------------------------------------------------------------------
    positionDataLength = len(positions) 
    if (positionDataLength==0):
       print("getJoint2DXYV cannot resolve positions for joint ",jointName," with no vector data")
       return x,y,visibility
   
    if (not rules['inputJointMap'].checkJointListDimensions(positions)):
       print("getJoint2DXYV cannot resolve positions for joint ",jointName," input joint map has a different dimension size..")
       return x,y,visibility



    #Get correct indexes for jointName
    #--------------------------------------------------------
    jID_X   = rules['inputJointMap'].getJointID_2DX(jointName)
    jID_Y   = rules['inputJointMap'].getJointID_2DY(jointName)
    jID_Vis = rules['inputJointMap'].getJointID_Visibility(jointName)
    #--------------------------------------------------------
    if ( (positionDataLength<=jID_X) or (positionDataLength<=jID_Y) or (positionDataLength<=jID_Vis) ):
       print("getJoint2DXYV cannot get positions for joint ",jointName," with a vector data of only ",positionDataLength)
       return x,y,visibility
    #--------------------------------------------------------
    if ( (jID_X==-1) or (jID_Y==-1) or (jID_Vis==-1) ):
       print("getJoint2DXYV could not resolve joint ",jointName," with vector data of ",positionDataLength)
       return x,y,visibility
    #--------------------------------------------------------
    #print("getJoint2DXYV(%s->%u/%u/%u) length %u"%(jointName,jID_X,jID_Y,jID_Vis,positionDataLength))
    #Pedantic behavior on missing data
    if (positions[jID_X]==0.0) or (positions[jID_Y]==0.0) or (positions[jID_Vis]==0.0):
       return x,y,visibility
    #------------------------------------ 
    x          = np.float32(positions[jID_X])
    y          = np.float32(positions[jID_Y])
    visibility = np.float32(positions[jID_Vis]) 
    #------------------------------------
    return x,y,visibility



def rotate2DPointsTest(cx,cy,jX,jY,angleToRotateInRadians):
 #-----------------------------------------------
 s = np.float32( np.sin(angleToRotateInRadians) )
 c = np.float32( np.cos(angleToRotateInRadians) )
 #-----------------------------------------------
 jX = np.float32(jX - cx)
 jY = np.float32(jY - cy)
 #-----------------------------------------------
 xnew = np.float32( (jX * c) - (jY * s) )
 ynew = np.float32( (jX * s) + (jY * c) )
 #-----------------------------------------------
 return xnew + cx , ynew + cy


def rotate2DPointsBasedOnJointAsCenter(rules,positions,angleToRotateInRadians,jointNameCenter):
    if (len(positions)==0):
            print("rotate2DPointsBasedOnJointAsCenter cannot work without input.. \n")
            return positions

    s = np.float32( np.sin(angleToRotateInRadians) )
    c = np.float32( np.cos(angleToRotateInRadians) )
            
    #-------------------------------------------------------- 
    cx,cy,cVisibility = getJoint2DXYV(rules,positions,jointNameCenter) 
    #--------------------------------------------------------
 
    if (cVisibility==0.0):
            print("rotate2DPointsBasedOnJointAsCenter: cannot work without pivot joint .. \n")
            return positions

    
    result = positions

    for jID in range(0,int(len(rules['NSDM'])) ):
            #--------------------------------------------------------
            jointName=rules['NSDM'][jID]['joint']
            jX,jY,jVisibility = getJoint2DXYV(rules,positions,jointName) 
            #--------------------------------------------------------
            #printf("Rotating point %0.2f,%0.2f using pivot %0.2f,%0.2f by %0.2f deg  -> "%(jX,jY,cx,cy,angle))
 
            #Translate point back to origin:
            jX = np.float32(jX - cx)
            jY = np.float32(jY - cy)
 
            #Rotate point
            xnew = np.float32( (jX * c) - (jY * s) )
            ynew = np.float32( (jX * s) + (jY * c) )
 
            #Translate point back:
            jID_X   = rules['inputJointMap'].getJointID_2DX(jointName)
            jID_Y   = rules['inputJointMap'].getJointID_2DY(jointName)
            jID_Vis = rules['inputJointMap'].getJointID_Visibility(jointName)
            #----------------------------------------------------------------
            result[jID_X]   = np.float32(xnew + cx)
            result[jID_Y]   = np.float32(ynew + cy)
            result[jID_Vis] = np.float32(jVisibility)
            
            #printf("%0.2f,%0.2f\n"%(result[jID*3+0],result[jID*3+1]));

    return result




def performNSRMAlignment(thisInput,configuration):
           angleToRotateInRadians = 0.0
           NSRMUseAlignmentToPivot = configuration['eNSRM']
           NSRMNormalizeAngles     = 0
           if ("NSRMNormalizeAngles" in configuration) and (configuration["NSRMNormalizeAngles"]==1): 
                                              NSRMNormalizeAngles = 1
           if (NSRMUseAlignmentToPivot==1):
              pivotPoint     = configuration['Alignment'][0]['jointStart']
              referencePoint = configuration['Alignment'][0]['jointEnd']
              #-----------------------------------------------------------------------------------------------
              if (pivotPoint!=referencePoint):
                pivotX,pivotY,pivotVisibility = getJoint2DXYV(configuration,thisInput,pivotPoint) 
                #--------------------------------------------------------------------------------------------
                referenceX,referenceY,referenceVisibility = getJoint2DXYV(configuration,thisInput,referencePoint)
                #--------------------------------------------------------------------------------------------        
                if ((pivotVisibility!=0) and (referenceVisibility!=0)): 
                    angleToRotateInRadians = getAngleToAlignToZero(pivotX,pivotY,referenceX,referenceY,NSRMNormalizeAngles)
                    rotatedInput           = rotate2DPointsBasedOnJointAsCenter(configuration,thisInput,angleToRotateInRadians,pivotPoint)
                    return angleToRotateInRadians,rotatedInput
              else:
                print("Pivot Point ",pivotPoint," and Reference Point ",referencePoint," are the same\n")
              #-----------------------------------------------------------------------------------------------
           return angleToRotateInRadians,thisInput
           



def getCompositeLabel(jointA,jointB,xOffset,yOffset,virtualPointType):
  labelI=jointA
  labelIX=""
  labelIY=""
  if (virtualPointType==1):
    if (xOffset<0):
        labelIX="minus"
    elif (xOffset>0):
        labelIX="plus"
    #----------------------------------
    if (yOffset<0):
        labelIY="minus"
    elif (yOffset>0):
        labelIY="plus"
    #----------------------------------
    labelI="virtual_"+jointA+"_x_"+labelIX+str(xOffset).replace('.','_').replace('-','_')+"_y_"+labelIY+str(yOffset).replace('.','_').replace('-','_')
  elif (virtualPointType==2):
    #----------------------------------
    labelI="halfway_"+jointA+"_and_"+jointB
  return labelI






def NSDMLabels(rules): 
    result=list() 
    
    useXY=1
    useAngles=0
    if (rules['NSDMAlsoUseAlignmentAngles']==1):
       useXY=0
       useAngles=1

    numberOfNSDMRules=len(rules['NSDM'])  
    print("Rules Number ",numberOfNSDMRules)

    for i in range(0,numberOfNSDMRules):
        for j in range(0,numberOfNSDMRules):
           if (i==j):
              if (i==0):
                 result.append("NSRM-angleUsedFor2DRotation_%u"%(i))
              else:
                 result.append("NSRM-scaleBetween_"+rules['NSDM'][i]['joint']+"_and_"+rules['NSDM'][i]['joint']) 
           else:
              #-----------------------------------------------------------------
              labelI = getCompositeLabel(
                                          rules['NSDM'][i]['joint'],
                                          rules['NSDM'][i]['halfWayFromThisAnd'],
                                          rules['NSDM'][i]['xOffset'],
                                          rules['NSDM'][i]['yOffset'],
                                          rules['NSDM'][i]['isVirtual']
                                        )
              #-----------------------------------------------------------------
              labelJ = getCompositeLabel(
                                          rules['NSDM'][j]['joint'],
                                          rules['NSDM'][j]['halfWayFromThisAnd'],
                                          rules['NSDM'][j]['xOffset'],
                                          rules['NSDM'][j]['yOffset'],
                                          rules['NSDM'][j]['isVirtual']
                                        ) 
              #-----------------------------------------------------------------
              if (useXY):
                 result.append("NSDM-%sX-%sX"%(labelI,labelJ))
                 result.append("NSDM-%sY-%sY"%(labelI,labelJ))
              if (useAngles):
                 result.append("NSRM-%sY-%sY-Angle"%(labelI,labelJ))
              #-----------------------------------------------------------------
              
    #print("NSDM matrix will look like this ",result)               
    return result; 

def inputIsEnoughToCreateNSDM(rules,thisInput):
    numberOfNSDMRules=len(rules['NSDM'])
    numberOfJointIDs =len(thisInput)
    for i in range(0,numberOfNSDMRules):
      jointName=rules['NSDM'][i]['joint']
      if (not rules['inputJointMap'].getJointID_Exists(jointName)):
         return False
    return True

def getListOfMissingNSRMJoints(rules,thisInput):
    missingList=list()
    numberOfNSDMRules=len(rules['NSDM'])
    numberOfJointIDs =len(thisInput)
    for i in range(0,numberOfNSDMRules):
      jointName=rules['NSDM'][i]['joint']
      if (not rules['inputJointMap'].getJointID_Exists(jointName)):
         missingList.append(jointName)
      else:
         iX,iY,iVisibility = getJoint2DXYV(rules,thisInput,jointName)
         if (iVisibility==0.0): 
            missingList.append(jointName)
    return missingList



def getCompositePoint(rules,i,thisInput):
      #-----------------------------------------------------------
      if (len(thisInput)==0): 
             print("getCompositePoint called with no input for element ",i)
             return np.float32(0.0),np.float32(0.0),np.float32(0.0),1
      #--------------------------------------------------------
      invalidPoint = 0
      jointName    = rules['NSDM'][i]['joint']
      #-----------------------------------------------------------
      iX,iY,iVisibility = getJoint2DXYV(rules,thisInput,jointName)
      #-----------------------------------------------------------
      if ( iX!=0.0 and iY!=0.0 and iVisibility!=0.0 ):
         #---------------------------------------------------------------------------
         #                           Synthetic Points
         #---------------------------------------------------------------------------
         if (rules['NSDM'][i]['isVirtual']==1):
                    iX=iX+rules['NSDM'][i]['xOffset']
                    iY=iY+rules['NSDM'][i]['yOffset']
         elif (rules['NSDM'][i]['isVirtual']==2):
                    secondTargetJointName=rules['NSDM'][i]['halfWayFromThisAnd']
                    secondTargetX,secondTargetY,secondTargetVisibility = getJoint2DXYV(rules,thisInput,secondTargetJointName)
                    if ((secondTargetX!=0.0) or (secondTargetY!=0.0)):
                            iX=np.float32((iX+secondTargetX)/2)
                            iY=np.float32((iY+secondTargetY)/2)
                    else:
                            invalidPoint = 1
                            iX           = np.float32(0.0)
                            iY           = np.float32(0.0)
                            iVisibility  = np.float32(0.0)
         #---------------------------------------------------------------------------
      else:
         #Added : Fixed bug! 11/5/2023
         #If either of X,Y is zero we treat the point as completely invisible
         invalidPoint = 1
         iX           = np.float32(0.0)
         iY           = np.float32(0.0)
         iVisibility  = np.float32(0.0)
      #-----------------------------------------------------------
      return iX,iY,iVisibility,invalidPoint




def createNSDMUsingRules(rules,thisInput,angleUsedToRotateInput): 
    result=list()
    #-----------------------------------------------------------------------------------------------------
    if (len(thisInput)==0): 
             print("createNSDMUsingRules called with no input")
             return result


    if (not rules['inputJointMap'].checkJointListDimensions(thisInput)):
             print("createNSDMUsingRules called with incorrect input size ")
             return thisInput
    #-----------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------
    NSRMNormalizeAngles     = 0
    if ("NSRMNormalizeAngles" in rules) and (rules["NSRMNormalizeAngles"]==1): 
                                              NSRMNormalizeAngles = 1

    doNormalization = (rules['NSDMNormalizationMasterSwitch']==1)
    useXY     = True
    useAngles = False
    if (rules['eNSRM']==1) or (rules['NSDMAlsoUseAlignmentAngles']==1):
       useXY = False
       useAngles = True
       doNormalization = False
    #-----------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------

    #-----------------------------------------------------------------------------------------------------
    #                                     ..Main NSRM parameters ..
    #-----------------------------------------------------------------------------------------------------
    numberOfNSDMRules=len(rules['NSDM'])
    for i in range(0,numberOfNSDMRules):
         #---------------------------------------------------------------------------      
         iX,iY,iVisibility,iInvalidPoint = getCompositePoint(rules,i,thisInput)
         #---------------------------------------------------------------------------
         for j in range(0,numberOfNSDMRules):
           #---------------------------------------------------------------------------
           jX,jY,jVisibility,jInvalidPoint = getCompositePoint(rules,j,thisInput)
           #---------------------------------------------------------------------------
           if (iInvalidPoint or jInvalidPoint):
                   #If any of the two joints is invalid, invalidate all output
                   if (useXY):
                     result.append(np.float32(0.0))
                     result.append(np.float32(0.0))
                   if (useAngles):
                     result.append(np.float32(0.0))
           else: 
                   if (useXY):
                     result.append(np.float32(iX-jX))
                     result.append(np.float32(iY-jY)) 
                   if (useAngles):
                     result.append(getAngleToAlignToZero(iX,iY,jX,jY,NSRMNormalizeAngles))
           #---------------------------------------------------------------------------

    #-----------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------
    #                                     ..New eNSRM diagonal parameters ..
    #----------------------------------------------------------------------------------------------------- 
    if (rules['eNSRM']==1):
      elementID=0
      #------------------------------------------------------------
      iJointName=rules['NSDM'][0]['joint'] #Pivot point
      iX,iY,iVisibility = getJoint2DXYV(rules,thisInput,iJointName)
      #------------------------------------------------------------
      for j in range(0,numberOfNSDMRules): 
            elementID = j * numberOfNSDMRules + j  # Calculate diagonal elementID based on j
            #---------------------------------------------------------------
            jJointName = rules['NSDM'][j]['joint']
            jX,jY,jVisibility = getJoint2DXYV(rules,thisInput,jJointName)
            #---------------------------------------------------------------
            if (jVisibility!=0.0) and (iVisibility!=0.0):
                    #Populate diagonal elements with distance from our pivot point
                    result[elementID] = getJoint2DDistancePoints(iX,iY,jX,jY)
            else:
                    result[elementID] = np.float32(0.0)
            #---------------------------------------------------------------
      #Overwrite first (null) element of NSRM matrix with the angle used to rotate the input
      result[0]=np.float32(angleUsedToRotateInput);
    #-----------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------

    if (doNormalization):
     #normalization is made for NSDM not xNSRM
     #Normalizing results.. ---------------------------------------
     numberOfNSDMScalingRules=len(rules['NormalizeNSDMBasedOn'])  
     if (numberOfNSDMScalingRules>0):
        numberOfDistanceSamples=0
        sumOfDistanceSamples=0.0
        for i in range(0,numberOfNSDMScalingRules):
            #------------------------------------------------------------------------
            iJointName=rules['NormalizeNSDMBasedOn'][i]['jointStart']
            iX,iY,iVisibility = getJoint2DXYV(rules,thisInput,iJointName)
            #------------------------------------------------------------------------
            jJointName=rules['NormalizeNSDMBasedOn'][i]['jointEnd']
            jX,jY,jVisibility = getJoint2DXYV(rules,thisInput,jJointName)
            #------------------------------------------------------------------------
            if (iJointName==jJointName):
               print("Error: Normalization Rule ",i," points to same start/end joint ",iJointID," == ",jJointID)
            distance = getJoint2DDistancePoints(iX,iY,jX,jY)
            if (distance>0.0):
                 numberOfDistanceSamples=numberOfDistanceSamples+1
                 sumOfDistanceSamples=sumOfDistanceSamples+distance

        #------------------------------------------------------------------------------------------------- 
        scaleDistance=1.0
        #------------------------------------------------------------------------------------------------- 
        if (numberOfDistanceSamples>0):
            scaleDistance=sumOfDistanceSamples/numberOfDistanceSamples
        #-------------------------------------------------------------------------------------------------
        #print("NSDM Scale = ",scaleDistance," \n")
        if (scaleDistance!=1.0):
            for i in range(0,len(result)):
                  result[i]=np.float32(result[i]/scaleDistance)
     #-------------------------------------------------------------------------------------------------
    
    #-----------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------
    if (useXY):
       for i in range(0,len(result)):
          if (result[i]!=0.0):
                result[i]=np.float32(0.5+result[i])
    #-------------------------------------------------------------------------------------------------
    #print(result)    
    #print("Result has ", len(result), " elements " )   
    #print("Result should have ", int( 2 * len(bodyWithoutHands) *  len(bodyWithoutHands) ), " elements " ) 
    return result; 

 

if __name__ == '__main__':
    print("NSDM.py is a library it cannot be run standalone")
     
    sign = 1.0 #This is positive +1.0

    iX=0.5; iY=0.5; jX=1.0; jY=1.0
    a = getAngleToAlignToZero(iX,iY,jX,jY)
    print("Points A(%0.2f,%0.2f) -> B(%0.2f,%0.2f) => angle %0.4f" %(iX,iY,jX,jY,a))
    jX,jY = rotate2DPointsTest(iX,iY,jX,jY,sign * a)
    print("Rotated it goes to -> B(%0.2f,%0.2f) => angle to align %0.4f" %(jX,jY,getAngleToAlignToZero(iX,iY,jX,jY)))

    iX=0.5; iY=0.5; jX=0.5; jY=1.0
    a = getAngleToAlignToZero(iX,iY,jX,jY)
    print("Points A(%0.2f,%0.2f) -> B(%0.2f,%0.2f) => angle %0.4f" %(iX,iY,jX,jY,a))
    jX,jY = rotate2DPointsTest(iX,iY,jX,jY,sign * a)
    print("Rotated it goes to -> B(%0.2f,%0.2f) => angle to align %0.4f" %(jX,jY,getAngleToAlignToZero(iX,iY,jX,jY)))

    iX=0.5; iY=0.5; jX=0.0; jY=1.0
    a = getAngleToAlignToZero(iX,iY,jX,jY)
    print("Points A(%0.2f,%0.2f) -> B(%0.2f,%0.2f) => angle %0.4f" %(iX,iY,jX,jY,a))
    jX,jY = rotate2DPointsTest(iX,iY,jX,jY,sign * a)
    print("Rotated it goes to -> B(%0.2f,%0.2f) => angle to align %0.4f" %(jX,jY,getAngleToAlignToZero(iX,iY,jX,jY)))

    iX=0.5; iY=0.5; jX=1.0; jY=0.5
    a = getAngleToAlignToZero(iX,iY,jX,jY)
    print("Points A(%0.2f,%0.2f) -> B(%0.2f,%0.2f) => angle %0.4f" %(iX,iY,jX,jY,a))
    jX,jY = rotate2DPointsTest(iX,iY,jX,jY,sign * a)
    print("Rotated it goes to -> B(%0.2f,%0.2f) => angle to align %0.4f" %(jX,jY,getAngleToAlignToZero(iX,iY,jX,jY)))

    iX=0.5; iY=0.5; jX=1.0; jY=0.0
    a = getAngleToAlignToZero(iX,iY,jX,jY)
    print("Points A(%0.2f,%0.2f) -> B(%0.2f,%0.2f) => angle %0.4f" %(iX,iY,jX,jY,a))
    jX,jY = rotate2DPointsTest(iX,iY,jX,jY,sign * a)
    print("Rotated it goes to -> B(%0.2f,%0.2f) => angle to align %0.4f" %(jX,jY,getAngleToAlignToZero(iX,iY,jX,jY)))

    iX=0.5; iY=0.5; jX=0.0; jY=0.0
    a = getAngleToAlignToZero(iX,iY,jX,jY)
    print("Points A(%0.2f,%0.2f) -> B(%0.2f,%0.2f) => angle %0.4f" %(iX,iY,jX,jY,a))
    jX,jY = rotate2DPointsTest(iX,iY,jX,jY,sign * a)
    print("Rotated it goes to -> B(%0.2f,%0.2f) => angle to align %0.4f" %(jX,jY,getAngleToAlignToZero(iX,iY,jX,jY)))

