#!/usr/bin/python3

import ctypes
import os
import sys
from ctypes import *
from os.path import exists
  
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


#This mimics the calibration files like ;
# https://github.com/AmmarkoV/RGBDAcquisition/blob/master/tools/Calibration/calibration.c
def readCalibrationFromFile(filename):
    calib = dict()
    if filename is None:
        return calib

    fp = None
    try:
        fp = open(filename, "r")
    except IOError:
        return calib

    # Our state
    # ----------------------------
    i = 0
    category = 0
    line_length = 0
    lines_at_current_category = 0
    # ----------------------------


    for line in fp:
        #--------------------------------------
        line = line.rstrip("\r\n")
        line_length = len(line)
        #--------------------------------------
        if line_length > 0:
            if line[line_length - 1] == '\n':
                line = line[:-1]
            if line[line_length - 1] == '\r':
                line = line[:-1]
        #--------------------------------------
        if line_length > 1:
            if line[line_length - 2] == '\n':
                line = line[:-2]
            if line[line_length - 2] == '\r':
                line = line[:-2]
        #--------------------------------------
        if line[0] == '%':
            lines_at_current_category = 0
        #--------------------------------------
        # ---------------------------- ---------------------------- ----------------------------  
        if line == "%I":
            category = 1
            calib["intrinsic"] = list()
        elif line == "%D":
            category = 2
        elif line == "%T":
            category = 3
            calib["extrinsicTranslation"] = list()
        elif line == "%R":
            category = 4
            calib["extrinsicRotationRodriguez"] = list()
        elif line == "%NF":
            category = 5
        elif line == "%UNIT":
            category = 6
        elif line == "%RT4*4":
            category = 7
            calib["extrinsic"] = list()
        elif line == "%Width":
            category = 8
        elif line == "%Height":
            category = 9
        else:
        # ---------------------------- ---------------------------- ----------------------------  
            if category == 1:
                calib["intrinsicParametersSet"] = 1
                lines_at_current_category = min(lines_at_current_category, 9)
                calib["intrinsic"].append(float(line))
                lines_at_current_category += 1
                if (lines_at_current_category==9):
                                                   category = 0
            elif category == 2:
                if lines_at_current_category == 0:
                    calib["k1"] = float(line)
                elif lines_at_current_category == 1:
                    calib["k2"] = float(line)
                elif lines_at_current_category == 2:
                    calib["p1"] = float(line)
                elif lines_at_current_category == 3:
                    calib["p2"] = float(line)
                elif lines_at_current_category == 4:
                    calib["k3"] = float(line)
                lines_at_current_category += 1
                if (lines_at_current_category==4):
                                                   category = 0
            elif category == 3:
                calib["extrinsicParametersSet"] = 1
                lines_at_current_category = min(lines_at_current_category, 3)
                calib["extrinsicTranslation"].append(float(line))
                lines_at_current_category += 1
                if (lines_at_current_category==3):
                                                   category = 0
            elif category == 4:
                lines_at_current_category = min(lines_at_current_category, 3)
                calib["extrinsicRotationRodriguez"].append(float(line))
                lines_at_current_category += 1
                if (lines_at_current_category==3):
                                                   category = 0
            elif category == 5:
                calib["nearPlane"] = float(line)
                category = 0
            elif category == 6:
                calib["farPlane"] = float(line)
                category = 0
            elif category == 7:
                lines_at_current_category = min(lines_at_current_category, 16)
                calib["extrinsic"].append(float(line))
                lines_at_current_category += 1
                category = 0
            elif category == 8:
                    calib["width"] = int(line)
                    category = 0
            elif category == 9:
                    calib["height"] = int(line)
                    category = 0
        # ---------------------------- ---------------------------- ----------------------------  

    fp.close()

    try:
        calib["fX"] = calib["intrinsic"][0]
        calib["fY"] = calib["intrinsic"][4]
        calib["cX"] = calib["intrinsic"][2]
        calib["cY"] = calib["intrinsic"][5] 
    except:
        print("No intrinsic matrix declared in ", filename)
        print("Cannot populate fX, fY, cX, cY")
   
  
    print("New calibration loaded : ",calib)

    return calib



def loadLibrary(filename,relativePath="",forceUpdate=False):
#--------------------------------------------------------
 if (relativePath!=""): 
     filename=relativePath+"/"+filename

 if (forceUpdate) or (not exists(filename)):
     print("Could not find BVH Library (",filename,"), compiling a fresh one..!")
     print("Current directory was (",os.getcwd(),") ")
     directory=os.path.dirname(os.path.abspath(filename))
     creationScript = directory+"/makeLibrary.sh"
     os.system(creationScript)
     #Magic JIT Just in time compilation, java has nothing on this :P 
 if not exists(filename):
     directory=os.path.dirname(os.path.abspath(filename))
     print("Could not make BVH Library, terminating")
     print("Directory we tried was : ",directory)
     sys.exit(0)
 libBVH = CDLL(filename)
 #call C function to check connection
 libBVH.connect() 
 libBVH.bvhConverter.restype  = c_int
 libBVH.bvhConverter.argtypes = c_int,POINTER(c_char_p)
 return libBVH
#--------------------------------------------------------


def splitDictionaryInLabelsAndFloats(arguments):
    #First prepare the labels of the joints we want to transmit
    #-------------------------------------------------- 
    labels = list(arguments.keys())
    labelsBytes = []
    for i in range(len(labels)):
        #Potential renaming..
        #---------------------------------------------
        #if ("endsite_" in labels[i]):
        #  if ("eye" in labels[i]):
        #   datasplit = labels[i].split("endsite_",1)
        #   newLabel="%s%s" % (datasplit[0],datasplit[1])  
        #   print(labels[i]," renamed to -> ",newLabel)
        #---------------------------------------------
        labelsBytes.append(bytes(labels[i], 'utf-8'))
    labelsCStr = (ctypes.c_char_p * len(labelsBytes))()
    labelsCStr[:] = labelsBytes 
    #-------------------------------------------------- 
    
    #Then prepare the array of floats we want to transmit
    #-------------------------------------------------- 
    values  = list(arguments.values())
    valuesF = list()
    for v in values:
       try:
         valuesF.append(float(v))
       except:
         print("Argument ",v,"cannot be casted to float..")
         valuesF.append(0.0)
    valuesArray    = (ctypes.c_float * len(valuesF))()
    valuesArray[:] = valuesF
    #--------------------------------------------------  

    argc=len(labelsBytes)

    return labelsCStr,valuesArray,argc
#--------------------------------------------------------


class BVH():
  def __init__(
               self,
               bvhPath:str,
               libraryPath:str = "./libBVHConverter.so",
               cameraCalibrationFile = "",
               forceLibUpdate=False
              ):
        print("Initializing BVH file ",bvhPath," from ",libraryPath)
        self.libBVH               = loadLibrary(libraryPath,forceUpdate = forceLibUpdate)
        self.numberOfJoints       = 0
        self.lastMAEErrorInPixels = 0.0
        self.traceStages          = False #If set to true each call will be emitted in stdout to speed-up debugging
        self.calib                = dict()
        #----------------------------------- 
        if (cameraCalibrationFile!=""): 
          if not exists(cameraCalibrationFile):
            print("Could not find renderer configuration file ",cameraCalibrationFile)
            raise FileNotFoundError
          self.configureRendererFromFile(cameraCalibrationFile)
        #----------------------------------- 
        if not exists(bvhPath):
            print("Could not find BVH file ",bvhPath)
            raise FileNotFoundError
        self.loadBVHFile(bvhPath)
  #--------------------------------------------------------
  def stage(self,message):
        if (self.traceStages):
            print(bcolors.WARNING,message,bcolors.ENDC)
  #--------------------------------------------------------
  def loadBVHFile(self,bvhPath):
        self.stage("loadBVHFile")
        # create byte objects from the strings
        arg1 = bvhPath.encode('utf-8')
        # send strings to c function
        self.libBVH.bvhConverter_loadAtomic.argtypes = [ctypes.c_char_p]
        self.libBVH.bvhConverter_loadAtomic.restype  = ctypes.c_int
        self.numberOfJoints = self.libBVH.bvhConverter_loadAtomic(arg1)
        if (self.numberOfJoints==0):
           print("Failed to load BVH file ",bvhPath)
        return self.numberOfJoints
  #--------------------------------------------------------
  def scale(self, scaleRatio:float):
        self.stage("scale")
        self.libBVH.bvhConverter_scale.argtypes = [ctypes.c_float]
        self.libBVH.bvhConverter_scale.restype  = ctypes.c_int
        return str(self.libBVH.bvhConverter_scale(scaleRatio));
  #--------------------------------------------------------
  def getJointName(self, jointID:int):
        self.stage("getJointName")
        self.libBVH.bvhConverter_getJointNameFromJointID.argtypes = [ctypes.c_int]
        self.libBVH.bvhConverter_getJointNameFromJointID.restype  = ctypes.c_char_p
        return str(self.libBVH.bvhConverter_getJointNameFromJointID(jointID).decode('UTF-8'));
  #--------------------------------------------------------
  def isJointEndSite(self, jointID:int):
        self.stage("isJointEndSite")
        self.libBVH.bvhConverter_isJointEndSite.argtypes = [ctypes.c_int]
        self.libBVH.bvhConverter_isJointEndSite.restype  = ctypes.c_int
        retval = self.libBVH.bvhConverter_isJointEndSite(jointID)
        return retval
  #--------------------------------------------------------
  def getJointParent(self, jointID:int):
        self.stage("getJointParent")
        self.libBVH.bvhConverter_getJointParent.argtypes = [ctypes.c_int]
        self.libBVH.bvhConverter_getJointParent.restype  = ctypes.c_int
        jointID = self.libBVH.bvhConverter_getJointParent(jointID)
        return jointID
  #--------------------------------------------------------
  def getJointParentList(self):
        self.stage("getJointParentList")
        jointList = list() 
        for jointID in range(0,self.numberOfJoints):
            jointList.append(int(self.getJointParent(jointID)))
        return jointList
  #--------------------------------------------------------
  def getMotionValueOfFrame(self, frameID:int, jointID:int):
        self.stage("getMotionValueOfFrame")
        self.libBVH.bvhConverter_getMotionValueOfFrame.argtypes = [ctypes.c_int,ctypes.c_int]
        self.libBVH.bvhConverter_getMotionValueOfFrame.restype  = ctypes.c_float
        value = self.libBVH.bvhConverter_getMotionValueOfFrame(frameID,jointID)
        return value
  #--------------------------------------------------------
  def getAllMotionValuesOfFrame(self, frameID:int):
        allMIDs=list()
        for mID in range(0,self.getNumberOfMotionValuesPerFrame()):
          allMIDs.append(self.getMotionValueOfFrame(frameID,mID))
        return allMIDs
  #--------------------------------------------------------
  def saveBVHFileFromList(self, filename:str, allMotionData:list):
        self.stage("saveBVHFileFromList")
        arg1 = filename.encode('utf-8') 
        #int bvhConverter_writeBVH(char * filename,int writeHierarchy,int writeMotion)
        self.libBVH.bvhConverter_writeBVH.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
        self.libBVH.bvhConverter_writeBVH.restype  = ctypes.c_int
        success = self.libBVH.bvhConverter_writeBVH(arg1,1,0) #Just write the Hierarchy part of the BVH file
        
        if (success):
         numberOfFrames = len(allMotionData)
         f = open(filename, 'a')
         f.write("MOTION\n");
         f.write("Frames: %u\n"%numberOfFrames);
         f.write("Frame Time: %0.8f\n"%(float(1/24)) );
         for fID in range(0,numberOfFrames):
          i=0
          for mID in allMotionData[fID]:
           if (i>0):
            f.write(' ')
           if (mID==0.0):
             f.write("0")
           else:
             f.write("%0.4f" % mID)
           i=i+1
          f.write('\n')  
         f.close()    

        #--------------------------------------------
        os.system("sed -i 's/rcollar/rCollar/g' out.bvh")
        os.system("sed -i 's/rshoulder/rShldr/g' out.bvh")
        os.system("sed -i 's/relbow/rForeArm/g' out.bvh")
        os.system("sed -i 's/rhand/rHand/g' out.bvh")
        #--------------------------------------------
        os.system("sed -i 's/lcollar/lCollar/g' out.bvh")
        os.system("sed -i 's/lshoulder/lShldr/g' out.bvh")
        os.system("sed -i 's/lelbow/lForeArm/g' out.bvh")
        os.system("sed -i 's/lhand/lHand/g' out.bvh")
        #--------------------------------------------
        os.system("sed -i 's/rhip/rThigh/g' out.bvh")
        os.system("sed -i 's/rknee/rShin/g' out.bvh")
        os.system("sed -i 's/rfoot/rFoot/g' out.bvh")
        #------------------------------------------------------ 
        os.system("sed -i 's/lhip/lThigh/g' out.bvh")
        os.system("sed -i 's/lknee/lShin/g' out.bvh")
        os.system("sed -i 's/lfoot/lFoot/g' out.bvh")


        return success
  #--------------------------------------------------------
  def setMotionValueOfFrame(self, frameID:int, jointID:int, value:float):
        self.stage("setMotionValueOfFrame")
        self.libBVH.bvhConverter_setMotionValueOfFrame.argtypes = [ctypes.c_int,ctypes.c_int,ctypes.c_float]
        self.libBVH.bvhConverter_setMotionValueOfFrame.restype  = ctypes.c_int
        success = self.libBVH.bvhConverter_setMotionValueOfFrame(frameID,jointID,value)
        return success
  #--------------------------------------------------------
  def getNumberOfMotionValuesPerFrame(self):
        self.stage("getNumberOfMotionValuesPerFrame")
        self.libBVH.bvhConverter_getNumberOfMotionValuesPerFrame.argtypes = []
        self.libBVH.bvhConverter_getNumberOfMotionValuesPerFrame.restype  = ctypes.c_int
        jointID = self.libBVH.bvhConverter_getNumberOfMotionValuesPerFrame()
        return jointID
  #--------------------------------------------------------
  def getNumberOfJoints(self):
        self.stage("getNumberOfJoints")
        self.libBVH.bvhConverter_getNumberOfJoints.argtypes = []
        self.libBVH.bvhConverter_getNumberOfJoints.restype  = ctypes.c_int
        jointID = self.libBVH.bvhConverter_getNumberOfJoints()
        return jointID
  #--------------------------------------------------------
  def getJointID(self, jointName:str):
        self.stage("getJointID")
        arg1 = jointName.encode('utf-8') 
        self.libBVH.bvhConverter_getJointNameJointID.argtypes = [ctypes.c_char_p]
        self.libBVH.bvhConverter_getJointNameJointID.restype  = ctypes.c_int
        jointID = self.libBVH.bvhConverter_getJointNameJointID(arg1)
        return jointID
  #--------------------------------------------------------
  def getJointList(self):
        self.stage("getJointList")
        jointList = list() 
        for jointID in range(0,self.numberOfJoints):
            jointList.append(self.getJointName(jointID))
        return jointList
  #--------------------------------------------------------
  def getJointRotationsForFrame(self, jointID:int, frameID:int):
        self.stage("getJointRotationsForFrame")
        if (self.isJointEndSite(jointID)==1):
          xRot=0.0
          yRot=0.0
          zRot=0.0  
        else: 
          #--------------------------------------------------------
          self.libBVH.bvhConverter_getBVHJointRotationXForFrame.argtypes = [ctypes.c_int, ctypes.c_int]
          self.libBVH.bvhConverter_getBVHJointRotationXForFrame.restype  = ctypes.c_float
          xRot = self.libBVH.bvhConverter_getBVHJointRotationXForFrame(frameID,jointID)
          #--------------------------------------------------------
          self.libBVH.bvhConverter_getBVHJointRotationYForFrame.argtypes = [ctypes.c_int, ctypes.c_int]
          self.libBVH.bvhConverter_getBVHJointRotationYForFrame.restype  = ctypes.c_float
          yRot = self.libBVH.bvhConverter_getBVHJointRotationYForFrame(frameID,jointID)
          #--------------------------------------------------------
          self.libBVH.bvhConverter_getBVHJointRotationZForFrame.argtypes = [ctypes.c_int, ctypes.c_int]
          self.libBVH.bvhConverter_getBVHJointRotationZForFrame.restype  = ctypes.c_float
          zRot = self.libBVH.bvhConverter_getBVHJointRotationZForFrame(frameID,jointID)
          #--------------------------------------------------------
        return xRot,yRot,zRot 
  #--------------------------------------------------------
  def getJoint3D(self, jointID:int):
        self.stage("getJoint3D")
        self.libBVH.bvhConverter_get3DX.argtypes = [ctypes.c_int]
        self.libBVH.bvhConverter_get3DX.restype  = ctypes.c_float
        x3D = self.libBVH.bvhConverter_get3DX(jointID)

        self.libBVH.bvhConverter_get3DY.argtypes = [ctypes.c_int]
        self.libBVH.bvhConverter_get3DY.restype  = ctypes.c_float
        y3D = self.libBVH.bvhConverter_get3DY(jointID)

        self.libBVH.bvhConverter_get3DZ.argtypes = [ctypes.c_int]
        self.libBVH.bvhConverter_get3DZ.restype  = ctypes.c_float
        z3D = self.libBVH.bvhConverter_get3DZ(jointID)

        return x3D,y3D,z3D 
  #--------------------------------------------------------
  def getJoint2D(self, jointID:int):
        self.stage("getJoint2D")
        self.libBVH.bvhConverter_get2DX.argtypes = [ctypes.c_int]
        self.libBVH.bvhConverter_get2DX.restype  = ctypes.c_float
        x2D = self.libBVH.bvhConverter_get2DX(jointID)

        self.libBVH.bvhConverter_get2DY.argtypes = [ctypes.c_int]
        self.libBVH.bvhConverter_get2DY.restype  = ctypes.c_float
        y2D = self.libBVH.bvhConverter_get2DY(jointID)

        #Flip X
        if (x2D!=0.0) or (y2D!=0.0):
           x2D = 1.0 - x2D

        return x2D,y2D 
  #--------------------------------------------------------
  def getJoint3DUsingJointName(self, jointName:str):
        return self.getJoint3D(self.getJointID(jointName)) 
  #--------------------------------------------------------
  def getJoint2DUsingJointName(self, jointName:str):
        return self.getJoint2D(self.getJointID(jointName)) 
  #--------------------------------------------------------
  def processFrame(self, frameID:int):
        self.stage("processFrame")
        self.libBVH.bvhConverter_processFrame.argtypes = [ctypes.c_int]
        self.libBVH.bvhConverter_processFrame.restype  = ctypes.c_int
        success = self.libBVH.bvhConverter_processFrame(frameID) 
        return success
  #--------------------------------------------------------
  def modify(self,arguments:dict,frameID=0):
    self.stage("modify")
    #print("BVH modify called with : ",arguments)
    if (not arguments):
        print("BVH modify called without arguments")
        return 0
    #Arguments is a dict with a lot of key/value pairs we want to transmit to the C code
    labelsCStr,valuesArray,argc = splitDictionaryInLabelsAndFloats(arguments)
    self.libBVH.bvhConverter_modifyAtomic.argtypes = [ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_float), ctypes.c_int, ctypes.c_int]
    success = self.libBVH.bvhConverter_modifyAtomic(labelsCStr,valuesArray,argc,frameID)
    return success
  #--------------------------------------------------------
  def configureRenderer(self,arguments:dict):
    #Arguments is a dict with a lot of key/value pairs we want to transmit to the C code
    labelsCStr,valuesArray,argc = splitDictionaryInLabelsAndFloats(arguments)
    self.libBVH.bvhConverter_rendererConfigurationAtomic.argtypes = [ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_float), ctypes.c_int]
    self.libBVH.bvhConverter_rendererConfigurationAtomic(labelsCStr,valuesArray,argc)
  #--------------------------------------------------------
  def configureRendererFromFile(self,cameraCalibrationFile:str):
    #from calibration import readCalibrationFromFile
    self.calib = readCalibrationFromFile(cameraCalibrationFile)
    if (self.calib):
               print("We found a calibration in file ",cameraCalibrationFile)
               print("calib : ",self.calib)
               self.configureRenderer(self.calib)
  #--------------------------------------------------------
  def get2DAnd3DAndBVHDictsForFrame(self,frameID=0):
    self.stage("get2DAnd3DAndBVHDictsForFrame ")
    #Arguments is a dict with a lot of key/value pairs we want to transmit to the C code
    self.processFrame(frameID=frameID)

    #Our output     
    #---------------
    data2D  = dict()
    data3D  = dict()
    dataBVH = dict() 
    #---------------

    for jointID in range(0,self.numberOfJoints):
    #-------------------------------------------------------
               #print("joint ID = ",jointID) 
               #-------------------------------------------
               jointName = self.getJointName(jointID).lower()
               #-------------------------------------------
               #print("Getting 3D")
               x3D,y3D,z3D = self.getJoint3D(jointID)
               data3D["3DX_"+jointName]=float(x3D)
               data3D["3DY_"+jointName]=float(y3D)
               data3D["3DZ_"+jointName]=float(z3D)
               #-------------------------------------------
               #print("Getting 2D")
               x2D,y2D = self.getJoint2D(jointID)
               data2D["2DX_"+jointName]=float(x2D)
               data2D["2DY_"+jointName]=float(y2D)
               #-------------------------------------------
               #print("Getting Joint Rotations")
               if (self.isJointEndSite(jointID)==0): #Do not try to recover rotations for EndSites (they dont have rotations)
                 xRot,yRot,zRot = self.getJointRotationsForFrame(jointID,frameID)
                 if (jointID==0):
                    dataBVH[jointName+"_Xposition"]=float(x3D)
                    dataBVH[jointName+"_Yposition"]=float(y3D)
                    dataBVH[jointName+"_Zposition"]=float(z3D)
                 dataBVH[jointName+"_Xrotation"]=float(xRot)
                 dataBVH[jointName+"_Yrotation"]=float(yRot)
                 dataBVH[jointName+"_Zrotation"]=float(zRot)
    #-------------------------------------------------------
    return data2D,data3D,dataBVH 
  #--------------------------------------------------------
  def fineTuneToMatch(self,bodyPart:str,target:dict,frameID=0,iterations=20,epochs=30,lr=0.01,fSampling=30.0,fCutoff=5.0,langevinDynamics=0.0):
    self.stage("fineTuneToMatch ")
    bodyPartCStr = bytes(bodyPart, 'utf-8')

    #Arguments is a dict with a lot of key/value pairs we want to transmit to the C code
    labelsCStr,valuesArray,argc = splitDictionaryInLabelsAndFloats(target)
    self.libBVH.bvhConverter_IKFineTune.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_float), ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_float, ctypes.c_float, ctypes.c_float,ctypes.c_float]
    self.libBVH.bvhConverter_IKFineTune.restype  = ctypes.c_float
    accuracy2D = self.libBVH.bvhConverter_IKFineTune(bodyPartCStr,labelsCStr,valuesArray,argc,frameID,iterations,epochs,lr,fSampling,fCutoff,langevinDynamics)
    print("HCD results for ",iterations," iterations ~> %0.2f pixels!" % accuracy2D)
    self.lastMAEErrorInPixels = accuracy2D 
    return self.get2DAnd3DAndBVHDictsForFrame(frameID=frameID)

   #return dict()  
  #--------------------------------------------------------

  def smooth(self,frameID=0,fSampling=30.0,fCutoff=5.0):
    self.stage("smooth ")
    #This call assumes that is called after subsequent(?) calls to fineTuneToMatch that have transmitted the BVH state..!
    self.libBVH.bvhConverter_smooth.argtypes = [ctypes.c_int, ctypes.c_float, ctypes.c_float]
    self.libBVH.bvhConverter_smooth.restype  = ctypes.c_int
    result = self.libBVH.bvhConverter_smooth(frameID,fSampling,fCutoff)
    return result==1

   #return dict()  
  #-------------------------------------------------------- 



if __name__== "__main__": 
   bvhFile = BVH(bvhPath="./headerWithHeadAndOneMotion.bvh",forceLibUpdate=True) 

   print("File has ",bvhFile.numberOfJoints," joints")

   print(" Joint List : ",bvhFile.getJointList())
   print(" Joint Parent List : ",bvhFile.getJointParentList())

   modifications = dict()
   modifications["hip_Xposition"]=100.0
   modifications["hip_Yposition"]=100.0
   modifications["hip_Zposition"]=-400.0
   modifications["hip_Xrotation"]=1.0
   modifications["hip_Yrotation"]=2.0
   modifications["hip_Zrotation"]=4.0
   bvhFile.modify(modifications)
   jointName = "neck"
   print("Joint ID for ",jointName," is ",bvhFile.getJointID(jointName))

   frameID=0

   for i in range(0,10):
     modifications["hip_Xposition"]=100.0 + i * 10.0
     bvhFile.modify(modifications)
     bvhFile.processFrame(frameID)
     x3D,y3D,z3D = bvhFile.getJoint3DUsingJointName(jointName)
     print(" I=",i," Joint=",jointName," 3D values for frame ",frameID," are ",x3D,",",y3D,",",z3D," ")

   x2D,y2D = bvhFile.getJoint2DUsingJointName(jointName)
   print(" Joint ",jointName," 2D values for frame ",frameID," are ",x2D,",",y2D)

   target2D = dict()
   target2D["2dx_head"]=0.4722689390182495
   target2D["2dy_head"]=0.1971915066242218
   target2D["visible_head"]=0.9999899864196777
   target2D["2dx_head_leye_0"]=0.46880775690078735
   target2D["2dy_head_leye_0"]=0.1777663230895996
   target2D["visible_head_leye_0"]=0.0
   target2D["2dx_endsite_eye.l"]=0.4609638452529907
   target2D["2dy_endsite_eye.l"]=0.18265053629875183
   target2D["visible_endsite_eye.l"]=0.9999715089797974
   target2D["2dx_head_leye_3"]=0.4599621295928955
   target2D["2dy_head_leye_3"]=0.17712406814098358
   target2D["visible_head_leye_3"]=0.0
   target2D["2dx_head_reye_3"]=0.4780046343803406
   target2D["2dy_head_reye_3"]=0.17705687880516052
   target2D["visible_head_reye_3"]=0.0
   target2D["2dx_endsite_eye.r"]=0.47662675380706787
   target2D["2dy_endsite_eye.r"]=0.17989128828048706
   target2D["visible_endsite_eye.r"]=0.9999359846115112
   target2D["2dx_head_reye_0"]=0.4844837188720703
   target2D["2dy_head_reye_0"]=0.17520418763160706
   target2D["visible_head_reye_0"]=0.0
   target2D["2dx_lear"]=0.4520418643951416
   target2D["2dy_lear"]=0.1892796754837036
   target2D["visible_lear"]=0.9999462366104126
   target2D["2dx_rear"]=0.47904515266418457
   target2D["2dy_rear"]=0.18252810835838318
   target2D["visible_rear"]=0.999884843826294
   target2D["2dx_head_outmouth_0"]=0.4792625308036804
   target2D["2dy_head_outmouth_0"]=0.20948739349842072
   target2D["visible_head_outmouth_0"]=0.0
   target2D["2dx_head_outmouth_6"]=0.4670618176460266
   target2D["2dy_head_outmouth_6"]=0.2107415348291397
   target2D["visible_head_outmouth_6"]=0.0
   target2D["2dx_lshoulder"]=0.43696290254592896
   target2D["2dy_lshoulder"]=0.2820419669151306
   target2D["visible_lshoulder"]=0.9999864101409912
   target2D["2dx_rshoulder"]=0.5070507228374481
   target2D["2dy_rshoulder"]=0.2563856244087219
   target2D["visible_rshoulder"]=0.9998794794082642
   target2D["2dx_lelbow"]=0.4353405833244324
   target2D["2dy_lelbow"]=0.3725816607475281
   target2D["visible_lelbow"]=0.9570146799087524
   target2D["2dx_relbow"]=0.5561909377574921
   target2D["2dy_relbow"]=0.323696106672287
   target2D["visible_relbow"]=0.974626362323761
   target2D["2dx_lhand"]=0.41237425804138184
   target2D["2dy_lhand"]=0.4415815770626068
   target2D["visible_lhand"]=0.9751281142234802
   target2D["2dx_rhand"]=0.6100260317325592
   target2D["2dy_rhand"]=0.36322692036628723
   target2D["visible_rhand"]=0.9656738638877869
   target2D["2dx_left_hand_pinky_4"]=0.40250515937805176
   target2D["2dy_left_hand_pinky_4"]=0.468822181224823
   target2D["visible_left_hand_pinky_4"]=0.9458165764808655
   target2D["2dx_right_hand_pinky_4"]=0.6301354765892029
   target2D["2dy_right_hand_pinky_4"]=0.36382848024368286
   target2D["visible_right_hand_pinky_4"]=0.9336408376693726
   target2D["2dx_left_hand_index_4"]=0.4018949866294861
   target2D["2dy_left_hand_index_4"]=0.46844327449798584
   target2D["visible_left_hand_index_4"]=0.9523322582244873
   target2D["2dx_right_hand_index_4"]=0.6305072009563446
   target2D["2dy_right_hand_index_4"]=0.361262708902359
   target2D["visible_right_hand_index_4"]=0.9438664317131042
   target2D["2dx_left_hand_thumb_4"]=0.40644168853759766
   target2D["2dy_left_hand_thumb_4"]=0.4600275158882141
   target2D["visible_left_hand_thumb_4"]=0.9461172819137573
   target2D["2dx_right_hand_thumb_4"]=0.6229645609855652
   target2D["2dy_right_hand_thumb_4"]=0.3644818663597107
   target2D["visible_right_hand_thumb_4"]=0.9423035979270935
   target2D["2dx_lhip"]=0.4714365005493164
   target2D["2dy_lhip"]=0.49608662724494934
   target2D["visible_lhip"]=0.9997338652610779
   target2D["2dx_rhip"]=0.5158871114253998
   target2D["2dy_rhip"]=0.48727133870124817
   target2D["visible_rhip"]=0.9996464252471924
   target2D["2dx_lknee"]=0.46627652645111084
   target2D["2dy_lknee"]=0.6712287068367004
   target2D["visible_lknee"]=0.9964113831520081
   target2D["2dx_rknee"]=0.5255001187324524
   target2D["2dy_rknee"]=0.6690568923950195
   target2D["visible_rknee"]=0.997963547706604
   target2D["2dx_lfoot"]=0.45652568340301514
   target2D["2dy_lfoot"]=0.799410879611969
   target2D["visible_lfoot"]=0.9943563342094421
   target2D["2dx_rfoot"]=0.5572476387023926
   target2D["2dy_rfoot"]=0.8126811981201172
   target2D["visible_rfoot"]=0.9981330037117004
   target2D["2dx_lheel"]=0.46518951654434204
   target2D["2dy_lheel"]=0.8143126964569092
   target2D["visible_lheel"]=0.9416220784187317
   target2D["2dx_rheel"]=0.5635095536708832
   target2D["2dy_rheel"]=0.8311944007873535
   target2D["visible_rheel"]=0.9331190586090088
   target2D["2dx_endsite_toe1-2.l"]=0.44505226612091064
   target2D["2dy_endsite_toe1-2.l"]=0.8585301637649536
   target2D["visible_endsite_toe1-2.l"]=0.9915184378623962
   target2D["2dx_endsite_toe1-2.r"]=0.5560439825057983
   target2D["2dy_endsite_toe1-2.r"]=0.8789670467376709
   target2D["visible_endsite_toe1-2.r"]=0.9957401752471924
   target2D["2dx_head_outmouth_3"]=0.4757194519042969
   target2D["2dy_head_outmouth_3"]=0.2063373625278473
   target2D["visible_head_outmouth_3"]=0.0
   target2D["2dx_head_nosebone_3"]=0.47762298583984375
   target2D["2dy_head_nosebone_3"]=0.19625969231128693
   target2D["visible_head_nosebone_3"]=0.0
   target2D["2dx_head_nostrills_2"]=0.4758298993110657
   target2D["2dy_head_nostrills_2"]=0.19925406575202942
   target2D["visible_head_nostrills_2"]=0.0
   target2D["2dx_head_nosebone_2"]=0.47625207901000977
   target2D["2dy_head_nosebone_2"]=0.1822354793548584
   target2D["visible_head_nosebone_2"]=0.0
   target2D["2dx_head_nosebone_1"]=0.47568726539611816
   target2D["2dy_head_nosebone_1"]=0.17861950397491455
   target2D["visible_head_nosebone_1"]=0.0
   target2D["2dx_head_inmouth_2"]=0.47532206773757935
   target2D["2dy_head_inmouth_2"]=0.20903927087783813
   target2D["visible_head_inmouth_2"]=0.0
   target2D["2dx_head_inmouth_6"]=0.4751202464103699
   target2D["2dy_head_inmouth_6"]=0.21265165507793427
   target2D["visible_head_inmouth_6"]=0.0
   target2D["2dx_head_outmouth_9"]=0.47498154640197754
   target2D["2dy_head_outmouth_9"]=0.21609455347061157
   target2D["visible_head_outmouth_9"]=0.0
   target2D["2dx_head_outmouth_2"]=0.48239976167678833
   target2D["2dy_head_outmouth_2"]=0.17039550840854645
   target2D["visible_head_outmouth_2"]=0.0
   target2D["2dx_head_outmouth_1"]=0.47901231050491333
   target2D["2dy_head_outmouth_1"]=0.2075507491827011
   target2D["visible_head_outmouth_1"]=0.0
   target2D["2dx_head_inmouth_1"]=0.47740042209625244
   target2D["2dy_head_inmouth_1"]=0.20893418788909912
   target2D["visible_head_inmouth_1"]=0.0
   target2D["2dx_head_reyebrow_4"]=0.4785383939743042
   target2D["2dy_head_reyebrow_4"]=0.1684335619211197
   target2D["visible_head_reyebrow_4"]=0.0
   target2D["2dx_head_reyebrow_1"]=0.4863806962966919
   target2D["2dy_head_reyebrow_1"]=0.16322830319404602
   target2D["visible_head_reyebrow_1"]=0.0
   target2D["2dx_head_reyebrow_3"]=0.48255395889282227
   target2D["2dy_head_reyebrow_3"]=0.16279345750808716
   target2D["visible_head_reyebrow_3"]=0.0
   target2D["2dx_head_reyebrow_0"]=0.4868544340133667
   target2D["2dy_head_reyebrow_0"]=0.16618424654006958
   target2D["visible_head_reyebrow_0"]=0.0
   target2D["2dx_head_inmouth_0"]=0.4783351421356201
   target2D["2dy_head_inmouth_0"]=0.20919831097126007
   target2D["visible_head_inmouth_0"]=0.0
   target2D["2dx_head_outmouth_10"]=0.47662580013275146
   target2D["2dy_head_outmouth_10"]=0.2154039442539215
   target2D["visible_head_outmouth_10"]=0.0
   target2D["2dx_head_outmouth_11"]=0.47859442234039307
   target2D["2dy_head_outmouth_11"]=0.2121586948633194
   target2D["visible_head_outmouth_11"]=0.0
   target2D["2dx_head_nostrills_1"]=0.4774726629257202
   target2D["2dy_head_nostrills_1"]=0.19906578958034515
   target2D["visible_head_nostrills_1"]=0.0
   target2D["2dx_head_nostrills_0"]=0.47901052236557007
   target2D["2dy_head_nostrills_0"]=0.19807150959968567
   target2D["visible_head_nostrills_0"]=0.0
   target2D["2dx_head_reyebrow_2"]=0.4849526286125183
   target2D["2dy_head_reyebrow_2"]=0.1620578020811081
   target2D["visible_head_reyebrow_2"]=0.0
   target2D["2dx_head_rchin_1"]=0.4867928624153137
   target2D["2dy_head_rchin_1"]=0.1828579306602478
   target2D["visible_head_rchin_1"]=0.0
   target2D["2dx_head_rchin_0"]=0.4863375425338745
   target2D["2dy_head_rchin_0"]=0.17663004994392395
   target2D["visible_head_rchin_0"]=0.0
   target2D["2dx_head_reye_4"]=0.4802663326263428
   target2D["2dy_head_reye_4"]=0.1782480776309967
   target2D["visible_head_reye_4"]=0.0
   target2D["2dx_head_rchin_2"]=0.4862361550331116
   target2D["2dy_head_rchin_2"]=0.195520281791687
   target2D["visible_head_rchin_2"]=0.0
   target2D["2dx_head_rchin_7"]=0.4762313961982727
   target2D["2dy_head_rchin_7"]=0.22934123873710632
   target2D["visible_head_rchin_7"]=0.0
   target2D["2dx_head_chin"]=0.473285973072052
   target2D["2dy_head_chin"]=0.2309272140264511
   target2D["visible_head_chin"]=0.0
   target2D["2dx_head_reye_2"]=0.4805048704147339
   target2D["2dy_head_reye_2"]=0.1739354431629181
   target2D["visible_head_reye_2"]=0.0
   target2D["2dx_head_reye_1"]=0.483254075050354
   target2D["2dy_head_reye_1"]=0.17367364466190338
   target2D["visible_head_reye_1"]=0.0
   target2D["2dx_head_nosebone_0"]=0.47514963150024414
   target2D["2dy_head_nosebone_0"]=0.1740616112947464
   target2D["visible_head_nosebone_0"]=0.0
   target2D["2dx_head_rchin_5"]=0.47995781898498535
   target2D["2dy_head_rchin_5"]=0.2206612378358841
   target2D["visible_head_rchin_5"]=0.0
   target2D["2dx_head_rchin_6"]=0.478015661239624
   target2D["2dy_head_rchin_6"]=0.22674590349197388
   target2D["visible_head_rchin_6"]=0.0
   target2D["2dx_head_inmouth_7"]=0.4774059057235718
   target2D["2dy_head_inmouth_7"]=0.21126437187194824
   target2D["visible_head_inmouth_7"]=0.0
   target2D["2dx_head_rchin_3"]=0.48438382148742676
   target2D["2dy_head_rchin_3"]=0.20477566123008728
   target2D["visible_head_rchin_3"]=0.0
   target2D["2dx_head_rchin_4"]=0.4815486669540405
   target2D["2dy_head_rchin_4"]=0.21430769562721252
   target2D["visible_head_rchin_4"]=0.0
   target2D["2dx_head_leye_4"]=0.4646076560020447
   target2D["2dy_head_leye_4"]=0.18126901984214783
   target2D["visible_head_leye_4"]=0.0
   target2D["2dx_head_leye_5"]=0.4679994583129883
   target2D["2dy_head_leye_5"]=0.17986340820789337
   target2D["visible_head_leye_5"]=0.0
   target2D["2dx_head_lchin_0"]=0.45561397075653076
   target2D["2dy_head_lchin_0"]=0.17862515151500702
   target2D["visible_head_lchin_0"]=0.0
   target2D["2dx_head_outmouth_4"]=0.47371816635131836
   target2D["2dy_head_outmouth_4"]=0.2061852067708969
   target2D["visible_head_outmouth_4"]=0.0
   target2D["2dx_head_inmouth_3"]=0.4718068242073059
   target2D["2dy_head_inmouth_3"]=0.20968028903007507
   target2D["visible_head_inmouth_3"]=0.0
   target2D["2dx_head_leyebrow_0"]=0.45764458179473877
   target2D["2dy_head_leyebrow_0"]=0.16966667771339417
   target2D["visible_head_leyebrow_0"]=0.0
   target2D["2dx_head_leyebrow_4"]=0.47118234634399414
   target2D["2dy_head_leyebrow_4"]=0.16888980567455292
   target2D["visible_head_leyebrow_4"]=0.0
   target2D["2dx_head_leyebrow_1"]=0.4586902856826782
   target2D["2dy_head_leyebrow_1"]=0.16491857171058655
   target2D["visible_head_leyebrow_1"]=0.0
   target2D["2dx_head_leyebrow_3"]=0.466333270072937
   target2D["2dy_head_leyebrow_3"]=0.16365274786949158
   target2D["visible_head_leyebrow_3"]=0.0
   target2D["2dx_head_inmouth_4"]=0.46794217824935913
   target2D["2dy_head_inmouth_4"]=0.21027947962284088
   target2D["visible_head_inmouth_4"]=0.0
   target2D["2dx_head_outmouth_8"]=0.4729023575782776
   target2D["2dy_head_outmouth_8"]=0.21605923771858215
   target2D["visible_head_outmouth_8"]=0.0
   target2D["2dx_head_nostrills_3"]=0.4738311171531677
   target2D["2dy_head_nostrills_3"]=0.19950126111507416
   target2D["visible_head_nostrills_3"]=0.0
   target2D["2dx_head_nostrills_4"]=0.47070103883743286
   target2D["2dy_head_nostrills_4"]=0.19887711107730865
   target2D["visible_head_nostrills_4"]=0.0
   target2D["2dx_head_leyebrow_2"]=0.46217411756515503
   target2D["2dy_head_leyebrow_2"]=0.16334131360054016
   target2D["visible_head_leyebrow_2"]=0.0
   target2D["2dx_head_lchin_1"]=0.4578735828399658
   target2D["2dy_head_lchin_1"]=0.18603739142417908
   target2D["visible_head_lchin_1"]=0.0
   target2D["2dx_head_lchin_2"]=0.45422083139419556
   target2D["2dy_head_lchin_2"]=0.19913220405578613
   target2D["visible_head_lchin_2"]=0.0
   target2D["2dx_head_lchin_7"]=0.4697527289390564
   target2D["2dy_head_lchin_7"]=0.2310400754213333
   target2D["visible_head_lchin_7"]=0.0
   target2D["2dx_head_leye_1"]=0.46723294258117676
   target2D["2dy_head_leye_1"]=0.1757526397705078
   target2D["visible_head_leye_1"]=0.0
   target2D["2dx_head_leye_2"]=0.4639958143234253
   target2D["2dy_head_leye_2"]=0.1749143898487091
   target2D["visible_head_leye_2"]=0.0
   target2D["2dx_head_lchin_5"]=0.4636954665184021
   target2D["2dy_head_lchin_5"]=0.22440548241138458
   target2D["visible_head_lchin_5"]=0.0
   target2D["2dx_head_lchin_6"]=0.46653610467910767
   target2D["2dy_head_lchin_6"]=0.22966337203979492
   target2D["visible_head_lchin_6"]=0.0
   target2D["2dx_head_inmouth_5"]=0.4714820384979248
   target2D["2dy_head_inmouth_5"]=0.21212585270404816
   target2D["visible_head_inmouth_5"]=0.0
   target2D["2dx_head_outmouth_7"]=0.4708884358406067
   target2D["2dy_head_outmouth_7"]=0.21508848667144775
   target2D["visible_head_outmouth_7"]=0.0
   target2D["2dx_head_lchin_3"]=0.4568250775337219
   target2D["2dy_head_lchin_3"]=0.20863905549049377
   target2D["visible_head_lchin_3"]=0.0
   target2D["2dx_head_lchin_4"]=0.4621039032936096
   target2D["2dy_head_lchin_4"]=0.217911496758461
   target2D["visible_head_lchin_4"]=0.0
   target2D["2dx_neck"]=0.47200681269168854
   target2D["2dy_neck"]=0.26921379566192627
   target2D["visible_neck"]=0.9999329447746277
   target2D["2dx_hip"]=0.4936618059873581
   target2D["2dy_hip"]=0.49167898297309875
   target2D["visible_hip"]=0.9996901452541351

   print("fineTuneToMatch")
   result = bvhFile.fineTuneToMatch("body",target2D,frameID=0,iterations=10,epochs=30)
   #print("Result ",result)

