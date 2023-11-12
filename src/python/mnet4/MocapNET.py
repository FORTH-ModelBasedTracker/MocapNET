#!/usr/bin/python3
#test
""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

#-------------------------------------------------------------------------------------------
from readCSV  import parseConfiguration,parseConfigurationInputJointMap,transformNetworkInput,initializeDecompositionForExecutionEngine,readGroundTruthFile,readCSVFile,parseOutputNormalization
from NSDM     import NSDMLabels,createNSDMUsingRules,inputIsEnoughToCreateNSDM,performNSRMAlignment
from EDM      import EDMLabels,createEDMUsingRules
from tools    import bcolors,checkIfFileExists,readListFromFile,convertListToLowerCase,secondsToHz,getEntryIndexInList,eprint
#-------------------------------------------------------------------------------------------
from BVH.bvhConverter import BVH
#-------------------------------------------------------------------------------------------
from principleComponentAnalysis import PCA
#-------------------------------------------------------------------------------------------

MOCAPNET_VERSION="4.0"

#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
import time
import os
import numpy as np
#-------------------------------------------------------------------------------------------
class MocapNETEnsembleCombination():
  def __init__(self):
      self.ensembleNameList = list()
      self.ensemblePathList = list()
  def addEnsemble(self,name:str,path:str):
      self.ensembleNameList.append(name)
      self.ensemblePathList.append(path)
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
def checkIfAllListedElementsExistInDict(theList,theDict):
  for element in theList:
     if not element in theDict:
           return False
  return True
#-------------------------------------------------------------------------------------------
def checkIfAnyListedElementsExistsInString(theList,theString):
  #--------------------------
  if (len(theList)==0):
     return False
  #--------------------------
  for element in theList:
     if element in theString:
           return True
  return False
#-------------------------------------------------------------------------------------------
def flipHorizontalInput(inputList):
  for k in inputList.keys():
      if ("2dx_" in k):
           inputList[k]=1.0-inputList[k]
  return inputList
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
def getSymmetricLEyeOutputs():
 #I AM NOT AT ALL SURE THE FOLLOWING ARE CORRECT..
 bn=dict()
 #---------------------------------------------------
 #These are the actual useful outputs.. that REye has..
 bn["hip_xposition"]               =  (0.0,"")     #0 #ignored
 bn["hip_yposition"]               =  (0.0,"")     #1 #ignored
 bn["hip_zposition"]               =  (0.0,"")     #2 #ignored
 bn["neck1_zrotation"]              =  (0.0,"")     #3 #ignored
 bn["neck1_xrotation"]              =  (0.0,"")     #4 #ignored
 bn["neck1_yrotation"]              =  (0.0,"")     #5 #ignored
 bn["eye.r_zrotation"]             =  (1.0,"eye.l_zrotation")               #
 bn["eye.r_xrotation"]             =  (1.0,"eye.l_xrotation")               #
 bn["oculi01.r_zrotation"]         =  (1.0,"oculi01.l_zrotation")           #
 bn["orbicularis03.r_xrotation"]   =  (1.0,"orbicularis03.l_xrotation")     #
 bn["jaw_xrotation"]   =  (0.0,"") #
 bn["jaw_yrotation"]   =  (0.0,"") #
 #---------------------------------------------------
 #The rest should all be ignored ?
 #---------------------------------------------------
 bn["oculi01.l_zrotation"]         =  (0.0,"")           #ignored
 bn["eye.l_zrotation"]             =  (0.0,"")               # ignored
 bn["eye.l_xrotation"]             =  (0.0,"")               # ignored
 bn["orbicularis04.r_xrotation"]   =  (0.0,"")     #ignored
 bn["orbicularis03.r_yrotation"]   =  (0.0,"")     #ignored
 bn["orbicularis04.r_yrotation"]   =  (0.0,"")     #ignored

 bn["levator06.l_xrotation"]   =  (0.0,"")     #ignored
 bn["levator06.r_xrotation"]   =  (0.0,"")     #ignored
 bn["levator03.l_zrotation"]   =  (0.0,"")     #ignored
 bn["levator03.r_zrotation"]   =  (0.0,"")     #ignored

 bn["oris03.l_zrotation"]   =  (0.0,"")     #ignored
 bn["oris03.r_zrotation"]   =  (0.0,"")     #ignored
 bn["oris07.l_zrotation"]   =  (0.0,"")     #ignored
 bn["oris07.r_zrotation"]   =  (0.0,"")     #ignored

 bn["oris04.l_zrotation"]   =  (0.0,"")     #ignored
 bn["oris04.r_zrotation"]   =  (0.0,"")     #ignored
 bn["oris06.l_zrotation"]   =  (0.0,"")     #ignored
 bn["oris06.r_zrotation"]   =  (0.0,"")     #ignored

 bn["orbicularis03.r_yrotation"]   =  (0.0,"")     #ignored
 bn["orbicularis04.r_yrotation"]   =  (0.0,"")     #ignored
 bn["orbicularis03.l_yrotation"]   =  (0.0,"")     #ignored
 bn["orbicularis04.l_yrotation"]   =  (0.0,"")     #ignored

 bn["orbicularis03.l_xrotation"]   =  (0.0,"")     # ignored
 bn["orbicularis04.l_xrotation"]   =  (0.0,"")     # ignored

 bn["levator06.l_yrotation"]   =  (0.0,"")     #ignored
 bn["levator06.r_yrotation"]   =  (0.0,"")     #ignored

 bn["oris03.l_xrotation"]   =  (0.0,"")     #ignored
 bn["oris03.l_yrotation"]   =  (0.0,"")     #ignored
 bn["oris07.l_yrotation"]   =  (0.0,"")     #ignored
 bn["oris03.r_xrotation"]   =  (0.0,"")     #ignored
 bn["oris03.r_yrotation"]   =  (0.0,"")     #ignored
 bn["oris07.r_yrotation"]   =  (0.0,"")     #ignored

 bn["oris05_xrotation"]   =  (0.0,"")     #ignored
 bn["oris05_yrotation"]   =  (0.0,"")     #ignored
 return bn
#---------------------------------------------------
def getSymmetricLEyeNameList():
 bn=dict()
 #---------------------------------------------------
 bn["head_reye_0"]     = "head_leye_3"      #0
 bn["head_reye_1"]     = "head_leye_2"      #1
 bn["head_reye_2"]     = "head_leye_1"      #2
 bn["head_reye_3"]     = "head_leye_0"      #3
 bn["head_reye_4"]     = "head_leye_5"      #4
 bn["head_reye_5"]     = "head_leye_4"      #5
 bn["head_reyebrow_0"] = "head_leyebrow_0"  #6
 bn["head_reyebrow_1"] = "head_leyebrow_1"  #7
 bn["head_reyebrow_2"] = "head_leyebrow_2"  #8
 bn["head_reyebrow_3"] = "head_leyebrow_3"  #9
 bn["head_reyebrow_4"] = "head_leyebrow_4"  #10
 bn["head_reye"]       = "head_leye"        #11
 bn["head_rchin_0"]    = "head_lchin_0"     #12
 bn["head_nostrills_2"]= "head_nostrills_2" #13
 bn["head_chin"]       = "head_chin"        #14
 return bn
#---------------------------------------------------
#---------------------------------------------------
#---------------------------------------------------


def getSymmetricLHandOutputs():
 bn=dict()
 #---------------------------------------------------
 bn["lhand_xposition"]         =  (-1.0,"rhand_xposition")          #0
 bn["lhand_yposition"]         =  (1.0,"rhand_yposition")           #1
 bn["lhand_zposition"]         =  (1.0,"rhand_zposition")           #2
 #--------------------------------------------------------------------
 #Flip Quaternion During Symmetric output Calculations
 bn["lhand_wrotation"]         =  (-1.0,"rhand_wrotation")          #3  {-w,z,y,x}
 bn["lhand_xrotation"]         =  ( 1.0,"rhand_zrotation")          #4
 bn["lhand_yrotation"]         =  ( 1.0,"rhand_yrotation")          #5
 bn["lhand_zrotation"]         =  ( 1.0,"rhand_xrotation")          #6
 #--------------------------------------------------------------------
 bn["finger2-1.l_zrotation"]   =  (-1.0,"finger2-1.r_zrotation")    #7
 bn["finger2-1.l_xrotation"]   =  (-1.0,"finger2-1.r_xrotation")    #8
 bn["finger2-1.l_yrotation"]   =  (-1.0,"finger2-1.r_yrotation")    #9
 bn["finger2-2.l_zrotation"]   =  (-1.0,"finger2-2.r_zrotation")    #10
 bn["finger2-2.l_xrotation"]   =  (-1.0,"finger2-2.r_xrotation")    #11
 bn["finger2-2.l_yrotation"]   =  (-1.0,"finger2-2.r_yrotation")    #12
 bn["finger2-3.l_zrotation"]   =  (-1.0,"finger2-3.r_zrotation")    #13
 bn["finger2-3.l_xrotation"]   =  (-1.0,"finger2-3.r_xrotation")    #14
 bn["finger2-3.l_yrotation"]   =  (-1.0,"finger2-3.r_yrotation")    #15
 #--------------------------------------------------------------------
 bn["finger3-1.l_zrotation"]   =  (-1.0,"finger3-1.r_zrotation")    #16
 bn["finger3-1.l_xrotation"]   =  (-1.0,"finger3-1.r_xrotation")    #17
 bn["finger3-1.l_yrotation"]   =  (-1.0,"finger3-1.r_yrotation")    #18
 bn["finger3-2.l_zrotation"]   =  (-1.0,"finger3-2.r_zrotation")    #19
 bn["finger3-2.l_xrotation"]   =  (-1.0,"finger3-2.r_xrotation")    #20
 bn["finger3-2.l_yrotation"]   =  (-1.0,"finger3-2.r_yrotation")    #21
 bn["finger3-3.l_zrotation"]   =  (-1.0,"finger3-3.r_zrotation")    #22
 bn["finger3-3.l_xrotation"]   =  (-1.0,"finger3-3.r_xrotation")    #23
 bn["finger3-3.l_yrotation"]   =  (-1.0,"finger3-3.r_yrotation")    #24
 #--------------------------------------------------------------------
 bn["finger4-1.l_zrotation"]   =  (-1.0,"finger4-1.r_zrotation")    #25
 bn["finger4-1.l_xrotation"]   =  (-1.0,"finger4-1.r_xrotation")    #26
 bn["finger4-1.l_yrotation"]   =  (-1.0,"finger4-1.r_yrotation")    #27
 bn["finger4-2.l_zrotation"]   =  (-1.0,"finger4-2.r_zrotation")    #28
 bn["finger4-2.l_xrotation"]   =  (-1.0,"finger4-2.r_xrotation")    #29
 bn["finger4-2.l_yrotation"]   =  (-1.0,"finger4-2.r_yrotation")    #30
 bn["finger4-3.l_zrotation"]   =  (-1.0,"finger4-3.r_zrotation")    #31
 bn["finger4-3.l_xrotation"]   =  (-1.0,"finger4-3.r_xrotation")    #32
 bn["finger4-3.l_yrotation"]   =  (-1.0,"finger4-3.r_yrotation")    #33
 #--------------------------------------------------------------------
 bn["finger5-1.l_zrotation"]   =  (-1.0,"finger5-1.r_zrotation")    #34
 bn["finger5-1.l_xrotation"]   =  (-1.0,"finger5-1.r_xrotation")    #35
 bn["finger5-1.l_yrotation"]   =  (-1.0,"finger5-1.r_yrotation")    #36
 bn["finger5-2.l_zrotation"]   =  (-1.0,"finger5-2.r_zrotation")    #37
 bn["finger5-2.l_xrotation"]   =  (-1.0,"finger5-2.r_xrotation")    #38
 bn["finger5-2.l_yrotation"]   =  (-1.0,"finger5-2.r_yrotation")    #39
 bn["finger5-3.l_zrotation"]   =  (-1.0,"finger5-3.r_zrotation")    #40
 bn["finger5-3.l_xrotation"]   =  (-1.0,"finger5-3.r_xrotation")    #41
 bn["finger5-3.l_yrotation"]   =  (-1.0,"finger5-3.r_yrotation")    #42
 #--------------------------------------------------------------------
 bn["lthumbBase_zrotation"]    =  (-1.0,"rthumbBase_zrotation")      #43 +?
 bn["lthumbBase_xrotation"]    =  (-1.0,"rthumbBase_xrotation")      #44 +?
 bn["lthumbBase_yrotation"]    =  (-1.0,"rthumbBase_yrotation")     #45
 bn["lthumb_zrotation"]        =  (-1.0,"rthumb_zrotation")         #46
 bn["lthumb_xrotation"]        =  (-1.0,"rthumb_xrotation")         #47
 bn["lthumb_yrotation"]        =  (-1.0,"rthumb_yrotation")         #48
 bn["finger1-2.l_zrotation"]   =  (-1.0,"finger1-2.r_zrotation")    #49
 bn["finger1-2.l_xrotation"]   =  (-1.0,"finger1-2.r_xrotation")    #50
 bn["finger1-2.l_yrotation"]   =  (-1.0,"finger1-2.r_yrotation")    #51
 bn["finger1-3.l_zrotation"]   =  (-1.0,"finger1-3.r_zrotation")    #52
 bn["finger1-3.l_xrotation"]   =  (-1.0,"finger1-3.r_xrotation")    #53
 bn["finger1-3.l_yrotation"]   =  (-1.0,"finger1-3.r_yrotation")    #54
 return bn
#---------------------------------------------------
def getSymmetricLHandNameList():
 bn=dict()
 #---------------------------------------------------
 #---------------------------------------------------
 bn["lhand"]               = "rhand"                    #0   - wrist
 bn["lthumb"]              = "rthumb"                   #1   - thumb_cmc
 bn["lthumbbase"]          = "rthumbbase"               #1 ?  - thumb_cmc
 bn["finger1-2.l"]         = "finger1-2.r"              #2   - thumb_mcp
 bn["finger1-3.l"]         = "finger1-3.r"              #3   - thumb_ip
 bn["endsite_finger1-3.l"] = "endsite_finger1-3.r"      #4   - thumb_tip
 bn["finger2-1.l"]         = "finger2-1.r"              #5   - index_finger_mcp
 bn["finger2-2.l"]         = "finger2-2.r"              #6   - index_finger_pip
 bn["finger2-3.l"]         = "finger2-3.r"              #7   - index_finger_dip
 bn["endsite_finger2-3.l"] = "endsite_finger2-3.r"      #8   - index_finger_tip
 bn["finger3-1.l"]         = "finger3-1.r"              #9   - middle_finger_mcp
 bn["finger3-2.l"]         = "finger3-2.r"              #10  - middle_finger_pip
 bn["finger3-3.l"]         = "finger3-3.r"              #11  - middle_finger_dip
 bn["endsite_finger3-3.l"] = "endsite_finger3-3.r"      #12  - middle_finger_tip
 bn["finger4-1.l"]         = "finger4-1.r"              #13  - ring_finger_mcp
 bn["finger4-2.l"]         = "finger4-2.r"              #14  - ring_finger_pip
 bn["finger4-3.l"]         = "finger4-3.r"              #15  - ring_finger_dip
 bn["endsite_finger4-3.l"] = "endsite_finger4-3.r"      #16  - ring_tip
 bn["finger5-1.l"]         = "finger5-1.r"              #17  - pinky_mcp
 bn["finger5-2.l"]         = "finger5-2.r"              #18  - pinky_pip
 bn["finger5-3.l"]         = "finger5-3.r"              #19  - pinky_dip
 bn["endsite_finger5-3.l"] = "endsite_finger5-3.r"      #20  - pinky_tip
 return bn
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
 


#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
class SimulatedMirroredEnsemble():
  def __init__(self,
               mirroredModel,
               mirroringName,
               symmetricNames         = list(),
               outputOperationsNeeded = list()
              ):
      self.mirroredModel          = mirroredModel
      self.partName               = mirroringName   
      self.inputReadyForTF        = np.empty([2, 1])
      self.NSRM                   = np.empty([2, 1])
      self.leftToRightNames       = symmetricNames
      self.mirroringName          = mirroringName
      self.outputOperationsNeeded = outputOperationsNeeded
      self.serial                 = mirroredModel.serial
      self.outputBVH              = dict()
      self.outputBVHMinima        = dict()
      self.outputBVHMaxima        = dict()
      #-------------------------------
      self.simulated              = True
      #-------------------------------
      self.output                 = dict()
      self.outputMinimumValue     = dict()
      self.outputMaximumValue     = dict()
      #-------------------------------
      self.directInputFlips  = dict()
      self.flippedInputFlips = dict()

      for key in self.mirroredModel.inputs:
           s = key.split("_",1)
           if (len(s)>0):
                 originalName = s[1]
                 if originalName in self.leftToRightNames:
                      flippedName       = self.leftToRightNames[originalName]
                      flippedXKey       = "2dx_%s"     % flippedName
                      flippedYKey       = "2dy_%s"     % flippedName
                      flippedVisibleKey = "visible_%s" % flippedName
                      self.flippedInputFlips["2dx_%s"  % originalName]     = flippedXKey       #<- this needs flip 
                      self.directInputFlips["2dy_%s"  % originalName]      = flippedYKey       #<- this we copy directly
                      self.directInputFlips["visible_%s"  % originalName]  = flippedVisibleKey #<- this we copy directly
 
      print("\n\n\nInputs that need to be subtracted from one : ",self.flippedInputFlips) 
      print("\n\n\nInputs that need to be just copied  : ",self.directInputFlips) 


  def getModel(self):
    return self.mirroredModel.model

  def getModelFlops(self): 
    return 0

  def getModelParameters(self):
    return 0

  def test(self):
    return 1

  def prepareInput(
                   self,
                   input2D :dict,
                   configuration : dict
                  ):
        self.inputReadyForTF = self.mirroredModel.predict(input2D=input2D)
        self.NSRM            = self.mirroredModel.NSRM
        return self.inputReadyForTF

  def predict(self,input2D :dict):
        #Replicating : https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/mnet3/src/MocapNET2/MocapNETLib2/solutionParts/rightHandSym.cpp
        #------------------------------------------------------------
        import copy
        flippedInput2D   = dict()
        #------------------------------------------------------------
        doInputFlips = True # Debug switch should alawys be set to True
        doOutputFlips = True # Debug switch should alawys be set to True
        #------------------------------------------------------------
        if (doInputFlips): #Do Input flips!
          #for key in input2D.keys():
          for keyR in self.mirroredModel.inputs: #<- fix right hand working only if left hand is visible
              key = keyR.lower()
              if (key in self.directInputFlips):
                  originalName = key
                  flippedName  = self.directInputFlips[originalName]
                  if (flippedName in input2D):
                      flippedInput2D[originalName] = float(input2D[flippedName])
              elif (key in self.flippedInputFlips):
                  originalName = key
                  flippedName  = self.flippedInputFlips[originalName]
                  if (flippedName in input2D):
                      if (float(input2D[flippedName])!=0.0): #<- This should be a check on the visibility channel 
                        flippedInput2D[originalName]    = 1.0 - float(input2D[flippedName])
              
        #----------------------------------------------------------------------------
        leftHandinputReadyForTF = copy.deepcopy(self.mirroredModel.inputReadyForTF)
        leftHandinputNSRM       = copy.deepcopy(self.mirroredModel.NSRM)
        # ===========================================================================
        originalName    = self.mirroredModel.partName
        self.mirroredModel.partName = self.partName
        #---
        self.mirroredOutput     = self.mirroredModel.predict(input2D=flippedInput2D) 
        #---
        self.mirroredModel.partName = originalName 
        #print("flipped yield ",self.mirroredOutput) #Debug
        # ===========================================================================
        self.inputReadyForTF    = copy.deepcopy(self.mirroredModel.inputReadyForTF)
        self.NSRM               = copy.deepcopy(self.mirroredModel.NSRM)
        #----------------------------------------------------------------------------
        if (doOutputFlips): #Do Output Flips!
           for originalKeyRaw in self.mirroredOutput:
                 originalKey = originalKeyRaw.lower()
                 #print("OUTPUT 3D FOUND ",originalKey," ",end="") #Debug
                 if (originalKey in self.outputOperationsNeeded):
                    flippedKey     =  self.outputOperationsNeeded[originalKey][1]
                    flippedFactor  =  self.outputOperationsNeeded[originalKey][0]
                    if (flippedFactor!=0.0) and (flippedKey!=""):
                       #print("USED ",flippedKey) #Debug
                       self.output[flippedKey] = flippedFactor * float(self.mirroredOutput[originalKey])
                    #else: #Debug
                    #   print("IGNORED ",flippedKey) #Debug

                 else:
                    eprint("SYMMETRY: THIS SHOULD NOT HAPPEN / NO RULE FOR ",originalKey,flippedKey)
        #------------------------------------------------------------
        #Restore left hand 
        self.mirroredModel.inputReadyForTF = copy.deepcopy(leftHandinputReadyForTF)
        self.mirroredModel.NSRM            = copy.deepcopy(leftHandinputNSRM)
        #------------------------------------------------------------
        return self.output
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------



#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
class MocapNET():
  def __init__(self,
               #-------------------------------------------------
               bvhFilePath:str         = "BVH/headerWithHeadAndOneMotion.bvh",
               disablePCACode          = 0,
               disableSmoothingCode    = 0,
               doPerformanceProfiling  = False,
               doHCDPostProcessing     = 1,
               hcdLearningRate         = 0.01,
               hcdEpochs               = 30,
               hcdIterations           = 15,
               langevinDynamics        = 0.0,
               bvhScale                = 1.0,
               addNoise                = 0.0,
               multiThreaded           = False,
               smoothingSampling       = 30.0,
               smoothingCutoff         = 5.0,

               bvhLibraryPath:str      = "BVH/libBVHConverter.so",
               smootherLibraryPath:str = "Smooth/libSmoothing.so",
               #-------------------------------------------------
               engine:str              = "onnx",
               ensembleToLoad: MocapNETEnsembleCombination = MocapNETEnsembleCombination(),
               #-------------------------------------------------
               record                  = False
               #-------------------------------------------------
              ):
               #-------------------------------------------------------------------------------
               self.record             = record
               self.inputHistory       = list()
               self.history            = list()
               self.outputHistory      = list()
               self.ensemble           = dict()
               #-------------------------------------------------------------------------------
               #First initialize the engine..
               self.engine = engine
               if (engine=="tensorflow") or (engine=="tf"):
                 from MocapNETTensorflow import MocapNETTensorflow
                 self.engineContext = MocapNETTensorflow()
               elif (engine=="tflite"):
                 from MocapNETTFLite import MocapNETTFLite
                 self.engineContext = MocapNETTFLite()
               elif (engine=="onnx"):
                 from MocapNETONNX import MocapNETONNX
                 self.engineContext = MocapNETONNX()
               else:
                 print("selectMocapNETClassBasedOnEngine:  Unknown engine ",engine)
                 sys.exit(1)
               #-------------------------------------------------------------------------------
               for ensembleID in range(0,len(ensembleToLoad.ensembleNameList)):
                ensembleName = ensembleToLoad.ensembleNameList[ensembleID] 
                partName     = "%s_all" % ensembleName
                ensemblePath = ensembleToLoad.ensemblePathList[ensembleID] 
                if (ensemblePath!="symmetric"):
                 print(bcolors.OKGREEN,"Loading ",ensembleName,"..",bcolors.ENDC)
                 configurationPath = "%s/%s_configuration.json" % (ensemblePath,ensembleName) 
                 if (engine=="tensorflow") or (engine=="tf"):
                   from MocapNETTensorflow import MocapNETTensorflowSubProblem
                   modelPath = "%s/" % (ensemblePath) 
                   self.ensemble[ensembleName] = MocapNETTensorflowSubProblem(
                                                                              context                  = self.engineContext,
                                                                              configPath               = configurationPath,
                                                                              modelPath                = modelPath,
                                                                              partName                 = partName,
                                                                              completelyDisablePCACode = disablePCACode
                                                                             )
                 elif (engine=="tflite"):
                   from MocapNETTFLite import MocapNETTFLiteSubProblem
                   modelPath = "%s/model.tflite" % (ensemblePath) 
                   self.ensemble[ensembleName] = MocapNETTFLiteSubProblem(
                                                                          context                  = self.engineContext,
                                                                          configPath               = configurationPath,
                                                                          modelPath                = modelPath,
                                                                          partName                 = partName,
                                                                          completelyDisablePCACode = disablePCACode
                                                                         )
                 elif (engine=="onnx"):
                   from MocapNETONNX import MocapNETONNXSubProblem
                   modelPath = "%s/model.onnx" % (ensemblePath) 
                   self.ensemble[ensembleName] = MocapNETONNXSubProblem(
                                                                        context                  = self.engineContext,
                                                                        configPath               = configurationPath,
                                                                        modelPath                = modelPath,
                                                                        partName                 = partName,
                                                                        completelyDisablePCACode = disablePCACode
                                                                       )
                elif (ensemblePath=="symmetric"):
                  #If we are handling a lhand we get an rhand for free :P
                  if (ensembleName=="rhand"):
                     self.ensemble["rhand"] = SimulatedMirroredEnsemble(
                                                                         mirroredModel          = self.ensemble["lhand"],
                                                                         mirroringName          = "rhand", 
                                                                         symmetricNames         = getSymmetricLHandNameList(),
                                                                         outputOperationsNeeded = getSymmetricLHandOutputs()
                                                                       )
                  #If we are handling a reye we get an leye for free :P
                  if (ensembleName=="leye"):
                     self.ensemble["leye"]  = SimulatedMirroredEnsemble(
                                                                         mirroredModel          = self.ensemble["reye"],
                                                                         mirroringName          = "leye", 
                                                                         symmetricNames         = getSymmetricLEyeNameList(),
                                                                         outputOperationsNeeded = getSymmetricLEyeOutputs()
                                                                       )
               #-------------------------------------------------------------------------------
               print(bcolors.OKGREEN,"Combined  network has ",self.getModelParameters()," parameters..",bcolors.ENDC)
               #-------------------------------------------------------------------------------
               #-------------------------------------------------------------------------------
               print(bcolors.OKGREEN,"Loading C/Python libraries..",bcolors.ENDC)
               self.multiThreaded      = multiThreaded
               self.doFineTuning       = doHCDPostProcessing
               self.addNoise           = addNoise
               self.smoothingSampling  = smoothingSampling
               self.smoothingCutoff    = smoothingCutoff
               self.bvhScale           = bvhScale
               self.lastMAEErrorInPixels = 0.0
               if (disableSmoothingCode==1):
                  self.smoothingSampling  = 0.0
                  self.smoothingCutoff    = 0.0
               self.hcdLearningRate    = hcdLearningRate
               self.hcdEpochs          = hcdEpochs
               self.hcdIterations      = hcdIterations 
               #-------------------------------------------------------------------------------
               self.langevinDynamics   = langevinDynamics
               self.bvhFilePath        = bvhFilePath
               self.bvh                = BVH(bvhPath = bvhFilePath,libraryPath = bvhLibraryPath)
               self.bvh.scale(self.bvhScale)
               self.bvhJointList       = convertListToLowerCase(self.bvh.getJointList())
               self.bvhJointParentList = self.bvh.getJointParentList()
               #-------------------------------------------------------------------------------
               self.incompleteUpperbodyInput = 1
               self.incompleteLowerbodyInput = 1
               #-------------------------------------------------------------------------------
               self.framesProcessed    = 0
               self.currentPrediction  = dict()
               self.previousPrediction = dict()
               self.input2D            = dict()
               self.output             = dict()
               self.output2D           = dict()
               self.outputBVH          = dict()
               self.outputBVHMinima    = dict()
               self.outputBVHMaxima    = dict()
               self.output3D           = dict()
               
               self.perfHistorySize    = 30
               #-------------------------------------------------------------------------------
               self.history_hz_2DEst   = []
               self.hz_2DEst           = 0.0
               self.history_hz_NN      = []
               self.hz_NN              = 0.0
               self.history_hz_HCD     = []
               self.hz_HCD             = 0.0
               self.history_hz_Vis     = []
               self.hz_Vis             = 0.0
               #-------------------------------------------------------------------------------
             

               #-------------------------------------------------------------------------------
               print("Caching networks : ")
               self.test()
               print(bcolors.OKGREEN,"MocapNET ready for use! ",bcolors.ENDC)
               #-------------------------------------------------------------------------------
               from tools import checkVersion
               checkVersion(MOCAPNET_VERSION)

  def recordBVH(self,val:bool):
        self.record=val
        return True

  def hasEnsemble(self,name):
        if (name in self.ensemble):
            return True
        else:
            return False


  def getUpperBodyModel(self):
        return self.ensemble["upperbody"].getModel()

  def getLowerBodyModel(self):
        return self.ensemble["lowerbody"].getModel()
  
  def getModelFlops(self):
    total = 0.0
    for k in self.ensemble.keys():
      total = total + self.ensemble[k].getModelFlops() 
    return total

  def getModelParameters(self):
    total = 0
    for k in self.ensemble.keys():
      total = total + self.ensemble[k].getModelParameters() 
    return total

  def getEnsembleSerials(self):
    description = ""
    from datetime import datetime, date, time, timezone
    #description = datetime.now().strftime("%Y-%m-%d %H:%M:%S ")
    description = datetime.now().strftime("%Y-%m-%d ")
    for k in self.ensemble.keys():
      description = description + k + ":" + self.ensemble[k].serial + " "
    return description 


  def test(self):
        #-------------------------------------------
        for k in self.ensemble.keys():
              print("Testing loaded ",k," model ")
              self.ensemble[k].test() 
        #-------------------------------------------


  def enforceBanlistOnOutput(self,output):
        #Banlist -------------------------------------
        if "abdomen_zrotation" in output.keys():
            output["abdomen_zrotation"]=0.0
        if "abdomen_xrotation" in output.keys():
            output["abdomen_xrotation"]=0.0
        if "abdomen_yrotation" in output.keys():
            output["abdomen_yrotation"]=0.0
        #--------------------------------------------
        if "chest_zrotation" in output.keys():
            output["chest_zrotation"]=0.0
        if "chest_xrotation" in output.keys():
            output["chest_xrotation"]=0.0
        if "chest_yrotation" in output.keys():
            output["chest_yrotation"]=0.0
        #--------------------------------------------
        return output


  def countMissingItemsPercentage(self, inputReadyForMocapNET ):
      return 0.0


  def perturbInput(self,input2D :dict):
        import random
        for i in range(5):
           print(bcolors.FAIL,"NOISE SCHEDULE IS ACTIVE AND SYNTHETIC NOISE IS ADDED (",self.addNoise,") ..",bcolors.ENDC)
        peturbedInput2D = input2D
        for coordLabel in peturbedInput2D.keys():
            #print(coordLabel)
            coordLabelL = coordLabel.lower()
            if ("2dx_" in coordLabelL) or ("2dy_" in coordLabelL):
                if (peturbedInput2D[coordLabel]>0.0):
                  perturbation = random.uniform(float(-self.addNoise/2.0),float(self.addNoise/2.0))   
                  #print("Perturbing ",peturbedInput2D[coordLabel]," with ",perturbation," -> ",end="")
                  peturbedInput2D[coordLabel]=peturbedInput2D[coordLabel] + perturbation
                  #print("  ",peturbedInput2D[coordLabel])

        return peturbedInput2D

  """
             Convert a dictionary of 2D inputs to MocapNET output
       (Whatever that maybe [it is listed in self.inputs and self.outputs])
  """
  def predict(self,input2D :dict):
        start = time.time()
        #--------------------------------------------------------------------------------------
        if (self.addNoise>0.0):
             input2D = self.perturbInput(input2D)
        #--------------------------------------------------------------------------------------
        self.input2D         = input2D
        self.output          = dict()
        self.outputBVHMinima = dict()
        self.outputBVHMaxima = dict()
        #--------------------------------------------------------------------------------------
        if (self.record): 
               self.inputHistory.append(input2D)
        #--------------------------------------------------------------------------------------

        #print("INPUT2D : ",input2D)
        for k in self.ensemble.keys():
              thisEnsemble = self.ensemble[k]
              #---------------------------------------------------------------
              ensembleOutput = thisEnsemble.predict(input2D)
              #print("Ensemble ",thisEnsemble.partName," : ",ensembleOutput) #Debug
              #---------------------------------------------------------------
              self.output.update(ensembleOutput)
              if (not thisEnsemble.simulated): 
                self.outputBVHMinima.update(thisEnsemble.outputMinimumValue)
                self.outputBVHMaxima.update(thisEnsemble.outputMaximumValue)
              #---------------------------------------------------------------

        self.output = self.enforceBanlistOnOutput(self.output)

        self.framesProcessed = self.framesProcessed + 1
        #--------------------------------------------------------------------------------------
        end = time.time() # Time elapsed
        self.hz_NN = secondsToHz(end - start)
        #--------------------------------------------------------------- 
        self.history_hz_NN.append(self.hz_NN) 
        if (len(self.history_hz_NN)>self.perfHistorySize): 
                    self.history_hz_NN.pop(0) #Keep mnet history on limits
        #--------------------------------------------------------------- 
           

        #If we want record, record the raw BVH prediction
        #print("RECORD : ",self.output)
        if (self.record):
               self.outputHistory.append(self.output) #This does not have HCD improvement.. 

        #print("\r MocapNET Wrapper NeuralNetwork Framerate : ",round(self.hz_NN,2)," fps           \r", end="", flush=True)
        #print("\n", end="", flush=True)

        return self.output

  
  def printStatus(self): 
        import sys
        sys.stdout.write("\rFrame "+str(self.framesProcessed)+"|"+self.engine+"|MPJPE "+str(round(self.bvh.lastMAEErrorInPixels,1))+" px|2D NN:"+str(round(self.hz_2DEst,1))+"Hz|MocapNET:"+str(round(self.hz_NN,1))+"Hz|HCD:"+str(round(self.hz_HCD,1))+"Hz     ")
        sys.stdout.flush()


  """
             Convert a dictionary of 2D inputs to MocapNET output
       (Whatever that maybe [it is listed in self.inputs and self.outputs])
  """
  def predictMultiThreaded(self,input2D :dict):
        #----------------------------------
        if (self.multiThreaded):
           start = time.time()
           #--------------------------------------------------------------------------------------
           if (self.addNoise>0.0):
              input2D = self.perturbInput(input2D)
           #--------------------------------------------------------------------------------------
           self.input2D         = input2D
           self.output          = dict()
           self.outputBVHMinima = dict()
           self.outputBVHMaxima = dict()
           #--------------------------------------------------------------------------------------
           if (self.record): 
               self.inputHistory.append(input2D)
           #--------------------------------------------------------------------------------------
           
           #Create and handle thread initialization if needed..
           self.threads = [] 
           import threading
           for k in self.ensemble.keys():
                    #---------------------------------------------------------------
                    thisEnsemble = self.ensemble[k]
                    self.threads.append(threading.Thread(target=thisEnsemble.predict, args=(self.input2D,)) )
                    #--------------------------------------------------------------- 
           #---------------------------------------
           for thread in self.threads:
              thread.start()
           #---------------------------------------
           #Parallel execution here..           
           #---------------------------------------
           for thread in self.threads:
              thread.join()
           #---------------------------------------
           for k in self.ensemble.keys():
              thisEnsemble = self.ensemble[k]
              self.output.update(thisEnsemble.output)
              if (k!="rhand") and (k!="lhand") and (k!="leye") : #/Why ?
                self.outputBVHMinima.update(thisEnsemble.outputMinimumValue)
                self.outputBVHMaxima.update(thisEnsemble.outputMaximumValue)
              #--------------------------------------------------------------- 
           self.output = self.enforceBanlistOnOutput(self.output)
           self.framesProcessed = self.framesProcessed + 1
           end = time.time() # Time elapsed
           self.hz_NN = secondsToHz(end - start)
           #--------------------------------------------------------------- 
           self.history_hz_NN.append(self.hz_NN) 
           if (len(self.history_hz_NN)>self.perfHistorySize): 
                    self.history_hz_NN.pop(0) #Keep mnet history on limits
           #--------------------------------------------------------------- 

           #If we want record, record the raw BVH prediction
           #print("RECORD MT : ",self.output)
           if (self.record):
               self.outputHistory.append(self.output) #This does not have HCD improvement.. 


           #print("\r MocapNET MultiThreaded NeuralNetwork Framerate : ",round(self.hz_NN,2)," fps           \r", end="", flush=True)
           #print("\n", end="", flush=True)
        else:
           print("Fallback to single threaded code..")
           self.predict(input2D)
        return self.output



  """
             Convert a dictionary of 2D inputs to MocapNET output
       (Whatever that maybe [it is listed in self.inputs and self.outputs])
  """
  def fineTune(self,input2D :dict, NNOutput:dict):
        if (self.bvh.modify(NNOutput)): 
           self.bvh.processFrame(0) #only have 1 frame ID

        if (self.hcdIterations>0) and (self.doFineTuning==1):
                #print(bcolors.OKGREEN,"Running HCD..",bcolors.ENDC)
                self.bvh.fineTuneToMatch("body",
                                          input2D,
                                          frameID          = 0,
                                          iterations       = self.hcdIterations,
                                          epochs           = self.hcdEpochs,
                                          lr               = self.hcdLearningRate, 
                                          fSampling        = self.smoothingSampling,
                                          fCutoff          = self.smoothingCutoff)
                self.bvh.processFrame(0) #This should now be updated with the IK fine tuned prediction..!


  def predict3DJoints(self,input2D :dict,runNN:bool=True,runHCD:bool=True):
        #Extract a BVH dict of BVH motion fields
        if ((runNN) or (len(self.previousPrediction)==0)): 
            if (self.multiThreaded): 
               rawBVHPrediction = self.predictMultiThreaded(input2D) #If multithreading is disabled this fallbacks to single threaded..
            else:
               rawBVHPrediction = self.predict(input2D)
            self.previousPrediction = rawBVHPrediction
        else:
            rawBVHPrediction = self.previousPrediction

        #This spams a lot..
        #print("Predictions from ensemble keys : ",rawBVHPrediction.keys())

        # Deal with 3D Mode
        #--------------------------------------------------------------------
        if ("upperbody" in self.ensemble) and ("lowerbody" in self.ensemble):
         if ('outputMode' in self.ensemble["upperbody"].configuration) and ('outputMode' in self.ensemble["lowerbody"].configuration):  
             #Running on a recent build with bvh/3d output mode switching
             if (self.ensemble["upperbody"].configuration['outputMode']=='3d') and (self.ensemble["lowerbody"].configuration['outputMode']=='3d'):
                 self.output3D = capitalizeCoordinateTags(self.output)
                 print(bcolors.OKGREEN,"DIRECT 3D RECOVERY!",bcolors.ENDC)
                 #print(self.output3D)
                 return self.output3D
        #--------------------------------------------------------------------



        #Modify our BVH armature with the new BVH values
        if (self.bvh.modify(rawBVHPrediction)): 
           
           #Remember BVH Pose
           self.outputBVH = rawBVHPrediction

           #Render to 2D/3D
           self.bvh.processFrame(0) #only have 1 frame ID <- we load our raw prediction
           
           fineTuningPasses = 0 
           if (self.hcdIterations>0) and (self.doFineTuning==1) and (runHCD):
                #print(bcolors.OKGREEN,"Running HCD..",bcolors.ENDC)
                start = time.time()
                if ("upperbody" in self.ensemble) or ("lowerbody" in self.ensemble):
                   self.bvh.fineTuneToMatch(
                                          "body",
                                          input2D,
                                          frameID          = 0,
                                          iterations       = self.hcdIterations,
                                          epochs           = self.hcdEpochs,
                                          lr               = self.hcdLearningRate,
                                          fSampling        = self.smoothingSampling,
                                          fCutoff          = self.smoothingCutoff,
                                          langevinDynamics = self.langevinDynamics
                                        )
                   fineTuningPasses = fineTuningPasses + 1
                   self.lastMAEErrorInPixels = self.bvh.lastMAEErrorInPixels
                if ("lhand" in self.ensemble):
                   self.bvh.fineTuneToMatch(
                                          "lhand",
                                          input2D,
                                          frameID          = 0,
                                          iterations       = self.hcdIterations,
                                          epochs           = self.hcdEpochs,
                                          lr               = self.hcdLearningRate,
                                          fSampling        = self.smoothingSampling,
                                          fCutoff          = self.smoothingCutoff,
                                          langevinDynamics = self.langevinDynamics
                                        )
                   fineTuningPasses = fineTuningPasses + 1
                if ("rhand" in self.ensemble):
                   self.bvh.fineTuneToMatch(
                                          "rhand",
                                          input2D,
                                          frameID          = 0,
                                          iterations       = self.hcdIterations,
                                          epochs           = self.hcdEpochs,
                                          lr               = self.hcdLearningRate,
                                          fSampling        = self.smoothingSampling,
                                          fCutoff          = self.smoothingCutoff,
                                          langevinDynamics = self.langevinDynamics
                                        )
                   fineTuningPasses = fineTuningPasses + 1

                #--------------------------------------------------------------------------------------
                self.bvh.smooth(frameID=0,fSampling = self.smoothingSampling,fCutoff = self.smoothingCutoff)
                #self.bvh.processFrame(0) # <- this is now done internally to simplify code.. This should now be updated with the IK fine tuned prediction..!
                #--------------------------------------------------------------------------------------
                end = time.time()
                # Time elapsed
                seconds = end - start
                if (seconds==0.0):
                     seconds=1.0
                # Calculate frames per second
                self.hz_HCD  = 1 / seconds
                #-------------------------------------------------------------
                self.history_hz_HCD.append(self.hz_HCD)
                if (len(self.history_hz_HCD)>self.perfHistorySize): 
                    self.history_hz_HCD.pop(0) #Keep mnet history on limits
                #-------------------------------------------------------------
                #if (fineTuningPasses>0):
                #   print("MocapNET HCD Fine tuning Framerate : ",round(self.hz_HCD,2)," fps           \n", end="", flush=True)



           #This block prevents(?) an endless loop of zeros..
           if ( self.bvh.lastMAEErrorInPixels<0.001 ):
                  print(bcolors.FAIL,"RESET SKELETON (",self.bvh.lastMAEErrorInPixels,") ",bcolors.ENDC)
                  #rawBVHPrediction["hip_XPosition"]=0.0
                  #rawBVHPrediction["hip_YPosition"]=0.0
                  #rawBVHPrediction["hip_ZPosition"]=-200.0
                  #self.bvh.modify(rawBVHPrediction)
                  self.bvh.setMotionValueOfFrame(0,2,-200.0)
                  self.bvh.processFrame(0) #only have 1 frame ID <- we load our raw prediction
                  self.bvh.lastMAEErrorInPixels = 1000.0
                  #print("input2D:",input2D)
                  #print("rawBVHPrediction:",rawBVHPrediction)




           #If we want record the file
           if (self.record): 
               self.history.append(self.bvh.getAllMotionValuesOfFrame(0)) 

           #Retreive 2D/3D Values
           self.output2D = dict()
           self.output3D = dict()
           for jointID in range(0,self.bvh.numberOfJoints):
               #-------------------------------------------
               jointName   = self.bvh.getJointName(jointID).lower()
               #-------------------------------------------
               x3D,y3D,z3D = self.bvh.getJoint3D(jointID)
               self.output3D["3DX_"+jointName]=float(x3D)
               self.output3D["3DY_"+jointName]=float(y3D)
               self.output3D["3DZ_"+jointName]=float(z3D)
               #-------------------------------------------
               x2D,y2D = self.bvh.getJoint2D(jointID)
               self.output2D["2DX_"+jointName]=float(x2D)
               self.output2D["2DY_"+jointName]=float(y2D)
               #-------------------------------------------
        else:
           print(bcolors.FAIL,"We where unable to process the BVH output",bcolors.ENDC)


        return self.output3D

  def __del__(self):
        print(' ')
        if (self.record):
               print("Write BVH Output!") 
               self.bvh.saveBVHFileFromList("out.bvh",self.history)

               from tools import saveCSVFileFromListOfDicts
               print("Write BVH Output in CSV format!") 
               saveCSVFileFromListOfDicts("out.csv",self.outputHistory)
               print("Write 2D Input!")
               saveCSVFileFromListOfDicts("in.csv",self.inputHistory)
        else:
               print("Did not record output due to --live mode")

        print('Thank you for using MocapNET!')
        print('https://github.com/FORTH-ModelBasedTracker/MocapNET')
 

#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
def easyMocapNETConstructor(
                            engine              = "onnx",
                            doProfiling         = False,
                            doHCDPostProcessing = 1,
                            hcdLearningRate     = 0.01,
                            hcdEpochs           = 30,
                            hcdIterations       = 15,
                            smoothingSampling   = 30.0,
                            smoothingCutoff     = 5.0, 
                            multiThreaded       = False,
                            bvhScale            = 1.0,
                            doBody              = True,
                            doUpperbody         = False, #<- These get auto activated if doBody=True
                            doLowerbody         = False, #<- These get auto activated if doBody=True
                            doFace              = False,
                            doREye              = False,
                            doMouth             = False,
                            doHands             = False,
                            doSymmetries        = True,
                            addNoise            = 0.0
                           ):
    combo = MocapNETEnsembleCombination()
    #--------------------------------------------------------------
    if (doFace):
         combo.addEnsemble("face","step1_face_all/")
    if (doMouth):
         combo.addEnsemble("mouth","step1_mouth_all/")
    if (doREye):
         combo.addEnsemble("reye","step1_reye_all/")
         if(doSymmetries):
           combo.addEnsemble("leye","symmetric")  #leye will get initialized automatically 
    if (doHands):
         combo.addEnsemble("lhand","step1_lhand_all/")
         if(doSymmetries):
           combo.addEnsemble("rhand","symmetric") #rhand will get initialized automatically 
    if (doBody or doLowerbody) :
         combo.addEnsemble("lowerbody","step1_lowerbody_all/")
    if (doBody or doUpperbody) :
         combo.addEnsemble("upperbody","step1_upperbody_all/")
    #--------------------------------------------------------------
    mnet = MocapNET(
                    doPerformanceProfiling = doProfiling,
                    doHCDPostProcessing    = doHCDPostProcessing,
                    hcdLearningRate        = hcdLearningRate,
                    hcdEpochs              = hcdEpochs,
                    hcdIterations          = hcdIterations, 
                    multiThreaded          = multiThreaded,
                    bvhScale               = bvhScale, 
                    engine                 = engine,
                    ensembleToLoad         = combo,
                    addNoise               = addNoise,
                    smoothingSampling      = smoothingSampling,
                    smoothingCutoff        = smoothingCutoff
                   )
    return mnet
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------

if __name__ == '__main__':
    mnet = MocapNET(
                    configUpperBodyPath = "step1_upperbody_all/upperbody_configuration.json",
                    modelUpperBodyPath="step1_upperbody_all",
                    configLowerBodyPath = "step1_lowerbody_all/lowerbody_configuration.json",
                    modelLowerBodyPath="step1_lowerbody_all",
                    bvhFilePath="BVH/headerWithHeadAndOneMotion.bvh"
                   )

    mnet.test()
    print("Survived Test!")
