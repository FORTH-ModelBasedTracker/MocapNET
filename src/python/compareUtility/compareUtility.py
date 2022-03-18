#!/usr/bin/env python3
#Written by Ammar Qammaz a.k.a AmmarkoV - 2020 
#pip install matplotlib h5py numpy --user

from align2DPoints import pointListsReturnAvgDistance,compute_similarity_transform
from drawPointClouds import findJointID,get3DDistance,setupDrawing,drawLimbDimensions,drawLimbError,drawFrameError,drawAfterEndOfComparison
from writeCSVResults import writeCSVFileResults,appendRAWResultsForGNUplot

import numpy as np
import gc
import os
import sys
import csv
import time
import array

import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from mpl_toolkits.mplot3d import axes3d, Axes3D  

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

ROOT_JOINT="midhip"

#I have added a seperate list with the joints we want to compare
#to avoid the weird parent lists when you remove one joint
JOINTS_TO_COMPARE=list()
#JOINTS_TO_COMPARE.append('Nose')       #0
JOINTS_TO_COMPARE.append('Neck')        #1
JOINTS_TO_COMPARE.append('RShoulder')   #2
JOINTS_TO_COMPARE.append('RElbow')      #3
JOINTS_TO_COMPARE.append('RWrist')      #4
JOINTS_TO_COMPARE.append('LShoulder')   #5
JOINTS_TO_COMPARE.append('LElbow')      #6
JOINTS_TO_COMPARE.append('LWrist')      #7
JOINTS_TO_COMPARE.append('MidHip')     #8
JOINTS_TO_COMPARE.append('RHip')        #9
JOINTS_TO_COMPARE.append('RKnee')       #10
JOINTS_TO_COMPARE.append('RAnkle')      #11
JOINTS_TO_COMPARE.append('LHip')        #12
JOINTS_TO_COMPARE.append('LKnee')       #13
JOINTS_TO_COMPARE.append('LAnkle')      #14


#These are our labels they have to match what we want to process
#If you don't want something to be part of the measurment remove it from here
#as well as from the JOINT_PARENTS list
#---------------------------------------------------------------------
JOINT_LABELS=list()
JOINT_LABELS.append('Nose')        #0
JOINT_LABELS.append('Neck')        #1
JOINT_LABELS.append('RShoulder')   #2
JOINT_LABELS.append('RElbow')      #3
JOINT_LABELS.append('RWrist')      #4
JOINT_LABELS.append('LShoulder')   #5
JOINT_LABELS.append('LElbow')      #6
JOINT_LABELS.append('LWrist')      #7
JOINT_LABELS.append('MidHip')      #8
JOINT_LABELS.append('RHip')        #9
JOINT_LABELS.append('RKnee')       #10
JOINT_LABELS.append('RAnkle')      #11
JOINT_LABELS.append('LHip')        #12
JOINT_LABELS.append('LKnee')       #13
JOINT_LABELS.append('LAnkle')      #14
JOINT_LABELS.append('REye')        #15
JOINT_LABELS.append('LEye')        #16
JOINT_LABELS.append('REar')        #17
JOINT_LABELS.append('LEar')        #18
JOINT_LABELS.append('LBigToe')     #19
JOINT_LABELS.append('LSmallToe')   #20
JOINT_LABELS.append('LHeel')       #21
JOINT_LABELS.append('RBigToe')     #22
JOINT_LABELS.append('RSmallToe')   #23
JOINT_LABELS.append('RHeel')       #24



#These are our parents they have to match JOINT_LABELS
#If you don't want something to be part of the measurment remove it from here
#as well as from the JOINT_LABELS list
#---------------------------------------------------------------------
JOINT_PARENTS=list()
JOINT_PARENTS.append(findJointID("Neck",JOINT_LABELS))      #0 Parent of Nose is Neck
JOINT_PARENTS.append(findJointID("MidHip",JOINT_LABELS))    #1 Parent of Neck is MidHip
JOINT_PARENTS.append(findJointID("Neck",JOINT_LABELS))      #2 Parent of RShoulder is Neck
JOINT_PARENTS.append(findJointID("RShoulder",JOINT_LABELS)) #3 Parent of RElbow is RShoulder
JOINT_PARENTS.append(findJointID("RElbow",JOINT_LABELS))    #4 etc ...
JOINT_PARENTS.append(findJointID("Neck",JOINT_LABELS))      #5
JOINT_PARENTS.append(findJointID("LShoulder",JOINT_LABELS)) #6
JOINT_PARENTS.append(findJointID("LElbow",JOINT_LABELS))    #7
JOINT_PARENTS.append(findJointID("MidHip",JOINT_LABELS))    #8
JOINT_PARENTS.append(findJointID("MidHip",JOINT_LABELS))    #9
JOINT_PARENTS.append(findJointID("RHip",JOINT_LABELS))      #10
JOINT_PARENTS.append(findJointID("RKnee",JOINT_LABELS))     #11
JOINT_PARENTS.append(findJointID("MidHip",JOINT_LABELS))    #12
JOINT_PARENTS.append(findJointID("LHip",JOINT_LABELS))      #13
JOINT_PARENTS.append(findJointID("LKnee",JOINT_LABELS))     #14
JOINT_PARENTS.append(findJointID("Nose",JOINT_LABELS))      #15
JOINT_PARENTS.append(findJointID("Nose",JOINT_LABELS))      #16
JOINT_PARENTS.append(findJointID("Nose",JOINT_LABELS))      #17
JOINT_PARENTS.append(findJointID("Nose",JOINT_LABELS))      #18 
JOINT_PARENTS.append(findJointID("LAnkle",JOINT_LABELS))    #19
JOINT_PARENTS.append(findJointID("LAnkle",JOINT_LABELS))    #20
JOINT_PARENTS.append(findJointID("LAnkle",JOINT_LABELS))    #21
JOINT_PARENTS.append(findJointID("RAnkle",JOINT_LABELS))    #22
JOINT_PARENTS.append(findJointID("RAnkle",JOINT_LABELS))    #23
JOINT_PARENTS.append(findJointID("RAnkle",JOINT_LABELS))    #24


#These are our labels that based on JOINT_PARENTS auto-create the appropriate labels
#---------------------------------------------------------------------
JOINT_PARENT_LABELS=list()
for i in range(0,len(JOINT_LABELS)):
   JOINT_PARENT_LABELS.append(JOINT_LABELS[JOINT_PARENTS[i]]) 
#---------------------------------------------------------------------


def checkIfFileExists(filename):
    return os.path.isfile(filename) 

def convert_bytes(num):
    """
    this function will convert bytes to MB.... GB... etc
    """
    step_unit = 1000.0 #1024 bad the size

    for x in ['bytes', 'KB', 'MB', 'GB', 'TB']:
        if num < step_unit:
            return "%3.1f %s" % (num, x)
        num /= step_unit

def getNumberOfLines(filename):
    print("Counting number of lines in file ",filename)
    with open(filename) as f:
        return sum(1 for line in f) 

#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
def readCSVFile(filename,memPercentage,csvDelimiter,useHalfFloats,groupOutput):
    print("CSV file :",filename,"..\n")

    if (not checkIfFileExists(filename)):
          print( bcolors.WARNING + "Input file "+filename+" does not exist, cannot read ground truth.." +  bcolors.ENDC)
          print("Current Directory was "+os.getcwd())
          sys.exit(0) 
    start = time.time()
     
    dtypeSelected=np.dtype(np.float32)  
    dtypeSelectedByteSize=int(dtypeSelected.itemsize) 
    if (useHalfFloats):
       dtypeSelected=np.dtype(np.float16) 
       dtypeSelectedByteSize=int(dtypeSelected.itemsize)

    progress=0.0
    sampleNumber=0
    receivedHeader=0
    inputNumberOfColumns=0

    inputLabels=list() 

    #-------------------------------------------------------------------------------------------------
    numberOfSamplesInput=getNumberOfLines(filename)-2 
    print(" Input file has ",numberOfSamplesInput," training samples\n")
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

 
    thisInput = array.array('f')  
    #---------------------------------  

    fi = open(filename, "r") 
    readerIn  = csv.reader( fi , delimiter=csvDelimiter, skipinitialspace=True) 
    for rowIn in readerIn: 
        #------------------------------------------------------
        if (not receivedHeader): #use header to get labels
           #------------------------------------------------------
           inputNumberOfColumns=len(rowIn)
           
           #Make sure CSV files that end with delimiter are correctly handled..
           if (inputNumberOfColumns>0):  
             if (rowIn[inputNumberOfColumns-1]==''): 
                inputNumberOfColumns=inputNumberOfColumns-1
           #-----------------------------------------------------  
           inputLabels = list(rowIn[i] for i in range(0,inputNumberOfColumns) )
           print("Number of Input elements : ",len(inputLabels))
           #------------------------------------------------------

           if (memPercentage==0):
               print("Will only return labels\n")
               return {'labels':inputLabels};


           #i=0
           #print("class Input(Enum):")
           #for label in inputLabels:
           #   print("     ",label," = ",i," #",int(i/3))
           #   print("     ",label,"=",int(i/3))
           #   i=i+1

           #---------------------------------  
           #         Allocate Lists 
           #---------------------------------
           for i in range(inputNumberOfColumns):
               thisInput.append(0.0)
           #---------------------------------  


           #---------------------------------  
           #      Allocate Numpy Arrays 
           #---------------------------------  
           inputSize=0
           startCompressed=0

           inputSize=inputSize+inputNumberOfColumns
           startCompressed=inputNumberOfColumns  
           
           npInputBytesize=0+numberOfSamplesLimit * inputSize * dtypeSelectedByteSize
           print(" Input file on disk has a shape of [",numberOfSamples,",",inputSize,"]")  
           print(" Input we will read has a shape of [",numberOfSamplesLimit,",",inputSize,"]")  
           print(" Input will occupy ",convert_bytes(npInputBytesize)," of RAM\n")  
           npInput = np.full([numberOfSamplesLimit,inputSize],fill_value=0,dtype=dtypeSelected,order='C')
           #----------------------------------------------------------------------------------------------------------
           receivedHeader=1 
           #sys.exit(0)
        else:
           #-------------------------------------------
           #  First convert our string INPUT to floats   
           #-------------------------------------------
           for i in range(inputNumberOfColumns):
                  thisInput[i]=float(rowIn[i])
           #------------------------------------------- 
           for num in range(0,inputNumberOfColumns):
                  npInput[sampleNumber,num]=float(thisInput[num]); 
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
    print("Size Occupied by data = ",totalNumberOfGigaBytes," GB \n")

    end = time.time()
    print("Time elapsed : ",(end-start)/60," mins")
    #---------------------------------------------------------------------

    if (groupOutput==0):
      #New better dictionary 
      output = dict() 
      for i in range(0,len(inputLabels)):
        lowerCaseName = inputLabels[i].lower()  
        #print("Joint ",lowerCaseName)
        output[lowerCaseName]=list()
        for frameID in range(0,len(npInput)):
          output[lowerCaseName].append(npInput[frameID][i]) 
      return output
    else:    
      #This is the old dictionary way (better for tensorflow training)
      return {'label':inputLabels, 'body':npInput };



#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------

#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------


def parseConfiguration(filename):
    import json
    configuration = [] 
    with open(filename) as f:
              s = f.read()
              s = s.replace('\t','')
              s = s.replace('\n','')
              s = s.replace(',}','}')
              s = s.replace(',]',']') 
              configuration = json.loads(s)
       
    #Resolve joint order for NSDM matrix..    
    jointID=0 
    
    global ROOT_JOINT
    ROOT_JOINT = "%s" % (configuration['RootJoint'].lower())
    print("Root joint is ",ROOT_JOINT)

    #Resolve joint order for NSDM Scaling ..        
    #---------------------------------------------------------------------------------------------------------------------------
    global JOINTS_TO_COMPARE
    JOINTS_TO_COMPARE.clear()
    for z in range(0,len(configuration['JointsToCompare'])):
                name=configuration['JointsToCompare'][z]['name']
                name=name.strip()
                JOINTS_TO_COMPARE.append(name) 
    #---------------------------------------------------------------------------------------------------------------------------   


    #---------------------------------------------------------------------------------------------------------------------------
    global JOINT_LABELS
    global JOINT_PARENTS
    JOINT_LABELS.clear()
    JOINT_PARENTS.clear()
    for z in range(0,len(configuration['JointHierarchy'])):
                name=configuration['JointHierarchy'][z]['jointName']
                name=name.strip()
                JOINT_LABELS.append(name) 

    for z in range(0,len(configuration['JointHierarchy'])):
                name=configuration['JointHierarchy'][z]['jointParent']
                name=name.strip()
                configuration['JointHierarchy'][z]['jointParentID'] = findJointID(name,JOINT_LABELS)
                JOINT_PARENTS.append(configuration['JointHierarchy'][z]['jointParentID'])
    #---------------------------------------------------------------------------------------------------------------------------   
 
    global JOINT_PARENT_LABELS
    JOINT_PARENT_LABELS.clear()
    for i in range(0,len(JOINT_LABELS)):
        name=JOINT_LABELS[JOINT_PARENTS[i]]
        name=name.strip()
        JOINT_PARENT_LABELS.append(name) 

    print("Configuration : ")
    print(configuration)
    return configuration



#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------

def swapJoints(theList,jointA,jointB):
 #======================
 jointAX="3dx_%s"%jointA
 jointAY="3dy_%s"%jointA
 jointAZ="3dz_%s"%jointA
 #======================
 jointBX="3dx_%s"%jointB
 jointBY="3dy_%s"%jointB
 jointBZ="3dz_%s"%jointB
 #======================

 print(bcolors.WARNING,"Attempting to swap ",jointA," with ",jointB,bcolors.ENDC)
 if ( jointAX in theList ) and ( jointBX in theList ) :
    print("Both joints exist..!")
    jAX=theList.pop(jointAX)
    jAY=theList.pop(jointAY)
    jAZ=theList.pop(jointAZ)
    #-----------------------
    jBX=theList.pop(jointBX)
    jBY=theList.pop(jointBY)
    jBZ=theList.pop(jointBZ)
    #-----------------------
    theList[jointBX]=jAX
    theList[jointBY]=jAY
    theList[jointBZ]=jAZ
    theList[jointAX]=jBX
    theList[jointAY]=jBY
    theList[jointAZ]=jBZ 
 elif ( jointAX in theList ):
    print("only joint ",jointAX," exists..!")
    jAX=theList.pop(jointAX)
    jAY=theList.pop(jointAY)
    jAZ=theList.pop(jointAZ)
    #-----------------------
    theList[jointBX]=jAX
    theList[jointBY]=jAY
    theList[jointBZ]=jAZ
 elif ( jointBX in theList ):
    print("only joint ",jointBX," exists..!")
    jBX=theList.pop(jointBX)
    jBY=theList.pop(jointBY)
    jBZ=theList.pop(jointBZ)
    #-----------------------
    theList[jointAX]=jBX
    theList[jointAY]=jBY
    theList[jointAZ]=jBZ 
 else:
    print(bcolors.FAIL,"Neither ",jointA," or ",jointB," exists..!",bcolors.ENDC)
 return theList


def printJointDistances(data,numberOfJointsToCompare,scaleX,scaleY,scaleZ):
 #All this code just to print out the distance of joints from ground truth..
 frameID=0
 for jointIDUnresolved in range(0,numberOfJointsToCompare):
   #------------------------------------------------------------------
   jointID = findJointID(JOINTS_TO_COMPARE[jointIDUnresolved],JOINT_LABELS) 
   if (jointID==-1):
         print(bcolors.WARNING,"Failed to resolve joint: ",JOINTS_TO_COMPARE[jointIDUnresolved],bcolors.ENDC)
         sys.exit(0)
   #------------------------------------------------------------------
   jointKey = JOINT_LABELS[jointID].lower() 
   parentKey = JOINT_PARENT_LABELS[jointID].lower() 
   pX = scaleX*data['3dx_%s' % parentKey][frameID]
   pY = scaleY*data['3dy_%s' % parentKey][frameID]
   pZ = scaleZ*data['3dz_%s' % parentKey][frameID]
   x = scaleX*data['3dx_%s' % jointKey][frameID]
   y = scaleY*data['3dy_%s' % jointKey][frameID]
   z = scaleZ*data['3dz_%s' % jointKey][frameID]
   dist =get3DDistance(
                       x,y,z,
                       pX,pY,pZ
                      )
   print(parentKey,"->",jointKey,"=",dist)


def AUC(values,minValue,maxValue):
 underCurve=0
 samples=len(values)
 for value in values: 
   if ((minValue<=value) and (value<=maxValue) ):
    underCurve=underCurve+1 
 return 100*(underCurve/samples)


def printJointNeededImprovements(dataA,scaleAX,scaleAY,scaleAZ,dataB,scaleBX,scaleBY,scaleBZ,numberOfJointsToCompare):
 #All this code just to print out the distance of joints from ground truth..
 frameID=0
 numberOfFrames = len(out['3dx_%s'%(JOINTS_TO_COMPARE[0].lower())])
 JOINT_DISTANCE_A=list()
 JOINT_DISTANCE_B=list()
 for jointIDUnresolved in range(0,numberOfJointsToCompare):
    JOINT_DISTANCE_A.append(0.0)
    JOINT_DISTANCE_B.append(0.0)

 for frameID in range(0,numberOfFrames):
  for jointIDUnresolved in range(0,numberOfJointsToCompare):
   #------------------------------------------------------------------
   jointID = findJointID(JOINTS_TO_COMPARE[jointIDUnresolved],JOINT_LABELS) 
   if (jointID==-1):
         print(bcolors.WARNING,"Failed to resolve joint: ",JOINTS_TO_COMPARE[jointIDUnresolved],bcolors.ENDC)
         sys.exit(0)
   #------------------------------------------------------------------
   jointKey = JOINT_LABELS[jointID].lower() 
   parentKey = JOINT_PARENT_LABELS[jointID].lower() 
   pX = scaleAX*dataA['3dx_%s' % parentKey][frameID]
   pY = scaleAY*dataA['3dy_%s' % parentKey][frameID]
   pZ = scaleAZ*dataA['3dz_%s' % parentKey][frameID]
   x = scaleAX*dataA['3dx_%s' % jointKey][frameID]
   y = scaleAY*dataA['3dy_%s' % jointKey][frameID]
   z = scaleAZ*dataA['3dz_%s' % jointKey][frameID]
   distA = get3DDistance(
                         x,y,z,
                         pX,pY,pZ
                        )
   JOINT_DISTANCE_A[jointIDUnresolved]+=distA
 
   pX = scaleBX*dataB['3dx_%s' % parentKey][frameID]
   pY = scaleBY*dataB['3dy_%s' % parentKey][frameID]
   pZ = scaleBZ*dataB['3dz_%s' % parentKey][frameID]
   x = scaleBX*dataB['3dx_%s' % jointKey][frameID]
   y = scaleBY*dataB['3dy_%s' % jointKey][frameID]
   z = scaleBZ*dataB['3dz_%s' % jointKey][frameID]
   distB = get3DDistance(
                         x,y,z,
                         pX,pY,pZ
                        )
   JOINT_DISTANCE_B[jointIDUnresolved]+=distB

 for jointIDUnresolved in range(0,numberOfJointsToCompare):
   #------------------------------------------------------------------
   jointID = findJointID(JOINTS_TO_COMPARE[jointIDUnresolved],JOINT_LABELS) 
   if (jointID==-1):
         print(bcolors.WARNING,"Failed to resolve joint: ",JOINTS_TO_COMPARE[jointIDUnresolved],bcolors.ENDC)
         sys.exit(0)
   #------------------------------------------------------------------
   jointKey = JOINT_LABELS[jointID].lower() 
   parentKey = JOINT_PARENT_LABELS[jointID].lower() 
   #------------------------------------------------------------------
   distanceAAverage = JOINT_DISTANCE_A[jointIDUnresolved]/numberOfFrames
   distanceBAverage = JOINT_DISTANCE_B[jointIDUnresolved]/numberOfFrames
   change = (distanceAAverage)-(distanceBAverage)
   if (change>0.01): 
       print(bcolors.FAIL,parentKey,"->",jointKey," needs to be increased ( change ",round(change,2)," mm ) ",bcolors.ENDC)
   elif (change<-0.01): 
       print(bcolors.OKBLUE,parentKey,"->",jointKey," needs to be decreased ( change ",round(change,2)," mm ) ",bcolors.ENDC)
   else: 
       print(bcolors.OKGREEN,parentKey,"->",jointKey," is ok ( finetune ",round(change,4)," mm )  ",bcolors.ENDC)


#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------------------------------



#Main() program
#-------------------------------------
averageErrorDistances=list()
averageMotionEstimationDistancesBetweenFrames=list()


#These are labeling infos
#Used to correctly pack the results
#-------------------------
subject="S?"
action="Unknown"
subaction="?"
camera="?"
actionLabel="?"
addedPixelNoise=0

#These are configuration parameters
#-------------------------------------------------------------------
everyNFrames=0    # Run on every frame of the CSV files by default
drawPlot=0        # Don't Draw plot by default
doProcrustes=1    # Do procrustes analysis 
procrustesScale=1 # Let procrustes also scale the point clouds 
ground="h36.csv"  # A default h36.csv filename 
groundDelim=','   # By default the h36.csv has a ',' delimiter
output="out.csv"  # A default output filename
outputDelim=';'   # Damien's output has ';' delimiters, mine has ','
#Multipliers of coordinates to set the correct units everywhere
#----------------
ourScaleX=1.0
ourScaleY=1.0
ourScaleZ=1.0
#----------------
groundScaleX=1.0
groundScaleY=1.0
groundScaleZ=1.0
#----------------
outputScale=1.0
#----------------
aucMin=0
aucMax=0

simulateAlignedJoint=0

#Modifiers on comparisons..
#------------------------------------------------------------------------------------------------ 
if (len(sys.argv)>1):
   #print('Argument List:', str(sys.argv))
   for i in range(0, len(sys.argv)):
       if (sys.argv[i]=="--simulateAlignedJoint"): 
          print(bcolors.WARNING,"A Joint virtual aligned joint will be added to comparison",bcolors.ENDC)
          simulateAlignedJoint=1
       if (sys.argv[i]=="--ban"): 
          print(bcolors.WARNING,"Joint ",sys.argv[i+1]," is banned from comparison",bcolors.ENDC)
          JOINTS_TO_COMPARE.remove(sys.argv[i+1]) 
       if (sys.argv[i]=="--noscale"): 
          procrustesScale=0
       if (sys.argv[i]=="--auc"): 
          aucMin=float(sys.argv[i+1])
          aucMax=float(sys.argv[i+2])
       if (sys.argv[i]=="--config"): 
          parseConfiguration(sys.argv[i+1])
       if (sys.argv[i]=="--outputscale"):  
          outputScale=float(sys.argv[i+1])
       if (sys.argv[i]=="--ourscale"):  
          ourScaleX=float(sys.argv[i+1])
          ourScaleY=float(sys.argv[i+2])
          ourScaleZ=float(sys.argv[i+3])
       if (sys.argv[i]=="--groundscale"):  
          groundScaleX=float(sys.argv[i+1])
          groundScaleY=float(sys.argv[i+2])
          groundScaleZ=float(sys.argv[i+3])
       if (sys.argv[i]=="--draw"): 
          drawPlot=1
       if (sys.argv[i]=="--from"):  
          output=sys.argv[i+1]
          outputDelim=sys.argv[i+2]
       if (sys.argv[i]=="--ground"): 
          ground=sys.argv[i+1]
          groundDelim=sys.argv[i+2]
       if (sys.argv[i]=="--noprocrustes"): 
          doProcrustes=0
       if (sys.argv[i]=="--every"): 
          everyNFrames=int(sys.argv[i+1])
       if (sys.argv[i]=="--info"):
          subject=sys.argv[i+1]
          action=sys.argv[i+2]
          camera=sys.argv[i+3]
          addedPixelNoise=float(sys.argv[i+4])
          print("\n Infos Set ",sys.argv[i+1])
          print("\n actionLabel ",action)
          actionLabel=action
          s=actionLabel.split("-")
          subaction=s[1]
          action=s[0]
          action=action.replace('/','')
          subaction=subaction.replace('/','')
          actionLabel=actionLabel.replace('/','')
          camera=camera.replace('/','')

          print("Subject ",subject," Action ",action," Subaction ",subaction," Camera ",camera)
#------------------------------------------------------------------------------------------------ 
   
if (drawPlot):
   print("Using matplotlib:",matplotlib.__version__)
   # === Plot and animate === 
   fig = plt.figure()
   fig.set_size_inches(19.2, 10.8, forward=True) 
 
   ax = fig.add_subplot(2, 2, 1, projection='3d') 
   ax2 = fig.add_subplot(2, 2, 2) 
   ax3 = fig.add_subplot(2, 2, 3) 
   ax4 = fig.add_subplot(2, 2, 4) 
   fig.subplots_adjust(left=0.05, bottom=0.05, right=0.95, top=0.95) 
 
   ax.set_xlabel('X Axis')
   ax.set_ylabel('Y Axis')
   ax.set_zlabel('Z Axis') 
   ax.view_init(90, 90) 

print("Joint To Compare : ")
print(JOINTS_TO_COMPARE)
 
print("Joint Labels : ")
print(JOINT_LABELS)

print("Joint Parents : ")
print(JOINT_PARENTS)

for z in range(0,len(JOINT_PARENTS)):
     print(JOINT_LABELS[JOINT_PARENTS[z]])


out=readCSVFile(output,1.0,outputDelim,0,0)
ground=readCSVFile(ground,1.0,groundDelim,0,0)



#------------------------------------------------------------------------------------------------ 
if (len(sys.argv)>1):
   #print('Argument List:', str(sys.argv))
   for i in range(0, len(sys.argv)):
       if (sys.argv[i]=="--swapOurJoint"): 
          print(bcolors.WARNING,"Joint ",sys.argv[i+1]," will be swapped with ",sys.argv[i+2],bcolors.ENDC)
          out=swapJoints(out,sys.argv[i+1].lower(),sys.argv[i+2].lower())
#------------------------------------------------------------------------------------------------ 


print("Ground keys : ",ground.keys())

print("Output keys : ",out.keys())

print("Output number of keys : %u " % len(out.keys()) )
print("GroundTruth number of keys : %u " % len(ground.keys()) )


print("Our Scale (XYZ) => (%0.2f,%0.2f,%0.2f)",ourScaleX,ourScaleY,ourScaleZ)
print("Ground Scale (XYZ) => (%0.2f,%0.2f,%0.2f)",groundScaleX,groundScaleY,groundScaleZ)

if ( len(out.keys()) != len(ground.keys()) ):
    print(bcolors.WARNING,"Inconsistent keys",bcolors.ENDC)
    #sys.exit(0)


totalError=0.0
totalSamples=0
numberOfJointsToCompare = len(JOINTS_TO_COMPARE)
numberOfFrames = len(out['3dx_%s'%(JOINTS_TO_COMPARE[0].lower())])


#-----------------------
alljointDistances=list()
#-----------------------
jointDistance=list()
for jointID in range(0,numberOfJointsToCompare):
    jointDistance.append(list())
#-----------------------


#------------------------------------------------------------------------------------------------ 
if (len(sys.argv)>1):
   #print('Argument List:', str(sys.argv))
   for i in range(0, len(sys.argv)):
       if (sys.argv[i]=="--frames"):  
          numberOfFrames=int(sys.argv[i+1])
#------------------------------------------------------------------------------------------------ 



####
print("Ground Truth Initial Dimensions")
print("___________________________________________")
printJointDistances(ground,numberOfJointsToCompare,groundScaleX,groundScaleY,groundScaleZ)
print("___________________________________________")

print("Our Data Initial  Dimensions")
print("___________________________________________")
printJointDistances(out,numberOfJointsToCompare,ourScaleX,ourScaleY,ourScaleZ)
print("___________________________________________")


print("Needed changes")
#if (procrustesScale):
#  print(bcolors.WARNING,"Run with --noscale to make sure the measurments here are ok!",bcolors.ENDC)
print("___________________________________________")
printJointNeededImprovements(ground,groundScaleX,groundScaleY,groundScaleZ,out,ourScaleX,ourScaleY,ourScaleZ,numberOfJointsToCompare)
print("___________________________________________")


#We want to loop over every frame that we loaded..
for frameID in range(0,numberOfFrames):
 #In many papers they run the comparison ever N frames
 #So depending on this setting you can actually execute 
 #the comparison computation everyNFrames
 #-----------------------------------------------------
 if ( (everyNFrames==0) or (frameID%everyNFrames==0) ):
   executeComputations=1
 else:
   executeComputations=0
 #-----------------------------------------------------

 if (executeComputations==1):
   ourPointCloud=list()
   h36PointCloud=list()
   xlineStart=list()
   xlineEnd=list()
   ylineStart=list()
   ylineEnd=list()
   zlineStart=list()
   zlineEnd=list()
 
   #Align midhip to make sure we start from a sane position
   offsetX=(ourScaleX*out['3dx_%s'%(ROOT_JOINT)][frameID])-(groundScaleX*ground['3dx_%s'%(ROOT_JOINT)][frameID])
   offsetY=(ourScaleY*out['3dy_%s'%(ROOT_JOINT)][frameID])-(groundScaleY*ground['3dy_%s'%(ROOT_JOINT)][frameID])
   offsetZ=(ourScaleZ*out['3dz_%s'%(ROOT_JOINT)][frameID])-(groundScaleZ*ground['3dz_%s'%(ROOT_JOINT)][frameID])

   for jointIDUnresolved in range(0,numberOfJointsToCompare):
   #------------------------------------------------------------------
     #Our Joints to compare list has a different indexing 
     jointID = findJointID(JOINTS_TO_COMPARE[jointIDUnresolved],JOINT_LABELS) 
     if (jointID==-1):
         print(bcolors.WARNING,"Failed to resolve joint: ",JOINTS_TO_COMPARE[jointIDUnresolved],bcolors.ENDC)
         sys.exit(0)
     #----------------------------------------------------------- 
     jointKey = JOINT_LABELS[jointID].lower() 
     parentKey = JOINT_PARENT_LABELS[jointID].lower() 
     #print("Frame %u / Key %s ( prev %s ) "% (frameID,jointKey,parentKey) )

     #------------------------------------------------------------------ 
     #We select the joint and push it to the point cloud for this particular frame    
     xM=(ourScaleX*out['3dx_%s' % jointKey][frameID])-offsetX
     yM=(ourScaleY*out['3dy_%s' % jointKey][frameID])-offsetY
     zM=(ourScaleZ*out['3dz_%s' % jointKey][frameID])-offsetZ
     ourPointCloud.append([xM,yM,zM])

     #These lineStarts/lineEnds and parentKeys help draw lines connecting
     #the dots in skeletons
     xParentM=(ourScaleX*out['3dx_%s' % parentKey][frameID])-offsetX
     yParentM=(ourScaleY*out['3dy_%s' % parentKey][frameID])-offsetY
     zParentM=(ourScaleZ*out['3dz_%s' % parentKey][frameID])-offsetZ
     xlineStart.append(xM) 
     ylineStart.append(yM) 
     zlineStart.append(zM) 
     xlineEnd.append(xParentM) 
     ylineEnd.append(yParentM) 
     zlineEnd.append(zParentM)
     #------------------------------------------------------------------ 
     #We select the h36 joint and push it to the point cloud for this particular frame    
     xH=groundScaleX*ground['3dx_%s' % jointKey][frameID]
     yH=groundScaleY*ground['3dy_%s' % jointKey][frameID]
     zH=groundScaleZ*ground['3dz_%s' % jointKey][frameID]
     h36PointCloud.append([xH,yH,zH])

     #These lineStarts/lineEnds and parentKeys help draw lines connecting
     #the dots in skeletons
     xParentH=groundScaleX*ground['3dx_%s' % parentKey][frameID]
     yParentH=groundScaleY*ground['3dy_%s' % parentKey][frameID]
     zParentH=groundScaleZ*ground['3dz_%s' % parentKey][frameID]
     xlineStart.append(xH) 
     ylineStart.append(yH) 
     zlineStart.append(zH) 
     xlineEnd.append(xParentH) 
     ylineEnd.append(yParentH) 
     zlineEnd.append(zParentH)
     #------------------------------------------------------------------ 

     #------------------------------------------------------------------ 
   
   #We package our lists in numpy to be able to easily manipulate them 
   #------------------------------------------------------------------ 
   np_h36PointCloud =  np.asarray(h36PointCloud,dtype=np.float32)
   np_ourPointCloud =  np.asarray(ourPointCloud,dtype=np.float32)
 
   
   #This is the main comparison after using procrustes and transforming the pointcloud
   #to align it or when just doing plain old euclidean distance
   #--------------------------------------------------------------------------------
   if (doProcrustes):
       d, Z, T, b, c = compute_similarity_transform(np_h36PointCloud,np_ourPointCloud,compute_optimal_scale=procrustesScale)
       #disparity=np.sqrt(d)  #d: squared error after transformation
       #print("compute_similarity_transform : ",disparity)

       #Our point cloud is brought to the same translation and rotation as h36 point cloud
       np_ourPointCloud = (b*np_ourPointCloud.dot(T))+c
       disparity = outputScale * pointListsReturnAvgDistance(np_ourPointCloud,np_h36PointCloud)
   else:
       disparity = outputScale * pointListsReturnAvgDistance(np_ourPointCloud,np_h36PointCloud)
   #-------------------------------------------------------------------------------- 
   
   #We want to calculate Mean Per Joint Position Error (MPJPE)
   #to do so we have to calculate the position error of each of the joints in our point cloud
   #sum it up and then divide it through the number of samples
   for jID in range(0,numberOfJointsToCompare):
   #------------------------------------------------------------------ 
     #We use the np_ourPointCloud and np_h36PointCloud so that if procrustes analysis is enabled it will be used..
     perJointDisparity= outputScale * get3DDistance(
                                                    np_ourPointCloud[jID][0],np_ourPointCloud[jID][1],np_ourPointCloud[jID][2],
                                                    np_h36PointCloud[jID][0],np_h36PointCloud[jID][1],np_h36PointCloud[jID][2]
                                                   )
     totalError+=perJointDisparity
     totalSamples+=1
     #We also keep every sample on a list to do an analysis in the end
     alljointDistances.append(perJointDisparity)
     jointDistance[jID].append(perJointDisparity)
   #------------------------------------------------------------------ 
   
   if (simulateAlignedJoint):
     totalError+=0
     totalSamples+=1
     alljointDistances.append(0)
     jointDistance[0].append(perJointDisparity)
   #---------------------------

   print("Frame %u / Disparity %f " % (frameID,disparity))
   averageErrorDistances.append(disparity)

   #Draw the plot that summarizes what is happening if --draw is given
   if (drawPlot):
      drawAfterEndOfComparison(
                               JOINTS_TO_COMPARE,
                               JOINT_LABELS,
                               JOINT_PARENTS,
                               JOINT_PARENT_LABELS, 
                               plt,ax,ax2,ax3,ax4,
                               xlineStart,xlineEnd,
                               ylineStart,ylineEnd,
                               zlineStart,zlineEnd,
                               np_ourPointCloud,
                               np_h36PointCloud,
                               frameID,
                               numberOfFrames,
                               everyNFrames,
                               disparity, 
                               averageErrorDistances,
                               averageMotionEstimationDistancesBetweenFrames 
                              )

#Since this always appends results be sure to remove the gnuplot.raw
#file before starting a new session..
appendRAWResultsForGNUplot("gnuplot.raw",alljointDistances)

#Final results we want is Mean Per Joint Position Error (MPJPE) so we rely on the perJointDisparities etc..
#The result unit depends on the input to the script but it should be in millimeters for MocapNET output
#to change units you can use the --ourscale X Y Z commandline parameter, however millimeters of accuracy
#is the norm for 3D pose estimation work..

print("\nMean Per Joint Error for ",totalSamples," samples is ",totalError/totalSamples)
median  =  np.median(alljointDistances)
mean    =  np.mean(alljointDistances)
average =  np.average(alljointDistances)
std     =  np.std(alljointDistances)
var     =  np.var(alljointDistances)
print("Mean is ",mean," Std is ",std," Var is ",var)

if (aucMax>0.0):
 print("AUC (",aucMin,",",aucMax,") is ",AUC(alljointDistances,aucMin,aucMax)," % ")

if (simulateAlignedJoint):
          print(bcolors.FAIL,"DONT USE RESULTS, THEY CONTAIN A SIMULATED ALIGNED JOINT",bcolors.ENDC)

writeHeader=0
if not os.path.exists("results.csv"):
   writeHeader=1
writeCSVFileResults(
                    JOINTS_TO_COMPARE,
                    "results.csv",
                    writeHeader,
                    alljointDistances,
                    jointDistance,
                    numberOfJointsToCompare,
                    subject,
                    action,
                    subaction,
                    camera,
                    actionLabel,
                    addedPixelNoise
                   )




if (drawPlot): 
  os.system("ffmpeg -framerate 25 -i p%05d.png  -s 1920x1080  -y -r 30 -pix_fmt yuv420p -threads 8  lastcomp.mp4 && rm ./p*.png") #
sys.exit(0)

