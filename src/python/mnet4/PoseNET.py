#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

#https://tfhub.dev/google/movenet/singlepose/lightning/4
#https://tfhub.dev/google/lite-model/movenet/singlepose/lightning/tflite/int8/4
#wget -q -O lite-model_movenet_singlepose_lightning_tflite_int8_4.tflite https://storage.googleapis.com/tfhub-lite-models/google/lite-model/movenet/singlepose/lightning/tflite/int8/4.tflite

#zip movenet.zip movenet/* movenet/*/*

trainingWidth=1920
trainingHeight=1080

def img_resizeWithPadding(img,targetWidth,targetHeight):
    import cv2
    width  = targetWidth
    height = targetHeight 
    h, w = img.shape[:2]
    pad_bottom, pad_right = 0, 0
    ratio = w / h

    if h > height or w > width:
        # shrinking image algorithm
        interp = cv2.INTER_AREA
    else:
        # stretching image algorithm
        interp = cv2.INTER_CUBIC

    w = width
    h = round(w / ratio)
    if h > height:
        h = height
        w = round(h * ratio)
    pad_top    = int(abs(height - h)/2)
    pad_bottom = int(abs(height - h)/2)
    pad_left   = int(abs(width - w)/2)
    pad_right  = int(abs(width - w)/2)

    scaled_img = cv2.resize(img, (w, h), interpolation=interp)
    padded_img = cv2.copyMakeBorder(scaled_img,pad_top,pad_bottom,pad_left,pad_right,borderType=cv2.BORDER_CONSTANT,value=[0,0,0])
    return padded_img


def img_resizeWithCrop(img, targetWidth,targetHeight):
    import cv2
    import numpy as np
    interpolation=cv2.INTER_AREA
    h, w = img.shape[:2]
    min_size = np.amin([h,w])

    # Centralize and crop
    crop_img = img[int(h/2-min_size/2):int(h/2+min_size/2), int(w/2-min_size/2):int(w/2+min_size/2)]
    resized = cv2.resize(crop_img, (targetWidth,targetHeight), interpolation=interpolation)
    return resized

def normalizedCoordinatesAdaptForVerticalImage(sourceWidth,sourceHeight,targetWidth,targetHeight,nX,nY): 
    import numpy as np
    h = sourceHeight
    w = sourceWidth
    min_size = np.amin([h,w])
    
    nY = int(h/2-min_size/2) + int(nY*min_size)
    nX = int(w/2-min_size/2) + int(nX*min_size)

    return float(nX/sourceWidth),float(nY/sourceHeight)



def normalizedCoordinatesAdaptToResizedCrop(sourceWidth,sourceHeight,trainingWidth,trainingHeight,nX,nY):
    if (sourceHeight>sourceWidth):
        print("PoseNET.py normalizedCoordinatesAdaptToResizedCrop: FIX VERTICAL IMAGE")
        import numpy as np
    
    return nX,nY


def getPoseNETBodyNameList():
 #nose, left eye, right eye, left ear, right ear, left shoulder, right shoulder, left elbow, right elbow, left wrist, right wrist, left hip, right hip, left knee, right knee, left ankle, right ankle
 bn=list()
 #---------------------------------------------------
 bn.append("head")              #0  - nose
 bn.append("endsite_eye.l")     #1  - left_eye  eye.l endsite_eye.l
 bn.append("endsite_eye.r")     #2  - right_eye eye.r endsite_eye.r
 bn.append("lear")              #3  - __temporalis02.l - left_ear ear.l
 bn.append("rear")              #4  - __temporalis02.r  right_ear ear.r
 bn.append("lshoulder")         #5  - left_shoulder
 bn.append("rshoulder")         #6  - right_shoulder
 bn.append("lelbow")            #7  - left_elbow
 bn.append("relbow")            #8  - right_elbow
 bn.append("lhand")             #9  - left_wrist
 bn.append("rhand")             #10 - right_wrist
 bn.append("lhip")              #11 - left_hip
 bn.append("rhip")              #12 - right_hip
 bn.append("lknee")             #13 - left_knee
 bn.append("rknee")             #14 - right_knee
 bn.append("lfoot")             #15 - left_ankle
 bn.append("rfoot")             #16 - right_ankle
 return bn
#---------------------------------------------------

def getBody25NameList():
 bn=list()
 #---------------------------------------------------
 bn.append("head")              #0  
 bn.append("neck")              #1  
 bn.append("rshoulder")         #2  
 bn.append("relbow")            #3  
 bn.append("rhand")             #4  
 bn.append("lshoulder")         #5  
 bn.append("lelbow")            #6  
 bn.append("lhand")             #7  
 bn.append("hip")               #8  
 bn.append("rhip")              #9  
 bn.append("rknee")             #10 
 bn.append("rfoot")             #11 
 bn.append("lhip")              #12 
 bn.append("lknee")             #13 
 bn.append("lfoot")             #14 
 bn.append("endsite_eye.r")     #15  eye.r endsite_eye.r 
 bn.append("endsite_eye.l")     #16  eye.l endsite_eye.l
 bn.append("lear")              #17 __temporalis02.l ear.l
 bn.append("rear")              #18 __temporalis02.r ear.r
 bn.append("endsite_toe1-2.l")  #19
 bn.append("endsite_toe5-3.l")  #20
 bn.append("lheel")             #21
 bn.append("endsite_toe1-2.r")  #22
 bn.append("endsite_toe5-3.r")  #23
 bn.append("rheel")             #24
 bn.append("bkg")               #25 
 return bn
#---------------------------------------------------

doFrameDumpingForTiles=0
dumpedFrameForTiles=0

def dumpPoseNETInputTile(pose2D):
  global dumpedFrameForTiles
  jointLabels = getBody25NameList()
  f = open("Input2DTile_%u.json" % dumpedFrameForTiles, "w")
  f.write("{\n")
  f.write("\"data\":[\n")
  
  i=0
  for joint in jointLabels:
    if (i!=0): 
       f.write(",")
    #------------------------
    if (('2dx_'+joint in pose2D) and ('2dy_'+joint in pose2D)  and ('visible_'+joint in pose2D)):    
       f.write(str(pose2D['2dx_'+joint]))
       f.write(",")
       f.write(str(pose2D['2dy_'+joint]))
       f.write(",")
       f.write(str(pose2D['visible_'+joint]))
    else:
       f.write("0,0,0")
    #------------------------
    i=i+1
  f.write("\n] }\n")


  f.close()
  dumpedFrameForTiles=dumpedFrameForTiles+1

def drawPoseNETLandmarks(predictions,image,threshold=0.25):
        import cv2
        sourceWidth  = image.shape[1]
        sourceHeight = image.shape[0]
        width  = image.shape[1]
        height = image.shape[0]
        jointLabels = getPoseNETBodyNameList() # getBody25NameList()
        jID = 0
        for joint in predictions:
            #print("Joint ",joint) 
            y2D = int(joint[0]*sourceHeight) 
            x2D = int(joint[1]*sourceWidth)
            vis2D = float(joint[2])
            color=(0,255,255)
            if (threshold>vis2D):
                color=(0,0,255)
  
            cv2.circle(image,(x2D,y2D),2,color)

            font = cv2.FONT_HERSHEY_SIMPLEX 
            org = (x2D,y2D)
            fontScale = 0.4
            thickness = 1
            message =  '%s|%0.4f' % (jointLabels[jID],vis2D) 
            image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
            jID += 1
        return image


def processPoseNETLandmarks(correctLabels,poseNETPose,currentAspectRatio,trainedAspectRatio,threshold=0.15,doFlipX=False):
   itemNumber     = 0
   mnetPose2D     = dict()
   aspectRatioFix = trainedAspectRatio / currentAspectRatio
   if poseNETPose is not None:
     for item in poseNETPose:
        thisLandmarkName = correctLabels[itemNumber].lower() 
        if (thisLandmarkName!=''):
          confidence = float(item[2])          
          #------------------------------- 
          x2D     = 0.0
          y2D     = 0.0
          visible = 0.0
          #------------------------------- 
          if (confidence>threshold):
             #print("Confidence ",confidence," > ",threshold)
             #First of all for some reason mediapipe images get flipped so we undo this
             if (doFlipX):
                x2D = float(1.0-item[1]) #Do Flip X
             else:
                x2D = float(item[1]) #Dont Flip X
             #second the camera used is different that the trained one so we need to fix the aspect ratio
             #x2D = trainedAspectRatio * ( x2D / currentAspectRatio)
             x2D = aspectRatioFix * x2D
             y2D = float(item[0])
             if (x2D==0) and (y2D==0):
               visible=0.0
             else:
               visible=1.0

          #------------------------------- 


          if (x2D<0) or (x2D>1.0) or (y2D<0) or (y2D>1.0):
              vis=0.0 #Update visibility on the fly

          #------------------------------- 
          labelX = "2dx_"+thisLandmarkName
          mnetPose2D[labelX]=x2D # <- we store the corrected 2D point
          labelY = "2dy_"+thisLandmarkName
          mnetPose2D[labelY]=y2D
          labelV = "visible_"+thisLandmarkName
          mnetPose2D[labelV]=visible
          if (x2D>1.0):
             print("Normalization Error(!) | Joint ",thisLandmarkName,"(",itemNumber,") x=",x2D," y=",y2D," v=",visible)
        itemNumber = itemNumber +1

   if ("2dx_rshoulder" in mnetPose2D) and ("2dy_rshoulder" in mnetPose2D) and ("visible_rshoulder" in mnetPose2D) and ("2dx_lshoulder" in mnetPose2D) and ("2dy_lshoulder" in mnetPose2D) and ("visible_lshoulder" in mnetPose2D) :
              #---------------------------------------------
              rX = float(mnetPose2D["2dx_rshoulder"])
              rY = float(mnetPose2D["2dy_rshoulder"])
              rV = float(mnetPose2D["visible_rshoulder"])
              #---------------------------------------------
              lX = float(mnetPose2D["2dx_lshoulder"])
              lY = float(mnetPose2D["2dy_lshoulder"])
              lV = float(mnetPose2D["visible_lshoulder"])
              #---------------------------------------------
              if (rV>0.0) and (lV>0.0):
                 mnetPose2D["2dx_neck1"]     = (rX+lX)/2
                 mnetPose2D["2dy_neck1"]     = (rY+lY)/2
                 mnetPose2D["visible_neck1"] = 1.0

   if (('visible_rhip' in mnetPose2D) and ('visible_lhip' in mnetPose2D)):
     if (float(mnetPose2D["visible_rhip"])>0.0 and float(mnetPose2D["visible_lhip"])>0.0):
        mnetPose2D["2dx_hip"]=(float(mnetPose2D["2dx_rhip"])+float(mnetPose2D["2dx_lhip"]))/2.0
        mnetPose2D["2dy_hip"]=(float(mnetPose2D["2dy_rhip"])+float(mnetPose2D["2dy_lhip"]))/2.0
        mnetPose2D["visible_hip"]=1.0

   if (('visible_rshoulder' in mnetPose2D) and ('visible_lshoulder' in mnetPose2D)):
     if (float(mnetPose2D["visible_rshoulder"])>0.0 and float(mnetPose2D["visible_lshoulder"])>0.0):
        mnetPose2D["2dx_neck"]=(float(mnetPose2D["2dx_rshoulder"])+float(mnetPose2D["2dx_lshoulder"]))/2.0
        mnetPose2D["2dy_neck"]=(float(mnetPose2D["2dy_rshoulder"])+float(mnetPose2D["2dy_lshoulder"]))/2.0
        mnetPose2D["visible_neck"]=1.0

   #-----------------------------------------------------------------
   if ('visible_rfoot' in mnetPose2D)  :
     mnetPose2D["2dx_endsite_toe1-2.r"]=mnetPose2D["2dx_rfoot"]
     mnetPose2D["2dy_endsite_toe1-2.r"]=mnetPose2D["2dy_rfoot"]
     mnetPose2D["visible_endsite_toe1-2.r"]=mnetPose2D["visible_rfoot"]
     mnetPose2D["2dx_endsite_toe5-3.r"]=mnetPose2D["2dx_rfoot"]
     mnetPose2D["2dy_endsite_toe5-3.r"]=mnetPose2D["2dy_rfoot"]
     mnetPose2D["visible_endsite_toe5-3.r"]=mnetPose2D["visible_rfoot"]
   #-----------------------------------------------------------------
   if ('visible_lfoot' in mnetPose2D)  :
     mnetPose2D["2dx_endsite_toe1-2.l"]=mnetPose2D["2dx_lfoot"]
     mnetPose2D["2dy_endsite_toe1-2.l"]=mnetPose2D["2dy_lfoot"]
     mnetPose2D["visible_endsite_toe1-2.l"]=mnetPose2D["visible_lfoot"]
     mnetPose2D["2dx_endsite_toe5-3.l"]=mnetPose2D["2dx_lfoot"]
     mnetPose2D["2dy_endsite_toe5-3.l"]=mnetPose2D["2dy_lfoot"]
     mnetPose2D["visible_endsite_toe5-3.l"]=mnetPose2D["visible_lfoot"]
    
   #Deactivate to stop dumping tiles
   global doFrameDumpingForTiles
   if (doFrameDumpingForTiles==1):
     dumpPoseNETInputTile(mnetPose2D)

   return mnetPose2D
#---------------------------------------------------
 


class PoseNETTFLite():
  def __init__(
               self,
               modelPath:str="movenet/lite-model_movenet_singlepose_lightning_tflite_int8_4.tflite",
              ):
               #Tensorflow attempt to be reasonable
               #------------------------------------------
               self.jointNames = getPoseNETBodyNameList()
               #------------------------------------------
               import tensorflow as tf
               # Initialize the TFLite interpreter
               self.interpreter = tf.lite.Interpreter(model_path=modelPath)
               self.interpreter.allocate_tensors()
               #------------------------------------------
               self.output     = dict()
               self.hz         = 0.0
               #------------------------------------------

  def get2DOutput(self):
        return self.output

  def convertImageToMocapNETInput(self,image,doFlipX=False,threshold=0.25):
        import tensorflow as tf
        import numpy as np
        import time
        import cv2
        sourceWidth  = image.shape[1]
        sourceHeight = image.shape[0]
        targetWidth  = 192
        targetHeight = 192
        currentAspectRatio=targetWidth/targetHeight
        trainedAspectRatio=trainingWidth/trainingHeight

        #Do resize on OpenCV end
        #----------------------------------------------------------------
        imageTransformed = img_resizeWithCrop(image,192,192)
        #imageTransformed = img_resizeWithPadding(image,192,192)
        imageTransformed = cv2.cvtColor(imageTransformed,cv2.COLOR_BGR2RGB)
        #----------------------------------------------------------------

        #Prepare image for Tensorflow
        #----------------------------------------------------------------
        imageTF     = np.expand_dims(imageTransformed, axis=0).astype('int32')
        #----------------------------------------------------------------

        # TF Lite format expects tensor type of float32.
        input_image    = tf.cast(imageTF, dtype=tf.uint8) # tf.float32
        input_details  = self.interpreter.get_input_details()
        output_details = self.interpreter.get_output_details()
        #-------------------------------------------------------------------


        start = time.time()
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        self.interpreter.set_tensor(input_details[0]['index'], input_image.numpy())
        self.interpreter.invoke()
 
        keypoints_with_scores = self.interpreter.get_tensor(output_details[0]['index']) # Output is a [1, 1, 17, 3] numpy array.
        predictions = keypoints_with_scores[0][0]
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        seconds = time.time() - start
        self.hz  = 1 / (seconds+0.0001)
        print("MoveNET TFLite Framerate : ",round(self.hz,2)," fps          ")

        
        currentAspectRatio=sourceWidth/sourceHeight #We "change" aspect ratio by restoring points
        for pointID in range(0,len(predictions)):
           #Joints have y,x,acc order
           nX,nY = normalizedCoordinatesAdaptToResizedCrop(sourceWidth,sourceHeight,targetWidth,targetHeight,predictions[pointID][1],predictions[pointID][0])
           nX,nY = normalizedCoordinatesAdaptForVerticalImage(sourceWidth,sourceHeight,trainingWidth,trainingHeight,nX,nY)
           predictions[pointID][1]=nX
           predictions[pointID][0]=nY

        self.output = processPoseNETLandmarks(self.jointNames,predictions,currentAspectRatio,trainedAspectRatio,threshold=threshold,doFlipX=doFlipX)
        #------------------------------------------------
        self.image = drawPoseNETLandmarks(predictions,image,threshold=threshold)
        #------------------------------------------------

        return self.output,image
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------





class PoseNETONNX():
  def __init__(
               self,
               modelPath:str="movenet/model.onnx",
              ):
               #Tensorflow attempt to be reasonable
               #------------------------------------------
               self.jointNames = getPoseNETBodyNameList()
               #------------------------------------------
               import onnxruntime as ort
               import onnx
               onnxModelForCheck  = onnx.load(modelPath)
               onnx.checker.check_model(onnxModelForCheck)
               print("ONNX devices available : ", ort.get_device()) 
               providers               = ['CPUExecutionProvider']
               #providers               = ['CUDAExecutionProvider']
               self.options = ort.SessionOptions()
               self.model              = ort.InferenceSession(modelPath, providers=providers, sess_options=self.options)
               for i in range(0,len(self.model.get_inputs())): 
                  print("ONNX INPUTS ",self.model.get_inputs()[i].name)
                  self.inputName = self.model.get_inputs()[i].name

               self.model_input_name   = self.model.get_inputs()
               #------------------------------------------
               self.output     = dict()
               self.hz         = 0.0
               #------------------------------------------

  def get2DOutput(self):
        return self.output

  def convertImageToMocapNETInput(self,image,doFlipX=False,threshold=0.25):
        import tensorflow as tf
        import numpy as np
        import time
        import cv2
        sourceWidth  = image.shape[1]
        sourceHeight = image.shape[0]
        targetWidth  = 192
        targetHeight = 192
        currentAspectRatio=targetWidth/targetHeight
        trainedAspectRatio=trainingWidth/trainingHeight
        #Do resize on OpenCV end
        #----------------------------------------------------------------
        imageTransformed = img_resizeWithCrop(image,targetWidth,targetHeight)
        #imageTransformed = img_resizeWithPadding(image,targetWidth,targetHeight)
        imageTransformed = cv2.cvtColor(imageTransformed,cv2.COLOR_BGR2RGB)
        #----------------------------------------------------------------

        #Hand image to Tensorflow
        #----------------------------------------------------------------
        imageONNX = np.expand_dims(imageTransformed, axis=0)
        #-------------------------------------------------------------------


        start = time.time()
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        thisInputONNX = { self.inputName : imageONNX.astype('int32')}
        #Run input through MocapNET
        output_names_onnx = [otp.name for otp in self.model.get_outputs()]
        keypoints_with_scores = self.model.run(output_names_onnx,thisInputONNX)[0][0]
        predictions = keypoints_with_scores[0]
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        seconds = time.time() - start
        self.hz  = 1 / (seconds+0.0001)
        print("MoveNET ONNX Framerate : ",round(self.hz,2)," fps          ")
        

        currentAspectRatio=sourceWidth/sourceHeight #We "change" aspect ratio by restoring points
        for pointID in range(0,len(predictions)):
           #Joints have y,x,acc order
           nX,nY = normalizedCoordinatesAdaptToResizedCrop(sourceWidth,sourceHeight,targetWidth,targetHeight,predictions[pointID][1],predictions[pointID][0])
           nX,nY = normalizedCoordinatesAdaptForVerticalImage(sourceWidth,sourceHeight,trainingWidth,trainingHeight,nX,nY)
           predictions[pointID][1]=nX
           predictions[pointID][0]=nY

        self.output = processPoseNETLandmarks(self.jointNames,predictions,currentAspectRatio,trainedAspectRatio,threshold=threshold,doFlipX=doFlipX)
        #----------------------------------------------------------------
        self.image = drawPoseNETLandmarks(predictions,image,threshold=threshold)
        #----------------------------------------------------------------

        return self.output,image
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
  





class PoseNET():
  def __init__(
               self,
               modelPath:str="movenet/",
              ):
               #Tensorflow attempt to be reasonable
               #------------------------------------------
               self.jointNames = getPoseNETBodyNameList()
               import tensorflow as tf
               self.model      = tf.saved_model.load(modelPath)
               self.movenet    = self.model.signatures['serving_default']
               #------------------------------------------
               self.output     = dict()
               self.hz         = 0.0
               #------------------------------------------
  def get2DOutput(self):
        return self.output

  def convertImageToMocapNETInput(self,image,doFlipX=False,threshold=0.25):
        import tensorflow as tf
        import numpy as np
        import time
        import cv2
        sourceWidth  = image.shape[1]
        sourceHeight = image.shape[0]
        targetWidth  = 192
        targetHeight = 192
        currentAspectRatio=targetWidth/targetHeight
        trainedAspectRatio=trainingWidth/trainingHeight
        #Do resize on OpenCV end
        #----------------------------------------------------------------
        imageTransformed = img_resizeWithCrop(image,192,192)
        #imageTransformed = img_resizeWithPadding(image,192,192)
        imageTransformed = cv2.cvtColor(imageTransformed,cv2.COLOR_BGR2RGB)
        #----------------------------------------------------------------

        #Prepare image for Tensorflow
        #----------------------------------------------------------------
        imageTF     = np.expand_dims(imageTransformed, axis=0).astype('int32')
        #----------------------------------------------------------------



        start = time.time()
        # TF Lite format expects tensor type of float32.
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        outputs = self.movenet(tf.cast(imageTF, dtype=tf.int32)) 
        keypoints_with_scores = outputs['output_0']
        predictionsRaw = keypoints_with_scores[0][0]
        #-------------------------------------------------------------------
        #-------------------------------------------------------------------
        seconds = time.time() - start
        self.hz  = 1 / (seconds+0.0001)
        print("MoveNET Framerate : ",round(self.hz,2)," fps          ")


        currentAspectRatio=sourceWidth/sourceHeight #We "change" aspect ratio by restoring points
        predictions=list()
        for pointID in range(0,len(predictionsRaw)):
           #Joints have y,x,acc order
           thisPoint = list()
           thisPoint.append(float(predictionsRaw[pointID][0])) #y
           thisPoint.append(float(predictionsRaw[pointID][1])) #x
           thisPoint.append(float(predictionsRaw[pointID][2])) #score
           nX,nY = normalizedCoordinatesAdaptToResizedCrop(sourceWidth,sourceHeight,targetWidth,targetHeight,thisPoint[1],thisPoint[0])
           nX,nY = normalizedCoordinatesAdaptForVerticalImage(sourceWidth,sourceHeight,trainingWidth,trainingHeight,nX,nY)
           thisPoint[0]= nY #Just update coords
           thisPoint[1]= nX #Just update coords
           predictions.append(thisPoint)
        #-------------------------------------------------------------------

        self.output = processPoseNETLandmarks(self.jointNames,predictions,currentAspectRatio,trainedAspectRatio,threshold=threshold,doFlipX=doFlipX)

        #Actually see what was handed to the tensorflow stuff..!
        #-----------------------------------------------------------------
        #import matplotlib.pyplot as plt
        #plt.title("TensorFlow Logo with shape {}".format(imageTF.shape))
        #plt.imshow(tf.squeeze(imageTF))
        #plt.show(block = False)
        #plt.pause(0.001)
        #-----------------------------------------------------------------

        self.image = drawPoseNETLandmarks(predictions,image,threshold=threshold)        
        #------------------------------------------------

        return self.output,image
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
  
 



def runPoseNETSerial(): 
  #Parse command line arguments
  #-----------------------------------------
  import sys
  import cv2
  import time
  headless             = False
  economicVisualization= False
  videoFilePath        = "webcam" 
  videoWidth           = 1280
  videoHeight          = 720
  doProfiling          = False
  doFlipX              = False
  engine               = "onnx"
  doNNEveryNFrames     = 4 # 3 
  bvhScale             = 1.0
  doHCDPostProcessing  = 1 
  hcdLearningRate      = 0.001
  hcdEpochs            = 99
  hcdIterations        = 99
  threshold            = 0.25
  calibrationFile      = ""
  plotBVHChannels      = False
  bvhAnglesForPlotting = list()
  bvhAllAnglesForPlotting = list()


  if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--nnsubsample"):
               doNNEveryNFrames    = int(sys.argv[i+1])
           if (sys.argv[i]=="--headless"):
              headless = True
           if (sys.argv[i]=="--flipx"):
              doFlipX = True
           if (sys.argv[i]=="--nonn"):
              doNNEveryNFrames = 1000
           if (sys.argv[i]=="--calib"):
              calibrationFile = sys.argv[i+1]
           if (sys.argv[i]=="--ik"):
               hcdLearningRate     = float(sys.argv[i+1])
               hcdEpochs           = int(sys.argv[i+2])
               hcdIterations       = int(sys.argv[i+3])
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
  #-----------------------------------------
  #python3 -m tf2onnx.convert --saved-model movenet --opset 14 --output movenet/model.onnx
  #zip movenet.zip movenet/* movenet/*/*
  #-----------------------------------------
  
  bvhAnglesForPlotting = list()
 
  # Initialize the PoseNET 
  if (engine=="tensorflow"):
      poseNET = PoseNET(modelPath="movenet/") 
  elif (engine=="onnx"):
      poseNET = PoseNETONNX(modelPath="movenet/model.onnx") 
  elif (engine=="tflite"):
      poseNET = PoseNETTFLite()
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
    print("\r PoseNET+MocepNET aggregate Framerate : ",round(fps,2)," fps           \r", end="", flush=True)
    print("\n", end="", flush=True)
  
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
    

        
    if not headless:
      cv2.imshow('MocapNET 4 using PoseNET Holistic 2D Joints', annotated_image)
      if (plotBVHChannels):
           cv2.imshow('MocapNET 4 using PoseNET Holistic Motion History',plotImage) 
 
      if cv2.waitKey(1) & 0xFF == 27:
         break


  cap.release()




def runPoseNETParallel(): 
  #Parse command line arguments
  #-----------------------------------------
  import sys
  import cv2
  import time
  import threading

  headless         = False
  videoFilePath    = "webcam" 
  doProfiling      = False
  doFlipX          = False
  engine           = "onnx"
  doNNEveryNFrames = 3
  plotBVHChannels  = 1
  bvhScale         = 1.0
  threshold        = 0.25


  if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--headless"):
              headless = True
           if (sys.argv[i]=="--flipx"):
              doFlipX = True
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
  if (videoFilePath=="esp"):
     from espStream import ESP32CamStreamer
     cap = ESP32CamStreamer()
  elif (videoFilePath=="webcam"):
     cap = cv2.VideoCapture(0)
     cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
     cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
  else:
     cap = cv2.VideoCapture(videoFilePath)
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
      poseNET = PoseNET(modelPath="movenet/") 
  elif (engine=="onnx"):
      poseNET = PoseNETONNX(modelPath="movenet/model.onnx") 
  elif (engine=="tflite"):
      poseNET = PoseNETTFLite()
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
    visualizeMocapNETEnsemble(mnet,previous_image,plotBVHChannels=plotBVHChannels,bvhAnglesForPlotting=bvhAnglesForPlotting,economic=True)
    #------------------------------------------------------------------------------------

    mocapNETInput    = poseNET.output
    next_image       = poseNET.image
    #------------------------------------------------------------------------------------
    seconds = time.time() - start
    fps  = 1 / (seconds+0.0001)
    print("\r MoveNET+MocepNET MT aggregate Framerate : ",round(fps,2)," fps           \r", end="", flush=True)
    print("\n", end="", flush=True)
  

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


    if not headless:
      cv2.imshow('MocapNET 4 using MoveNET Holistic 2D Joints', previous_image)
      if (plotBVHChannels):
           cv2.imshow('MocapNET 4 using MoveNET Holistic Motion History',plotImage) 
 
      if cv2.waitKey(1) & 0xFF == 27:
         break

 
    previous_image = next_image  
  cap.release()











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
    #



