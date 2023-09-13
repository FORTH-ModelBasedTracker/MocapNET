#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import gc
import os
import sys
import time

import tensorflow as tf
from tensorflow import keras

from tensorflow.keras.layers import Input, Dense
from tensorflow.keras.models import Model
from tensorflow.keras.models import Sequential
from tensorflow.keras.models import model_from_json
from tensorflow.keras.utils  import plot_model

import numpy as np 
from readCSV  import parseConfiguration,checkIfConfigurationHierarchyIsTheSameAsLabelList,readGroundTruthFile,selectOnlySpecificColumnsOfNumpyArray
from DNNModel import loadModel,combineModels,combineAsSingleModel,saveModel
from tools    import bcolors,dumpListToFile,createDirectory,checkIfPathExists,notification,getRAMInformation
from DNNTraining import printTrainingVersion,setTensorflowBackendToHalfFloats


def doCombinedModelValidation(outputDirectoryForCombinedModels,groundTruth,merged):
       if (groundTruth["in"].shape[0]!=groundTruth["out"].shape[0]):
                      print(bcolors.FAIL,"Cannot perform network validation, mismatched input/output..",bcolors.ENDC)
                      sys.exit(1)
       #----------------------------------------------------------------------------------
       print(bcolors.OKBLUE,"Doing network validation..",bcolors.ENDC)
       print("We have ",groundTruth["in"].shape[0]," input samples")
       print("We have ",groundTruth["in"].shape[1]," elements in each input sample")
       print("We have ",groundTruth["out"].shape[0]," output samples")
       print("We have ",groundTruth["out"].shape[1]," elements in each output sample")
       #-------------------------------------------------------------------------------
       numberOfOutputs = groundTruth["out"].shape[1]
       #-------------------------------------------------------------------------------
       outputScalars      = [1.0]     * numberOfOutputs
       outputOffsets      = [0.0]     * numberOfOutputs
       outputMinima       = [-6000.0] * numberOfOutputs
       outputMaxima       = [6000.0]  * numberOfOutputs
       #-------------------------------------------------------------------------------
       from readCSV import parseOutputNormalization
       from tools   import readListFromFile,convertListToLowerCase
       #-------------------------------------------------------------------------------
       outputs       = convertListToLowerCase(readListFromFile(outputDirectoryForCombinedModels+"/neuralNetworkOutputs.list"))
       outputOffsets = parseOutputNormalization(outputDirectoryForCombinedModels,"/outputOffsets.csv",outputs,outputOffsets)
       outputScalars = parseOutputNormalization(outputDirectoryForCombinedModels,"/outputScalarsFraction.csv",outputs,outputScalars)
       outputMinima  = parseOutputNormalization(outputDirectoryForCombinedModels,"/outputMinima.csv",outputs,outputMinima)
       outputMaxima  = parseOutputNormalization(outputDirectoryForCombinedModels,"/outputMaxima.csv",outputs,outputMaxima)
       #-------------------------------------------------------------------------------
       losses = list()
       for outputItem in range(0,groundTruth["out"].shape[1]):
           losses.append(0.0)
       #----------------------------------------------------------------------------------
       for sample in range(0,groundTruth["in"].shape[0]):
               networkInput = np.asarray([groundTruth["in"][sample]],dtype=np.float32)  
               print("Sample : ",sample," |",end='')
               inferenceOutputs = merged(tf.cast(networkInput,dtype=tf.float32))
               for outputItem in range(0,groundTruth["out"].shape[1]):
                   out = float(inferenceOutputs[0][outputItem])
                   #------------------------------------------------------------   
                   out = (out * outputScalars[i]) + outputOffsets[i]
                   if (out>outputMaxima[i]):
                              out=outputMaxima[i]
                   if (out<outputMinima[i]):
                              out=outputMinima[i]
                   #------------------------------------------------------------
                   thisLoss = float(abs(out-groundTruth["out"][sample][outputItem]))
                   print("|",thisLoss,"|",end='')
                   losses[outputItem] += thisLoss
               print(" ")
       #----------------------------------------------------------------------------------
       for outputItem in range(0,groundTruth["out"].shape[1]):
           print("Output item ",outputItem," loss ~=> ",float(losses[outputItem]/groundTruth["out"].shape[1]) )
       sys.exit(0)
    #-----------------------------------------------------------------------------------------------------------------




def step1_DNNCombine(useCSVBackend,groundTruth):
    printTrainingVersion()
    print("""
███████╗███╗   ██╗███████╗███████╗███╗   ███╗██████╗ ██╗     ███████╗
██╔════╝████╗  ██║██╔════╝██╔════╝████╗ ████║██╔══██╗██║     ██╔════╝
█████╗  ██╔██╗ ██║███████╗█████╗  ██╔████╔██║██████╔╝██║     █████╗  
██╔══╝  ██║╚██╗██║╚════██║██╔══╝  ██║╚██╔╝██║██╔══██╗██║     ██╔══╝  
███████╗██║ ╚████║███████║███████╗██║ ╚═╝ ██║██████╔╝███████╗███████╗
╚══════╝╚═╝  ╚═══╝╚══════╝╚══════╝╚═╝     ╚═╝╚═════╝ ╚══════╝╚══════╝                                                                                                              
""")


    print("DNNTrainer starting up..\n")
    np.set_printoptions(precision=2)
    np.set_printoptions(formatter={"float_kind": lambda x: "%g" % x})

    #Defaults 
    #-------------------
    useHalfFloats    = 0
    useNonCompressed = 1
    useCompressed    = 1
    #-------------------

    #This will be get populated using the --config argument
    configuration = dict()
    configurationPath=""

    numberOfChannelsPerNSDMElement=2
     
    #datasetDirectory="../../DNNTracker/dataset"
    datasetDirectory="dataset/generated/"

    #By default no training samples need to be loaded
    trainingSamplesPercentage=0.0

    #Default Names 
    #------------------------------------------------
    hierarchyPartName               = "body"
    label                           = "unknown"
    dataFile                        = "body"
    testFile                        = "body_test"
    outputMode                      = "bvh"
    configuration['outputMode']     = "bvh"
    doModelOptimization             = False
    doModelValidation               = False
    inputLabels                     = list()
    outputLabels                    = list()
    #------------------------------------------------
    outputDirectoryForTrainedModels ="step0FrontBody"
    outputDirectoryForCombinedModels="step1FrontBody"
    filesPath=outputDirectoryForTrainedModels #"./"
    #------------------------------------------------
    if (len(sys.argv)>1):
       print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--mem"):
              print("Will load more than 0 training samples.. ",sys.argv[i+1],"\n");
              trainingSamplesPercentage=float(sys.argv[i+1])
           if (sys.argv[i]=="--optimize"):
              print("Will also attempt to optimize network\n");
              trainingSamplesPercentage=1.0 #<- This should be like --mem 1.0
              doModelOptimization = True
           if (sys.argv[i]=="--validate"):
              print("Will also attempt to validate network\n");
              trainingSamplesPercentage=1.0 #<- This should be like --mem 1.0
              doModelValidation = True
           if (sys.argv[i]=="--config"):
              configurationPath = sys.argv[i+1]
              configuration = parseConfiguration(configurationPath)
              label = configuration['OutputDirectory']

              if (configuration['NSDMAlsoUseAlignmentAngles']==1):
                  numberOfChannelsPerNSDMElement=1
                  print("Number of Channels Per NSDM element set to  ",numberOfChannelsPerNSDMElement)


              if (configuration['precision']=="fp16"):
                 useHalfFloats=1
                 configuration['rememberWeights']=0
                 setTensorflowBackendToHalfFloats()
              elif (configuration['precision']!="fp32"):
                 print(bcolors.WARNING,"Cannot use ",configuration['precision']," precision\n",bcolors.ENDC)

           if (sys.argv[i]=="--lpdnn"):
              print("Force Network Padding\n") 
              configuration['padEnsembleInput'] = 1
           if (sys.argv[i]=="--3d"):
              print("The network will generate 3D points instead of angles\n")
              outputMode = "3d"
              configuration['outputMode']   = "3d"
              configuration['groupOutputs'] = 3
           if (sys.argv[i]=="--halffloats"):
              useHalfFloats=1
              setTensorflowBackendToHalfFloats()
           if (sys.argv[i]=="--from"):
              print("\nCombining.. ",sys.argv[i+1],"\n");
              outputDirectoryForTrainedModels = sys.argv[i+1]
           if (sys.argv[i]=="--categoryall"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_all" % hierarchyPartName
              outputDirectoryForTrainedModels = "step2_%s" % dataFile
              startModel=0
              endModel=6
              print("stepAll has been reduced to a single encoder so no need to merge it we will just copy it")
              #os.system('cp step0All/hip_Yrotation/model.h5 step0All/mergedModel.h5')
              #os.system('cp step0All/hip_Yrotation/model.json step0All/mergedModel.json')

              groundTruth = readGroundTruthFile(
                                                configuration,
                                                "Reference",
                                                "%s/2d_%s.csv" % (datasetDirectory,dataFile),
                                                "%s/%s_%s.csv" % (datasetDirectory,configuration['outputMode'],dataFile),
                                                0,
                                                numberOfChannelsPerNSDMElement,
                                                0,#dont use radians
                                                0,#dont use half floats
                                               )
              #combineAsSingleModel(outputDirectoryForTrainedModels,"hip_Yrotation",len(groundTruth['labelIn']),len(groundTruth['labelOut']),"mergedModel","all")
              combineAsSingleModel(outputDirectoryForTrainedModels,"Category",len(groundTruth['labelIn']),len(groundTruth['labelOut']),"mergedModelCategory","all")
              sys.exit(0)
           #-----------------------------------------------
           #                New 4 way split
           #-----------------------------------------------
           if (sys.argv[i]=="--all"):
              hierarchyPartName=sys.argv[i+1]
              label="all"
              dataFile="%s_%s" % (hierarchyPartName,label)
              outputDirectoryForTrainedModels ="step0_%s" % dataFile
              outputDirectoryForCombinedModels="step1_%s" % dataFile
           if (sys.argv[i]=="--back"):
              hierarchyPartName=sys.argv[i+1]
              label="back"
              dataFile="%s_%s" % (hierarchyPartName,label)
              outputDirectoryForTrainedModels ="step0_%s" % dataFile
              outputDirectoryForCombinedModels="step1_%s" % dataFile
           if (sys.argv[i]=="--front"):
              hierarchyPartName=sys.argv[i+1]
              label="front"
              dataFile="%s_%s" % (hierarchyPartName,label)
              outputDirectoryForTrainedModels ="step0_%s" % dataFile
              outputDirectoryForCombinedModels="step1_%s" % dataFile
           if (sys.argv[i]=="--left"):
              hierarchyPartName=sys.argv[i+1]
              label="left"
              dataFile="%s_%s" % (hierarchyPartName,label)
              outputDirectoryForTrainedModels ="step0_%s" % dataFile
              outputDirectoryForCombinedModels="step1_%s" % dataFile
           if (sys.argv[i]=="--right"):
              hierarchyPartName=sys.argv[i+1]
              label="right"
              dataFile="%s_%s" % (hierarchyPartName,label)
              outputDirectoryForTrainedModels ="step0_%s" % dataFile
              outputDirectoryForCombinedModels="step1_%s" % dataFile

    #Combine needs to read the decomposition files from the outputDirectoryForTrainedModels
    #---------------------------------------------------------------------------------------------------
    pathToDefaultDecompositionFilesFolder = configuration['doPCA'].split("/")
    if (len(pathToDefaultDecompositionFilesFolder)>0):
       #Resolve PCA name.. :(
       x = pathToDefaultDecompositionFilesFolder[1].split("$")
       if (len(x)>0):
          #If there is a $ character this is our place holder to autocomplete our dataFile 
          print("PCA filename is resolved from ",configuration['doPCA'])
          print(" to ")
          configuration['doPCA']=outputDirectoryForTrainedModels+"/"+x[0]+dataFile+".pca"
          print(configuration['doPCA'])


    #Calculate number of channels per NSDM elements
    #---------------------------------------------------------------------------------------------------
    numberOfChannelsPerNSDMElement=2
    if (configuration['NSDMAlsoUseAlignmentAngles']==1):
       numberOfChannelsPerNSDMElement=1
    print("Number of Channels Per NSDM element ",numberOfChannelsPerNSDMElement)
    configuration['numberOfChannelsPerNSDMElement']=numberOfChannelsPerNSDMElement


    #Eigenposes should be loaded before Train/Test files since they are used in the input transform step
    #---------------------------------------------------------------------------------------------------
    if ("eigenPoses" in configuration):
        #We have eigenPoses Declared!
        if (configuration['eigenPoses']==1):
           #We have eigenPoses Active!
           configuration['eigenPoseData'] = readGroundTruthFile(
                                                                configuration,
                                                                "Eigenposes",
                                                                "%s/2d_%s_eigenposes.csv"  % (datasetDirectory,dataFile),
                                                                "%s/%s_%s_eigenposes.csv"  % (datasetDirectory,configuration['outputMode'],dataFile), #configuration['outputMode'] is either bvh or 3d
                                                                1.0,
                                                                numberOfChannelsPerNSDMElement,
                                                                configuration['useRadians'],
                                                                useHalfFloats
                                                               )


    #We only want headers so we ask for 0.0 memory usage which means just the labels and one record..
    #groundTruth  = readGroundTruthFile("Test" ,"../../DNNTracker/dataset/2d_test.csv","../../DNNTracker/dataset/bvh_test.csv",0.0,useNonCompressed,useCompressed,configuration['useRadians'],useHalfFloats)




    #==============================================================================================================================
    #==============================================================================================================================
    #==============================================================================================================================
    #The inputLabels/outputLabels are very important to create a combined network that has the 
    #correct outputs, there are 2 ways to load them, the old way was through the groundtruth file
    #the new way is through the .list files, since neuralNetworkOutputs.list gets overwritten
    #to enable quick copy of networks 
    #--------------------------------------------------------------------------------------------
    inputLabelsNew=""; outputLabelsNew=""; inputLabelsOld=""; outputLabelsOld=""
    #--------------------------------------------------------------------------------------------
    from tools import checkIfFileExists,readListFromFile,convertListToLowerCase,checkIfListsAreTheSame
    #--------------------------------------------------------------------------------------------
    inputLabelsPath  = outputDirectoryForTrainedModels+"/neuralNetworkInputs.list"
    outputLabelsPath = outputDirectoryForTrainedModels+"/neuralNetworkOutputsOriginal.list"
    if (checkIfFileExists(inputLabelsPath) and checkIfFileExists(outputLabelsPath) ):
       print("Using labels from %s / %s..\n" % (inputLabelsPath,outputLabelsPath) )
       inputLabelsNew  = readListFromFile(inputLabelsPath)
       outputLabelsNew = readListFromFile(outputLabelsPath)
    #--------------------------------------------------------------------------------------------
    #If we are using CSV backend AND  ( new labels loading failed  or  we want to do model optimization ), 
    if (useCSVBackend)   and   ( (inputLabelsNew=="") or (outputLabelsNew=="")    or   ((doModelOptimization) and (trainingSamplesPercentage>0.0)) ):
      groundTruth = readGroundTruthFile(
                                        configuration,
                                        "Reference",
                                        "%s/2d_%s.csv" % (datasetDirectory,dataFile),
                                        "%s/%s_%s.csv" % (datasetDirectory,configuration['outputMode'],dataFile),
                                        trainingSamplesPercentage,
                                        numberOfChannelsPerNSDMElement,
                                        configuration['useRadians'],
                                        useHalfFloats
                                       )
      inputLabelsOld  = groundTruth['labelIn']
      outputLabelsOld = groundTruth['labelOut']
    #--------------------------------------------------------------------------------------------
    #We need the labels for our input and output..!
    #We used to get these from the ground truth file, however nowadays
    #they also exist in the neuralNetworkInputs.list / neuralNetworkOutputs.list
    #all this code exists in order to try to be backwards compatible :(
    #--------------------------------------------------------------------------------------------
    if (inputLabelsNew!="") and (outputLabelsNew!="") and (inputLabelsOld!="") and (outputLabelsOld!=""):
       print(bcolors.OKGREEN,"We have both new and old input labels.. Selecting the new labels",bcolors.ENDC)
       inputLabels  = inputLabelsNew
       outputLabels = outputLabelsNew
       if (len(inputLabelsOld)!=len(inputLabelsNew)) or (len(outputLabelsOld)!=len(outputLabelsNew)):
           print(bcolors.FAIL,"I/O labels mismatch between test label set and I/O .list files ..",bcolors.ENDC)
           sys.exit(1)
       if ( not checkIfListsAreTheSame(inputLabelsOld,inputLabelsNew) ):
           print(bcolors.FAIL,"I/O labels mismatch between input test label set and I/O .list files ..",bcolors.ENDC)
           sys.exit(1)
       if ( not checkIfListsAreTheSame(outputLabelsOld,outputLabelsNew) ):
           print(bcolors.FAIL,"I/O labels mismatch between output test label set and I/O .list files ..",bcolors.ENDC)
           sys.exit(1)
    elif (inputLabelsNew=="") and (outputLabelsNew=="") and (inputLabelsOld!="") and (outputLabelsOld!=""):
       print(bcolors.OKGREEN,"We only have OLD labels from groundTruth, using them",bcolors.ENDC)
       inputLabels  = inputLabelsOld
       outputLabels = outputLabelsOld
    elif (inputLabelsNew!="") and (outputLabelsNew!="") and (inputLabelsOld=="") and (outputLabelsOld==""):
       print(bcolors.OKGREEN,"We only have NEW labels from .list files, using them",bcolors.ENDC)
       inputLabels  = inputLabelsNew
       outputLabels = outputLabelsNew
    else:
       print(bcolors.FAIL,"Weird combination of I/O labels, don't know what to do so stopping to avoid mistakes..",bcolors.ENDC)
       sys.exit(1)
    #--------------------------------------------------------------------------------------------
    #==============================================================================================================================
    #==============================================================================================================================
    #==============================================================================================================================

    numberOfOutputColumns=len(outputLabels)

    #Shorthand to be able to do tricks without altering configuration
    #-----------------------------------------------------------
    numberOfOutputsOfEachEncoder = configuration['groupOutputs']
    if (numberOfOutputsOfEachEncoder==0):
           #This special case means that the encoder should span everything automatically..!
           numberOfOutputsOfEachEncoder = numberOfOutputColumns #len(groundTruthTest['out'][0])
           print(bcolors.WARNING + "step0_DNNTrainer.py : Setting numberOfOutputsOfEachEncoder = %u" % numberOfOutputsOfEachEncoder +  bcolors.ENDC )

    #--------------------------------------------------------------------------------------------
    if ( len(inputLabels)==0 or len(outputLabels)==0  ):
       print("I/O labels could not be parsed, we don't know what to do to combine encoders..")
       sys.exit(1)
    #--------------------------------------------------------------------------------------------
    if (not checkIfConfigurationHierarchyIsTheSameAsLabelList(configuration,inputLabels,1)):
       print("Inconsistent Configuration association compared to the inputLabelList")
       sys.exit(1)
    #--------------------------------------------------------------------------------------------
    if ('probabilisticOutput' in configuration) and (configuration['probabilisticOutput']==1):
       if (numberOfOutputsOfEachEncoder!=1): 
          print(bcolors.FAIL,"Overriding group outputs setting and setting it to 1!",bcolors.ENDC)
          numberOfOutputsOfEachEncoder=1
    #--------------------------------------------------------------------------------------------

    startModel = 0                            # We want to combine all labels so we start from zero..!
    endModel   = len(outputLabels) # We want to combine all labels so we start at label length zero..!
    print("Will read from %s , start at %u and stop at %u ..\n" % (outputDirectoryForTrainedModels,startModel,endModel) )
    print("Combined model will have input size of ",len(inputLabels)," and output size of ",len(outputLabels)," \n")


    outputName="none"
    expectedModelNames=list() 
     #----------------------------------------------------------------------------------------
    #we take care of new potentially more complex naming scheme based on the output groupping
    for outputType in range(startModel,endModel,numberOfOutputsOfEachEncoder):
        #if we have multiple outputs groupped we want to concatenate all their names to keep track of all handled joints
        if (numberOfOutputsOfEachEncoder==numberOfOutputColumns):
           #Guard against : File name too long [Op:CreateSummaryFileWriter]  
           outputName="everything"
           expectedModelNames.append(outputName)
        elif (numberOfOutputsOfEachEncoder==1):
           outputName=outputLabels[outputType]
           expectedModelNames.append(outputName)
        elif (numberOfOutputsOfEachEncoder>1):
           outputName = ""
           isFirst    = 1
           for nameToConcatenate in range(outputType,outputType+numberOfOutputsOfEachEncoder):
               if (isFirst==0):
                   outputName=outputName+"-" #so that output is "3DX_hip_3DY_hip_3DZ_hip" instead of "3DX_hip3DY_hip3DZ_hip"
               outputName = outputName + outputLabels[nameToConcatenate]
               isFirst    = 0
           expectedModelNames.append(outputName)
     #----------------------------------------------------------------------------------------
   
    print("Expected model names are : ",expectedModelNames)

    if (checkIfPathExists(outputDirectoryForCombinedModels)):
       print("We found old combined output (%s), we will wipe it to make sure everything is clean! ",outputDirectoryForCombinedModels)
       os.system('rm -rf %s/'% (outputDirectoryForCombinedModels))
    os.system('mkdir %s/'% (outputDirectoryForCombinedModels))

    padEnsembleInput=0
    if 'padEnsembleInput' in configuration:
        padEnsembleInput=int(configuration['padEnsembleInput'])
  
    #Make a fresh merged Model
    mergedModelInMemory,mergedOutputs,selectedColumns = combineModels(
                                                                      configuration,
                                                                      outputDirectoryForTrainedModels,
                                                                      "mergedModel",
                                                                      len(inputLabels),
                                                                      len(outputLabels), #The outputs we get in total remain the same 
                                                                      expectedModelNames,      # outputLabels
                                                                      startModel,
                                                                      len(expectedModelNames), #This is actually shrunk down from endModel,
                                                                      label,
                                                                      lpdnnPadding=padEnsembleInput
                                                                     )

    #Load fresh merged Model using its h5/json format
    #no longer reloading merged
    merged = mergedModelInMemory
    #merged=loadModel(outputDirectoryForTrainedModels,"mergedModel") 
    gc.collect()




    #If --optimize or --mem>0 try to optimize network
    #------------------------------------------------------------------------------
    if (doModelOptimization) and (trainingSamplesPercentage>0.0) and (numberOfOutputsOfEachEncoder==1) and (configuration['probabilisticOutput']==0):
       print(bcolors.OKBLUE,"Doing network optimization..",bcolors.ENDC)
       trainingSelectedOutput = selectOnlySpecificColumnsOfNumpyArray(groundTruth['out'],selectedColumns,useHalfFloats)
       from DNNOptimize import pruneModel,clusterModel
       optimized = pruneModel(merged,groundTruth['in'],trainingSelectedOutput)
       #These 2 can't work together!
       #optimized = clusterModel(merged,configuration,groundTruth['in'],trainingSelectedOutput)
       #saveModel(outputDirectoryForTrainedModels,optimized,name="optimizedModel") <- RuntimeError: Unable to create link (name already exists) on clustering

  
       #from DNNOptimize import quantizeModel
       #Quantizing a tf.keras Model inside another tf.keras Model is not supported.
       #optimized = quantizeModel(merged,groundTruth['in'],trainingSelectedOutput)
       #saveModel(outputDirectoryForTrainedModels,optimized,name="optimizedQuantizedModel")

       print("Overriding model with optimized version!")
       merged=optimized
    else: 
       print("To also optimize model please supply --optimize ( OR --mem VALUE with a value >0 )")
    #------------------------------------------------------------------------------
    print(bcolors.OKBLUE,"Saving Combined model to ",outputDirectoryForCombinedModels,"..",bcolors.ENDC)
    startAt = time.time()
    merged.save(outputDirectoryForCombinedModels, save_format='tf') # <- Actually Save Neural Network to File
    endAt = time.time()
    print(bcolors.OKGREEN,"Save operation took %0.2f mins "%((endAt-startAt)/60),bcolors.ENDC)

    #We will also copy with it the two I/O lists to make our lifes easier 
    print(bcolors.OKBLUE,"Handle configuration files..",bcolors.ENDC)
    #---------------------------------------------------------------------------------------------------------------------
    os.system('cp %s/*.csv %s/'                     % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
    os.system('cp %s/summary.html %s/'              % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
    os.system('cp %s/outputMinima.csv %s/'          % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
    os.system('cp %s/outputMaxima.csv %s/'          % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
    os.system('cp %s/outputScalars.csv %s/'         % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
    os.system('cp %s/outputScalarsFraction.csv %s/' % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
    os.system('cp %s/outputOffsets.csv %s/'         % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
    os.system('cp %s/neuralNetworkJoints.list %s/'  % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
    os.system('cp %s/neuralNetworkInputs.list %s/'  % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
    #---------------------------------------------------------------------------------------------------------------------
    configFileStandaloneName = os.path.basename(configurationPath) 
    os.system('cp %s/%s %s/' % (outputDirectoryForTrainedModels,configFileStandaloneName,outputDirectoryForCombinedModels)) 
    
    #PLEASE KEEP IN MIND THAT THE BASH SCRIPTS ALSO DO THEIR OWN COPIES SO THIS IS  
    #IS CURRENTLY WAY MORE COMPLICATED THAT IT NEEDS TO..!

    #Initially the idea was to directly grab the .pca file from the dataset/ dir however nowadays things are more complex 
    #os.system('cp %s/%s %s/' % (outputDirectoryForTrainedModels,pcaFileStandaloneName,outputDirectoryForCombinedModels))
    #We will now copy *ALL* files hoping to cover everything!  
    pcaFileStandaloneName    = os.path.basename(configuration['doPCA'])
    os.system('cp %s/%s %s/'  % (outputDirectoryForTrainedModels,pcaFileStandaloneName,outputDirectoryForCombinedModels))
    os.system('cp %s/%s* %s/' % (outputDirectoryForTrainedModels,pcaFileStandaloneName,outputDirectoryForCombinedModels))

    if (numberOfOutputsOfEachEncoder!=numberOfOutputColumns):
       #Write culled list ..!
       print("Overwriting neuralNetworkOutputs.list with culled output list")
       dumpListToFile(outputDirectoryForCombinedModels+"/neuralNetworkOutputs.list" ,mergedOutputs)
    else:
       print("We are targeting everything at once so not copying instead of overwriting neuralNetworkOutputs.list with culled output list")
       os.system('cp %s/neuralNetworkOutputs.list %s/'  % (outputDirectoryForTrainedModels,outputDirectoryForCombinedModels))
       
    
    if (numberOfOutputsOfEachEncoder==numberOfOutputColumns):
      print("We won't mess around with outputs..") #Case when everything is selected at once..
    elif (numberOfOutputsOfEachEncoder>1):
      os.system('cp %s %s' % (outputDirectoryForCombinedModels+"/neuralNetworkOutputs.list",outputDirectoryForCombinedModels+"/neuralNetworkOutputsOriginal.list"))
      from tools import splitTextBasedOnGroupNumber 
      splitTextBasedOnGroupNumber(numberOfOutputsOfEachEncoder,outputDirectoryForCombinedModels+"/neuralNetworkOutputsOriginal.list",outputDirectoryForCombinedModels+"/neuralNetworkOutputs.list")

    #My PC doesn't have enough RAM for this..
    #print(bcolors.OKBLUE,"Also Creating a TensorRT Model..",bcolors.ENDC)
    #-----------------------------------------------------------------
    #from DNNOptimize import convertToTensorRT
    #convertToTensorRT(merged,precision="fp16")
    #merged.save(outputDirectoryForCombinedModels+"/trt_fp16", save_format='tf') #save directory..
    #convertToTensorRT(merged,precision="fp32")
    #merged.save(outputDirectoryForCombinedModels+"/trt_fp32", save_format='tf') #save directory..
    #-----------------------------------------------------------------

    # Try to do validation 
    #-----------------------------------------------------------------------------------------------------------------
    if (doModelValidation) and (trainingSamplesPercentage>0.0) and (numberOfOutputsOfEachEncoder==1):
       doCombinedModelValidation(outputDirectoryForCombinedModels,groundTruth,merged)
    #-----------------------------------------------------------------------------------------------------------------
       

    RAM = getRAMInformation()
    if (RAM["free"]<2000000):
        print(bcolors.FAIL,"We do not have enough memory (we have ",RAM["free"],") to generate a TF-Lite model..",bcolors.ENDC)
        print(" Please manually run : ",bcolors.OKGREEN," tflite_convert --saved_model_dir=%s --output_file=%s/model.tflite"% (outputDirectoryForCombinedModels,outputDirectoryForCombinedModels),bcolors.ENDC)
    else:
        print(bcolors.OKBLUE,"Also Creating a TF-Lite Model..",bcolors.ENDC)
        gc.collect()
        #------------------------------------------------------------------------------
        converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir=outputDirectoryForCombinedModels)
        converter.optimizations = [tf.lite.Optimize.DEFAULT]
        if (doModelOptimization) and (trainingSamplesPercentage>0.0):
           #If we gave a training sample set then we can enable post training quantization on TFLite
           #converter.representative_dataset      = groundTruth['in']
           #converter.target_spec.supported_ops   = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8] 
           #converter.target_spec.supported_types = [tf.int8]
           #converter.inference_input_type        = tf.float32 # tf.int8  # or tf.uint8
           #converter.inference_output_type       = tf.float32 # tf.int8  # or tf.uint8
           pass
        tflite_model = converter.convert()
        with open('%s/model.tflite' % outputDirectoryForCombinedModels, 'wb') as f:
            f.write(tflite_model)
    #------------------------------------------------------------------------------

    #tf2onnx hogs a lot of memory, we will not use any inputs after this point
    del groundTruth
    gc.collect()

    #Show signatures.. 
    #https://www.tensorflow.org/guide/saved_model#the_savedmodel_format_on_disk
    print(bcolors.OKBLUE,"Double-check tensorflow model signatures..",bcolors.ENDC)
    os.system('saved_model_cli show --dir %s/ --tag_set serve --signature_def serving_default' % (outputDirectoryForCombinedModels))
    #--------------------------------------------------------------------------------------------------------------------------------------

    RAM = getRAMInformation()
    if (RAM["free"]<2000000):
        print(bcolors.FAIL,"We do not have enough memory (we have ",RAM["free"],") to generate an ONNX model..",bcolors.ENDC)
        print(" Please manually run : ",bcolors.OKGREEN,"  python3 -m tf2onnx.convert --saved-model %s --opset 17 --tag serve --output %s/model.onnx " % (outputDirectoryForCombinedModels,outputDirectoryForCombinedModels),bcolors.ENDC)
        print("See : https://onnxruntime.ai/docs/reference/compatibility.html for the best OPSET")
    else:
        print(" Please manually run : ",bcolors.OKGREEN,"  python3 -m tf2onnx.convert --saved-model %s --opset 17 --tag serve --output %s/model.onnx " % (outputDirectoryForCombinedModels,outputDirectoryForCombinedModels),bcolors.ENDC)
        #This gets the main python script killed for some reason on cvrldemo.. 
        #print(bcolors.OKBLUE,"Also Creating an ONNX Model..",bcolors.ENDC) #assuming python3 -m pip install tf2onnx
    #--------------------------------------------------------------------------------------------------------------------------------------
        #os.system("python3 -m tf2onnx.convert --saved-model %s --opset 14 --tag serve --output %s/model.onnx " % (outputDirectoryForCombinedModels,outputDirectoryForCombinedModels))
    #--------------------------------------------------------------------------------------------------------------------------------------
    notification("MocapNET Network Compilation","Done merging %s "%(outputDirectoryForCombinedModels))
    print("To view training run : ",bcolors.OKGREEN," tensorboard --logdir %s/tensorboard/ --bind_all" % (outputDirectoryForTrainedModels),bcolors.ENDC)
    print(bcolors.OKGREEN,"Done merging",bcolors.ENDC)
    sys.exit(0)

if __name__ == '__main__':
    useCSVBackend=1
    emptyTrainData=dict()    
    step1_DNNCombine(useCSVBackend,emptyTrainData)
