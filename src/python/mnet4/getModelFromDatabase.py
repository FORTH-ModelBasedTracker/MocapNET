#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import os
import sys
import gc
import time
import numpy as np 
#--------------------------------------------------
from tools import bcolors,notification,checkIfPathExists,checkIfPathIsDirectory,checkIfFileExists,getRAMInformation

ONNX_OPSET=17 # Was 14

def downloadAndParseDatabase():
       file = "modelDatabase.json"
       if (checkIfFileExists(file)):
           print("Erasing previous database snapshot")
           os.system('rm "%s"' % (file))
       os.system('wget "http://ammar.gr/mocapnet/mnet4/database.php" && mv database.php "%s"' % file)
       import json
       db = []
       with open(file) as f:
              s = f.read()
              s = s.replace('\t','')
              s = s.replace('\n','')
              s = s.replace(',}','}')
              s = s.replace(',]',']') 
              db = json.loads(s)
       return db


def quickCopy(step0,step1):
    onnxModel = "%s/model.onnx" % step0
    tfliteModel = "%s/model.tflite" % step0
    if ( (not checkIfFileExists(onnxModel)) or (not checkIfFileExists(tfliteModel)) ):
        print("No quick copy capability (old MocapNET package)")
        return False
    
    os.system('rm -rf %s/' % step1)
    os.system('mkdir  %s/' % step1)
    os.system('cp  %s/* %s/' % (step0,step1) )
    return True

def downloadAndCompileSingle(file,part,step0,step1,json,pca,allowQuickCopy=True,download=1):
    #---------------------------------------------------------------------------------------
    if (download):
       if (not checkIfFileExists(file)):
           os.system('wget "http://ammar.gr/mocapnet/mnet4/%s"' % (file))
       else:
           print("Ensemble %s already exists locally" % (file))   
    #---------------------------------------------------------------------------------------
    if checkIfPathExists(step0) and checkIfPathIsDirectory(step0):
       print(bcolors.OKBLUE,"Clean up ",step0," \n",bcolors.ENDC)
       os.system('rm -rf %s/'% (step0))
    else:
       print("Do not need to clear step0 ",step0)
    if checkIfPathExists(step1) and checkIfPathIsDirectory(step1):
       print(bcolors.OKBLUE,"Clean up ",step1," \n",bcolors.ENDC)
       os.system('rm -rf %s/'% (step1))
    else:
       print("Do not need to clear step1 ",step1)
    print(bcolors.OKBLUE,"Extracting models from ",file," \n",bcolors.ENDC)
    #print(bcolors.OKBLUE,'tar -xf %s' % (file),bcolors.ENDC)
    os.system('tar -xf %s' % (file))
    
    if (allowQuickCopy) and (quickCopy(step0,step1)):
        print(bcolors.OKBLUE,"Quick Copied models \n",bcolors.ENDC)
    else:
        print(bcolors.OKBLUE,"Combine models \n",bcolors.ENDC)
        #os.system('cp %s/%s dataset/'%(step0,pca)) #We dont need to do this it will be done automatically from step1_DNNCombine.py
        #os.system('mkdir %s/'%step1)  #We dont need to do this it will be done automatically from step1_DNNCombine.py
        #os.system('cp %s/%s %s/'%(step0,json,step1))#We dont need to do this it will be done automatically from step1_DNNCombine.py
        #os.system('cp %s/%s %s/'%(step0,pca,step1))#We dont need to do this it will be done automatically from step1_DNNCombine.py
        print(bcolors.OKBLUE,"Combining %s \n"%part,bcolors.ENDC)
        print(bcolors.OKBLUE,'python3 -m step1_DNNCombine --config %s/%s --all %s'%(step0,json,part),bcolors.ENDC)
        return_value = os.system('python3 -m step1_DNNCombine --config %s/%s --all %s'%(step0,json,part))
        if (return_value!=0):
          print(bcolors.FAIL,"Failed preparing ",part," stopping model database retrieval",bcolors.ENDC)
          raise IOError
        #---------------------------------------------------------------------------------------
        gc.collect()
        #---------------------------------------------------------------------------------------
        RAM = getRAMInformation()
        if (RAM["free"]>=2000000):
            os.system('python3 -m tf2onnx.convert --saved-model %s --opset %u --tag serve --output %s/model.onnx' % (step1,ONNX_OPSET,step1))
            os.system('tflite_convert --saved_model_dir=%s --output_file=%s/model.tflite' % (step1,step1))
        else:
            print(bcolors.FAIL,"Not Enough RAM (we have ",RAM["free"],") to perform ONNX/TF-Lite conversions!",bcolors.ENDC)

    notification("MocapNET Database","Finished Compiling MocapNET %s ensemble" % part)

def downloadAndCompileModel(fileUpper,fileLower,fileHand="",fileFace="",fileReye="",fileMouth="",allowQuickCopy=True,download=1):
    #---------------------------------------------------------------------------------------
    if (fileUpper!=""):
       downloadAndCompileSingle(
                                fileUpper,
                                "upperbody",
                                "step0_upperbody_all",
                                "step1_upperbody_all",
                                "upperbody_configuration.json",
                                "upperbody_all.pca",
                                allowQuickCopy=allowQuickCopy
                               )
    #---------------------------------------------------------------------------------------
    if (fileLower!=""):
       downloadAndCompileSingle(
                                fileLower,
                                "lowerbody",
                                "step0_lowerbody_all",
                                "step1_lowerbody_all",
                                "lowerbody_configuration.json",
                                "lowerbody_all.pca",
                                allowQuickCopy=allowQuickCopy
                               )
    #---------------------------------------------------------------------------------------
    if (fileHand!=""):
       downloadAndCompileSingle(
                                fileHand,
                                "lhand",
                                "step0_lhand_all",
                                "step1_lhand_all",
                                "lhand_configuration.json",
                                "lhand_all.pca",
                                allowQuickCopy=allowQuickCopy
                               )
    #---------------------------------------------------------------------------------------
    if (fileFace!=""):
       downloadAndCompileSingle(
                                fileFace,
                                "face",
                                "step0_face_all",
                                "step1_face_all",
                                "face_configuration.json",
                                "face_all.pca",
                                allowQuickCopy=allowQuickCopy
                               )
    #---------------------------------------------------------------------------------------
    if (fileReye!=""):
       downloadAndCompileSingle(
                                fileReye,
                                "reye",
                                "step0_reye_all",
                                "step1_reye_all",
                                "reye_configuration.json",
                                "reye_all.pca",
                                allowQuickCopy=allowQuickCopy
                               )
    #---------------------------------------------------------------------------------------
    if (fileMouth!=""):
       downloadAndCompileSingle(
                                fileMouth,
                                "mouth",
                                "step0_mouth_all",
                                "step1_mouth_all",
                                "mouth_configuration.json",
                                "mouth_all.pca",
                                allowQuickCopy=allowQuickCopy
                               )
    #---------------------------------------------------------------------------------------
    print(bcolors.OKGREEN,"Models ready for use \n",bcolors.ENDC)
    print(bcolors.OKGREEN,"try : python3 -m PoseNET --from shuffle.webm\n",bcolors.ENDC)
    print(bcolors.OKGREEN,"or  : python3 -m evaluateMocapNET --config dataset/body_configuration.json --all body --skip 5\n",bcolors.ENDC)
    #---------------------------------------------------------------------------------------
    notification("MocapNET Database","Finished Compiling MocapNET ensemble")



def retrieveAndSetupModel(PCADimensions:int,parameterCount:int,download:int=0):
    #Baseline
    fileUpper = "87A_Training-22-05-06_13-22-16-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    fileLower = "87B_Training-22-05-06_22-29-54-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
     
    #PCA 15 - 105K Parameters
    if (PCADimensions==15) and (parameterCount==105):
       fileUpper = "81B_Training-22-05-03_05-10-48-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "82B_Training-22-05-03_15-03-10-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (PCADimensions==15) and (parameterCount==32):
       fileUpper = "84B_Training-22-05-05_17-12-02-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "85B_Training-22-05-06_03-08-00-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (PCADimensions==30) and (parameterCount==105):
       fileUpper = "79B_Training-22-05-01_06-59-02-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "80B_Training-22-05-01_17-01-13-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (PCADimensions==30) and (parameterCount==32):
       fileUpper = "90A_Training-22-05-08_17-56-16-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "90B_Training-22-05-09_00-15-55-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (PCADimensions==60) and (parameterCount==105):
       fileUpper = "94A_Training-22-05-10_13-08-52-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "94B_Training-22-05-10_19-18-19-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==60) and (parameterCount==32):
       fileUpper = "93A_Training-22-05-09_17-37-46-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "93B_Training-22-05-10_01-27-31-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (PCADimensions==90) and (parameterCount==105):
       fileUpper = "77B_Training-22-04-29_18-02-28-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "78B_Training-22-04-30_03-20-33-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (PCADimensions==90) and (parameterCount==32):
       fileUpper = "88A_Training-22-05-07_09-20-26-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "88B_Training-22-05-07_17-03-03-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (PCADimensions==120) and (parameterCount==105):
       fileUpper = "96A_Training-22-05-11_09-31-02-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "96B_Training-22-05-11_15-52-05-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==120) and (parameterCount==32):
       fileUpper = "77A_Training-22-04-28_22-55-49-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "78A_Training-22-04-29_05-11-09-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==150) and (parameterCount==105):
       fileUpper = "79A_Training-22-04-30_12-55-12-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "80A_Training-22-04-30_19-08-26-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==150) and (parameterCount==32):
       fileUpper = "92A_Training-22-05-09_11-28-35-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "92B_Training-22-05-09_18-00-37-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==180) and (parameterCount==105):
       fileUpper = "99A_Training-22-05-13_01-55-31-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "99B_Training-22-05-13_08-45-27-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==180) and (parameterCount==32):
       fileUpper = "91A_Training-22-05-08_11-35-19-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "91B_Training-22-05-08_18-15-12-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==210) and (parameterCount==105):
       fileUpper = "83A_Training-22-05-03_03-35-52-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "83B_Training-22-05-03_10-10-19-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==210) and (parameterCount==30):
       fileUpper = "103A_Training-22-05-20_07-41-34-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "103B_Training-22-05-21_01-16-48-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==210) and (parameterCount==32):
       fileUpper = "84A_Training-22-05-04_11-39-49-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "85A_Training-22-05-04_19-08-05-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==210) and (parameterCount==417):
       #12 layers deep!
       fileUpper = "106A_Training-22-07-11_09-35-20-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "106B_Training-22-07-11_15-57-01-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==210) and (parameterCount==326):
       #12 layers deep!
       fileUpper = "107A_Training-22-07-13_09-30-24-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "107B_Training-22-07-13_15-40-47-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==210) and (parameterCount==83):
       #6 layers deep!
       fileUpper = "100A_Training-22-05-14_21-23-11-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "100B_Training-22-05-15_02-28-09-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==210) and (parameterCount==64):
       #4 layers deep!
       fileUpper = "101A_Training-22-05-16_04-13-58-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "101B_Training-22-05-16_08-08-49-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (PCADimensions==323) and (parameterCount==105):
       fileUpper = "95A_Training-22-05-11_04-10-01-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "95B_Training-22-05-11_11-26-16-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (PCADimensions==323) and (parameterCount==32):
       print("TODO")
    elif (PCADimensions==323) and (parameterCount==153):
       fileUpper = "87A_Training-22-05-06_13-22-16-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "87B_Training-22-05-06_22-29-54-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    else:
       print(bcolors.WARNING,"Unknown combination PCA dims ",PCADimensions," and ",parameterCount,"K parameters \n",bcolors.ENDC)
       print(bcolors.WARNING,"Completely halting execution to avoid a wrong run!\n",bcolors.ENDC)
       sys.exit(1)
       return;
    
    print("Selecting PCA dims ",PCADimensions," and ",parameterCount," K parameters \n")
    downloadAndCompileModel(fileUpper,fileLower,download=download)
    


def retrieveAndSetupBasedOnSerial(serial:int,allowQuickCopy:bool=True,download:int=0):
    #Baseline
    fileUpper = "" #"116A_Training-22-09-01_00-27-23-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    fileLower = "" #"116B_Training-22-09-01_06-19-41-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    fileHand  = ""
    fileFace  = ""
    fileReye  = ""
    fileMouth = ""
 
    if (serial==116):
       fileUpper = "116A_Training-22-09-01_00-27-23-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "116B_Training-22-09-01_06-19-41-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (serial==117):
       fileUpper = "117A_Training-22-09-02_15-14-17-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "117B_Training-22-09-03_03-56-55-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (serial==118):
       fileUpper = "118A_Training-22-09-05_02-34-35-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "118B_Training-22-09-05_10-27-46-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (serial==119):
       fileUpper = "119A_Training-22-09-05_23-19-05-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "119B_Training-22-09-06_05-52-27-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (serial==120):
       fileUpper = "120A_Training-22-09-06_19-16-27-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "120B_Training-22-09-07_00-21-03-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (serial==125):
       fileUpper = "125-A-Training-22-10-12_06-28-17-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "125-B-Training-22-10-12_12-06-36-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (serial==128):
       fileUpper = "128-a-remanual.tar.bz2"
       fileLower = "128-b-remanual.tar.bz2"
    elif (serial==129):
       fileUpper = "129-A-Training-22-10-18_01-17-44-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "129-B-Training-22-10-18_07-11-41-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==130):
       fileUpper = "130-A-Training-22-10-19_23-50-56-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "130-B-Training-22-10-20_07-05-38-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (serial==131):
       fileUpper = "131-A-manual.tar.bz2"
       fileLower = "131-B-manual.tar.bz2"
    elif (serial==132):
       fileUpper = "132-A-Training-22-10-23_21-16-03-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "132-B-Training-22-10-24_07-28-31-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==133):
       fileUpper = "133-A-Training-22-10-25_08-24-58-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "133-B-Training-22-10-25_16-11-05-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==134):
       fileUpper = "134-A-Training-22-10-27_06-52-13-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "134-B-Training-22-10-28_20-21-16-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (serial==137):
       fileUpper = "137-A-Training-22-10-29_05-15-56-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "137-B-Training-22-10-29_10-55-49-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==138):
       fileUpper = "138-A-Training-22-10-30_03-45-34-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "138-B-Training-22-10-30_12-51-38-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==139):
       fileUpper = "139-A-Training-22-10-31_04-38-46-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "139-B-Training-22-10-31_13-29-44-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==140):
       fileUpper = "140-A-Training-22-11-02_09-41-59-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "140-B-Training-22-11-02_17-37-13-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==150):
       fileUpper = "150-A-Training-22-12-01_05-34-13-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "150-B-Training-22-12-02_06-58-52-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==168):
       fileUpper = "168-A-Training-23-01-28_22-23-09-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "168-B-Training-23-01-31_02-23-50-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileHand  = "169A-Training-23-02-02_19-05-54-lhand-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileFace  = "167A-Training-23-01-27_10-23-03-face-elina-kriti-Ubuntu-22.04.tar.bz2"
    elif (serial==170):
       fileUpper = "170-A-Training-23-02-05_18-10-16-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "170-B-Training-23-02-06_03-49-34-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileHand  = "169B-Training-23-02-05_13-51-44-lhand-cvrl-demo-Ubuntu-18.04.tar.bz2"
    elif (serial==174):
       fileUpper = "174-A-Training-23-02-16_12-34-37-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "174-B-Training-23-02-17_07-21-02-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileHand  = "169B-Training-23-02-05_13-51-44-lhand-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileFace  = "176A-Training-23-02-17_15-49-14-face-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==177):
       fileUpper = "174-A-Training-23-02-16_12-34-37-upperbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileLower = "174-B-Training-23-02-17_07-21-02-lowerbody-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileHand  = "175B-Training-23-02-21_05-10-46-lhand-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileFace  = "176A-Training-23-02-17_15-49-14-face-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==193):
       fileUpper = "193-A-Training-23-03-18_22-07-35-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "193-B-Training-23-03-21_01-31-46-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       #fileHand  = "175B-Training-23-02-21_05-10-46-lhand-cvrl-demo-Ubuntu-18.04.tar.bz2"
       #fileFace  = "176A-Training-23-02-17_15-49-14-face-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==209):
       fileUpper = "209-A-Training-23-03-23_16-11-00-upperbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileLower = "209-B-Training-23-03-24_13-16-47-lowerbody-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileHand  = "217A-Training-23-04-11_03-40-56-lhand-cvrl-demo-Ubuntu-18.04.tar.bz2"
       #fileFace  = "176A-Training-23-02-17_15-49-14-face-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==217):
       fileHand  = "217A-Training-23-04-11_03-40-56-lhand-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileUpper = ""
       fileLower = ""
    elif (serial==232):
       fileUpper = "232-A-Training-23-04-21_17-14-06-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2" #ffe5156750f3 is supermicro docker container
       fileLower = "232-B-Training-23-04-21_21-37-52-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileHand  = "230A-Training-23-04-24_11-14-08-lhand-cvrl-demo-Ubuntu-18.04.tar.bz2"
       fileFace  = "233A-Training-23-04-23_06-39-35-face-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==234):
       fileUpper = "234-A-Training-23-04-25_06-27-18-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "234-B-Training-23-04-26_00-29-55-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==241):
       fileUpper = "241-A-Training-23-05-05_20-51-52-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "241-B-Training-23-05-06_01-32-59-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==242):
       fileUpper = "242-A-Training-23-05-07_11-16-13-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "242-B-Training-23-05-07_16-18-47-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==262):
       fileUpper = "262-A-Training-23-05-20_10-41-38-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "262-B-Training-23-05-21_12-00-38-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       #fileHand  = "251A-Training-23-05-19_17-32-01-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       #fileReye  = "261B-Training-23-05-19_03-25-32-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
       #fileMouth = "253A-Training-23-05-13_09-53-34-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==269):
       fileUpper = "268-A-Training-23-05-27_20-01-59-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "268-B-Training-23-05-28_16-05-19-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileHand  = "269A-Training-23-05-30_22-06-06-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileReye  = "267B-Training-23-05-26_06-50-57-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileMouth = "269B-Training-23-05-29_09-46-36-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==266):
       fileUpper = "266-A-Training-23-05-25_12-31-28-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "266-B-Training-23-05-25_21-46-50-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==268):
       fileUpper = "268-A-Training-23-05-27_20-01-59-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "268-B-Training-23-05-28_16-05-19-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==270):
       fileUpper = "270-A-Training-23-05-30_16-32-29-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "270-B-Training-23-05-31_12-36-21-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==272):
       fileUpper = "272-A-Training-23-06-03_06-50-09-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "272-B-Training-23-06-04_22-01-12-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==274):
       fileUpper = "274-A-Training-23-06-07_23-45-42-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "274-B-Training-23-06-09_19-32-34-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==277):
       fileUpper = "277-A-Training-23-06-11_21-50-57-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "277-B-Training-23-06-13_08-41-09-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==279):
       fileUpper = "279-A-Training-23-06-15_13-36-59-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "279-B-Training-23-06-16_23-44-25-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==282):
       fileUpper = "282-A-Training-23-06-18_06-16-23-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "282-B-Training-23-06-19_03-35-29-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==284):
       #fileUpper = "282-A-Training-23-06-18_06-16-23-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       #fileLower = "282-B-Training-23-06-19_03-35-29-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       #fileHand  = "280B-Training-23-06-17_14-26-51-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileReye  = "283A-Training-23-06-19_10-01-31-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileMouth = "284A-Training-23-06-20_12-01-36-mouth-ammar-forth-Ubuntu-20.04.tar.bz2" #<- Paper FACE
    elif (serial==285):
       fileUpper = "285-A-Training-23-06-20_18-26-12-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "285-B-Training-23-06-21_10-17-45-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==287):
       fileUpper = "287-A-Training-23-06-22_13-25-01-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "287-B-Training-23-06-23_07-02-15-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==289):
       fileUpper = "289-A-Training-23-06-24_21-37-02-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "289-B-Training-23-06-25_03-03-02-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
    elif (serial==290):
       fileUpper = "289-A-Training-23-06-24_21-37-02-upperbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileLower = "289-B-Training-23-06-25_03-03-02-lowerbody-cvrl-demo-Ubuntu-20.04.tar.bz2"
       fileHand  = "290A-Training-23-06-28_10-31-52-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileReye  = "290B-Training-23-06-27_14-48-40-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileMouth = "284B-Training-23-06-21_03-14-26-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==292):
       fileUpper = "292-A-Training-23-07-09_18-18-30-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "292-B-Training-23-07-10_13-51-52-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==293):
       fileMouth = "293B-Training-23-07-13_05-05-04-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==294):
       fileUpper = "294-A-Training-23-07-11_19-04-21-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "294-B-Training-23-07-12_01-32-27-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==295):
       fileUpper = "295-A-Training-23-07-12_16-47-41-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "295-B-Training-23-07-12_22-49-19-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==297):
       fileUpper = "297-A-Training-23-07-14_16-50-22-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "297-B-Training-23-07-14_21-34-29-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileReye  = "297A-Training-23-07-14_07-26-12-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileMouth = "293B-Training-23-07-13_05-05-04-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==300):
       fileUpper = "300-A-Training-23-07-16_02-44-28-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "300-B-Training-23-07-16_07-40-40-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==301):
       fileUpper = "301-A-Training-23-07-16_17-58-25-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "301-B-Training-23-07-16_22-45-49-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==302):
       fileUpper = "302-A-Training-23-07-17_06-32-23-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "302-B-Training-23-07-17_11-35-01-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==304):
       fileUpper = "302-A-Training-23-07-17_06-32-23-upperbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileLower = "302-B-Training-23-07-17_11-35-01-lowerbody-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileHand  = "290A-Training-23-06-28_10-31-52-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileMouth = "299B-Training-23-07-17_08-29-17-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileReye  = "304A-Training-23-07-19_16-51-17-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==305):
       fileHand  = "305B-Training-23-07-29_09-52-21-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==307):
       fileHand  = "307A-Training-23-08-19_23-23-06-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
    elif (serial==310): #COMMON LOSS EXPERIMENTS
       fileHand  = "307B-Training-23-09-08_13-54-22-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "309-A-Training-23-09-08_00-20-09-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "310B.tar.bz2"
       fileMouth = "299B-Training-23-07-17_08-29-17-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileReye  = "304A-Training-23-07-19_16-51-17-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==314): #COMMON LOSS EXPERIMENTS
       fileHand  = "314C-Training-23-09-12_09-40-45-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "314-A-Training-23-09-11_07-21-06-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "314-B-Training-23-09-11_17-55-38-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "299B-Training-23-07-17_08-29-17-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileReye  = "304A-Training-23-07-19_16-51-17-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==314): #COMMON LOSS EXPERIMENTS
       fileHand  = "314C-Training-23-09-12_09-40-45-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "314-A-Training-23-09-11_07-21-06-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "314-B-Training-23-09-11_17-55-38-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "299B-Training-23-07-17_08-29-17-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileReye  = "304A-Training-23-07-19_16-51-17-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==-314): #AUGMENTED HAND FROM THE FUTURE / COMMON LOSS EXPERIMENTS
       fileHand  = "323A-Training-23-10-11_21-40-44-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "314-A-Training-23-09-11_07-21-06-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "314-B-Training-23-09-11_17-55-38-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "299B-Training-23-07-17_08-29-17-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileReye  = "304A-Training-23-07-19_16-51-17-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==315): #COMMON LOSS EXPERIMENTS
       fileHand  = "314D-Training-23-09-14_02-14-17-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "315-A-Training-23-09-14_00-34-56-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "315-B-Training-23-09-14_09-00-31-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "299B-Training-23-07-17_08-29-17-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileReye  = "304A-Training-23-07-19_16-51-17-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==317): #COMMON LOSS EXPERIMENTS
       fileHand  = "315C-Training-23-09-16_09-40-44-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "317-A-Training-23-09-19_00-29-49-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "317-B-Training-23-09-19_08-00-19-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "299B-Training-23-07-17_08-29-17-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileReye  = "304A-Training-23-07-19_16-51-17-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==318): #COMMON LOSS EXPERIMENTS
       fileHand  = "318C-Training-23-09-28_13-58-59-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "318-A-Training-23-09-19_23-26-18-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "318-B-Training-23-09-20_08-16-26-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "299B-Training-23-07-17_08-29-17-mouth-ammar-forth-Ubuntu-20.04.tar.bz2"
       fileReye  = "304A-Training-23-07-19_16-51-17-reye-ammar-forth-Ubuntu-20.04.tar.bz2"
    elif (serial==319): #COMMON LOSS EXPERIMENTS
       fileHand  = "318C-Training-23-09-28_13-58-59-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "319-A-Training-23-09-21_11-34-19-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "319-B-Training-23-09-21_18-59-56-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "318D-Training-23-09-25_17-06-26-mouth-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileReye  = "320B-Training-23-09-24_23-08-19-reye-ammar-forth-Ubuntu-22.04.tar.bz2"
    elif (serial==324): #COMMON LOSS EXPERIMENTS
       fileHand  = "323B-Training-23-10-26_21-41-51-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "324-A-Training-23-10-11_07-49-40-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "324-B-Training-23-10-11_17-24-31-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "318D-Training-23-09-25_17-06-26-mouth-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileReye  = "320B-Training-23-09-24_23-08-19-reye-ammar-forth-Ubuntu-22.04.tar.bz2"
    elif (serial==325): #COMMON LOSS EXPERIMENTS
       fileHand  = "323B-Training-23-10-26_21-41-51-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "325-A-Training-23-10-13_19-20-50-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "325-B-Training-23-10-13_23-35-55-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "318D-Training-23-09-25_17-06-26-mouth-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileReye  = "320B-Training-23-09-24_23-08-19-reye-ammar-forth-Ubuntu-22.04.tar.bz2"
    elif (serial==326): #COMMON LOSS EXPERIMENTS
       fileHand  = "323B-Training-23-10-26_21-41-51-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "326-A-Training-23-10-14_10-10-30-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "326-B-Training-23-10-14_17-49-14-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "318D-Training-23-09-25_17-06-26-mouth-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileReye  = "320B-Training-23-09-24_23-08-19-reye-ammar-forth-Ubuntu-22.04.tar.bz2"
    elif (serial==327): #COMMON LOSS EXPERIMENTS
       fileHand  = "323B-Training-23-10-26_21-41-51-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "327-A-Training-23-10-19_21-52-53-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "327-B-Training-23-10-20_04-48-26-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "318D-Training-23-09-25_17-06-26-mouth-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileReye  = "320B-Training-23-09-24_23-08-19-reye-ammar-forth-Ubuntu-22.04.tar.bz2"
    elif (serial==328): #COMMON LOSS EXPERIMENTS
       fileHand  = "323B-Training-23-10-26_21-41-51-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "328-A-Training-23-10-20_17-07-08-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "328-B-Training-23-10-20_23-34-06-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "318D-Training-23-09-25_17-06-26-mouth-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileReye  = "320B-Training-23-09-24_23-08-19-reye-ammar-forth-Ubuntu-22.04.tar.bz2"
    elif (serial==339): #COMMON LOSS EXPERIMENTS
       fileHand  = "323B-Training-23-10-26_21-41-51-lhand-ffe5156750f3-Ubuntu-20.04.tar.bz2"
       fileUpper = "339-A-Training-23-11-10_08-28-59-upperbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileLower = "339-B-Training-23-11-10_15-36-02-lowerbody-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileMouth = "318D-Training-23-09-25_17-06-26-mouth-ammar-forth-Ubuntu-22.04.tar.bz2"
       fileReye  = "320B-Training-23-09-24_23-08-19-reye-ammar-forth-Ubuntu-22.04.tar.bz2"
    else:
       print(bcolors.WARNING,"Unknown serial ",serial,bcolors.ENDC)
       print(bcolors.WARNING,"Completely halting execution to avoid a wrong run!\n",bcolors.ENDC)
       sys.exit(1)
       return;
    
    print("Selecting Serial ",serial," => ",fileUpper," ",fileLower," \n")
    print("FileHand ",fileHand," fileFace ",fileFace," \n")
    downloadAndCompileModel(fileUpper,fileLower,fileHand=fileHand,fileFace=fileFace,fileReye=fileReye,fileMouth=fileMouth,allowQuickCopy=allowQuickCopy,download=download)
    

if __name__ == '__main__':

    startEverythingAt = time.time()
   
    RAM = getRAMInformation()
    print("RAM : ",RAM)
    if (RAM["free"]<2000000):
        print(bcolors.FAIL,"We do not have enough memory (we have ",RAM["free"],") for ONNX / TF-Lite model so stopping..",bcolors.ENDC)
        raise MemoryError
        

    db = downloadAndParseDatabase()
    print(db)
    #sys.exit(0)
    

    serial=0
    allowQuickCopy = False
    #When working standalone assume simple csv backend
    if (len(sys.argv)>1):
       print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--noquickcopy"):
              allowQuickCopy = False
           if (sys.argv[i]=="--quickcopy"):
              allowQuickCopy = True
       #--------------------------------------
       for i in range(0, len(sys.argv)): 
           if (sys.argv[i]=="--getPCA"):
              PCADimensions =int(sys.argv[i+1])
              parameterCount=int(sys.argv[i+2])
              retrieveAndSetupModel(PCADimensions,parameterCount)
           if (sys.argv[i]=="--get"):
              serial =int(sys.argv[i+1]) 
              retrieveAndSetupBasedOnSerial(serial,allowQuickCopy=allowQuickCopy,download=1)
       #--------------------------------------
       for i in range(0, len(sys.argv)): 
           if (sys.argv[i]=="--pack"):
              print(bcolors.WARNING,"Packaging to -> mnet4_%u.zip "%(serial),bcolors.ENDC)
              os.system('zip mnet4_%u.zip step1_*/*' % (serial))
               
       

    endAt = time.time()
    finalMessage="getModelFromDatabase run took %0.2f mins"%((endAt-startEverythingAt)/60)
    print(finalMessage)



