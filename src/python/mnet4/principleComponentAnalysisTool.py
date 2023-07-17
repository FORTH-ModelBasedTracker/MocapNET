#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""
import sys
import numpy as np

from readCSV import parseConfiguration,splitNumpyArray,readGroundTruthFile
from DNNModel import setupDNNModelsUsingJSONConfiguration

OKGREEN = '\033[92m'
ENDC = '\033[0m'
FAIL = '\033[91m'

"""
Retreive the available system RAM
"""
def getRAMInformation():
    """
    Get node total memory and memory usage
    """
    with open('/proc/meminfo', 'r') as mem:
        ret = {}
        tmp = 0
        for i in mem:
            sline = i.split()
            if str(sline[0]) == 'MemTotal:':
                ret['total'] = int(sline[1])
            elif str(sline[0]) in ('MemFree:', 'Buffers:', 'Cached:'):
                tmp += int(sline[1])
        ret['free'] = int(tmp)
        ret['used'] = int(ret['total']) - int(ret['free'])
    return ret


if __name__ == '__main__':

    print("""
██████╗ ███████╗ ██████╗ ██████╗ ███╗   ███╗██████╗  ██████╗ ███████╗███████╗
██╔══██╗██╔════╝██╔════╝██╔═══██╗████╗ ████║██╔══██╗██╔═══██╗██╔════╝██╔════╝
██║  ██║█████╗  ██║     ██║   ██║██╔████╔██║██████╔╝██║   ██║███████╗█████╗  
██║  ██║██╔══╝  ██║     ██║   ██║██║╚██╔╝██║██╔═══╝ ██║   ██║╚════██║██╔══╝  
██████╔╝███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║██║     ╚██████╔╝███████║███████╗
╚═════╝ ╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝      ╚═════╝ ╚══════╝╚══════╝""")

    configuration = []
    configurationPath=""

    #Use the whole training Dataset by default
    memPercentage=1.0

    #networkCompression=1.0
    useHalfFloats = 0
    useRadians    = 0
  
    #I/O settings
    outputDirectoryForTrainedModels = "step0FrontBody"
    dataFile                        = "body"
    testFile                        = "body_test"
    hierarchyPartName               = "body"
    outputMode                      = "bvh"


    # Sample call with some example parameters : 
    #       python3 principleComponentAnalysisTool.py --config dataset/body_configuration.json --mem 1000 --highlight 3 --all body --show --mode 3

    highlightChannel = -1 #<- default shows PC4
    batch = 0
    mode  = 1 
    dimensions = 0
    decompositionType = "pca"
    viewA = 45
    viewB = 45
    
    visualize    = 0
    dumpRules    = 1
    addScreePlot = 0

    label = "not defined"
    if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--config"):
              configurationPath=sys.argv[i+1]
              configuration = parseConfiguration(configurationPath)
              setupDNNModelsUsingJSONConfiguration(configuration)
              decompositionType = configuration['decompositionType']
              dimensions = int(configuration['PCADimensionsKept'])
              if (configuration['precision']=="fp16"):
                 useHalfFloats=1
                 configuration['rememberWeights']=0
                 setBackendToHalfFloats()
              elif (configuration['precision']!="fp32"):
                 print(bcolors.WARNING,"Cannot use ",configuration['precision']," precision\n",bcolors.ENDC)
           if (sys.argv[i]=="--mem"):
              print("\nMemory usage ",sys.argv[i+1]);
              memPercentage=float(sys.argv[i+1])
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
           if (sys.argv[i]=="--type"):
              print("\nSet Mode to ",sys.argv[i+1]);
              decompositionType=sys.argv[i+1]
              configuration['decompositionType'] = decompositionType
           if (sys.argv[i]=="--dims"):
              print("\nSet Dimensions to ",sys.argv[i+1]);
              dimensions=int(sys.argv[i+1])
              configuration['PCADimensionsKept']=str(dimensions)
           if (sys.argv[i]=="--mode"):
              print("\nSet Mode to ",sys.argv[i+1]);
              mode=int(sys.argv[i+1])
           if (sys.argv[i]=="--highlight"):
              print("\nSet Highlight to ",sys.argv[i+1]);
              highlightChannel=int(sys.argv[i+1])
           if (sys.argv[i]=="--view"):
              print("\nSet View to ",sys.argv[i+1],sys.argv[i+2]);
              viewA=int(sys.argv[i+1])
              viewB=int(sys.argv[i+2])
           if (sys.argv[i]=="--batch"):
              batch=1
              visualize = 1
           if (sys.argv[i]=="--nosave"):
              dumpRules=0
           if (sys.argv[i]=="--show"):
              visualize=1
           if (sys.argv[i]=="--noscree"):
              addScreePlot=0


    if (decompositionType==""):
           print(OKGREEN,"Not doing everything because no decomposition type requested",ENDC) 
           sys.exit(0)
       

    #The default compatibility setting is the BMVC2019 2channel NSDM, however nowadays we use NSRM
    numberOfChannelsPerNSDMElement=2
    if (configuration['NSDMAlsoUseAlignmentAngles']==1):
       numberOfChannelsPerNSDMElement=1
    print("Number of Channels Per NSDM element ",numberOfChannelsPerNSDMElement)


    #Hardcoded path for dataset if using CSV file
    #datasetDirectory="/home/ammar/Documents/Programming/DNNTracker/DNNTracker/dataset"
    #datasetDirectory="../../DNNTracker/dataset"
    datasetDirectory="dataset/generated/"


    RAMBefore = getRAMInformation()
    print("Initial | free RAM ",RAMBefore['free']," KB");

    groundTruthTrain = readGroundTruthFile(
                                             configuration,
                                             "%s Train data for decomposition" % (decompositionType),
                                             "%s/2d_%s.csv"  % (datasetDirectory,dataFile),
                                             "%s/%s_%s.csv"  % (datasetDirectory,outputMode,dataFile), #outputMode is either bvh or 3d
                                             memPercentage,
                                             numberOfChannelsPerNSDMElement,
                                             useRadians,
                                             useHalfFloats,
                                             completelyDisablePCACode = 1 #If we do PCA while trying to do PCA it will be madness
                                          )
 
    RAMAfter = getRAMInformation()
    print("After reading the dataset | free RAM ",RAMAfter['free']," KB");
    #----------------------------------------------------------------------------------------------------------
    startOfNSxMColumn = len(groundTruthTrain['labelInNoNSDM'])
    numberOfNSxMColumns = len(groundTruthTrain['labelIn'])-startOfNSxMColumn
    print("2D data are 0 to ",startOfNSxMColumn)
    print("NSRM data are ",startOfNSxMColumn , " to ",numberOfNSxMColumns)
 
    if (mode==1):
       NSxM  = groundTruthTrain['in']
       label = "on 2D+NSRM data"
    elif (mode==2):
       NSxM  = splitNumpyArray(groundTruthTrain['in'],startOfNSxMColumn,numberOfNSxMColumns,useHalfFloats)
       label = "on NSRM data"
    elif (mode==3):
       NSxM  = splitNumpyArray(groundTruthTrain['in'],0,startOfNSxMColumn,useHalfFloats)
       label = "on 2D data"
    elif (mode==4):
       NSxM  = groundTruthTrain['out']
       label = "on all BVH Data"
    elif (mode==5): 
       onlyRotationalBVHOutputs = len(groundTruthTrain['labelOut']) - 3
       print("Will take output from 3-",onlyRotationalBVHOutputs,"/",len(groundTruthTrain['labelOut']))
       NSxM  = splitNumpyArray(groundTruthTrain['out'],3,onlyRotationalBVHOutputs,useHalfFloats)
       label = "on rot. BVH Data"
    elif (mode==6): 
       onlyLimbRotationalBVHOutputs = len(groundTruthTrain['labelOut']) - 6
       print("Will take output from 6-",onlyLimbRotationalBVHOutputs,"/",len(groundTruthTrain['labelOut']))
       NSxM  = splitNumpyArray(groundTruthTrain['out'],6,onlyLimbRotationalBVHOutputs,useHalfFloats)
       label = "on limb rot. BVH Data"

    #----------------------------------------------------------------------------------------------------------
    RAMSplit = getRAMInformation()
    print("After splitting the dataset | free RAM ",RAMSplit['free']," KB");
    del groundTruthTrain['in']
    if (not visualize):
       del groundTruthTrain['out'] 
    RAMFree = getRAMInformation()
    print("After freeing original input | free RAM ",RAMFree['free']," KB");

    print(OKGREEN,"Will perform dimensionality reduction on ",label,ENDC)
    
    if (decompositionType=="pca"):
       print("Attempting PCA using my code with ",dimensions," elements")
       print("Our Input Shape is ",NSxM.shape[0],"x",NSxM.shape[1])
       from principleComponentAnalysis import PCA
       ourDecomposition = PCA(inputData=NSxM,decompositionType=decompositionType)
       for i in range(0,len(ourDecomposition.cumulative)):
          thisValue = 0.0
          try:
            thisValue = ourDecomposition.cumulative[i].real
          except:
            thisValue = ourDecomposition.cumulative[i]
          #--------------------------------------------------- 
          if (thisValue>0.99): 
            print(OKGREEN,"Useless? PCA Dim #",i," - ",thisValue*100,"%",ENDC)
          else:          
            print("PCA Dim #",i," - Explanation of data until here ",thisValue*100,"%")
          if (dimensions==i):
            print(FAIL," ------------- OUR LIMIT IS SET HERE (",dimensions,") ------------- ",ENDC)
    else:
       from dataDecomposition import Decomposition
       print("Attempting Generic Decomposition ",decompositionType," with ",dimensions," elements")
       print("Our Input Shape is ",NSxM.shape[0],"x",NSxM.shape[1])
       ourDecomposition = Decomposition(inputData=NSxM,decompositionType=decompositionType,selectedPCADimensions=dimensions)

    if (dumpRules):
       print("Dumping rules to json file..")
       x = configuration['doPCA'].split("$")
       if (len(x)>0):
          #If there is a $ character this is our place holder to autocomplete our dataFile 
          print("We resolve ",configuration['doPCA'])
          print(" to ")
          configuration['doPCA']=x[0]+dataFile+".pca"
          print(configuration['doPCA'])
       ourDecomposition.save(configuration['doPCA']) 

    #Number of PCA components to plot on second plot
    #-----------------------------------------------------------------------------
    onlyScreePlotNDimensions=8
    if (batch==1) or (visualize):
      if (highlightChannel<0):
       colors = list()
       colorLabel = "highlighting PC-4"
      else:
       colorLabel="highlighting "+ groundTruthTrain['labelOut'][highlightChannel]
       colors = groundTruthTrain['out'][:,highlightChannel]
    #-----------------------------------------------------------------------------
    filename = ""
    if (batch==1):
      filename = "%s_%s_%s_%u_v%u%u.png"%(decompositionType,label,colorLabel,int(memPercentage),viewA,viewB)
      filename = filename.replace(' ','_')
    #-----------------------------------------------------------------------------
    if (visualize):
       ourDecomposition.visualize(NSxM,saveToFile=filename,onlyScreePlotNDimensions=onlyScreePlotNDimensions,label=label,colors=colors,colorLabel=colorLabel,viewAzimuth=viewA,viewElevation=viewB,showScree=addScreePlot)
    
    print(OKGREEN,"Finished with ",decompositionType," code!",ENDC)
