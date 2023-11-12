#!/usr/bin/python3
""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

def getColor(i):
  if i>107:
       i = i % 108
  #---------------------
  if (i==0): 
    return (247,252,253)
  elif (i==1): 
    return (224,236,244)
  elif (i==2): 
    return (191,211,230)
  elif (i==3): 
    return (158,188,218)
  elif (i==4): 
    return (140,150,198)
  elif (i==5): 
    return (140,107,177)
  elif (i==6): 
    return (136,65,157)
  elif (i==7): 
    return (129,15,124)
  elif (i==8): 
    return (77,0,75)
  elif (i==9): 
    return (247,252,253)
  elif (i==10): 
    return (229,245,249)
  elif (i==11): 
    return (204,236,230)
  elif (i==12): 
    return (153,216,201)
  elif (i==13): 
    return (102,194,164)
  elif (i==14): 
    return (65,174,118)
  elif (i==15): 
    return (35,139,69)
  elif (i==16): 
    return (0,109,44)
  elif (i==17): 
    return (0,68,27)
  elif (i==18): 
    return (247,252,240)
  elif (i==19): 
    return (224,243,219)
  elif (i==20): 
    return (204,235,197)
  elif (i==21): 
    return (168,221,181)
  elif (i==22): 
    return (123,204,196)
  elif (i==23): 
    return (78,179,211)
  elif (i==24): 
    return (43,140,190)
  elif (i==25): 
    return (8,104,172)
  elif (i==26): 
    return (8,64,129)
  elif (i==27): 
    return (255,247,236)
  elif (i==28): 
    return (254,232,200)
  elif (i==29): 
    return (253,212,158)
  elif (i==30): 
    return (253,187,132)
  elif (i==31): 
    return (252,141,89)
  elif (i==32): 
    return (239,101,72)
  elif (i==33): 
    return (215,48,31)
  elif (i==34): 
    return (179,0,0)
  elif (i==35): 
    return (127,0,0)
  elif (i==36): 
    return (255,247,251)
  elif (i==37): 
    return (236,231,242)
  elif (i==38): 
    return (208,209,230)
  elif (i==39): 
    return (166,189,219)
  elif (i==40): 
    return (116,169,207)
  elif (i==41): 
    return (54,144,192)
  elif (i==42): 
    return (5,112,176)
  elif (i==43): 
    return (4,90,141)
  elif (i==44): 
    return (2,56,88)
  elif (i==45): 
    return (255,247,251)
  elif (i==46): 
    return (236,226,240)
  elif (i==47): 
    return (208,209,230)
  elif (i==48): 
    return (166,189,219)
  elif (i==49): 
    return (103,169,207)
  elif (i==50): 
    return (54,144,192)
  elif (i==51): 
    return (2,129,138)
  elif (i==52): 
    return (1,108,89)
  elif (i==53): 
    return (1,70,54)
  elif (i==54): 
    return (247,244,249)
  elif (i==55): 
    return (231,225,239)
  elif (i==56): 
    return (212,185,218)
  elif (i==57): 
    return (201,148,199)
  elif (i==58): 
    return (223,101,176)
  elif (i==59): 
    return (231,41,138)
  elif (i==60): 
    return (206,18,86)
  elif (i==61): 
    return (152,0,67)
  elif (i==62): 
    return (103,0,31)
  elif (i==63): 
    return (255,247,243)
  elif (i==64): 
    return (253,224,221)
  elif (i==65): 
    return (252,197,192)
  elif (i==66): 
    return (250,159,181)
  elif (i==67): 
    return (247,104,161)
  elif (i==68): 
    return (221,52,151)
  elif (i==69): 
    return (174,1,126)
  elif (i==70): 
    return (122,1,119)
  elif (i==71): 
    return (73,0,106)
  elif (i==72): 
    return (255,255,229)
  elif (i==73): 
    return (247,252,185)
  elif (i==74): 
    return (217,240,163)
  elif (i==75): 
    return (173,221,142)
  elif (i==76): 
    return (120,198,121)
  elif (i==77): 
    return (65,171,93)
  elif (i==78): 
    return (35,132,67)
  elif (i==79): 
    return (0,104,55)
  elif (i==80): 
    return (0,69,41)
  elif (i==81): 
    return (255,255,217)
  elif (i==82): 
    return (237,248,177)
  elif (i==83): 
    return (199,233,180)
  elif (i==84): 
    return (127,205,187)
  elif (i==85): 
    return (65,182,196)
  elif (i==86): 
    return (29,145,192)
  elif (i==87): 
    return (34,94,168)
  elif (i==88): 
    return (37,52,148)
  elif (i==89): 
    return (8,29,88)
  elif (i==90): 
    return (255,255,229)
  elif (i==91): 
    return (255,247,188)
  elif (i==92): 
    return (254,227,145)
  elif (i==93): 
    return (254,196,79)
  elif (i==94): 
    return (254,153,41)
  elif (i==95): 
    return (236,112,20)
  elif (i==96): 
    return (204,76,2)
  elif (i==97): 
    return (153,52,4)
  elif (i==98): 
    return (102,37,6)
  elif (i==99): 
    return (255,255,204)
  elif (i==100): 
    return (255,237,160)
  elif (i==101): 
    return (254,217,118)
  elif (i==102): 
    return (254,178,76)
  elif (i==103): 
    return (253,141,60)
  elif (i==104): 
    return (252,78,42)
  elif (i==105): 
    return (227,26,28)
  elif (i==106): 
    return (189,0,38)
  elif (i==107): 
    return (128,0,38)

  return (255,255,255)

def drawMissingInput(image):
    import cv2
    width  = image.shape[1]
    height = image.shape[0]
    color = (0,0,255)
    cv2.line(image, pt1=(0,0), pt2=(width,height), color=color, thickness=12)
    cv2.line(image, pt1=(0,0+height), pt2=(width,0), color=color, thickness=12)
    font = cv2.FONT_HERSHEY_SIMPLEX 
    org = (int(width/2)-300,int(height/2)) 
    fontScale = 2 
    color = (0,0,0)
    thickness = 2
    message =  'Incomplete Input' 
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    org = (int(width/2)+2-300,int(height/2)+2) 
    color = (255,255,255)
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    return image



def drawPoseNETLandmarks(predictions,image,threshold=0.25,jointLabels=dict()):
        import cv2
        sourceWidth  = image.shape[1]
        sourceHeight = image.shape[0]
        width  = image.shape[1]
        height = image.shape[0]
        #jointLabels = getPoseNETBodyNameList() # getBody25NameList()
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



def resolveXY(input2D,joint,width,height,flipX=False): 
   x2D=0
   y2D=0
   jointName2DX = "2dx_"+joint
   jointName2DY = "2dy_"+joint 
   if ( jointName2DX in input2D ) and ( jointName2DY in input2D ):
           if (flipX):
              x2D = int((1.0-input2D[jointName2DX])*width)
           else:
              x2D = int(input2D[jointName2DX]*width)
           y2D = int(input2D[jointName2DY]*height)
   else:
           print("Cannot resolve ",joint)
   #print(joint," resolved to ",x2D,",",y2D)
   return x2D,y2D



def drawMocapNETInput(input2D,image,flipX=False,doLines=True):
    import cv2
    if (type(image)==type(None)):
       print("Invalid Image given, can't do anything with it")
       return image
    width  = image.shape[1]
    height = image.shape[0]
    #print("Drawing output to ",width,"x",height," cvmat")

    if (doLines):
      #Draw lines
      #====================================================================
      t=8
      x1,y1=resolveXY(input2D,"rshoulder",width,height,flipX=flipX)
      x2,y2=resolveXY(input2D,"relbow",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,255,0), thickness=t)
      x1,y1=resolveXY(input2D,"rhand",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,255,0), thickness=t)
      x1,y1=resolveXY(input2D,"rshoulder",width,height,flipX=flipX)
      x2,y2=resolveXY(input2D,"neck",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,255,0), thickness=t)
      x2,y2=resolveXY(input2D,"hip",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,255,0), thickness=t)
      x1,y1=resolveXY(input2D,"rhip",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,255,0), thickness=t)
      x2,y2=resolveXY(input2D,"rknee",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,255,0), thickness=t)
      x1,y1=resolveXY(input2D,"rfoot",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,255,0), thickness=t)

      x1,y1=resolveXY(input2D,"lshoulder",width,height,flipX=flipX)
      x2,y2=resolveXY(input2D,"lelbow",width,height,flipX=flipX) 
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,0,255), thickness=t)
      x1,y1=resolveXY(input2D,"lhand",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,0,255), thickness=t)
      x1,y1=resolveXY(input2D,"lshoulder",width,height,flipX=flipX)
      x2,y2=resolveXY(input2D,"neck",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,0,255), thickness=t)
      x2,y2=resolveXY(input2D,"hip",width,height,flipX=flipX) 
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,0,255), thickness=t)
      x1,y1=resolveXY(input2D,"lhip",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,0,255), thickness=t)
      x2,y2=resolveXY(input2D,"lknee",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,0,255), thickness=t)
      x1,y1=resolveXY(input2D,"lfoot",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,0,255), thickness=t)

      x1,y1=resolveXY(input2D,"neck",width,height,flipX=flipX)
      x2,y2=resolveXY(input2D,"head",width,height,flipX=flipX)
      cv2.line(image, pt1=(x1,y1), pt2=(x2,y2), color=(0,255,255), thickness=t)
      #====================================================================


    font = cv2.FONT_HERSHEY_SIMPLEX 
    fontScale = 0.4 
    thickness = 1 
    color = (0,0,0)

    for jointRaw in input2D:
        #print("Joint ",jointRaw)
        jointSplit    = jointRaw.lower().split("_",1)
        if (len(jointSplit)>1):
          joint         = jointSplit[1].lower()
          jointName2DX = "2dx_"+joint
          jointName2DY = "2dy_"+joint 
          if ( jointName2DX in input2D ) and ( jointName2DY in input2D ):
           if (flipX):
              x2D = int((1.0-input2D[jointName2DX])*width)
           else:
              x2D = int(input2D[jointName2DX]*width)
           y2D = int(input2D[jointName2DY]*height) 
           #print("IS Joint ",joint,x2D,y2D)
           color=(0,255,255)
           circleSize = 2
           if (len(joint)>0):
               #We have a joint Name
             if not 'head' in joint:
                circleSize = 4 #body joints are bigger

             
             if (len(joint)>1):
                if (joint[len(joint)-2]=='.') and (joint[len(joint)-1]=='r'): #Right Joint
                       color=(0,255,0)  #GREEN COLOR
                if (joint[len(joint)-2]=='.') and (joint[len(joint)-1]=='l'): #Left Joint
                       color=(0,0,255)  #RED COLOR

             if (joint[0]=='r'): #Right Joint
               color=(0,255,0)  #GREEN COLOR
             elif (joint[0]=='l'): #Left Joint
               color=(0,0,255)  #RED COLOR
             elif ("head_l" in joint): 
               color=(0,0,255)  #RED COLOR
               #image = cv2.putText(image, "%s" % (joint.replace("head_","")) , (x2D+2,y2D), font, fontScale, color, thickness, cv2.LINE_AA)
             elif ("head_r" in joint): 
               color=(0,255,0)  #GREEN COLOR
               #image = cv2.putText(image, "%s" % (joint.replace("head_","")) , (x2D+2,y2D), font, fontScale, color, thickness, cv2.LINE_AA)
             #else:
             #  image = cv2.putText(image, "%s" % (joint) , (x2D+2,y2D), font, fontScale+0.7, color, thickness, cv2.LINE_AA)
           cv2.circle(image,(x2D,y2D),circleSize,color,cv2.FILLED)
           #if ("lshoulder"==joint) or ("lelbow"==joint) or ("lhand"==joint):
           #   image = cv2.putText(image, "%s" % (joint) , (x2D+2,y2D), font, fontScale, color, thickness, cv2.LINE_AA)
           if ("head_reye"==joint) or ("head_leye"==joint) or ("reye"==joint) or ("leye"==joint):
              color=(255,0,0)
              circleSize = 4
              image = cv2.putText(image, "%s" % (joint) , (x2D+2,y2D), font, fontScale, color, thickness, cv2.LINE_AA)
           cv2.circle(image,(x2D,y2D),circleSize,color,cv2.FILLED)

           #Post Visualization score
           #jointNameVis = "visible_"+joint 
           #image = cv2.putText(image, "%0.2f" % (input2D[jointNameVis]) , (x2D+2,y2D), font, fontScale, color, thickness, cv2.LINE_AA)

           #if ('__' in joint): #Print __temporalis joint
           #   image = cv2.putText(image, joint , (x2D+2,y2D), font, fontScale, color, thickness, cv2.LINE_AA)
    return image

def drawMocapNETOutput(mnet,image,xOffset=0): #set xOffset to -400 to make visualization more clean by seperating 2D/3D
    import cv2
    if (type(image)==type(None)):
       print("Invalid Image given, can't do anything with it")
       return image
    width  = image.shape[1]
    height = image.shape[0]
    #print("Drawing output to ",width,"x",height," cvmat")
 
    jointID = 0
    for joint in mnet.bvhJointList:
    #----------------------------------------------------------------------------------------------
        jointParentID = mnet.bvhJointParentList[jointID]
        jointParent   = mnet.bvhJointList[jointParentID]
        #print("Joint ",joint)
        #print("Joint Parent",jointParent)

        #Enforce Joint LowerCase
        joint         = joint.lower()
        jointParent   = mnet.bvhJointList[jointParentID].lower()
    
        doThisDraw = 1
        #----------------------------------------------------------------------------------------------
        jointName2DX = "2DX_"+joint
        jointName2DY = "2DY_"+joint 
        if ( not jointName2DX in mnet.output2D ) or ( not jointName2DY in mnet.output2D ): 
           doThisDraw = 0
        elif (mnet.output2D[jointName2DX]==0.0) and (mnet.output2D[jointName2DY]==0.0):  
           doThisDraw = 0
        else:
           xA2D = xOffset + int((1.0-mnet.output2D[jointName2DX])*width)
           yA2D = int(mnet.output2D[jointName2DY]*height) 
        #----------------------------------------------------------------------------------------------
        jointParentName2DX = "2DX_"+jointParent
        jointParentName2DY = "2DY_"+jointParent 
        if ( not jointParentName2DX in mnet.output2D ) or ( not jointParentName2DY in mnet.output2D ):
           doThisDraw = 0
        elif (mnet.output2D[jointParentName2DX]==0.0) and (mnet.output2D[jointParentName2DY]==0.0):  
           doThisDraw = 0
        else: 
           xB2D = xOffset + int((1.0-mnet.output2D[jointParentName2DX])*width)
           yB2D = int(mnet.output2D[jointParentName2DY]*height)
        #----------------------------------------------------------------------------------------------
        if (doThisDraw):
           color=(255,0,0)     #BLUE COLOR
           if (joint[0]=='l'):
               color=(0,0,255) #RED COLOR
           if (joint[0]=='r'):
               color=(0,255,0) #GREEN COLOR
           cv2.line(image, pt1=(xA2D,yA2D), pt2=(xB2D,yB2D), color=color, thickness=12)
        #----------------------------------------------------------------------------------------------
        jointID = jointID + 1
    #----------------------------------------------------------------------------------------------

    for joint in mnet.bvhJointList:
        #print("Joint ",joint)
        joint         = joint.lower()
        jointName2DX = "2DX_"+joint
        jointName2DY = "2DY_"+joint 
        if ( jointName2DX in mnet.output2D ) and ( jointName2DY in mnet.output2D ):
         if (mnet.output2D[jointName2DX]!=0.0) or (mnet.output2D[jointName2DY]!=0.0):  
           x2D = xOffset + int((1.0-mnet.output2D["2DX_"+joint])*width)
           y2D = int(mnet.output2D["2DY_"+joint]*height) 
           color=(0,255,255)
           cv2.circle(image,(x2D,y2D),2,color)
    #----------------------------------------------------------------------------------------------
    return image




def drawDescriptor(name,elements,image,x,y,w,h):
    #------------------------------------
    if (elements.shape[1]==0):
       return image
    #------------------------------------
    import cv2
    block = int(w / elements.shape[1])    
    #------------------------------------
    if (block==0):    
       return image
    #------------------------------------
    #print("WIDTH ",w," BLOCK",block," ELEMENTS ",elements.shape[1])
    eI = 0
    for xI in range(x,x+w-block,block):
           xA2D=xI
           yA2D=y
           xB2D=xI+block
           yB2D=y+h
           #----------------------------------------------
           val = elements[0][eI]
           #----------------------------------------------
           greenValue = 0.0
           blueValue  = 0.0
           #----------------------------------------------
           if (val<0.0):
               blueValue=abs(val)
           else:
               greenValue=val
           #----------------------------------------------
           color=(
                    min(255,int(255.0 * blueValue)),
                    min(255,int(255.0 * greenValue)),
                    min(255,int(25.5  * greenValue)) 
                 )
           #----------------------------------------------
           if (xA2D!=0.0) and (yA2D!=0.0) and (xB2D!=0.0) and (yB2D!=0.0):
                cv2.line(image, pt1=(xA2D,yA2D), pt2=(xB2D,yB2D), color=color, thickness=12)
           eI +=1
           eI = min(eI,elements.shape[1]-1)
    #----------------------------------------------
    font = cv2.FONT_HERSHEY_SIMPLEX 
    fontScale = 0.5 
    thickness = 1
    org = (x+10,y+int(h/2)+5) 
    color = (0,0,0)
    image = cv2.putText(image, name , org, font, fontScale, color, thickness, cv2.LINE_AA)
    org = (x+8,y+int(h/2)+3) 
    color = (255,255,255)
    image = cv2.putText(image, name , org, font, fontScale, color, thickness, cv2.LINE_AA)

    return image



def printNSDM(nsdm):
    import math
    from tools    import bcolors
    NSRMDimension = int(math.sqrt(len(nsdm))) 
    eI = 0
    for yI in range(0,NSRMDimension):
       for xI in range(0,NSRMDimension): 
           #-------------------------
           val        = nsdm[eI]
           eI +=1
           if (val==0.0):
               print(bcolors.FAIL,end=" ")
           elif (val<0.0):
               print(bcolors.OKBLUE,end="")
           else:
               print(bcolors.OKGREEN,end=" ")
           print("%0.2f " % val,end="")
           print(bcolors.ENDC,end="")
       print(" ")


def drawNSRM(name,elements,image,x,y,w,h):
    import cv2
    import math
    #print("Draw NSRM with ",len(elements)," elements ")
    NSRMDimension = int(math.sqrt(len(elements))) 
    blockX = int(w/NSRMDimension)
    blockY = int(h/NSRMDimension)

    #print("WIDTH ",w," BLOCK",block," ELEMENTS ",elements.shape[1])
    if (NSRMDimension<4):
         print("drawNSRM not drawing matrix with len(elements) = ",len(elements))
         return image

    eI = 0
    for yI in range(0,NSRMDimension):
       for xI in range(0,NSRMDimension):
           xA2D=x + xI*blockX
           yA2D=y + yI*blockY
           xB2D=xA2D+blockX
           yB2D=yA2D+blockY
           #-------------------------
           val        = elements[eI]
           eI +=1
           #-------------------------
           redValue   = 0
           greenValue = 0
           blueValue  = 0
           #-------------------------
           if (val==0.0):
               redValue   = 1
           elif (val<0.0):
               blueValue  = abs(val)#/2
           else:
               greenValue = val#/2
           #-------------------------          
           color=(
                   int(255.0 * blueValue),           #B
                   int(255.0 * greenValue),          #G 
                   int(255.0 * redValue)             #R
                 )
           #-------------------------
           cv2.rectangle(image, pt1=(xA2D,yA2D), pt2=(xB2D,yB2D), color=color, thickness=-1)
    #-----------------------------------
    font = cv2.FONT_HERSHEY_SIMPLEX 
    fontScale = 0.5 
    thickness = 1
    org = (x,y-10) 
    color = (0,0,0)
    image = cv2.putText(image, name , org, font, fontScale, color, thickness, cv2.LINE_AA)
    org = (x-2,y-8) 
    color = (255,255,255)
    image = cv2.putText(image, name , org, font, fontScale, color, thickness, cv2.LINE_AA)

    return image




def drawMAE2DError(name,mae,image,x,y,w,h):
    if (mae<=0.0):
        return
    import cv2
    #-----------------------------------------
    color = (123,123,123)
    if (mae<127):
       color = (0,255-(mae*2),0) #B G R
    elif (mae<255): 
       color = (0,mae,mae) #B G R
    else: 
       color = (0,0,min(255,mae-255)) #B G R
    #-----------------------------------------
    xA2D=x
    yA2D=y
    xB2D=x+w
    yB2D=y+h 
    cv2.rectangle(image, pt1=(xA2D,yA2D), pt2=(xB2D,yB2D), color=color, thickness=-1)
    #-----------------------------------------
    font = cv2.FONT_HERSHEY_SIMPLEX 
    fontScale = 0.4
    thickness = 1
    #-----------------------------------------
    yOffset=15
    message =  '%s ' % (name) 
    org = (x+2,y+2+yOffset) 
    color = (0,0,0)
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    org = (x,y+yOffset) 
    color = (255,255,255)
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    #-----------------------------------------
    message =  '%0.2f' % (mae) 
    color = (0,0,0)
    org = (x,y+20+yOffset) 
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    color = (255,255,255)
    org = (x+2,y+22+yOffset) 
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    #-----------------------------------------


def calculateRelativeValue(y,h,value,minimum,maximum):
    if (maximum==minimum):
       return int(y + (h/2)) 
    #-------------------------------------------------
    #TODO IMPROVE THIS!
    vRange = (maximum - minimum)
    return int( y + (h/2) - ( value / vRange ) * (h/2) )


def drawMocapNETSinglePlot(history,plotNumber,itemName,image,x,y,w,h,minimumValue,maximumValue):
    import cv2
    color=getColor(plotNumber)
    if (minimumValue==maximumValue):
      color = (40,40,40)

    cv2.line(image, pt1=(x,y+h), pt2=(x+w,y+h), color=color, thickness=1)
    cv2.line(image, pt1=(x,y),   pt2=(x,y+h),   color=color, thickness=1)

    font = cv2.FONT_HERSHEY_SIMPLEX 
    org = (x,y) 
    fontScale = 0.3 
    tColor = (123,123,123)
    thickness = 1
    message =  '%s #%u  ' % (itemName,plotNumber) 
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    message =  'Max %0.2f ' % (maximumValue) 
    org = (x,y+10) 
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    message =  'Min %0.2f ' % (minimumValue) 
    org = (x,y+h+10) 
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    

    for frameID in range(1,len(history)):
            #Old code
            #previousValue =  int(y+history[frameID-1][itemName] + h/2)
            #nextValue     =  int(y+history[frameID][itemName]   + h/2)
            #-------------------------------------------------------------------------------------
            previousValue = calculateRelativeValue(y,h,history[frameID-1][itemName],minimumValue,maximumValue)
            nextValue     = calculateRelativeValue(y,h,history[frameID][itemName],minimumValue,maximumValue)
            #-------------------------------------------------------------------------------------
            jointPointPrev = (int(x+ frameID-1),      previousValue )
            jointPointNext = (int(x+ frameID),        nextValue )
            #cv::Scalar usedColor = getColorFromIndex(joint);
            if (itemName=="hip_yrotation"):  
                color=(0,0,255) 
            
            cv2.line(image, pt1=jointPointPrev, pt2=jointPointNext, color=color, thickness=1)  
     
    #old code
    #org = (int(x+len(history)),int(y+history[len(history)-1][itemName] + h/2)) 
    org = (int(x+len(history)), calculateRelativeValue(y,h,history[len(history)-1][itemName],minimumValue,maximumValue) ) 
    message =  '%0.2f' % (history[len(history)-1][itemName]) 
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
#---------------------------------------------------------------------------------------------


def drawMocapNETSinglePlotValueList(valueList,plotNumber,itemName,image,x,y,w,h,minimumValue,maximumValue):
    import cv2
    import numpy as np
    color=getColor(plotNumber)
    if (minimumValue==maximumValue):
      color = (40,40,40) #Dead plot

    listMaxValue = np.max(valueList)
    if (listMaxValue>maximumValue):
          maximumValue=listMaxValue*2 #Adapt to maximum

    #------------------------------------------------------------------
    cv2.line(image, pt1=(x,y+h), pt2=(x+w,y+h), color=color, thickness=1)
    cv2.line(image, pt1=(x,y),   pt2=(x,y+h),   color=color, thickness=1)

    font = cv2.FONT_HERSHEY_SIMPLEX 
    org = (x,y) 
    fontScale = 0.3 
    tColor = (123,123,123)
    thickness = 1
    message =  '%s #%u  ' % (itemName,plotNumber) 
    image = cv2.putText(image, message , (x-1,y-1), font, fontScale, (0,0,0) , thickness, cv2.LINE_AA)
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    message =  'Max %0.2f ' % (maximumValue) 
    org = (x,y+10) 
    image = cv2.putText(image, message , (x-1,y+10-1), font, fontScale, (0,0,0) , thickness, cv2.LINE_AA)
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    message =  'Min %0.2f ' % (minimumValue) 
    org = (x,y+h+10) 
    image = cv2.putText(image, message , (x-1,y+h+10-1), font, fontScale, (0,0,0) , thickness, cv2.LINE_AA)
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
    

    for frameID in range(1,len(valueList)):
            #Old code
            #previousValue =  int(y+history[frameID-1][itemName] + h/2)
            #nextValue     =  int(y+history[frameID][itemName]   + h/2)
            #-------------------------------------------------------------------------------------
            previousValue = calculateRelativeValue(y,h,valueList[frameID-1],minimumValue,maximumValue)
            nextValue     = calculateRelativeValue(y,h,valueList[frameID],minimumValue,maximumValue)
            #-------------------------------------------------------------------------------------
            jointPointPrev = (int(x+ frameID-1),      previousValue )
            jointPointNext = (int(x+ frameID),        nextValue )
            #cv::Scalar usedColor = getColorFromIndex(joint);
            if (itemName=="hip_yrotation"):  
                color=(0,0,255) 
            
            cv2.line(image, pt1=jointPointPrev, pt2=jointPointNext, color=color, thickness=1)  
     
    #old code
    #org = (int(x+len(valueList)),int(y+valueList[len(valueList)-1] + h/2)) 
    org = (int(x+len(valueList)), calculateRelativeValue(y,h,valueList[len(valueList)-1],minimumValue,maximumValue) ) 
    message =  '%0.2f' % (valueList[len(valueList)-1]) 
    image = cv2.putText(image, message , org, font, fontScale, (0,0,0), thickness, cv2.LINE_AA)

    org = (1+int(x+len(valueList)), 1+calculateRelativeValue(y,h,valueList[len(valueList)-1],minimumValue,maximumValue) ) 
    image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)

#---------------------------------------------------------------------------------------------



def drawMocapNETAllPlots(history,width,height,minimumLimits=dict(),maximumLimits=dict()):
 import cv2
 import numpy as np
#-------------------------------------------------
 imageForPlot = np.zeros([height,width,3],dtype=np.uint8)
#-------------------------------------------------
 margin = 25
 x = 0
 y = margin
 widthOfGraphs  = 80
 heightOfGraphs = 80
#-------------------------------------------------
 plotNumber = 0
 if (len(history)>0):
   for itemName in history[0].keys():
       minimumValue=-180.0
       maximumValue= 180.0
       if (itemName in minimumLimits) and (itemName in maximumLimits):
          minimumValue=float(minimumLimits[itemName])
          maximumValue=float(maximumLimits[itemName])

       if (minimumValue!=maximumValue):
          drawMocapNETSinglePlot(history,plotNumber,itemName,imageForPlot,x,y,widthOfGraphs,heightOfGraphs,minimumValue,maximumValue)
          plotNumber=plotNumber+1
          y = y + heightOfGraphs + margin
          if (y + heightOfGraphs > height - heightOfGraphs):
            y = margin
            x = x + widthOfGraphs + margin
 #cv2.imshow('Motion History',imageForPlot) 
 return imageForPlot
#-------------------------------------------------



def drawMocapNETFrequencyPlots(history):
 import numpy as np
 import matplotlib.pyplot as plt
#-------------------------------------------------
 if (len(history)>0):
   for itemName in history[0].keys(): 
      output="freq_%s.png" % itemName

      data=list()
      for frameID in range(1,len(history)):
         data.append(float(history[frameID][itemName])) 
      plt.cla()
      plt.hist(data, bins=250)
      # Add labels and title
      plt.xlabel('Value')
      plt.ylabel('Frequency')
      plt.title('Histogram of %s of %u values'%(itemName,len(history)))
      # Save figure as PNG file
      plt.savefig(output)
      #fig.savefig(output)
#-------------------------------------------------



def drawValueLineInRange(history, minimumValues, maximumValues, label, result_img, x, y, w, h):
 # Calculate the position of the value within the specified range
  
 if (h>20) and (len(history)>0):
  h = h-20
  lastItem = len(history)-1
  if (label in history[lastItem]) and (label in minimumValues) and (label in maximumValues) :
    minimumValue = minimumValues[label]
    maximumValue = maximumValues[label] 
    import cv2
    #print("Items to select ",history[lastItem].keys())
    #print("Items to select ",len(history[lastItem]))
    value = history[lastItem][label]
    #print("DRAW ",value," between ",minimumValue," and ",maximumValue)
    normalized_value = (value - minimumValue) / (maximumValue - minimumValue)
    x_pos = int(x + normalized_value * w) 

    # Draw a horizontal line to represent the value
    #print("cv2.line")
    color = (255,255,255)
    colorB = (123,123,123)

    cv2.putText(result_img, label, (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1)
    cv2.putText(result_img, "%0.2f"%value, (x, y + 15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1)

    cv2.line(result_img, (x, y+h), (x+w,y+h),  colorB, 2) #Horizontal line
    cv2.line(result_img, (x, y+5), (x,y+h),    colorB, 2) #Vertical line start
    cv2.line(result_img, (x+w,y+5), (x+w,y+h), colorB, 2) #Vertical line end

    #Draw labels l/r
    cv2.putText(result_img, "%0.1f"% minimumValue , (x,y+h+20),   cv2.FONT_HERSHEY_SIMPLEX, 0.3, color, 1)
    cv2.putText(result_img, "%0.1f"% maximumValue , (x+w,y+h+20), cv2.FONT_HERSHEY_SIMPLEX, 0.3, color, 1)

    #Draw Arrow
    cv2.line(result_img, (x_pos, y), (x_pos, y + h), color, 2)
    cv2.line(result_img, (x_pos-10, y+h-10), (x_pos, y + h), color, 2)
    cv2.line(result_img, (x_pos+10, y+h-10), (x_pos, y + h), color, 2)
  else: 
    print("Failed visualizing ",label," in range [",minimumValue,",",maximumValue,"]") 



def drawMNETSerials(mnet,image,x,y):
    import cv2
    #-----------------------------------------
    font = cv2.FONT_HERSHEY_SIMPLEX 
    fontScale = 0.4
    thickness = 1
    #-----------------------------------------
    #print("MNET Serials ",mnet.getEnsembleSerials()) 
    #-----------------------------------------
    color = (0,0,0)
    org = (x+2,y+2) 
    image = cv2.putText(image, mnet.getEnsembleSerials() , org, font, fontScale, color, thickness, cv2.LINE_AA)
    org = (x,y) 
    color = (255,255,255)
    image = cv2.putText(image, mnet.getEnsembleSerials() , org, font, fontScale, color, thickness, cv2.LINE_AA)
    #-----------------------------------------


def registerVisualizationTime(mnet,startTime):
    import time
    end = time.time() # Time elapsed
    from tools import secondsToHz
    mnet.hz_Vis = secondsToHz(end - startTime)
    mnet.history_hz_Vis.append(mnet.hz_Vis)
    if (len(mnet.history_hz_Vis)>mnet.perfHistorySize): 
            mnet.history_hz_Vis.pop(0) #Keep mnet history on limits


def visualizeMocapNETEnsemble(mnet,annotated_image,plotBVHChannels=0,bvhAnglesForPlotting=list(),economic=False,drawOutput=True):
 import time
 start = time.time() # Time elapsed 
 try: 
    #from MocapNETVisualization import drawMocapNETOutput,drawMocapNETAllPlots,drawMissingInput,drawDescriptor,drawNSRM,drawMAE2DError
    #------------------------------------------------------------------------------------
    if (drawOutput):
      if ("upperbody" in mnet.ensemble):
        drawMocapNETOutput(mnet,annotated_image) #only draw 3D ouput if upperbody is loaded and working..
    
    drawMocapNETInput(mnet.input2D,annotated_image,doLines=(drawOutput==False))
    if (economic):
       registerVisualizationTime(mnet,start)
       return annotated_image,annotated_image
    #------------------------------------------------------------------------------------

    width  = annotated_image.shape[1]
    height = annotated_image.shape[0]

    locY = 10
    if ("upperbody" in mnet.ensemble) and (mnet.ensemble["upperbody"].configuration["decompositionType"]!=""):
       dcmp = mnet.ensemble["upperbody"].configuration["decompositionType"]
       drawDescriptor("%s upperbody" % dcmp,mnet.ensemble["upperbody"].inputReadyForTF,annotated_image,10,locY,annotated_image.shape[1]-20,5); locY+=15
    if ("lowerbody" in mnet.ensemble) and (mnet.ensemble["lowerbody"].configuration["decompositionType"]!=""):
       dcmp = mnet.ensemble["lowerbody"].configuration["decompositionType"]
       drawDescriptor("%s lowerbody"% dcmp,mnet.ensemble["lowerbody"].inputReadyForTF,annotated_image,10,locY,annotated_image.shape[1]-20,5); locY+=15
    if ("face" in mnet.ensemble) and (mnet.ensemble["face"].configuration["decompositionType"]!=""):
       dcmp = mnet.ensemble["face"].configuration["decompositionType"]
       drawDescriptor("%s face"% dcmp,mnet.ensemble["face"].inputReadyForTF,annotated_image,10,locY,annotated_image.shape[1]-20,5); locY+=15
    if ("reye" in mnet.ensemble) and (mnet.ensemble["reye"].configuration["decompositionType"]!=""):
       dcmp = mnet.ensemble["reye"].configuration["decompositionType"]
       drawDescriptor("%s reye"% dcmp,mnet.ensemble["reye"].inputReadyForTF,annotated_image,10,locY,annotated_image.shape[1]-20,5); locY+=15
       dcmp = mnet.ensemble["leye"].configuration["decompositionType"]
       drawDescriptor("%s leye"% dcmp,mnet.ensemble["leye"].inputReadyForTF,annotated_image,10,locY,annotated_image.shape[1]-20,5); locY+=15
    if ("mouth" in mnet.ensemble) and (mnet.ensemble["mouth"].configuration["decompositionType"]!=""):
       dcmp = mnet.ensemble["mouth"].configuration["decompositionType"]
       drawDescriptor("%s mouth"% dcmp,mnet.ensemble["mouth"].inputReadyForTF,annotated_image,10,locY,annotated_image.shape[1]-20,5); locY+=15
    if ("lhand" in mnet.ensemble) and (mnet.ensemble["lhand"].configuration["decompositionType"]!=""):
       dcmp = mnet.ensemble["lhand"].configuration["decompositionType"]
       drawDescriptor("%s lhand"% dcmp,mnet.ensemble["lhand"].inputReadyForTF,annotated_image,10,locY,annotated_image.shape[1]-20,5); locY+=15
       dcmp = mnet.ensemble["rhand"].configuration["decompositionType"]
       drawDescriptor("%s rhand"% dcmp,mnet.ensemble["rhand"].inputReadyForTF,annotated_image,10,locY,annotated_image.shape[1]-20,5); locY+=15
    #--------------------------------------------------------------------------------------------------------------
    NSRM_Y = 120
    NSRM_Body_Y = NSRM_Y
    if ("upperbody" in mnet.ensemble):
       drawNSRM("NSRM Up",mnet.ensemble["upperbody"].NSRM,annotated_image,10,NSRM_Y,100,100); NSRM_Y+=130
    if ("lowerbody" in mnet.ensemble):
       drawNSRM("NSRM Down",mnet.ensemble["lowerbody"].NSRM,annotated_image,120,NSRM_Body_Y,100,100);# NSRM_Y+=130
    if ("face" in mnet.ensemble):
      drawNSRM("NSRM Face",mnet.ensemble["face"].NSRM,annotated_image,10,NSRM_Y,100,100); NSRM_Y+=130
    if ("leye" in mnet.ensemble):
      drawNSRM("NSRM LEye",mnet.ensemble["leye"].NSRM,annotated_image,120,NSRM_Y,100,100); 
    if ("reye" in mnet.ensemble):
      drawNSRM("NSRM REye",mnet.ensemble["reye"].NSRM,annotated_image,10,NSRM_Y,100,100); NSRM_Y+=130
    if ("mouth" in mnet.ensemble):
      drawNSRM("NSRM Mouth",mnet.ensemble["mouth"].NSRM,annotated_image,10,NSRM_Y,100,100); NSRM_Y+=130
    if ("lhand" in mnet.ensemble):
      drawNSRM("NSRM LHand",mnet.ensemble["lhand"].NSRM,annotated_image,10,NSRM_Y,100,100); 
      drawNSRM("NSRM RHand",mnet.ensemble["rhand"].NSRM,annotated_image,120,NSRM_Y,100,100); NSRM_Y+=130


    #drawValueLineInRange(value, minimumValue, maximumValue, label, result_img, x, y, w, h):
    #These cause the "failed visualizing" error to be emitted from drawValueLineInRange in google collab (why though?)
    drawValueLineInRange(bvhAnglesForPlotting,mnet.outputBVHMinima,mnet.outputBVHMaxima,"hip_xposition",annotated_image,10,NSRM_Y,100,50); NSRM_Y+=90
    drawValueLineInRange(bvhAnglesForPlotting,mnet.outputBVHMinima,mnet.outputBVHMaxima,"hip_yposition",annotated_image,10,NSRM_Y,100,50); NSRM_Y+=90
    drawValueLineInRange(bvhAnglesForPlotting,mnet.outputBVHMinima,mnet.outputBVHMaxima,"hip_zposition",annotated_image,10,NSRM_Y,100,50)
    
    #--------------------------------------------------------------------------------------------------------------
    drawMAE2DError("2D M.A.E.",mnet.lastMAEErrorInPixels,annotated_image,width-70,height-120,width-10,height-90)
    #--------------------------------------------------------------------------------------------------------------

    perfWidgetY = 120
    if (len(mnet.history_hz_2DEst)>0):
      drawMocapNETSinglePlotValueList(mnet.history_hz_2DEst,1,"RGB->2D FPS",annotated_image,width-70,perfWidgetY,70,70,0.0,30.0)
      perfWidgetY += 100

    if (len(mnet.history_hz_NN)>0):
      drawMocapNETSinglePlotValueList(mnet.history_hz_NN,1,"NN FPS",annotated_image,width-70,perfWidgetY,70,70,0.0,30.0)
      perfWidgetY += 100

    if (len(mnet.history_hz_HCD)>0):
      drawMocapNETSinglePlotValueList(mnet.history_hz_HCD,1,"HCD FPS",annotated_image,width-70,perfWidgetY,70,70,0.0,30.0)
      perfWidgetY += 100

    if (len(mnet.history_hz_Vis)>0):
      drawMocapNETSinglePlotValueList(mnet.history_hz_Vis,1,"Visualization",annotated_image,width-70,perfWidgetY,70,70,0.0,30.0)
      perfWidgetY += 100

    drawMNETSerials(mnet,annotated_image,10,30)
    


    #if (mnet.incompleteUpperbodyInput and mnet.incompleteLowerbodyInput): 
    #  drawMissingInput(annotated_image)
    if (plotBVHChannels==1):
        plotImage = drawMocapNETAllPlots(bvhAnglesForPlotting,1920,920,minimumLimits=mnet.outputBVHMinima,maximumLimits=mnet.outputBVHMaxima)
        registerVisualizationTime(mnet,start)
        return annotated_image,plotImage
 except Exception as e:
    print("\n\n\n\nFAILED: Exception while visualizing : ",e,"\n\n\n\n")  
 #Fall-through
 registerVisualizationTime(mnet,start)
 return annotated_image,annotated_image




if __name__ == '__main__':
     print("MocapNETVisualization.py is a library and cannot run standalone")
