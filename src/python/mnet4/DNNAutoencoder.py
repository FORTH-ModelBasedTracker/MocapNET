 #!/usr/bin/python3
import os
import sys
import gc
import time
from keras.layers import Input, Dense
from keras.models import Model
from keras.models import Sequential
from keras.models import model_from_json
from keras.utils import plot_model

import keras.callbacks
import numpy as np 
from readCSV import checkIfTestAndTrainListsAreOk,readGroundTruthFile,splitNumpyArray
from DNNModel import loadModel,saveModel,newAutoencoderModel,saveConfiguration,saveLastCompletedJob
from plotDNN import plotDNNHistory
from compress2DPoints import getNumberOfCompressedJoints
 
def printArray(X):   
    for sample in range(0,len(X)):
      for item in range(0,len(X[sample])):
          print(X[sample][item],end=" ")
      print("\n\n\n")

def zeroFirstSixParameters(X): 
    for sample in range (0,len(X)):
      X[sample][0]=0.0
      X[sample][1]=0.0
      X[sample][2]=0.0
      X[sample][3]=0.0
      X[sample][4]=0.0
      X[sample][5]=0.0

def normalizeParameters(X,labels):
    useSTD=0
    startPosition=0    
    endPosition=len(X[0])
    for item in range(startPosition,endPosition):
      outputName=labels[item]
      #----------------------------------------
      itemOnlyArray = splitNumpyArray(X,item)
      #----------------------------------------
      maximum=np.max(itemOnlyArray)
      minimum=np.min(itemOnlyArray)
      median=np.median(itemOnlyArray)
      mean=np.mean(itemOnlyArray)
      std=np.std(itemOnlyArray)
      var=np.var(itemOnlyArray)
      #---------------------------------
      titleString="%s : Median=%0.2f,Mean=%0.2f,Std=%0.2f,Var=%0.2f" % (outputName,median,mean,std,var)
      print("Stats of",titleString)
      
      if (useSTD):
         if (std==0.0):
             std=1.0
         for sample in range(0,len(X)):
             meanCorrected = X[sample][item] - mean
             X[sample][item] = float(meanCorrected/std) 
      else:
         normalizationValue = (maximum-minimum)
         if (normalizationValue==0.0):
             normalizationValue=1.0
         for sample in range(0,len(X)):
             meanCorrected = X[sample][item] - minimum
             X[sample][item] = float(meanCorrected/normalizationValue) 


def writeSkeleton(X,name):
    os.system('cp header.bvh %s.bvh'  % name)
    fileP = open("%s.bvh" % name,"a") 
    fileP.write("\nMOTION\n")
    fileP.write("Frames: 1\n") 
    fileP.write("Frame Time: 0.04\n") #25fps 
    for i in range(0,len(X)):
        fileP.write("%0.4f" % X[i])
        fileP.write(" ")
    fileP.write("\n") 

def getNumberOfSkeletonsCloseToNeedle(haystack,needle,hitAccuracy,threshold):
    numberOfMatches=0
    startPosition=6    
    endPosition=len(haystack[0])
    numberOfItems=endPosition-startPosition
    for sample in range(0,len(haystack)):
        thisItemMatches=0  
        for item in range(startPosition,endPosition):
           if (np.abs(needle[item]-haystack[sample][item])<threshold):
              thisItemMatches=thisItemMatches+1
        #print("Hits ",thisItemMatches,"/",numberOfItems," = ",float(thisItemMatches/numberOfItems))
        if (thisItemMatches>=numberOfItems*hitAccuracy):
           numberOfMatches=numberOfMatches+1
 
    return numberOfMatches

def getMeanSkeleton(X,labels):
    startPosition=0    
    endPosition=len(X[0])

    meanSkeleton=list()
    normalizationOfSkeleton=list()

    for item in range(startPosition,endPosition):
      outputName=labels[item]
      #----------------------------------------
      itemOnlyArray = splitNumpyArray(X,item)
      #----------------------------------------
      maximum=np.max(itemOnlyArray)
      minimum=np.min(itemOnlyArray)
      mean=np.mean(itemOnlyArray)
      #--------------------------------- 
      meanSkeleton.append(mean)
      normalizationOfSkeleton.append(maximum-minimum)
    print("Mean Skeleton : ")
    for i in range(0,len(meanSkeleton)):
        print(meanSkeleton[i],",",end="")
    print("\nNormalization Of Skeleton : ")
    for i in range(0,len(normalizationOfSkeleton)):
        print(normalizationOfSkeleton[i],",",end="")
    print("\n")
    
    writeSkeleton(meanSkeleton,"meanSkeleton")
    percentage=0.7
    threshold=1.0
    print("%0.2f%% - %0.2f deg - There are %u/%u skeletons close to the mean one\n" % (100*percentage,threshold,getNumberOfSkeletonsCloseToNeedle(X,meanSkeleton,percentage,threshold),len(X)))
    percentage=0.7
    threshold=5.0
    print("%0.2f%% - %0.2f deg - There are %u/%u skeletons close to the mean one\n" % (100*percentage,threshold,getNumberOfSkeletonsCloseToNeedle(X,meanSkeleton,percentage,threshold),len(X)))
    percentage=0.8
    threshold=5.0
    print("%0.2f%% - %0.2f deg - There are %u/%u skeletons close to the mean one\n" % (100*percentage,threshold,getNumberOfSkeletonsCloseToNeedle(X,meanSkeleton,percentage,threshold),len(X)))
    percentage=0.85
    threshold=10.0
    print("%0.2f%% - %0.2f deg - There are %u/%u skeletons close to the mean one\n" % (100*percentage,threshold,getNumberOfSkeletonsCloseToNeedle(X,meanSkeleton,percentage,threshold),len(X)))
    percentage=0.90
    threshold=10.0
    print("%0.2f%% - %0.2f deg - There are %u/%u skeletons close to the mean one\n" % (100*percentage,threshold,getNumberOfSkeletonsCloseToNeedle(X,meanSkeleton,percentage,threshold),len(X)))
    return meanSkeleton,normalizationOfSkeleton

print("DNNTrainer starting up..\n")
np.set_printoptions(precision=2) 
np.set_printoptions(formatter={"float_kind": lambda x: "%g" % x})

 
#At this point we are ready..
#We should free whatever memory we have allocated because it is training time..!
gc.collect()
 
compressionFactor=2
mode=1
memPercentage=0.1
dataFile="dataFront"

if (len(sys.argv)>1):
   #print('Argument List:', str(sys.argv))
   for i in range(0, len(sys.argv)):
       if (sys.argv[i]=="--mem"):
          print("\nMemory usage ",sys.argv[i+1]);
          memPercentage=float(sys.argv[i+1])
       if (sys.argv[i]=="--compression"):
          print("\nCompression Factor",sys.argv[i+1]);
          compressionFactor=float(sys.argv[i+1])
       if (sys.argv[i]=="--all"): 
          #Don't mix this --all with the step2_OrientatonClassifier.py --all 
          hierarchyPartName=sys.argv[i+1] 
          dataFile="%s_all" % hierarchyPartName
          outputDirectoryForTrainedModels="autoencoder_%s" % dataFile
       if (sys.argv[i]=="--back"): 
          hierarchyPartName=sys.argv[i+1] 
          dataFile="%s_back" % hierarchyPartName
          outputDirectoryForTrainedModels="autoencoder_%s" % dataFile
       if (sys.argv[i]=="--front"): 
          hierarchyPartName=sys.argv[i+1]
          dataFile="%s_front" % hierarchyPartName
          outputDirectoryForTrainedModels="autoencoder_%s" % dataFile
       if (sys.argv[i]=="--left"): 
          hierarchyPartName=sys.argv[i+1]
          dataFile="%s_left" % hierarchyPartName
          outputDirectoryForTrainedModels="autoencoder_%s" % dataFile
       if (sys.argv[i]=="--right"): 
          hierarchyPartName=sys.argv[i+1]
          dataFile="%s_right" % hierarchyPartName
          outputDirectoryForTrainedModels="autoencoder_%s" % dataFile
       #-----------------------------------------------    
       if (sys.argv[i]=="--in"):
          mode=0
       if (sys.argv[i]=="--out"):
          mode=1
  
print("\n-------------------------------------------------");
useNonCompressed=1
useCompressed=1
numberOfEpochs=1000
useRadians=0 

testMemPercentage=0.01
if (memPercentage==0): testMemPercentage=memPercentage 
if (memPercentage>1): testMemPercentage=memPercentage 

groundTruthTrain = readGroundTruthFile("Train","../../DNNTracker/dataset/2d_%s.csv" % dataFile,"../../DNNTracker/dataset/bvh_%s.csv" % dataFile,memPercentage,useNonCompressed,useCompressed,useRadians)
groundTruthTest  = readGroundTruthFile("Test","../../DNNTracker/dataset/2d_test.csv" ,"../../DNNTracker/dataset/bvh_test.csv",testMemPercentage,useNonCompressed,useCompressed,useRadians)
checkIfTestAndTrainListsAreOk(groundTruthTest,groundTruthTrain)
print("\n-------------------------------------------------");


if (mode==0):
   field='in'
   fieldLabel='labelIn'
else:
   field='out'
   fieldLabel='labelOut'

outputName="autoencoder"
startAt = time.time()
autoencoder = newAutoencoderModel(
                                  outputName,
                                  len(groundTruthTrain[field][0]),
                                  compressionFactor
                                 ) 

getMeanSkeleton(groundTruthTrain[field],groundTruthTrain[fieldLabel])
 
normalizeParameters(groundTruthTrain[field],groundTruthTrain[fieldLabel])
sys.exit(0)



if (field=='out'):
   zeroFirstSixParameters(groundTruthTest[field])

#printArray(groundTruthTest[field])

history = autoencoder.fit(
                          groundTruthTrain[field],groundTruthTrain[field],
                          epochs=numberOfEpochs,
                          batch_size=128,
                          shuffle=True,
                          validation_data=(groundTruthTest[field],groundTruthTest[field])
                         )


#---------------------------------------------------------------------
#-------------------- STORE CONFIGURATION ----------------------------
endAt = time.time()
print("Time required to train ",outputName," was ",(endAt-startAt)/60," mins")
#---------------------------------------------------------------------
saveModel(outputName,autoencoder) 
#---------------------------------------------------------------------

print("Sucessfully ended..\n")
