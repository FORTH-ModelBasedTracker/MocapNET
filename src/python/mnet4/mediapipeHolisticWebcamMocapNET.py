#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

#pip install mediapipe pandas pillow matplotlib opencv-python

import cv2
import mediapipe as mp
import numpy as np
import time
import sys
import os

from readCSV  import parseConfiguration,zeroOutXYJointsThatAreInvisible,performNSRMAlignment
from NSDM     import NSDMLabels,createNSDMUsingRules
from tools    import secondsToHz,eprint
from MocapNET import MocapNET

mp_drawing   = mp.solutions.drawing_utils
mp_holistic  = mp.solutions.holistic
mp_face_mesh = mp.solutions.face_mesh
#------------------------------------------------------------------------------------------------
LEFT_EYE     = [362, 382, 381, 380, 374, 373, 390, 249, 263, 466, 388, 387, 386, 385,384, 398 ]
LEFT_IRIS    = [474,475, 476, 477]
RIGHT_EYE    = [33, 7, 163, 144, 145, 153, 154, 155, 133, 173, 157, 158, 159, 160, 161 , 246 ] 
RIGHT_IRIS   = [469, 470, 471, 472]
#------------------------------------------------------------------------------------------------

#I have added a seperate list with the joints
from holisticPartNames import  getHolisticBodyNameList, getHolisticFaceNameList, getHolisticLHandNameList, getHolisticRHandNameList, processPoseLandmarks, guessLandmarks
#------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MEDIAPIPE_BODY_LANDMARK_NAMES  = getHolisticBodyNameList()
MEDIAPIPE_FACE_LANDMARK_NAMES  = getHolisticFaceNameList()
MEDIAPIPE_LHAND_LANDMARK_NAMES = getHolisticLHandNameList()
MEDIAPIPE_RHAND_LANDMARK_NAMES = getHolisticRHandNameList()
#------------------------------------------------------------------------------------------------------------------------------------------------------------------------


def embed_to_1920x1080(img, canvas_w=1920, canvas_h=1080):
    if img is None:
      return None
    h, w = img.shape[:2]

    # Scale to fit inside canvas while preserving aspect ratio
    scale = min(canvas_w / w, canvas_h / h)
    new_w = int(w * scale)
    new_h = int(h * scale)

    resized = cv2.resize(img, (new_w, new_h), interpolation=cv2.INTER_AREA)

    # Create black canvas
    if len(img.shape) == 2:
        canvas = np.zeros((canvas_h, canvas_w), dtype=img.dtype)
    else:
        canvas = np.zeros((canvas_h, canvas_w, img.shape[2]), dtype=img.dtype)

    # Center the resized image
    x_offset = (canvas_w - new_w) // 2
    y_offset = (canvas_h - new_h) // 2

    canvas[y_offset:y_offset + new_h, x_offset:x_offset + new_w] = resized
    return canvas

class MediaPipePose():
  def __init__(self,doMediapipeVisualization = False):
               #Tensorflow attempt to be reasonable
               #------------------------------------------
               self.mp         = mp_holistic.Holistic(static_image_mode=True)
               #------------------------------------------
               self.doMediapipeVisualization = doMediapipeVisualization
               self.output     = dict()
               self.mp_drawing = mp.solutions.drawing_utils
               self.mp_drawing_styles = mp.solutions.drawing_styles
               self.mp_pose = mp.solutions.pose
               self.pose = self.mp_pose.Pose(static_image_mode=True,smooth_landmarks=True,model_complexity=0,enable_segmentation=True,min_detection_confidence=0.5)
               #------------------------------------------
  def get2DOutput(self):
        return self.output

  def processIncomingBodyOnlyLandmarks(self,body,currentAspectRatio,trainedAspectRatio):
    #This function runs ~10000Hz
    #----------------------------------------------- 
    mnetPose2D = dict()
    #------------------------------------------------------------------------------------
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_BODY_LANDMARK_NAMES ,body,currentAspectRatio,trainedAspectRatio)
    #------------------------------------------------------------------------------------
    guessLandmarks(mnetPose2D) #Some landmarks ( neck, hip need to be guessed by others )
    #-----------------------------------------------
    return mnetPose2D

  def processIncomingAllLandmarks(self,landmarks,currentAspectRatio,trainedAspectRatio):
    #This function runs ~10000Hz
    #----------------------------------------------- 
    mnetPose2D = dict()
    #------------------------------------------------------------------------------------
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_BODY_LANDMARK_NAMES ,landmarks.pose_landmarks,currentAspectRatio,trainedAspectRatio)
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_LHAND_LANDMARK_NAMES,landmarks.left_hand_landmarks,currentAspectRatio,trainedAspectRatio,threshold=0.001)
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_RHAND_LANDMARK_NAMES,landmarks.right_hand_landmarks,currentAspectRatio,trainedAspectRatio,threshold=0.001)
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_FACE_LANDMARK_NAMES ,landmarks.face_landmarks,currentAspectRatio,trainedAspectRatio)
    #------------------------------------------------------------------------------------
    guessLandmarks(mnetPose2D) #Some landmarks ( neck, hip need to be guessed by others )
    #-----------------------------------------------
    return mnetPose2D

  def convertImageToMocapNETInput(self,image):
    #holistic = self.mp
    if (type(image)==type(None)):
       print("Invalid Image given, can't do anything with it")
       return dict() , image

    width  = image.shape[1]
    height = image.shape[0]
    if ((width==0) or (height==0)):
       print("Cannot work with empty image")
       return dict() , image    

    #Try to speed up by scaling down image
    #if ( (width>1024) or (height>720) ):
    #   width  = int(image.shape[1]/2)
    #   height = int(image.shape[0]/2)
    #   image  = cv2.resize(image, (width,height))

    #The aspect ratios involved
    currentAspectRatio = width/height
    trainedAspectRatio = 1920/1080
    #-----------------------------------------------

    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    mocapNETInput = dict() 
    if (True):
    # with mp_holistic.Holistic(static_image_mode=True) as holistic:
              start = time.time()
              #-----------------------------------------------
              results = self.pose.process(image)
              #results = holistic.process(image)
              #Draw the pose annotation on the image.
              image.flags.writeable = True
              image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

              if (self.doMediapipeVisualization):
             
                try:
                     self.mp_drawing.draw_landmarks(annotated_image, results.face_landmarks      , mp_holistic.FACEMESH_TESSELATION) #This used to be called FACE_CONNECTIONS
                except:
                     self.mp_drawing.draw_landmarks(annotated_image, results.face_landmarks      , mp_holistic.FACE_CONNECTIONS) #This used to be called FACE_CONNECTIONS

                self.mp_drawing.draw_landmarks(image, results.left_hand_landmarks,  mp_holistic.HAND_CONNECTIONS)
                self.mp_drawing.draw_landmarks(image, results.right_hand_landmarks, mp_holistic.HAND_CONNECTIONS)
                self.mp_drawing.draw_landmarks(
                                         image,
                                         results.pose_landmarks,
                                         self.mp_pose.POSE_CONNECTIONS,
                                         landmark_drawing_spec=self.mp_drawing_styles.get_default_pose_landmarks_style()
                                       )

              #Body only
              #-----------------------------------------------
              mocapNETInput    = self.processIncomingBodyOnlyLandmarks(results.pose_landmarks,currentAspectRatio,trainedAspectRatio)
              #mocapNETInput    = self.processIncomingAllLandmarks(results,currentAspectRatio,trainedAspectRatio)
              #-----------------------------------------------
              
              end = time.time()
              # Time elapsed
              seconds = end - start
              if (seconds==0.0):
                       seconds=1.0
              # Calculate frames per second
              fps  = 1 / seconds
              #print("Mediapipe Pose Only 2D Joint Estimation Framerate : ",round(fps,2)," fps \n", end="", flush=True)


    self.output = mocapNETInput
    return mocapNETInput,image



#------------------------------------------------
#------------------------------------------------
#------------------------------------------------
class MediaPipeHolistic():
  def __init__(self,doMediapipeVisualization = False):
               #Tensorflow attempt to be reasonable
               #------------------------------------------
               self.mp         = mp_holistic.Holistic(
                                                        static_image_mode=False,
                                                        model_complexity=1, #1 = default complexity, 2 = high complexity
                                                        min_detection_confidence=0.2,
                                                        min_tracking_confidence=0.2
                                                     )
               #------------------------------------------
               #self.mpHands    = mp.solutions.hands.Hands( 
               #                                            min_detection_confidence=0.5,
               #                                            min_tracking_confidence=0.5
               #                                          )
               #------------------------------------------
               self.mpFace     = mp_face_mesh.FaceMesh(
                                                        max_num_faces=1,
                                                        refine_landmarks=True,
                                                        min_detection_confidence=0.3,
                                                        min_tracking_confidence=0.3
                                                      )
               #------------------------------------------
               self.doMediapipeVisualization = doMediapipeVisualization
               self.output     = dict()
               #------------------------------------------
  def get2DOutput(self):
        return self.output

  def processIncomingHolisticLandmarks(self,face,lhand,rhand,body,currentAspectRatio,trainedAspectRatio):
    mnetPose2D = dict()
    #------------------------------------------------------------------------------------
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_BODY_LANDMARK_NAMES ,body ,currentAspectRatio,trainedAspectRatio,useVisibility=False)
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_LHAND_LANDMARK_NAMES,lhand,currentAspectRatio,trainedAspectRatio,useVisibility=False)
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_RHAND_LANDMARK_NAMES,rhand,currentAspectRatio,trainedAspectRatio,useVisibility=False)
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_FACE_LANDMARK_NAMES ,face ,currentAspectRatio,trainedAspectRatio,useVisibility=False)
    #------------------------------------------------------------------------------------
    guessLandmarks(mnetPose2D) #Some landmarks ( neck, hip need to be guessed by others )
    #------------------------------------------------------------------------------------
    return mnetPose2D

  def convertImageToMocapNETInput(self,image):
    holisticEstimator  = self.mp
    faceEstimator = self.mpFace
    #handEstimator = self.mpHands

    if (type(image)==type(None)):
       print("Invalid Image given, can't do anything with it")
       return dict() , image

    #-----------------------------------------
    width  = image.shape[1]
    height = image.shape[0]
    if ((width==0) or (height==0)):
       print("Cannot work with empty image")
       return dict() , image    
    #-----------------------------------------
    currentAspectRatio = width/height
    trainedAspectRatio = 1920/1080
    #-----------------------------------------

    start = time.time()
    #--------------------------------------------------------------------------------------------------
    # To improve performance, optionally mark the image as not writeable to pass by reference.
    image.flags.writeable = False
    #==============================================
    #================= MEDIAPIPE ================== 
    #==============================================
    results  = holisticEstimator.process(image)
    resultsF = faceEstimator.process(image) #Extra Face
    #resultsH = handEstimator.process(image)
    #==============================================

    # Draw the hand annotations on the image.
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    #--------------------------------------------------------------------------------------------------
    end = time.time()
    # Time elapsed
    seconds = end - start
    if (seconds==0.0):
             seconds=1.0
    # Calculate frames per second
    fps  = 1 / seconds
    #print("Mediapipe Holistic 2D Joint Estimation Framerate : ",round(fps,2)," fps \n", end="", flush=True)

    #annotated_image = image.copy()
    annotated_image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR) #Our annotated image must be BGR to show correctly

    #--------------------------------------------------------------------------------------------------
    if (self.doMediapipeVisualization):
      #Compensate for name mediapipe change.. 
      try:
       mp_drawing.draw_landmarks(annotated_image, results.face_landmarks     , mp_holistic.FACEMESH_TESSELATION) #This used to be called FACE_CONNECTIONS
      except:
       mp_drawing.draw_landmarks(annotated_image, results.face_landmarks     , mp_holistic.FACE_CONNECTIONS) #This used to be called FACE_CONNECTIONS
      #--------------------------------------------------------------------------------------------------
      mp_drawing.draw_landmarks(annotated_image, results.left_hand_landmarks , mp_holistic.HAND_CONNECTIONS)
      mp_drawing.draw_landmarks(annotated_image, results.right_hand_landmarks, mp_holistic.HAND_CONNECTIONS)
      #--------------------------------------------------------------------------------------------------
      #for hand_landmarks in resultsH.multi_hand_landmarks:
      #  mp_drawing.draw_landmarks(annotated_image, hand_landmarks, mp_hands.HAND_CONNECTIONS)
      #--------------------------------------------------------------------------------------------------
      # Use mp_holistic.UPPER_BODY_POSE_CONNECTIONS for drawing below when upper_body_only is set to True.
      mp_drawing.draw_landmarks(annotated_image, results.pose_landmarks      , mp_holistic.POSE_CONNECTIONS)
    #--------------------------------------------------------------------------------------------------
    mocapNETInput     = self.processIncomingHolisticLandmarks(
                                                               results.face_landmarks,
                                                               results.left_hand_landmarks,
                                                               results.right_hand_landmarks,
                                                               results.pose_landmarks,
                                                               currentAspectRatio,
                                                               trainedAspectRatio
                                                             )

    #EYES
    #-------------------------------------------------------------------------------------------- 
    img_h, img_w = image.shape[:2]
    if resultsF.multi_face_landmarks:
                      mesh_points=np.array([np.multiply([p.x, p.y], [img_w, img_h]).astype(int) for p in resultsF.multi_face_landmarks[0].landmark])
                      (l_cx, l_cy), l_radius = cv2.minEnclosingCircle(mesh_points[LEFT_IRIS])
                      (r_cx, r_cy), r_radius = cv2.minEnclosingCircle(mesh_points[RIGHT_IRIS])
                      center_left  = np.array([l_cx, l_cy], dtype=np.int32)
                      center_right = np.array([r_cx, r_cy], dtype=np.int32)
                      #cv2.circle(annotated_image, center_left,  int(l_radius), (255,0,0), -1, cv2.LINE_AA)
                      #cv2.circle(annotated_image, center_right, int(r_radius), (255,0,0), -1, cv2.LINE_AA)
                      #--------------------------------------------------------
                      from holisticPartNames import normalize2DPointWhileAlsoMatchingTrainingAspectRatio
                      #--------------------------------------------------------
                      lEyex2D = float(l_cx/img_w)  #Dont Flip X
                      lEyey2D = float(l_cy/img_h)
                      lEyex2D,lEyey2D =  normalize2DPointWhileAlsoMatchingTrainingAspectRatio(lEyex2D,lEyey2D,currentAspectRatio)
                      lEyeVis = 0.0
                      if ( (lEyex2D!=0.0) or (lEyey2D!=0.0) ):
                                                               lEyeVis = 1.0
                      mocapNETInput["2dx_head_leye"]         = lEyex2D
                      mocapNETInput["2dy_head_leye"]         = lEyey2D
                      mocapNETInput["visible_head_leye"]     = lEyeVis
                      mocapNETInput["2dx_endsite_eye.l"]     = lEyex2D
                      mocapNETInput["2dy_endsite_eye.l"]     = lEyey2D
                      mocapNETInput["visible_endsite_eye.l"] = lEyeVis
                      #--------------------------------------------------------
                      rEyex2D = float(r_cx/img_w)  #Dont Flip X
                      rEyey2D = float(r_cy/img_h)
                      rEyex2D,rEyey2D =  normalize2DPointWhileAlsoMatchingTrainingAspectRatio(rEyex2D,rEyey2D,currentAspectRatio)
                      rEyeVis = 0.0
                      if ( (rEyex2D!=0.0) or (rEyey2D!=0.0) ):
                                                               rEyeVis = 1.0
                      mocapNETInput["2dx_head_reye"]         = rEyex2D
                      mocapNETInput["2dy_head_reye"]         = rEyey2D
                      mocapNETInput["visible_head_reye"]     = rEyeVis
                      mocapNETInput["2dx_endsite_eye.r"]     = rEyex2D
                      mocapNETInput["2dy_endsite_eye.r"]     = rEyey2D
                      mocapNETInput["visible_endsite_eye.r"] = rEyeVis
                      #-------------------------------------------------------- 
    else:
                      #print("UNABLE TO GET EYES")
                      pass
                      #don't populate them at all..
                      #mocapNETInput["2dx_head_leye"]     = 0.0
                      #mocapNETInput["2dy_head_leye"]     = 0.0
                      #mocapNETInput["visible_head_leye"] = 0.0
                      #mocapNETInput["2dx_head_reye"]     = 0.0
                      #mocapNETInput["2dy_head_reye"]     = 0.0
                      #mocapNETInput["visible_head_reye"] = 0.0
                      #-------------------------------------------------------- 

    #--------------------------------------------------------------------------------------------------
    #from MocapNETVisualization import drawMocapNETInput
    #drawMocapNETInput(mocapNETInput,annotated_image)
    #--------------------------------------------------------------------------------------------------
    #-------------------------------------------------------------------------------------------- 
    self.output = mocapNETInput
    #-------------------------------------------------------------------------------------------- 
    return mocapNETInput,annotated_image
#------------------------------------------------
#------------------------------------------------
#------------------------------------------------



def crop_image_around_landmarks(image, annotated_image, mocapNETInput, padding=20):
    h, w = image.shape[:2]
    xs, ys = [], []

    # Extract all 2D visible coordinates
    for key in mocapNETInput:
        if key.startswith("2dx_"):
            part_name = key.replace("2dx_", "")
            x = mocapNETInput[key]
            y_key = f"2dy_{part_name}"
            vis_key = f"visible_{part_name}"

            if y_key in mocapNETInput and vis_key in mocapNETInput:
                y = mocapNETInput[y_key]
                visible = mocapNETInput[vis_key]
                if visible > 0.5:
                    xs.append(int(x * w))
                    ys.append(int(y * h))

    if not xs or not ys:
        # No valid landmarks
        return image, annotated_image

    min_x = max(min(xs) - padding, 0)
    max_x = min(max(xs) + padding, w)
    min_y = max(min(ys) - padding, 0)
    max_y = min(max(ys) + padding, h)

    # Crop both images
    cropped_image = image[min_y:max_y, min_x:max_x]
    cropped_annotated = annotated_image[min_y:max_y, min_x:max_x]

    return cropped_image, cropped_annotated




 

#------------------------------------------------
#------------------------------------------------
#------------------------------------------------
def streamPosesFromCameraToMocapNET():
  engine           = "onnx"
  headless         = False
  doProfiling      = False
  multiThreaded    = False
  videoFilePath    = "webcam"
  videoWidth       = 1280
  videoHeight      = 720
  saveVideo        = False
  illustrate       = False
  dumpData         = False
  doBody           = True
  doUpperBody      = True
  doLowerBody      = True
  embedInputFrameToTrainingAspectRatio = True
  doFace           = False
  doREye           = False
  doMouth          = False
  doHands          = False
  aspectCorrection = 1.0
  scale            = 1.0
  addNoise         = 0.0
  doNNEveryNFrames = 0
  frameSkip        = 0
  doHCDPostProcessing = 1
  hcdLearningRate     = 0.001
  hcdEpochs           = 30
  hcdIterations       = 10
  smoothingSampling   = 30.0
  smoothingCutoff     = 5.0
  plotBVHChannels     = False
  liveDemo            = False
  alterFocalLength    = False
  calibrationFile = ""
  bvhAnglesForPlotting    = list()
  bvhAllAnglesForPlotting = list()

  #python3 -m mediapipeHolisticWebcamMocapNET --from damien.avi --face --nobody --plot --save
  #python3 -m mediapipeHolisticWebcamMocapNET --from damien.avi --nobody --face --plot --aspectCorrection 0.8 --save

  if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--headless"):
              headless = True
           if (sys.argv[i]=="--live"):
              liveDemo = True
           if (sys.argv[i]=="--raw"):
              embedInputFrameToTrainingAspectRatio = False
           if (sys.argv[i]=="--mt"):
              multiThreaded = True
           if (sys.argv[i]=="--calib"):
              calibrationFile = sys.argv[i+1]
           if (sys.argv[i]=="--focalLength"):
              fX = float(sys.argv[i+1])
              fY = float(sys.argv[i+2])
              alterFocalLength = True
           if (sys.argv[i]=="--frameskip"):
              frameSkip    = int(sys.argv[i+1])
           if (sys.argv[i]=="--nnsubsample"):
              doNNEveryNFrames    = int(sys.argv[i+1])
           if (sys.argv[i]=="--ik"):
               hcdLearningRate     = float(sys.argv[i+1])
               hcdEpochs           = int(sys.argv[i+2])
               hcdIterations       = int(sys.argv[i+3])
           if (sys.argv[i]=="--smooth"):
               smoothingSampling   = float(sys.argv[i+1])
               smoothingCutoff     = float(sys.argv[i+2])
           if (sys.argv[i]=="--noik"):
              doHCDPostProcessing = 0
           if (sys.argv[i]=="--aspectCorrection"):
              aspectCorrection=float(sys.argv[i+1])
           if (sys.argv[i]=="--noise"):
              addNoise=float(sys.argv[i+1])
           if (sys.argv[i]=="--size"):
              videoWidth  = int(sys.argv[i+1])
              videoHeight = int(sys.argv[i+2])
           if (sys.argv[i]=="--scale"):
              scale=float(sys.argv[i+1])
           if (sys.argv[i]=="--plot"):
              plotBVHChannels=True
           if (sys.argv[i]=="--all"):
              doBody=True
              doREye=True
              doMouth=True
              doHands=True
           if (sys.argv[i]=="--dump"):
              dumpData=True
              saveVideo=True
           if (sys.argv[i]=="--nobody"):
              doBody           = False
              doUpperBody      = False
              doLowerBody      = False
           if (sys.argv[i]=="--noupperbody"):
              doBody      = False
              doUpperBody = False
           if (sys.argv[i]=="--nolowerbody"):
              doBody      = False
              doLowerBody = False
           if (sys.argv[i]=="--face"):
              doFace=True
           if (sys.argv[i]=="--eyes") or (sys.argv[i]=="--reye"):
              doREye=True
           if (sys.argv[i]=="--mouth"):
              doMouth=True
           if (sys.argv[i]=="--hands"):
              doHands=True
           if (sys.argv[i]=="--save"):
              saveVideo=True
           if (sys.argv[i]=="--illustrate"):
              illustrate=True
              saveVideo=True
           if (sys.argv[i]=="--engine"):
              engine=sys.argv[i+1]
              print("Selecting engine : ",engine)
           if (sys.argv[i]=="--from"):
              videoFilePath=sys.argv[i+1]
           if (sys.argv[i]=="--profile"):
              doProfiling=True


  if "/dev/video" in videoFilePath:
     smoothingSampling   = 20.0
     smoothingCutoff     = 8.0
     print("Special Low Smoothing setup") 
       


  from MocapNETVisualization import drawMocapNETOutput,drawDescriptor,drawNSRM,drawMAE2DError,drawMocapNETAllPlots

  #Select a MocapNET class from tensorflow/tensorrt/onnx/tf-lite engines
  from MocapNET import easyMocapNETConstructor
  mnet = easyMocapNETConstructor(
                                 engine,
                                 doProfiling         = doProfiling,
                                 multiThreaded       = multiThreaded,
                                 doHCDPostProcessing = doHCDPostProcessing,
                                 hcdLearningRate     = hcdLearningRate,
                                 hcdEpochs           = hcdEpochs,
                                 hcdIterations       = hcdIterations,
                                 smoothingSampling   = smoothingSampling,
                                 smoothingCutoff     = smoothingCutoff,   
                                 bvhScale            = scale,
                                 doBody              = doBody,
                                 doUpperbody         = doUpperBody,
                                 doLowerbody         = doLowerBody,
                                 doFace              = doFace,
                                 doREye              = doREye,
                                 doMouth             = doMouth,
                                 doHands             = doHands,
                                 addNoise            = addNoise
                                )

  if (calibrationFile!=""):
        print("Enforcing Calibration file : ",calibrationFile)
        mnet.bvh.configureRendererFromFile(calibrationFile)
  if (alterFocalLength):
              commands = dict()
              commands["fX"] = fX
              commands["fY"] = fY
              mnet.bvh.configureRenderer(commands)

#To scale the intrinsic camera parameters (focal length and principal point) when changing the resolution of the image from 640x480 to 1280x720, you can use the following formula:
#New Focal Length (fx_new, fy_new) = (New Image Width / Original Image Width) * Original Focal Length
#New Principal Point (cx_new, cy_new) = (New Image Width / Original Image Width) * Original Principal Point


  mnet.test()
  mnet.recordBVH(not liveDemo) 

  #Body only
  mp = None
  if (doFace or doMouth or doMouth or doHands):
     mp = MediaPipeHolistic(doMediapipeVisualization = False)
  else:
     mp = MediaPipePose(doMediapipeVisualization = False)

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

  #------------------------------------------------
  #------------------------------------------------
  #------------------------------------------------
  print("\n\n\n\nStarting MocapNET using MediaPipe 2D Joint Estimator Input")
  print("Please wait until input processing finishes!")
  maxBrokenFrames = 100
  brokenFrames = 0
  totalProcessingTime = 0
  totalProcessingTimeSamples = 0
  while cap.isOpened():
    success, image = cap.read()

    if (embedInputFrameToTrainingAspectRatio):
        image = embed_to_1920x1080(image)

    #Frame skipping..!
    if (frameSkip>0):
       for i in range(0,frameSkip):
          frameNumber = frameNumber + 1
          success, image = cap.read()
          

    plotImage = image
    thisFrameIsBroken = False
    if not success:
      thisFrameIsBroken = True
      eprint("Ignoring empty camera frame : ",brokenFrames,"/",maxBrokenFrames)
      brokenFrames = brokenFrames + 1
      if (brokenFrames>maxBrokenFrames):
        break
      else: 
        continue
      # If loading a video, use 'break' instead of 'continue'.
      #continue

    if ( aspectCorrection!=1.0 ):
       width  = int(image.shape[1]*aspectCorrection)
       height = int(image.shape[0])
       image  = cv2.resize(image, (width,height))
    imageClean = image.copy()

    #--------------------------------------------------------------------------------------------------------------
    start = time.time() # Time elapsed
    mocapNETInput,annotated_image = mp.convertImageToMocapNETInput(image) 
    end = time.time() # Time elapsed
    mnet.hz_2DEst = secondsToHz(end - start)
    mnet.history_hz_2DEst.append(mnet.hz_2DEst)
    if (len(mnet.history_hz_2DEst)>mnet.perfHistorySize): 
            mnet.history_hz_2DEst.pop(0) #Keep mnet history on limits
    #--------------------------------------------------------------------------------------------------------------
    doNN = 1 
    if (doNNEveryNFrames>0):
        doNN = (frameNumber%doNNEveryNFrames)==0
    #--------------------------------------------------------------------------------------------------------------
    mocapNET3DOutput  = mnet.predict3DJoints(mocapNETInput,runNN=doNN,runHCD=True) 
    mocapNETBVHOutput = mnet.outputBVH
    bvhAnglesForPlotting.append(mocapNETBVHOutput)
    bvhAllAnglesForPlotting.append(mocapNETBVHOutput)
    if (len(bvhAnglesForPlotting)>100):
       bvhAnglesForPlotting.pop(0)
    #--------------------------------------------------------------------------------------------------------------
    if (not thisFrameIsBroken):
          totalProcessingTime        = totalProcessingTime + (time.time() - start)
          totalProcessingTimeSamples = totalProcessingTimeSamples + 1
    #--------------------------------------------------------------------------------------------------------------
    from MocapNETVisualization import visualizeMocapNETEnsemble
    image,plotImage = visualizeMocapNETEnsemble(mnet,annotated_image,plotBVHChannels=plotBVHChannels,bvhAnglesForPlotting=bvhAnglesForPlotting)
    #--------------------------------------------------------------------------------------------------------------
    frameNumber = frameNumber + 1

    mnet.printStatus()

    if (dumpData):
         import json
         print("Dumping All Output Data..")
         dumped_data = dict()

         for k in mnet.ensemble.keys():
              thisEnsemble = mnet.ensemble[k]
              print("NSRM ",k," ",thisEnsemble.NSRM.tolist())
              dumped_data["NSRM_%s"%k]  = thisEnsemble.NSRM.tolist()

         dumped_data["2DInput"]   = mocapNETInput
         dumped_data["3DOutput"]  = mocapNET3DOutput
         dumped_data["BVHOutput"] = mocapNETBVHOutput

         print("Dumping descriptors_%05u.json" % (frameNumber))

         with open("descriptors_%05u.json" % (frameNumber), "w") as fp:
              json.dump(str(dumped_data) , fp)

    if (saveVideo): 
        if (illustrate):
          rgb,mn = crop_image_around_landmarks(imageClean, annotated_image, mocapNETInput, padding=50)
          cv2.imwrite('%05u_rgb.jpg'%(frameNumber), rgb)
          cv2.imwrite('%05u_mnet.jpg'%(frameNumber), mn)
        cv2.imwrite('colorFrame_0_%05u.jpg'%(frameNumber), annotated_image)
        if (plotBVHChannels):
              cv2.imwrite('plotFrame_0_%05u.jpg'%(frameNumber), plotImage)
        
    if not headless:
      cv2.imshow('MocapNET 4 using MediaPipe Holistic 2D Joints', annotated_image)
      if (plotBVHChannels):
           cv2.imshow('MocapNET 4 using MediaPipe Holistic Motion History',plotImage) 
 
      if cv2.waitKey(1) & 0xFF == 27:
         break
 
  if (dumpData):
       print("Package everything!")
       from tools import getFilenameWithoutExtension
       packageName = getFilenameWithoutExtension(videoFilePath)
       os.system("zip %s.zip descriptors_0*.json in.csv out.csv out.bvh livelastRun3DHiRes.mp4 2d_out.csv  3d_out.csv bvh_out.csv map_out.csv " % packageName) 
       os.system("rm descriptors_0*.json in.csv out.csv out.bvh livelastRun3DHiRes.mp4 2d_out.csv 3d_out.csv bvh_out.csv map_out.csv")

  if (totalProcessingTimeSamples>0):
          print("Average processing time for ",totalProcessingTimeSamples," frames : ",end="")
          print(" %0.1f milliseconds "%(1000 * (totalProcessingTime/totalProcessingTimeSamples)),end="")
          print(secondsToHz(totalProcessingTime/totalProcessingTimeSamples)," Hz")
  else:
          print("No average processing time statistics..")
    

  if (saveVideo): #                                              1280x720 by default
     os.system("ffmpeg -framerate 30 -i colorFrame_0_%%05d.jpg -s %ux%u  -y -r 30 -pix_fmt yuv420p -threads 8 livelastRun3DHiRes.mp4  " % (videoWidth,videoHeight)) # && rm colorFrame_0_*.jpg
     if (plotBVHChannels):
        os.system("ffmpeg -framerate 30 -i plotFrame_0_%05d.jpg -s 1200x720  -y -r 30 -pix_fmt yuv420p -threads 8 livelastPlot3DHiRes.mp4 && rm plotFrame_0_*.jpg")
     

  del mnet #So that the out.bvh file gets created..
  if (not liveDemo):
    os.system("rm 2d_out.csv 3d_out.csv bvh_out.csv map_out.csv")
    os.system("./GroundTruthDumper --from out.bvh --setPositionRotation -2.6 0 2000 0 0 0  --csv ./ out.csv 2d+3d+bvh ") # Remove noise offsetPositionRotation


  cap.release()
#------------------------------------------------
#------------------------------------------------
#------------------------------------------------



if __name__ == '__main__':
    streamPosesFromCameraToMocapNET()
