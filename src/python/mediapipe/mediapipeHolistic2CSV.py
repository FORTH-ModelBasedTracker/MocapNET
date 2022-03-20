#!/usr/bin/python3
import cv2
import mediapipe as mp
import time
import os
import sys

mp_drawing = mp.solutions.drawing_utils
mp_holistic = mp.solutions.holistic


from C_Parser import readCListFromFile

#I have added a seperate list with the joints  
#TODO: take care of case of labels
from holisticPartNames import getHolisticBodyNameList
MEDIAPIPE_POSE_LANDMARK_NAMES=getHolisticBodyNameList()
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('Head')              #0
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('endsite_eyel.r')    #1
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('endsite_eye.r')     #2
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('endsite_eyer.r')    #3
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('endsite_eyer.l')    #4
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('endsite_eye.l')     #5
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('endsite_eyel.l')    #6
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('REar')              #7
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('LEar')              #8
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('RMouth')            #9
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('LMouth')            #10
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('RShoulder')         #11
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('LShoulder')         #12
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('RElbow')            #13
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('LElbow')            #14
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('RWrist')            #15
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('LWrist')            #16
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('finger5-3.r')       #17
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('finger5-3.l')       #18
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('finger2-3.r')       #19
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('finger2-3.l')       #20
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('finger1-3.r')       #21
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('finger1-3.l')       #22
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('RHip')              #23
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('LHip')              #24
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('RKnee')             #25
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('LKnee')             #26
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('RAnkle')            #27
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('LAnkle')            #28
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('RFoot')             #29
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('LFoot')             #30
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('endsite_toe1-2.r')  #31
#MEDIAPIPE_POSE_LANDMARK_NAMES.append('endsite_toe1-2.l')  #32

 

def appendListOf2DXY(fo,listToUse):
  firstElement=0
  for element in listToUse:
    if (element.find("End of")==-1 ):

       if (firstElement==0):
         firstElement=1
       else:
         fo.write(",")
       elementLowercased=element.lower()
       fo.write("2DX_")
       fo.write(elementLowercased)
       fo.write(",2DY_")
       fo.write(elementLowercased)
       fo.write(",visible_")
       fo.write(elementLowercased)


def appendValuesOfListOf2DXY(fo,listToUse,values):
  firstElement=0
  itemNumber=0
  for element in listToUse:
    if (element.find("End of")==-1 ):

       if (firstElement==0):
         firstElement=1
       else:
         fo.write(",")

       thisLandmarkName = listToUse[itemNumber].lower() 
       labelX = "2DX_"+thisLandmarkName
       labelY = "2DY_"+thisLandmarkName
       labelV = "visible_"+thisLandmarkName

       #--------------------------------
       if labelX in values:
          fo.write(str(values[labelX]))
       else:
          fo.write("0.0")
       #--------------------------------
       fo.write(",")
       if labelY in values:
          fo.write(str(values[labelY]))
       else:
          fo.write("0.0")
       #--------------------------------
       fo.write(",")
       if labelV in values:
          fo.write(str(values[labelV]))
       else:
          fo.write("0.0")
       #-------------------------------- 
    itemNumber=itemNumber+1



def appendZerosForListOf2DXY(fo,listToUse):
  firstElement=0
  for element in listToUse:
    if (element.find("End of")==-1 ):

       if (firstElement==0):
         firstElement=1
       else:
         fo.write(",")
       elementLowercased=element.lower()
       fo.write("0,0,0")






def checkIfFileExists(filename):
    return os.path.isfile(filename) 

def createDirectory(path):
    if not os.path.exists(path):
       os.makedirs(path)


def drawListNumbers(image,lst):
  font = cv2.FONT_HERSHEY_SIMPLEX
  org = (50, 50)
  fontScale = 1 
  color = (255, 0, 0)
  thickness = 2
  cv2.putText(image, 'Parsing dataset through MediaPipe Holistic', org, font, fontScale, color, thickness, cv2.LINE_AA)
  itemNumber=0
  #for item in lst['landmark']:
  if lst is not None:
   for item in lst.landmark:
     org = ( int(item.x * image.shape[1]) , int(item.y * image.shape[0]) ) 
     cv2.putText(image, '%u'%(itemNumber), org, font, fontScale, color, thickness, cv2.LINE_AA)
     itemNumber = itemNumber +1



def processPoseLandmarks(mnetPose2D,correctLabels,holisticPose):
   itemNumber=0
   if holisticPose is not None:
     for item in holisticPose.landmark:
        thisLandmarkName = correctLabels[itemNumber].lower() 
        labelX = "2DX_"+thisLandmarkName
        mnetPose2D[labelX]=item.x
        labelY = "2DY_"+thisLandmarkName
        mnetPose2D[labelY]=item.y
        labelV = "visible_"+thisLandmarkName
        mnetPose2D[labelV]=item.visibility
        
        print("Joint ",thisLandmarkName,"(",itemNumber,") x=",item.x," y=",item.y," z=",item.z)
        itemNumber = itemNumber +1
   return mnetPose2D


def guessLandmarks(mnetPose2D):
   if mnetPose2D is not None:
        if ("2DX_rshoulder" in mnetPose2D) and ("2DY_rshoulder" in mnetPose2D) and ("visible_rshoulder" in mnetPose2D) and ("2DX_lshoulder" in mnetPose2D) and ("2DY_lshoulder" in mnetPose2D) and ("visible_lshoulder" in mnetPose2D) :
              #---------------------------------------------
              rX = float(mnetPose2D["2DX_rshoulder"])
              rY = float(mnetPose2D["2DY_rshoulder"])
              rV = float(mnetPose2D["visible_rshoulder"])
              #---------------------------------------------
              lX = float(mnetPose2D["2DX_lshoulder"])
              lY = float(mnetPose2D["2DY_lshoulder"])
              lV = float(mnetPose2D["visible_lshoulder"])
              #---------------------------------------------
              if (rV>0.0) and (lV>0.0):
                 mnetPose2D["2DX_neck"]=(rX+lX)/2
                 mnetPose2D["2DY_neck"]=(rY+lY)/2
                 mnetPose2D["visible_neck"]=(rV+lV)/2
        #---------------------------------------------------

        if ("2DX_rhip" in mnetPose2D) and ("2DY_rhip" in mnetPose2D) and ("visible_rhip" in mnetPose2D) and ("2DX_lhip" in mnetPose2D) and ("2DY_lhip" in mnetPose2D) and ("visible_lhip" in mnetPose2D) :
              #---------------------------------------------
              rX = float(mnetPose2D["2DX_rhip"])
              rY = float(mnetPose2D["2DY_rhip"])
              rV = float(mnetPose2D["visible_rhip"])
              #---------------------------------------------
              lX = float(mnetPose2D["2DX_lhip"])
              lY = float(mnetPose2D["2DY_lhip"])
              lV = float(mnetPose2D["visible_lhip"])
              #---------------------------------------------
              if (rV>0.0) and (lV>0.0):
                 mnetPose2D["2DX_hip"]=(rX+lX)/2
                 mnetPose2D["2DY_hip"]=(rY+lY)/2
                 mnetPose2D["visible_hip"]=(rV+lV)/2
        #---------------------------------------------------
   return mnetPose2D




#In an attempt to reduce the upkeep of this codebase as much as possible
#the python code parses directly the C defines to get the order of joints
#sorry about that but I am just one person maintaining all of this code :P 
headNames       = readCListFromFile("src/MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp","HeadNames[]")
leftHandNames   = readCListFromFile("src/MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp","COCOLeftHandNames[]")
rightHandNames  = readCListFromFile("src/MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp","COCORightHandNames[]")
body25BodyNames = readCListFromFile("src/MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp","Body25BodyNames[]")
#---------------------------------------------------------------------------


outputDatasetPath="frames/shuffle.webm"
videoFilePath="shuffle.webm"

if (len(sys.argv)>1):
   #print('Argument List:', str(sys.argv))
   for i in range(0, len(sys.argv)):
       if (sys.argv[i]=="--from"):
          videoFilePath=sys.argv[i+1]
       if (sys.argv[i]=="-o"):
          outputDatasetPath=sys.argv[i+1]


outputDatasetPath=outputDatasetPath+"-mpdata"

#Make sure output path exists..
createDirectory(outputDatasetPath)

fo = open("%s/2dJoints_mediapipe.csv" % (outputDatasetPath), "w")

#Uncomment to double check..!
#fo.write("frameNumber,skeletonID,totalSkeletons,2DX_head,2DY_head,visible_head,2DX_neck,2DY_neck,visible_neck,2DX_rshoulder,2DY_rshoulder,visible_rshoulder,2DX_relbow,2DY_relbow,visible_relbow,2DX_rhand,2DY_rhand,visible_rhand,2DX_lshoulder,2DY_lshoulder,visible_lshoulder,2DX_lelbow,2DY_lelbow,visible_lelbow,2DX_lhand,2DY_lhand,visible_lhand,2DX_hip,2DY_hip,visible_hip,2DX_rhip,2DY_rhip,visible_rhip,2DX_rknee,2DY_rknee,visible_rknee,2DX_rfoot,2DY_rfoot,visible_rfoot,2DX_lhip,2DY_lhip,visible_lhip,2DX_lknee,2DY_lknee,visible_lknee,2DX_lfoot,2DY_lfoot,visible_lfoot,2DX_endsite_eye.r,2DY_endsite_eye.r,visible_endsite_eye.r,2DX_endsite_eye.l,2DY_endsite_eye.l,visible_endsite_eye.l,2DX_rear,2DY_rear,visible_rear,2DX_lear,2DY_lear,visible_lear,2DX_endsite_toe1-2.l,2DY_endsite_toe1-2.l,visible_endsite_toe1-2.l,2DX_endsite_toe5-3.l,2DY_endsite_toe5-3.l,visible_endsite_toe5-3.l,2DX_lheel,2DY_lheel,visible_lheel,2DX_endsite_toe1-2.r,2DY_endsite_toe1-2.r,visible_endsite_toe1-2.r,2DX_endsite_toe5-3.r,2DY_endsite_toe5-3.r,visible_endsite_toe5-3.r,2DX_rheel,2DY_rheel,visible_rheel,2DX_bkg,2DY_bkg,visible_bkg,2DX_lhand,2DY_lhand,visible_lhand,2DX_lthumb,2DY_lthumb,visible_lthumb,2DX_finger1-2.l,2DY_finger1-2.l,visible_finger1-2.l,2DX_finger1-3.l,2DY_finger1-3.l,visible_finger1-3.l,2DX_endsite_finger1-3.l,2DY_endsite_finger1-3.l,visible_endsite_finger1-3.l,2DX_finger2-1.l,2DY_finger2-1.l,visible_finger2-1.l,2DX_finger2-2.l,2DY_finger2-2.l,visible_finger2-2.l,2DX_finger2-3.l,2DY_finger2-3.l,visible_finger2-3.l,2DX_endsite_finger2-3.l,2DY_endsite_finger2-3.l,visible_endsite_finger2-3.l,2DX_finger3-1.l,2DY_finger3-1.l,visible_finger3-1.l,2DX_finger3-2.l,2DY_finger3-2.l,visible_finger3-2.l,2DX_finger3-3.l,2DY_finger3-3.l,visible_finger3-3.l,2DX_endsite_finger3-3.l,2DY_endsite_finger3-3.l,visible_endsite_finger3-3.l,2DX_finger4-1.l,2DY_finger4-1.l,visible_finger4-1.l,2DX_finger4-2.l,2DY_finger4-2.l,visible_finger4-2.l,2DX_finger4-3.l,2DY_finger4-3.l,visible_finger4-3.l,2DX_endsite_finger4-3.l,2DY_endsite_finger4-3.l,visible_endsite_finger4-3.l,2DX_finger5-1.l,2DY_finger5-1.l,visible_finger5-1.l,2DX_finger5-2.l,2DY_finger5-2.l,visible_finger5-2.l,2DX_finger5-3.l,2DY_finger5-3.l,visible_finger5-3.l,2DX_endsite_finger5-3.l,2DY_endsite_finger5-3.l,visible_endsite_finger5-3.l,2DX_rhand,2DY_rhand,visible_rhand,2DX_rthumb,2DY_rthumb,visible_rthumb,2DX_finger1-2.r,2DY_finger1-2.r,visible_finger1-2.r,2DX_finger1-3.r,2DY_finger1-3.r,visible_finger1-3.r,2DX_endsite_finger1-3.r,2DY_endsite_finger1-3.r,visible_endsite_finger1-3.r,2DX_finger2-1.r,2DY_finger2-1.r,visible_finger2-1.r,2DX_finger2-2.r,2DY_finger2-2.r,visible_finger2-2.r,2DX_finger2-3.r,2DY_finger2-3.r,visible_finger2-3.r,2DX_endsite_finger2-3.r,2DY_endsite_finger2-3.r,visible_endsite_finger2-3.r,2DX_finger3-1.r,2DY_finger3-1.r,visible_finger3-1.r,2DX_finger3-2.r,2DY_finger3-2.r,visible_finger3-2.r,2DX_finger3-3.r,2DY_finger3-3.r,visible_finger3-3.r,2DX_endsite_finger3-3.r,2DY_endsite_finger3-3.r,visible_endsite_finger3-3.r,2DX_finger4-1.r,2DY_finger4-1.r,visible_finger4-1.r,2DX_finger4-2.r,2DY_finger4-2.r,visible_finger4-2.r,2DX_finger4-3.r,2DY_finger4-3.r,visible_finger4-3.r,2DX_endsite_finger4-3.r,2DY_endsite_finger4-3.r,visible_endsite_finger4-3.r,2DX_finger5-1.r,2DY_finger5-1.r,visible_finger5-1.r,2DX_finger5-2.r,2DY_finger5-2.r,visible_finger5-2.r,2DX_finger5-3.r,2DY_finger5-3.r,visible_finger5-3.r,2DX_endsite_finger5-3.r,2DY_endsite_finger5-3.r,visible_endsite_finger5-3.r,2DX_head_rchin_0,2DY_head_rchin_0,visible_head_rchin_0,2DX_head_rchin_1,2DY_head_rchin_1,visible_head_rchin_1,2DX_head_rchin_2,2DY_head_rchin_2,visible_head_rchin_2,2DX_head_rchin_3,2DY_head_rchin_3,visible_head_rchin_3,2DX_head_rchin_4,2DY_head_rchin_4,visible_head_rchin_4,2DX_head_rchin_5,2DY_head_rchin_5,visible_head_rchin_5,2DX_head_rchin_6,2DY_head_rchin_6,visible_head_rchin_6,2DX_head_rchin_7,2DY_head_rchin_7,visible_head_rchin_7,2DX_head_chin,2DY_head_chin,visible_head_chin,2DX_head_lchin_7,2DY_head_lchin_7,visible_head_lchin_7,2DX_head_lchin_6,2DY_head_lchin_6,visible_head_lchin_6,2DX_head_lchin_5,2DY_head_lchin_5,visible_head_lchin_5,2DX_head_lchin_4,2DY_head_lchin_4,visible_head_lchin_4,2DX_head_lchin_3,2DY_head_lchin_3,visible_head_lchin_3,2DX_head_lchin_2,2DY_head_lchin_2,visible_head_lchin_2,2DX_head_lchin_1,2DY_head_lchin_1,visible_head_lchin_1,2DX_head_lchin_0,2DY_head_lchin_0,visible_head_lchin_0,2DX_head_reyebrow_0,2DY_head_reyebrow_0,visible_head_reyebrow_0,2DX_head_reyebrow_1,2DY_head_reyebrow_1,visible_head_reyebrow_1,2DX_head_reyebrow_2,2DY_head_reyebrow_2,visible_head_reyebrow_2,2DX_head_reyebrow_3,2DY_head_reyebrow_3,visible_head_reyebrow_3,2DX_head_reyebrow_4,2DY_head_reyebrow_4,visible_head_reyebrow_4,2DX_head_leyebrow_4,2DY_head_leyebrow_4,visible_head_leyebrow_4,2DX_head_leyebrow_3,2DY_head_leyebrow_3,visible_head_leyebrow_3,2DX_head_leyebrow_2,2DY_head_leyebrow_2,visible_head_leyebrow_2,2DX_head_leyebrow_1,2DY_head_leyebrow_1,visible_head_leyebrow_1,2DX_head_leyebrow_0,2DY_head_leyebrow_0,visible_head_leyebrow_0,2DX_head_nosebone_0,2DY_head_nosebone_0,visible_head_nosebone_0,2DX_head_nosebone_1,2DY_head_nosebone_1,visible_head_nosebone_1,2DX_head_nosebone_2,2DY_head_nosebone_2,visible_head_nosebone_2,2DX_head_nosebone_3,2DY_head_nosebone_3,visible_head_nosebone_3,2DX_head_nostrills_0,2DY_head_nostrills_0,visible_head_nostrills_0,2DX_head_nostrills_1,2DY_head_nostrills_1,visible_head_nostrills_1,2DX_head_nostrills_2,2DY_head_nostrills_2,visible_head_nostrills_2,2DX_head_nostrills_3,2DY_head_nostrills_3,visible_head_nostrills_3,2DX_head_nostrills_4,2DY_head_nostrills_4,visible_head_nostrills_4,2DX_head_reye_0,2DY_head_reye_0,visible_head_reye_0,2DX_head_reye_1,2DY_head_reye_1,visible_head_reye_1,2DX_head_reye_2,2DY_head_reye_2,visible_head_reye_2,2DX_head_reye_3,2DY_head_reye_3,visible_head_reye_3,2DX_head_reye_4,2DY_head_reye_4,visible_head_reye_4,2DX_head_reye_5,2DY_head_reye_5,visible_head_reye_5,2DX_head_leye_0,2DY_head_leye_0,visible_head_leye_0,2DX_head_leye_1,2DY_head_leye_1,visible_head_leye_1,2DX_head_leye_2,2DY_head_leye_2,visible_head_leye_2,2DX_head_leye_3,2DY_head_leye_3,visible_head_leye_3,2DX_head_leye_4,2DY_head_leye_4,visible_head_leye_4,2DX_head_leye_5,2DY_head_leye_5,visible_head_leye_5,2DX_head_outmouth_0,2DY_head_outmouth_0,visible_head_outmouth_0,2DX_head_outmouth_1,2DY_head_outmouth_1,visible_head_outmouth_1,2DX_head_outmouth_2,2DY_head_outmouth_2,visible_head_outmouth_2,2DX_head_outmouth_3,2DY_head_outmouth_3,visible_head_outmouth_3,2DX_head_outmouth_4,2DY_head_outmouth_4,visible_head_outmouth_4,2DX_head_outmouth_5,2DY_head_outmouth_5,visible_head_outmouth_5,2DX_head_outmouth_6,2DY_head_outmouth_6,visible_head_outmouth_6,2DX_head_outmouth_7,2DY_head_outmouth_7,visible_head_outmouth_7,2DX_head_outmouth_8,2DY_head_outmouth_8,visible_head_outmouth_8,2DX_head_outmouth_9,2DY_head_outmouth_9,visible_head_outmouth_9,2DX_head_outmouth_10,2DY_head_outmouth_10,visible_head_outmouth_10,2DX_head_outmouth_11,2DY_head_outmouth_11,visible_head_outmouth_11,2DX_head_inmouth_0,2DY_head_inmouth_0,visible_head_inmouth_0,2DX_head_inmouth_1,2DY_head_inmouth_1,visible_head_inmouth_1,2DX_head_inmouth_2,2DY_head_inmouth_2,visible_head_inmouth_2,2DX_head_inmouth_3,2DY_head_inmouth_3,visible_head_inmouth_3,2DX_head_inmouth_4,2DY_head_inmouth_4,visible_head_inmouth_4,2DX_head_inmouth_5,2DY_head_inmouth_5,visible_head_inmouth_5,2DX_head_inmouth_6,2DY_head_inmouth_6,visible_head_inmouth_6,2DX_head_inmouth_7,2DY_head_inmouth_7,visible_head_inmouth_7,2DX_head_reye,2DY_head_reye,visible_head_reye,2DX_head_leye,2DY_head_leye,visible_head_leye\n") 
 
#//Generate CSV header..
fo.write("frameNumber,skeletonID,totalSkeletons,")
appendListOf2DXY(fo,body25BodyNames)
fo.write(",")
appendListOf2DXY(fo,leftHandNames)
fo.write(",")
appendListOf2DXY(fo,rightHandNames)
fo.write(",")
appendListOf2DXY(fo,headNames)
fo.write("\n")

frameNumber = 0
# For webcam input:
cap = cv2.VideoCapture(videoFilePath)

with mp_holistic.Holistic(static_image_mode=True) as holistic:
  while cap.isOpened():
    success, image = cap.read()
    if not success:
      print("Ignoring empty camera frame.")
      # If loading a video, use 'break' instead of 'continue'.
      break
    
    cv2.imwrite("%s/colorFrame_0_%05u.jpg"%(outputDatasetPath,frameNumber), image)
    start = time.time()

    # Flip the image horizontally for a later selfie-view display, and convert
    # the BGR image to RGB.
    image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)
    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    results = holistic.process(image)
 
    # Draw the hand annotations on the image.
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

    end = time.time()
    # Time elapsed
    seconds = end - start
    #print ("Time taken : {0} seconds".format(seconds))
    # Calculate frames per second
    fps  = 1 / seconds
    print("{0} fps".format(fps))
 	 

    annotated_image = image.copy()
    mp_drawing.draw_landmarks(annotated_image, results.face_landmarks,       mp_holistic.FACE_CONNECTIONS)
    mp_drawing.draw_landmarks(annotated_image, results.left_hand_landmarks,  mp_holistic.HAND_CONNECTIONS)
    mp_drawing.draw_landmarks(annotated_image, results.right_hand_landmarks, mp_holistic.HAND_CONNECTIONS)
    # Use mp_holistic.UPPER_BODY_POSE_CONNECTIONS for drawing below when upper_body_only is set to True.
    mp_drawing.draw_landmarks(annotated_image, results.pose_landmarks,       mp_holistic.POSE_CONNECTIONS)
    print(results.pose_landmarks)

    drawListNumbers(annotated_image,results.pose_landmarks)
    mnetPose2D = dict()
    processPoseLandmarks(mnetPose2D,MEDIAPIPE_POSE_LANDMARK_NAMES,results.pose_landmarks)
    processPoseLandmarks(mnetPose2D,leftHandNames,results.left_hand_landmarks)
    processPoseLandmarks(mnetPose2D,rightHandNames,results.right_hand_landmarks)

    guessLandmarks(mnetPose2D)

    #fo.write("frameNumber,skeletonID,totalSkeletons,")
    fo.write("%u,0,1,"%(frameNumber))
    appendValuesOfListOf2DXY(fo,body25BodyNames,mnetPose2D)
    fo.write(",")
    appendValuesOfListOf2DXY(fo,leftHandNames,mnetPose2D) 
    fo.write(",")
    appendValuesOfListOf2DXY(fo,rightHandNames,mnetPose2D) 
    fo.write(",")
    appendZerosForListOf2DXY(fo,headNames)
    fo.write("\n")
    
    #Done with frame -------------------------------------
    frameNumber = frameNumber + 1


    cv2.imshow('MediaPipe Holistic', annotated_image)
    if cv2.waitKey(5) & 0xFF == 27:
      break
cap.release()


fo.close()

