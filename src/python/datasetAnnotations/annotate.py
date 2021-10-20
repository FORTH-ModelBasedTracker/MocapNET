
# import required module 
import sys
import cv2 
import numpy as np
from datetime import datetime 


pathToDataset="/home/ammar/Documents/Datasets/ai_challenger"
filename = "%s/train/annotation.json" % pathToDataset


import json
with open(filename) as jsonData:
          data = json.load(jsonData)

print (len(data))
print (data.keys())
if ( len(data['images']) != len(data['annotations'])  ):
   print("Mismatching image/annotation data")
   sys.exit(0)


for imageID in range(0,len(data['images'])):
   imageData=data['images'][imageID]
   imageAnnotations=data['annotations'][imageID]
   print(imageData)
   loadImage=cv2.imread("%s/train/images/%s"%(pathToDataset,imageData['file_name']),-1)
   
   #Draw Bounding Box   
   cv2.rectangle(loadImage, (imageAnnotations['bbox'][0],imageAnnotations['bbox'][1]),(imageAnnotations['bbox'][2],imageAnnotations['bbox'][3]), (255,255,255), 5)

   radius = 5
   color = (123,123,123)
   
   for keypointID in range(0,int( len(imageAnnotations['keypoints'])/3 ) ):
      cv2.circle(loadImage, (imageAnnotations['keypoints'][keypointID*3+0],imageAnnotations['keypoints'][keypointID*3+1]) , radius, color, -1)

   cv2.imshow("Annotations", loadImage)
   print(imageAnnotations)

   cv2.waitKey(1000)  

