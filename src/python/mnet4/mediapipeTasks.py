#!/usr/bin/python3

"""
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH"

Mediapipe dropped the old "solutions" python API (mp.solutions.holistic / pose / face_mesh / drawing_utils)
after version 0.10.21, everything from 0.10.30 and mediapipe 1.x onwards only ships the Tasks API.
This module reimplements the small part of the solutions API that MocapNET uses on top of the Tasks API,
so the rest of the code keeps receiving landmarks in exactly the shape it always did :

     import mediapipeTasks as mp
     mp_holistic = mp.holistic
     mp_drawing  = mp.drawing_utils

     with mp_holistic.Holistic(static_image_mode=True) as holistic:
          results = holistic.process(imageInRGB)
          results.pose_landmarks.landmark[0].x  #<- same as before

The Tasks API needs its models as .task bundles instead of having them baked in the wheel, they are
downloaded once (to mediapipeModels/) the first time an estimator that needs them is constructed.
"""

import os
import sys
import time
import urllib.request

import numpy as np
import mediapipe as mp

from mediapipe.tasks.python.core.base_options            import BaseOptions
from mediapipe.tasks.python.vision                       import drawing_utils  as tasksDrawingUtils
from mediapipe.tasks.python.vision                       import drawing_styles as tasksDrawingStyles
from mediapipe.tasks.python.vision.core.vision_task_running_mode import VisionTaskRunningMode
from mediapipe.tasks.python.vision.face_landmarker       import FaceLandmarker,     FaceLandmarkerOptions,     FaceLandmarksConnections
from mediapipe.tasks.python.vision.hand_landmarker       import HandLandmarksConnections
from mediapipe.tasks.python.vision.holistic_landmarker   import HolisticLandmarker, HolisticLandmarkerOptions
from mediapipe.tasks.python.vision.pose_landmarker       import PoseLandmarker,     PoseLandmarkerOptions,     PoseLandmarksConnections

#------------------------------------------------------------------------------------------------
# The connection lists, same names the solutions API used so that drawing code does not change
#------------------------------------------------------------------------------------------------
POSE_CONNECTIONS     = PoseLandmarksConnections.POSE_LANDMARKS
HAND_CONNECTIONS     = HandLandmarksConnections.HAND_CONNECTIONS
FACEMESH_TESSELATION = FaceLandmarksConnections.FACE_LANDMARKS_TESSELATION
FACEMESH_CONTOURS    = FaceLandmarksConnections.FACE_LANDMARKS_CONTOURS
FACEMESH_LIPS        = FaceLandmarksConnections.FACE_LANDMARKS_LIPS
FACEMESH_FACE_OVAL   = FaceLandmarksConnections.FACE_LANDMARKS_FACE_OVAL
FACEMESH_IRISES      = FaceLandmarksConnections.FACE_LANDMARKS_LEFT_IRIS + FaceLandmarksConnections.FACE_LANDMARKS_RIGHT_IRIS
FACE_CONNECTIONS     = FACEMESH_TESSELATION #<- ancient name of FACEMESH_TESSELATION, kept because old code still asks for it

#------------------------------------------------------------------------------------------------
# Model bundles, downloaded on demand and then cached on disk
#------------------------------------------------------------------------------------------------
MODEL_DIRECTORY = os.environ.get("MEDIAPIPE_MODEL_DIRECTORY", os.path.join(os.path.dirname(os.path.abspath(__file__)), "mediapipeModels"))

MODEL_BUNDLES = {
                  "holistic"   : "holistic_landmarker/holistic_landmarker/float16/latest/holistic_landmarker.task",
                  "pose_lite"  : "pose_landmarker/pose_landmarker_lite/float16/latest/pose_landmarker_lite.task",
                  "pose_full"  : "pose_landmarker/pose_landmarker_full/float16/latest/pose_landmarker_full.task",
                  "pose_heavy" : "pose_landmarker/pose_landmarker_heavy/float16/latest/pose_landmarker_heavy.task",
                  "face"       : "face_landmarker/face_landmarker/float16/latest/face_landmarker.task",
                }

MODEL_URL_BASE = "https://storage.googleapis.com/mediapipe-models"


def getModelPath(modelName):
   """Return the local path of a .task bundle, retrieving it from the mediapipe model zoo if we don't have it yet"""
   if (modelName not in MODEL_BUNDLES):
        raise ValueError("Unknown mediapipe model %s, known models are %s" % (modelName, list(MODEL_BUNDLES.keys())))

   remotePath = MODEL_BUNDLES[modelName]
   localPath  = os.path.join(MODEL_DIRECTORY, os.path.basename(remotePath))

   if (os.path.isfile(localPath)):
        return localPath

   url = "%s/%s" % (MODEL_URL_BASE, remotePath)
   print("Downloading mediapipe model %s from %s " % (modelName, url))
   os.makedirs(MODEL_DIRECTORY, exist_ok=True)
   #Download next to the target and rename, so an interrupted download never leaves a half model behind..
   temporaryPath = "%s.part" % (localPath)
   urllib.request.urlretrieve(url, temporaryPath)
   os.replace(temporaryPath, localPath)
   print("Stored mediapipe model at %s " % (localPath))
   return localPath


#------------------------------------------------------------------------------------------------
# Landmark containers that look like the protobuf messages the solutions API used to hand out
#------------------------------------------------------------------------------------------------
class Landmark():
   """A landmark shaped like the old NormalizedLandmark protobuf message.
      Only the models that track the body populate visibility, for the face/hand landmarks it stays 0.0
      exactly like the unset protobuf field did."""
   __slots__ = ("x", "y", "z", "visibility", "presence", "hasVisibility")

   def __init__(self, x=0.0, y=0.0, z=0.0, visibility=None, presence=None):
        self.x             = x
        self.y             = y
        self.z             = z
        self.hasVisibility = (visibility is not None)
        self.visibility    = visibility if (visibility is not None) else 0.0
        self.presence      = presence   if (presence   is not None) else 0.0


class LandmarkList():
   """A landmark list shaped like the old NormalizedLandmarkList protobuf message, .landmark is what MocapNET reads.
      A face carries 478 landmarks so the conversion only happens if somebody actually asks for them."""
   __slots__ = ("taskLandmarks", "convertedLandmarks")

   def __init__(self, taskLandmarks):
        self.taskLandmarks      = taskLandmarks
        self.convertedLandmarks = None

   @property
   def landmark(self):
        if (self.convertedLandmarks is None):
             self.convertedLandmarks = [Landmark(x=lm.x, y=lm.y, z=lm.z, visibility=lm.visibility, presence=lm.presence) for lm in self.taskLandmarks]
        return self.convertedLandmarks

   def __len__(self):
        return len(self.taskLandmarks)

   def __bool__(self):
        return len(self.taskLandmarks) > 0


def landmarkListFromTaskLandmarks(taskLandmarks):
   """Wrap a Tasks API landmark list in the old protobuf-ish container.
      An empty list means the part was not detected, and the solutions API used to signal that with a None."""
   if (taskLandmarks is None) or (len(taskLandmarks) == 0):
        return None
   return LandmarkList(taskLandmarks)


def convertImageToMediapipeImage(image):
   """Wrap an RGB numpy array in an mp.Image, which is what every Tasks API call expects"""
   if (image is None):
        raise ValueError("Cannot convert an empty image to a mediapipe image")
   if (image.dtype != np.uint8):
        image = image.astype(np.uint8)
   return mp.Image(image_format=mp.ImageFormat.SRGB, data=np.ascontiguousarray(image))


#------------------------------------------------------------------------------------------------
# The estimators
#------------------------------------------------------------------------------------------------
class TaskSolution():
   """Common plumbing, the Tasks API splits image/video work in two calls and wants timestamps for video"""

   def __init__(self, staticImageMode):
        self.staticImageMode = staticImageMode
        self.lastTimestamp   = -1
        self.landmarker      = None

   def runningMode(self):
        if (self.staticImageMode):
             return VisionTaskRunningMode.IMAGE
        return VisionTaskRunningMode.VIDEO

   def nextTimestamp(self):
        """Video mode wants milliseconds that never go backwards between two calls"""
        timestamp = int(time.monotonic() * 1000)
        if (timestamp <= self.lastTimestamp):
             timestamp = self.lastTimestamp + 1
        self.lastTimestamp = timestamp
        return timestamp

   def detect(self, image):
        mediapipeImage = convertImageToMediapipeImage(image)
        if (self.staticImageMode):
             return self.landmarker.detect(mediapipeImage)
        return self.landmarker.detect_for_video(mediapipeImage, self.nextTimestamp())

   def close(self):
        if (self.landmarker is not None):
             self.landmarker.close()
             self.landmarker = None

   def __enter__(self):
        return self

   def __exit__(self, exceptionType, exceptionValue, exceptionTraceback):
        self.close()
        return False


class HolisticResults():
   """What Holistic.process() used to return"""
   __slots__ = ("pose_landmarks", "pose_world_landmarks", "face_landmarks", "left_hand_landmarks", "right_hand_landmarks", "segmentation_mask")

   def __init__(self, result):
        self.pose_landmarks       = landmarkListFromTaskLandmarks(result.pose_landmarks)
        self.pose_world_landmarks = landmarkListFromTaskLandmarks(result.pose_world_landmarks)
        self.face_landmarks       = landmarkListFromTaskLandmarks(result.face_landmarks)
        self.left_hand_landmarks  = landmarkListFromTaskLandmarks(result.left_hand_landmarks)
        self.right_hand_landmarks = landmarkListFromTaskLandmarks(result.right_hand_landmarks)
        self.segmentation_mask    = result.segmentation_mask.numpy_view() if (result.segmentation_mask is not None) else None


class Holistic(TaskSolution):
   """Stand-in for the removed mp.solutions.holistic.Holistic

      The face landmarks of the holistic bundle always include the 10 refined iris landmarks (478 in total),
      so unlike the solutions API there is no need to run a second FaceMesh estimator to get the eyes.

      model_complexity/smooth_landmarks/refine_face_landmarks are accepted so that existing call sites keep
      working but the holistic bundle has a single complexity and smooths internally when it runs in video mode."""

   def __init__(
                 self,
                 static_image_mode        = False,
                 model_complexity         = 1,
                 smooth_landmarks         = True,
                 enable_segmentation      = False,
                 refine_face_landmarks    = True,
                 min_detection_confidence = 0.5,
                 min_tracking_confidence  = 0.5,
                 modelPath                = None
               ):
        TaskSolution.__init__(self, static_image_mode)

        options = HolisticLandmarkerOptions(
                                             base_options                  = BaseOptions(model_asset_path = modelPath if modelPath is not None else getModelPath("holistic")),
                                             running_mode                  = self.runningMode(),
                                             min_face_detection_confidence = min_detection_confidence,
                                             min_face_landmarks_confidence = min_tracking_confidence,
                                             min_pose_detection_confidence = min_detection_confidence,
                                             min_pose_landmarks_confidence = min_tracking_confidence,
                                             min_hand_landmarks_confidence = min_tracking_confidence,
                                             output_segmentation_mask      = enable_segmentation,
                                           )
        self.landmarker = HolisticLandmarker.create_from_options(options)

   def process(self, image):
        return HolisticResults(self.detect(image))


class PoseResults():
   """What Pose.process() used to return"""
   __slots__ = ("pose_landmarks", "pose_world_landmarks", "segmentation_mask")

   def __init__(self, result):
        self.pose_landmarks       = landmarkListFromTaskLandmarks(result.pose_landmarks[0]       if len(result.pose_landmarks)       else None)
        self.pose_world_landmarks = landmarkListFromTaskLandmarks(result.pose_world_landmarks[0] if len(result.pose_world_landmarks) else None)
        self.segmentation_mask    = None
        if (result.segmentation_masks is not None) and (len(result.segmentation_masks) > 0):
             self.segmentation_mask = result.segmentation_masks[0].numpy_view()


class Pose(TaskSolution):
   """Stand-in for the removed mp.solutions.pose.Pose, model_complexity picks the lite/full/heavy bundle just like it used to"""

   MODEL_FOR_COMPLEXITY = { 0 : "pose_lite", 1 : "pose_full", 2 : "pose_heavy" }

   def __init__(
                 self,
                 static_image_mode        = False,
                 model_complexity         = 1,
                 smooth_landmarks         = True,
                 enable_segmentation      = False,
                 min_detection_confidence = 0.5,
                 min_tracking_confidence  = 0.5,
                 modelPath                = None
               ):
        TaskSolution.__init__(self, static_image_mode)

        modelName = Pose.MODEL_FOR_COMPLEXITY.get(model_complexity, "pose_full")
        options   = PoseLandmarkerOptions(
                                           base_options                  = BaseOptions(model_asset_path = modelPath if modelPath is not None else getModelPath(modelName)),
                                           running_mode                  = self.runningMode(),
                                           num_poses                     = 1,
                                           min_pose_detection_confidence = min_detection_confidence,
                                           min_pose_presence_confidence  = min_detection_confidence,
                                           min_tracking_confidence       = min_tracking_confidence,
                                           output_segmentation_masks     = enable_segmentation,
                                         )
        self.landmarker = PoseLandmarker.create_from_options(options)

   def process(self, image):
        return PoseResults(self.detect(image))


class FaceMeshResults():
   """What FaceMesh.process() used to return, multi_face_landmarks stays a list because it can hold more than one face"""
   __slots__ = ("multi_face_landmarks",)

   def __init__(self, result):
        faces = [landmarkListFromTaskLandmarks(face) for face in result.face_landmarks]
        faces = [face for face in faces if face is not None]
        self.multi_face_landmarks = faces if len(faces) > 0 else None


class FaceMesh(TaskSolution):
   """Stand-in for the removed mp.solutions.face_mesh.FaceMesh, the face bundle always returns the refined 478 landmarks"""

   def __init__(
                 self,
                 static_image_mode        = False,
                 max_num_faces            = 1,
                 refine_landmarks         = True,
                 min_detection_confidence = 0.5,
                 min_tracking_confidence  = 0.5,
                 modelPath                = None
               ):
        TaskSolution.__init__(self, static_image_mode)

        options = FaceLandmarkerOptions(
                                         base_options                  = BaseOptions(model_asset_path = modelPath if modelPath is not None else getModelPath("face")),
                                         running_mode                  = self.runningMode(),
                                         num_faces                     = max_num_faces,
                                         min_face_detection_confidence = min_detection_confidence,
                                         min_face_presence_confidence  = min_detection_confidence,
                                         min_tracking_confidence       = min_tracking_confidence,
                                       )
        self.landmarker = FaceLandmarker.create_from_options(options)

   def process(self, image):
        return FaceMeshResults(self.detect(image))


#------------------------------------------------------------------------------------------------
# Drawing, the Tasks API has its own drawing_utils but it wants Tasks landmarks and no Nones
#------------------------------------------------------------------------------------------------
DrawingSpec = tasksDrawingUtils.DrawingSpec

WHITE_COLOR = tasksDrawingUtils.WHITE_COLOR
BLACK_COLOR = tasksDrawingUtils.BLACK_COLOR
RED_COLOR   = tasksDrawingUtils.RED_COLOR
GREEN_COLOR = tasksDrawingUtils.GREEN_COLOR
BLUE_COLOR  = tasksDrawingUtils.BLUE_COLOR


def draw_landmarks(
                    image,
                    landmark_list,
                    connections             = None,
                    landmark_drawing_spec   = DrawingSpec(color=RED_COLOR),
                    connection_drawing_spec = DrawingSpec(),
                    is_drawing_landmarks    = True
                  ):
   """Draw a landmark list on a BGR image, undetected parts arrive here as None and are silently skipped"""
   if (landmark_list is None):
        return
   tasksDrawingUtils.draw_landmarks(
                                     image,
                                     landmark_list.taskLandmarks, #<- the Tasks drawing code wants its own landmark type, which is what we kept inside
                                     connections,
                                     landmark_drawing_spec,
                                     connection_drawing_spec,
                                     is_drawing_landmarks
                                   )


#------------------------------------------------------------------------------------------------
# The mp.solutions.* namespaces, so that call sites only change which module they import
#------------------------------------------------------------------------------------------------
class holistic():
   Holistic             = Holistic
   POSE_CONNECTIONS     = POSE_CONNECTIONS
   HAND_CONNECTIONS     = HAND_CONNECTIONS
   FACEMESH_TESSELATION = FACEMESH_TESSELATION
   FACEMESH_CONTOURS    = FACEMESH_CONTOURS
   FACEMESH_IRISES      = FACEMESH_IRISES
   FACE_CONNECTIONS     = FACE_CONNECTIONS


class pose():
   Pose             = Pose
   POSE_CONNECTIONS = POSE_CONNECTIONS


class face_mesh():
   FaceMesh             = FaceMesh
   FACEMESH_TESSELATION = FACEMESH_TESSELATION
   FACEMESH_CONTOURS    = FACEMESH_CONTOURS
   FACEMESH_IRISES      = FACEMESH_IRISES


class drawing_utils():
   draw_landmarks = staticmethod(draw_landmarks)
   DrawingSpec    = DrawingSpec


drawing_styles = tasksDrawingStyles


if __name__ == '__main__':
   #python3 -m mediapipeTasks ../../../dataset/test.jpg
   import cv2
   imagePath = sys.argv[1] if (len(sys.argv) > 1) else "../../../dataset/test.jpg"
   image     = cv2.imread(imagePath)
   if (image is None):
        print("Could not read %s " % (imagePath))
        sys.exit(1)

   with Holistic(static_image_mode=True) as holisticEstimator:
        results = holisticEstimator.process(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
        for partName in ("pose_landmarks", "face_landmarks", "left_hand_landmarks", "right_hand_landmarks"):
             part = getattr(results, partName)
             print("%-22s : %s landmarks" % (partName, len(part) if part is not None else "no"))
        draw_landmarks(image, results.face_landmarks,       FACEMESH_TESSELATION)
        draw_landmarks(image, results.left_hand_landmarks,  HAND_CONNECTIONS)
        draw_landmarks(image, results.right_hand_landmarks, HAND_CONNECTIONS)
        draw_landmarks(image, results.pose_landmarks,       POSE_CONNECTIONS, landmark_drawing_spec=drawing_styles.get_default_pose_landmarks_style())
        cv2.imwrite("mediapipeTasks.jpg", image)
        print("Wrote mediapipeTasks.jpg")
