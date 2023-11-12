#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import onnxruntime as ort
import onnx
import os
import sys
import time

#-------------------------------------------------------------------------------------------
from readCSV  import parseConfiguration,parseConfigurationInputJointMap,transformNetworkInput,initializeDecompositionForExecutionEngine,readGroundTruthFile,readCSVFile,parseOutputNormalization
from NSDM     import NSDMLabels,createNSDMUsingRules,inputIsEnoughToCreateNSDM,performNSRMAlignment
from EDM      import EDMLabels,createEDMUsingRules
from tools    import bcolors,eprint,checkIfFileExists,readListFromFile,convertListToLowerCase,secondsToHz,capitalizeCoordinateTags,getEntryIndexInList,parseSerialNumberFromSummary
#-------------------------------------------------------------------------------------------
from BVH.bvhConverter import BVH
#-------------------------------------------------------------------------------------------
#from Smooth.smoothing import Smooth
#-------------------------------------------------------------------------------------------
from principleComponentAnalysis import PCA
#-------------------------------------------------------------------------------------------

import numpy as np

#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
class MocapNETONNXSubProblem():
  def __init__(self,
               context,
               configPath:str,
               modelPath:str,
               partName:str,
               completelyDisablePCACode = 0
              ):
               #self.options = context.sess_options
               self.options = ort.SessionOptions()
               #-------------------------------------------------------------------------------
               self.useOutputLimits = True #Careful, this should always be on!
               self.partName        = partName
               self.configPath      = configPath
               self.configuration   = parseConfiguration(configPath)
               self.part            = self.configuration["OutputDirectory"] 
               self.inputName       = "input_all"
               self.modelPath       = modelPath
               self.modelDirectory  = os.path.dirname(self.modelPath)
               self.frameNumber     = 0
               #-------------------------------------------------------------------------------
               onnxModelForCheck       = onnx.load(modelPath)
               onnx.checker.check_model(onnxModelForCheck) 
               print("ONNX devices available : ", ort.get_device()) 
               providers               = ['CPUExecutionProvider']
               #providers               = ['CUDAExecutionProvider']
               self.model              = ort.InferenceSession(modelPath, providers=providers, sess_options=self.options)
               for i in range(0,len(self.model.get_inputs())): 
                  print("ONNX INPUTS ",self.model.get_inputs()[i].name)
                  self.inputName = self.model.get_inputs()[i].name

               self.model_input_name   = self.model.get_inputs()
               #-------------------------------------------------------------------------------
               self.inputsWithNSRM     = convertListToLowerCase(readListFromFile(self.modelDirectory+"/neuralNetworkInputs.list"))
               self.inputs             = convertListToLowerCase(readListFromFile(self.modelDirectory+"/neuralNetworkJoints.list"))
               self.outputs            = convertListToLowerCase(readListFromFile(self.modelDirectory+"/neuralNetworkOutputs.list"))
               self.configuration      = parseConfigurationInputJointMap(self.configuration,self.inputs)
               self.serial             = parseSerialNumberFromSummary(self.modelDirectory+"/summary.html")
               #-------------------------------------------------------------------------------
               self.inputReadyForTF    = np.empty([2, 1])
               self.NSRM               = np.empty([2, 2])
               #-------------------------------------------------------------------------------
               self.emptyList          = [0.0] * len(self.inputsWithNSRM)
               self.emptyInput         = np.asarray([self.emptyList],dtype=np.float32)
               self.emptyList          = [0.0] * len(self.outputs)
               self.emptyOutput        = np.asarray([self.emptyList],dtype=np.float32)
               #-------------------------------------------------------------------------------
               self.outputScalars      = [1.0] * len(self.outputs)
               self.outputOffsets      = [0.0] * len(self.outputs)
               self.outputMinima       = [-6000.0] * len(self.outputs) #huge limit that essentially doesn't limit anything
               self.outputMaxima       = [6000.0]  * len(self.outputs) #huge limit that essentially doesn't limit anything
               #-------------------------------------------------------------------------------
               self.outputOffsets      = parseOutputNormalization(self.modelDirectory,"/outputOffsets.csv",self.outputs,self.outputOffsets)
               self.outputScalars      = parseOutputNormalization(self.modelDirectory,"/outputScalarsFraction.csv",self.outputs,self.outputScalars)
               self.outputMinima       = parseOutputNormalization(self.modelDirectory,"/outputMinima.csv",self.outputs,self.outputMinima)
               self.outputMaxima       = parseOutputNormalization(self.modelDirectory,"/outputMaxima.csv",self.outputs,self.outputMaxima)
               #-------------------------------------------------------------------------------
               if (self.outputs[0]=="depth"):
                     self.outputs[0]="hip_zposition"
               #-------------------------------------------------------------------------------
               print("Output Mapping :")
               for jointID in range(0,len(self.outputs)):
                   #self.outputScalars[jointID] = 1 / float(self.outputScalars[jointID])
                   #print(" - Output ",self.outputs[jointID]," limits [",self.outputMinima[jointID],",",self.outputMaxima[jointID],"] scalar ",self.outputScalars[jointID]," offset ",self.outputOffsets[jointID])
                   print("Out %s|Min %0.2f|Max %0.2f|Scalar %0.2f|Offset %0.2f"%(self.outputs[jointID],self.outputMinima[jointID],self.outputMaxima[jointID],self.outputScalars[jointID],self.outputOffsets[jointID]))
               #-------------------------------------------------------------------------------
               self.incompleteInput    = 1
               #-------------------------------------------------------------------------------
               self.simulated          = False
               #-------------------------------------------------------------------------------
               self.output             = dict()
               self.outputMinimumValue = dict()
               self.outputMaximumValue = dict()
               #-------------------------------------------------------------------------------
               self.disablePCACode = completelyDisablePCACode
               if (not self.disablePCACode):
                  self.decompositionEngine = initializeDecompositionForExecutionEngine(self.configuration,self.modelDirectory,self.partName,disablePCACode=self.disablePCACode)
               #-------------------------------------------------------------------------------
               #The default compatibility setting is the BMVC2019 2channel NSDM, however nowadays we use NSRM
               numberOfChannelsPerNSDMElement=2
               if (self.configuration['NSDMAlsoUseAlignmentAngles']==1):
                  numberOfChannelsPerNSDMElement=1
               print("Number of Channels Per NSDM element ",numberOfChannelsPerNSDMElement)
               #-------------------------------------------------------------------------------
               if ("eigenPoses" in self.configuration): 
                       if (self.configuration['eigenPoses']==1): 
                          self.configuration['eigenPoseData'] = readGroundTruthFile(
                                                                self.configuration,
                                                                "Eigenposes",
                                                                "%s/2d_%s_eigenposes.csv"  % (os.path.dirname(self.modelPath),self.partName),
                                                                "%s/%s_%s_eigenposes.csv"  % (os.path.dirname(self.modelPath),self.configuration['outputMode'],self.partName), #configuration['outputMode'] is either bvh or 3d
                                                                1.0,
                                                                numberOfChannelsPerNSDMElement,
                                                                int(self.configuration['useRadians']),#useRadians,
                                                                0,#useHalfFloats
                                                                externalDecomposition=self.decompositionEngine
                                                               )
               #-------------------------------------------------------------------------------
               print("\n\n")
               print("Inputs :",self.inputs)
               print("Outputs :",self.outputs)
               #-------------------------------------------------------------------------------

  def getModel(self):
    return self.model

  def getModelFlops(self): 
    print("ONNX has no flops calculator")
    return 0

  def getModelParameters(self):
    print("ONNX has no model parameters calculator")
    return 0

  def test(self):
        #-------------------------------------------
        thisInputONNX     = { self.inputName : self.emptyInput}
        output_names_onnx = [otp.name for otp in self.model.get_outputs()]
        predictions       = self.model.run(output_names_onnx,thisInputONNX)[0][0]
        #-------------------------------------------
        return 1

  def prepareInput(self,input2D :dict,configuration : dict):
        from readCSV import prepareInputG
        thisFullInput, self.NSRM, thisInput, angleToRotate, missingRatio = prepareInputG(input2D,configuration,self.inputs,self.inputsWithNSRM,self.part,self.decompositionEngine,self.disablePCACode)
        #appendCSVToFile(self.inputName+".csv",thisFullInput,fID=self.frameNumber) # <-----------------
        inputReadyForTF = np.asarray([thisFullInput],dtype=np.float32)
        return inputReadyForTF,missingRatio

  def logProbabilisticOutput(self,outputFromNN,resolution=60,increment=6.0,numberOfJoints=30):
        xs=list()
        #----------------------------------
        value = -180.0
        inc   =  increment
        for r in range(0,resolution):
            xs.append(value)
            value=value+inc
        #----------------------------------

        ys=list()
        #----------------------------------
        for j in range(0,numberOfJoints):
               rs=list()
               for r in range(0,resolution):
                  rs.append(outputFromNN[(j*resolution) + r])
               ys.append(rs)
        #----------------------------------

        # Importing packages
        import matplotlib.pyplot as plt
         
        #plt.figure(figsize=(80,80))
        plt.clf()
        plt.title("Output Distributions %s "%(self.partName))

        # Define data values
        #print("Should plot %u lines"%numberOfJoints)
        for j in range(0,numberOfJoints):
              plt.plot(xs, ys[j], label='%s (#%u)' %(self.outputs[j],j))
              #print("Plot %u"%j)
 
        plt.legend()
        #plt.show() #<-This blocks
        plt.draw()
        plt.pause(0.01)





  """
             Convert a dictionary of 2D inputs to MocapNET output
       (Whatever that may be [it is listed in self.inputs and self.outputs])
  """
  def castProbabilisticOutputToDiscreteOutput(self, outputFromNN):
        if ('probabilisticOutput' in self.configuration) and (self.configuration['probabilisticOutput']==1):
            print(bcolors.OKGREEN,"DOING PROBABILISTIC OUTPUT",bcolors.ENDC)

            #Resolution incrementation 
            inc        =  10.0
            #-------------------------
            minV       =  -180.0 
            maxV       =  180.0
            resolution =  0 #<- gets automatically calculated as a function of inc..
            #-------------------------
            i = minV
            while(i<maxV):
                resolution = resolution + 1
                i=i+inc
            #-------------------------

            numberOfJoints = int(len(outputFromNN) / resolution)

            #Draw plot!
            self.logProbabilisticOutput(outputFromNN,resolution=resolution,increment=inc,numberOfJoints=numberOfJoints)

            pickedOutput = list()
            for j in range(0,numberOfJoints):
               bestChoice = 0
               bestValue  = 0
               value = minV
               for r in range(0,resolution):
                if (outputFromNN[(j*resolution) + r]>bestValue): 
                  bestValue  = outputFromNN[(j*resolution + r)]
                  bestChoice = value
                value=value+inc
               pickedOutput.append(bestChoice)

            #pickedOutput[0] = 0
            #pickedOutput[1] = 0
            pickedOutput[2] = -160
            pickedOutput[3] = 0
            pickedOutput[4] = 0
            pickedOutput[5] = 0
            return pickedOutput
        return outputFromNN  



  """
             Convert a dictionary of 2D inputs to MocapNET output
       (Whatever that maybe [it is listed in self.inputs and self.outputs])
  """
  def predict(self, input2D:dict):
        #print("Predict ",self.partName)
        self.inputReadyForTF,missingRatio = self.prepareInput(input2D,self.configuration)

        if (missingRatio>0.3):
           eprint("Not running ",self.partName," due to missing joints ")#,input2D)
           return self.output


        #Turns out on some decompositions like FastICA there are a lot of zeros!
        #-----------------------------------------------
        #Save cycles by not executing an empty data blob
        #-----------------------------------------------
        self.incompleteInput = 0 #<- This needs to be set to 0 to mark input is received..!

        #Cast and then run input through MocapNET
        thisInputONNX     = { self.inputName : self.inputReadyForTF }
        output_names_onnx = [otp.name for otp in self.model.get_outputs()]
        predictions       = self.model.run(output_names_onnx,thisInputONNX)[0][0]
        #predictions       = self.model(self.inputReadyForTF,training=False) 

        #PROBABILISTIC MODE
        if ('probabilisticOutput' in self.configuration) and (self.configuration['probabilisticOutput']==1):
          predictions = self.castProbabilisticOutputToDiscreteOutput(predictions)

        self.output = dict()
        if (len(predictions)!=len(self.outputs)):
           print(bcolors.FAIL,"Something bad happened.. the network regressed a different number of parameters (",len(predictions),") than what we expected (",len(self.outputs),") ",bcolors.ENDC)
           raise IOError
           #Go on with it
           return self.output

        #Values to list..
        outputValueList = list()

        for i in range (len(self.outputs)):
            outputValueList.append(float(predictions[i]))

        #==============================================================================================================
        #                             THIS SHOULD BE COMMON IN TENSORFLOW/TF-LITE/ONNX
        #==============================================================================================================
        #Gather our numpy array output in the form of a labeled dictionary 
        if (self.useOutputLimits):
         #Take into account output offsets/scaling
         for i in range (len(self.outputs)):
            #This should be the exact oposite of the operation in readCSV.py line 550
            recoveredValue = (float(outputValueList[i]) * float(self.outputScalars[i])) + float(self.outputOffsets[i])
            #---------------------------------------------------------------
            if (recoveredValue > self.outputMaxima[i]):
                 recoveredValue = self.outputMaxima[i]
            if (recoveredValue < self.outputMinima[i]):
                 recoveredValue = self.outputMinima[i]
            #---------------------------------------------------------------
            element                          = self.outputs[i]
            self.output[element]             = recoveredValue  
            self.outputMinimumValue[element] = float(self.outputMinima[i])
            self.outputMaximumValue[element] = float(self.outputMaxima[i])
            #---------------------------------------------------------------
        else:
         #Not using limits
         for i in range (len(self.outputs)):
            element                          = self.outputs[i]
            self.output[element]             = float(outputValueList[i])
            self.outputMinimumValue[element] = float(self.outputMinima[i])
            self.outputMaximumValue[element] = float(self.outputMaxima[i])
        #==============================================================================================================
        #==============================================================================================================



        self.frameNumber = self.frameNumber + 1
        return self.output
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
class PoseNETONNX():
  def __init__(
               self,
               modelPath:str="movenet/model.onnx",
               targetWidth  = 192,
               targetHeight = 192,
               trainingWidth  = 1920,
               trainingHeight = 1080,
              ):
               #Tensorflow attempt to be reasonable
               #------------------------------------------
               from holisticPartNames import getPoseNETBodyNameList
               self.jointNames = getPoseNETBodyNameList()
               #------------------------------------------
               import onnxruntime as ort
               import onnx
               onnxModelForCheck  = onnx.load(modelPath)
               onnx.checker.check_model(onnxModelForCheck)
               print("ONNX devices available : ", ort.get_device()) 
               providers               = ['CPUExecutionProvider']
               #providers               = ['CUDAExecutionProvider']
               self.options = ort.SessionOptions()
               self.model              = ort.InferenceSession(modelPath, providers=providers, sess_options=self.options)
               for i in range(0,len(self.model.get_inputs())): 
                  print("ONNX INPUTS ",self.model.get_inputs()[i].name)
                  self.inputName = self.model.get_inputs()[i].name

               self.model_input_name   = self.model.get_inputs()
               #------------------------------------------
               self.output         = dict()
               self.hz             = 0.0
               self.targetWidth    = targetWidth
               self.targetHeight   = targetHeight
               self.trainingWidth  = trainingWidth
               self.trainingHeight = trainingHeight
               #------------------------------------------

  def get2DOutput(self):
        return self.output

  def convertImageToMocapNETInput(self,image,doFlipX=False,threshold=0.05):
        import tensorflow as tf
        import numpy as np
        import time
        import cv2
        sourceWidth  = image.shape[1]
        sourceHeight = image.shape[0]
        currentAspectRatio=self.targetWidth/self.targetHeight
        trainedAspectRatio=self.trainingWidth/self.trainingHeight
        #Do resize on OpenCV end
        #----------------------------------------------------------------
        from tools import img_resizeWithCrop,normalizedCoordinatesAdaptForVerticalImage,normalizedCoordinatesAdaptToResizedCrop
        imageTransformed = img_resizeWithCrop(image,self.targetWidth,self.targetHeight)
        #imageTransformed = img_resizeWithPadding(image,self.targetWidth,self.targetHeight)
        imageTransformed = cv2.cvtColor(imageTransformed,cv2.COLOR_BGR2RGB)
        #----------------------------------------------------------------

        #Hand image to Tensorflow
        #----------------------------------------------------------------
        imageONNX = np.expand_dims(imageTransformed, axis=0)
        #-------------------------------------------------------------------


        start = time.time()
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        thisInputONNX = { self.inputName : imageONNX.astype('int32')}
        #Run input through MocapNET
        output_names_onnx = [otp.name for otp in self.model.get_outputs()]
        keypoints_with_scores = self.model.run(output_names_onnx,thisInputONNX)[0][0]
        predictions = keypoints_with_scores[0]
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        seconds = time.time() - start
        self.hz  = 1 / (seconds+0.0001)
        #print("MoveNET ONNX Framerate : ",round(self.hz,2)," fps          ")
        

        currentAspectRatio=sourceWidth/sourceHeight #We "change" aspect ratio by restoring points
        for pointID in range(0,len(predictions)):
           #Joints have y,x,acc order
           nX,nY = normalizedCoordinatesAdaptToResizedCrop(sourceWidth,sourceHeight,self.targetWidth,self.targetHeight,predictions[pointID][1],predictions[pointID][0])
           nX,nY = normalizedCoordinatesAdaptForVerticalImage(sourceWidth,sourceHeight,self.trainingWidth,self.trainingHeight,nX,nY)
           predictions[pointID][1]=nX
           predictions[pointID][0]=nY

        from holisticPartNames import processPoseNETLandmarks
        self.output = processPoseNETLandmarks(self.jointNames,predictions,currentAspectRatio,trainedAspectRatio,threshold=threshold,doFlipX=doFlipX)
        #----------------------------------------------------------------
        from MocapNETVisualization import drawPoseNETLandmarks
        self.image = drawPoseNETLandmarks(predictions,image,threshold=threshold,jointLabels=self.jointNames)
        #----------------------------------------------------------------

        return self.output,image
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
class MocapNETONNX():
  def __init__(self	):
               print("""
 ██████╗ ███╗   ██╗███╗   ██╗██╗  ██╗
██╔═══██╗████╗  ██║████╗  ██║╚██╗██╔╝
██║   ██║██╔██╗ ██║██╔██╗ ██║ ╚███╔╝ 
██║   ██║██║╚██╗██║██║╚██╗██║ ██╔██╗ 
╚██████╔╝██║ ╚████║██║ ╚████║██╔╝ ██╗
 ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═══╝╚═╝  ╚═╝""")
               self.sess_options = ort.SessionOptions()
               self.sess_options.log_severity_level = 3 #<- log_level
               self.sess_options.intra_op_num_threads = 4
               #self.sess_options.execution_mode = ort.ExecutionMode.ORT_SEQUENTIAL
               self.sess_options.execution_mode = ort.ExecutionMode.ORT_PARALLEL
               self.sess_options.inter_op_num_threads = 4
               self.sess_options.graph_optimization_level = ort.GraphOptimizationLevel.ORT_ENABLE_ALL
 

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
    mnet = MocapNETONNX()
    print("Survived Test!")
#-------------------------------------------------------------------------------------------------------------------------------------------------------
