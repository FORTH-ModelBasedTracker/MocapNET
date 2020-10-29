#!/usr/bin/env python3
#Written by Ammar Qammaz a.k.a AmmarkoV - 2020

import numpy as np

import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as animation  

from mpl_toolkits.mplot3d import axes3d, Axes3D  
print("Using matplotlib:",matplotlib.__version__)


def pointListReturnXYZListForScatterPlot(A):
    numberOfPoints=A.shape[0] 
    xs=list()
    ys=list()
    zs=list()
    for i in range(0,numberOfPoints):
       xs.append(A[i][0])
       ys.append(A[i][1])
       zs.append(A[i][2])
    return xs,ys,zs


def setupDrawing(fig,ax,ax2,ax3,ax4):
   # === Plot and animate === 
   fig = plt.figure()
   fig.set_size_inches(19.2, 10.8, forward=True) 
 
   ax = fig.add_subplot(2, 2, 1, projection='3d') 
   ax2 = fig.add_subplot(2, 2, 2) 
   ax3 = fig.add_subplot(2, 2, 3) 
   ax4 = fig.add_subplot(2, 2, 4) 
   fig.subplots_adjust(left=0.05, bottom=0.05, right=0.95, top=0.95) 
 
   ax.set_xlabel('X Axis')
   ax.set_ylabel('Y Axis')
   ax.set_zlabel('Z Axis') 
   ax.view_init(90, 90) 


def get3DDistance(jX,jY,jZ,pX,pY,pZ):
 return np.sqrt( ((jX-pX)*(jX-pX)) + ((jY-pY)*(jY-pY)) + ((jZ-pZ)*(jZ-pZ)) )

def findJointID(jointName,labels):
  for i in range(0,len(labels)):
      if (jointName==labels[i]):
           return i
  print("Cannot find joint ",jointName," !")
  return -1


#-----------------------------------------------------------------------------------------------------------------------
def drawLimbDimensions(JOINTS_TO_COMPARE,JOINT_LABELS,JOINT_PARENTS,JOINT_PARENT_LABELS,h36mPoints,ourPoints,numberOfJoints,ax2):
 labels = list()
 h36m_distances = list()
 mnet_distances = list()
 
 for jointID in range(0,len(JOINTS_TO_COMPARE)):
       #--------------------------------------- 
       jointIDGlobal = findJointID(JOINTS_TO_COMPARE[jointID],JOINT_LABELS)
       jointParentID=findJointID(JOINT_PARENT_LABELS[JOINT_PARENTS[jointIDGlobal]],JOINTS_TO_COMPARE)
       #--------------------------------------- 
       if (jointParentID!=-1) and (jointIDGlobal!=-1):
          labels.append(JOINT_LABELS[jointIDGlobal])
          jX=h36mPoints[jointID][0]
          jY=h36mPoints[jointID][1]
          jZ=h36mPoints[jointID][2]
          pX=h36mPoints[jointParentID][0]
          pY=h36mPoints[jointParentID][1]
          pZ=h36mPoints[jointParentID][2]
          distances=get3DDistance(jX,jY,jZ,pX,pY,pZ) 
          h36m_distances.append(distances)
          #--------------------------------------- 
          jX=ourPoints[jointID][0]
          jY=ourPoints[jointID][1]
          jZ=ourPoints[jointID][2]
          pX=ourPoints[jointParentID][0]
          pY=ourPoints[jointParentID][1]
          pZ=ourPoints[jointParentID][2]
          distances=get3DDistance(jX,jY,jZ,pX,pY,pZ) 
          mnet_distances.append(distances)
       #--------------------------------------- 

 #------------------------------------------------- 
 x = np.arange(len(labels))  # the label locations
 width = 0.35  # the width of the bars

 rects1 = ax2.bar(x - width/2, h36m_distances, width, label='H36M')
 rects2 = ax2.bar(x + width/2, mnet_distances, width, label='Our Method')

  # Add some text for labels, title and custom x-axis tick labels, etc.
 ax2.set_ylim(auto=False,bottom=0,top=800)
 ax2.set_ylabel('Limb dimensions in millimeters')
 ax2.set_title('Comparison of H36M limb dimensions')
 ax2.set_xticks(x)
 ax2.set_xticklabels(labels, rotation=45, rotation_mode="anchor")
 ax2.legend()

 #autolabel(rects1)
 #autolabel(rects2)
#-----------------------------------------------------------------------------------------------------------------------




#-----------------------------------------------------------------------------------------------------------------------
def drawLimbError(JOINTS_TO_COMPARE,JOINT_LABELS,JOINT_PARENTS,JOINT_PARENT_LABELS,h36mPoints,ourPoints,numberOfJoints,ax3):
 labels = list()
 error_distances = list()  
 for jointID in range(0,len(JOINTS_TO_COMPARE)): 
       #---------------------------------------  
       jointIDGlobal = findJointID(JOINTS_TO_COMPARE[jointID],JOINT_LABELS)
       #--------------------------------------- 
       if (jointIDGlobal!=-1):
          labels.append(JOINT_LABELS[jointIDGlobal])
          jX=h36mPoints[jointID][0]
          jY=h36mPoints[jointID][1]
          jZ=h36mPoints[jointID][2]
          pX=ourPoints[jointID][0]
          pY=ourPoints[jointID][1]
          pZ=ourPoints[jointID][2]
          distances=get3DDistance(jX,jY,jZ,pX,pY,pZ) 
          error_distances.append(distances)
       #---------------------------------------  

 #------------------------------------------------- 
 x = np.arange(len(labels))  # the label locations
 width = 0.35  # the width of the bars

 rects1 = ax3.bar(x - width/2, error_distances, width, label='Error in millimeters') 

  # Add some text for labels, title and custom x-axis tick labels, etc.
 ax3.set_ylim(auto=False,bottom=0,top=250)
 ax3.set_ylabel('Error in millimeters')
 ax3.set_title('Comparison of 3D estimation error')
 ax3.set_xticks(x)
 ax3.set_xticklabels(labels, rotation=45, rotation_mode="anchor")
 ax3.legend()
#-----------------------------------------------------------------------------------------------------------------------



#-----------------------------------------------------------------------------------------------------------------------
def drawFrameError(averageErrorDistances,averageMotionEstimationDistancesBetweenFrames,ax4):
 
 #minimum=np.min(averageErrorDistances)
 #ax4.plot((0,len(averageErrorDistances)), (minimum,minimum),label='Minimum Error')

 #maximum=np.max(averageErrorDistances)
 #ax4.plot((0,len(averageErrorDistances)), (maximum,maximum),label='Maximum Error')

 #median=np.median(averageErrorDistances)
 #ax4.plot((0,len(averageErrorDistances)), (median,median),label='Median Error (%0.2f mm)'% median)

 average=np.average(averageErrorDistances) 
 ax4.plot((0,len(averageErrorDistances)), (average,average),label='Average of average errors (%0.2f mm)' % average)

 ax4.plot(averageErrorDistances, label='Our method average error in millimeters')
 ax4.plot(averageMotionEstimationDistancesBetweenFrames, label='Distance of average joint from previous frame')

 #ax4.set_ylim(auto=False,bottom=0,top=250)
 ax4.set_xlabel('Experiment frame number')
 ax4.set_ylabel('Millimeters')
 ax4.set_title('Average 3D estimation error per frame') 
 #ax4.set_xticklabels(labels, rotation=45, rotation_mode="anchor")
 ax4.legend()
#-----------------------------------------------------------------------------------------------------------------------






def drawAfterEndOfComparison(
                             JOINTS_TO_COMPARE,
                             JOINT_LABELS,
                             JOINT_PARENTS,
                             JOINT_PARENT_LABELS,
                             plt,ax,ax2,ax3,ax4,
                             xlineStart,xlineEnd,
                             ylineStart,ylineEnd,
                             zlineStart,zlineEnd,
                             ourPointCloud,
                             h36PointCloud,
                             frameID,
                             numberOfFrames,
                             everyNFrames,
                             disparity,
                             averageErrorDistances,
                             averageMotionEstimationDistancesBetweenFrames
                            ):
      plt.cla() 
      ax.cla()
      ax2.cla()
      ax3.cla()
      ax4.cla()

      #Print Skeletons and their connected lines.. 
      #for i in range(0,len(xlineStart)): 
      #    ax.plot([xlineStart[i],xlineEnd[i]],[ylineStart[i],ylineEnd[i]],zs=[zlineStart[i],zlineEnd[i]])
      #--------------------------------------------
      xs, ys, zs = pointListReturnXYZListForScatterPlot(ourPointCloud)
      ax.scatter(xs, ys, zs)
      xs, ys, zs = pointListReturnXYZListForScatterPlot(h36PointCloud)
      ax.scatter(xs, ys, zs) 

      #Secondary plots for limb lengths etc.. 
      numberOfJoints = len(ourPointCloud) 
      drawLimbDimensions(JOINTS_TO_COMPARE,JOINT_LABELS,JOINT_PARENTS,JOINT_PARENT_LABELS,h36PointCloud,ourPointCloud,numberOfJoints,ax2) 
      drawLimbError(JOINTS_TO_COMPARE,JOINT_LABELS,JOINT_PARENTS,JOINT_PARENT_LABELS,h36PointCloud,ourPointCloud,numberOfJoints,ax3)
      drawFrameError(averageErrorDistances,averageMotionEstimationDistancesBetweenFrames,ax4)
 
      #------------------------- 
      ax.text2D(0.05, 0.95, "Frame %u/%u - Increment %u - Procrustes Average Error %0.2f mm"%(frameID,numberOfFrames,everyNFrames,disparity) , transform=ax.transAxes)
      #ax.text2D(0.05, 0.05, "RHand %0.2f mm / LHand %0.2f mm / RFoot %0.2f mm / LFoot %0.2f mm "%(currentDistances[4],currentDistances[7],currentDistances[11],currentDistances[14]) , transform=ax.transAxes)
      #------------------------- 
      ax.set_xlim(auto=False,left=-600,right=300)
      ax.set_ylim(auto=False,bottom=-1600,top=200)
      ax.set_zlim(auto=False,bottom=2000,top=6000)
      ax.set_xlabel('X Axis')
      ax.set_ylabel('Y Axis')
      ax.set_zlabel('Z Axis') 
      #------------------------- 
      plt.show(block=False) 
      plt.savefig('p%05u.png'%frameID)
      #fig.canvas.draw() 
      plt.pause(0.001)




