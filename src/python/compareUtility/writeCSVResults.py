#!/usr/bin/env python3
#Written by Ammar Qammaz a.k.a AmmarkoV - 2020

import numpy as np

#-----------------------------------------------------------------------------------------------------------------------
def writeCSVFileResults(
                        JOINT_LABELS,  
                        outputFile,
                        addHeader,
                        globalJointDistances,
                        perJointDistance,
                        numberOfJoints,
                        subject,
                        action,
                        subaction,
                        camera,
                        actionLabel,
                        addedPixelNoise
                       ):
  #Write header----------------
  if (addHeader):
      file = open(outputFile,"w")  
      file.write("Subject,")
      file.write("Action,")
      file.write("ActionLabel,")
      file.write("Subaction,")
      file.write("Camera,")
      file.write("Noise,")
      file.write("Global_Median,")
      #file.write("Global_Mean,")
      file.write("Global_Average,")
      file.write("Global_Std,")
      file.write("Global_Var,")
      for jointID in range(0,numberOfJoints):
            file.write("%s_Median,"%JOINT_LABELS[jointID])
            #file.write("%s_Mean,"%JOINT_LABELS[jointID])
            file.write("%s_Average,"%JOINT_LABELS[jointID])
            file.write("%s_Std,"%JOINT_LABELS[jointID])
            file.write("%s_Var"%JOINT_LABELS[jointID])
            if (jointID!=numberOfJoints-1):
             file.write(",")
            else:   
             file.write("\n")
  else:
      file = open(outputFile,"a")  
  #---------------------------- 

  median=np.median(globalJointDistances)
  mean=np.mean(globalJointDistances)
  average=np.average(globalJointDistances)
  std=np.std(globalJointDistances)
  var=np.var(globalJointDistances)
  print("\nGlobal Median:",median," Average:",average," Std:",std,"Var:",var)    #file.write(subject)
  file.write(subject)
  file.write(",")
  file.write(action)
  file.write(",")
  file.write(actionLabel)
  file.write(",")
  file.write(subaction)
  file.write(",")
  file.write(camera)  
  file.write(",")
  file.write(str(addedPixelNoise))  
  file.write(",")
  file.write(str(median))
  file.write(",")
  #file.write(str(mean))
  #file.write(",")
  file.write(str(average))
  file.write(",")
  file.write(str(std))
  file.write(",")
  file.write(str(var))
  file.write(",")
  
  for jointID in range(0,numberOfJoints):
        median=np.median(perJointDistance[jointID])
        mean=np.mean(perJointDistance[jointID])
        average=np.average(perJointDistance[jointID])
        std=np.std(perJointDistance[jointID])
        var=np.var(perJointDistance[jointID])
        print("Joint ",JOINT_LABELS[jointID]," Median:",median," Mean:",mean," Average:",average," Std:",std,"Var:",var)
        file.write(str(median))
        file.write(",")
        #file.write(str(mean))
        #file.write(",")
        file.write(str(average))
        file.write(",")
        file.write(str(std))
        file.write(",")
        file.write(str(var))   
        if (jointID!=numberOfJoints-1):
         file.write(",")
        else:   
         file.write("\n")

  file.close()
#--------------------------------------------



#-----------------------------------------------------------------------------------------------------------------------
def appendRAWResultsForGNUplot(outputFile,globalJointDistances):
    fileHandler = open(outputFile, "a")
    for measurement in globalJointDistances:
        fileHandler.write(str(measurement))
        fileHandler.write("\n")
    fileHandler.close()
