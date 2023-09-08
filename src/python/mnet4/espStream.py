# HTTP Multipart MJPEG downloader .. 
# Edited version to work with HTTP Multipart MJPEG
# Based on https://github.com/sglvladi/TensorFlowObjectDetectionTutorial

#pip install numpy matplotlib pil opencv-python --user

import numpy as np
import os
import six.moves.urllib as urllib
import sys
import tarfile
import zipfile
import cv2

from collections import defaultdict
from io import StringIO
from matplotlib import pyplot as plt
from PIL import Image

# Helper code
def load_image_into_numpy_array(image):
    (im_width, im_height) = image.size
    return np.array(image.getdata()).reshape(
        (im_height, im_width, 3)).astype(np.uint8)


class ESP32CamStreamer():
  def __init__(self,
               url        = "http://192.168.1.33:80",
               timeout    = 35,
               readBuffer = 2048
              ):
      self.stream = urllib.request.urlopen(url,timeout=timeout)
      self.should_stop = False
      self.bytebuffer = bytes()
      self.readBuffer = readBuffer

  def isOpened(self):
      return True

  def release(self):  
      del self.stream
      del self.should_stop
      del self.bytebuffer

  def read(self):  
            # Read frame from camera
            success = True 
            while True:
                self.bytebuffer += self.stream.read(self.readBuffer)
                a = self.bytebuffer.find(b'\xff\xd8')
                b = self.bytebuffer.find(b'\xff\xd9')
                if a != -1 and b != -1:
                    jpg = self.bytebuffer[a:b+2]
                    self.bytebuffer = self.bytebuffer[b+2:]
                    try:
                       self.image_np = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
                    except:
                       success=False
                    break
            return success,self.image_np 
  def visualize(
                self,
                windowname='Object Detection',
                width=800,
                height=600
               ): 
            # Display output
            cv2.imshow(windowname, cv2.resize(self.image_np,(width,height)))

            if cv2.waitKey(10) & 0xff == ord('q'):
                cv2.destroyAllWindows()
                self.should_stop = True
                #break

