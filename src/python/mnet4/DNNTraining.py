#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""


import os 
import sys
import gc
import time
import json


import tensorflow as tf
from tensorflow import keras

#from tensorflow.keras.backend.tensorflow_backend import set_session

#from tensorflow.keras import backend as K
from tensorflow.keras.layers import Input, Dense
from tensorflow.keras.models import Model
from tensorflow.keras.models import Sequential
from tensorflow.keras.models import model_from_json
from tensorflow.keras.utils import plot_model

import tensorflow.keras.callbacks
import numpy as np
from numba import njit #Test
from tools import bcolors,checkIfFileExists

def printTrainingVersion():
       print("""
███╗   ███╗ ██████╗  ██████╗ █████╗ ██████╗ ███╗   ██╗███████╗████████╗
████╗ ████║██╔═══██╗██╔════╝██╔══██╗██╔══██╗████╗  ██║██╔════╝╚══██╔══╝
██╔████╔██║██║   ██║██║     ███████║██████╔╝██╔██╗ ██║█████╗     ██║   
██║╚██╔╝██║██║   ██║██║     ██╔══██║██╔═══╝ ██║╚██╗██║██╔══╝     ██║   
██║ ╚═╝ ██║╚██████╔╝╚██████╗██║  ██║██║     ██║ ╚████║███████╗   ██║   
╚═╝     ╚═╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝╚══════╝   ╚═╝   
       """)                                                                       
       #-----------------------------
       #BMVC 21 paper was submitted with Keras 2.2.4/Tensorflow 1.14/Numpy 1.16.2
       #-----------------------------
       print("")
       print("Tensorflow version : ",tf.__version__)
       #print("Keras version      : ",keras.__version__) <- no longer available in TF-2.13
       print("Numpy version      : ",np.__version__)
       #-----------------------------
       from tensorflow.python.platform import build_info as tf_build_info
       print("TF/CUDA version    : ",tf_build_info.build_info['cuda_version'])
       print("TF/CUDNN version   : ",tf_build_info.build_info['cudnn_version'])
       #-----------------------------
       print("")
       #-----------------------------

def forceCPU():
       print(bcolors.WARNING,"User selected to use CPU mode",bcolors.ENDC)
       os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"   # see issue #152
       os.environ["CUDA_VISIBLE_DEVICES"] = ""

def limitGPUMemory():
       print("Limiting GPU memory usage for multiple instances..\n")
       config = tf.compat.v1.ConfigProto()
       config.gpu_options.per_process_gpu_memory_fraction = 0.2
       config.gpu_options.visible_device_list = "0"
       set_session(tf.compat.v1.Session(config=config))

def setTensorflowBackendToHalfFloats():
   dtype='float16'
   
   #WARNING:tensorflow:Mixed precision compatibility check (mixed_float16): WARNING
   #Your GPU may run slowly with dtype policy mixed_float16 because it does not have compute capability
   #of at least 7.0. Your GPU: Quadro P6000, compute capability 6.1

   from tensorflow.keras.mixed_precision import experimental as mixed_precision
   policy = mixed_precision.Policy('mixed_float16')
   mixed_precision.set_policy(policy)

   tf.keras.backend.set_floatx(dtype)
   # default is 1e-7 which is too small for float16.  Without adjusting the epsilon, we will get NaN predictions because of divide by zero problems
   tf.keras.backend.set_epsilon(1e-4)

   print('Compute dtype: %s' % policy.compute_dtype)
   print('Variable dtype: %s' % policy.variable_dtype)
   print("\nUsing Half-Floats for training ")


def getRSquared(groundTruth, neuralNetworkOutput):
    """
    Computes the R-squared metric.
    https://en.wikipedia.org/wiki/Coefficient_of_determination

    Parameters:
        -- groundTruth (numpy.ndarray): 1D array representing the ground truth.
        -- neuralNetworkOutput (numpy.ndarray): 1D array representing the neural network output.

    Returns:
        -- float: R-squared metric.
    """

    # Compute the mean of the ground truth
    mean_ground_truth = np.mean(groundTruth)

    # Compute the sum of squares of the residuals
    ss_residuals = np.sum((groundTruth - neuralNetworkOutput) ** 2)

    # Compute the sum of squares of the total variation
    ss_total = np.sum((groundTruth - mean_ground_truth) ** 2)

    # Compute the R-squared metric
    r_squared = 1 - (ss_residuals / ss_total)

    return r_squared



@njit
def getRSquaredInPlace(groundTruth,mean_ground_truth,N,neuralNetworkOutput):
    """
    Computes the R-squared metric.
    https://en.wikipedia.org/wiki/Coefficient_of_determination

    Parameters:
        -- groundTruth (numpy.ndarray): 1D array representing the ground truth.
        -- neuralNetworkOutput (numpy.ndarray): 1D array representing the neural network output.

    Returns:
        -- float: R-squared metric.
    """

    #N = groundTruth.shape[0]
    if N == 0:
      return np.NAN

    # Compute the mean of the ground truth
    #mean_ground_truth = np.mean(groundTruth)

    ss_residuals = np.float32(0.0)
    ss_total     = np.float32(0.0)

    for i in range(N):
        #--------------------------------------
        gt            = np.float32(groundTruth[i][0])         #groundTruth is a tensorflow array so each element is also an array -> thus we need [0]
        nn_out        = np.float32(neuralNetworkOutput[i][0]) #neuralNetworkOutput is a tensorflow array so each element is also an array -> thus we need [0]
        #--------------------------------------
        delta         = gt - nn_out
        ss_residuals += delta ** 2
        #--------------------------------------
        delta         = gt - mean_ground_truth
        ss_total     += delta ** 2

    # Compute the R-squared metric
    r_squared = np.float32(1 - (ss_residuals / ss_total))

    return r_squared




def getRSquaredForNeuralNetwork(mocapNETNetwork,networkInput,groundtruthOutput):
    import time
    startAt = time.time()
    #-----------------------------------------------------
    try:
        print("Extracting R² : Getting neural network response..")
        predictions = mocapNETNetwork.predict(networkInput)
        #Since the groundtruth is very large and the machines I use don't have
        #enough RAM this is an in place R^2 calculation to make sure the computer doesn't enter a page swapping 
        #loop that makes training grind to a halt..! 
        print("Extracting R² : Calculating..")

        mean_ground_truth = np.float32(np.mean(groundtruthOutput))
        #print("groundtruthOutput: ",groundtruthOutput)
        rSQ = getRSquaredInPlace(groundtruthOutput,mean_ground_truth,groundtruthOutput.shape[0],predictions)
    
        #This is faster (if you have spare memory..)
        #rSQ = getRSquared(groundtruthOutput,predictions)
        del predictions
    #except:
    except Exception as e: 
        print("Failed to extract R² ..")
        print("Error was : ",e)
        rSQ = np.NAN
    #-----------------------------------------------------    
    #-----------------------------------------------------
    endAt = time.time()
    print("R² = ",rSQ," it took ",(endAt-startAt)/60," mins to compute")
    return rSQ


def logTrainingResults( 
                       filename,
                       configuration,
                       outputType,
                       outputName,
                       history,
                       metrics
                      ):
   if (outputType==0):
       numberOfModelParametersStr="?"
       if 'modelParameters' in configuration:
             numberOfModelParametersStr="%u" % int(configuration['modelParameters']) 

       #First record - Start of File
       inputProcessing=""
       if (configuration['eNSRM']):
           inputProcessing=inputProcessing+" eNSRM"
       if ('NSRMNormalizeAngles' in configuration) and (configuration['NSRMNormalizeAngles']):
          if (configuration['NSRMNormalizeAngles']==1):
           inputProcessing=inputProcessing+" NSRMNormalizeAngles" #Half
          elif (configuration['NSRMNormalizeAngles']==2):
           inputProcessing=inputProcessing+" NSRMNormalizeAnglesFull"
          else:
           inputProcessing=inputProcessing+" NSRMNormalizeAnglesUNKNOWN" 
       if (configuration['EDM']):
           inputProcessing=inputProcessing+" EDM"
       if (configuration['eigenPoses']):
           inputProcessing=inputProcessing+" eigenPoses"
       if (configuration['autobuilder']):
           inputProcessing=inputProcessing+" autobuilder"
       if (  (configuration['decompositionType']!="") and (configuration['doPCA']!="") and ( int(configuration['PCADimensionsKept'])>0 ) ):
           inputProcessing=inputProcessing+" %s(%u)"%(configuration['decompositionType'],configuration['PCADimensionsKept'])
           if (configuration['PCAAlsoKeepRawData']):
              inputProcessing=inputProcessing+"+PCA_RawData"
       inputProcessing=inputProcessing+" act("+configuration['activationFunction']+")"
       inputProcessing=inputProcessing+" rand("+configuration['weightRandomizationFunction']+")"
       inputProcessing=inputProcessing+" "+configuration['optimizer']
       if (configuration['balanced2DInputs']):
           inputProcessing=inputProcessing+" balanced2DInputs"

       if (configuration['useQuadLoss']!=0):
           inputProcessing=inputProcessing+" quadLoss"

       if (configuration['setConstantSeedForReproducibleTraining']!=1):
          inputProcessing=inputProcessing+" random seed"

       if (configuration['include2DInputVisibilityFlags']==0):
          inputProcessing=inputProcessing+" novis2Dflag"

       experimentDescription=""
       experimentDescription=experimentDescription+"group %u"%(configuration["groupOutputs"])

       if (configuration['useRadians']):
           experimentDescription=experimentDescription+" radians"
       #--------------------------------------------------------------------
       if ('outputValueDistribution' in configuration):
                  if (configuration['outputValueDistribution']=='balanced'):
                         experimentDescription=experimentDescription+" balanced offset"
                  elif (configuration['outputValueDistribution']=='positive'):
                         experimentDescription=experimentDescription+" positive offset"
                  elif (configuration['outputValueDistribution']=='negative'):
                         experimentDescription=experimentDescription+" negative offset"
       #--------------------------------------------------------------------
       if ('outputNormalizationStrategy' in configuration):
                         experimentDescription=experimentDescription+" outputNormalizationStrategy="+configuration['outputNormalizationStrategy']
       #--------------------------------------------------------------------
       if (float(configuration['outputMultiplier'])!=1.0):
           experimentDescription=experimentDescription+" outputScaling=%0.2f" % float(configuration['outputMultiplier'])
       experimentDescription=experimentDescription+" NNDepth=%u"%configuration['neuralNetworkDepth']

       #FIRST! RECORD!
       with open(filename, 'w') as f:
            f.write("""
                       <html>
                       <head>
                         <meta charset=\"UTF-8\">
                         <title>Training summary of experiment %s for %s </title>
                       </head>
                       <body>
                       """%(configuration['label'],configuration['hierarchyPartName']))

            #Add the hostname..
            import socket
            f.write("%s<br>\n"%socket.gethostname())
     

            f.write("""
                       <table border=1>
                         <tr>
                           <td>Date</td>
                           <td>Part</td>
                           <td>BatchSize</td>
                           <td>Epochs</td>
                           <td>&lambda;</td>
                           <td>Remember</td>
                           <td>Occlusions</td>
                           <td>Input Processing</td>
                           <td>Hard Mining</td>
                         </tr>
                         <tr>
                           <td>%s</td>
                           <td>%s</td>
                           <td>%u</td>
                           <td>%u/%u/%u</td>
                           <td>%0.2f</td>
                           <td>c %u/p %u</td>
                           <td>%u</td>
                           <td>%s</td>
                           <td></td>
                         </tr>
                       </table>
                    """% (
                          configuration['date'],
                          configuration['hierarchyPartName'],
                          configuration['defaultBatchSize'],
                          configuration["defaultNumberOfEpochs"],configuration["highNumberOfEpochs"],configuration["veryHighNumberOfEpochs"],
                          configuration["lamda"],
                          configuration["rememberConsecutiveWeights"],configuration["rememberWeights"],
                          configuration["ignoreOcclusions"]==0,
                          inputProcessing
                         )  
                   )

            f.write("""
                       <table border=1>
                         <tr>
                           <td>Serial</td>
                           <td>Start</td>
                           <td>End</td>
                           <td>Training Samples</td>
                           <td>Model Size</td>
                           <td>Dropout/L.R</td>
                           <td>Description</td>
                         </tr>
                         <tr>
                           <td>%s</td>
                           <td>%u</td>
                           <td>%u</td>
                           <td>%u</td>
                           <td>%s</td>
                           <td>%0.2f/%0.5f</td>
                           <td>%s</td>
                         </tr>
                       </table>
                       <table border=1>
                             <tr>
                                <td>Joint</td>
                                <td>R² / Start Loss / M.A.E</td>
                                <td>R² / End Loss / M.A.E</td>
                                <td>Tr.Epochs</td> 
                                <td>Min/Max</td>
                                <td>Offset</td>
                                <td>Scalar</td>
                                <td>Scalar Fr.</td>
                             </tr>
                    """% (
                          configuration['label'],
                          configuration['startPosition'],
                          configuration['endPosition'],
                          configuration['trainingSamples'],
                          numberOfModelParametersStr, 
                          configuration['dropoutRate'],
                          configuration['learningRate'],
                          experimentDescription
                         )
                    )
            f.close()
   #----------------------------------

   try: 
    lowestLossAchievedAt = 0
    if ("loss" in history.history) and ("mae" in history.history): 
     # Initialize variables to store the initial and lowest loss
     initial_loss = history.history['loss'][0]
     lowest_loss = initial_loss

     initial_mae = history.history['mae'][0]
     lowest_mae = initial_mae

     # Iterate through the history object
     count = 0
     for i, loss in enumerate(history.history['loss']):
     # Update the lowest loss if a lower value is found
       if loss < lowest_loss:
           lowest_loss = loss
           lowestLossAchievedAt = count
       count = count + 1

     lowest_mae = history.history['mae'][lowestLossAchievedAt]

     #By default don't scale
     #----------------------------------------------------------------------------
     outputMinimumValue         = 0.0
     outputMaximumValue         = 0.0
     outputOffsetValue          = 0.0
     outputScalarValues         = 1.0
     outputScalarFractionValues = 1.0
     #----------------------------------------------------------------------------
     for v in range(0,len(configuration["outputOffsetLabels"])):
         if (outputName.lower()==configuration["outputOffsetLabels"][v].lower()):
              #we found a rule!
              outputMinimumValue         = float(configuration["outputOffsetMinima"][v])
              outputMaximumValue         = float(configuration["outputOffsetMaxima"][v])
              outputOffsetValue          = float(configuration["outputOffsetValues"][v])
              outputScalarValues         = float(configuration["outputScalarValues"][v])
              outputScalarFractionValues = float(configuration["outputScalarFractionValues"][v])
     #----------------------------------------------------------------------------
     initialVAL = ""
     lowestVAL  = ""
     if ("val_loss" in history.history) and ("val_mae" in history.history): 
         initial_VALloss = history.history['val_loss'][0]
         lowest_VALloss  = history.history['val_loss'][lowestLossAchievedAt]
         initial_VALmae  = history.history['val_mae'][0]
         lowest_VALmae   = history.history['val_mae'][lowestLossAchievedAt]
         initial_VALmae  = initial_VALmae * outputScalarFractionValues
         lowest_VALmae   = lowest_VALmae  * outputScalarFractionValues
         #--------------------------------------
         initialVAL      = " - "
         if ("test_rsquared_start" in metrics):
                 initialVAL = " - R² %0.2f/"%(metrics["test_rsquared_start"])
         initialVAL      = initialVAL + "%0.4f/%0.4f" % (initial_VALloss,initial_VALmae)
         #--------------------------------------
         lowestVAL       = " - "
         if ("test_rsquared_end" in metrics):
                 lowestVAL = " - R² %0.2f/"%(metrics["test_rsquared_end"])
         lowestVAL       = lowestVAL  + "%0.4f/%0.4f" % (lowest_VALloss,lowest_VALmae)

     #Every output now gets scaled always
     initial_mae = initial_mae * outputScalarFractionValues
     lowest_mae  = lowest_mae  * outputScalarFractionValues

     with open(filename, 'a') as f:
            f.write("<tr><td>")
            f.write(outputName)
            f.write("</td>")
            #------------------------------------------------
            #Start Loss
            f.write("<td>")
            if ("train_rsquared_start" in metrics):
                 f.write("R² %0.2f/"%(metrics["train_rsquared_start"]))
            f.write("%0.4f/%0.4f%s"%(initial_loss,initial_mae,initialVAL))
            f.write("</td>")
            #------------------------------------------------
            #End Loss
            f.write("<td>")
            if ("train_rsquared_end" in metrics):
                 f.write("R² %0.2f/"%(metrics["train_rsquared_end"]))
            f.write("%0.4f/%0.4f%s"%(lowest_loss,lowest_mae,lowestVAL))
            f.write("</td>")
            #------------------------------------------------

            #Training Epochs ----------------------------
            f.write("<td>")
            f.write("%u"%(lowestLossAchievedAt))
            f.write("</td>")
            #Min/Max ----------------------------
            f.write("<td>")
            f.write("%0.4f/%0.4f"%(outputMinimumValue,outputMaximumValue))
            f.write("</td>")
            #Offset -----------------------------
            f.write("<td>")
            f.write("%0.4f"%(outputOffsetValue))
            f.write("</td>")
            #Scalar -----------------------------
            f.write("<td>")
            f.write("%0.4f"%(outputScalarValues))
            f.write("</td>")
            f.write("<td>")
            f.write("%0.4f"%(outputScalarFractionValues))
            f.write("</td>")
            #------------------------------------------------
            f.write("</tr>")
            f.close()
   except:
    print("An exception occurred in logTrainingResults") 



def regularTraining(tensorboardLabel,mocapNETNetwork,numberOfEpochs,batchSize,earlyStoppingPatience,trainInput,trainOutput,testInput,testOutput,minD,modelIsTrivial=False,haveTestSet=False,useHalfFloats=False):
       #We use a checkpoint system to return best model..
       if (os.path.isfile("bestW.hdf5")):
           print(bcolors.WARNING,"Found a forgotten checkpoint file, erasing it to avoid trouble",bcolors.ENDC)
           os.system('rm bestW.hdf5')
       
       #whatToMonitor='mean_absolute_error'  
       #minimumDelta=0.05 
       whatToMonitor='loss'
       
       metrics = dict()

       if (useHalfFloats):
         print("Early stopping will use MAE instead of loss due to half floats..")
         whatToMonitor='mae'
       minimumDelta=minD
       
       #To see use : 
       #tensorboard --logdir step0_upperbody_all/tensorboard --bind_all
       #------------------------------------------------------------------------
       tensorboard = keras.callbacks.TensorBoard(log_dir=tensorboardLabel,histogram_freq=1)
       #------------------------------------------------------------------------
       earlystopper = keras.callbacks.EarlyStopping(
                                                    monitor=whatToMonitor,
                                                    min_delta=minimumDelta,
                                                    patience=earlyStoppingPatience,
                                                    verbose=1, 
                                                    mode='auto'
                                                   ) 
       #-------------------------------------------------------------------------
       checkpointer = keras.callbacks.ModelCheckpoint(
                                                      filepath="bestW.hdf5",
                                                      monitor=whatToMonitor,
                                                      mode='min',
                                                      verbose=1,
                                                      save_freq='epoch',
                                                      save_best_only=True,
                                                      save_weights_only=True
                                                     )  
       #------------------------------------------------------------------------
       callbacks = [
                    earlystopper,
                    checkpointer,
                    tensorboard
                   ]
       #------------------------------------------------------------------------

       print("regularTraining begins -> BatchSize=%u / NumberOfEpochs=%u "%(batchSize,numberOfEpochs))  
       #------------------------------------------------------------------------
       if (haveTestSet==False):
          #Train without any test data ( probably to conserve memory )
          if (not modelIsTrivial):
             metrics["train_rsquared_start"] = getRSquaredForNeuralNetwork(mocapNETNetwork,trainInput,trainOutput)
          history = mocapNETNetwork.fit(
                                        trainInput,trainOutput,
                                        epochs=numberOfEpochs,
                                        batch_size=batchSize,
                                        shuffle=True,
                                        #steps_per_epoch=5, #<- Use more steps per epoch?
                                        callbacks=callbacks
                                       )
          if (not modelIsTrivial):
             metrics["train_rsquared_end"] = getRSquaredForNeuralNetwork(mocapNETNetwork,trainInput,trainOutput)
       else:
          #Train with test data 
          if (not modelIsTrivial):
             metrics["train_rsquared_start"] = getRSquaredForNeuralNetwork(mocapNETNetwork,trainInput,trainOutput)
             metrics["test_rsquared_start"]  = getRSquaredForNeuralNetwork(mocapNETNetwork,testInput,testOutput)
          history = mocapNETNetwork.fit(
                                        trainInput,trainOutput,
                                        epochs=numberOfEpochs,
                                        batch_size=batchSize,
                                        shuffle=True,
                                        validation_data=(testInput,testOutput),
                                        #validation_split=0.2,
                                        callbacks=callbacks
                                       )
          if (not modelIsTrivial):
             metrics["train_rsquared_end"] = getRSquaredForNeuralNetwork(mocapNETNetwork,trainInput,trainOutput)
             metrics["test_rsquared_end"]  = getRSquaredForNeuralNetwork(mocapNETNetwork,testInput,testOutput)
       #------------------------------------------------------------------------
       if (checkIfFileExists('bestW.hdf5')):
          print("We use the best possible model from our ModelCheckpoint")  
          mocapNETNetwork.load_weights('bestW.hdf5')
          os.system('rm bestW.hdf5') #Get rid of the checkpoint as soon as we are done reading it
       else:
          print("No bestW.hdf5 file found, just using last iteration..")  
       #------------------------------------------------------------------------
       return history,mocapNETNetwork,metrics
#-------------------------------------------------------------------------------------------------------------------------------------------------

def compareTrainingOutputs(groundTruth,neuralOutput):
    if (len(groundTruth)!=len(neuralOutput)):
       print("Outputs have a different size")
       return np.nan
    
    values = len(groundTruth)
      
    losses = (groundTruth[:]-neuralOutput[:])**2
    return losses
#----------------------------------------------------

def appendHistory(baseHistory,historyToAppend):
     #print("baseHistory -> ",type(baseHistory))
     #print("baseHistory Contents -> ",baseHistory)
     #print("historyToAppend -> ",type(historyToAppend))
     #print("historyToAppend Contents -> ",historyToAppend)
     #print("baseHistory.history -> ",type(baseHistory.history))
     #print("historyToAppend.history -> ",type(historyToAppend.history))
     #-----------------------------------------------------------------------------------------------------------
     if ("mae" in baseHistory.history)  and  ("mae" in historyToAppend.history):
        baseHistory.history['mae']  = baseHistory.history['mae'] + historyToAppend.history['mae']
     if ("loss" in baseHistory.history)  and  ("loss" in historyToAppend.history):
        baseHistory.history['loss'] = baseHistory.history['loss'] + historyToAppend.history['loss']
     if ("val_loss" in baseHistory.history) and  ("val_loss" in historyToAppend.history):
        baseHistory.history['val_loss'] = baseHistory.history['val_loss'] + historyToAppend.history['val_loss']
     if ("val_mae" in baseHistory.history)  and  ("val_mae" in historyToAppend.history):
        baseHistory.history['val_mae'] = baseHistory.history['val_mae'] + historyToAppend.history['val_mae']
     #-----------------------------------------------------------------------------------------------------------
     return baseHistory


def getLossManually(mocapNETNetwork,trainInput,trainOutput):
    predictions = mocapNETNetwork.predict(trainInput) 
    losses = compareTrainingOutputs(predictions,trainOutput)
    #print(losses)
    print("Calculating Loss Statistics :")
    #-------------------------- 
    maximum = np.max(losses)
    minimum = np.min(losses)
    median  = np.median(losses)
    mean    = np.mean(losses)
    from tools import calculateStandardDeviationInPlaceKnowingMean,convertStandardDeviationToVariance
    std     = calculateStandardDeviationInPlaceKnowingMean(losses,mean) #np.std(losses)
    var     = convertStandardDeviationToVariance(std) #np.var(losses)
    #-------------------------- 
    titleString="Min=%0.2f,Max=%0.2f,Median=%0.2f,Mean=%0.2f,Std=%0.2f,Var=%0.2f" % (minimum,maximum,median,mean,std,var)
    print(bcolors.WARNING," %s " % titleString,bcolors.ENDC)

    #difficultyRating = 1.5
    difficultyRating = 5.0 # 4.0 1.5 2.8
    badScoreAbove = mean + difficultyRating * np.sqrt(std)
    print("Threshold for difficult poses is ",badScoreAbove)
          
    difficultPosesIndexes=list() 
    numberOfTrainingSamples = len(trainInput) 
    for z in range(0,numberOfTrainingSamples):
            if (np.any(losses[z]>badScoreAbove)):
                        difficultPosesIndexes.append(z)
    del losses
    
    return difficultPosesIndexes,mean,std



def onlineHardExampleMiningTraining(tensorboardLabel,mocapNETNetwork,numberOfEpochs,numberOfHardEpochs,numberOfNormalEpochsAfterHard,batchSize,earlyStoppingPatience,trainInput,trainOutput,testInput,testOutput,minD,modelIsTrivial=False,haveTestSet=False,useHalfFloats=False):
    #First do a regular training..
    halfEpochs=int(numberOfEpochs/2)
    if (numberOfEpochs==1):
        halfEpochs=numberOfEpochs
    print(" Online Hard Example Mining Training session .. ")

    metrics = dict()
    if (not modelIsTrivial):
       metrics["train_rsquared_start"] = getRSquaredForNeuralNetwork(mocapNETNetwork,trainInput,trainOutput)

    dtypeSelected=np.float32
    if (useHalfFloats):
       dtypeSelected=np.float16       
    totalHistory,mocapNETNetwork = regularTraining(tensorboardLabel,mocapNETNetwork,halfEpochs,batchSize,earlyStoppingPatience,trainInput,trainOutput,testInput,testOutput,minD,haveTestSet=haveTestSet,useHalfFloats=useHalfFloats)
    
    if (numberOfEpochs==1):
       print(" Trivial output will not mine etc. ")
       return totalHistory

    bestTrainingMAE=0.0
    successfulUpdates=0
    
    REGULAR_EPOCHS_AFTER_HARD=numberOfNormalEpochsAfterHard
    HARD_EPOCHS=numberOfHardEpochs
    
    miningEpochs = int(halfEpochs/2)

    print("Getting initial state of network")
    difficultPosesIndexes,mean,std = getLossManually(mocapNETNetwork,trainInput,trainOutput) 
 
    print("Backing up model with mae ",mean)
    mocapNETNetwork.save_weights("modelBackup.h5")
    bestTrainingMAE=mean      

    for i in range(0,miningEpochs):
         #-------------------------------------------------------------------------------------------------------------------------------------------------------
         #-------------------------------------------------------------------------------------------------------------------------------------------------------
         #-------------------------------------------------------------------------------------------------------------------------------------------------------
         numberOfTrainingSamples = len(trainInput) 
         inputSize  = len (trainInput[1])
         outputSize = len (trainOutput[1])

         ratioOfDatasetThatIsDifficult = float ( len(difficultPosesIndexes) / numberOfTrainingSamples ) 

         #If more than 10% of the dataset is difficult
         if (ratioOfDatasetThatIsDifficult>0.5):
            print(bcolors.FAIL," More than half of the dataset is hard (%u samples) so we train on all samples as hard %u/%u" % (len(difficultPosesIndexes),i,miningEpochs),bcolors.ENDC)  
            regularHistory,mocapNETNetwork = regularTraining(tensorboardLabel,mocapNETNetwork,HARD_EPOCHS,batchSize,5,trainInput,trainOutput,testInput,testOutput,minD,haveTestSet=haveTestSet,useHalfFloats=useHalfFloats) #difficultInput,difficultOutput 
            totalHistory   = appendHistory(totalHistory,regularHistory)
         elif (ratioOfDatasetThatIsDifficult>0.1):
            difficultInput  = np.full([len(difficultPosesIndexes),inputSize],fill_value=0,dtype=dtypeSelected,order='C') 
            difficultOutput = np.full([len(difficultPosesIndexes),outputSize],fill_value=0,dtype=dtypeSelected,order='C') 
            for z in range(0,len(difficultPosesIndexes)):
               for field in range(0,inputSize):
                     difficultInput[z,field]=trainInput[difficultPosesIndexes[z],field]
               for field in range(0,outputSize):
                     difficultOutput[z,field]=trainOutput[difficultPosesIndexes[z],field]
 
            print(bcolors.OKBLUE," Will now train on %u difficult samples %u/%u" % (len(difficultPosesIndexes),i,miningEpochs),bcolors.ENDC)
            newHistory,mocapNETNetwork   = regularTraining(tensorboardLabel,mocapNETNetwork,HARD_EPOCHS,batchSize,5,difficultInput,difficultOutput,testInput,testOutput,minD,haveTestSet=haveTestSet,useHalfFloats=useHalfFloats)
            totalHistory = appendHistory(totalHistory,newHistory)
            del difficultInput
            del difficultOutput

            if (mean<bestTrainingMAE):
               print(bcolors.OKGREEN,"Inter-success... beat previous model with mae ",bestTrainingMAE," with this model that has ",mean," mae",bcolors.ENDC)
               successfulUpdates=successfulUpdates+1
               mocapNETNetwork.save_weights("modelBackup.h5")
               bestTrainingMAE=mean 

            print(" Will now train on all samples not to forget them" )       
            regularHistory,mocapNETNetwork = regularTraining(tensorboardLabel,mocapNETNetwork,REGULAR_EPOCHS_AFTER_HARD,batchSize,earlyStoppingPatience,trainInput,trainOutput,testInput,testOutput,minD,haveTestSet=haveTestSet,useHalfFloats=useHalfFloats)
            totalHistory   = appendHistory(totalHistory,regularHistory)
         else:
            print(bcolors.OKGREEN,"The training has gone pretty well for all samples..",bcolors.ENDC)
            regularHistory,mocapNETNetwork = regularTraining(tensorboardLabel,mocapNETNetwork,1,batchSize,1,trainInput,trainOutput,testInput,testOutput,minD,haveTestSet=haveTestSet,useHalfFloats=useHalfFloats)
            totalHistory   = appendHistory(totalHistory,regularHistory)
            break
         
         #Tensorflow leaves the existing models on GPU, as a result we run out of memory,
         #try to collect garbage now..
         gc.collect()

         #Check what happened..
         difficultPosesIndexes,mean,std = getLossManually(mocapNETNetwork,trainInput,trainOutput)  
         #-----------------------      
         if (mean<bestTrainingMAE):
            print(bcolors.OKGREEN,"We beat previous model with mae ",bestTrainingMAE," with this model that has ",mean," mae",bcolors.ENDC)
            successfulUpdates=successfulUpdates+1
            mocapNETNetwork.save_weights("modelBackup.h5")
            bestTrainingMAE=mean 
         else:
            print(bcolors.FAIL,"We scored worse than mae ",bestTrainingMAE,", current score was ",mean," mae",bcolors.ENDC) 
         #-----------------------
         #-------------------------------------------------------------------------------------------------------------------------------------------------------
         #-------------------------------------------------------------------------------------------------------------------------------------------------------
         #-------------------------------------------------------------------------------------------------------------------------------------------------------

    
    if (successfulUpdates==0):
           print(bcolors.FAIL,"This OHME code was a waste of time",bcolors.ENDC)
    
    print("Using the best model that had a mae of %0.2f after %u updates"%(bestTrainingMAE,successfulUpdates))
    mocapNETNetwork.load_weights("modelBackup.h5")
    if (not modelIsTrivial):
       metrics["train_rsquared_end"] = getRSquaredForNeuralNetwork(mocapNETNetwork,trainInput,trainOutput)
    del difficultPosesIndexes

    return totalHistory,mocapNETNetwork,metrics
#-------------------------------------------------------------------------------------------------------------------------------------------------


