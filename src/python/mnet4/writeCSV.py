#!/usr/bin/python3
import numpy as np 
import csv
import gc
import time
import array
import sys 


def writeCSVFileHeader(filenameOutput,inputListLabels,inputStart,inputEnd):
    inputNumber=0
    fileCSV = open(filenameOutput,"w") 
    for entry in inputListLabels[inputStart:inputEnd]:
        #if (inputNumber>=inputStart) and (inputNumber<inputEnd): 
           fileCSV.write(str(entry))
           fileCSV.write(",")
        #++inputNumber
    fileCSV.write("\n")
    fileCSV.close()   
 

def writeCSVFileBody(filenameOutput,inputList,inputStart,inputEnd):
    inputNumber=0
    fileCSV = open(filenameOutput,"a") 
    for entry in inputList[inputStart:inputEnd]:
        #if (inputNumber>=inputStart) and (inputNumber<inputEnd): 
           fileCSV.write(str(entry))
           fileCSV.write(",")
        #++inputNumber 
    fileCSV.write("\n")
    fileCSV.close()   

     
