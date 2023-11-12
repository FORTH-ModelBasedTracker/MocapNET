#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import tensorflow as tf
import os
import sys
import time

#-------------------------------------------------------------------------------------------
from readCSV  import parseConfiguration,parseConfigurationInputJointMap,transformNetworkInput,initializeDecompositionForExecutionEngine,readGroundTruthFile,readCSVFile,parseOutputNormalization
from NSDM     import NSDMLabels,createNSDMUsingRules,inputIsEnoughToCreateNSDM,performNSRMAlignment
from EDM      import EDMLabels,createEDMUsingRules
from tools    import bcolors,checkIfFileExists,readListFromFile,convertListToLowerCase,secondsToHz,getEntryIndexInList,parseSerialNumberFromSummary
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
class MocapNETTFLiteSubProblem():
  def __init__(self,
               context,
               configPath:str,
               modelPath:str,
               partName:str,
               numberOfThreads = 4,
               completelyDisablePCACode = 0
              ):
               #-------------------------------------------------------------------------------
               self.useOutputLimits = True #Careful, this should always be on!
               self.partName      = partName
               self.configPath    = configPath
               self.configuration = parseConfiguration(configPath)
               self.part          = self.configuration["OutputDirectory"] 
               self.inputName     = "input_all"
               self.modelPath     = modelPath
               self.modelDirectory= os.path.dirname(self.modelPath)
               self.frameNumber     = 0
               #-------------------------------------------------------------------------------
               #The default compatibility setting is the BMVC2019 2channel NSDM, however nowadays we use NSRM
               numberOfChannelsPerNSDMElement=2
               if (self.configuration['NSDMAlsoUseAlignmentAngles']==1):
                  numberOfChannelsPerNSDMElement=1
               print("Number of Channels Per NSDM element ",numberOfChannelsPerNSDMElement)
               if ("eigenPoses" in self.configuration): 
                       if (self.configuration['eigenPoses']==1): 
                          self.configuration['eigenPoseData'] = readGroundTruthFile(
                                                                self.configuration,
                                                                "Eigenposes",
                                                                "%s/2d_%s_eigenposes.csv"  % (os.path.dirname(self.modelPath),self.partName ),
                                                                "%s/%s_%s_eigenposes.csv"  % (os.path.dirname(self.modelPath),self.configuration['outputMode'],self.partName), #configuration['outputMode'] is either bvh or 3d
                                                                1.0,
                                                                numberOfChannelsPerNSDMElement,
                                                                0,#useRadians,
                                                                0,#useHalfFloats
                                                                externalDecomposition=self.decompositionEngine
                                                               )
               #-------------------------------------------------------------------------------
               self.model         =  tf.lite.Interpreter(
                                                          model_path=self.modelPath, 
                                                          num_threads=numberOfThreads
                                                        )

               self.model.allocate_tensors() 
               self.input_details  = self.model.get_input_details()
               self.output_details = self.model.get_output_details()

               # check the type of the input tensor
               self.floating_model = self.input_details[0]['dtype'] == np.float32
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
               self.outputScalars      = [1.0] * len(self.outputs)
               self.outputOffsets      = [0.0] * len(self.outputs)
               self.outputMinima       = [-6000.0] * len(self.outputs) #huge limit that essentially doesn't limit anything
               self.outputMaxima       = [6000.0]  * len(self.outputs) #huge limit that essentially doesn't limit anything
               #-------------------------------------------------------------------------------
               self.outputOffsets = parseOutputNormalization(self.modelDirectory,"/outputOffsets.csv",self.outputs,self.outputOffsets)
               #self.outputScalars      = parseOutputNormalization(self.modelDirectory,"/outputScalars.csv",self.outputs,self.outputScalars)
               #for jointID in range(0,len(self.outputs)):
               #    self.outputScalars[jointID] = 1 / float(self.outputScalars[jointID])
               self.outputScalars = parseOutputNormalization(self.modelDirectory,"/outputScalarsFraction.csv",self.outputs,self.outputScalars)
               self.outputMinima  = parseOutputNormalization(self.modelDirectory,"/outputMinima.csv",self.outputs,self.outputMinima)
               self.outputMaxima  = parseOutputNormalization(self.modelDirectory,"/outputMaxima.csv",self.outputs,self.outputMaxima)
               #-------------------------------------------------------------------------------
               if (self.outputs[0]=="depth"):
                     self.outputs[0]="hip_zposition"
               #-------------------------------------------------------------------------------
               print("Output Mapping :")
               for jointID in range(0,len(self.outputs)):
                   #self.outputScalars[jointID] = 1 / float(self.outputScalars[jointID])
                   #print("Output ",self.outputs[jointID]," min ",self.outputMinima[jointID]," max ",self.outputMaxima[jointID]," scalar ",self.outputScalars[jointID]," offset ",self.outputOffsets[jointID])
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
               print("\n\n")
               print("Model Dir :",self.modelDirectory)
               print("Inputs :",self.inputs)
               print("Outputs :",self.outputs)
               #-------------------------------------------------------------------------------

  def getModel(self):
        return self.model

  def getModelFlops(self):
    print("TF-Lite has no flops calculator")
    return 0

  def getModelParameters(self):
    model = self.model
    #concrete_func = model.signatures["serving_default"]    
    #print( concrete_func.inputs[0] )
    #print( concrete_func.inputs[0].shape )
    #inputShape = str(concrete_func.inputs[0].shape)
    #inputShape = inputShape.strip("() ")
    #inputShape = inputShape.replace(",", "x")
    #inputShape = inputShape.replace("None", "1")
    #inputShape = inputShape.strip(' ')
    #print("Input Shape is : ",inputShape)
    #------------------------------------------
    totalParameters = 0
    try:
      trainableParams     = np.sum([np.prod(v.get_shape()) for v in model.trainable_weights])
      totalParameters     = int(totalParameters + nonTrainableParams)
    except:
      print("Could not get model trainable parameters for TF-Lite model..!")

    try: 
      nonTrainableParams  = np.sum([np.prod(v.get_shape()) for v in model.non_trainable_weights])
      totalParameters     = int(totalParameters + nonTrainableParams)
    except:
      print("Could not get model non-trainable parameters for TF-Lite model..!")


    return totalParameters


  def test(self):
        #-------------------------------------------
        emptyList = [0.0] * len(self.inputsWithNSRM)
        emptyInput =np.asarray([emptyList],dtype=np.float32)
        #-------------------------------------------
        #print("Running zeros ")
        self.model.set_tensor(self.input_details[0]['index'],emptyInput)
        self.model.invoke()
        predictions = self.model.get_tensor(self.output_details[0]['index'])
        #-------------------------------------------
        return 1

  def prepareInput(self,input2D :dict,configuration : dict):
        from readCSV import prepareInputG
        thisFullInput, self.NSRM,  thisInput, angleToRotate, missingRatio = prepareInputG(input2D,configuration,self.inputs,self.inputsWithNSRM,self.part,self.decompositionEngine,self.disablePCACode)

        inputReadyForTF = np.asarray([thisFullInput],dtype=np.float32)
        return inputReadyForTF,missingRatio


  """
             Convert a dictionary of 2D inputs to MocapNET output
       (Whatever that maybe [it is listed in self.inputs and self.outputs])
  """
  def predict(self,input2D :dict):

        self.inputReadyForTF,missingRatio  = self.prepareInput(input2D,self.configuration)

        if (missingRatio>0.3):
           print("Not running ",self.partName," due to missing joints ")
           return self.output

        
        #Turns out on some decompositions like FastICA there are a lot of zeros!
        #-----------------------------------------------
        #Save cycles by not executing an empty data blob
        #-----------------------------------------------
        self.incompleteInput = 0 #<- This needs to be set to 0 to mark input is received..!
        #totalData = 1
        #zeroData  = 0
        #for element in self.inputReadyForTF[0].tolist(): 
        #    #print("ELEMENT ",element)
        #    totalData = totalData + 1
        #    if ( (element<0.005) and (element>-0.005) ):
        #       zeroData=zeroData + 1
        #missingRatio = zeroData/totalData
        #print("Missing Ratio : ",missingRatio)
        #if (missingRatio>0.4):
        #   print(bcolors.FAIL,"Not executing NN with empty data ",bcolors.ENDC)
        #   #Reset armature..!
        #   for k in self.output.keys():
        #       self.output[k]=0.0
        #   self.incompleteInput = 1
        #   return self.output 
        #-----------------------------------------------

        self.model.set_tensor(self.input_details[0]['index'],self.inputReadyForTF)
        self.model.invoke()
        predictions = self.model.get_tensor(self.output_details[0]['index'])

        self.output = dict()
        if (len(predictions[0])!=len(self.outputs)):
           print(bcolors.FAIL,"Something bad happened.. the ",self.partName," network regressed a different number of parameters (",len(predictions[0]),") than what we expected (",len(self.outputs),") ",bcolors.ENDC)
           raise IOError
           return self.output
        

        #Values to list..
        outputValueList = list()

        for i in range (len(self.outputs)):
            outputValueList.append(float(predictions[0][i]))

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
#-------------------------------------------------------------------------------------------------------------------------------------------------------
class PoseNETTFLite():
  def __init__(
               self,
               modelPath:str="movenet/lite-model_movenet_singlepose_lightning_tflite_int8_4.tflite",
               targetWidth  = 192,
               targetHeight = 192,
               numberOfThreads = 4,
               trainingWidth  = 1920,
               trainingHeight = 1080,
              ):
               #Tensorflow attempt to be reasonable
               #------------------------------------------
               from holisticPartNames import getPoseNETBodyNameList
               self.jointNames = getPoseNETBodyNameList()
               #------------------------------------------
               import tensorflow as tf
               # Initialize the TFLite interpreter
               self.interpreter = tf.lite.Interpreter(model_path=modelPath,num_threads=numberOfThreads)
               self.interpreter.allocate_tensors()
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

        #Prepare image for Tensorflow
        #----------------------------------------------------------------
        imageTF     = np.expand_dims(imageTransformed, axis=0).astype('int32')
        #----------------------------------------------------------------

        # TF Lite format expects tensor type of float32.
        input_image    = tf.cast(imageTF, dtype=tf.uint8) # tf.float32
        input_details  = self.interpreter.get_input_details()
        output_details = self.interpreter.get_output_details()
        #-------------------------------------------------------------------


        start = time.time()
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        self.interpreter.set_tensor(input_details[0]['index'], input_image.numpy())
        self.interpreter.invoke()
 
        keypoints_with_scores = self.interpreter.get_tensor(output_details[0]['index']) # Output is a [1, 1, 17, 3] numpy array.
        predictions = keypoints_with_scores[0][0]
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        seconds = time.time() - start
        self.hz  = 1 / (seconds+0.0001)
        #print("MoveNET TFLite Framerate : ",round(self.hz,2)," fps          ")

        
        currentAspectRatio=sourceWidth/sourceHeight #We "change" aspect ratio by restoring points
        for pointID in range(0,len(predictions)):
           #Joints have y,x,acc order
           nX,nY = normalizedCoordinatesAdaptToResizedCrop(sourceWidth,sourceHeight,self.targetWidth,self.targetHeight,predictions[pointID][1],predictions[pointID][0])
           nX,nY = normalizedCoordinatesAdaptForVerticalImage(sourceWidth,sourceHeight,self.trainingWidth,self.trainingHeight,nX,nY)
           predictions[pointID][1]=nX
           predictions[pointID][0]=nY

        from holisticPartNames import processPoseNETLandmarks
        self.output = processPoseNETLandmarks(self.jointNames,predictions,currentAspectRatio,trainedAspectRatio,threshold=threshold,doFlipX=doFlipX)
        #------------------------------------------------
        from MocapNETVisualization import drawPoseNETLandmarks
        self.image = drawPoseNETLandmarks(predictions,image,threshold=threshold,jointLabels=self.jointNames)
        #------------------------------------------------

        return self.output,image
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------




#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------

class MocapNETTFLite():
  def __init__(self,):
               print("""
████████╗███████╗    ██╗     ██╗████████╗███████╗
╚══██╔══╝██╔════╝    ██║     ██║╚══██╔══╝██╔════╝
   ██║   █████╗█████╗██║     ██║   ██║   █████╗  
   ██║   ██╔══╝╚════╝██║     ██║   ██║   ██╔══╝  
   ██║   ██║         ███████╗██║   ██║   ███████╗
   ╚═╝   ╚═╝         ╚══════╝╚═╝   ╚═╝   ╚══════╝""")
               #-------------------------------------------------------------------------------
               #do nothing :P 
               #-------------------------------------------------------------------------------
 
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
    mnet = MocapNETTFLite()
    print("Survived Test!")
#-------------------------------------------------------------------------------------------------------------------------------------------------------
