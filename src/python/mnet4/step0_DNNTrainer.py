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
 
#Print environment variables for debugging purposes..
print("os.environ=")
print(os.environ)

from DNNTraining import forceCPU,limitGPUMemory
#Maybe Use CPU----------------------------------------------------------------
if (len(sys.argv)>1):
   for i in range(0, len(sys.argv)):
       if (sys.argv[i]=="--cpu"): 
           forceCPU()
#Constrain memory usage-------------------------------------------------------
       if (sys.argv[i]=="--savegpumem"):
          limitGPUMemory()
#-----------------------------------------------------------------------------



import tensorflow as tf
#--------------------------------------------------
from tensorflow import keras
from tensorflow.keras.models import Model
from tensorflow.keras.models import Sequential
from tensorflow.keras.models import model_from_json
from tensorflow.keras.utils  import plot_model
#--------------------------------------------------
import numpy as np 
#--------------------------------------------------
from readCSV     import readCSVFile,parseConfiguration,checkIfConfigurationHierarchyIsTheSameAsLabelList,readCSVLabels,checkIfTestAndTrainListsAreOk,readGroundTruthFile,splitNumpyArray,calculateOutputNormalization,executeNormalization,checkDataForNaN
from DNNModel    import setupDNNModelsUsingJSONConfiguration,loadModel,saveModel,newEncoderModelSelector,newTrivialModel,saveConfiguration,saveLastCompletedJob,saveProgressEnded
from DNNTraining import printTrainingVersion,setTensorflowBackendToHalfFloats,logTrainingResults,regularTraining,onlineHardExampleMiningTraining
from tools       import bcolors,dumpListToFile,getRAMInformation,createDirectory,getConfigurationJointPriority,notification

print("Tensorflow DNNTrainer v2 starting up..\n")
printTrainingVersion()
print("""
████████╗██████╗  █████╗ ██╗███╗   ██╗██╗███╗   ██╗ ██████╗ 
╚══██╔══╝██╔══██╗██╔══██╗██║████╗  ██║██║████╗  ██║██╔════╝ 
   ██║   ██████╔╝███████║██║██╔██╗ ██║██║██╔██╗ ██║██║  ███╗
   ██║   ██╔══██╗██╔══██║██║██║╚██╗██║██║██║╚██╗██║██║   ██║
   ██║   ██║  ██║██║  ██║██║██║ ╚████║██║██║ ╚████║╚██████╔╝
   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═══╝ ╚═════╝ 
""")

print("Ensuring the same seed for reproducible results always..\n")
from numpy.random import seed
seed(1) 
tf.random.set_seed(2)
np.set_printoptions(precision=2) 
np.set_printoptions(formatter={"float_kind": lambda x: "%g" % x})


#Workaround for error : 
#W tensorflow/stream_executor/gpu/asm_compiler.cc:235] Your CUDA software stack is old. We fallback to the NVIDIA driver for some compilation. 
#Update your CUDA version to get the best performance. 
#The ptxas error was: ptxas fatal   : Value 'sm_86' is not defined for option 'gpu-name'
if "CUDA_VISIBLE_DEVICES" in os.environ:
      print(bcolors.OKGREEN,"CUDA DEVICE SELECTION : ",os.environ["CUDA_VISIBLE_DEVICES"]," ",bcolors.ENDC)
else:
      os.environ["CUDA_VISIBLE_DEVICES"] = "0"  #This should be 0 to signal device #0 ( a.k.a first GPU )
      print(bcolors.OKGREEN,"SELECTED CUDA DEVICE : ",os.environ["CUDA_VISIBLE_DEVICES"]," ",bcolors.ENDC)

#Make sure CUDA cache is not disabled!
os.environ['CUDA_CACHE_DISABLE'] = '0'
#Try to presist cudnn 
os.environ['TF_USE_CUDNN_BATCHNORM_SPATIAL_PERSISTENT'] = '1'
#Try to allocate as little memory as possible
os.environ['TF_FORCE_GPU_ALLOW_GROWTH'] = 'true'
#Use seperate threads so execution is not throttled by CPU
os.environ['TF_GPU_THREAD_MODE']   = 'gpu_private'
#0 = all messages are logged (default behavior)
#1 = INFO messages are not printed
#2 = INFO and WARNING messages are not printed
#3 = INFO, WARNING, and ERROR messages are not printed
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '1' # <- only print warnings or errors..


#Workaround for error : 
#  Could not create cudnn handle: CUDNN_STATUS_INTERNAL_ERROR
import tensorflow as tf
#config = tf.compat.v1.ConfigProto()
#config.gpu_options.allow_growth = True
#config.log_device_placement     = True
#sess   = tf.compat.v1.Session(config=config)
#tf.compat.v1.keras.backend.set_session(sess)
#-------------------------------------------









def step0_DNNTrainer(useCSVBackend,groundTruthTrain,groundTruthTest):
    #----------------------------------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------------------------------
    #                          Command-Line configuration and parameters and stuff
    #----------------------------------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------------------------------

    #Note starting time
    startEverythingAt = time.time() 

    #1/0 = safe settings
    saveResultsToDisk=1
    if (not saveResultsToDisk):
       input("\n\n\n\n\nResults will not be saved..! All of the computing time will go in vain..\nAre you sure you want to do this ?\n\n\n\n")

    #Options in case of cluster operation
    startPosition       = 0
    endPosition         = 0 
    jobsPerCluster      = 0
    jobsPerClusterStart = 0
    jobsPerClusterEnd   = 0

    #Cheapout on the test by default
    testMemPercentage=0.0 
    if (useCSVBackend):  
       print(bcolors.OKGREEN,"By default use CSV test data!",bcolors.ENDC)
       testMemPercentage=1.0 
       
    if (len(groundTruthTest['out'])>0):
       print(bcolors.OKGREEN,"Will use test data!",bcolors.ENDC)
       testMemPercentage=1.0
    else:
       print(bcolors.WARNING,"Will not use test data",bcolors.ENDC)

    #Use the whole training Dataset by default
    memPercentage=1.0

    #networkCompression=1.0
    useHalfFloats = 0

    #Resume training options
    executeTraining=1
    onlyExecuteTrainingOfSpecificModelType=0
     
    #AutoStart Tensorboard, disabled by default!
    autostartTensorboard=0

    #This needs to be populated using the --config argument otherwise session will fail..
    configuration = dict()
    configurationPath=""
    doModelValidation = False

    #I/O settings
    outputDirectoryForTrainedModels = "step0FrontBody"
    dataFile                        = "body"
    hierarchyPartName               = "body"
    outputMode                      = "bvh"
    configuration['outputMode']     = "bvh"
    label                           = "unnamed"
  
    # Sample call with some example parameters : 
    #       python3 step0_DNNTrainer.py --config dataset/body_configuration.json --front body --mem 1000
    #       python3 step0_DNNTrainer.py --config dataset/upperbody_configuration.json --all upperbody --mem 1000
    #       python3 step0_DNNTrainer.py --config dataset/lowerbody_configuration.json --all lowerbody --mem 1000


    if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--config"):
              configurationPath=sys.argv[i+1]
              configuration = parseConfiguration(configurationPath)
              setupDNNModelsUsingJSONConfiguration(configuration)
              if (configuration['precision']=="fp16"):
                 useHalfFloats=1
                 configuration['rememberWeights']=0
                 setTensorflowBackendToHalfFloats()
              elif (configuration['precision']!="fp32"):
                 print(bcolors.WARNING,"Cannot use ",configuration['precision']," precision\n",bcolors.ENDC)
           if (sys.argv[i]=="--3d"):
              print(bcolors.WARNING,"The network will generate 3D points instead of angles\n",bcolors.ENDC)
              outputMode = "3d"
              configuration['outputMode'] = "3d"
              configuration['groupOutputs'] = 3
           if (sys.argv[i]=="--dry"):
              print(bcolors.WARNING,"THIS WILL BE A DRY RUN JUST TO SPIT OUT A NETWORK\n",bcolors.ENDC)
              configuration['veryHighNumberOfEpochs']=1
              configuration['highNumberOfEpochs']=1
              configuration['defaultNumberOfEpochs']=1
              configuration['useOnlineHardExampleMining']=0
              memPercentage=1000
           if (sys.argv[i]=="--label"):
              label=sys.argv[i+1] 
           if (sys.argv[i]=="--halffloats"):
              useHalfFloats=1
              configuration['rememberWeights']=0
              setTensorflowBackendToHalfFloats()
           if (sys.argv[i]=="--validate"):
              print(bcolors.WARNING,"Will just attempt to validate data loading\n",bcolors.ENDC);
              memPercentage=5000 #<- load only 5000 samples
              doModelValidation = True
           if (sys.argv[i]=="--notest"):
              print("\nNot using Test");
              testMemPercentage=0.0
           if (sys.argv[i]=="--test"):
              print("\nUsing Test");
              testMemPercentage=1.0
           if (sys.argv[i]=="--cluster"):
              print("\nOperating as node ",int(sys.argv[i+1])+1," of a ",sys.argv[i+2]," part cluster ");
              jobsPerCluster=1
              jobsPerClusterStart=int(sys.argv[i+1])
              jobsPerClusterEnd=int(sys.argv[i+2])
           if (sys.argv[i]=="--compress"):
              print("\nCompression Ratio set to ",sys.argv[i+1]);
              networkCompression=float(sys.argv[i+1])
           if (sys.argv[i]=="--resume"): #Resume/Start have same functionality
              print("\Resuming from Job ",sys.argv[i+1]);
              startPosition=int(sys.argv[i+1])
           if (sys.argv[i]=="--start"):  #Resume/Start have same functionality
              print("\nStarting at Job ",sys.argv[i+1]);
              startPosition=int(sys.argv[i+1])
           if (sys.argv[i]=="--tensorboard"):
              print("\nWill autostart tensorboard");
              autostartTensorboard=1
           if (sys.argv[i]=="--mem"):
              print("\nMemory usage ",sys.argv[i+1]);
              memPercentage=float(sys.argv[i+1])
           #old "classifier" before classifier was a thing
           if (sys.argv[i]=="--categoryall"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_all" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
              #set up to do jobs 0-6
              startPosition=0
              jobsPerCluster=1
              jobsPerClusterStart=4
              jobsPerClusterEnd=5
              networkCompression=0.8
              highNumberOfEpochs=60
           #-----------------------------------------------
           #                New 4way split
           #-----------------------------------------------
           if (sys.argv[i]=="--dataset"):
              print("\nOverriding dataset ",dataFile," and using ",sys.argv[i+1]);
              dataFile=sys.argv[i+1]
           if (sys.argv[i]=="--all"):
              #Don't mix this --all with the step2_OrientatonClassifier.py --all
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_all" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
           if (sys.argv[i]=="--back"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_back" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
           if (sys.argv[i]=="--front"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_front" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
           if (sys.argv[i]=="--left"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_left" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
           if (sys.argv[i]=="--right"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_right" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
           #-----------------------------------------------

    if (len(configuration)==0):
       print(bcolors.FAIL,"Training cannot run without a configuration!",bcolors.ENDC)
       sys.exit(0)

    if (configuration['probabilisticOutput']==1):
       if (configuration['groupOutputs']!=1): 
          print(bcolors.FAIL,"Overriding group outputs setting and setting it to 1!",bcolors.ENDC)
          configuration['groupOutputs']=1


    #Resolve PCA name.. :(
    x = configuration['doPCA'].split("$",1)
    if (len(x)>0):
          #If there is a $ character this is our place holder to autocomplete our dataFile 
          print("PCA filename is resolved from ",configuration['doPCA'])
          print(" to ")
          configuration['doPCA']=x[0]+dataFile+".pca"
          print(configuration['doPCA'])



    print("Creating directory for training models "+outputDirectoryForTrainedModels)
    createDirectory(outputDirectoryForTrainedModels)
    #sys.exit(0)
    #----------------------------------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------------------------------

    
    #The default compatibility setting is the BMVC2019 2channel NSDM, however nowadays we use NSRM
    numberOfChannelsPerNSDMElement=2
    if (configuration['NSDMAlsoUseAlignmentAngles']==1):
       numberOfChannelsPerNSDMElement=1
    print("Number of Channels Per NSDM element ",numberOfChannelsPerNSDMElement)
    configuration['numberOfChannelsPerNSDMElement']=numberOfChannelsPerNSDMElement
 
    #Printout the current configuration with regard to what network to inherit from
    if (configuration['rememberWeights']):
       print("Reloading previous training results")
    else:
       print("Not reloading")


    #Populate configuration with extra context variables
    configuration['hierarchyPartName']               = hierarchyPartName
    configuration['dataFile']                        = dataFile
    configuration['outputDirectoryForTrainedModels'] = outputDirectoryForTrainedModels
    configuration['label']                           = label

    #----------------------------------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------------------------------
    #                               Read our training and test datasets
    #----------------------------------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------------------------------
    print("\n-------------------------------------------------");
    startAt = time.time()
     
    #Special case if the user has a set memory requirement we try to mimic it for test as well..
    if (testMemPercentage==1.0):
       if (memPercentage==0): testMemPercentage=memPercentage
       if (memPercentage>1):  testMemPercentage=memPercentage

    #Hardcoded path for dataset if using CSV file
    #datasetDirectory="/home/ammar/Documents/Programming/DNNTracker/DNNTracker/dataset"
    #datasetDirectory="../../DNNTracker/dataset"
    datasetDirectory="dataset/generated/"


    #Easier and faster way to get output list..
    if (useCSVBackend):
       inputLabelsWithoutNSDM = readCSVLabels("%s/2d_%s.csv" % (datasetDirectory,dataFile))
       outputLabels           = readCSVLabels("%s/%s_%s.csv" % (datasetDirectory,configuration['outputMode'],dataFile)) #configuration['outputMode'] is either bvh or 3d
    else:
       inputLabelsWithoutNSDM = groundTruthTrain["labelInNoNSDM"]
       outputLabels           = groundTruthTrain["labelOut"] 


    if (not checkIfConfigurationHierarchyIsTheSameAsLabelList(configuration,inputLabelsWithoutNSDM,0)):
       print(bcolors.WARNING +"step0_DNNTrainer.py : Inconsistent Configuration association compared to the inputLabelList" +  bcolors.ENDC )
       sys.exit(1)


    print("Input Labels (no NSDM , %u elements ) : " % (len(inputLabelsWithoutNSDM)) );
    print(inputLabelsWithoutNSDM)

    print("\nOutput Labels ( %u elements ) : "% (len(outputLabels)) );
    print(outputLabels)

    numberOfOutputColumns=len(outputLabels)
    #Initially we would have to do all outputs ourself
    endPosition=numberOfOutputColumns

    #But if a cluster has been set up we should only do *some* of the training
    if (jobsPerCluster):
        startPosition = jobsPerClusterStart
        endPosition   = jobsPerClusterEnd


    configuration['startPosition']                   = startPosition
    configuration['endPosition']                     = endPosition 
    print("\nWill run from ",startPosition," to ",endPosition," / ",numberOfOutputColumns);

    RAMBefore = getRAMInformation()
    print("Initial | free RAM ",RAMBefore['free']," KB");


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
    
    #If we want to use the CSV loading back end here
    #otherwise we assume groundTruthTrain and groundTruthTest have been already provided
    if (useCSVBackend):
      #---------------------------------------------------------------
      groundTruthTrain = readGroundTruthFile(
                                             configuration,
                                             "Train",
                                             "%s/2d_%s.csv"  % (datasetDirectory,dataFile),
                                             "%s/%s_%s.csv"  % (datasetDirectory,configuration['outputMode'],dataFile), #configuration['outputMode'] is either bvh or 3d
                                             memPercentage,
                                             numberOfChannelsPerNSDMElement,
                                             configuration['useRadians'],
                                             useHalfFloats
                                            )
      #---------------------------------------------------------------
      if (testMemPercentage>0):
        groundTruthTest  = readGroundTruthFile( 
                                               configuration,
                                               "Test",
                                               "%s/2d_%s_test.csv" % (datasetDirectory,dataFile),
                                               "%s/%s_%s_test.csv" % (datasetDirectory,configuration['outputMode'],dataFile), #configuration['outputMode'] is either bvh or 3d
                                               testMemPercentage,
                                               numberOfChannelsPerNSDMElement,
                                               configuration['useRadians'],
                                               useHalfFloats
                                              )
        checkIfTestAndTrainListsAreOk(groundTruthTest,groundTruthTrain)
        configuration['testOutputLabels'] = groundTruthTest['labelOut']
      #--------------------------------------------------------------- 
    configuration['trainingSamples']   = groundTruthTrain["out"].shape[0] 
    configuration['trainOutputLabels'] = groundTruthTrain['labelOut']
    if not 'testOutputLabels' in configuration: 
             configuration['testOutputLabels'] = list()

    #Offset normalization for train/test dataset..!
    #---------------------------------------------------------------
    print("Normalization code activated always : ")
    #First calculate the offsets..
    configuration    = calculateOutputNormalization(configuration,outputDirectoryForTrainedModels,groundTruthTrain)
    #Then apply the offsets in both train and test data ( if test data exists ) 
    groundTruthTrain = executeNormalization(configuration,groundTruthTrain)
    checkDataForNaN(groundTruthTrain)
    if (testMemPercentage>0):
               groundTruthTest = executeNormalization(configuration,groundTruthTest)
               checkDataForNaN(groundTruthTest)
    #---------------------------------------------------------------

    #Update neuralNetworkLists..  
    dumpListToFile(outputDirectoryForTrainedModels+"/neuralNetworkJoints.list" ,groundTruthTrain['labelInClean'])
    dumpListToFile(outputDirectoryForTrainedModels+"/neuralNetworkInputs.list" ,groundTruthTrain['labelIn'])
    dumpListToFile(outputDirectoryForTrainedModels+"/neuralNetworkOutputs.list",groundTruthTrain['labelOut'])

    if (doModelValidation):
        numberOfSamples = groundTruthTrain["in"].shape[0]
        numberOfOutputs = groundTruthTrain["in"].shape[1]
        from tools import appendCSVToFile
        for sampleID in range(0,numberOfSamples):
           appendCSVToFile("validateInput.csv", groundTruthTrain["in"][sampleID], fID=sampleID)
           appendCSVToFile("validateOutput.csv",groundTruthTrain["out"][sampleID],fID=sampleID)
        print("Ending after validation check..")
        sys.exit(1)

    RAMAfter = getRAMInformation()
    print("After reading the dataset | free RAM ",RAMAfter['free']," KB");
    #----------------------------------------------------------------------------------------------------------
    endAt = time.time()
    print("Memory Occupied ",(RAMAfter['used']-RAMBefore['used'])/1024," MB")
    print("Time required to load from disk was ",(endAt-startAt)/60," mins")
    print("\n-------------------------------------------------");
    #----------------------------------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------------------------------

    if (not checkIfConfigurationHierarchyIsTheSameAsLabelList(configuration,groundTruthTrain['labelIn'],1)):
       print(bcolors.WARNING +"step0_DNNTrainer.py : Inconsistent Configuration association compared to the training label input" +  bcolors.ENDC )
       sys.exit(1)

 
    #At this point we are ready..
    #We should free whatever memory we have allocated because it is training time..!
    #------------------------
    gc.collect()
    #------------------------

    #Shorthand to be able to do tricks without altering configuration
    #-----------------------------------------------------------
    numberOfOutputsOfEachEncoder = configuration['groupOutputs']
    if (numberOfOutputsOfEachEncoder==0):
           #This special case means that the encoder should span everything automatically..!
           numberOfOutputsOfEachEncoder = numberOfOutputColumns #len(groundTruthTest['out'][0])
           print(bcolors.WARNING + "step0_DNNTrainer.py : Setting numberOfOutputsOfEachEncoder = %u" % numberOfOutputsOfEachEncoder +  bcolors.ENDC )
    #-----------------------------------------------------------
    #-------------------------------------------------------------------------------------------------------------
    previousModelCached = newTrivialModel("dummy",666,len(groundTruthTrain['in'][0]),numberOfOutputsOfEachEncoder)
    previousModelType   = 666 #something crazy that does not trigger at first loop
    currentModelType    = 0
    outputName          = "none"
    previousOutputName  = "none"
    #-------------------------------------------------------------------------------------------------------------
    
    #Cast NumpyArray as a tensor to make sure it has a constant signature on GPU
    #and reduce training latencies 
    #----------------------------------------------------------------------------
    trainIn = tf.convert_to_tensor(groundTruthTrain['in'],dtype=tf.float32)
    testIn  = []
    if (testMemPercentage>0):
         testIn  = tf.convert_to_tensor(groundTruthTest['in'],dtype=tf.float32)
    #----------------------------------------------------------------------------


    for outputType in range(startPosition,endPosition,numberOfOutputsOfEachEncoder):
        print("-----------------------------------------------------------------------\n")
        startAt = time.time()
        previousOutputName=outputName

        if (configuration['probabilisticOutput']==1):
           outputName = groundTruthTrain['labelOut'][outputType]
           filename   = "%s/bvh_%s_%s.csv"  % (datasetDirectory,dataFile,outputName) 
           csvFile    = readCSVFile(filename,memPercentage,useHalfFloats)
           trainOut   = csvFile['body']
           testOut    = []
           numberOfOutputsOfEachEncoder = len(csvFile['body'][0])
           median     = 0
           mean       = 0
           std        = 1 #So that not everything is considered trivial..
           var        = 0
        else:
           #if we have multiple outputs groupped we want to concatenate all their names to keep track of all handled joints
           #----------------------------------------------------------------------------------------------------------------
           if (numberOfOutputsOfEachEncoder==numberOfOutputColumns):
              #Guard against : File name too long [Op:CreateSummaryFileWriter]  
              outputName="everything"
           elif (numberOfOutputsOfEachEncoder==1):
              outputName=groundTruthTrain['labelOut'][outputType]
           elif (numberOfOutputsOfEachEncoder>1):
              outputName=""
              isFirst=1
              for nameToConcatenate in range(outputType,outputType+numberOfOutputsOfEachEncoder):
                  if (isFirst==0):
                      outputName=outputName+"-" #so that output is "3DX_hip_3DY_hip_3DZ_hip" instead of "3DX_hip3DY_hip3DZ_hip"
                  outputName=outputName + groundTruthTrain['labelOut'][nameToConcatenate]
                  isFirst=0
           #----------------------------------------------------------------------------------------------------------------


           print(bcolors.OKGREEN,"\n\n\nOutput ",outputType,"/",endPosition," - ",outputDirectoryForTrainedModels," - ",outputName,"\n",bcolors.ENDC)
           if (testMemPercentage>0):
             testOutNP = splitNumpyArray(groundTruthTest['out'],outputType,numberOfOutputsOfEachEncoder,useHalfFloats)
             testOut   = tf.convert_to_tensor(testOutNP,dtype=tf.float32)
           else:
             testOutNP = []
             testOut   = []
           #------------------------------------------------------------------------------------------------------- 
           trainOutNP = splitNumpyArray(groundTruthTrain['out'],outputType,numberOfOutputsOfEachEncoder,useHalfFloats)
           trainOut   = tf.convert_to_tensor(trainOutNP,dtype=tf.float32)

           #-----------------------------------------------------------------------------------------------------------------------
           #                                      EXTRACT SOME STATISTICS FOR OUR OUTPUT
           #-----------------------------------------------------------------------------------------------------------------------
           if (numberOfOutputsOfEachEncoder==1):
               from readCSV import resolveNormalizationRules
               column,resolved = resolveNormalizationRules(configuration,outputName)
               if (resolved):
                  print("Output Normalization offset : ",configuration["outputOffsetValues"][column])
                  print("Output Normalization scalar : ",configuration["outputScalarValues"][column])
                  print("Output Normalization minima : ",configuration["outputOffsetMinima"][column])
                  print("Output Normalization maxima : ",configuration["outputOffsetMaxima"][column])
               else:
                  print(bcolors.FAIL,"Failed resolving ",outputName," against ",configuration["outputOffsetLabels"],bcolors.ENDC)
                 
           print("Now getting fresh output stats ... ")
           minimum = np.min(trainOutNP)
           maximum = np.max(trainOutNP)
           median  = np.median(trainOutNP)
           mean    = np.mean(trainOutNP)
           from tools import calculateStandardDeviationInPlaceKnowingMean,convertStandardDeviationToVariance
           std     = calculateStandardDeviationInPlaceKnowingMean(trainOutNP,mean)
           var     = convertStandardDeviationToVariance(std)
           title_string=" `%s` : Min/Max=%0.2f/%0.2f,Median=%0.2f,Mean=%0.2f,Std=%0.2f,Var=%0.2f" % (outputName,minimum,maximum,median,mean,std,var)
           print("Statistics of Output ",title_string)
           endAt = time.time()
           print("Time required to extract statistics ",outputName," was ",(endAt-startAt)," seconds")
           #-----------------------------------------------------------------------------------------------------------------------


        #-----------------------------------------------------------------------------------------------------------------------
        #                 DECIDE ON WHAT MODEL/PRIORITY WE WANT FOR PARTICULAR OUTPUT/CONFIGURATION
        #-----------------------------------------------------------------------------------------------------------------------
        startAt = time.time()
        finalOutputPathForModel = ("%s/%s") % (outputDirectoryForTrainedModels,outputName)
        #-----------------------------------------------------------------------------------------------------------------------
        #print("The parent of network ",outputName," is ",parentNetwork)
        #rememberNetworkPath="%s/%s"%(outputDirectoryForTrainedModels,parentNetwork)
        #if ( (configuration['rememberWeights']) and (parentNetwork!="none")):
        #   print("Will remember from ",rememberNetworkPath)
        #-----------------------------------------------------------------------------------------------------------------------


        outputDataStandardDeviation = std
        from modelDecisionCriteria import decideOnWhatModelToUseForOutput
        mocapNetStep0,numberOfEpochs,numberOfBatchSize,currentModelType,modelIsTrivial = decideOnWhatModelToUseForOutput(
                                                                                                                         configuration,
                                                                                                                         outputType,
                                                                                                                         outputName,
                                                                                                                         len(groundTruthTrain['in'][0]), #numberOfInputsOfEachEncoder
                                                                                                                         len(inputLabelsWithoutNSDM),#numberOfInputsWithoutNSDMOfEachEncoder,
                                                                                                                         numberOfChannelsPerNSDMElement,
                                                                                                                         numberOfOutputsOfEachEncoder,
                                                                                                                         previousOutputName,
                                                                                                                         previousModelCached,
                                                                                                                         previousModelType,
                                                                                                                         outputDataStandardDeviation,
                                                                                                                         outputDirectoryForTrainedModels
                                                                                                                        )
 
        if (onlyExecuteTrainingOfSpecificModelType!=0):
            executeTraining=0
            if (onlyExecuteTrainingOfSpecificModelType==currentModelType):
                executeTraining=1

        #-----------------------------------------------------------------------------------------------------------------
        RAMBefore = getRAMInformation()
        print("Before executing training | free RAM ",RAMBefore['free']," KB");
        from  DNNModel import countModelParameters
        try:
           #Report MAX number of parameters!
           newNumberOfParameters = int(countModelParameters(mocapNetStep0))
           print("new number of parameters => ",newNumberOfParameters)
           if 'modelParameters' in configuration:
               print("old number of parameters => ",configuration['modelParameters'])
               if (int(configuration['modelParameters'])<int(newNumberOfParameters)):
                             print("updating => ",newNumberOfParameters)
                             configuration['modelParameters'] = newNumberOfParameters
               else:
                             print("not updating number of parameters")

           else:
               print("storing as first number of parameters => ",newNumberOfParameters)
               configuration['modelParameters'] = newNumberOfParameters
        except:
           print("Failed counting model parameters")
        #-----------------------------------------------------------------------------------------------------------------
        #--------------------------------------------------- TRAIN -------------------------------------------------------
        #-----------------------------------------------------------------------------------------------------------------
        if executeTraining:
           tensorboardLabel = "%s/tensorboard/%s" % (outputDirectoryForTrainedModels,outputName)
           os.system("rm -rf %s" % (tensorboardLabel) ) # Wipe the particular tensorboard label if it exists, reason being that otherwise it is impossible to understand what is happening
           if ( autostartTensorboard==1 ):
               os.system("tensorboard --logdir %s/tensorboard&" % (outputDirectoryForTrainedModels) )
               os.system("xdg-open http://127.0.0.1:6006&")
               autostartTensorboard=2 # <- make sure this gets executed only once

           #---------------------------------------------------------------------
           #We use a checkpoint system to return best model..
           if ( (configuration['useOnlineHardExampleMining']) and (currentModelType!=1) ):
                #only use fancy training on non-trivial models..
                history,mocapNetStep0,metrics = onlineHardExampleMiningTraining(
                                                                        tensorboardLabel,
                                                                        mocapNetStep0,
                                                                        numberOfEpochs,
                                                                        configuration['hardMiningEpochs'],
                                                                        configuration['normalMiningEpochs'],
                                                                        numberOfBatchSize,
                                                                        configuration['earlyStoppingPatience'],
                                                                        trainIn,
                                                                        trainOut,
                                                                        testIn,
                                                                        testOut,
                                                                        configuration['minEarlyStoppingDelta'], #min loss delta  0.0001
                                                                        modelIsTrivial=modelIsTrivial,
                                                                        haveTestSet=(testMemPercentage>0),
                                                                        useHalfFloats=useHalfFloats
                                                                       )
           else:
                history,mocapNetStep0,metrics = regularTraining(
                                                        tensorboardLabel,
                                                        mocapNetStep0,
                                                        numberOfEpochs,
                                                        numberOfBatchSize,
                                                        configuration['earlyStoppingPatience'],
                                                        trainIn,
                                                        trainOut,
                                                        testIn,
                                                        testOut,
                                                        configuration['minEarlyStoppingDelta'], #min loss delta  0.0001
                                                        modelIsTrivial=modelIsTrivial,
                                                        haveTestSet=(testMemPercentage>0),
                                                        useHalfFloats=useHalfFloats
                                                       )

           logTrainingResults( 
                               ("%s/summary.html") % (outputDirectoryForTrainedModels),
                               configuration,
                               outputType,
                               outputName,
                               history,
                               metrics
                             )
           #---------------------------------------------------------------------

           #Free space occupied by split array..
           del trainOutNP
           del trainOut
           if (testMemPercentage>0):
              del testOutNP
              del testOut
           gc.collect()

           #---------------------------------------------------------------------
           #-------------------- STORE CONFIGURATION ----------------------------
           #---------------------------------------------------------------------
           endAt = time.time()
           print("Time required to train ",outputName," was ",(endAt-startAt)/60," mins")

           if (saveResultsToDisk):
               #----------------------------------------------------------------------------------------------------------------
               saveConfiguration(
                                 outputDirectoryForTrainedModels,
                                 mocapNetStep0,
                                 outputName,
                                 mocapNetStep0.count_params(),
                                 numberOfEpochs,
                                 numberOfBatchSize,
                                 groundTruthTrain['labelIn'],
                                 groundTruthTrain['labelOut'],
                                 outputType,
                                 history,
                                 startAt,
                                 endAt,
                                 median,mean,std,var
                                )
               finalOutputPathForModel = ("%s/%s") % (outputDirectoryForTrainedModels,outputName)
               print("Saving resulting encoder/model...\n")
               saveModel(finalOutputPathForModel,mocapNetStep0)
               saveLastCompletedJob(finalOutputPathForModel,outputType,endPosition,(endPosition-outputType)*(endAt-startAt)/60 )
               #----------------------------------------------------------------------------------------------------------------
               #End of training encoder
               #----------------------------------------------------------------------------------------------------------------
           notification("MocapNET Training","Done training %s/%s @ %0.2f min"% (finalOutputPathForModel,outputType,(endAt-startAt)/60))
           if (not modelIsTrivial):
               print("Model is not trivial so using it as previous model..!")
               #When you have a trivial model then this makes the previous model mechanism useless
               # so only updating non trivial models..
               previousModelCached=mocapNetStep0
               previousModelType=currentModelType
           else:
               print("Model is trivial so NOT using it as previous model..!")
           #---------------------------------------------------------------------------------------------------------

    #---------------------------------------------------------------------------------------------------------
    print("Copying configuration details after training...\n")
    #---------------------------------------------------------------------------------------------------------
    if ("eigenPoses" in configuration):
        if (configuration['eigenPoses']==1):
           os.system('cp %s/2d_%s_eigenposes.csv %s/' % (datasetDirectory,dataFile,outputDirectoryForTrainedModels))
           os.system('cp %s/%s_%s_eigenposes.csv %s/' % (datasetDirectory,configuration['outputMode'],dataFile,outputDirectoryForTrainedModels))
           os.system('cp %s/*%s_eigenposes.csv %s/'   % (datasetDirectory,dataFile,outputDirectoryForTrainedModels))
    os.system('cp %s %s/' % (configurationPath,outputDirectoryForTrainedModels))
    os.system('cp %s/../DNNModel.py %s/' % (outputDirectoryForTrainedModels,outputDirectoryForTrainedModels))
    os.system('cp %s/../NSDM.py %s/' % (outputDirectoryForTrainedModels,outputDirectoryForTrainedModels))
    os.system('cp %s %s/' % (configuration['doPCA'],outputDirectoryForTrainedModels))
    os.system('git log -1 > %s/git' % (outputDirectoryForTrainedModels))
    #---------------------------------------------------------------------------------------------------------
    if 'decompositionEngine' in configuration: 
              for intermediateFileThatMustBeCopied in configuration['decompositionEngine'].trackedFiles:
                  print(bcolors.OKGREEN,"Copying decomposition engine file ",intermediateFileThatMustBeCopied," ..",bcolors.ENDC)
                  os.system('mv %s %s/' % (intermediateFileThatMustBeCopied,outputDirectoryForTrainedModels))
    #---------------------------------------------------------------------------------------------------------
    #Also store the I/O list as an easily parseable file..
    dumpListToFile(outputDirectoryForTrainedModels+"/neuralNetworkJoints.list" ,groundTruthTrain['labelInClean'])
    dumpListToFile(outputDirectoryForTrainedModels+"/neuralNetworkInputs.list" ,groundTruthTrain['labelIn'])
    dumpListToFile(outputDirectoryForTrainedModels+"/neuralNetworkOutputs.list",groundTruthTrain['labelOut'])
    dumpListToFile(outputDirectoryForTrainedModels+"/neuralNetworkOutputsOriginal.list",groundTruthTrain['labelOut'])
    #---------------------------------------------------------------------------------------------------------
           
    #-----------------------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------------------------------------
    endAt = time.time()

    finalMessage="Successfully trained for %0.2f mins ( %0.2f days ) using a l=%0.2f"%((endAt-startEverythingAt)/60 , (endAt-startEverythingAt)/(60*60*24) , configuration['lamda'] )
    saveProgressEnded( finalMessage )
    print(" %s "%finalMessage)
    print("Sucessfully ended..\n")

    if ( autostartTensorboard==2 ):
               os.system("killall tensorboard&")
               autostartTensorboard=0

    #Print the next command in case run manually to enable copy paste :P !
    nextCommand="step1_DNNCombine.py " 
    for i in range(1, len(sys.argv)):
        nextCommand=nextCommand+sys.argv[i]+" "
    print(nextCommand)

if __name__ == '__main__':
    #When working standalone assume simple csv backend
    emptyData     = dict()
    emptyTestData = { 'labelInNoNSDM': [], 'labelIn': [], 'in': [], 'labelOut':[], 'out': [] }
    useCSVBackend=1
    step0_DNNTrainer(useCSVBackend,emptyData,emptyTestData)
