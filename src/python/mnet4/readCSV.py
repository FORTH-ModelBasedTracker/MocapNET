#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import numpy as np
import os
import csv
import gc
import time
import array
import sys

from NSDM  import NSDMLabels,createNSDMUsingRules,performNSRMAlignment,getJoint2DXYV
from EDM   import EDMLabels,createEDMUsingRules
from tools import bcolors,checkIfFileExists,getNumberOfLines,getNumberOfLinesOS,convert_bytes,eprint
from dataDecomposition import EmptyDecomposition


"""
    typically we work with data[NUMBER_OF_SAMPLES,NUMBER_OF_COLUMNS] 
    calling splitNumpyArray(data,4,4,0) means get back columns 4,5,6,7 so it will return a data[NUMBER_OF_SAMPLES,4] 
"""
def splitNumpyArray(data,column,columnsToTake,useHalfFloats):
    #---------------------------------------------------------------------------------------------
    numberOfSamples = len(data)
    #numberOfColumns = len(data[0])
    #---------------------------------------------------------------------------------------------
    if (useHalfFloats):
       npOutput = np.full([numberOfSamples,columnsToTake],fill_value=0,dtype=np.float16,order='C')
       for outCol in range(0,columnsToTake):
           for num in range(0,numberOfSamples):
                    npOutput[num,outCol]=np.float16(data[num,column+outCol])
    #---------------------------------------------------------------------------------------------
    else:
       npOutput = np.full([numberOfSamples,columnsToTake],fill_value=0,dtype=np.float32,order='C')
       for outCol in range(0,columnsToTake):
           for num in range(0,numberOfSamples):
                    npOutput[num,outCol]=np.float32(data[num,column+outCol])
    #---------------------------------------------------------------------------------------------
    return npOutput;


"""
    typically we work with data[NUMBER_OF_SAMPLES,NUMBER_OF_COLUMNS] 
    calling selectOnlySpecificColumnsOfNumpyArray(data,list,0) means get back only columns listed with 0 on the list 
"""
def selectOnlySpecificColumnsOfNumpyArray(data,columnListWithSelections,useHalfFloats):
    numberOfSamples=len(data)
    numberOfOriginalColumns = len(data[0])
    numberOfSelectedColumns = sum(columnListWithSelections)
    print("We will select ",numberOfSelectedColumns,"/",numberOfOriginalColumns," columns")
    outCol=0
       
    #---------------------------------------------------------------------------------------------
    if (useHalfFloats):
       npOutput = np.full([numberOfSamples,numberOfSelectedColumns],fill_value=0,dtype=np.float16,order='C')
       for inCol in range(0,numberOfOriginalColumns):
           if (columnListWithSelections[inCol]!=0):  
              for num in range(0,numberOfSamples):
                    npOutput[num,outCol]=np.float16(data[num,outCol])
              outCol=outCol+1
    #---------------------------------------------------------------------------------------------
    else:
       npOutput = np.full([numberOfSamples,numberOfSelectedColumns],fill_value=0,dtype=np.float32,order='C')
       for inCol in range(0,numberOfOriginalColumns):
           if (columnListWithSelections[inCol]!=0):  
              for num in range(0,numberOfSamples):
                    npOutput[num,outCol]=np.float32(data[num,outCol])
              outCol=outCol+1
    #---------------------------------------------------------------------------------------------
    return npOutput;


"""
    Perform a series of checks on testList and trainList to make sure they correspond correctly
"""
def checkIfTestAndTrainListsAreOk(testList,trainList):
    return 1
    if (len(trainList['labelIn'])!=len(trainList['in'])):
         print('Mismatch of training labels in (',len(trainList['labelIn']),') and actual input (',len(trainList['in']),') ')
         sys.exit(0)
    if (len(trainList['labelOut'])!=len(trainList['out'])):
         print('Mismatch of training labels out (',len(trainList['labelOut']),') and actual output (',len(trainList['out']),') ')
         sys.exit(0)
    if (len(trainList['in'])!=len(trainList['out'])):
         print('Mismatch of training labels in input (',len(trainList['in']),') and output (',len(trainList['out']),') ')
         sys.exit(0)
    if (len(testList['labelIn'])!=len(testList['in'])):
         print('Mismatch of testing labels in (',len(testList['labelIn']),') and actual input (',len(testList['in']),') ')
         sys.exit(0)
    if (len(testList['labelOut'])!=len(testList['out'])):
         print('Mismatch of testing labels out (',len(testList['labelOut']),') and actual output (',len(testList['out']),') ')
         sys.exit(0)
    if (len(testList['in'])!=len(testList['out'])):
         print('Mismatch of testing labels in input (',len(testList['in']),') and output (',len(testList['out']),') ')
         sys.exit(0)
    return 1;

"""
    Our Ground Truth has perfect knowledge of all joints, however based on the settings it also has invisibility flags
    so that we can hide joints and simulate occlusions for the neural network. This function zeroes out X and Y coordinates
    based on the invisibility flag.
"""
def zeroOutXYJointsThatAreInvisible(thisInput,rules):
    if (len(thisInput)==0): 
             print("zeroOutXYJointsThatAreInvisible called with no input for element ")
             return thisInput


    if (not rules['inputJointMap'].checkJointListDimensions(thisInput)):
             print("zeroOutXYJointsThatAreInvisible called with incorrect input size ")
             return thisInput

    #checkIfConfigurationHierarchyIsTheSameAsLabelList
    #head -n 1 dataset/2d_body_all.csv

    for z in range(0,len(rules['NSDM'])):
        #--------------------------------------------------------
        jointName=rules['NSDM'][z]['joint']
        jID_X   = rules['inputJointMap'].getJointID_2DX(jointName)
        jID_Y   = rules['inputJointMap'].getJointID_2DY(jointName)
        jID_Vis = rules['inputJointMap'].getJointID_Visibility(jointName)
        #--------------------------------------------------------
        if (thisInput[jID_Vis]==0.0):
          #We have a 2D joint that is marked as invisible! 
          if (rules["ignoreOcclusions"]):
            #If the master setting ignore occlusions is we set all visibility flags as visible..
            thisInput[jID_Vis] = 1.0
          #BUG 24/6/2023 lookup was looking at hierarchy instead of NSDM part for immuneToSelfOcclusions: 
          elif (not 'immuneToSelfOcclusions' in rules['NSDM'][z]) or (rules['NSDM'][z]['immuneToSelfOcclusions']==0):
            #If the master setting for the particular hierarchy  is not immune to invisibility, we erase it ..
            thisInput[jID_X]   = 0.0
            thisInput[jID_Y]   = 0.0
            thisInput[jID_Vis] = 0.0 #This is already zero but leaving it here for easier inspection
    return thisInput



"""
    Our Input has the form of 2DX_joint,2DY_joint,visibility_joint, .... ,
    2D x and y coordinates are normalized in the range of [0,1], however using this function they can be balanced in the range [-0.5,0.5]
    to study the effects on the neural network.
"""
def balance2DXYVis(thisInput,rules):
    if (len(thisInput)==0): 
             print("balance2DXYVis called with no input for element ")
             return thisInput

    if (not rules['inputJointMap'].checkJointListDimensions(thisInput)):
             print("balance2DXYVis called with incorrect input size ")
             return thisInput

    #checkIfConfigurationHierarchyIsTheSameAsLabelList
    #head -n 1 dataset/2d_body_all.csv
    isVisible=0
    for z in range(0,len(rules['NSDM'])):
        #--------------------------------------------------------
        jointName = rules['NSDM'][z]['joint']
        jID_X     = rules['inputJointMap'].getJointID_2DX(jointName)
        jID_Y     = rules['inputJointMap'].getJointID_2DY(jointName)
        jID_Vis   = rules['inputJointMap'].getJointID_Visibility(jointName)
        #--------------------------------------------------------
        isVisible = thisInput[jID_Vis]
        if (isVisible==1) or (rules["ignoreOcclusions"]):
            thisInput[jID_X] = thisInput[jID_X] - 0.5
            thisInput[jID_Y] = thisInput[jID_Y] - 0.5
    return thisInput


"""
   This function rearranges the 2D input discarding the visibility flag
"""
def rearrange2DInput(thisInput,rules):
    if (len(thisInput)==0): 
             print("rearrange2DInput called with no input for element ")
             return thisInput

    if (not rules['inputJointMap'].checkJointListDimensions(thisInput)):
             print("rearrange2DInput called with incorrect input size ")
             return thisInput

    rearrangedInput = list()
    for z in range(0,len(rules['NSDM'])):
        #--------------------------------------------------------
        jointName = rules['NSDM'][z]['joint']
        jID_X     = rules['inputJointMap'].getJointID_2DX(jointName)
        jID_Y     = rules['inputJointMap'].getJointID_2DY(jointName)
        jID_Vis   = rules['inputJointMap'].getJointID_Visibility(jointName)
        #--------------------------------------------------------
        rearrangedInput.append(np.float32(thisInput[jID_X]))
        rearrangedInput.append(np.float32(thisInput[jID_Y]))
        if ('include2DInputVisibilityFlags' in rules) and (rules['include2DInputVisibilityFlags']==1): #Was flipped, bug 8/5/2023
           rearrangedInput.append(np.float32(thisInput[jID_Vis]))
        #--------------------------------------------------------
    return rearrangedInput




"""
    A sanity check counting if the configuration file corresponds to our label list!
"""
def checkIfConfigurationHierarchyIsTheSameAsLabelList(configuration,labelList,useNSDM):
    #Manual check using : 
    #                      head -1 dataset/2d_body_all.csv
    #               and : 
    #                     cat dataset/body_configuration.json | grep joint
    #                     
    #This code part checks if our new JSON configuration corresponds to a labelList we encountered
    #The label list will have two parts, a normal one that should have 1-1 correspondance with configuration['hierarchy']
    #and a second one that should have 1-1 correspondance with the NSDM list extracted from the configuraton using the NSDMLabels call
    numberOfInputElements=len(labelList) 
    numberOfElementsInRulesHierarchy=len(configuration['hierarchy'])
    
    #We begin by copying the configuration to an (initially) empty list
    configurationHierarchyList=list()
    for i in range(0,numberOfElementsInRulesHierarchy):
       configurationHierarchyList.append("2DX_"+configuration['hierarchy'][i]['joint'])
       configurationHierarchyList.append("2DY_"+configuration['hierarchy'][i]['joint'])
       configurationHierarchyList.append("visible_"+configuration['hierarchy'][i]['joint'])

    if (useNSDM):
       #We can also add the NSDM elements if it is wanted..   
       configurationHierarchyList.extend(NSDMLabels(configuration))
    
    if ( (configuration['doPCA']!="") ):
                 print("PCA will fake everything to get past check")
                 configurationHierarchyList=list()
                 for i in range(0,int(configuration['PCADimensionsKept'])):
                     configurationHierarchyList.append("PCA-"+str(i))  
                 return 1


    #At this point the two lists should be identical..
    numberOfElementsInRulesHierarchy=len(configurationHierarchyList)

    if (numberOfElementsInRulesHierarchy!=numberOfInputElements):
        print(bcolors.FAIL,'Rule file inconsistency',bcolors.ENDC)
        print(bcolors.FAIL,'numberOfElementsInRulesHierarchy = ',numberOfElementsInRulesHierarchy,bcolors.ENDC)
        print(bcolors.FAIL,'numberOfInputElements = ',numberOfInputElements,bcolors.ENDC)
        print('Number of Configuration Rules ',numberOfElementsInRulesHierarchy,' Number of Encountered Input Elements ',numberOfInputElements)
        print('Configuration from JSON ',configurationHierarchyList)
        print('Data we encountered ',labelList)
        return 0 
          

    success=1
    for i in range(0,numberOfElementsInRulesHierarchy):
        nB=labelList[i]
        nA=configurationHierarchyList[i] 

        if (nA!=nB): 
          print (bcolors.FAIL,"#",i," json(",nA,") != (",nB,")",bcolors.ENDC)
          success=0

    del configurationHierarchyList
    return success



"""
    Return the configuration from a filename pointing to a .json file..!
"""
def parseConfiguration(filename):
    import json
    configuration = dict()
    with open(filename) as f:
              s = f.read()
              s = s.replace('\t','')
              s = s.replace('\n','')
              s = s.replace(',}','}')
              s = s.replace(',]',']') 
              configuration = json.loads(s)

    #Backwards compatibility
    #----------------------------------------------
    if not 'useRadians' in configuration:
           configuration['useRadians']=0
    if (configuration['useRadians']==1):
           #If we are using radians everything will be smaller so scale accordingly early stopping deltas!
           configuration['minEarlyStoppingDelta']=float(configuration['minEarlyStoppingDelta']) * float(0.01) 
    if not 'outputMode' in configuration:
           configuration['outputMode']="bvh"
    if not 'padEnsembleInput' in configuration:
           configuration['padEnsembleInput']=0
    if not 'probabilisticOutput' in configuration:
           configuration['probabilisticOutput']=0
    if not ('decompositionType' in configuration) and ('doPCA' in configuration):
           configuration['decompositionType']='pca'
    if not ('outputMultiplier' in configuration):
           configuration['outputMultiplier']=float(1.0)
    #----------------------------------------------
    if not 'include2DInputVisibilityFlags' in configuration:
           configuration['include2DInputVisibilityFlags']=1
    #----------------------------------------------
    if (float(configuration['outputMultiplier'])>1.0):
           print("Rescaling early stopping delta from ",configuration['minEarlyStoppingDelta'])
           configuration['minEarlyStoppingDelta']=float(configuration['minEarlyStoppingDelta']) * float(configuration['outputMultiplier'])
           print("To an early stopping delta of ",configuration['minEarlyStoppingDelta'])
    #----------------------------------------------
    if not 'setConstantSeedForReproducibleTraining' in configuration:
           configuration["setConstantSeedForReproducibleTraining"]=1 
    #----------------------------------------------
    if not 'weightRandomizationFunction' in configuration:
           configuration["weightRandomizationFunction"]="auto"
           #print(bcolors.FAIL +"Please declare a weightRandomizationFunction in ",filename," "+bcolors.ENDC)
           #sys.exit(1)
    #----------------------------------------------
    if (configuration["weightRandomizationFunction"]=="auto"):
           print("Will use automatically selected randomization function (old code)")
    elif (configuration["weightRandomizationFunction"]=="glorot_uniform"):
           print("Using Glorot/Xavier randomization (https://www.tensorflow.org/api_docs/python/tf/keras/initializers/GlorotUniform)")
    elif (configuration["weightRandomizationFunction"]=="lecun_normal"):
           print("Using Lecun Normal randomization (https://www.tensorflow.org/api_docs/python/tf/keras/initializers/LecunNormal)")
    elif (configuration["weightRandomizationFunction"]=="he_uniform"):
           print("Using He Uniform randomization (https://www.tensorflow.org/api_docs/python/tf/keras/initializers/HeUniform)")
    else:
         print(bcolors.FAIL+"The randomization function you used ",configuration["weightRandomizationFunction"]," is not in whitelist"+bcolors.ENDC)
         sys.exit(1)
    #----------------------------------------------
    if not ('outputValueDistribution' in configuration):
         print("Weird, no outputValueDistribution in configuration ~-> ",filename)
         configuration['outputValueDistribution']='default'
         #sys.exit(1)
    else:
      if (configuration['outputValueDistribution']!='balanced'):
         pass
      elif (configuration['outputValueDistribution']!='positive'):
         pass
      elif (configuration['outputValueDistribution']!='negative'):
         pass
      elif (configuration['outputValueDistribution']!='default'):
         pass
      else:
         print(bcolors.FAIL+"Configuration error: outputValueDistribution valid options are balanced|positive|negative|default"+bcolors.ENDC)
         sys.exit(1)

    from datetime import datetime
    # Get the current date and time
    now = datetime.now()
    # Format the date string
    configuration['date']= now.strftime("%d/%m/%Y")


    return configuration





"""
    New code on jointMap.py that uses a dict to resolve 2D CSV input labels to the correct column that has the data
"""
def parseConfigurationInputJointMap(configuration,labelsInput):
   from jointMap import JointDataMapper
   configuration['inputJointMap'] = JointDataMapper("foo",configuration,labelsInput)
   return configuration



"""
   This is a mini-function to initialize the decomposition code inside the readGroundTruthFile for an execution engine and make code more compact..
"""
def initializeDecompositionForExecutionEngine(configuration,modelDirectory,partName,disablePCACode=False):
               print("initializeDecompositionForExecutionEngine")
               from dataDecomposition import EmptyDecomposition
               ourDecomposition = EmptyDecomposition()

               if (configuration['decompositionType']==""):
                  return ourDecomposition

 
               if (not disablePCACode):
                  PCAFile   = ""
                  x = configuration['doPCA'].split("$")
                  if (len(x)>0):
                      #If there is a $ character this is our place holder to autocomplete our dataFile 
                      print("We resolve ",configuration['doPCA'])
                      print(" to ")
                      PCAFile=x[0]+partName+".pca" 
                      print(PCAFile)
                      print("Simple is ",PCAFile)
                      #We use the model pca file
                      PCAFile=modelDirectory+"/"+partName+".pca"
                      print("Per model is ",PCAFile)

                  if  (not 'decompositionType' in configuration) or (configuration['decompositionType']=="pca"):
                         from principleComponentAnalysis import PCA
                         ourDecomposition = PCA(decompositionType='pca',savedFile=PCAFile)
                  else:
                         from dataDecomposition import Decomposition
                         ourDecomposition = Decomposition(decompositionType=configuration['decompositionType'],selectedPCADimensions=configuration['PCADimensionsKept'])


                  if ( (configuration['decompositionType']!="") and (configuration['doPCA']!="") and ( int(configuration['PCADimensionsKept'])>0 ) ):
                          if (ourDecomposition.load(PCAFile)): 
                             print(bcolors.OKGREEN + "Opened Decomposition file "+ PCAFile +  bcolors.ENDC)
                             #Important our decomposition engine exposed to the rest of the code
                             configuration['decompositionEngine']=ourDecomposition
                          if (ourDecomposition.numberOfSamplesFittedOn==0):
                             print(bcolors.WARNING + "Could not load PCA file "+ configuration['doPCA'] +  bcolors.ENDC)
                  else:
                             print(bcolors.OKGREEN + "Decomposition is disabled" + bcolors.ENDC)

               return ourDecomposition
               #--------------------------------------------



"""
  Return an ARRAY ( not a list or a dict ) with the input of our Neural Network after constructing all other required descriptors according to our configuration 
"""
def transformNetworkInput(
                           configuration,
                           ourDecomposition,
                           inputToTransform,
                           dtypeSelected,
                           completelyDisablePCACode = 0,
                           completelyDisableEigenposes = 0
                         ):
           from array import array
  
           # inputToTransform contains the input we want to transform,
           # we also suppose that configuration['inputJointMap'] holds our joint mapping information..
           # we need to perform many operations, which will gradually transform our input to network input according to our configuration  
           
           #INPUT IS : inputToTransform
           #-----------------------------------------------------------------------------------------------------------------------------
           # First :  Hide invisible joints if allowed by our configuration  
           #-----------------------------------------------------------------------------------------------------------------------------
           processedInputToTransform = zeroOutXYJointsThatAreInvisible(inputToTransform,configuration)
           #-----------------------------------------------------------------------------------------------------------------------------
           # Second : Align "processedInputToTransform" using our pivot and reference point ( if eNSRM is set to 1 in .json settings )
           #-----------------------------------------------------------------------------------------------------------------------------
           angleToRotate,processedInputToTransform = performNSRMAlignment(processedInputToTransform,configuration)
           #-----------------------------------------------------------------------------------------------------------------------------
           # Third :  re balance 2D input from [0..1] to [-0.5..0.5] ( THIS WORKS VERY BADLY )   
           if ('balanced2DInputs' in configuration) and (configuration['balanced2DInputs']==1):
                      print("balanced2DInputs WORKS VERY BADLY for some reason..")
                      processedInputToTransform = balance2DXYVis(processedInputToTransform,configuration)
           #-----------------------------------------------------------------------------------------------------------------------------
           # Fourth :  ALWAYS! REPACK 2D input to make sure visibility flags are included (or not) no matter what data inputToTransform holds
           # WARNING: do not use rearranged2DInput for anything else..
           rearranged2DInput      = rearrange2DInput(processedInputToTransform,configuration)
           #Convert 2D input to an array with just repacked 2D inputs
           rearranged2DInputArray = array("f", rearranged2DInput)           #<- THIS PART OF THE ARRAY ONLY HAS OUR 2D INPUTS
           #-----------------------------------------------------------------------------------------------------------------------------
           #OUTPUT IS : rearranged2DInput


           #-------------------------------------------
           #           Collect descriptors
           #-------------------------------------------
           #INPUT is processedInputToTransform 
           thisDescriptor = list()
           #  Add an EDM descriptor
           if ('EDM' in configuration) and (int(configuration['EDM'])==1):
              thisDescriptor.extend(createEDMUsingRules(configuration,processedInputToTransform)) #important to use processedInputToTransform
              thisDescriptor.append(angleToRotate)
           #  Add an NSDM descriptor
           if (int(configuration['eNSRM'])==1) or (int(configuration['NSDMNormalizationMasterSwitch'])==1) or (int(configuration['NSDMAlsoUseAlignmentAngles'])==1):
              thisDescriptor.extend(createNSDMUsingRules(configuration,processedInputToTransform,angleToRotate) ) #important to use processedInputToTransform
           thisDescriptorArray = array("f", thisDescriptor) 
           #OUTPUT is thisDescriptor 
           #------------------------------------------- 

           #-------------------------------------------
           #           Gather full input
           #-------------------------------------------
           thisFullInputArray = array("f")
           thisFullInputArray.extend(rearranged2DInputArray) #only use 2D input as absolute vector, do not feed it to things depending on JointMap
           thisFullInputArray.extend(thisDescriptorArray)
           #-------------------------------------------
           
           #-------------------------------------------
           #             Pack in a PCA
           #-------------------------------------------
           if (not completelyDisablePCACode):
              if (  (configuration['decompositionType']!="") and (configuration['doPCA']!="") and (ourDecomposition.numberOfSamplesFittedOn!=0) and ( int(configuration['PCADimensionsKept'])>0 ) ):
                  #print("ourDecomposition.transform ",len(thisFullInputArray)," elements ")
                  transformedInput      = ourDecomposition.transform(np.asarray([thisFullInputArray],dtype=dtypeSelected),selectedPCADimensions=int(configuration['PCADimensionsKept'])) 
                  transformedInputList  = transformedInput[0].real.tolist()
                  transformedInputArray = array("f",transformedInputList)
                  #---------------------------------------------------------------------------------
                  if ('PCAAlsoKeepRawData' in configuration) and (configuration['PCAAlsoKeepRawData']==1):
                      #We will also include the RAW UNROTATED DATA! 
                      thisFullInputArray = rearranged2DInputArray #only use 2D input as absolute vector, do not feed it to things depending on JointMap
                      thisFullInputArray.extend(transformedInputArray)
                  else:
                      #We will not keep raw data so we will serve ONLY THE PCA DATA!
                      thisFullInputArray = transformedInputArray
                  #---------------------------------------------------------------------------------
                  if (len(transformedInput[0])!=int(configuration['PCADimensionsKept'])):
                      print(bcolors.WARNING +"Inconsistent size of input after dimensionality reduction, expected %u and got %u "%( int(configuration['PCADimensionsKept']) , len(thisFullInputArray) ) +  bcolors.ENDC)
                      sys.exit(1)
           #-------------------------------------------

           #-------------------------------------------
           #           Add EigenPose Data
           #-------------------------------------------
           if ('eigenPoses' in configuration) and (int(configuration['eigenPoses'])==1) and (completelyDisableEigenposes==0):
              from EigenPoses import createEigenPosesUsingRules
              thisFullInputArray.extend(createEigenPosesUsingRules(configuration,thisFullInputArray))
           #-------------------------------------------
           return thisFullInputArray, thisDescriptorArray, rearranged2DInputArray, angleToRotate



def prepareInputG(input2D :dict,configuration : dict, expectedInputList,dummyUnneededInput,part,decompositionEngine,disablePCACode):
        thisInput = list()

        #input2D is a dictionary that has 2D information on all the body. We need to make a list that directly corresponds to the expected Input List 
        totalJoints    = 0
        existingJoints = 0
        #We gather all elements in expectedInputList 
        for element in expectedInputList: 
            totalJoints = totalJoints + 1
            if element in input2D: 
               #If the element exists then we append it
               thisInput.append(np.float32(input2D[element]))
               existingJoints = existingJoints + 1
            else:
               #If it does not we append a zero, to maintain the vector dimensions
               eprint(bcolors.FAIL,"prepareInputG: NSRM Input Element `",element,"` is missing",bcolors.ENDC)
               thisInput.append(np.float32(0.0))
         
        #After trying all elements in our input dictionary we now know how much data is missing  
        missingRatio = float ( 1.0 - (existingJoints/totalJoints) ) 
        if (missingRatio>0.3):
           #print("Input : ",input2D)
           eprint(bcolors.FAIL,"Too many elements missing for ",part," missingRatio : ",missingRatio,"..",bcolors.ENDC)
        
        #thisInput now holds all of our data, lets transform it!
        #-----------------------------------------------------------------------------------------------------------------------------------------------
        thisFullInput, thisNSDM, thisInput, angleToRotate = transformNetworkInput(configuration,decompositionEngine,thisInput,np.float32,disablePCACode)
        #-----------------------------------------------------------------------------------------------------------------------------------------------
        #print(bcolors.WARNING,"prepareInputG: transformed ",input2D," to ",thisFullInput,bcolors.ENDC)
        return thisFullInput, thisNSDM, thisInput, angleToRotate, missingRatio



"""
    Attempt to count how many elemnts transformNetworkInput produces and make them a list of strings 
"""
def transformNetworkInputLabels(configuration,inputLabelsOnly,inputNumberOfColumns,dtypeSelected,ourDecomposition,thisInput,completelyDisablePCACode = 0,completelyDisableEigenposes = 0):
      thisLabels = list()

      if (completelyDisablePCACode):
         print("transformNetworkInputLabels doesnt know how to work with PCA code disabled :(")
    
      emptyInput = list()
      for i in range(0,inputNumberOfColumns):
        emptyInput.append(0.0)

      print("Empty input for labels has %u elements " % len(emptyInput))
      emptyFullInput, emptyNSDMArray, emptyInputArray, emptyAngleToRotate = transformNetworkInput(
                                                                                                   configuration,
                                                                                                   ourDecomposition,
                                                                                                   emptyInput,
                                                                                                   dtypeSelected,
                                                                                                   completelyDisablePCACode = completelyDisablePCACode,
                                                                                                   completelyDisableEigenposes = completelyDisableEigenposes
                                                                                                 )

      for i in range(0,len(emptyFullInput)):
           thisLabels.append("in%u"%i)

      print("Generated labels have %u elements " % len(thisLabels))
      print("Full input has %u elements " % len(emptyFullInput))
      print("NSxM has %u elements " % len(emptyNSDMArray))
      print("2D data have %u elements " % len(emptyInputArray))
      #-----------------------------------------------------------------------------------------------------------------------------------------------
      return thisLabels



def resolveNormalizationRules(configuration,label):
    column   = 0
    resolved = False
    labelLowerCase = label.lower()
    #----------------------------------------------------------
    for labelSearch in configuration["outputOffsetLabels"]:
        if (labelLowerCase==labelSearch.lower()):
                       resolved = True
                       break;
        else:
                       column += 1
    #----------------------------------------------------------
    if (not resolved):
        print(bcolors.FAIL,"Could not resolve ",label,bcolors.ENDC)
    return column,resolved



"""
    This function applies the offsets stored in configuration["outputOffsetValues"]
"""
def executeNormalization(configuration,data):
    #--------------------------------------------------------------
    if ("out" in data) and ("outputOffsetValues" in configuration) and ("outputScalarValues" in configuration):
           offsets         = configuration["outputOffsetValues"]
           scalars         = configuration["outputScalarValues"]
           minima          = configuration["outputOffsetMinima"]
           maxima          = configuration["outputOffsetMaxima"]
           numberOfSamples = data["out"].shape[0]
           numberOfColumns = data["out"].shape[1]
           print("executeNormalization: Output has ",numberOfColumns," columns ")
           for column in range(numberOfColumns):
              #Actual normalization of column
              #---------------------------------------------
               for sample in range(numberOfSamples):
                     data["out"][sample][column] = np.float32(    ( data["out"][sample][column] - np.float32(offsets[column]) ) * np.float32(scalars[column])    )
              #---------------------------------------------
                 
           #Get statistics to make sure everything went ok..!
           for column in range(data["out"].shape[1]):
              #---------------------------------------------
              titleString="%s |  Offset %0.2f Scalar %0.2f | Min=%0.2f,Max=%0.2f |" % (data["labelOut"][column],offsets[column],scalars[column],minima[column],maxima[column])
              print(bcolors.OKGREEN," %s " % titleString,bcolors.ENDC)
              #---------------------------------------------
    #--------------------------------------------------------------
           print(bcolors.OKGREEN," Done normalizing output ",bcolors.ENDC)
    #--------------------------------------------------------------
    return data



"""
    This function just calculates the offsets and stores them in configuration["outputOffsetValues"]
"""
def calculateOutputNormalization(configuration,outputDirectoryForTrainedModels,data):
    if ("out" in data):
           numberOfSamples = data["out"].shape[0]
           numberOfOutputs = data["out"].shape[1]
           print("calculateOutputNormalization: Output has ",numberOfOutputs," columns ")
           minima  = list()
           maxima  = list()
           offsets = list()
           scalars = list()
           scalarsFraction=list()
           for column in range(numberOfOutputs):
              #Calculate offset for each column..
              #---------------------------------------------
              thisOffset=np.float32(0.0)
              thisMinima=np.float32(0.0)
              thisMaxima=np.float32(0.0)
              if (numberOfSamples>0):
                 #We have available elements so lets do a proper min/max initialization 
                 thisMinima=np.float32(data["out"][0][column])
                 thisMaxima=np.float32(data["out"][0][column])
              #Actually calculate min/max for all samples  
              #---------------------------------------------
              for sample in range(numberOfSamples):
                     if (thisMinima>data["out"][sample][column]):
                             thisMinima = np.float32(data["out"][sample][column])
                     if (thisMaxima<data["out"][sample][column]):
                             thisMaxima = np.float32(data["out"][sample][column])
              #--------------------------------------------------------------
              #After having calculated all min/max
              if ('outputValueDistribution' in configuration):
                  if (configuration['outputValueDistribution']=='balanced'):
                         thisOffset = np.float32((thisMaxima+thisMinima)/2.0)  
                  elif (configuration['outputValueDistribution']=='positive'):
                         thisOffset = np.float32(thisMinima)
                  elif (configuration['outputValueDistribution']=='negative'):
                         thisOffset = np.float32(thisMaxima)
                  else:
                         thisOffset = np.float32(0.0)
              #--------------------------------------------------------------
              #Store offset and min/max
              offsets.append(thisOffset)
              minima.append(thisMinima)
              maxima.append(thisMaxima)
              #---------------------------------------------

              #Calculate scalars for each column (if we use radians and if we are a rotation!)..
              #---------------------------------------------
              thisScalar=np.float32(1.0)
              if ( (configuration['useRadians']) and ('rotation' in data["labelOut"][column]) ):
                 if ( ('hand' in data["labelOut"][column])  and ('hand' in configuration['rootJoint']) ):
                   print(data["labelOut"][column]," is a special case, will not scale radians to avoid scaling hand root quaternions!")
                 else:
                   thisScalar= np.float32(0.017453292519943295) #Scale to radians..
              #---------------------------------------------
              if ('outputNormalizationStrategy' in configuration) and (configuration['outputNormalizationStrategy']=="normalize"): 
                   if (thisMaxima!=thisMinima):
                       thisScalar = thisScalar / np.float32((thisMaxima-thisMinima)/2)
              if ( 'outputMultiplier' in configuration ) and  ( configuration['outputMultiplier'] ):
                   thisScalar = thisScalar * np.float32(configuration['outputMultiplier'])
              #---------------------------------------------
              scalars.append(np.float32(thisScalar))
              scalarsFraction.append(np.float32(1.0/thisScalar))
              #---------------------------------------------

           #Store offsets..! ---------------------------------------------
           configuration["outputOffsetLabels"]         = data["labelOut"]
           configuration["outputOffsetMinima"]         = minima
           configuration["outputOffsetMaxima"]         = maxima
           configuration["outputOffsetValues"]         = offsets
           configuration["outputScalarValues"]         = scalars
           configuration["outputScalarFractionValues"] = scalarsFraction

           #Write CSV outputOffsets.csv 
           from tools import dumpMiniCSVToFile 
           #------------------------------------------------------------------------
           dumpMiniCSVToFile('%s/outputOffsets.csv'         % (outputDirectoryForTrainedModels),data["labelOut"],offsets,         numberOfOutputs)
           dumpMiniCSVToFile('%s/outputMinima.csv'          % (outputDirectoryForTrainedModels),data["labelOut"],minima,          numberOfOutputs)
           dumpMiniCSVToFile('%s/outputMaxima.csv'          % (outputDirectoryForTrainedModels),data["labelOut"],maxima,          numberOfOutputs)
           dumpMiniCSVToFile('%s/outputScalars.csv'         % (outputDirectoryForTrainedModels),data["labelOut"],scalars,         numberOfOutputs)
           dumpMiniCSVToFile('%s/outputScalarsFraction.csv' % (outputDirectoryForTrainedModels),data["labelOut"],scalarsFraction, numberOfOutputs)
           #------------------------------------------------------------------------
    print(bcolors.OKGREEN," Done calculating normalization offsets ",bcolors.ENDC)
    return configuration
 



def parseOutputNormalization(modelDirectory,csvFile,outputNames,initialValues):
     from tools import bcolors,getEntryIndexInList
     from readCSV import readCSVFile
     #--------------------------------------------------------------------------------------------------------------
     if (checkIfFileExists(modelDirectory+csvFile)):
                   correctOffsets = readCSVFile(modelDirectory+csvFile,1.0,0)
                   for jointID in range(0,len(outputNames)):
                       resolvedPosition = getEntryIndexInList(outputNames,correctOffsets["label"][jointID].lower()) 
                       if (resolvedPosition!=-1):
                           print("outputOffsets ",jointID,"/",resolvedPosition," <-> ",correctOffsets["label"][jointID]," <-> ",correctOffsets["body"][0][jointID]," <-> ",outputNames[resolvedPosition])
                           initialValues[resolvedPosition] = np.float32(correctOffsets["body"][0][jointID])
                       else:
                           print(bcolors.FAIL,"Unresolved loading ",jointID," - ",correctOffsets["label"][jointID],bcolors.ENDC)
     #--------------------------------------------------------------------------------------------------------------
     return initialValues





def checkDataForNaN(data):
    #Check for NaNs and Inf values that might influence data fitting
    #----------------------------------------------------------------------------------------------------------------------------
    if ( np.isnan(data['in']).any() ) or ( np.isinf(data['in']).any() ):
       print(bcolors.FAIL,"Fatal : Our input contains NaN or Inf values will not carry on with erroneous input ",bcolors.ENDC)
       sys.exit(1)
    else:
       print(bcolors.OKGREEN,"Our input is NaN/Inf free ",bcolors.ENDC)
    #----------------------------------------------------------------------------------------------------------------------------
    if ( np.isnan(data['out']).any() ) or ( np.isinf(data['out']).any() ):
       print(bcolors.FAIL,"Fatal : Our output contains NaN or Inf values will not carry on with erroneous input ",bcolors.ENDC)
       sys.exit(1)
    else:
       print(bcolors.OKGREEN,"Our output is NaN/Inf free ",bcolors.ENDC)
    #----------------------------------------------------------------------------------------------------------------------------





def filterOutEmptyInputSamples(data,sampleNumber):
    #print("Will now attempt to filter out empty input samples")
    numberOfOutputs = len(data) 
    emptyInputsInThisSample = 0
    for output in range(0,numberOfOutputs):
          if (data[output]==0.0):
             emptyInputsInThisSample = emptyInputsInThisSample + 1
    if (emptyInputsInThisSample==numberOfOutputs):
          #print("Sample ",sampleNumber," has ",float(emptyInputsInThisSample/numberOfOutputs)," empty outputs")
          return True
    return False




def readGroundTruthFile(configuration,typeOfGroundTruthFile,filenameInput,filenameOutput,memPercentage,numberOfNSDMElements,useRadians,useHalfFloats,completelyDisablePCACode = 0,externalDecomposition=EmptyDecomposition):
    print("Accessing ",typeOfGroundTruthFile," file In:",filenameInput," file Out:",filenameOutput,"..\n")
    start = time.time()
    #------------------------------------------------------------------------------------------------------------------------------------------------
    if (not checkIfFileExists(filenameInput)):
          print( bcolors.FAIL + "Input file "+filenameInput+" does not exist, cannot read ground truth "+typeOfGroundTruthFile+".." +  bcolors.ENDC)
          print("Current Directory was "+os.getcwd())
          sys.exit(1)
    if (not checkIfFileExists(filenameOutput)):
          print( bcolors.FAIL + "Output file "+filenameOutput+" does not exist, cannot read ground truth "+typeOfGroundTruthFile+".." +  bcolors.ENDC)
          print("Current Directory was "+os.getcwd())
          sys.exit(1)
    #------------------------------------------------------------------------------------------------------------------------------------------------
    if  (not 'outputOffsetLabels' in configuration) or (not 'outputOffsetMinima' in configuration) or  (not 'outputOffsetMaxima' in configuration) or  (not 'outputOffsetValues' in configuration) :
          print( bcolors.OKGREEN + "No Min/Max Offset/Scalar normalization data during this load operation.." +  bcolors.ENDC)
    #------------------------------------------------------------------------------------------------------------------------------------------------


    
    #Make sure ourDecomposition points to the "correct" decomposition class based on our configuration
    if (type(externalDecomposition)==type(EmptyDecomposition)):
     #------------------------------------------------------------------------------------------------------------------------------------------------
     if  (not 'decompositionType' in configuration) or (configuration['decompositionType']=="pca"):
       from principleComponentAnalysis import PCA
       print(bcolors.OKGREEN + "Using my PCA code.." + bcolors.ENDC)
       ourDecomposition = PCA(decompositionType='pca')
     else:
       from dataDecomposition import Decomposition
       print(bcolors.OKGREEN + "Using Sk-Learn decomposition code.." + bcolors.ENDC)
       ourDecomposition = Decomposition(decompositionType=configuration['decompositionType'],selectedPCADimensions=configuration['PCADimensionsKept'])
     #------------------------------------------------------------------------------------------------------------------------------------------------
     if (not completelyDisablePCACode):
      if ( (configuration['decompositionType']!="") and (configuration['doPCA']!="") and ( int(configuration['PCADimensionsKept'])>0 ) ):
        if (ourDecomposition.load(configuration['doPCA'])): 
           print(bcolors.OKGREEN + "Opened PCA file "+ configuration['doPCA'] +  bcolors.ENDC)
           #Important our decomposition engine exposed to the rest of the code
           configuration['decompositionEngine']=ourDecomposition
        if (ourDecomposition.numberOfSamplesFittedOn==0):
           print(bcolors.WARNING + "Could not load decomposition file "+ configuration['doPCA'] +  bcolors.ENDC)
      else:
           print(bcolors.OKGREEN + "PCA is disabled, change \"doPCA\": \"dataset/$.pca\" in configuration to re-enable it" + bcolors.ENDC)
    #------------------------------------------------------------------------------------------------------------------------------------------------
    else:
      if (not completelyDisablePCACode):
         ourDecomposition = externalDecomposition
         print(bcolors.OKGREEN + "Using externally initialized decomposition code" + bcolors.ENDC)
    #------------------------------------------------------------------------------------------------------------------------------------------------


    #Disable eigenposes if we are reading eigenposes :P
    #-------------------------------------------------------------------------------------------------
    completelyDisableEigenposes = 1
    if (not "Eigenposes"==typeOfGroundTruthFile):
      if ("eigenPoses" in configuration) and (configuration['eigenPoses']==1):
               completelyDisableEigenposes = 0
    #-------------------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------------------
    print(bcolors.OKGREEN + "Input/Output files "+filenameInput+","+filenameOutput+" exist in filesystem" +  bcolors.ENDC)
    #-------------------------------------------------------------------------------------------------
    dtypeSelected=np.dtype(np.float32)  
    dtypeSelectedByteSize=int(dtypeSelected.itemsize) 
    if (useHalfFloats):
       dtypeSelected=np.dtype(np.float16) 
       dtypeSelectedByteSize=int(dtypeSelected.itemsize)
    #-------------------------------------------------------------------------------------------------
    progress                 = 0.0
    sampleNumber             = 0
    CSVInputNumberOfColumns  = 0 
    CSVOutputNumberOfColumns = 0 
    receivedHeader           = False
    #-------------------------------------------------------------------------------------------------
    inputLabels           = list()
    inputLabelsClean      = list()
    outputLabels          = list() 
    #-------------------------------------------------------------------------------------------------
    numberOfSamplesInput  = getNumberOfLines(filenameInput)-2
    numberOfSamplesOutput = getNumberOfLines(filenameOutput)-2
    #-------------------------------------------------------------------------------------------------
    print(typeOfGroundTruthFile," Input file has ",numberOfSamplesInput," training samples\n")
    print(typeOfGroundTruthFile," Output file has ",numberOfSamplesOutput," training samples\n")
    #-------------------------------------------------------------------------------------------------
    if (numberOfSamplesInput!=numberOfSamplesOutput):
       print(bcolors.WARNING +"Input files (",filenameInput," and ",filenameOutput,") are inconsistent cannot continue"+  bcolors.ENDC)
       sys.exit(1)
    #-------------------------------------------------------------------------------------------------

    numberOfSamples = numberOfSamplesInput
    numberOfSamplesLimit=int(numberOfSamples*memPercentage)
    #------------------------------------------------------------------------------------------------- 
    if (memPercentage==0.0):
        print("readGroundTruthFile was asked to occupy 0 memory so this probably means we just want one record")    
        numberOfSamplesLimit=2
    if (memPercentage>1.0):
        print("Memory Limit will be interpreted as a raw value..")     
        numberOfSamplesLimit=int(memPercentage)
    #-------------------------------------------------------------------------------------------------
    

    encounteredSamplesThatAreEmpty = list()
    #---------------------------------
    numberOfInputElements = 0
    #---------------------------------
    thisInput  = array.array('f')
    thisOutput = array.array('f')
    #---------------------------------
    fi = open(filenameInput,  "r")
    fo = open(filenameOutput, "r")
    #---------------------------------
    readerIn  = csv.reader( fi , delimiter =',', skipinitialspace=True)
    readerOut = csv.reader( fo , delimiter =',', skipinitialspace=True)
    #---------------------------------
    for rowIn,rowOut in zip(readerIn,readerOut): 
        #------------------------------------------------------
        if (not receivedHeader): #use header to get labels
           #------------------------------------------------------
           CSVInputNumberOfColumns  = len(rowIn)       
           CSVOutputNumberOfColumns = len(rowOut)
           print("Number of Input Columns : " ,CSVInputNumberOfColumns)
           print("Number of Output Columns : ",CSVOutputNumberOfColumns)
           #---------------------------------  
           #Add labels for pure joints 
           #---------------------------------  
           inputLabelsOnly = list(rowIn[i] for i in range(0,CSVInputNumberOfColumns) )
           #As soon as we have the 2D input labels populate the configuration with the mapping ..! 
           configuration   = parseConfigurationInputJointMap(configuration,inputLabelsOnly)
           #---------------------------------  
           inputLabelsClean= inputLabelsOnly
           print("Number of Input elements without NSRM/NSDM, just joint 2D data : ",len(inputLabels))
            
           inputLabels = transformNetworkInputLabels(
                                                     configuration,
                                                     inputLabelsOnly,
                                                     CSVInputNumberOfColumns,
                                                     dtypeSelected,
                                                     ourDecomposition,
                                                     thisInput,
                                                     completelyDisablePCACode    = completelyDisablePCACode,
                                                     completelyDisableEigenposes = completelyDisableEigenposes
                                                    )
           print("inputLabels:",inputLabels)
           numberOfInputElements = len(inputLabels)

           print("\n\n\nNumber of Input elements with NSRM/NSDM: ",len(inputLabels),"\n\n\n")
           outputLabels = list(rowOut[i] for i in range(0,CSVOutputNumberOfColumns) )
           print("Input : ",inputLabels,"\n")
           #------------------------------------------------------ 
           print("Number of Output elements : ",len(outputLabels))
           print("Output : ",outputLabels,"\n")
           print("\n Now reading ",typeOfGroundTruthFile," files.. \n")
           #------------------------------------------------------
           if (memPercentage==0):
               print("Will only return labels\n")
               return {'labelIn':inputLabels,  'labelOut':outputLabels};

           #------------------------------------------  
           #  Allocate enough elements in the arrays 
           #------------------------------------------
           print(bcolors.WARNING,"Allocated input will have %u items / line in is %u items long "%(CSVInputNumberOfColumns,len(rowIn)) , bcolors.ENDC)
           for i in range(CSVInputNumberOfColumns): #
              thisInput.append(np.float32(0.0))
           #------------------------------------------
           for i in range(CSVOutputNumberOfColumns):
              thisOutput.append(np.float32(0.0))
           #-------------------------------------------

           #-------------------------------------------
           #           Allocate Numpy Arrays 
           #-------------------------------------------
           inputSize = len(inputLabels) 
           #-------------------------------------------

           #--------------------------------------------------------------------------------------------------------------------------
           npInputBytesize=0+numberOfSamplesLimit * inputSize * dtypeSelectedByteSize
           print("Input file on disk : ",filenameInput)
           print(typeOfGroundTruthFile," Input file on disk has a shape of [",numberOfSamples,",",inputSize,"]")  
           print(typeOfGroundTruthFile," Input we will read has a shape of [",numberOfSamplesLimit,",",inputSize,"]")  
           print(typeOfGroundTruthFile," Input will occupy ",convert_bytes(npInputBytesize)," of RAM\n")  
           npInput = np.full([numberOfSamplesLimit,inputSize],fill_value=0,dtype=dtypeSelected,order='C')
           #--------------------------------------------------------------------------------------------------------------------------
           npOutputByteSize=0+numberOfSamplesLimit * CSVOutputNumberOfColumns * dtypeSelectedByteSize
           print("Output file on disk : ",filenameOutput)
           print(typeOfGroundTruthFile," Output file on disk has a shape of [",numberOfSamples,",",CSVOutputNumberOfColumns,"]")  
           print(typeOfGroundTruthFile," Output we will read has a shape of [",numberOfSamplesLimit,",",CSVOutputNumberOfColumns,"]")  
           print(typeOfGroundTruthFile," Output will occupy ",convert_bytes(npOutputByteSize)," of RAM")  
           npOutput = np.full([numberOfSamplesLimit,CSVOutputNumberOfColumns],fill_value=0,dtype=dtypeSelected,order='C')
           #--------------------------------------------------------------------------------------------------------------------------
           print("\n Total occupied memory ",convert_bytes(npInputBytesize+npOutputByteSize)," of RAM\n")  
           
           receivedHeader=True
        else:
           #-------------------------------------------
           #  First convert our string INPUT to floats   
           #-------------------------------------------
           if (len(thisInput)!=len(rowIn)):
               print(bcolors.FAIL,"Inconsistency : array allocated %u elements, ParsedInput has %u elements , CSVInputNumberOfColumns has %u elements "%(len(thisInput),len(rowIn),CSVInputNumberOfColumns),bcolors.ENDC)
           for i in range(CSVInputNumberOfColumns):
                  thisInput[i]=(np.float32(rowIn[i]))
           #--------------------------------------------------------------------------------------------------------------------------
           if (filterOutEmptyInputSamples(thisInput,sampleNumber)):
              encounteredSamplesThatAreEmpty.append(sampleNumber)

           thisFullInput, thisNSDM, thisRearrangedInput, angleToRotate = transformNetworkInput(
                                                                                               configuration,
                                                                                               ourDecomposition,
                                                                                               thisInput,
                                                                                               dtypeSelected,
                                                                                               completelyDisablePCACode, 
                                                                                               completelyDisableEigenposes
                                                                                              )
           #--------------------------------------------------------------------------------------------------------------------------

           #-------------------------------------------
           #  Then convert our string OUTPUT to floats
           #-------------------------------------------
           for i in range(CSVOutputNumberOfColumns):
                  thisOutput[i]=np.float32(rowOut[i])
           #--------------------------------------------------------------------------------------
           
           #======================================================= 
           #             ACTUAL STORE IN NUMPY ARRAYS 
           #======================================================= 
           #======================================================= 
           if (useHalfFloats):
              #Populate data in the correct order in the final NumPy array
              #-------------------------------------------
              #Convert Input 
              for num in range(0,numberOfInputElements):
                  npInput[sampleNumber,num]  = np.float16(thisFullInput[num]); 
              #-------------------------------------------
              #Convert Output 
              for num in range(0,CSVOutputNumberOfColumns):
                  npOutput[sampleNumber,num] = np.float16(thisOutput[num]);
              #-------------------------------------------
           else:
              #Populate data in the correct order in the final NumPy array
              #-------------------------------------------
              #Convert Input 
              for num in range(0,numberOfInputElements):
                  npInput[sampleNumber,num]  = np.float32(thisFullInput[num]); 
              #-------------------------------------------
              for num in range(0,CSVOutputNumberOfColumns):
                  npOutput[sampleNumber,num] = np.float32(thisOutput[num]);
              #-------------------------------------------
           #=======================================================
           #=======================================================

           sampleNumber=sampleNumber+1

        if (numberOfSamples>0):
           progress=sampleNumber/numberOfSamplesLimit 
           if (sampleNumber%1000==0) :
              progressString = "%0.2f"%float(100*progress)  
              print("\rReading ",typeOfGroundTruthFile," from disk (",sampleNumber,") - ",progressString," %      \r", end="", flush=True)  

        if (numberOfSamplesLimit<=sampleNumber):
           print("\rStopping reading file to obey memory limit given by parameter --mem ",memPercentage,"\n")
           break
    #-------------------------------------------
    fi.close()
    fo.close()
    del readerIn
    del readerOut
    gc.collect()

    #-------------------------------------------
    print("\n",typeOfGroundTruthFile," read, Samples: ",sampleNumber,", was expecting ",numberOfSamples," samples\n") 
    print(npInput.shape)
    print(npOutput.shape)
    #-------------------------------------------
    totalNumberOfBytes=npInput.nbytes+npOutput.nbytes;
    totalNumberOfGigaBytes=totalNumberOfBytes/1073741824; 
    print("GPU Size Occupied by ",typeOfGroundTruthFile," data = ",totalNumberOfGigaBytes," GB \n")
    #-------------------------------------------
    end = time.time()

    print("Time elapsed : ",(end-start)/60," mins")
    

    #Filter out / Delete samples that are empty..
    print("Encountered ",len(encounteredSamplesThatAreEmpty)," empty samples :")
    print(encounteredSamplesThatAreEmpty)
    encounteredSamplesThatAreEmpty.reverse() #if not reversed we will screw up ordering..!
    for rowToDelete in encounteredSamplesThatAreEmpty:
          print("Deleting sample ",rowToDelete)
          npInput  = np.delete(npInput,  (int(rowToDelete)), axis=0)
          npOutput = np.delete(npOutput, (int(rowToDelete)), axis=0)



    #-------------------------------------------
    data = { 'labelInNoNSDM':inputLabelsOnly, 'labelIn':inputLabels, 'labelInClean':inputLabelsClean, 'in':npInput, 'labelOut':outputLabels, 'out':npOutput }
    #-------------------------------------------
    checkDataForNaN(data)
    #-------------------------------------------
    
    return data




#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------

"""
    readCSVLabels opens the first line of a CSV file and gives back a list with its labels
"""
def readCSVLabels(filename):
    if (not checkIfFileExists(filename)):
          print( bcolors.FAIL + "readCSVLabels: Input file "+filename+" does not exist, cannot read ground truth.." +  bcolors.ENDC)
          print("Current Directory was "+os.getcwd())
          sys.exit(1)

    print(bcolors.OKGREEN,"readCSVLabels: file ",filename," exists in filesystem",bcolors.ENDC)
    labels=list()
    fi = open(filename, "r")
    readerIn  = csv.reader( fi , delimiter =',', skipinitialspace=True)
    for rowIn in readerIn: 
           numberOfColumns=len(rowIn)       
           labels = list(rowIn[i] for i in range(0,numberOfColumns) )
           break;
    fi.close()
    
    #-------------------------------------------------------------- 
    originalRetrievedSize = len(labels)
    print(bcolors.OKGREEN,"it has ",originalRetrievedSize," elements",bcolors.ENDC)
    #Remove any empty occurances..
    try:
           labels.remove('') 
    except:
           print("CSV files doesnt seem to have blanks")
    
    if (originalRetrievedSize!=len(labels)):
          print(bcolors.FAIL,"Incorrect CSV line endings..",bcolors.ENDC)
          print(bcolors.FAIL,"Check if `head -n 1 ",filename,"` ends with a , ",bcolors.ENDC)
    #-------------------------------------------------------------- 

    return labels 

#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------

"""
    writeCSVFile Reads a CSV file where the body is floats.!
"""
def writeCSVFile(filename,data):
    f = open(filename, 'w')
    labels = data['label']
    body   = data['body']
    #Write header..
    #------------------------------------------------------------------------  
    for column in range(len(data['label'])):
      if (column>0):
        f.write(',')
      f.write("%s"%(data['label'][column]))
    f.write('\n')
    #------------------------------------------------------------------------  
    #Write body..
    #------------------------------------------------------------------------
    bodyLength = data['body'].shape[0]
    for frame in range(bodyLength): 
     for column in range(len(data['label'])):
      if (column>0):
        f.write(',') 
      f.write("%f"%(data['body'][frame][column]))
     f.write('\n')
    #------------------------------------------------------------------------  
    f.close()




"""
    readCSVFile Reads a CSV file where the body is floats.!
"""
def readCSVFile(filename,memPercentage=1.0,useHalfFloats=False):
    print("CSV file :",filename,"..\n")

    if (not checkIfFileExists(filename)):
          print( bcolors.FAIL + "Input file "+filename+" does not exist, cannot read ground truth.." +  bcolors.ENDC)
          print("Current Directory was "+os.getcwd())
          return dict()
    start = time.time()
     
    dtypeSelected=np.dtype(np.float32)  
    dtypeSelectedByteSize=int(dtypeSelected.itemsize) 
    if (useHalfFloats):
       dtypeSelected=np.dtype(np.float16) 
       dtypeSelectedByteSize=int(dtypeSelected.itemsize)

    progress=0.0
    sampleNumber=0
    receivedHeader=False
    inputNumberOfColumns=0 
    outputNumberOfColumns=0 

    inputLabels=list() 

    #-------------------------------------------------------------------------------------------------
    numberOfSamplesInput=getNumberOfLines(filename)-1
    print(" Input file has ",numberOfSamplesInput," training samples\n")
    #-------------------------------------------------------------------------------------------------


    numberOfSamples = numberOfSamplesInput
    numberOfSamplesLimit=int(numberOfSamples*memPercentage)
    #------------------------------------------------------------------------------------------------- 
    if (memPercentage==0.0):
        print("readCSVFile was asked to occupy 0 memory so this probably means we just want one record")    
        numberOfSamplesLimit=2
    if (memPercentage>1.0):
        print("Memory Limit will be interpreted as a raw value..")     
        numberOfSamplesLimit=int(memPercentage)
    #-------------------------------------------------------------------------------------------------

 
    #---------------------------------  
    thisInput = array.array('f')  
    #---------------------------------  

    fi = open(filename, "r") 
    readerIn  = csv.reader( fi , delimiter =',', skipinitialspace=True) 
    for rowIn in readerIn: 
        #------------------------------------------------------
        if (not receivedHeader): #use header to get labels
           #------------------------------------------------------
           inputNumberOfColumns=len(rowIn)
           inputLabels = list(rowIn[i] for i in range(0,inputNumberOfColumns) )
           print("Number of Input elements : ",len(inputLabels))
           #------------------------------------------------------

           if (memPercentage==0):
               print("Will only return labels\n")
               return {'label':inputLabels}; 

           #---------------------------------  
           #         Allocate Lists 
           #---------------------------------
           for i in range(inputNumberOfColumns):
               thisInput.append(np.float32(0.0))
           #---------------------------------  


           #---------------------------------  
           #      Allocate Numpy Arrays 
           #---------------------------------  
           inputSize=0
           startCompressed=0
           inputSize=inputNumberOfColumns
           startCompressed=inputNumberOfColumns  
           
           npInputBytesize=0+numberOfSamplesLimit * inputSize * dtypeSelectedByteSize
           print(" Input file on disk has a shape of [",numberOfSamples,",",inputSize,"]")  
           print(" Input we will read has a shape of [",numberOfSamplesLimit,",",inputSize,"]")  
           print(" Input will occupy ",convert_bytes(npInputBytesize)," of RAM\n")  
           npInput = np.full([numberOfSamplesLimit,inputSize],fill_value=0,dtype=dtypeSelected,order='C')
           #----------------------------------------------------------------------------------------------------------
           receivedHeader=True
           #----------------------------------------------------------------------------------------------------------
        else:
           #-------------------------------------------
           #  First convert our string INPUT to floats   
           #-------------------------------------------
           for i in range(inputNumberOfColumns):
                  try:
                    thisInput[i]=np.float32(rowIn[i]) 
                  except:
                    print("Could not parse ",rowIn[i]," to a float ")
                    thisInput[i]=np.float32(0.0) 
           #------------------------------------------- 
           for num in range(0,inputNumberOfColumns):
                  npInput[sampleNumber,num]=np.float32(thisInput[num]); 
           #-------------------------------------------
           sampleNumber=sampleNumber+1

        if (numberOfSamples>0):
           progress=sampleNumber/numberOfSamplesLimit

        if (sampleNumber%1000==0) :
           progressString = "%0.2f"%float(100*progress)  
           print("\rReading from disk (",sampleNumber,") - ",progressString," %      \r", end="", flush=True)  

        if (numberOfSamplesLimit<=sampleNumber):
           print("\rStopping reading file to obey memory limit given by parameter --mem ",memPercentage,"\n")
           break
    #-------------------------------------------
    fi.close()
    del readerIn
    gc.collect()


    print("\n read, Samples: ",sampleNumber,", was expecting ",numberOfSamples," samples\n") 
    print(npInput.shape)

    totalNumberOfBytes=npInput.nbytes;
    totalNumberOfGigaBytes=totalNumberOfBytes/1073741824; 
    print("GPU Size Occupied by data = ",totalNumberOfGigaBytes," GB \n")

    end = time.time()
    print("Time elapsed : ",(end-start)/60," mins")

    labelDict = dict()
    for i in range(0,len(inputLabels)):
       labelDict[inputLabels[i]]=i
    #---------------------------------------------------------------------
    return {'label':inputLabels,  'labelLookup':labelDict, 'body':npInput };



if __name__ == '__main__':
    print("readCSV.py is a library it cannot be run standalone")
 
