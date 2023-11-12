#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import os 
from tools    import secondsToHz,eprint

#https://tfhub.dev/google/movenet/singlepose/lightning/4
#https://tfhub.dev/google/lite-model/movenet/singlepose/lightning/tflite/int8/4
#wget -q -O lite-model_movenet_singlepose_lightning_tflite_int8_4.tflite https://storage.googleapis.com/tfhub-lite-models/google/lite-model/movenet/singlepose/lightning/tflite/int8/4.tflite

#zip movenet.zip movenet/* movenet/*/*

trainingWidth  = 1920
trainingHeight = 1080

def getCaptureDeviceFromPath(videoFilePath,videoWidth,videoHeight):
  import cv2
  #------------------------------------------
  if (videoFilePath=="esp"):
     from espStream import ESP32CamStreamer
     cap = ESP32CamStreamer()
  elif (videoFilePath=="webcam"):
     cap = cv2.VideoCapture(0)
     cap.set(cv2.CAP_PROP_FRAME_WIDTH, videoWidth)
     cap.set(cv2.CAP_PROP_FRAME_HEIGHT, videoHeight)
  elif (videoFilePath=="/dev/video0"):
     cap = cv2.VideoCapture(0)
     cap.set(cv2.CAP_PROP_FRAME_WIDTH, videoWidth)
     cap.set(cv2.CAP_PROP_FRAME_HEIGHT, videoHeight)
  elif (videoFilePath=="/dev/video1"):
     cap = cv2.VideoCapture(1)
     cap.set(cv2.CAP_PROP_FRAME_WIDTH, videoWidth)
     cap.set(cv2.CAP_PROP_FRAME_HEIGHT, videoHeight)
  elif (videoFilePath=="/dev/video2"):
     cap = cv2.VideoCapture(2)
     cap.set(cv2.CAP_PROP_FRAME_WIDTH, videoWidth)
     cap.set(cv2.CAP_PROP_FRAME_HEIGHT, videoHeight)
  else:
     from tools import checkIfPathIsDirectory
     if (checkIfPathIsDirectory(videoFilePath) and (not "/dev/" in videoFilePath) ):
        from folderStream import FolderStreamer
        cap = FolderStreamer(path=videoFilePath,width=videoWidth,height=videoHeight)
        mnet.bvh.configureRendererFromFile("%s/color.calib"%videoFilePath)
     else:
        cap = cv2.VideoCapture(videoFilePath)
  return cap 



def runPoseNETSerial(): 
  #Parse command line arguments
  #-----------------------------------------
  import sys
  import cv2
  import time
  headless             = False
  economicVisualization= False
  saveVideo            = False
  videoFilePath        = "webcam" 
  videoWidth           = 1280
  videoHeight          = 720
  doProfiling          = False
  doFlipX              = False
  engine               = "onnx"
  doNNEveryNFrames     = 1 # 3 
  bvhScale             = 1.0
  doHCDPostProcessing  = 1 
  hcdLearningRate      = 0.001
  hcdEpochs            = 15
  hcdIterations        = 30
  smoothingSampling    = 30.0
  smoothingCutoff      = 5.0
  threshold            = 0.05
  calibrationFile      = ""
  plotBVHChannels      = False
  bvhAnglesForPlotting = list()
  bvhAllAnglesForPlotting = list()


  if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--save"):
              saveVideo=True
           if (sys.argv[i]=="--nnsubsample"):
              doNNEveryNFrames    = int(sys.argv[i+1])
           if (sys.argv[i]=="--headless"):
              headless = True
           if (sys.argv[i]=="--flipx"):
              doFlipX = True
           if (sys.argv[i]=="--plot"):
              plotBVHChannels=True
           if (sys.argv[i]=="--nonn"):
              doNNEveryNFrames = 1000
           if (sys.argv[i]=="--calib"):
              calibrationFile = sys.argv[i+1]
           if (sys.argv[i]=="--ik"):
               hcdLearningRate     = float(sys.argv[i+1])
               hcdEpochs           = int(sys.argv[i+2])
               hcdIterations       = int(sys.argv[i+3])
           if (sys.argv[i]=="--smooth"):
               smoothingSampling   = float(sys.argv[i+1])
               smoothingCutoff     = float(sys.argv[i+2])
           if (sys.argv[i]=="--noik"):
              doHCDPostProcessing = 0
              doNNEveryNFrames = 1
           if (sys.argv[i]=="--scale"):
              bvhScale=float(sys.argv[i+1])
           if (sys.argv[i]=="--from"):
              videoFilePath=sys.argv[i+1]
           if (sys.argv[i]=="--engine"):
              engine=sys.argv[i+1]
           if (sys.argv[i]=="--plot"):
              plotBVHChannels=True

  # For webcam input:
  frameNumber = 0
  #------------------------------------------ 
  cap = getCaptureDeviceFromPath(videoFilePath,videoWidth,videoHeight)

  #-----------------------------------------
  #python3 -m tf2onnx.convert --saved-model movenet --opset 14 --output movenet/model.onnx
  #zip movenet.zip movenet/* movenet/*/*
  #-----------------------------------------
  
  bvhAnglesForPlotting = list()
 
  # Initialize the PoseNET 
  if (engine=="tensorflow"):
      from MocapNETTensorflow import PoseNET
      poseNET = PoseNET(modelPath="movenet/",trainingWidth=trainingWidth,trainingHeight=trainingHeight) 
  elif (engine=="onnx"):
      from MocapNETONNX import PoseNETONNX
      poseNET = PoseNETONNX(modelPath="movenet/model.onnx",trainingWidth=trainingWidth,trainingHeight=trainingHeight) 
  elif (engine=="tflite"):
      from MocapNETTFLite import PoseNETTFLite
      poseNET = PoseNETTFLite(trainingWidth=trainingWidth,trainingHeight=trainingHeight)
  else:
      print("Unknown engine (",engine,") for MoveNET")
      sys.exit(1)


  #Select a MocapNET class from tensorflow/tensorrt/onnx/tf-lite engines
  from MocapNET import easyMocapNETConstructor
  mnet = easyMocapNETConstructor( 
                                 engine,
                                 doProfiling         = doProfiling,
                                 doBody              = False, #<- override whole body
                                 doUpperbody         = True,
                                 doLowerbody         = True,
                                 doHCDPostProcessing = doHCDPostProcessing, 
                                 hcdLearningRate     = hcdLearningRate,
                                 hcdEpochs           = hcdEpochs,
                                 hcdIterations       = hcdIterations,
                                 smoothingSampling   = smoothingSampling,
                                 smoothingCutoff     = smoothingCutoff,   
                                 doFace              = False,
                                 doREye              = False,
                                 doMouth             = False,
                                 doHands             = False,
                                 bvhScale=bvhScale
                               )


  if (calibrationFile!=""):
        print("Enforcing Calibration file : ",calibrationFile)
        mnet.bvh.configureRendererFromFile(calibrationFile)

  mnet.test()

 
  #------------------------------------------------
  #------------------------------------------------
  #------------------------------------------------
  print("\n\n\n\nStarting MocapNET in Singlethreaded mode using BlazePose 2D Input")
  print("Please wait until input processing finishes!")
  while cap.isOpened():
    success, annotated_image = cap.read()
    if not success:
      print("Ignoring empty camera frame.")
      break
      # If loading a video, use 'break' instead of 'continue'.
      #continue
    #print(image.type)
     
    start = time.time() 
    #Our 2D Joint Estimation
    #------------------------------------------------------------------------------------
    mocapNETInput,annotated_image = poseNET.convertImageToMocapNETInput(annotated_image,doFlipX=doFlipX,threshold=threshold)
    #------------------------------------------------------------------------------------ 
    end = time.time() # Time elapsed
    mnet.hz_2DEst = secondsToHz(end - start)
    mnet.history_hz_2DEst.append(mnet.hz_2DEst)
    if (len(mnet.history_hz_2DEst)>mnet.perfHistorySize): 
            mnet.history_hz_2DEst.pop(0) #Keep mnet history on limits



    #Our 3D Joint Estimation
    #------------------------------------------------------------------------------------
    doNN = (frameNumber%doNNEveryNFrames)==0
    mocapNET3DOutput = mnet.predict3DJoints(mocapNETInput,runNN=doNN,runHCD=True)
    mocapNETBVHOutput = mnet.outputBVH
    bvhAnglesForPlotting.append(mocapNETBVHOutput)
    bvhAllAnglesForPlotting.append(mocapNETBVHOutput)
    if (len(bvhAnglesForPlotting)>100):
       bvhAnglesForPlotting.pop(0)
    #------------------------------------------------------------------------------------
    from MocapNETVisualization import visualizeMocapNETEnsemble
    image,plotImage = visualizeMocapNETEnsemble(mnet,annotated_image,plotBVHChannels=plotBVHChannels,bvhAnglesForPlotting=bvhAnglesForPlotting,economic=economicVisualization)
    #------------------------------------------------------------------------------------


    seconds = time.time() - start
    fps  = 1 / (seconds+0.0001)
    #print("\r PoseNET+MocepNET aggregate Framerate : ",round(fps,2)," fps           \r", end="", flush=True)
    #print("\n", end="", flush=True)
  
    font = cv2.FONT_HERSHEY_SIMPLEX 
    org = (50, 50) 
    fontScale = 1 
    color = (0,0,0)
    thickness = 2 

    message =  'MNET4+ ST/%s/NN:%u/%0.2f fps (2DNN %0.2f/3DNN %0.2f/3DHCD %0.2f)' % (engine,doNN,fps,poseNET.hz,mnet.hz_NN,mnet.hz_HCD) 
    annotated_image = cv2.putText(annotated_image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    org = (52, 52) 
    color = (255,255,255)
    annotated_image = cv2.putText(annotated_image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)

    #cv2.imwrite('mediapipe_%05u.jpg'%frameNumber, annotated_image)
    frameNumber = frameNumber + 1
    

    if (saveVideo): 
        cv2.imwrite('colorFrame_0_%05u.jpg'%(frameNumber), annotated_image)
        if (plotBVHChannels):
              cv2.imwrite('plotFrame_0_%05u.jpg'%(frameNumber), plotImage)
        
    if not headless:
      cv2.imshow('MocapNET 4 using PoseNET Holistic 2D Joints', annotated_image)
      if (plotBVHChannels):
           cv2.imshow('MocapNET 4 using PoseNET Holistic Motion History',plotImage) 
 
      if cv2.waitKey(1) & 0xFF == 27:
         break



  cap.release()

  if (saveVideo): #                                              1280x720 by default
     os.system("ffmpeg -framerate 30 -i colorFrame_0_%%05d.jpg -s %ux%u  -y -r 30 -pix_fmt yuv420p -threads 8 livelastRun3DHiRes.mp4 && rm colorFrame_0_*.jpg " % (videoWidth,videoHeight)) # 
     if (plotBVHChannels):
        os.system("ffmpeg -framerate 30 -i plotFrame_0_%05d.jpg -s 1200x720  -y -r 30 -pix_fmt yuv420p -threads 8 livelastPlot3DHiRes.mp4 && rm plotFrame_0_*.jpg")
     




def runPoseNETParallel(): 
  #Parse command line arguments
  #-----------------------------------------
  import sys
  import cv2
  import time
  import threading

  headless         = False
  videoFilePath    = "webcam" 
  videoWidth           = 1280
  videoHeight          = 720
  saveVideo        = False
  plotBVHChannels  = False
  doProfiling      = False
  doFlipX          = False
  engine           = "onnx"
  doNNEveryNFrames = 1 
  bvhScale         = 1.0
  threshold        = 0.05


  if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--headless"):
              headless = True
           if (sys.argv[i]=="--flipx"):
              doFlipX = True
           if (sys.argv[i]=="--plot"):
              plotBVHChannels=True
           if (sys.argv[i]=="--scale"):
              bvhScale=float(sys.argv[i+1])
           if (sys.argv[i]=="--noplot"):
              plotBVHChannels=0
           if (sys.argv[i]=="--from"):
              videoFilePath=sys.argv[i+1]
           if (sys.argv[i]=="--engine"):
              engine=sys.argv[i+1]
           if (sys.argv[i]=="--dump"):
              global doFrameDumpingForTiles
              doFrameDumpingForTiles=1


  # For webcam input:
  #-----------------------------------------
  frameNumber = 0
  cap = getCaptureDeviceFromPath(videoFilePath,videoWidth,videoHeight)
  #-----------------------------------------
  #python3 -m tf2onnx.convert --saved-model movenet --opset 14 --output movenet/model.onnx
  #zip movenet.zip movenet/* movenet/*/*
  #-----------------------------------------
  
  from MocapNETVisualization import drawMocapNETOutput,drawMocapNETAllPlots,drawMissingInput
  #It is important for MocapNET to be the first to be initialized!
  #So the tensorflow configuration will be set by it ( if tensorflow engine is selected )
  #Select a MocapNET class from tensorflow/tensorrt/onnx/tf-lite engines
  from MocapNET import easyMocapNETConstructor
  mnet = easyMocapNETConstructor(engine,doProfiling=doProfiling,bvhScale=bvhScale)
  mnet.test()

  bvhAnglesForPlotting = list()
 
 
  # Initialize the PoseNET 
  if (engine=="tensorflow"):
      from MocapNETTensorflow import PoseNET
      poseNET = PoseNET(modelPath="movenet/",trainingWidth=trainingWidth,trainingHeight=trainingHeight) 
  elif (engine=="onnx"):
      from MocapNETONNX import PoseNETONNX
      poseNET = PoseNETONNX(modelPath="movenet/model.onnx",trainingWidth=trainingWidth,trainingHeight=trainingHeight) 
  elif (engine=="tflite"):
      from MocapNETTFLite import PoseNETTFLite
      poseNET = PoseNETTFLite(trainingWidth=trainingWidth,trainingHeight=trainingHeight)
  else:
      print("Unknown engine (",engine,") for MoveNET")
      sys.exit(1)



  if cap.isOpened():
    success, previous_image = cap.read()
    if not success:
      print("Could not grab first frame!.")
      sys.exit(0)
    mocapNETInput,previous_image = poseNET.convertImageToMocapNETInput(previous_image,doFlipX=doFlipX,threshold=threshold)
 
  #------------------------------------------------
  #------------------------------------------------
  #------------------------------------------------
  print("Starting MocapNET in Multithreaded mode using BlazePose 2D Input")
  while cap.isOpened():
    success, next_image = cap.read()
    if not success:
      print("Ignoring empty camera frame.")
      break
      # If loading a video, use 'break' instead of 'continue'.
      #continue
    #print(image.type)
    
    start = time.time() 
    #Our 2D Joint Estimation AND 3D Joint Estimation happening in parallel
    #------------------------------------------------------------------------------------ 
    doNN = (frameNumber%doNNEveryNFrames)==0
    t1 = threading.Thread(name='predict3DJoints', target=mnet.predict3DJoints, args=(mocapNETInput,),kwargs={'runNN': doNN , 'runHCD' : True})
    t2 = threading.Thread(name='convertImageToMocapNETInput', target=poseNET.convertImageToMocapNETInput, args=(next_image,))
    #------------------------------------------------------------------------------------
    t1.start()
    t2.start() 
    # All threads running in parallel, now we wait
    #                 ...
    t1.join()
    t2.join()
    #------------------------------------------------------------------------------------
    mocapNET3DOutput  = mnet.output3D
    mocapNETBVHOutput = mnet.outputBVH
    bvhAnglesForPlotting.append(mocapNETBVHOutput)
    if (len(bvhAnglesForPlotting)>100):
       bvhAnglesForPlotting.pop(0)

    #------------------------------------------------------------------------------------
    from MocapNETVisualization import visualizeMocapNETEnsemble
    image,plotImage = visualizeMocapNETEnsemble(mnet,previous_image,plotBVHChannels=plotBVHChannels,bvhAnglesForPlotting=bvhAnglesForPlotting,economic=True)
    #------------------------------------------------------------------------------------

    mocapNETInput    = poseNET.output
    next_image       = poseNET.image
    #------------------------------------------------------------------------------------
    seconds = time.time() - start
    fps  = 1 / (seconds+0.0001)
    #print("\r MoveNET+MocepNET MT aggregate Framerate : ",round(fps,2)," fps           \r", end="", flush=True)
    #print("\n", end="", flush=True)
  

    frameNumber = frameNumber + 1
    

    #drawMocapNETOutput(mnet,previous_image)
    font = cv2.FONT_HERSHEY_SIMPLEX 
    org = (50, 50) 
    fontScale = 1 
    color = (0,0,0)
    thickness = 2 

    message =  'MNET4+ MT/%s/NN:%u/%0.2f fps (2DNN %0.2f/3DNN %0.2f/3DHCD %0.2f)' % (engine,doNN,fps,poseNET.hz,mnet.hz_NN,mnet.hz_HCD) 
    previous_image = cv2.putText(previous_image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    org = (52, 52) 
    color = (255,255,255)
    previous_image = cv2.putText(previous_image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    #------------------------------------------------------------------------------------------------------------

    if (saveVideo): 
        cv2.imwrite('colorFrame_0_%05u.jpg'%(frameNumber), annotated_image)
        if (plotBVHChannels):
              cv2.imwrite('plotFrame_0_%05u.jpg'%(frameNumber), plotImage)
        
    if not headless:
      cv2.imshow('MocapNET 4 using MoveNET Holistic 2D Joints', previous_image)
      if (plotBVHChannels):
           cv2.imshow('MocapNET 4 using MoveNET Holistic Motion History',plotImage) 
 
      if cv2.waitKey(1) & 0xFF == 27:
         break
 
    previous_image = next_image  

  cap.release()

  if (saveVideo): #            
                                    #  1280x720 by default
     os.system("ffmpeg -framerate 30 -i colorFrame_0_%%05d.jpg -s %ux%u  -y -r 30 -pix_fmt yuv420p -threads 8 livelastRun3DHiRes.mp4 && rm colorFrame_0_*.jpg " % (videoWidth,videoHeight)) # 
     if (plotBVHChannels):
        os.system("ffmpeg -framerate 30 -i plotFrame_0_%05d.jpg -s 1200x720  -y -r 30 -pix_fmt yuv420p -threads 8 livelastPlot3DHiRes.mp4 && rm plotFrame_0_*.jpg")
     




if __name__ == '__main__':
  doSerialRun = True
  import sys
  if (len(sys.argv)>1):
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--mt"):
               doSerialRun = False
               runPoseNETParallel()

  if (doSerialRun):
      runPoseNETSerial()

