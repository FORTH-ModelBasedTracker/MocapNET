#!/usr/bin/python3

import ctypes
import os
import sys
from ctypes import *
from os.path import exists


#--------------------------------------------------------
def readCSV(filename):
  result=dict()
  import csv
  with open(filename,newline='') as csvfile:
        readerIn = csv.reader(csvfile,delimiter=',',quotechar='"')
        for rowIn in readerIn: 
           numberOfColumns=len(rowIn)       
           labels = list(rowIn[i] for i in range(0,numberOfColumns) )
           if (labels[0]!=''):
              newList = list()
              newList.append(labels[1])
              newList.append(labels[2])
              result[labels[0]]=newList
  return result
#--------------------------------------------------------
def gatherAllBVHFiles(directoryPath):
   results = list()
   for f in os.scandir(directoryPath):
       if f.is_file():
          #print("f.path=",f.path)
          if (f.path.find(".bvh")!=-1):
              #print("Adding BVH file ",f.path)
              results.append(f.path)
   return results;
#--------------------------------------------------------
def gatherAllBVHDirectories(directoryPath):
   results = list()
   for f in os.scandir(directoryPath):
       if f.is_dir():
          #print("Adding files in ",f.path)
          results = results + gatherAllBVHFiles(f.path)
   return results
#--------------------------------------------------------
def writeListToFile(theList,theFilename):
   file = open(theFilename, "w")
   for theList in allBVHFiles:
      file.write(theList + "\n")
   file.close()
#--------------------------------------------------------
def appendJSONEnding(theFilename):
   file = open(theFilename, "a")  # append mode
   file.write("  ]\n  } \n")
   file.close()
#--------------------------------------------------------



def loadLibrary(filename):
 if not exists(filename):
     print("Could not find BVH Library (",filename,"), compiling a fresh one..!")
     print("Current directory was (",os.getcwd(),") ")
     directory=os.path.dirname(os.path.abspath(filename))
     os.system(directory+"/makeLibrary.sh")
 if not exists(filename):
     print("Could not make BVH Library, terminating")
     sys.exit(0)
 libBVH = CDLL(filename)
 #call C function to check connection
 libBVH.connect() 
 libBVH.bvhConverter.restype = c_int
 libBVH.bvhConverter.argtypes = c_int,POINTER(c_char_p)
 return libBVH
#--------------------------------------------------------
def bvhConvert(libBVH,arguments):
    argumentBytes = []
    for i in range(len(arguments)):
        argumentBytes.append(bytes(arguments[i], 'utf-8'))
    argv = (ctypes.c_char_p * len(argumentBytes))()
    argv[:] = argumentBytes 
    argc=len(argumentBytes)
    libBVH.bvhConverter(argc,argv)
#--------------------------------------------------------











if __name__== "__main__":
 #python main : 
 pythonFlags=list()
 #Add any arguments given in the python script directly!
 if (len(sys.argv)>1):
   print('Supplied argument List:', str(sys.argv))
   for i in range(0, len(sys.argv)):
     pythonFlags.append(sys.argv[i])
     if (sys.argv[i]=="--update"): 
         print('Deleting previous libBVHConverter.so to force update!\n')
         os.system("rm libBVHConverter.so")




 libBVH = loadLibrary("./libBVHConverter.so")
 

 
 datasetDirectory = "/home/ammar/Documents/Programming/DNNTracker/DNNTracker/dataset/MotionCapture"
 outputDirectory = os.getcwd()
 allBVHFiles = gatherAllBVHDirectories(datasetDirectory) 
 #Keep list for debug.. 
 writeListToFile(allBVHFiles,"listOfBVHFiles.txt")
 print("Output Directory = ",outputDirectory) 
 print("Found ",len(allBVHFiles)," BVH files")


 #Package Annotations as JSON
 print("Packaging annotations in JSON format")
 annotations=readCSV(datasetDirectory+"/cmu-mocap-annotations.csv")
 import json
 with open("annotations.json", "w") as outfile:
    json.dump(annotations, outfile, indent=4)


 mode="json" # json or csv
 extension="."+mode
 bodyPart="upperbody"
 os.system("rm 2d_"+bodyPart+extension)
 os.system("rm 3d_"+bodyPart+extension)
 os.system("rm bvh_"+bodyPart+extension)


 minRotationLimit="-45"
 maxRotationLimit="45"
 minORIENTATION="-55" #-45  default , -55 with 10 deg safety  
 maxORIENTATION="55"  # 45  default , 55 with 10 deg safety
 minDepth="900"  #1000 original
 maxDepth="4500" #3000 is too small
 
 #HIDEBODY="--hide2DLocationOfJoints 0 8 abdomen chest eye.r eye.l toe1-2.r toe5-3.r toe1-2.l toe5-3.l" #We want  to contain these joints in the BVH file and solve them, but they do not
 #SELECTBODY="--selectJoints 1 23 hip eye.r eye.l abdomen chest neck head rshoulder relbow rhand lshoulder lelbow  lhand rhip rknee rfoot lhip lknee lfoot toe1-2.r toe5-3.r toe1-2.l toe5-3.l $HIDEBODY"
 #--randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $FRONT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $FRONT_MAX_ORIENTATION $MAX_LIM" "$SELECTLOWERBODY $RAND_LOWER_BODY

 #$MIN_DEPTH $MAX_DEPTH $MIN_LIM -179.999999 $MIN_LIM $MAX_LIM 180 $MAX_LIM

 numberOfFilesProcessed = 0
 for bvhFile in allBVHFiles:
    args=list()
    args.append("--printparams") 
    args.append("--haltonerror")
    args.append("--from")
    args.append(bvhFile)
 #RAND_UPPER_BODY="--perturbJointAngles 2 30.0 rshoulder lshoulder --perturbJointAngles 2 16.0 relbow lelbow --perturbJointAngles 2 10.0 abdomen chest"
 #RAND_LOWER_BODY="--perturbJointAngles 2 30.0 rhip lhip --perturbJointAngles 4 10.0 lknee rknee lfoot rfoot --perturbJointAngles 2 10.0 abdomen chest"
    args.append("--filtergimballocks"); args.append("4")
    #startingFlags.append("--repeat")          ;  startingFlags.append("1")

    #Hip Position/Rotation Randomization
    args.append("--randomize2D")
    args.append(minDepth)  #Minimum distance from the camera
    args.append(maxDepth)  #Maximum distance from the camera
    args.append(minRotationLimit)  #
    args.append("-179.99")
    args.append(minRotationLimit)
    args.append(maxRotationLimit)
    args.append("179.99")
    args.append(maxRotationLimit)
    #-------------------------------------
 

    #Upper body joints
    #-------------------------------------
    args.append("--selectJoints") 
    args.append("1")
    args.append("23") #Number of joints to select
    args.append("hip")
    args.append("eye.r")
    args.append("eye.l")
    args.append("abdomen")
    args.append("chest")
    args.append("neck")
    args.append("head")
    args.append("rshoulder")
    args.append("relbow")
    args.append("rhand")
    args.append("lshoulder")
    args.append("lelbow")
    args.append("lhand")
    args.append("rhip")
    args.append("rknee")
    args.append("rfoot")
    args.append("lhip")
    args.append("lknee")
    args.append("lfoot")
    args.append("toe1-2.r")
    args.append("toe5-3.r")
    args.append("toe1-2.l")
    args.append("toe5-3.l")
    #-------------------------------------

    #Deactivate spare joints
    #-------------------------------------
    args.append("--hide2DLocationOfJoints")
    args.append("0")
    args.append("8") #Number of joints to hide
    args.append("abdomen")
    args.append("chest")
    args.append("eye.r")
    args.append("eye.l")
    args.append("toe1-2.r")
    args.append("toe5-3.r")
    args.append("toe1-2.l")
    args.append("toe5-3.l")
    #-------------------------------------


    args.append("--occlusions") 
    args.append("--"+mode) 
    args.append(outputDirectory) 
    args.append(bodyPart+extension) 
    args.append("2d+bvh")
    args = args + pythonFlags
    bvhConvert(libBVH,args)
    numberOfFilesProcessed = numberOfFilesProcessed + 1
    if (numberOfFilesProcessed==2): 
       print("Stopping now at ",numberOfFilesProcessed," limit")
       break

 appendJSONEnding("2d_"+bodyPart+extension)
 appendJSONEnding("3d_"+bodyPart+extension)
 appendJSONEnding("bvh_"+bodyPart+extension)

 #./GroundTruthDumper $VIEW_COMMANDS --haltonerror --from $BVHFILE --filtergimballocks 4 $3 --repeat $ITERATIONS $2 --occlusions --csv $outputDir $1 2d+bvh # --bvh $outputDir/$f-random.bvh
