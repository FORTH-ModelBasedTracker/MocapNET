#!/usr/bin/env python3
#Written by Ammar Qammaz a.k.a AmmarkoV - 2020

import h5py
import numpy as np
import csv
import os
import sys 
 
 
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


#Taken from https://github.com/una-dinosauria/3d-pose-baseline/blob/master/src/procrustes.py
def compute_similarity_transform(X, Y, compute_optimal_scale=False):
  """
  A port of MATLAB's `procrustes` function to Numpy.
  Adapted from http://stackoverflow.com/a/18927641/1884420
  Args
    X: array NxM of targets, with N number of points and M point dimensionality
    Y: array NxM of inputs
    compute_optimal_scale: whether we compute optimal scale or force it to be 1
  Returns:
    d: squared error after transformation
    Z: transformed Y
    T: computed rotation
    b: scaling
    c: translation
  """ 

  #We create a normalized version of X and Y called X0,Y0
  #that is centered on 0
  muX = X.mean(0)
  muY = Y.mean(0)

  X0 = X - muX
  Y0 = Y - muY

  ssX = (X0**2.).sum()
  ssY = (Y0**2.).sum()

  # centred Frobenius norm
  normX = np.sqrt(ssX)
  normY = np.sqrt(ssY)

  # scale to equal (unit) norm
  X0 = X0 / normX
  Y0 = Y0 / normY
  
  #For reference this is the SciPy version :
  #https://github.com/scipy/scipy/blob/v1.9.3/scipy/spatial/_procrustes.py#L15-L130
  #https://github.com/scipy/scipy/blob/v1.9.3/scipy/linalg/_procrustes.py#L12-L89

  # optimum rotation matrix of Y
  A = np.dot(X0.T, Y0)

  #full_matrices => bool, optional
  #If True (default), u and vh have the shapes (..., M, M) and (..., N, N), respectively. 
  #Otherwise, the shapes are (..., M, K) and (..., K, N), respectively, where K = min(M, N).
  U,s,Vt = np.linalg.svd(A,full_matrices=False)
  V      = Vt.T
  T      = np.dot(V, U.T)

  # Make sure we have a rotation
  detT     = np.linalg.det(T)
  V[:,-1] *= np.sign( detT )
  s[-1]   *= np.sign( detT )
  T        = np.dot(V, U.T)
  #-------------------------
  traceTA  = s.sum()
  #-------------------------
  if compute_optimal_scale:  # Compute optimum scaling of Y.
    b = traceTA * normX / normY
    d = 1 - traceTA**2
    Z = normX*traceTA*np.dot(Y0, T) + muX
  else:  # If no scaling allowed
    b = 1
    d = 1 + ssY/ssX - 2 * traceTA * normY / normX
    Z = normY*np.dot(Y0, T) + muX
  #-------------------------
  c = muX - b*np.dot(muY, T)
  #-------------------------
  return d, Z, T, b, c




"""
Calculate Area Under Curve (AUC)
"""
def AUC(values,minValue,maxValue):
 underCurve=0
 samples=len(values)
 for value in values: 
   if ((minValue<=value) and (value<=maxValue) ):
    underCurve=underCurve+1 
 return 100*(underCurve/samples)


"""
Pythagorean theorem, get the 3D distance between two 3D points given their X,Y,Z coordinates 
"""
def get3DDistance(jX,jY,jZ,pX,pY,pZ):
 return np.sqrt( ((jX-pX)*(jX-pX)) + ((jY-pY)*(jY-pY)) + ((jZ-pZ)*(jZ-pZ)) )


"""
Given two lists of 3D points A,B calculate their average distance 
"""
def calculateAverageDistanceOf3DPoints(A,B):
    numberOfPoints=A.shape[0] 

    if (numberOfPoints==0):
      print(bcolors.FAIL,"calculateAverageDistanceOf3DPoints(A,B), A has no points!",bcolors.ENDC)
      return np.inf
    elif (A.shape[0]!=B.shape[0]):
      print(bcolors.FAIL,"Error comparing point lists of different length",bcolors.ENDC)
      return np.inf
    
    distance=0.0
    for i in range(0,numberOfPoints):
       #---------
       xA=A[i][0]
       yA=A[i][1]
       zA=A[i][2]
       #---------
       xB=B[i][0]
       yB=B[i][1]
       zB=B[i][2]
       #----------------------------------------------
       distance += get3DDistance(xA,yA,zA,xB,yB,zB)
       #----------------------------------------------

    return distance/numberOfPoints


"""
Return the jointID of a jointName in a list of labels without being case sensitive
"""
def findJointID(jointName,labels):
  jointNameStreamlined=jointName.lower().strip()
  for i in range(0,len(labels)):
      labelStreamlined=labels[i].lower().strip()
      if (jointNameStreamlined==labelStreamlined):
           return i
  print(bcolors.FAIL,"Cannot find joint `%s` between %u labels !"%(jointNameStreamlined,len(labels)),bcolors.ENDC)
  #print(labels)
  return -1



def compareGroundTruthToPrediction(configuration,groundTruth,prediction,doProcrustes=1,allowProcrustesToChangeScale=1,jointsToCompare=list(),useSciKitImplementation=False):
    #print("GroundTruth : ",groundTruth)
    #print("Prediction  : ",prediction)
    
    #Automatically fill joints to compare if it is empty with whatever currently
    #exists in configuration
    numberOfJoints = len(configuration["hierarchy"])
    if (len(jointsToCompare)==0):
      for jID in range (0,numberOfJoints):
         jointsToCompare.append(configuration["hierarchy"][jID]["joint"].lower())
    else:
      numberOfJoints = len(jointsToCompare)
    #--------------------------------------------------------------------------

    #Initialize our variables 
    #-------------------------------
    comparedJoints          = list()
    groundTruth3DPoints     = list()
    mnet3DPoints            = list()
    #-------------------------------
    scale                   = 1.0
    outputScale             = 10.0 # We go from Centimeters to Millimeters!
    numberOfJointsToCompare = 0   
    #-------------------------------
 
    for jointName in jointsToCompare:
       jointName = jointName.lower() #Make double sure if supplied as argument

       labelX = '3DX_%s' % jointName
       labelY = '3DY_%s' % jointName
       labelZ = '3DZ_%s' % jointName

       if (  
            ( labelX in groundTruth ) and 
            ( labelY in groundTruth ) and 
            ( labelZ in groundTruth ) and 
            ( labelX in prediction )  and 
            ( labelY in prediction )  and 
            ( labelZ in prediction )
          ):
           comparedJoints.append(jointName)
           numberOfJointsToCompare = numberOfJointsToCompare + 1
           #--------------------------------------------
           #       Grab ground truth for point  
           #--------------------------------------------
           x3DGT=scale*groundTruth[labelX]
           y3DGT=scale*groundTruth[labelY]
           z3DGT=scale*groundTruth[labelZ]
           #--------------------------------------------
           groundTruth3DPoints.append([x3DGT,y3DGT,z3DGT])
           #--------------------------------------------

           #--------------------------------------------
           #            Grab MocapNET point  
           #--------------------------------------------
           x3DMNET=scale*prediction[labelX]
           y3DMNET=scale*prediction[labelY]
           z3DMNET=scale*prediction[labelZ]
           #--------------------------------------------
           mnet3DPoints.append([x3DMNET,y3DMNET,z3DMNET])
           #--------------------------------------------
       else:
           print(bcolors.WARNING,"Joint ",jointName," was not found this will influence results ")
 
    if (numberOfJointsToCompare==0):
       print(bcolors.FAIL,"No joints where found ..",bcolors.ENDC)

    #We package our lists in numpy to be able to easily manipulate them 
    #------------------------------------------------------------------ 
    np_GTPointCloud  =  np.asarray(groundTruth3DPoints,dtype=np.float32)
    np_OURPointCloud =  np.asarray(mnet3DPoints,dtype=np.float32)
    #------------------------------------------------------------------ 

    #This is the main comparison after using procrustes and transforming the pointcloud
    #to align it or when just doing plain old euclidean distance
    #--------------------------------------------------------------------------------
    if (useSciKitImplementation):
      from scipy.spatial import procrustes
      mtx1, mtx2, disparity = procrustes(np_GTPointCloud,np_OURPointCloud)
      np_GTPointCloud=mtx1
      np_OURPointCloud=mtx2
    elif (doProcrustes):
       d, Z, T, b, c = compute_similarity_transform(np_GTPointCloud,np_OURPointCloud,compute_optimal_scale=allowProcrustesToChangeScale)
       #disparity=np.sqrt(d)  #d: squared error after transformation
       #print("compute_similarity_transform : ",disparity)

       #Our point cloud is brought to the same translation and rotation as h36 point cloud
       np_OURPointCloud = (b*np_OURPointCloud.dot(T))+c
       disparity = outputScale * calculateAverageDistanceOf3DPoints(np_OURPointCloud,np_GTPointCloud)
    else:
       disparity = outputScale * calculateAverageDistanceOf3DPoints(np_OURPointCloud,np_GTPointCloud)
    #-------------------------------------------------------------------------------- 
    #Here for each element in ground truth we want to get the same point from prediction..

    #We want to calculate Mean Per Joint Position Error (MPJPE)
    #to do so we have to calculate the position error of each of the joints in our point cloud
    #sum it up and then divide it through the number of samples
    totalError = 0.0
    totalSamples = 0
    #alljointDistances = list()
    jointDistance = dict()  
    for jID in range(0,numberOfJointsToCompare):
    #------------------------------------------------------------------ 
      #We use the np_ourPointCloud and np_h36PointCloud so that if procrustes analysis is enabled it will be used..
      perJointDisparity= outputScale * get3DDistance(
                                                     np_OURPointCloud[jID][0],np_OURPointCloud[jID][1],np_OURPointCloud[jID][2],
                                                     np_GTPointCloud[jID][0] ,np_GTPointCloud[jID][1] ,np_GTPointCloud[jID][2]
                                                    )
      totalError+=perJointDisparity
      totalSamples+=1
      #We also keep every sample on a list to do an analysis in the end
      #alljointDistances.append(perJointDisparity)
      jointDistance[comparedJoints[jID]]=perJointDisparity
    #------------------------------------------------------------------ 
    
    jointDistance["meanAverageError"] = disparity
    #print("Frame %u / Disparity %f " % (frameID,disparity))
    return jointDistance



