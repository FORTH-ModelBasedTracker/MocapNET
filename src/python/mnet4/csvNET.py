#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

#pip install mediapipe pandas pillow matplotlib opencv-python

import cv2
import numpy as np
import time
import sys
import os

from readCSV  import parseConfiguration,zeroOutXYJointsThatAreInvisible,performNSRMAlignment,splitNumpyArray
from NSDM     import NSDMLabels,createNSDMUsingRules
from MocapNET import MocapNET,flipHorizontalInput
from tools    import getDirectoryFromPath, checkIfFileExists, secondsToHz, eprint

#------------------------------------------------
#------------------------------------------------
#------------------------------------------------
class csvNET():
  def __init__(self,
               datasetDirectory = "dataset/generated",
               dataFile         = "face",
               filenamePostfix  = "_all",
               fromCSV          = "",
               mem              = 1.0,
               outputMode       = "bvh",
               height           = 1080,
               width            = 1920
               ):
               #------------------------------------------
               from readCSV import readCSVFile
               if (fromCSV!=""):
                 self.data2D  = readCSVFile("%s" % (fromCSV),mem,0)
                 self.dataBVH = dict()
               else:
                 self.data2D  = readCSVFile("%s/2d_%s%s.csv" % (datasetDirectory,dataFile,filenamePostfix),mem,0)
                 self.dataBVH = readCSVFile("%s/%s_%s%s.csv" % (datasetDirectory,outputMode,dataFile,filenamePostfix),mem,0)
               #------------------------------------------
               self.height              = height
               self.width               = width
               self.output              = dict()
               #------------------------------------------
               self.error               = dict()
               self.error["body"]       = list()
               #------------------------------------------
               self.rSquared            = dict()
               self.rSquared["body"]    = list() 
               #------------------------------------------
               self.responsesNN         = dict()
               self.responsesNN["body"] = list()
               #------------------------------------------

  def getNumberOfSamples(self): 
           numberOfSamples = self.data2D["body"].shape[0]
           #numberOfOutputs = data["out"].shape[1]
           return numberOfSamples

  def get2DOutput(self):
        return self.output
  
  def registerNNResponse(self,outputToCompare,mocapNETResponse):
    self.responsesNN["label"]=outputToCompare
    thisRow  =  list()
    for label in outputToCompare:
          label = label.lower() # <- make it lowercase
          thisRow.append(mocapNETResponse[label])
    self.responsesNN["body"].append(thisRow)

  def registerError(self,outputToCompare,mocapNETDifference):
    self.error["label"]=outputToCompare
    thisRow  =  list()
    for label in outputToCompare:
          label = label.lower() # <- make it lowercase
          thisRow.append(mocapNETDifference[label])
    self.error["body"].append(thisRow)


  def saveCSVDataAsCSVFile(self,labelprefix,data,filename):
    if (data) and ("label" in data) and ("body" in data):
      print("Writting encountered error at ",filename)
      f = open(filename, 'w')

      #Write header
      recordsWritten=0 
      for item in data["label"]:
            if (recordsWritten>0):
               f.write(',')
            f.write("%s_%s" % (labelprefix,item)) #We prepend loss to make sure  this is not used the wrong way..
            recordsWritten=recordsWritten+1
      f.write('\n')
      #--------------------------------------------------------------------------------------------------
      #Write body
      for sample in range(0,len(data["body"])):
          recordsWritten=0 
          for value in range(0,len(data["body"][sample])):
            if (recordsWritten>0):
               f.write(',')
            f.write("%f" % (data["body"][sample][value]))
            recordsWritten=recordsWritten+1
          f.write('\n')
      f.close()
      print("Done writting ",filename)
    #--------------------------------------------------------------------------------------------------

  def saveErrorAsCSV(self,filename):
      self.saveCSVDataAsCSVFile("loss",self.error,filename)

  def saveResponseAsCSV(self,filename):
      self.saveCSVDataAsCSVFile("NNOutput",self.responsesNN,filename)

  def saveRSquared(self,filename):
    if (self.dataBVH):
      self.rSquared["label"] = self.dataBVH["label"]
      
      thisResult = list()
      column = 0
      for columnName in self.dataBVH["label"]:
        #We cut a single dimension as a 1 dim numpy array to make things easier..
        singleOutputDimensionGroundTruth = splitNumpyArray(self.dataBVH["body"],column,1,0)
        meanGroundTruth = np.mean(singleOutputDimensionGroundTruth)
        #print(columnName," -> mean ",meanGroundTruth)

        totalSumOfSquaresProportionalToTheVarianceOfData = 0.0 
        for sample in range(0,singleOutputDimensionGroundTruth.shape[0]):
            totalSumOfSquaresProportionalToTheVarianceOfData = totalSumOfSquaresProportionalToTheVarianceOfData + ((singleOutputDimensionGroundTruth[sample][0] - meanGroundTruth) ** 2)
        #print("Total Sum of Squares = ",totalSumOfSquaresProportionalToTheVarianceOfData," samples = ",singleOutputDimensionGroundTruth.shape[0])

        sumOfSquaresOfResiduals = 0.0
        for sample in range(0,len(self.error["body"])):
            sumOfSquaresOfResiduals = sumOfSquaresOfResiduals + (self.error["body"][sample][column] ** 2)
        #print("Sum of Squares of residuals = ",sumOfSquresOfResiduals," samples = ",len(self.error["body"]))
        
        if (totalSumOfSquaresProportionalToTheVarianceOfData!=0.0):
          rsquared = float( 1.0 - sumOfSquaresOfResiduals/totalSumOfSquaresProportionalToTheVarianceOfData )
          print("R² for %s = %0.2f "%(columnName,rsquared))
          thisResult.append(rsquared)
        else:
          print("R² for %s = NaN "%(columnName))
          thisResult.append(float('nan'))
        
        column = column + 1
        results = list()
        results.append(thisResult)
        self.rSquared["body"] = results
        self.saveCSVDataAsCSVFile("RSquared",self.rSquared,filename)
      

  def compareToGroundTruth(self,fID,outputToCompare):
   if (self.dataBVH):
    count = 0
    mocapNETOutput     = dict()
    mocapNETDifference = dict()
    #--------------------------------------------------------------------------------------------------
    for label in self.dataBVH["label"]:
          label = label.lower() # <- make it lowercase
          if (label in outputToCompare):
             mocapNETOutput[label]     = outputToCompare[label]
             mocapNETDifference[label] = abs(self.dataBVH["body"][fID][count] - outputToCompare[label])
             #print("For ",label," we regressed ",outputToCompare[label]," gt was ",self.dataBVH["body"][fID][count]," | loss -> ",mocapNETDifference[label] )
          else:
             print("Output ",label," missing")
             mocapNETDifference[label] = abs(self.dataBVH["body"][fID][count]) 
          count = count + 1
    #-------- Automatically register error ----------------
    self.registerError(outputToCompare,mocapNETDifference)
    self.registerNNResponse(outputToCompare,mocapNETOutput)
    #------------------------------------------------------
    return mocapNETDifference
    #--------------------------------------------------------------------------------------------------
   else:
    return float("nan")


  def getMocapNETInputForFrameID(self,fID,img):
    count = 0
    mocapNETInput = dict()
    for label in self.data2D["label"]:
          label = label.lower() # <- make it lowercase
          mocapNETInput[label] = self.data2D["body"][fID][count]
          count = count + 1  
    #--------------------------------------------------------------------------------------------------
    #annotated_image = np.zeros((self.height,self.width,3), np.uint8) #<- this is now provided to this function
    #from MocapNETVisualization import drawMocapNETInput
    #drawMocapNETInput(mocapNETInput,annotated_image)
    #--------------------------------------------------------------------------------------------------
    #print("Frame : ",fID," -> ",mocapNETInput)
    from holisticPartNames import  guessLandmarks
    mocapNETInput = guessLandmarks(mocapNETInput)
    #-------------------------------------------------------------------------------------------- 
    self.output = mocapNETInput
    #-------------------------------------------------------------------------------------------- 
    return mocapNETInput,img
#------------------------------------------------
#------------------------------------------------
#------------------------------------------------

def calculate_relative_magnitudes(edm, n):
    row_magnitudes = []
    column_magnitudes = []
    for i in range(n):
        row_magnitude = sum(edm[i*n:(i+1)*n])
        row_magnitudes.append(row_magnitude)
        column_magnitude = sum(edm[i::n])
        column_magnitudes.append(column_magnitude)
    central_element = edm[n*n // 2]
    row_magnitudes_sum         = sum(row_magnitudes) - central_element
    column_magnitudes_sum      = sum(column_magnitudes) - central_element
    max_magnitude              = max(row_magnitudes_sum, column_magnitudes_sum)
    relative_row_magnitudes    = [m / max_magnitude for m in row_magnitudes]
    relative_column_magnitudes = [m / max_magnitude for m in column_magnitudes]
    return relative_row_magnitudes, relative_column_magnitudes


def getNSRMInterest(mnet,part=""):
  from tools import appendCSVToFile
  if part in mnet.ensemble:
       NSRM = mnet.ensemble[part].NSRM
       appendCSVToFile("study.csv",NSRM,fID=mnet.framesProcessed-1)
       #row,column = calculate_relative_magnitudes(NSRM,int(np.sqrt(len(NSRM))))
       #print("ROW ",row," COLUMN ",column)
       #appendCSVToFile("study.csv",row,fID=mnet.framesProcessed-1)
  
 

#------------------------------------------------
#------------------------------------------------
#------------------------------------------------
def streamPosesFromCameraToMocapNET():
  engine = "onnx"
  headless         = False
  dataFile         = "face"
  whiteBkg         = False
  doProfiling      = False
  doMnetVisualization = True
  doVisualization  = True
  csvFilePath      = "" 
  saveVideo        = False
  flipHorizontal   = False
  doBody           = True
  doUpperbody      = False, #<- These get auto activated if doBody=True
  doLowerbody      = False, #<- These get auto activated if doBody=True
  doFace           = False
  doREye           = False
  doMouth          = False
  doHands          = False
  addNoise         = 0.0
  aspectCorrection = 1.0
  scale            = 1.0
  mem              = 1.0
  windowDelay      = 1
  doHCDPostProcessing = 1
  hcdLearningRate     = 0.001
  hcdEpochs           = 30
  hcdIterations       = 15
  plotBVHChannels  = False
  bvhAnglesForPlotting    = list()
  bvhAllAnglesForPlotting = list()
  study = ""
  calibrationFile = ""
  keepColorImage  = " && rm colorFrame_0_*.jpg "
  keepPlotImage   = " && rm plotFrame_0_*.jpg "
  #python3 mediapipeHolisticWebcamMocapNET.py --from damien.avi --face --nobody --plot --save
  #python3 -m csvNET --from ammarFaceFar.csv --study face --face --nobody
  # python3 -m csvNET --from ammarFaceFar.csv --mouth --reye --nobody --plot --save 

  #Visually inspect validation set :
  #python3 -m csvNET --from dataset/generated/2d_body_all_test.csv --noik 
  #python3 -m csvNET --from dataset/generated/2d_body_all_test.csv 


  if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--headless"):
              headless = True
           if (sys.argv[i]=="--nomnetvisualization"):
              doMnetVisualization = False
           if (sys.argv[i]=="--novisualization"):
              doVisualization = False
           if (sys.argv[i]=="--ik"):
               hcdLearningRate     = float(sys.argv[i+1])
               hcdEpochs           = int(sys.argv[i+2])
               hcdIterations       = int(sys.argv[i+3])
           if (sys.argv[i]=="--noik"):
              doHCDPostProcessing = 0
           if (sys.argv[i]=="--aspectCorrection"):
              aspectCorrection=float(sys.argv[i+1])
           if (sys.argv[i]=="--noise"):
              addNoise=float(sys.argv[i+1])
           if (sys.argv[i]=="--scale"):
              scale=float(sys.argv[i+1])
           if (sys.argv[i]=="--calib"):
              calibrationFile = sys.argv[i+1]
           if (sys.argv[i]=="--study"):
              study = sys.argv[i+1]
           if (sys.argv[i]=="--flipHorizontal"):
              flipHorizontal = True
           if (sys.argv[i]=="--plot"):
              plotBVHChannels=True
           if (sys.argv[i]=="--all"):
              doBody=True
              doREye=True
              doMouth=True
              doHands=True
           if (sys.argv[i]=="--nobody"):
              doBody      = False
              doUpperbody = False 
              doLowerbody = False
           if (sys.argv[i]=="--upperbody"):
              doBody      = False
              doUpperbody = True 
           if (sys.argv[i]=="--lowerbody"):
              doBody      = False
              doLowerbody = True 
           if (sys.argv[i]=="--face"):
              doFace=True
              dataFile="face" #Use 2d_face_all.csv as input if not --from is activated
           if (sys.argv[i]=="--reye"):
              doREye=True
              dataFile="reye" #Use 2d_reye_all.csv as input if not --from is activated
           if (sys.argv[i]=="--mouth"):
              doMouth=True
              dataFile="mouth" #Use 2d_mouth_all.csv as input if not --from is activated
           if (sys.argv[i]=="--hands"):
              doHands=True
           if (sys.argv[i]=="--white"):
              whiteBkg = True
           if (sys.argv[i]=="--keep"):
              keepColorImage = " "
              keepPlotImage  = " "
           if (sys.argv[i]=="--save"):
              saveVideo=True
           if (sys.argv[i]=="--engine"):
              engine=sys.argv[i+1]
              print("Selecting engine : ",engine)
           if (sys.argv[i]=="--from"):
              csvFilePath=sys.argv[i+1]
           if (sys.argv[i]=="--profile"):
              doProfiling=True
           if (sys.argv[i]=="--mem"):
              mem=int(sys.argv[i+1])
           if (sys.argv[i]=="--delay"):
              windowDelay=int(sys.argv[i+1])

  from MocapNETVisualization import drawMocapNETOutput,drawDescriptor,drawNSRM,drawMAE2DError,drawMocapNETAllPlots

  #Select a MocapNET class from tensorflow/tensorrt/onnx/tf-lite engines
  from MocapNET import easyMocapNETConstructor
  mnet = easyMocapNETConstructor(
                                 engine,
                                 doProfiling=doProfiling,
                                 doHCDPostProcessing=doHCDPostProcessing,
                                 hcdLearningRate=hcdLearningRate,
                                 hcdEpochs = hcdEpochs,
                                 hcdIterations = hcdIterations,
                                 bvhScale=scale,
                                 doBody=doBody,
                                 doUpperbody=doUpperbody,
                                 doLowerbody=doLowerbody,
                                 doFace=doFace,
                                 doREye=doREye,
                                 doMouth=doMouth,
                                 doHands=doHands,
                                 addNoise=addNoise
                                )

  mnet.test()
  mnet.recordBVH(True) 
  #Body only
  mp = csvNET(mem=mem,dataFile=dataFile,fromCSV=csvFilePath)
   
  from folderStream import FolderStreamer
  baseNameForFolderThatMayContainImages = getDirectoryFromPath(csvFilePath)
  print("baseNameForFolderThatMayContainImages = ",baseNameForFolderThatMayContainImages)
  imageStream = FolderStreamer(path=baseNameForFolderThatMayContainImages)
  if (checkIfFileExists("%s/color.calib" % baseNameForFolderThatMayContainImages)):
        mnet.bvh.configureRendererFromFile("%s/color.calib"%baseNameForFolderThatMayContainImages)


  if (calibrationFile!=""):
        print("Enforcing Calibration file : ",calibrationFile)
        mnet.bvh.configureRendererFromFile(calibrationFile)
  

  from MocapNETVisualization import visualizeMocapNETEnsemble
  #------------------------------------------------
  #------------------------------------------------
  #------------------------------------------------
  print("\n\n\n\nStarting MocapNET using CSV 2D Input")
  print("Please wait until input processing finishes!")
  for frameNumber in range(0, mp.getNumberOfSamples() ):
    #attempt to visualize (!)
    #--------------------------------------------------------------------------------------------------------------
    success,img = imageStream.read()
    if (not success):
        if (whiteBkg):
            annotated_image = np.full((mp.height,mp.width,3), 255, dtype=np.uint8)
        else: 
            annotated_image = np.zeros((mp.height,mp.width,3), np.uint8)
    else:
        annotated_image = img
    #--------------------------------------------------------------------------------------------------------------
    start = time.time() # Time elapsed
    mocapNETInput,annotated_image = mp.getMocapNETInputForFrameID(frameNumber,annotated_image) 
    #--------------------------------------------------------------------------------------------------------------
    if (flipHorizontal):
        mocapNETInput = flipHorizontalInput(mocapNETInput)
    #--------------------------------------------------------------------------------------------------------------
    mocapNET3DOutput  = mnet.predict3DJoints(mocapNETInput) 
    mocapNETBVHOutput = mnet.outputBVH
   
    getNSRMInterest(mnet,part=study)

    difference = mp.compareToGroundTruth(frameNumber,mocapNETBVHOutput)
    #print("Frame ",frameNumber," difference ~=> ",difference)
    #print("Frame ",frameNumber,"/",mp.getNumberOfSamples())

    bvhAnglesForPlotting.append(mocapNETBVHOutput)
    bvhAllAnglesForPlotting.append(mocapNETBVHOutput)
    if (len(bvhAnglesForPlotting)>100):
       bvhAnglesForPlotting.pop(0)

    if (doVisualization):
     #--------------------------------------------------------------------------------------------------------------
     annotated_image,plotImage = visualizeMocapNETEnsemble(mnet,annotated_image,plotBVHChannels=plotBVHChannels,bvhAnglesForPlotting=bvhAnglesForPlotting,drawOutput=doMnetVisualization)
     #--------------------------------------------------------------------------------------------------------------


     mnet.printStatus()


     if (saveVideo): 
        cv2.imwrite('colorFrame_0_%05u.jpg'%(frameNumber), annotated_image)
        if (plotBVHChannels):
              cv2.imwrite('plotFrame_0_%05u.jpg'%(frameNumber), plotImage)    
     #--------------------------------------------------------------------------------------------------------------
     if not headless:
        cv2.imshow('MocapNET 4 using CSV Data Holistic 2D Joints', annotated_image)
        if (plotBVHChannels):
            cv2.imshow('MocapNET 4 using CSV Data Holistic Motion History',plotImage) 
 
        if cv2.waitKey(1) & 0xFF == 27:
          break 

     #--------------------------------------------------------------------------------------------------------------
     if (csvFilePath == ""):
       #This uses MATPLOTLIB and is very slow
       #only emmit this when working with ground truth and not an ad hoc CSV file
       from MocapNETVisualization import drawMocapNETFrequencyPlots 
       drawMocapNETFrequencyPlots(bvhAllAnglesForPlotting)
     #--------------------------------------------------------------------------------------------------------------
  
  #Finished
  #========================== 
  mp.saveErrorAsCSV("loss.csv")
  mp.saveResponseAsCSV("freq.csv")
  mp.saveRSquared("rSquared.csv")

  if (doVisualization) and (saveVideo): 
     os.system("ffmpeg -framerate 30 -i colorFrame_0_%%05d.jpg  -s 1200x720  -y -r 30 -pix_fmt yuv420p -threads 8 livelastRun3DHiRes.mp4 %s" % keepColorImage)
     if (plotBVHChannels):
        os.system("ffmpeg -framerate 30 -i plotFrame_0_%%05d.jpg  -s 1200x720  -y -r 30 -pix_fmt yuv420p -threads 8 livelastPlot3DHiRes.mp4 %s" % keepPlotImage)
      
   
  del mnet #So that the out.bvh file gets created..
  if (csvFilePath != ""):
        os.system("rm 2d_out.csv 3d_out.csv bvh_out.csv map_out.csv")
        os.system("./GroundTruthDumper --from out.bvh --setPositionRotation -2.6 0 2000 0 0 0  --csv ./ out.csv 2d+3d+bvh ") # Remove noise offsetPositionRotation

#------------------------------------------------
#------------------------------------------------
#------------------------------------------------

#TEST
#python3 -m csvNET --nobody --face
if __name__ == '__main__':
    streamPosesFromCameraToMocapNET()
