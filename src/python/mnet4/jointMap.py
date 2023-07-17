#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

class JointDataMapper:
  #-----------------------------------------------------------------
  def __init__(self, csvFilename, configuration , jointLabels):
    self.filename = csvFilename
    self.jointLabelToJointIDMap = dict()
    self.numberOfExpectedLabels = len(jointLabels)
    jID = 0
    for jointDescription in jointLabels:
        self.jointLabelToJointIDMap[jointDescription.lower()]=jID
        jID = jID+1
  #-----------------------------------------------------------------
  def checkJointListDimensions(self,jointLabels):
    return self.numberOfExpectedLabels == len(jointLabels)
  #-----------------------------------------------------------------
  def getJointID_2DX(self,jointName):
    nname = "2dx_"+jointName.lower()  
    if (not nname in self.jointLabelToJointIDMap):
        print("getJointID_2DX: Error resolving ",nname)
        return -1 
    return self.jointLabelToJointIDMap[nname]
  #-----------------------------------------------------------------
  def getJointID_2DY(self,jointName):
    nname = "2dy_"+jointName.lower()  
    if (not nname in self.jointLabelToJointIDMap):
        print("getJointID_2DY: Error resolving ",nname)
        return -1 
    return self.jointLabelToJointIDMap[nname]
  #-----------------------------------------------------------------
  def getJointID_2DZ(self,jointName):
    nname = "2dz_"+jointName.lower()  
    if (not nname in self.jointLabelToJointIDMap):
        print("getJointID_2DZ: Error resolving ",nname)
        return -1 
    return self.jointLabelToJointIDMap[nname]
  #-----------------------------------------------------------------
  def getJointID_3DX(self,jointName):
    nname = "3dx_"+jointName.lower()
    if (not nname in self.jointLabelToJointIDMap):
        print("getJointID_3DX: Error resolving ",nname)
        return -1 
    return self.jointLabelToJointIDMap[nname]
  #-----------------------------------------------------------------
  def getJointID_3DY(self,jointName):
    nname = "3dy_"+jointName.lower()
    if (not nname in self.jointLabelToJointIDMap):
        print("getJointID_3DY: Error resolving ",nname)
        return -1 
    return self.jointLabelToJointIDMap[nname]
  #-----------------------------------------------------------------
  def getJointID_3DZ(self,jointName):
    nname = "3dz_"+jointName.lower()  
    if (not nname in self.jointLabelToJointIDMap):
        print("getJointID_3DZ: Error resolving ",nname)
        return -1 
    return self.jointLabelToJointIDMap[nname]
  #-----------------------------------------------------------------
  def getJointID_Visibility(self,jointName):
    nname = "visible_"+jointName.lower()  
    if (not nname in self.jointLabelToJointIDMap):
        print("getJointID_Visibility: Error resolving ",nname)
        return -1 
    return self.jointLabelToJointIDMap[nname]
  #-----------------------------------------------------------------
  def getJointID_Exists(self,jointName):
    nnameX = "2dx_"+jointName.lower()  
    nnameY = "2dy_"+jointName.lower()  
    if ((nnameX in self.jointLabelToJointIDMap) and (nnameY in self.jointLabelToJointIDMap)):
       return True
    return False
  #-----------------------------------------------------------------

if __name__ == '__main__':
    print("jointMap.py is a library it cannot be run standalone")
