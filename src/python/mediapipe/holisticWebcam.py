#!/usr/bin/python3

import cv2
import mediapipe as mp
import time

mp_drawing = mp.solutions.drawing_utils
mp_holistic = mp.solutions.holistic

# For webcam input:
cap = cv2.VideoCapture(0)

with mp_holistic.Holistic(static_image_mode=True) as holistic:
  while cap.isOpened():
    success, image = cap.read()
    if not success:
      print("Ignoring empty camera frame.")
      # If loading a video, use 'break' instead of 'continue'.
      continue

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
    print("\r Framerate : ",round(fps,2)," fps \r", end="", flush=True)

    annotated_image = image.copy()
    #Compensate for name mediapipe change..
    try:
       mp_drawing.draw_landmarks(annotated_image, results.face_landmarks      , mp_holistic.FACEMESH_TESSELATION) #This used to be called FACE_CONNECTIONS
    except:
       mp_drawing.draw_landmarks(annotated_image, results.face_landmarks      , mp_holistic.FACE_CONNECTIONS) #This used to be called FACE_CONNECTIONS

    mp_drawing.draw_landmarks(annotated_image, results.left_hand_landmarks, mp_holistic.HAND_CONNECTIONS)
    mp_drawing.draw_landmarks(annotated_image, results.right_hand_landmarks, mp_holistic.HAND_CONNECTIONS)
    # Use mp_holistic.UPPER_BODY_POSE_CONNECTIONS for drawing below when
    # upper_body_only is set to True.
    mp_drawing.draw_landmarks(annotated_image, results.pose_landmarks, mp_holistic.POSE_CONNECTIONS)

    cv2.imshow('MediaPipe Holistic', annotated_image)
    if cv2.waitKey(5) & 0xFF == 27:
      break
cap.release()

