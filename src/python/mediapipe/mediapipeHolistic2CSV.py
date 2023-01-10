#!/usr/bin/python3
import cv2
import mediapipe as mp
import time
import os
import sys

from tools import checkIfFileExists,createDirectory

#Get drawing/holistic
mp_drawing  = mp.solutions.drawing_utils
mp_holistic = mp.solutions.holistic

#I have added a seperate list with the joints
from holisticPartNames import getHolisticBodyNameList, getHolisticFaceNameList, processPoseLandmarks, guessLandmarks
MEDIAPIPE_POSE_LANDMARK_NAMES=getHolisticBodyNameList()
MEDIAPIPE_FACE_LANDMARK_NAMES=getHolisticFaceNameList()
 
#In an attempt to reduce the upkeep of this codebase as much as possible
#the python code parses directly the C defines to get the order of joints
#sorry about that but I am just one person maintaining all of this code :P 
#this also means you need to run the python script from the root directory of the repository!
from C_Parser import readCListFromFile
headNames          = readCListFromFile("src/MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp","HeadNames[]")
leftHandNames      = readCListFromFile("src/MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp","COCOLeftHandNames[]")
rightHandNames     = readCListFromFile("src/MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp","COCORightHandNames[]")
body25BodyNames    = readCListFromFile("src/MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp","Body25BodyNames[]")
#---------------------------------------------------------------------------


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
  itemNumber=0
  for element in listToUse:
    if (element.find("End of")==-1 ):

       thisLandmarkName = listToUse[itemNumber].lower() 
       labelX = "2DX_"+thisLandmarkName
       labelY = "2DY_"+thisLandmarkName
       labelV = "visible_"+thisLandmarkName

       #--------------------------------
       fo.write(",")
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
  for element in listToUse:
    if (element.find("End of")==-1 ):
       elementLowercased=element.lower()
       fo.write(",0,0,0")



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



def convertStreamToMocapNETCSV():
    videoFilePath    ="shuffle.webm"
    outputDatasetPath="frames/shuffle.webm"

    if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--from"):
              videoFilePath=sys.argv[i+1]
           if (sys.argv[i]=="-o"):
              outputDatasetPath=sys.argv[i+1]
              #Strip last character of output path if it is a /
              if ( outputDatasetPath[len(outputDatasetPath)-1] == '/' ):
                  outputDatasetPath =  outputDatasetPath[:-1]


    #We append an -mpdata to make sure this is different from the OpenPose output
    outputDatasetPath=outputDatasetPath+"-mpdata"

    #Make sure output path exists..
    createDirectory(outputDatasetPath)

    fo = open("%s/2dJoints_mediapipe.csv" % (outputDatasetPath), "w")

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
    
    maxFailedFrames = 100
    failedFrames = 0
    frameNumber = 0
    # For webcam input:
    cap = cv2.VideoCapture(videoFilePath)

    with mp_holistic.Holistic(static_image_mode=True) as holistic:
      while cap.isOpened():
        success, image = cap.read()
        if not success:
          print("\rIgnoring empty camera frame. ",failedFrames,"/",maxFailedFrames,"\r", end="", flush=True)
          failedFrames = failedFrames + 1
          # If loading a video, use 'break' instead of 'continue'.
          if (failedFrames>100):
              break
        else:
          failedFrames = 0

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
          # Calculate frames per second
          fps  = 1 / seconds
          print("\r Frame : ",frameNumber,"   |   ",round(fps,2)," fps          \r", end="", flush=True)

          annotated_image = image.copy()
          #Compensate for name mediapipe change..
          try:
            mp_drawing.draw_landmarks(annotated_image, results.face_landmarks      , mp_holistic.FACEMESH_TESSELATION) #This used to be called FACE_CONNECTIONS
          except:
            mp_drawing.draw_landmarks(annotated_image, results.face_landmarks      , mp_holistic.FACE_CONNECTIONS) #This used to be called FACE_CONNECTIONS

          mp_drawing.draw_landmarks(annotated_image, results.left_hand_landmarks,  mp_holistic.HAND_CONNECTIONS)
          mp_drawing.draw_landmarks(annotated_image, results.right_hand_landmarks, mp_holistic.HAND_CONNECTIONS)
          # Use mp_holistic.UPPER_BODY_POSE_CONNECTIONS for drawing below when upper_body_only is set to True.
          mp_drawing.draw_landmarks(annotated_image, results.pose_landmarks,       mp_holistic.POSE_CONNECTIONS)

          #Draw visualization of 2D joints!
          drawListNumbers(annotated_image,results.pose_landmarks)

          mnetPose2D = dict()
          #------------------------------------------------------------------------------------
          processPoseLandmarks(mnetPose2D,MEDIAPIPE_POSE_LANDMARK_NAMES,results.pose_landmarks)
          processPoseLandmarks(mnetPose2D,leftHandNames                ,results.left_hand_landmarks)
          processPoseLandmarks(mnetPose2D,rightHandNames               ,results.right_hand_landmarks)
          processPoseLandmarks(mnetPose2D,MEDIAPIPE_FACE_LANDMARK_NAMES,results.face_landmarks)
          #------------------------------------------------------------------------------------
          guessLandmarks(mnetPose2D) #Some landmarks ( neck, hip need to be guessed by others )

          skeletonID     = 0
          totalSkeletons = 1
          #Write this CSV record -------------------------------------
          fo.write("%u,%u,%u,"%(frameNumber,skeletonID,totalSkeletons))
          appendValuesOfListOf2DXY(fo,body25BodyNames,mnetPose2D)
          appendValuesOfListOf2DXY(fo,leftHandNames  ,mnetPose2D)
          appendValuesOfListOf2DXY(fo,rightHandNames ,mnetPose2D)
          appendValuesOfListOf2DXY(fo,headNames      ,mnetPose2D)
          fo.write("\n")
          #-----------------------------------------------------------

          #Done with frame -------------------------------------
          frameNumber = frameNumber + 1


          #Show visualization frame -------------------------------------
          cv2.imshow('MediaPipe Holistic', annotated_image)
          if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    fo.close()

    print("Done dumping to CSV using mediapipe")
    print(" try running with MocapNET using : ")
    print("       ./MocapNET2CSV --from %s/2dJoints_mediapipe.csv --show 3 --hands" % (outputDatasetPath))


if __name__ == '__main__':
    convertStreamToMocapNETCSV()
