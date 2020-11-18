#!/usr/bin/env python3

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

width=1920.0
height=1080.0


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def pointListReturnXYListForScatterPlot(A):
    numberOfPoints=A.shape[0] 
    xs=list()
    ys=list()
    frameNumber=A[0]
    skeletonID=A[1]
    totalSkeletons=A[2]
    for i in range(3,int(numberOfPoints/3)):
       x=width*float(A[i*3+0])
       y=height*float(A[i*3+1])
       visibility=int(A[i*3+2])
       if (visibility==1):
          xs.append(x)
          ys.append(y)
          print('Point(',x,',',y,')')
    return xs,ys

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



drawPlot=1

ground=readCSVFile("test2D.csv",1.0,',',0,1)


if (drawPlot):
   print("Using matplotlib:",matplotlib.__version__)
   # === Plot and animate === 
   fig = plt.figure()
   fig.set_size_inches(19.2, 10.8, forward=True) 
 
   ax = fig.add_subplot(1, 2, 1) 
   ax2 = fig.add_subplot(1, 2, 2) 
   #ax3 = fig.add_subplot(2, 2, 3) 
   #ax4 = fig.add_subplot(2, 2, 4) 
   fig.subplots_adjust(left=0.05, bottom=0.05, right=0.95, top=0.95) 
 
   ax.set_xlabel('X Axis')
   ax.set_ylabel('Y Axis')
   #ax.set_zlabel('Z Axis') 
   #ax.view_init(90, 90) 


print("Ground truth file has %u elements ",len(ground['body']))


for i in range(0,len(ground['body'])):
 if (drawPlot):
   plt.cla() 
   ax.cla()
   ax2.cla() 

   xs, ys = pointListReturnXYListForScatterPlot(ground['body'][i])
 
   ax.scatter(xs, ys)
   img = plt.imread("images/im%u.png" % i)
   ax2.imshow(img)

   #------------------------- 
   ax.set_xlim(auto=False,left=0,right=width)
   ax.set_ylim(auto=False,bottom=height,top=0)
   #ax.set_zlim(auto=False,bottom=2000,top=6000)
   ax.set_xlabel('X Axis')
   ax.set_ylabel('Y Axis')
   #ax.set_zlabel('Z Axis') 
   #------------------------- 
   plt.show(block=False) 
   #plt.savefig('p%05u.png'%i)
   #fig.canvas.draw() 
   plt.pause(0.102)
