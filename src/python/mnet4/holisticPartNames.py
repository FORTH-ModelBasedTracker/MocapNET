#!/usr/bin/python3
""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

from tools import bcolors 

dumpedFrameForTiles=0


def normalize2DPointWhileAlsoMatchingTrainingAspectRatio(x, y, currentAspectRatio, trainingAspectRatio=float(1920/1080)):
    if x == 0 and y == 0:
        # Only Fix Aspect Ratio on visible points to preserve 0,0,0 that are
        # important to the neural network.
        return x, y

    aspectRatioDiff = currentAspectRatio - trainingAspectRatio
    if (aspectRatioDiff<0): 
        aspectRatioDiff = -1 * aspectRatioDiff

    #print("ASPECT RATIO DIFFERENCE => ",aspectRatioDiff)
    if aspectRatioDiff<0.001:
        # Only Fix Aspect Ratio when it needs to be fixed..
        return x, y


    if currentAspectRatio > trainingAspectRatio:
        targetHeight = 1.0
        targetWidth  = currentAspectRatio
    else:
        targetWidth  = 1.0
        targetHeight = 1.0 / currentAspectRatio

    addX = (targetWidth - trainingAspectRatio) / 2
    addY = (targetHeight - 1.0) / 2

    targetAspectRatio = targetWidth / targetHeight
    if int(targetAspectRatio * 100) != int(currentAspectRatio * 100):
        print("Failed to perfectly match training aspect ratio ({:.5f}), managed to reach ({:.5f})".format(currentAspectRatio, targetAspectRatio))

    x = (x + addX) / targetWidth
    y = (y + addY) / targetHeight

    return x, y


def normalize2DPointWhileAlsoMatchingTrainingAspectRatioSimple(x, y, currentAspectRatio, trainingAspectRatio=float(1920/1080)):
    aspectRatioFix = float(trainedAspectRatio/currentAspectRatio)
    return aspectRatioFix * x, y


#-------------------------------------------------------------------------------------------------------------------
def processPoseLandmarks(mnetPose2D,correctLabels,holisticPose,currentAspectRatio,trainedAspectRatio,flipX=False,useVisibility=True,visibilityThreshold=0.3,label="mediapipe"):
   itemNumber     = 0
   aspectRatioFix = float(trainedAspectRatio/currentAspectRatio)
   #------------------------------------------------------------
   if holisticPose is not None:
     for item in holisticPose.landmark:
        thisLandmarkName = correctLabels[itemNumber].lower() 
        if (thisLandmarkName!=''):
          #--------------------------------
          #Do Visibility logic -> transform
          #--------------------------------
          x   = item.x
          y   = item.y
          z   = item.z
          vis = item.visibility
          if (useVisibility):
             if (vis<visibilityThreshold):
               x   = 0.0
               y   = 0.0
               z   = 0.0
               vis = 0.0
             else:
               if (flipX): 
                 x = 1.0 - x #Do Flip X
               vis = 1.0
          else:
             if (x!=0.0) or (y!=0.0):
                if (flipX): 
                  x   = 1.0 - x #Do Flip X
                vis = 1.0
          #--------------------------------

          #--------------------------------
          x2D,y2D = normalize2DPointWhileAlsoMatchingTrainingAspectRatio(x,y,currentAspectRatio)
          #--------------------------------

          #--------------------------------
          #First of all for some reason mediapipe images get flipped so we undo this
          #Second the camera used is different that the trained one so we need to fix the aspect ratio 
          #---------------------------------------
          labelX = "2dx_"+thisLandmarkName
          mnetPose2D[labelX] = x2D # <- we store the corrected 2D point
          labelY = "2dy_"+thisLandmarkName
          mnetPose2D[labelY] = y2D
          labelV = "visible_"+thisLandmarkName

          if (x2D<0) or (x2D>1.0) or (y2D<0) or (y2D>1.0):
              vis=0.0 #Update visibility on the fly

          mnetPose2D[labelV] = vis
          #--------------------------------------- 
          #DEBUG WHAT IS HAPPENING HERE
          #python3 mediapipeHolisticWebcamMocapNET.py --aspectCorrection 0.8  --from damien.avi --face --nobody
          #print("Joint ",thisLandmarkName,"(",itemNumber,") x=",x," y=",y," z=",z," vis=",vis)
        itemNumber = itemNumber +1
   else:
        #print(bcolors.FAIL,"processPoseLandmarks without a Mediapipe Pose(?)",bcolors.ENDC)
        pass
   return mnetPose2D
#-------------------------------------------------------------------------------------------------------------------
def guessLandmarks(mnetPose2D):
   if mnetPose2D is not None:
        #---------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------
        #---------------------------------------------------
        if ("2dx_head_reye" in mnetPose2D) and ("2dy_head_reye" in mnetPose2D) and ("visible_head_reye" in mnetPose2D):
              #---------------------------------------------
              rEyeX = float(mnetPose2D["2dx_head_reye"])
              rEyeY = float(mnetPose2D["2dy_head_reye"])
              rEyeV = float(mnetPose2D["visible_head_reye"])
              #---------------------------------------------
              if (rEyeV>0.0):
                 mnetPose2D["2dx_endsite_eye.r"]     = rEyeX
                 mnetPose2D["2dy_endsite_eye.r"]     = rEyeY
                 mnetPose2D["visible_endsite_eye.r"] = rEyeV
        #---------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------
        #---------------------------------------------------
        if ("2dx_head_leye" in mnetPose2D) and ("2dy_head_leye" in mnetPose2D) and ("visible_head_leye" in mnetPose2D):
              #---------------------------------------------
              lEyeX = float(mnetPose2D["2dx_head_leye"])
              lEyeY = float(mnetPose2D["2dy_head_leye"])
              lEyeV = float(mnetPose2D["visible_head_leye"])
              #---------------------------------------------
              if (lEyeV>0.0):
                 mnetPose2D["2dx_endsite_eye.l"]     = lEyeX
                 mnetPose2D["2dy_endsite_eye.l"]     = lEyeY
                 mnetPose2D["visible_endsite_eye.l"] = lEyeV
        #---------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------
        #---------------------------------------------------
        if ("2dx_head_nosebone_3" in mnetPose2D) and ("2dy_head_nosebone_3" in mnetPose2D) and ("visible_head_nosebone_3" in mnetPose2D):
              #---------------------------------------------
              headX = float(mnetPose2D["2dx_head_nosebone_3"])
              headY = float(mnetPose2D["2dy_head_nosebone_3"])
              headV = float(mnetPose2D["visible_head_nosebone_3"])
              #---------------------------------------------
              if (headV>0.0):
                 mnetPose2D["2dx_head"]     = headX
                 mnetPose2D["2dy_head"]     = headY
                 mnetPose2D["visible_head"] = headV
        #---------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------
        #---------------------------------------------------




        #---------------------------------------------------
        if ("2dx_rshoulder" in mnetPose2D) and ("2dy_rshoulder" in mnetPose2D) and ("visible_rshoulder" in mnetPose2D) and ("2dx_lshoulder" in mnetPose2D) and ("2dy_lshoulder" in mnetPose2D) and ("visible_lshoulder" in mnetPose2D):
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
                 mnetPose2D["2dx_neck"]     = float((rX+lX)/2)
                 mnetPose2D["2dy_neck"]     = float((rY+lY)/2)
                 mnetPose2D["visible_neck"] = float((rV+lV)/2)
                 #--------------------------------------
                 mnetPose2D["2dx_neck1"]     = float((rX+lX)/2)
                 mnetPose2D["2dy_neck1"]     = float((rY+lY)/2)
                 mnetPose2D["visible_neck1"] = float((rV+lV)/2)
        #---------------------------------------------------
        #---------------------------------------------------
        #---------------------------------------------------
        if ("2dx_rhip" in mnetPose2D) and ("2dx_rhip" in mnetPose2D) and ("visible_rhip" in mnetPose2D) and ("2dx_lhip" in mnetPose2D) and ("2dy_lhip" in mnetPose2D) and ("visible_lhip" in mnetPose2D) :
              #---------------------------------------------
              rX = float(mnetPose2D["2dx_rhip"])
              rY = float(mnetPose2D["2dy_rhip"])
              rV = float(mnetPose2D["visible_rhip"])
              #---------------------------------------------
              lX = float(mnetPose2D["2dx_lhip"])
              lY = float(mnetPose2D["2dy_lhip"])
              lV = float(mnetPose2D["visible_lhip"])
              #---------------------------------------------
              if (rV>0.0) and (lV>0.0):
                 mnetPose2D["2dx_hip"]     = float((rX+lX)/2)
                 mnetPose2D["2dy_hip"]     = float((rY+lY)/2)
                 mnetPose2D["visible_hip"] = float((rV+lV)/2)
        #---------------------------------------------------
        #---------------------------------------------------
        #---------------------------------------------------
        if ("2dx_endsite_toe1-2.l" in mnetPose2D) and ("2dy_endsite_toe1-2.l" in mnetPose2D) and ("visible_endsite_toe1-2.l" in mnetPose2D) :
              #---------------------------------------------
              mnetPose2D["2dx_endsite_toe5-3.l"]     = mnetPose2D["2dx_endsite_toe1-2.l"]
              mnetPose2D["2dy_endsite_toe5-3.l"]     = mnetPose2D["2dy_endsite_toe1-2.l"]
              mnetPose2D["visible_endsite_toe5-3.l"] = mnetPose2D["visible_endsite_toe1-2.l"]
        #---------------------------------------------------
        #---------------------------------------------------
        #---------------------------------------------------
        if ("2dx_endsite_toe1-2.r" in mnetPose2D) and ("2dy_endsite_toe1-2.r" in mnetPose2D) and ("visible_endsite_toe1-2.r" in mnetPose2D) :
              #---------------------------------------------
              mnetPose2D["2dx_endsite_toe5-3.r"]     = mnetPose2D["2dx_endsite_toe1-2.r"]
              mnetPose2D["2dy_endsite_toe5-3.r"]     = mnetPose2D["2dy_endsite_toe1-2.r"]
              mnetPose2D["visible_endsite_toe5-3.r"] = mnetPose2D["visible_endsite_toe1-2.r"]
        #---------------------------------------------------
        #---------------------------------------------------
        #---------------------------------------------------
        if ("2dx_rear" in mnetPose2D) and ("2dy_rear" in mnetPose2D) and ("visible_rear" in mnetPose2D) :
              #---------------------------------------------
              mnetPose2D["2dx_ear.r"]     = mnetPose2D["2dx_rear"]
              mnetPose2D["2dy_ear.r"]     = mnetPose2D["2dy_rear"]
              mnetPose2D["visible_ear.r"] = mnetPose2D["visible_rear"]
        #---------------------------------------------------
        #---------------------------------------------------
        #---------------------------------------------------
        if ("2dx_rear" in mnetPose2D) and ("2dy_rear" in mnetPose2D) and ("visible_rear" in mnetPose2D) :
              #---------------------------------------------
              mnetPose2D["2dx___temporalis02.r"]     = mnetPose2D["2dx_rear"]
              mnetPose2D["2dy___temporalis02.r"]     = mnetPose2D["2dy_rear"]
              mnetPose2D["visible___temporalis02.r"] = mnetPose2D["visible_rear"]
              mnetPose2D["2dx_ear.r"]     = mnetPose2D["2dx_rear"]
              mnetPose2D["2dy_ear.r"]     = mnetPose2D["2dy_rear"]
              mnetPose2D["visible_ear.r"] = mnetPose2D["visible_rear"]
        #---------------------------------------------------
        #---------------------------------------------------
        #---------------------------------------------------
        if ("2dx_lear" in mnetPose2D) and ("2dy_lear" in mnetPose2D) and ("visible_lear" in mnetPose2D) :
              #---------------------------------------------
              mnetPose2D["2dx___temporalis02.l"]     = mnetPose2D["2dx_lear"]
              mnetPose2D["2dy___temporalis02.l"]     = mnetPose2D["2dy_lear"]
              mnetPose2D["visible___temporalis02.l"] = mnetPose2D["visible_lear"]
              mnetPose2D["2dx_ear.l"]     = mnetPose2D["2dx_lear"]
              mnetPose2D["2dy_ear.l"]     = mnetPose2D["2dy_lear"]
              mnetPose2D["visible_ear.l"] = mnetPose2D["visible_lear"]
        #---------------------------------------------------
        #---------------------------------------------------
        #---------------------------------------------------
        if ("2dx_neck" in mnetPose2D) and ("2dy_neck" in mnetPose2D) and ("visible_neck" in mnetPose2D):
              #---------------------------------------------
              neckX = float(mnetPose2D["2dx_neck"])
              neckY = float(mnetPose2D["2dy_neck"])
              neckV = float(mnetPose2D["visible_neck"])
              #---------------------------------------------
              if (neckV>0.0):
                 mnetPose2D["2dx_neck1"]     = neckX
                 mnetPose2D["2dy_neck1"]     = neckY
                 mnetPose2D["visible_neck1"] = neckV
        #---------------------------------------------------
        #---------------------------------------------------
        #---------------------------------------------------

   return mnetPose2D







def dumpPoseNETInputTile(pose2D):
  global dumpedFrameForTiles
  jointLabels = getBody25BodyNameList() #getBody25NameList()
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





def processPoseNETLandmarks(correctLabels,poseNETPose,currentAspectRatio,trainedAspectRatio,threshold=0.01,doFlipX=False,doFrameDumpingForTiles=False):
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
   if (doFrameDumpingForTiles):
     dumpPoseNETInputTile(mnetPose2D)

   return mnetPose2D
#---------------------------------------------------










#MocapNET list of expected inputs
#frameNumber,skeletonID,totalSkeletons,2DX_head,2DY_head,visible_head,2DX_neck,2DY_neck,visible_neck,2DX_rshoulder,2DY_rshoulder,visible_rshoulder,2DX_relbow,2DY_relbow,visible_relbow,2DX_rhand,2DY_rhand,visible_rhand,2DX_lshoulder,2DY_lshoulder,visible_lshoulder,2DX_lelbow,2DY_lelbow,visible_lelbow,2DX_lhand,2DY_lhand,visible_lhand,2DX_hip,2DY_hip,visible_hip,2DX_rhip,2DY_rhip,visible_rhip,2DX_rknee,2DY_rknee,visible_rknee,2DX_rfoot,2DY_rfoot,visible_rfoot,2DX_lhip,2DY_lhip,visible_lhip,2DX_lknee,2DY_lknee,visible_lknee,2DX_lfoot,2DY_lfoot,visible_lfoot,2DX_endsite_eye.r,2DY_endsite_eye.r,visible_endsite_eye.r,2DX_endsite_eye.l,2DY_endsite_eye.l,visible_endsite_eye.l,2DX_rear,2DY_rear,visible_rear,2DX_lear,2DY_lear,visible_lear,2DX_endsite_toe1-2.l,2DY_endsite_toe1-2.l,visible_endsite_toe1-2.l,2DX_endsite_toe5-3.l,2DY_endsite_toe5-3.l,visible_endsite_toe5-3.l,2DX_lheel,2DY_lheel,visible_lheel,2DX_endsite_toe1-2.r,2DY_endsite_toe1-2.r,visible_endsite_toe1-2.r,2DX_endsite_toe5-3.r,2DY_endsite_toe5-3.r,visible_endsite_toe5-3.r,2DX_rheel,2DY_rheel,visible_rheel,2DX_bkg,2DY_bkg,visible_bkg,


def getHolisticBodyNameList():
 bn=list()
 #---------------------------------------------------
 bn.append("head")              #0  - nose
 bn.append("head_leye_0")       #1  - left_eye_inner
 bn.append("endsite_eye.l")     #2  - left_eye eye.l
 bn.append("head_leye_3")       #3  - left_eye_outer
 bn.append("head_reye_3")       #4  - right_eye_inner
 bn.append("endsite_eye.r")     #5  - right_eye eye.r
 bn.append("head_reye_0")       #6  - right_eye_outer
 bn.append("lear")              #7  - left_ear   | ear.l  | __temporalis02.l
 bn.append("rear")              #8  - right_ear  | ear.r  | __temporalis02.r
 bn.append("head_outmouth_0")   #9  - mouth_left
 bn.append("head_outmouth_6")   #10 - mouth_right 
 bn.append("lshoulder")         #11 - left_shoulder
 bn.append("rshoulder")         #12 - right_shoulder
 bn.append("lelbow")            #13 - left_elbow
 bn.append("relbow")            #14 - right_elbow
 bn.append("lhand")             #15 - left_wrist
 bn.append("rhand")             #16 - right_wrist
 bn.append("finger5-3.l")       #17 - left_pinky
 bn.append("finger5-3.r")       #18 - right_pinky
 bn.append("finger2-3.l")       #19 - left_index
 bn.append("finger2-3.r")       #20 - right_index
 bn.append("finger1-3.l")       #21 - left_thumb
 bn.append("finger1-3.r")       #22 - right_thumb
 bn.append("lhip")              #23 - left_hip
 bn.append("rhip")              #24 - right_hip
 bn.append("lknee")             #25 - left_knee
 bn.append("rknee")             #26 - right_knee
 bn.append("lfoot")             #27 - left_ankle
 bn.append("rfoot")             #28 - right_ankle
 bn.append("lheel")             #29 - left_heel
 bn.append("rheel")             #30 - right_heel
 bn.append("endsite_toe1-2.l")  #31 - left_foot_index
 bn.append("endsite_toe1-2.r")  #32 - right_foot_index
 return bn
#---------------------------------------------------


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

def getBody25BodyNameList():
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
 bn.append("endsite_eye.r")     #15
 bn.append("endsite_eye.l")     #16
 bn.append("rear")              #17
 bn.append("lear")              #18
 bn.append("endsite_toe1-2.l")  #19
 bn.append("endsite_toe5-3.l")  #20
 bn.append("lheel")             #21
 bn.append("endsite_toe1-2.r")  #22
 bn.append("endsite_toe5-3.r")  #23
 bn.append("rheel")             #24
 #bn.append("bkg")               #25
 return bn
#---------------------------------------------------




#2DX_lhand,2DY_lhand,visible_lhand,2DX_lthumb,2DY_lthumb,visible_lthumb,2DX_finger1-2.l,2DY_finger1-2.l,visible_finger1-2.l,2DX_finger1-3.l,2DY_finger1-3.l,visible_finger1-3.l,2DX_endsite_finger1-3.l,2DY_endsite_finger1-3.l,visible_endsite_finger1-3.l,2DX_finger2-1.l,2DY_finger2-1.l,visible_finger2-1.l,2DX_finger2-2.l,2DY_finger2-2.l,visible_finger2-2.l,2DX_finger2-3.l,2DY_finger2-3.l,visible_finger2-3.l,2DX_endsite_finger2-3.l,2DY_endsite_finger2-3.l,visible_endsite_finger2-3.l,2DX_finger3-1.l,2DY_finger3-1.l,visible_finger3-1.l,2DX_finger3-2.l,2DY_finger3-2.l,visible_finger3-2.l,2DX_finger3-3.l,2DY_finger3-3.l,visible_finger3-3.l,2DX_endsite_finger3-3.l,2DY_endsite_finger3-3.l,visible_endsite_finger3-3.l,2DX_finger4-1.l,2DY_finger4-1.l,visible_finger4-1.l,2DX_finger4-2.l,2DY_finger4-2.l,visible_finger4-2.l,2DX_finger4-3.l,2DY_finger4-3.l,visible_finger4-3.l,2DX_endsite_finger4-3.l,2DY_endsite_finger4-3.l,visible_endsite_finger4-3.l,2DX_finger5-1.l,2DY_finger5-1.l,visible_finger5-1.l,2DX_finger5-2.l,2DY_finger5-2.l,visible_finger5-2.l,2DX_finger5-3.l,2DY_finger5-3.l,visible_finger5-3.l,2DX_endsite_finger5-3.l,2DY_endsite_finger5-3.l,visible_endsite_finger5-3.l
def getHolisticLHandNameList():
 bn=list()
 #---------------------------------------------------
 bn.append("lhand")                    #0   - wrist
 bn.append("lthumb")                   #1   - thumb_cmc
 bn.append("finger1-2.l")              #2   - thumb_mcp
 bn.append("finger1-3.l")              #3   - thumb_ip
 bn.append("endsite_finger1-3.l")      #4   - thumb_tip
 bn.append("finger2-1.l")              #5   - index_finger_mcp
 bn.append("finger2-2.l")              #6   - index_finger_pip
 bn.append("finger2-3.l")              #7   - index_finger_dip
 bn.append("endsite_finger2-3.l")      #8   - index_finger_tip
 bn.append("finger3-1.l")              #9   - middle_finger_mcp
 bn.append("finger3-2.l")              #10  - middle_finger_pip
 bn.append("finger3-3.l")              #11  - middle_finger_dip
 bn.append("endsite_finger3-3.l")      #12  - middle_finger_tip
 bn.append("finger4-1.l")              #13  - ring_finger_mcp
 bn.append("finger4-2.l")              #14  - ring_finger_pip
 bn.append("finger4-3.l")              #15  - ring_finger_dip
 bn.append("endsite_finger4-3.l")      #16  - ring_tip
 bn.append("finger5-1.l")              #17  - pinky_mcp
 bn.append("finger5-2.l")              #18  - pinky_pip
 bn.append("finger5-3.l")              #19  - pinky_dip
 bn.append("endsite_finger5-3.l")      #20  - pinky_tip
 return bn
#---------------------------------------------------




#2DX_rhand,2DY_rhand,visible_rhand,2DX_rthumb,2DY_rthumb,visible_rthumb,2DX_finger1-2.r,2DY_finger1-2.r,visible_finger1-2.r,2DX_finger1-3.r,2DY_finger1-3.r,visible_finger1-3.r,2DX_endsite_finger1-3.r,2DY_endsite_finger1-3.r,visible_endsite_finger1-3.r,2DX_finger2-1.r,2DY_finger2-1.r,visible_finger2-1.r,2DX_finger2-2.r,2DY_finger2-2.r,visible_finger2-2.r,2DX_finger2-3.r,2DY_finger2-3.r,visible_finger2-3.r,2DX_endsite_finger2-3.r,2DY_endsite_finger2-3.r,visible_endsite_finger2-3.r,2DX_finger3-1.r,2DY_finger3-1.r,visible_finger3-1.r,2DX_finger3-2.r,2DY_finger3-2.r,visible_finger3-2.r,2DX_finger3-3.r,2DY_finger3-3.r,visible_finger3-3.r,2DX_endsite_finger3-3.r,2DY_endsite_finger3-3.r,visible_endsite_finger3-3.r,2DX_finger4-1.r,2DY_finger4-1.r,visible_finger4-1.r,2DX_finger4-2.r,2DY_finger4-2.r,visible_finger4-2.r,2DX_finger4-3.r,2DY_finger4-3.r,visible_finger4-3.r,2DX_endsite_finger4-3.r,2DY_endsite_finger4-3.r,visible_endsite_finger4-3.r,2DX_finger5-1.r,2DY_finger5-1.r,visible_finger5-1.r,2DX_finger5-2.r,2DY_finger5-2.r,visible_finger5-2.r,2DX_finger5-3.r,2DY_finger5-3.r,visible_finger5-3.r,2DX_endsite_finger5-3.r,2DY_endsite_finger5-3.r,visible_endsite_finger5-3.r
def getHolisticRHandNameList():
 bn=list()
 #---------------------------------------------------
 bn.append("rhand")                    #0   - wrist
 bn.append("rthumb")                   #1   - thumb_cmc
 bn.append("finger1-2.r")              #2   - thumb_mcp
 bn.append("finger1-3.r")              #3   - thumb_ip
 bn.append("endsite_finger1-3.r")      #4   - thumb_tip
 bn.append("finger2-1.r")              #5   - index_finger_mcp
 bn.append("finger2-2.r")              #6   - index_finger_pip
 bn.append("finger2-3.r")              #7   - index_finger_dip
 bn.append("endsite_finger2-3.r")      #8   - index_finger_tip
 bn.append("finger3-1.r")              #9   - middle_finger_mcp
 bn.append("finger3-2.r")              #10  - middle_finger_pip
 bn.append("finger3-3.r")              #11  - middle_finger_dip
 bn.append("endsite_finger3-3.r")      #12  - middle_finger_tip
 bn.append("finger4-1.r")              #13  - ring_finger_mcp
 bn.append("finger4-2.r")              #14  - ring_finger_pip
 bn.append("finger4-3.r")              #15  - ring_finger_dip
 bn.append("endsite_finger4-3.r")      #16  - ring_tip
 bn.append("finger5-1.r")              #17  - pinky_mcp
 bn.append("finger5-2.r")              #18  - pinky_pip
 bn.append("finger5-3.r")              #19  - pinky_dip
 bn.append("endsite_finger5-3.r")      #20  - pinky_tip
 return bn
#---------------------------------------------------

#THIS IS HERE FOR COMPATIBILITY
def getOpenPoseFaceNameList():
 bn=list()
 #---------------------------------------------------
 bn.append("head_rchin_0")         # 0
 bn.append("head_rchin_1")         # 1
 bn.append("head_rchin_2")         # 2     
 bn.append("head_rchin_3")         # 3
 bn.append("head_rchin_4")         # 4
 bn.append("head_rchin_5")         # 5
 bn.append("head_rchin_6")         # 6
 bn.append("head_rchin_7")         # 7
 bn.append("head_chin")            # 8
 bn.append("head_lchin_7")         # 9
 bn.append("head_lchin_6")         # 10
 bn.append("head_lchin_5")         # 11
 bn.append("head_lchin_4")         # 12
 bn.append("head_lchin_3")         # 13
 bn.append("head_lchin_2")         # 14
 bn.append("head_lchin_1")         # 15
 bn.append("head_lchin_0")         # 16
 bn.append("head_reyebrow_0")      # 17
 bn.append("head_reyebrow_1")      # 18
 bn.append("head_reyebrow_2")      # 19
 bn.append("head_reyebrow_3")      # 20
 bn.append("head_reyebrow_4")      # 21
 bn.append("head_leyebrow_4")      # 22
 bn.append("head_leyebrow_3")      # 23
 bn.append("head_leyebrow_2")      # 24
 bn.append("head_leyebrow_1")      # 25
 bn.append("head_leyebrow_0")      # 26
 bn.append("head_nosebone_0")      # 27 
 bn.append("head_nosebone_1")      # 28
 bn.append("head_nosebone_2")      # 29
 bn.append("head_nosebone_3")      # 30
 bn.append("head_nostrills_0")     # 31 
 bn.append("head_nostrills_1")     # 32 
 bn.append("head_nostrills_2")     # 33 
 bn.append("head_nostrills_3")     # 34 
 bn.append("head_nostrills_4")     # 35 
 bn.append("head_reye_0")          # 36 
 bn.append("head_reye_1")          # 37 
 bn.append("head_reye_2")          # 38 
 bn.append("head_reye_3")          # 39 
 bn.append("head_reye_4")          # 40 
 bn.append("head_reye_5")          # 41 
 bn.append("head_leye_0")          # 42 
 bn.append("head_leye_1")          # 43 
 bn.append("head_leye_2")          # 44 
 bn.append("head_leye_3")          # 45 
 bn.append("head_leye_4")          # 46 
 bn.append("head_leye_5")          # 47 
 bn.append("head_outmouth_0")      # 48 
 bn.append("head_outmouth_1")      # 49 
 bn.append("head_outmouth_2")      # 50 
 bn.append("head_outmouth_3")      # 51 
 bn.append("head_outmouth_4")      # 52 
 bn.append("head_outmouth_5")      # 53 
 bn.append("head_outmouth_6")      # 54 
 bn.append("head_outmouth_7")      # 55 
 bn.append("head_outmouth_8")      # 56 
 bn.append("head_outmouth_9")      # 57 
 bn.append("head_outmouth_10")     # 58 
 bn.append("head_outmouth_11")     # 59 
 bn.append("head_inmouth_0")       # 60
 bn.append("head_inmouth_1")       # 61
 bn.append("head_inmouth_2")       # 62
 bn.append("head_inmouth_3")       # 63
 bn.append("head_inmouth_4")       # 64
 bn.append("head_inmouth_5")       # 65
 bn.append("head_inmouth_6")       # 66
 bn.append("head_inmouth_7")       # 67
 bn.append("head_reye")            # 68 
 bn.append("head_leye")            # 69  
 return bn
#---------------------------------------------------

#2DX_head_rchin_0,2DY_head_rchin_0,visible_head_rchin_0,2DX_head_rchin_1,2DY_head_rchin_1,visible_head_rchin_1,2DX_head_rchin_2,2DY_head_rchin_2,visible_head_rchin_2,2DX_head_rchin_3,2DY_head_rchin_3,visible_head_rchin_3,2DX_head_rchin_4,2DY_head_rchin_4,visible_head_rchin_4,2DX_head_rchin_5,2DY_head_rchin_5,visible_head_rchin_5,2DX_head_rchin_6,2DY_head_rchin_6,visible_head_rchin_6,2DX_head_rchin_7,2DY_head_rchin_7,visible_head_rchin_7,2DX_head_chin,2DY_head_chin,visible_head_chin,2DX_head_lchin_7,2DY_head_lchin_7,visible_head_lchin_7,2DX_head_lchin_6,2DY_head_lchin_6,visible_head_lchin_6,2DX_head_lchin_5,2DY_head_lchin_5,visible_head_lchin_5,2DX_head_lchin_4,2DY_head_lchin_4,visible_head_lchin_4,2DX_head_lchin_3,2DY_head_lchin_3,visible_head_lchin_3,2DX_head_lchin_2,2DY_head_lchin_2,visible_head_lchin_2,2DX_head_lchin_1,2DY_head_lchin_1,visible_head_lchin_1,2DX_head_lchin_0,2DY_head_lchin_0,visible_head_lchin_0,2DX_head_reyebrow_0,2DY_head_reyebrow_0,visible_head_reyebrow_0,2DX_head_reyebrow_1,2DY_head_reyebrow_1,visible_head_reyebrow_1,2DX_head_reyebrow_2,2DY_head_reyebrow_2,visible_head_reyebrow_2,2DX_head_reyebrow_3,2DY_head_reyebrow_3,visible_head_reyebrow_3,2DX_head_reyebrow_4,2DY_head_reyebrow_4,visible_head_reyebrow_4,2DX_head_leyebrow_4,2DY_head_leyebrow_4,visible_head_leyebrow_4,2DX_head_leyebrow_3,2DY_head_leyebrow_3,visible_head_leyebrow_3,2DX_head_leyebrow_2,2DY_head_leyebrow_2,visible_head_leyebrow_2,2DX_head_leyebrow_1,2DY_head_leyebrow_1,visible_head_leyebrow_1,2DX_head_leyebrow_0,2DY_head_leyebrow_0,visible_head_leyebrow_0,2DX_head_nosebone_0,2DY_head_nosebone_0,visible_head_nosebone_0,2DX_head_nosebone_1,2DY_head_nosebone_1,visible_head_nosebone_1,2DX_head_nosebone_2,2DY_head_nosebone_2,visible_head_nosebone_2,2DX_head_nosebone_3,2DY_head_nosebone_3,visible_head_nosebone_3,2DX_head_nostrills_0,2DY_head_nostrills_0,visible_head_nostrills_0,2DX_head_nostrills_1,2DY_head_nostrills_1,visible_head_nostrills_1,2DX_head_nostrills_2,2DY_head_nostrills_2,visible_head_nostrills_2,2DX_head_nostrills_3,2DY_head_nostrills_3,visible_head_nostrills_3,2DX_head_nostrills_4,2DY_head_nostrills_4,visible_head_nostrills_4,2DX_head_reye_0,2DY_head_reye_0,visible_head_reye_0,2DX_head_reye_1,2DY_head_reye_1,visible_head_reye_1,2DX_head_reye_2,2DY_head_reye_2,visible_head_reye_2,2DX_head_reye_3,2DY_head_reye_3,visible_head_reye_3,2DX_head_reye_4,2DY_head_reye_4,visible_head_reye_4,2DX_head_reye_5,2DY_head_reye_5,visible_head_reye_5,2DX_head_leye_0,2DY_head_leye_0,visible_head_leye_0,2DX_head_leye_1,2DY_head_leye_1,visible_head_leye_1,2DX_head_leye_2,2DY_head_leye_2,visible_head_leye_2,2DX_head_leye_3,2DY_head_leye_3,visible_head_leye_3,2DX_head_leye_4,2DY_head_leye_4,visible_head_leye_4,2DX_head_leye_5,2DY_head_leye_5,visible_head_leye_5,2DX_head_outmouth_0,2DY_head_outmouth_0,visible_head_outmouth_0,2DX_head_outmouth_1,2DY_head_outmouth_1,visible_head_outmouth_1,2DX_head_outmouth_2,2DY_head_outmouth_2,visible_head_outmouth_2,2DX_head_outmouth_3,2DY_head_outmouth_3,visible_head_outmouth_3,2DX_head_outmouth_4,2DY_head_outmouth_4,visible_head_outmouth_4,2DX_head_outmouth_5,2DY_head_outmouth_5,visible_head_outmouth_5,2DX_head_outmouth_6,2DY_head_outmouth_6,visible_head_outmouth_6,2DX_head_outmouth_7,2DY_head_outmouth_7,visible_head_outmouth_7,2DX_head_outmouth_8,2DY_head_outmouth_8,visible_head_outmouth_8,2DX_head_outmouth_9,2DY_head_outmouth_9,visible_head_outmouth_9,2DX_head_outmouth_10,2DY_head_outmouth_10,visible_head_outmouth_10,2DX_head_outmouth_11,2DY_head_outmouth_11,visible_head_outmouth_11,2DX_head_inmouth_0,2DY_head_inmouth_0,visible_head_inmouth_0,2DX_head_inmouth_1,2DY_head_inmouth_1,visible_head_inmouth_1,2DX_head_inmouth_2,2DY_head_inmouth_2,visible_head_inmouth_2,2DX_head_inmouth_3,2DY_head_inmouth_3,visible_head_inmouth_3,2DX_head_inmouth_4,2DY_head_inmouth_4,visible_head_inmouth_4,2DX_head_inmouth_5,2DY_head_inmouth_5,visible_head_inmouth_5,2DX_head_inmouth_6,2DY_head_inmouth_6,visible_head_inmouth_6,2DX_head_inmouth_7,2DY_head_inmouth_7,visible_head_inmouth_7,2DX_head_reye,2DY_head_reye,visible_head_reye,2DX_head_leye,2DY_head_leye,visible_head_leye
def getHolisticFaceNameList(useNumbers=False):
       #Last Update : 27/03/23 (Automatic via associate2D files)
      bn=list()
      if (useNumbers):
        for i in range(0,486):
           bn.append("head_%u"%i)
        return bn
 #---------------------------------------------------
 #IMPORTANT MAKE SURE TO KEEP THIS IN SYNC WITH holisticDumped.py
 #---------------------------------------------------
      bn.append("head_outmouth_3")     #0
      bn.append("head")     #1
      bn.append("head_nostrills_2")     #2
      bn.append("")     #3
      bn.append("head_nosebone_3")     #4
      bn.append("head_nosebone_2")     #5
      bn.append("")     #6
      bn.append("")     #7
      bn.append("")     #8
      bn.append("")     #9
      bn.append("")     #10
      bn.append("")     #11 head_outmouth_3
      bn.append("head_inmouth_2")     #12
      bn.append("")     #13head_inmouth_2
      bn.append("")     #14head_inmouth_6
      bn.append("")     #15
      bn.append("head_inmouth_6")     #16
      bn.append("head_outmouth_9")     #17head_outmouth_9
      bn.append("")     #18
      bn.append("head_nostrills_2")     #19
      bn.append("")     #20
      bn.append("")     #21
      bn.append("")     #22
      bn.append("")     #23
      bn.append("")     #24
      bn.append("")     #25
      bn.append("")     #26
      bn.append("")     #27
      bn.append("")     #28
      bn.append("")     #29
      bn.append("")     #30
      bn.append("")     #31
      bn.append("")     #32
      bn.append("head_reye_0")     #33 was head_reye_3
      bn.append("head_rchin_0")     #34 was head_rchin_0
      bn.append("")     #35
      bn.append("")     #36
      bn.append("head_outmouth_2")     #37head_outmouth_4
      bn.append("head_inmouth_1")     #38
      bn.append("")     #39
      bn.append("head_outmouth_1")     #40
      bn.append("")     #41
      bn.append("")     #42
      bn.append("")     #43
      bn.append("")     #44
      bn.append("")     #45
      bn.append("")     #46
      bn.append("")     #47
      bn.append("")     #48
      bn.append("")     #49
      bn.append("")     #50
      bn.append("")     #51
      bn.append("head_reyebrow_2")     #52 was head_reyebrow_2
      bn.append("head_reyebrow_1")     #53 was head_reyebrow_3
      bn.append("")     #54
      bn.append("head_reyebrow_4")     #55 was head_reyebrow_0
      bn.append("")     #56
      bn.append("")     #57
      bn.append("")     #58 was head_lchin_3
      bn.append("")     #59
      bn.append("")     #60
      bn.append("head_outmouth_0")     #61
      bn.append("")     #62
      bn.append("")     #63
      bn.append("")     #64
      bn.append("head_reyebrow_3")     #65 was head_reyebrow_1
      bn.append("")     #66 head_leyebrow_3
      bn.append("")     #67
      bn.append("")     #68
      bn.append("")     #69
      bn.append("head_reyebrow_0")     #70
      bn.append("")     #71
      bn.append("")     #72
      bn.append("")     #73head_outmouth_5
      bn.append("")     #74
      bn.append("")     #75
      bn.append("")     #76
      bn.append("")     #77
      bn.append("head_inmouth_0")     #78
      bn.append("head_nostrills_1")     #79
      bn.append("")     #80
      bn.append("")     #81
      bn.append("")     #82head_inmouth_3
      bn.append("")     #83
      bn.append("head_outmouth_10")     #84head_outmouth_8
      bn.append("")     #85
      bn.append("head_inmouth_7")     #86
      bn.append("")     #87head_inmouth_5
      bn.append("")     #88
      bn.append("")     #89
      bn.append("")     #90
      bn.append("head_outmouth_11")     #91
      bn.append("")     #92
      bn.append("")     #93
      bn.append("")     #94
      bn.append("")     #95
      bn.append("")     #96
      bn.append("")     #97
      bn.append("")     #98
      bn.append("")     #99
      bn.append("")     #100
      bn.append("")     #101
      bn.append("head_nostrills_0")     #102
      bn.append("")     #103
      bn.append("")     #104
      bn.append("")     #105 head_leyebrow_2
      bn.append("")     #106
      bn.append("")     #107
      bn.append("")     #108
      bn.append("")     #109
      bn.append("")     #110
      bn.append("")     #111
      bn.append("")     #112
      bn.append("")     #113
      bn.append("")     #114
      bn.append("")     #115
      bn.append("")     #116
      bn.append("")     #117
      bn.append("")     #118
      bn.append("")     #119
      bn.append("")     #120
      bn.append("")     #121
      bn.append("")     #122
      bn.append("")     #123
      bn.append("")     #124
      bn.append("")     #125
      bn.append("")     #126
      bn.append("")     #127
      bn.append("")     #128
      bn.append("")     #129
      bn.append("")     #130
      bn.append("")     #131
      bn.append("")     #132
      bn.append("head_reye_3")     #133
      bn.append("")     #134
      bn.append("")     #135
      bn.append("")     #136
      bn.append("head_rchin_2")     #137 was head_rchin_1
      bn.append("head_rchin_5")     #138
      bn.append("")     #139
      bn.append("")     #140
      bn.append("")     #141
      bn.append("")     #142
      bn.append("")     #143
      bn.append("head_reye_5")     #144 head_reye_4
      bn.append("")     #145
      bn.append("")     #146
      bn.append("")     #147
      bn.append("")     #148 was head_rchin_7
      bn.append("")     #149 was head_rchin_6
      bn.append("")     #150 was head_rchin_5
      bn.append("")     #151
      bn.append("head_chin")     #152
      bn.append("head_reye_4")     #153  head_reye_5
      bn.append("")     #154
      bn.append("")     #155  head_reye_0
      bn.append("")     #156  was head_reyebrow_4
      bn.append("")     #157
      bn.append("head_reye_2")     #158 head_reye_1
      bn.append("")     #159
      bn.append("head_reye_1")     #160 head_reye_2
      bn.append("")     #161
      bn.append("")     #162
      bn.append("")     #163
      bn.append("")     #164
      bn.append("")     #165
      bn.append("")     #166
      bn.append("")     #167
      bn.append("head_nosebone_0")     #168
      bn.append("")     #169
      bn.append("head_rchin_6")     #170
      bn.append("head_rchin_7")     #171
      bn.append("")     #172 was head_rchin_4
      bn.append("")     #173
      bn.append("")     #174
      bn.append("")     #175
      bn.append("")     #176
      bn.append("head_rchin_3")     #177
      bn.append("")     #178
      bn.append("")     #179
      bn.append("")     #180head_outmouth_7
      bn.append("")     #181
      bn.append("")     #182
      bn.append("")     #183head_inmouth_4
      bn.append("")     #184
      bn.append("")     #185head_outmouth_6
      bn.append("")     #186
      bn.append("")     #187
      bn.append("")     #188
      bn.append("")     #189
      bn.append("")     #190
      bn.append("")     #191
      bn.append("")     #192
      bn.append("")     #193
      bn.append("")     #194
      bn.append("")     #195
      bn.append("")     #196
      bn.append("head_nosebone_1")     #197
      bn.append("")     #198
      bn.append("")     #199
      bn.append("")     #200
      bn.append("")     #201
      bn.append("")     #202
      bn.append("")     #203
      bn.append("")     #204
      bn.append("")     #205
      bn.append("")     #206
      bn.append("")     #207
      bn.append("")     #208
      bn.append("")     #209
      bn.append("")     #210
      bn.append("")     #211
      bn.append("")     #212
      bn.append("")     #213 was head_rchin_2
      bn.append("")     #214
      bn.append("head_rchin_4")     #215
      bn.append("")     #216
      bn.append("")     #217
      bn.append("")     #218
      bn.append("")     #219
      bn.append("")     #220
      bn.append("")     #221
      bn.append("")     #222
      bn.append("")     #223
      bn.append("")     #224
      bn.append("")     #225
      bn.append("")     #226
      bn.append("head_rchin_1")     #227 was head_rchin_0
      bn.append("")     #228
      bn.append("")     #229
      bn.append("")     #230
      bn.append("")     #231
      bn.append("")     #232
      bn.append("")     #233
      bn.append("")     #234
      bn.append("")     #235
      bn.append("")     #236
      bn.append("")     #237
      bn.append("")     #238
      bn.append("")     #239
      bn.append("")     #240
      bn.append("")     #241
      bn.append("")     #242
      bn.append("")     #243
      bn.append("")     #244
      bn.append("")     #245
      bn.append("")     #246
      bn.append("")     #247
      bn.append("")     #248
      bn.append("")     #249
      bn.append("")     #250
      bn.append("")     #251
      bn.append("")     #252
      bn.append("")     #253
      bn.append("")     #254
      bn.append("")     #255
      bn.append("")     #256
      bn.append("")     #257
      bn.append("")     #258
      bn.append("")     #259
      bn.append("")     #260
      bn.append("")     #261
      bn.append("")     #262
      bn.append("head_leye_3")     #263
      bn.append("head_lchin_0")     #264 was head_lchin_0
      bn.append("")     #265
      bn.append("")     #266
      bn.append("head_outmouth_4")     #267head_outmouth_2
      bn.append("head_inmouth_3")     #268
      bn.append("")     #269head_outmouth_1
      bn.append("head_outmouth_5")     #270
      bn.append("")     #271
      bn.append("")     #272
      bn.append("")     #273
      bn.append("")     #274
      bn.append("")     #275
      bn.append("")     #276
      bn.append("")     #277
      bn.append("")     #278
      bn.append("")     #279
      bn.append("")     #280
      bn.append("")     #281
      bn.append("head_leyebrow_2")     #282 was head_leyebrow_2
      bn.append("head_leyebrow_1")     #283 was head_leyebrow_3
      bn.append("")     #284
      bn.append("head_leyebrow_4")     #285 was head_leyebrow_0
      bn.append("")     #286
      bn.append("")     #287
      bn.append("")     #288 was head_lchin_3
      bn.append("")     #289
      bn.append("")     #290
      bn.append("head_outmouth_6")     #291head_outmouth_0
      bn.append("")     #292
      bn.append("")     #293
      bn.append("")     #294
      bn.append("head_leyebrow_3")     #295 was head_leyebrow_1
      bn.append("")     #296
      bn.append("")     #297
      bn.append("")     #298
      bn.append("")     #299
      bn.append("head_leyebrow_0")     #300
      bn.append("")     #301
      bn.append("")     #302
      bn.append("")     #303
      bn.append("")     #304
      bn.append("")     #305
      bn.append("")     #306
      bn.append("")     #307
      bn.append("head_inmouth_4")     #308
      bn.append("head_nostrills_3")     #309
      bn.append("")     #310
      bn.append("")     #311
      bn.append("")     #312head_inmouth_1
      bn.append("")     #313
      bn.append("head_outmouth_8")     #314head_outmouth_10
      bn.append("")     #315
      bn.append("head_inmouth_5")     #316
      bn.append("")     #317head_inmouth_7
      bn.append("")     #318
      bn.append("")     #319
      bn.append("")     #320
      bn.append("head_outmouth_7")     #321
      bn.append("")     #322
      bn.append("")     #323 was head_lchin_1
      bn.append("")     #324head_inmouth_0
      bn.append("")     #325
      bn.append("")     #326
      bn.append("")     #327
      bn.append("")     #328
      bn.append("")     #329
      bn.append("")     #330
      bn.append("head_nostrills_4")     #331
      bn.append("")     #332
      bn.append("")     #333
      bn.append("")     #334
      bn.append("")     #335
      bn.append("")     #336
      bn.append("")     #337
      bn.append("")     #338
      bn.append("")     #339
      bn.append("")     #340
      bn.append("")     #341
      bn.append("")     #342
      bn.append("")     #343
      bn.append("")     #344
      bn.append("")     #345
      bn.append("")     #346
      bn.append("")     #347
      bn.append("")     #348
      bn.append("")     #349
      bn.append("")     #350
      bn.append("")     #351
      bn.append("")     #352
      bn.append("")     #353
      bn.append("")     #354
      bn.append("")     #355
      bn.append("")     #356
      bn.append("")     #357
      bn.append("")     #358
      bn.append("")     #359
      bn.append("")     #360
      bn.append("")     #361 was head_lchin_2
      bn.append("head_leye_0")     #362
      bn.append("")     #363
      bn.append("")     #364
      bn.append("")     #365
      bn.append("head_lchin_2")     #366
      bn.append("head_lchin_5")     #367
      bn.append("")     #368
      bn.append("")     #369
      bn.append("")     #370
      bn.append("")     #371
      bn.append("")     #372
      bn.append("head_leye_4")     #373
      bn.append("")     #374
      bn.append("")     #375
      bn.append("")     #376
      bn.append("")     #377 was head_lchin_7
      bn.append("")     #378 was head_lchin_6
      bn.append("")     #379 was head_lchin_5
      bn.append("head_leye_5")     #380 head_reye_4
      bn.append("")     #381
      bn.append("")     #382
      bn.append("")     #383 was head_leyebrow_4
      bn.append("")     #384
      bn.append("head_leye_1")     #385 head_reye_2
      bn.append("")     #386
      bn.append("head_leye_2")     #387 head_reye_1
      bn.append("")     #388
      bn.append("")     #389
      bn.append("")     #390 head_reye_5
      bn.append("")     #391
      bn.append("")     #392
      bn.append("")     #393
      bn.append("")     #394
      bn.append("head_lchin_6")     #395
      bn.append("head_lchin_7")     #396
      bn.append("")     #397 was head_lchin_4
      bn.append("")     #398
      bn.append("")     #399
      bn.append("")     #400
      bn.append("head_lchin_3")     #401
      bn.append("")     #402
      bn.append("")     #403
      bn.append("")     #404
      bn.append("")     #405head_outmouth_11
      bn.append("")     #406
      bn.append("")     #407
      bn.append("")     #408
      bn.append("")     #409
      bn.append("")     #410
      bn.append("")     #411
      bn.append("")     #412
      bn.append("")     #413
      bn.append("")     #414
      bn.append("")     #415
      bn.append("")     #416
      bn.append("")     #417
      bn.append("")     #418
      bn.append("")     #419
      bn.append("")     #420
      bn.append("")     #421
      bn.append("")     #422
      bn.append("")     #423
      bn.append("")     #424
      bn.append("")     #425
      bn.append("")     #426
      bn.append("")     #427
      bn.append("")     #428
      bn.append("")     #429
      bn.append("")     #430
      bn.append("")     #431
      bn.append("")     #432
      bn.append("")     #433
      bn.append("")     #434
      bn.append("head_lchin_4")     #435
      bn.append("")     #436
      bn.append("")     #437
      bn.append("")     #438
      bn.append("")     #439
      bn.append("")     #440
      bn.append("")     #441
      bn.append("")     #442
      bn.append("")     #443
      bn.append("")     #444
      bn.append("")     #445
      bn.append("")     #446
      bn.append("head_lchin_1")     #447 was head_lchin_0
      bn.append("")     #448
      bn.append("")     #449
      bn.append("")     #450
      bn.append("")     #451
      bn.append("")     #452
      bn.append("")     #453
      bn.append("")     #454
      bn.append("")     #455
      bn.append("")     #456
      bn.append("")     #457
      bn.append("")     #458
      bn.append("")     #459
      bn.append("")     #460
      bn.append("")     #461
      bn.append("")     #462
      bn.append("")     #463
      bn.append("")     #464
      bn.append("")     #465  head_reye_3
      bn.append("")     #466  head_reye_0
      bn.append("")     #467
      bn.append("")     #468
      bn.append("")     #469
      bn.append("")     #470
      bn.append("")     #471
      bn.append("")     #472
      bn.append("")     #473
      bn.append("")     #474
      bn.append("")     #475
      bn.append("")     #476
      bn.append("")     #477
      bn.append("")     #478
      bn.append("")     #479
      bn.append("")     #480
      bn.append("")     #481
      bn.append("")     #482
      bn.append("")     #483
      bn.append("")     #484
      bn.append("")     #485
      bn.append("")                    #486   -
      return bn
#---------------------------------------------------


if __name__ == '__main__':
    holistic = getHolisticFaceNameList()
    openpose = getOpenPoseFaceNameList()
    
    print("The correspondence between OpenPose/IBUG/Multi-Pie facial landmarks (1..68) is ")
    for i in range(0,len(openpose)):
         for z in range(0,len(holistic)):
              if (openpose[i]==holistic[z]):
                print("%u, "%z,end="")




