#!/usr/bin/python3

import os
import sys
import cv2
import mediapipe as mp
mp_drawing = mp.solutions.drawing_utils
mp_hands = mp.solutions.hands

# For webcam input:
#pathToFiles='/media/ammar/CVRLDatasets/vae-hands-3d/RHD/RHD_published_v2/evaluation/color'
pathToFiles='/home/ammar/Documents/Datasets/RHD/images'
files=os.listdir(pathToFiles)
files.sort()

#Check everything ok
#print(files)
#sys.exit(0)

with mp_hands.Hands( min_detection_confidence=0.5, min_tracking_confidence=0.5, max_num_hands=1) as hands:
  #for filename in files:
  for i in range(0,2727):
    filename='im%u.png'%i
    fullPathToImageFile="%s/%s"%(pathToFiles,filename)
    image = cv2.imread(fullPathToImageFile)

    # Flip the image horizontally for a later selfie-view display, and convert
    # the BGR image to RGB.
    image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)
    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    results = hands.process(image)

    # Draw the hand annotations on the image.
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    if results.multi_hand_landmarks:
      for hand_landmarks in results.multi_hand_landmarks:
        mp_drawing.draw_landmarks(image,hand_landmarks,mp_hands.HAND_CONNECTIONS)
    cv2.imshow('MediaPipe Hands', image)
    
    if (results.multi_handedness!=None):
      handedness = [ handedness.classification[0].label for handedness in results.multi_handedness ]
      print(filename,",",handedness[0])
    else: 
      #print(filename,",?")
      key = cv2.waitKey(0)    
      #print(key)  
      if key==49 : 
        print(filename,",Left")
      if key==48 : 
        print(filename,",Right")
      
  
    if cv2.waitKey(1) & 0xFF == 27:
      break


