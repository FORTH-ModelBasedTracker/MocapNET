#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""
#TODO : FIX 3D MODE! 
import os 
import sys
import gc
import time

#--------------------------------------------------
import numpy as np 
#--------------------------------------------------
from readCSV     import parseConfiguration,checkIfConfigurationHierarchyIsTheSameAsLabelList,readCSVLabels,readCSVFile,checkIfTestAndTrainListsAreOk,readGroundTruthFile,splitNumpyArray
from DNNModel    import setupDNNModelsUsingJSONConfiguration
from DNNTraining import printTrainingVersion,regularTraining,onlineHardExampleMiningTraining
from tools       import bcolors,dumpListToFile,getRAMInformation,createDirectory,checkIfFileExists,getConfigurationJointIsDeclaredInHierarchy,getConfigurationJointPriority,getParentNetwork,notification
from getModelFromDatabase import retrieveAndSetupModel,retrieveAndSetupBasedOnSerial

print("evaluateMocapNET starting up..\n")
printTrainingVersion()
print("""
███████╗██╗   ██╗ █████╗ ██╗     ██╗   ██╗ █████╗ ████████╗███████╗
██╔════╝██║   ██║██╔══██╗██║     ██║   ██║██╔══██╗╚══██╔══╝██╔════╝
█████╗  ██║   ██║███████║██║     ██║   ██║███████║   ██║   █████╗  
██╔══╝  ╚██╗ ██╔╝██╔══██║██║     ██║   ██║██╔══██║   ██║   ██╔══╝  
███████╗ ╚████╔╝ ██║  ██║███████╗╚██████╔╝██║  ██║   ██║   ███████╗
╚══════╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝
                                                                   
""")

print("""
██████╗  ██████╗ ██████╗ ██╗   ██╗
██╔══██╗██╔═══██╗██╔══██╗╚██╗ ██╔╝
██████╔╝██║   ██║██║  ██║ ╚████╔╝ 
██╔══██╗██║   ██║██║  ██║  ╚██╔╝  
██████╔╝╚██████╔╝██████╔╝   ██║   
╚═════╝  ╚═════╝ ╚═════╝    ╚═╝ 
""")

print("Ensuring the same seed for reproductible results always..\n")
from numpy.random import seed
seed(1)
np.set_printoptions(precision=2) 
np.set_printoptions(formatter={"float_kind": lambda x: "%g" % x})
 

def setBackendToHalfFloats():
   dtype='float16'
   print("\nUsing Half-Floats for training ");
   

#-----------------------------------------------------------------------------------------------------------------------
def identifyProblematicOutput(mapPath):
  print("READING evaluateMocapNET.lastresults.csv")
  #-----------------------------------------------------------------
  losses   = readCSVFile("evaluateMocapNET.lastresults.csv" , 1.0 , 0)
  #-----------------------------------------------------------------
  f = open("evaluateMocapNET.identified.csv", "w") 
  f.write("SampleID,MAE,BVH,fID,cumulative\n")
  # Using readlines()
  file1 = open(mapPath, 'r')
  Lines = file1.readlines() 
  #-----------------------------------------------------------------
  minimum=np.min(losses['body'])
  maximum=np.max(losses['body'])
  median=np.median(losses['body'])
  mean=np.mean(losses['body'])
  std=np.std(losses['body'])
  var=np.var(losses['body'])
  print(" Min/Max=%0.2f/%0.2f,Median=%0.2f,Mean=%0.2f,Std=%0.2f,Var=%0.2f" % (minimum,maximum,median,mean,std,var))
  sampleID = 0
  hits = 0
  previousBVHFile="dummy initial value"
  consecutiveHits=0
  errorThreshold=250.0
  for item in losses['body']:
      if (item>errorThreshold):
        bvhFile = Lines[sampleID+1].split(",")[0]
        fID     = int(Lines[sampleID+1].split(",")[1])
        #----------------------------------------------
        if (previousBVHFile!=bvhFile):
           consecutiveHits=0
           previousBVHFile=bvhFile
        else:
           consecutiveHits=consecutiveHits+1
        #---------------------------------------------- 
        print(sampleID," error=",item[0]," -> ",bvhFile," fID ",fID)
        f.write("%u,%f,%s,%u,%u\n"%(sampleID,item[0],bvhFile,fID,consecutiveHits))
        hits=hits+1
      sampleID=sampleID+1  
  #---------------------------------------------------------------
  print("Found ",hits," occurances of error > ",errorThreshold)
  file1.close()

#-----------------------------------------------------------------------------------------------------------------------
def appendRAWResultsForGNUplot(outputFile,globalJointDistances):
    fileHandler = open(outputFile, "w")
    fileHandler.write(outputFile)
    fileHandler.write("\n")
    for measurement in globalJointDistances:
        fileHandler.write(str(measurement))
        fileHandler.write("\n")
    fileHandler.close()
#-----------------------------------------------------------------------------------------------------------------------
def ground3DJointsFromBVH(bvh,rawBVHPrediction :dict):
        #Extract a BVH dict of BVH motion fields
        #Modify our BVH armature with the new BVH values
        if (bvh.modify(rawBVHPrediction)): 

           #Render to 2D/3D
           bvh.processFrame(0) #only have 1 frame ID

           #Retreive 2D/3D Values
           output2D = dict()
           output3D = dict()
           for jointID in range(0,bvh.numberOfJoints):
               #-------------------------------------------
               jointName   = bvh.getJointName(jointID).lower()
               #-------------------------------------------
               x3D,y3D,z3D = bvh.getJoint3D(jointID)
               output3D["3DX_"+jointName]=float(x3D)
               output3D["3DY_"+jointName]=float(y3D)
               output3D["3DZ_"+jointName]=float(z3D)
               #-------------------------------------------
               x2D,y2D = bvh.getJoint2D(jointID)
               output2D["2DX_"+jointName]=float(x2D)
               output2D["2DY_"+jointName]=float(y2D)
               #-------------------------------------------
        else:
           print(bcolors.FAIL,"We where unable to process the BVH output",bcolors.ENDC)

        return output3D

 
def evaluateMocapNET(useCSVBackend,groundTruthTrain,groundTruthTest):
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
    testMemPercentage=1.0

    #Use the whole training Dataset by default
    memPercentage=1.0

    #networkCompression=1.0
    useHalfFloats = 0
    useRadians    = 0

    #Resume training options
    step=1
    compareAllJoints=0
     
    #AutoStart Tensorboard, disabled by default!
    autostartTensorboard=0
   
    doDebugVisualization = False
    doProfiling          = False
    doOnlyIdentification = False

    #I/O settings
    outputDirectoryForTrainedModels = "step0FrontBody"
    dataFile                        = "body"
    hierarchyPartName               = "body"
    outputMode                      = "bvh" 
    
    useTestData                     = True

    
    doNNEveryNFrames                = 1
    doHCDPostProcessing             = 0
    hcdLearningRate                 = 0.001
    hcdEpochs                       = 30
    hcdIterations                   = 15

    #This needs to be populated using the --config argument otherwise session will fail..
    configuration = []
    configurationPath=""
  
    engine       = "onnx"
    label        = "evaluation"
    doProcrustes = 1

    # Sample call with some example parameters :  ( needs --all body to have all joints ) if you see MocapNET always
    # gets initiallized with upper/lower networks, this is just needed to correctly parse the ground truth test files
    #                          --getPCA 15 105 
    #       python3 -m evaluateMocapNET --getPCA 15 32 --config dataset/body_configuration.json --skip 5 --all body
    #       python3 -m evaluateMocapNET --config dataset/body_configuration.json --all body --skip 5 --mem 100 
    #       python3 -m evaluateMocapNET --getPCA 210 64 --mem 10 --config dataset/body_configuration.json --all body
    #       python3 -m evaluateMocapNET --get 128 --config dataset/body_configuration.json --all body
    #       python3 -m evaluateMocapNET --config dataset/body_configuration.json --all body --engine onnx --3d #<- Evaluate 3D data!
    #
    # To generate evaluateMocapNET.lastresults.csv and then evaluateMocapNET.identified.csv do :
    #       python3 -m evaluateMocapNET --config dataset/body_configuration.json --all body --mem 1.0 --engine onnx --traindataset
    #       python3 -m evaluateMocapNET --config dataset/body_configuration.json --all body --mem 10  --engine onnx --traindataset --identify
    #-----------------------------------------------------------------------------------------------------------------------------------------
    if (len(sys.argv)>1):
       print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--hcd"):
              doHCDPostProcessing  = 1
           if (sys.argv[i]=="--visualize"): 
              doDebugVisualization = True 
           if (sys.argv[i]=="--ik"):
               doHCDPostProcessing = 1
               hcdLearningRate     = float(sys.argv[i+1])
               hcdEpochs           = int(sys.argv[i+2])
               hcdIterations       = int(sys.argv[i+3])
           if (sys.argv[i]=="--nnsubsample"):
               doNNEveryNFrames    = int(sys.argv[i+1])
           if (sys.argv[i]=="--noprocrustes"):
              doProcrustes=0
           if (sys.argv[i]=="--3d"):
              print(bcolors.WARNING,"Evaluate in 3D mode\n",bcolors.ENDC)
              #if needed by the model this will get auto activated later..
              outputMode = "3d"
           if (sys.argv[i]=="--identify"):
              doOnlyIdentification=True
           if (sys.argv[i]=="--engine"):
              engine=sys.argv[i+1]
              print("Selecting engine : ",engine)
           if (sys.argv[i]=="--profile"):
              doProfiling=True
           if (sys.argv[i]=="--label"):
              label=sys.argv[i+1]
           if (sys.argv[i]=="--get"):
              serial =int(sys.argv[i+1]) 
              retrieveAndSetupBasedOnSerial(serial,download=1)
           if (sys.argv[i]=="--getPCA"):
              PCADimensions =int(sys.argv[i+1])
              parameterCount=int(sys.argv[i+2])
              label = "evaluation_pca%u_param%u" % (PCADimensions,parameterCount)
              print("Get ",label)
              retrieveAndSetupModel(PCADimensions,parameterCount,download=1)
           if (sys.argv[i]=="--config"):
              configurationPath=sys.argv[i+1]
              configuration = parseConfiguration(configurationPath)
              setupDNNModelsUsingJSONConfiguration(configuration)
           if (sys.argv[i]=="--mem"):
              print("\nMemory usage ",sys.argv[i+1]);
              memPercentage=float(sys.argv[i+1])
           if (sys.argv[i]=="--skip"):
              print("\nSkipping frames ",sys.argv[i+1]);
              step=int(sys.argv[i+1])
           if (sys.argv[i]=="--full"):
              print("\nComparing All Joints : ",sys.argv[i+1]);
              compareAllJoints=int(sys.argv[i+1]) 
           #-----------------------------------------------
           #                New 4way split
           #-----------------------------------------------
           if (sys.argv[i]=="--traindataset"):
              print("\n Using training data instead of test data");
              useTestData=False
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
       print(bcolors.FAIL,"Evaluation cannot run without a configuration!",bcolors.ENDC)
       sys.exit(0)
 

    #Resolve PCA name.. :(
    x = configuration['doPCA'].split("$")
    if (len(x)>0):
          #If there is a $ character this is our place holder to autocomplete our dataFile 
          print("PCA filename is resolved from ",configuration['doPCA'])
          print(" to ")
          configuration['doPCA']=x[0]+dataFile+".pca"
          print(configuration['doPCA'])



    print("Creating directory for training models "+outputDirectoryForTrainedModels)
    createDirectory(outputDirectoryForTrainedModels)
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

    #Auto activate  3D outputMode if needed
    if ('outputMode' in configuration) and (configuration['outputMode'] == "3d"):
         outputMode="3d"

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
                                                                "%s/%s_%s_eigenposes.csv"  % (datasetDirectory,outputMode,dataFile), # comparisonMode / configuration['outputMode'] is either bvh or 3d
                                                                1.0,
                                                                numberOfChannelsPerNSDMElement,
                                                                useRadians,
                                                                useHalfFloats
                                                               )


    datasetUsedLabel="Train"
    filenamePostfix=""
    if (useTestData):
           datasetUsedLabel="Test"
           filenamePostfix="_test"

    #Easier and faster way to get output list..
    if (useCSVBackend):
       inputLabelsWithoutNSDM = readCSVLabels("%s/2d_%s%s.csv" % (datasetDirectory,dataFile,filenamePostfix))
       outputLabels           = readCSVLabels("%s/%s_%s%s.csv" % (datasetDirectory,outputMode,dataFile,filenamePostfix)) #outputMode is either bvh or 3d
       mapPath                = "%s/map_%s%s.csv" % (datasetDirectory,dataFile,filenamePostfix)
    else:
       inputLabelsWithoutNSDM = groundTruthTrain["labelInNoNSDM"]
       outputLabels           = groundTruthTrain["labelOut"]
       #No Map Path will cause an error in --identify


    if (not checkIfConfigurationHierarchyIsTheSameAsLabelList(configuration,inputLabelsWithoutNSDM,0)):
       print(bcolors.WARNING +"step0_DNNTrainer.py : Inconsistent Configuration association compared to the inputLabelList" +  bcolors.ENDC )
       sys.exit(1)


    print("Input Labels (no NSDM) : ");
    print(inputLabelsWithoutNSDM)

    print("\nOutput Labels : ");
    print(outputLabels)

    numberOfOutputColumns=len(outputLabels)
    #Initially we would have to do all outputs ourself
    endPosition=numberOfOutputColumns

    #But if a cluster has been set up we should only do *some* of the training
    if (jobsPerCluster):
        startPosition = jobsPerClusterStart
        endPosition   = jobsPerClusterEnd


    print("\nWill run from ",startPosition," to ",endPosition," / ",numberOfOutputColumns);

    RAMBefore = getRAMInformation()
    print("Initial | free RAM ",RAMBefore['free']," KB");
    
    #If we want to use the CSV loading back end here
    #otherwise we assume groundTruthTrain and groundTruthTest have been already provided
    if (useCSVBackend):
      #---------------------------------------------------------------
        #Simplify..
        data2D  = readCSVFile("%s/2d_%s%s.csv" % (datasetDirectory,dataFile,filenamePostfix),testMemPercentage,useHalfFloats)
        dataBVH = readCSVFile("%s/%s_%s%s.csv" % (datasetDirectory,outputMode,dataFile,filenamePostfix),testMemPercentage,useHalfFloats)
        groundTruthTest = dict()
        groundTruthTest['labelIn']  = data2D['label']
        groundTruthTest['in']       = data2D['body']
        groundTruthTest['labelOut'] = dataBVH['label']
        groundTruthTest['out']      = dataBVH['body']
        checkIfTestAndTrainListsAreOk(groundTruthTest,groundTruthTrain)
      #--------------------------------------------------------------- 

    RAMAfter = getRAMInformation()
    print("After reading the dataset | free RAM ",RAMAfter['free']," KB");
    #----------------------------------------------------------------------------------------------------------
    endAt = time.time()
    print("Memory Occupied ",(RAMAfter['used']-RAMBefore['used'])/1024," MB")
    print("Time required to load from disk was ",(endAt-startAt)/60," mins")
    #
    print("\n-------------------------------------------------");
    #----------------------------------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------------------------------

    #We have lodaded everything as normal but we only want to do identification in this run!
    if (doOnlyIdentification):
        identifyProblematicOutput(mapPath)
        sys.exit(0)

    #At this point we are ready..
    #We should free whatever memory we have allocated because it is training time..!
    #------------------------
    gc.collect()
    #------------------------

    #Select a MocapNET class from tensorflow/tensorrt/onnx/tf-lite engines
    from MocapNET import easyMocapNETConstructor
    mnet = easyMocapNETConstructor(engine,
                                   doProfiling=doProfiling,
                                   doHCDPostProcessing = doHCDPostProcessing,
                                   hcdLearningRate     = hcdLearningRate,
                                   hcdEpochs           = hcdEpochs,
                                   hcdIterations       = hcdIterations
                                  ) #<- do not use post processing to actually evaluate MocapNET NN
    mnet.test()

    from align3DPoints import compareGroundTruthToPrediction
    
    #                                                      Use body to grab all joints
    #python3 evaluateMocapNET.py --mem 50 --config dataset/body_configuration.json --all upperbody
    #-----------------------------------------------
    totalSamples            = 0
    totalError              = 0.0
    averageErrorDistances   = list()
    allErrorDistanceSamples = list()
    errorSamplesPerJoint    = dict()
    #-----------------------------------------------
    numberOfSamples = len(groundTruthTest['in'])

    #Declare joints we are interested in..
    #-----------------------------------------
    JOINTS_TO_COMPARE=list()
    if (compareAllJoints==0):
        JOINTS_TO_COMPARE.append('neck')        #0
        JOINTS_TO_COMPARE.append('rshoulder')   #1
        JOINTS_TO_COMPARE.append('relbow')      #2
        JOINTS_TO_COMPARE.append('rhand')       #3
        JOINTS_TO_COMPARE.append('lshoulder')   #4
        JOINTS_TO_COMPARE.append('lelbow')      #5
        JOINTS_TO_COMPARE.append('lhand')       #6
        JOINTS_TO_COMPARE.append('hip')         #7
        JOINTS_TO_COMPARE.append('rhip')        #8
        JOINTS_TO_COMPARE.append('rknee')       #9
        JOINTS_TO_COMPARE.append('rfoot')       #10
        JOINTS_TO_COMPARE.append('lhip')        #11
        JOINTS_TO_COMPARE.append('lknee')       #12
        JOINTS_TO_COMPARE.append('lfoot')       #13
    else:
        print(bcolors.FAIL,"TODO: IMPLEMENT COMPARE ALL JOINTS",bcolors.ENDC)
        sys.exit(1)
    
    #Allocate errors per joint
    #-----------------------------------------
    if (len(JOINTS_TO_COMPARE)==0):
      numberOfJoints  = len(configuration["hierarchy"])
      for jID in range (0,numberOfJoints):
            jointName = configuration["hierarchy"][jID]["joint"].lower()
            errorSamplesPerJoint[jointName]=list()
            JOINTS_TO_COMPARE.append(jointName) 
    else:
      numberOfJoints  = len(JOINTS_TO_COMPARE)
      for jID in range (0,numberOfJoints):
            jointName = JOINTS_TO_COMPARE[jID].lower()
            errorSamplesPerJoint[jointName]=list()
    #-----------------------------------------
    print("Doing evaluation based on errors of the following ",numberOfJoints," joints : ",JOINTS_TO_COMPARE)


    f = open("evaluateMocapNET.lastresults.csv", "w") 
    #Append CSV Log
    f.write("mae_from_%u_to_%u_step_%u\n" % (0,numberOfSamples,step))

    framerates     = list()
    beginTime      = time.time()
    evaluationMode = "?"
    frameNumber    = 0 #<- simpler counter for nn subsampling
    for thisSample in range(0,numberOfSamples,step) : 
            thisInputRaw     = groundTruthTest['in'][thisSample]
            correctOutputRaw = groundTruthTest['out'][thisSample] 
                            
            regressedResult  = dict()
            correctResult    = dict()


            #DATA Looks like this : { 'labelInNoNSDM':names2D, 'labelIn':inputLabels, 'in':npInput, 'labelOut':outputLabels, 'out':npOutput };
            thisInput = dict()
            for i in range(0,len(groundTruthTest['labelIn'])):
                  thisLabelIn = groundTruthTest['labelIn'][i].lower()
                  if ( ( thisLabelIn.find("edm-")== -1 )  and ( thisLabelIn.find("nsrm-")== -1 ) ): 
                         thisInput[thisLabelIn] = float(groundTruthTest['in'][thisSample][i])

            #If we are dealing with just BVH output use training BVH output to extract 3D data 
            #------------------------------------------------------------------------------------------------------
            if outputMode=="bvh":# (not 'outputMode' in configuration) or (configuration['outputMode'] == "bvh"):
                  evaluationMode="bvh"
                  correctOutput = dict()
                  for i in range(0,len(groundTruthTest['labelOut'])):
                      correctOutput[groundTruthTest['labelOut'][i]] = float(groundTruthTest['out'][thisSample][i])
                  correctResult = ground3DJointsFromBVH(mnet.bvh,correctOutput)
            #------------------------------------------------------------------------------------------------------
            #If we are dealing with 3D output immediately use training 3D output to compare 3D data 
            elif outputMode=="3d":# ('outputMode' in configuration) and (configuration['outputMode'] == "3d"):
                  evaluationMode="3d"
                  correctResult = dict()
                  for i in range(0,len(groundTruthTest['labelOut'])):
                      correctResult[groundTruthTest['labelOut'][i]] = float(groundTruthTest['out'][thisSample][i])
            #------------------------------------------------------------------------------------------------------
            startTime = time.time()
            #================================================
            #================================================
            doNN = 1
            if (doHCDPostProcessing) and (doNNEveryNFrames>1):
                doNN = (frameNumber%doNNEveryNFrames)==0
            #print("Frame ",frameNumber," doNN = ",doNN)
            regressedResult = mnet.predict3DJoints(thisInput,runNN=doNN)
            #================================================
            #================================================
            endTime = time.time()
            processing_time = endTime - startTime
            framerates.append(float(processing_time)) 
            if (processing_time==0): 
                        processing_time=1
            #------------------------------------------------------------------------------------------------------

            #------------------------------------------------------------------------------------------------------
            #------------------------------------------------------------------------------------------------------
            #------------------------------------------------------------------------------------------------------
            # Debug visualization (we shouldn't be doing a visualization in evaluateMocapNET.py)
            #------------------------------------------------------------------------------------------------------
            if (doDebugVisualization):
                import cv2
                import numpy as np
                bgcolor = 30
                annotated_image = np.full((1080,1920,3), bgcolor , dtype=np.uint8) #<-- 
                from MocapNETVisualization import visualizeMocapNETEnsemble
                image,plotImage = visualizeMocapNETEnsemble(mnet,annotated_image)

                cv2.imshow('MocapNET 4 Evaluation', image)
                if cv2.waitKey(1) & 0xFF == 27:
                      break            
            #------------------------------------------------------------------------------------------------------
            #------------------------------------------------------------------------------------------------------
            #------------------------------------------------------------------------------------------------------

            #print("\n\n\n\nthisInput ",thisInput)
            #print("\n\n\n\ncorrectOutput ",correctOutput)
            #print("\n\n\n\regressedResult ",regressedResult)
            #print("\n\n\n\n")
            stats = compareGroundTruthToPrediction(
                                                    configuration,
                                                    correctResult,    #<- We want to compare this 
                                                    regressedResult,  #<- to This 
                                                    doProcrustes=doProcrustes,
                                                    allowProcrustesToChangeScale=1,
                                                    jointsToCompare=JOINTS_TO_COMPARE
                                                  )

            #Append CSV log
            f.write("%f\n" % (float(stats["meanAverageError"])))

            #print("\nstats ",stats)
            print("\nstats={",end="")
            for k in stats.keys():
                print("'%s':%0.2f, "%(k,stats[k]),end="")
            print("}\n")


            progressMessage = "Progress : %0.2f %%   %u / %u    %0.2f fps" % ((100*thisSample)/numberOfSamples,thisSample,numberOfSamples,1/processing_time) 
            print(progressMessage," Evaluation Mode :",bcolors.OKGREEN,evaluationMode,bcolors.ENDC,"\n\n\n\n")
            
            #numberOfJoints = len(configuration["hierarchy"])
            for jID in range (0,numberOfJoints):
                jointName = JOINTS_TO_COMPARE[jID]
                if jointName in stats:
                   allErrorDistanceSamples.append(float(stats[jointName]))
                   errorSamplesPerJoint[jointName].append(float(stats[jointName]))
                   totalSamples = totalSamples + 1
                   totalError = totalError + float(stats[jointName])
                else:
                   print("Could not find ",jointName)

            averageErrorDistances.append(float(stats["meanAverageError"]))
            frameNumber = frameNumber + 1 #<- simpler counter for nn subsampling

    endTime = time.time()
    print("Finished in ",(endTime-beginTime)/60," minutes")

    f.close()


    import numpy as np
    appendRAWResultsForGNUplot("alldistance.raw",averageErrorDistances)
    #------------------------------------------
    for jID in range (0,numberOfJoints):
            jointName = JOINTS_TO_COMPARE[jID]
            appendRAWResultsForGNUplot("joint_%s.raw"%jointName,errorSamplesPerJoint[jointName])
    #------------------------------------------
    meanProcessingTime = np.mean(framerates)
    minimum            = np.min(averageErrorDistances)
    maximum            = np.max(averageErrorDistances)
    median             = np.median(averageErrorDistances)
    mean               = np.mean(averageErrorDistances)
    std                = np.std(averageErrorDistances)
    var                = np.var(averageErrorDistances)
    #------------------------------------------
    title_string=" Min/Max=%0.1f/%0.1f,Mean=%0.1f,Median=%0.1f,Std=%0.1f,Var=%0.1f" % (minimum,maximum,mean,median,std,var)
    
    #Show a summary 
    #------------------------------------------
    print("Flops : ",mnet.getModelFlops()," Model Params : ",mnet.getModelParameters())
    print("Upperbody Decomposition : ",mnet.ensemble["upperbody"].configuration["decompositionType"]," | Input Size : ",mnet.ensemble["upperbody"].configuration["PCADimensionsKept"])
    print("EDM upperbody : ",mnet.ensemble["upperbody"].configuration["EDM"])
    print("eNSRM upperbody : ",mnet.ensemble["upperbody"].configuration["eNSRM"])
    print("Lowerbody Decomposition : ",mnet.ensemble["lowerbody"].configuration["decompositionType"]," | Input Size : ",mnet.ensemble["lowerbody"].configuration["PCADimensionsKept"])
    print("EDM lowerbody : ",mnet.ensemble["lowerbody"].configuration["EDM"])
    print("eNSRM lowerbody : ",mnet.ensemble["lowerbody"].configuration["eNSRM"])
    print("Sample Step : %u  / Total Samples %u "%(step,numberOfSamples))
    print("Procrustes : %u"%doProcrustes) 
    print("Mean Framerate : %0.2f fps"%(1/meanProcessingTime))
    print("doHCDPostProcessing = ",doHCDPostProcessing)
    print("doNNEveryNFrames    = ",doNNEveryNFrames)
    print("hcdLearningRate     = ",hcdLearningRate)
    print("hcdEpochs           = ",hcdEpochs)
    print("hcdIterations       = ",hcdIterations)

    print("Result : %s "%title_string)
    notification("MocapNET Network Evaluation","Done evaluating -> %s " % (title_string))
    
    f = open(label+"_results.csv", "w")
    f.write("Params,PCA,FPS,Min,Max,Median,Mean,Std,Var,HCD,NNSubsample,HCD_LR,HCD_Epochs,HCD_Iterations\n")
    f.write("%0.2f"%(mnet.getModelParameters()/1000.0))
    f.write(",")
    f.write(str(mnet.ensemble["upperbody"].configuration["PCADimensionsKept"]))
    f.write(",")
    f.write("%0.2f"%(1/meanProcessingTime))
    f.write(",")
    f.write(str(minimum))
    f.write(",")
    f.write(str(maximum))
    f.write(",")
    f.write(str(median))
    f.write(",")
    f.write(str(mean))
    f.write(",")
    f.write(str(std))
    f.write(",")
    f.write(str(var))
    f.write(",")
    f.write(str(doHCDPostProcessing))
    f.write(",")
    f.write(str(doNNEveryNFrames))
    f.write(",")
    f.write(str(hcdLearningRate))
    f.write(",")
    f.write(str(hcdEpochs))
    f.write(",")
    f.write(str(hcdIterations))
    f.write("\n")
    f.close()

    #Plotting !
    #------------------------------------------
    import matplotlib.pyplot as plt

    fig, ax = plt.subplots(figsize=(8, 4))
    #------------------------------------------
    x = averageErrorDistances
    mu = 200
    sigma = 25
    n_bins = 100
    # plot the cumulative histogram
    n, bins, patches = ax.hist(x, n_bins, density=True, histtype='step',cumulative=True, label='Accuracy')
    #------------------------------------------
    # Add a line showing the expected distribution.
    y = ((1 / (np.sqrt(2 * np.pi) * sigma)) *
    np.exp(-0.5 * (1 / sigma * (bins - mu))**2))
    y = y.cumsum()
    y /= y[-1]
    #------------------------------------------
    #ax.plot(bins, y, 'k--', linewidth=1.5, label='Theoretical') 
    # tidy up the figure
    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Cumulative step histograms')
    ax.set_xlabel('3D Error (mm)')
    ax.set_ylabel('Likelihood of occurrence')
    #------------------------------------------
    #plt.show()
    print("Done")


if __name__ == '__main__':
    #When working standalone assume simple csv backend
    emptyData     = dict()
    emptyTestData = { 'labelInNoNSDM': [], 'labelIn': [], 'in': [], 'labelOut':[], 'out': [] }
    useCSVBackend=1
    evaluateMocapNET(useCSVBackend,emptyData,emptyTestData)
