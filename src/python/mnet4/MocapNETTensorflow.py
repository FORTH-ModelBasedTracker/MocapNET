#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

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

import time
import os
import numpy as np

#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
class MocapNETTensorflowSubProblem():
  def __init__(self,
               context,
               configPath:str,
               modelPath:str,
               partName:str,
               device:str="/device:GPU:0",
               doPerformanceProfiling = False,
               tensorboard            = 0,
               completelyDisablePCACode = 0
              ):
               #-------------------------------------------------------------------------------
               self.useOutputLimits = True #Careful, this should always be on!
               self.partName      = partName
               self.configPath    = configPath
               from readCSV  import parseConfiguration
               self.configuration = parseConfiguration(configPath)
               self.part          = partName#self.configuration["OutputDirectory"]
               self.modelPath     = modelPath
               self.modelDirectory= os.path.dirname(self.modelPath)
               from DNNModel import loadNewModel
               self.model         = loadNewModel(modelPath)
               self.device        = device
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
               import tensorflow as tf
               rmsprop=tf.keras.optimizers.RMSprop(learning_rate=0.002, rho=0.9, epsilon=tf.keras.backend.epsilon())  
               self.model.compile(
                                  optimizer=rmsprop,
                                  loss='mse',
                                  metrics=['mae', 'acc'],
                                  jit_compile=True #<- this may cause trouble on non-XLA builds?
                                 )
               self.modelKeras = self.model
               #print("MocapNET Model for ",partName," has the following signatures ",self.model.signatures)
               self.model    = self.model.signatures['serving_default']

               self.profile       = doPerformanceProfiling
               self.tensorboard   = tensorboard
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
               self.networkInputList  = [0.0] * len(self.inputsWithNSRM)
               self.networkInput = np.asarray([self.networkInputList],dtype=np.float32)
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
               print("Inputs :",self.inputs)
               print("Outputs :",self.outputs)
               #-------------------------------------------------------------------------------

  def getModel(self):
        return self.modelKeras

  def getModelFlops(self):
    import tensorflow as tf
    from tensorflow.python.framework.convert_to_constants import  convert_variables_to_constants_v2_as_graph
    model = self.modelKeras
    concrete = tf.function(lambda inputs: model(inputs))
    concrete_func = concrete.get_concrete_function(
        [tf.TensorSpec([1, *inputs.shape[1:]]) for inputs in model.inputs])
    frozen_func, graph_def = convert_variables_to_constants_v2_as_graph(concrete_func)
    with tf.Graph().as_default() as graph:
        tf.graph_util.import_graph_def(graph_def, name='')
        run_meta = tf.compat.v1.RunMetadata()
        opts = tf.compat.v1.profiler.ProfileOptionBuilder.float_operation()
        flops = tf.compat.v1.profiler.profile(graph=graph, run_meta=run_meta, cmd="op", options=opts)
 
        totalFLOPS = int(flops.total_float_ops) 
        return totalFLOPS
    return 0

  def getModelParameters(self):
    model = self.modelKeras
    #------------------------------------------------------------------------------------------
    trainableParams     = np.sum([np.prod(v.get_shape()) for v in model.trainable_weights])
    nonTrainableParams  = np.sum([np.prod(v.get_shape()) for v in model.non_trainable_weights])
    totalParameters     = int(trainableParams + nonTrainableParams)
    return totalParameters

  def test(self):
        #-------------------------------------------
        emptyList = [0.0] * len(self.inputsWithNSRM)
        emptyInput =np.asarray([emptyList],dtype=np.float32)
        #-------------------------------------------
        #print(bcolors.FAIL,"Running zeros ",bcolors.ENDC)
        import tensorflow as tf
        outputs = self.model(tf.cast(emptyInput,dtype=tf.float32)) #,training=False            
        #print("Outputs : ",outputs)
        #print("Output Keys", outputs.keys())
        outKey = list(outputs.keys())[0]
        outputsRaw = outputs[outKey]
        predictions = outputsRaw

        #predictions = self.model(emptyInput,training=False) 
        #print("MocapNET result = ",predictions)
        #-------------------------------------------
        return 1

  def prepareInput(self,input2D :dict,configuration : dict):
        from readCSV import prepareInputG
        thisFullInput, self.NSRM, thisInput, angleToRotate, missingRatio = prepareInputG(input2D,configuration,self.inputs,self.inputsWithNSRM,self.part,self.decompositionEngine,self.disablePCACode)

        #i=0
        #for value in thisFullInput: 
        #   self.networkInput[i]=value
        #   i=i+1
        #import tensorflow as tf
        #self.networkInput = tf.convert_to_tensor(self.networkInputNumpy)
        self.networkInput = np.asarray([thisFullInput],dtype=np.float32)
        return self.networkInput,missingRatio
        #-----------------------------  
        #inputReadyForTF = np.asarray([thisFullInput],dtype=np.float32)
        #return inputReadyForTF


  """
             Convert a dictionary of 2D inputs to MocapNET output
       (Whatever that maybe [it is listed in self.inputs and self.outputs])
  """
  def predict(self,input2D :dict):

        #This call works @ 400Hz
        #--------------------------------------------------------------------------------------
        self.inputReadyForTF,missingRatio = self.prepareInput(input2D,self.configuration)


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
        #
        #   self.incompleteInput = 1
        #   return self.output
        #--------------------------------------------------------------------------------------
        predictions = list()  
        self.output = dict()      


        #--------------------------------------------------------------------------------------
        if (self.profile): 
            #Run input through MocapNET and Profile code (slower)
            print(bcolors.WARNING,"WARNING: Profiling NN enabled, execution will be slower",bcolors.ENDC) 
            predictions = self.modelKeras.predict(self.inputReadyForTF,callbacks = [self.tensorboard])  
        else:
            #As stated in https://github.com/keras-team/keras/blob/v2.8.0/keras/engine/training.py#L1825-L2012 : 
            # and https://keras.io/getting_started/faq/#whats-the-difference-between-model-methods-predict-and-call
            import tensorflow as tf
            with tf.device(self.device):
            #with tf.device('/device:CPU:0'):
               inferenceOutputs = self.model(tf.cast(self.inputReadyForTF,dtype=tf.float32)) # ,training=False We shouldn't run predict to get as fast results as possible
               #print("Outputs : ",outputs)
               #print("Output Keys", outputs.keys())
               outKey = list(inferenceOutputs.keys())[0]
               outputsRaw = inferenceOutputs[outKey]
               #outputsRaw = outputs['result_all']
               predictions = outputsRaw
        #--------------------------------------------------------------------------------------


        if (len(predictions)>0):
          if(len(predictions[0])!=len(self.outputs)):
            print(bcolors.FAIL,"Something bad happened.. the network regressed a different number of parameters than what we expected",bcolors.ENDC)
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
class PoseNET():
  def __init__(
               self,
               modelPath:str="movenet/",
               targetWidth  = 192,
               targetHeight = 192,
               trainingWidth  = 1920,
               trainingHeight = 1080,
              ):
               #Tensorflow attempt to be reasonable
               #------------------------------------------
               from holisticPartNames import getPoseNETBodyNameList
               self.jointNames = getPoseNETBodyNameList()
               import tensorflow as tf
               self.model      = tf.saved_model.load(modelPath)
               self.movenet    = self.model.signatures['serving_default']
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


        start = time.time()
        # TF Lite format expects tensor type of float32.
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        outputs = self.movenet(tf.cast(imageTF, dtype=tf.int32)) 
        keypoints_with_scores = outputs['output_0']
        predictionsRaw = keypoints_with_scores[0][0]
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        seconds = time.time() - start
        self.hz  = 1 / (seconds+0.0001)
        #print("MoveNET Framerate : ",round(self.hz,2)," fps          ")


        currentAspectRatio=sourceWidth/sourceHeight #We "change" aspect ratio by restoring points
        predictions=list()
        for pointID in range(0,len(predictionsRaw)):
           #Joints have y,x,acc order
           thisPoint = list()
           thisPoint.append(float(predictionsRaw[pointID][0])) #y
           thisPoint.append(float(predictionsRaw[pointID][1])) #x
           thisPoint.append(float(predictionsRaw[pointID][2])) #score
           nX,nY = normalizedCoordinatesAdaptToResizedCrop(sourceWidth,sourceHeight,self.targetWidth,self.targetHeight,thisPoint[1],thisPoint[0])
           nX,nY = normalizedCoordinatesAdaptForVerticalImage(sourceWidth,sourceHeight,self.trainingWidth,self.trainingHeight,nX,nY)
           thisPoint[0]= nY #Just update coords
           thisPoint[1]= nX #Just update coords
           predictions.append(thisPoint)
        #-------------------------------------------------------------------

        from holisticPartNames import processPoseNETLandmarks
        self.output = processPoseNETLandmarks(self.jointNames,predictions,currentAspectRatio,trainedAspectRatio,threshold=threshold,doFlipX=doFlipX)

        from MocapNETVisualization import drawPoseNETLandmarks
        self.image = drawPoseNETLandmarks(predictions,image,threshold=threshold,jointLabels=self.jointNames)        
        #------------------------------------------------

        return self.output,image
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------------------------------------------------

def get_available_devices():
    from tensorflow.python.client import device_lib
    local_device_protos = device_lib.list_local_devices()
    return [x.name for x in local_device_protos if x.device_type == 'GPU' or x.device_type == 'CPU']

class MocapNETTensorflow():
  def __init__(self,
               doPerformanceProfiling = False,
              ):
               print("""
████████╗███████╗███╗   ██╗███████╗ ██████╗ ██████╗ ███████╗██╗      ██████╗ ██╗    ██╗
╚══██╔══╝██╔════╝████╗  ██║██╔════╝██╔═══██╗██╔══██╗██╔════╝██║     ██╔═══██╗██║    ██║
   ██║   █████╗  ██╔██╗ ██║███████╗██║   ██║██████╔╝█████╗  ██║     ██║   ██║██║ █╗ ██║
   ██║   ██╔══╝  ██║╚██╗██║╚════██║██║   ██║██╔══██╗██╔══╝  ██║     ██║   ██║██║███╗██║
   ██║   ███████╗██║ ╚████║███████║╚██████╔╝██║  ██║██║     ███████╗╚██████╔╝╚███╔███╔╝
   ╚═╝   ╚══════╝╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚══════╝ ╚═════╝  ╚══╝╚══╝""")
               self.doPerformanceProfiling = doPerformanceProfiling

               #Tensorflow attempt to be reasonable
               #------------------------------------------
               import gc
               gc.collect() #Do garbage collection before allocating TF stuff
               import os
               os.environ['TF_ENABLE_GPU_GARBAGE_COLLECTION']='false'
               #Make sure CUDA cache is not disabled!
               os.environ['CUDA_CACHE_DISABLE'] = '0'
               #Try to presist cudnn 
               os.environ['TF_USE_CUDNN_BATCHNORM_SPATIAL_PERSISTENT'] = '1'
               #Try to allocate as little memory as possible
               os.environ['TF_FORCE_GPU_ALLOW_GROWTH'] = 'true'
               #Use seperate threads so execution is not throttled by CPU
               os.environ['TF_GPU_THREAD_MODE'] = 'gpu_private'
               #0 = all messages are logged (default behavior)
               #1 = INFO messages are not printed
               #2 = INFO and WARNING messages are not printed
               #3 = INFO, WARNING, and ERROR messages are not printed
               os.environ['TF_CPP_MIN_LOG_LEVEL'] = '0'
               #improve the stability of the auto-tuning process used to select the fastest convolution algorithms
               os.environ['TF_AUTOTUNE_THRESHOLD'] = '1'
               #------------------------------------------
               import tensorflow as tf
               devices = get_available_devices()
               print("Available Tensorflow devices are : ",devices)
               self.device = '/device:CPU:0'
               for device in devices:
                   if (device.find("GPU")!=-1):
                       self.device = device 
               print("Selecting device : ",self.device)

               #If enabled, an op will be placed on CPU if any of the following are true 
               #1 - there's no GPU implementation for the OP
               #2 - no GPU devices are known or registered
               #3 - need to co-locate with reftype input(s) which are from CPU
               tf.config.set_soft_device_placement(True)
               
               #Only give the warning when not profiling otherwise we will get an error!
               if (not doPerformanceProfiling):
                  tf.config.experimental.set_device_policy('explicit')

               try:
                 tf.config.run_functions_eagerly(True) 
                 tf.config.experimental.set_synchronous_execution(False)
               except:
                 #Invalid device or cannot modify virtual devices once initialized.
                 pass

               try:
                 physical_devices = tf.config.list_physical_devices('CPU')
                 tf.config.experimental.set_memory_growth(physical_devices[0], True)
                 physical_devices = tf.config.list_physical_devices('GPU')
                 tf.config.experimental.set_memory_growth(physical_devices[0], True)
               except:
                 #Invalid device or cannot modify virtual devices once initialized.
                 pass
               
               try: 
                 tf.config.threading.set_intra_op_parallelism_threads(8)
                 tf.config.threading.set_inter_op_parallelism_threads(8)
               except:
                 #Most probably : RuntimeError: Intra op parallelism cannot be modified after initialization
                 pass

               if (doPerformanceProfiling):
                  import tensorflow as tf
                  self.tensorboard = tf.keras.callbacks.TensorBoard(log_dir = "profiling",histogram_freq = 1)
                  #tensorboard --bind_all --logdir profiling
                  from DNNModel import startProfiling
                  startProfiling()
               else:
                  self.tensorboard=0
               

  def __del__(self):
        if (self.doPerformanceProfiling):
                  from DNNModel import stopProfiling
                  stopProfiling()
                  print("To see profile results \nUse :\n tensorboard --logdir profiling ")
        print('TFLite stopped.')
 
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
    mnet = MocapNETTensorflow()
    print("Survived Test!")
#-------------------------------------------------------------------------------------------------------------------------------------------------------
