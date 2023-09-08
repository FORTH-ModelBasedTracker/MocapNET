from scipy.stats import qmc
import numpy as np
import random


"""
python3 sobolRandomDatasetGenerator.py --upperbody --exponent 21
rm dataset/generated/2d_upperbody_all.csv dataset/generated/bvh_upperbody_all.csv
./GroundTruthDumper --from dataset/headerWithHeadAndOneMotion.bvh --importCSVPoses sobolUpperbody_2097152.csv --filterOccludedJoints --csv dataset/generated/ upperbody_all.csv 2d+3d+bvh 

python3 sobolRandomDatasetGenerator.py --lowerbody --exponent 21
rm dataset/generated/2d_lowerbody_all.csv dataset/generated/bvh_lowerbody_all.csv
./GroundTruthDumper --from dataset/headerWithHeadAndOneMotion.bvh --importCSVPoses sobolLowerbody_2097152.csv --csv dataset/generated/ lowerbody_all.csv 2d+3d+bvh 

python3 sobolRandomDatasetGenerator.py --lhand --exponent 21
rm dataset/generated/2d_lhand_all.csv dataset/generated/bvh_lhand_all.csv
./GroundTruthDumper --from dataset/lhand.qbvh --importCSVPoses sobolLHand_2097152.csv --csv dataset/generated/ lhand_all.csv 2d+3d+bvh

scp -P 2234  dataset/generated/2d_upperbody_all.csv dataset/generated/bvh_upperbody_all.csv dataset/generated/2d_lowerbody_all.csv dataset/generated/bvh_lowerbody_all.csv dataset/generated/2d_lhand_all.csv dataset/generated/bvh_lhand_all.csv ammar@cvrldemo:/home/ammar/Documents/Programming/DNNTracker/tensorflow2GPU/src/dataset/generated
"""

#python3 -m csvNET --from SCANREYE.csv --reye --nobody --plot --save 
def scanReye(filename):
  f = open(filename, 'w')
  f.write("2DX_head_reye_0,2DY_head_reye_0,visible_head_reye_0,2DX_head_reye_1,2DY_head_reye_1,visible_head_reye_1,2DX_head_reye_2,2DY_head_reye_2,visible_head_reye_2,2DX_head_reye_3,2DY_head_reye_3,visible_head_reye_3,2DX_head_reye_4,2DY_head_reye_4,visible_head_reye_4,2DX_head_reye_5,2DY_head_reye_5,visible_head_reye_5,2DX_head_nostrills_2,2DY_head_nostrills_2,visible_head_nostrills_2,2DX_head_rchin_0,2DY_head_rchin_0,visible_head_rchin_0,2DX_head_chin,2DY_head_chin,visible_head_chin,2DX_head_reyebrow_0,2DY_head_reyebrow_0,visible_head_reyebrow_0,2DX_head_reyebrow_1,2DY_head_reyebrow_1,visible_head_reyebrow_1,2DX_head_reyebrow_2,2DY_head_reyebrow_2,visible_head_reyebrow_2,2DX_head_reyebrow_3,2DY_head_reyebrow_3,visible_head_reyebrow_3,2DX_head_reyebrow_4,2DY_head_reyebrow_4,visible_head_reyebrow_4,2DX_head_reye,2DY_head_reye,visible_head_reye\n")

  for x  in range(710,740): #710 - 750
     for y  in range(270,280): #range(270,271): #260 - 290
         xF = float(x / 1000) 
         yF = float(y / 1000) 
         f.write("0.706034,0.271945,1,0.713014,0.265075,1,0.724606,0.264025,1,0.734642,0.276731,1,0.723602,0.280737,1,0.71263,0.277377,1,0.755537,0.331723,1,0.688397,0.274243,1,0.749762,0.466904,1,0.690998,0.262801,1,0.703323,0.247993,1,0.713234,0.238449,1,0.731472,0.246738,1,0.742246,0.255172,1,%f,%f,1\n" % (xF,yF) )
  f.close()
  import sys
  sys.exit(0)

def getUpperbodyList():
   #-----------------------------------
   minima    = list()
   maxima    = list()
   joints    = list()
   constants = dict()
   rootDofs  = list()
   minR = -179.90
   maxR =  179.90
   dof       = 0
   #-----------------------------------
   #joints.append("hip_Xposition");       minima.append(-340.0);   maxima.append(340.0);  rootDofs.append(dof); dof+=1  # -529 529 original
   #joints.append("hip_Yposition");       minima.append(-80.0);    maxima.append(80.0);   rootDofs.append(dof); dof+=1  # -207 208 original 
   #joints.append("hip_Zposition");       minima.append(-500.0);   maxima.append(-100.0); rootDofs.append(dof); dof+=1   # -450 -90 original
   #-------------------------------------------------------------------------------
   #joints.append("hip_Zrotation");       minima.append(-45.0);    maxima.append(45.0);   rootDofs.append(dof); dof+=1 
   #joints.append("hip_Yrotation");       minima.append(minR);     maxima.append(maxR);   rootDofs.append(dof); dof+=1 
   #joints.append("hip_Xrotation");       minima.append(-45.0);    maxima.append(45.0);   rootDofs.append(dof); dof+=1 
   #-------------------------------------------------------------------------------
   joints.append("abdomen_Zrotation");   minima.append(-175.82);  maxima.append(maxR);   dof+=1 
   joints.append("abdomen_Xrotation");   minima.append(-87.84);   maxima.append(92.43);  dof+=1 
   joints.append("abdomen_Yrotation");   minima.append(-176.7);   maxima.append(173.33); dof+=1 
   joints.append("chest_Zrotation");     minima.append(-63.64);   maxima.append(59.23);  dof+=1 
   joints.append("chest_Xrotation");     minima.append(-59.62);   maxima.append(42.48);  dof+=1 
   joints.append("chest_Yrotation");     minima.append(-30.52);   maxima.append(34.27);  dof+=1 
   joints.append("neck1_Zrotation");     minima.append(-74.41);   maxima.append(97.04);  dof+=1 
   joints.append("neck1_Xrotation");     minima.append(-63.63);   maxima.append(81.78);  dof+=1 
   joints.append("neck1_Yrotation");     minima.append(-63.31);   maxima.append(71.25);  dof+=1 
   joints.append("head_Zrotation");      minima.append(minR);     maxima.append(maxR);   dof+=1 
   joints.append("head_Xrotation");      minima.append(-89.24);   maxima.append(86.77);  dof+=1 
   joints.append("head_Yrotation");      minima.append(-178.54);  maxima.append(maxR);   dof+=1 
   #-------------------------------------------------------------------------------
   joints.append("rshoulder_Zrotation"); minima.append(minR);     maxima.append(maxR);   dof+=1 
   joints.append("rshoulder_Xrotation"); minima.append(-102.07);  maxima.append(103.87); dof+=1 
   joints.append("rshoulder_Yrotation"); minima.append(minR);     maxima.append(maxR);   dof+=1 
   joints.append("relbow_Zrotation");    minima.append(-44.67);   maxima.append(34.52);  dof+=1 
   joints.append("relbow_Xrotation");    minima.append(-68.55);   maxima.append(9.42);   dof+=1 
   joints.append("relbow_Yrotation");    minima.append(-110.29);  maxima.append(164.34); dof+=1 
   joints.append("rhand_Zrotation");     minima.append(-169.3);   maxima.append(4.01);   dof+=1 
   joints.append("rhand_Xrotation");     minima.append(-88.3);    maxima.append(85.24);  dof+=1 
   joints.append("rhand_Yrotation");     minima.append(minR);     maxima.append(maxR);   dof+=1 
   #-------------------------------------------------------------------------------
   joints.append("lshoulder_Zrotation"); minima.append(minR);     maxima.append(maxR);   dof+=1 
   joints.append("lshoulder_Xrotation"); minima.append(-103.92);  maxima.append(103.45); dof+=1 
   joints.append("lshoulder_Yrotation"); minima.append(minR);     maxima.append(maxR);   dof+=1 
   joints.append("lelbow_Zrotation");    minima.append(-14.97);   maxima.append(47.34);  dof+=1 
   joints.append("lelbow_Xrotation");    minima.append(-68.43);   maxima.append(8.68);   dof+=1 
   joints.append("lelbow_Yrotation");    minima.append(-163.07);  maxima.append(15.34);  dof+=1 
   joints.append("lhand_Zrotation");     minima.append(-7.17);    maxima.append(172.41); dof+=1 
   joints.append("lhand_Xrotation");     minima.append(-88.13);   maxima.append(85.43);  dof+=1 
   joints.append("lhand_Yrotation");     minima.append(minR);     maxima.append(maxR);   dof+=1 
   #-----------------------------------
   return minima,maxima,joints,constants,dof,rootDofs


#python3 sobolRandomDatasetGenerator.py --exponent 21 --lowerbody --copy
def getLowerbodyList():
   #-----------------------------------
   minima    = list()
   maxima    = list()
   joints    = list()
   constants = dict()
   rootDofs  = list()
   minR = -179.90
   maxR =  179.90
   dof       = 0
   #-------------------------------------------------------------------------------
   #These are no longer needed because they are randomized by the GroundTruthGenerator tool!
   #joints.append("hip_Xposition");     minima.append(-340.0);  maxima.append(340.0);   rootDofs.append(dof); dof+=1   # -529 529 original 
   #joints.append("hip_Yposition");     minima.append(-80.0);   maxima.append(80.0);    rootDofs.append(dof); dof+=1     # -207 208 original
   #joints.append("hip_Zposition");     minima.append(-500.0);  maxima.append(-100.0);  rootDofs.append(dof); dof+=1      # -450 -90 original 
   #-------------------------------------------------------------------------------
   #joints.append("hip_Zrotation");     minima.append(-45.0);   maxima.append(45.0);    rootDofs.append(dof); dof+=1 
   #joints.append("hip_Yrotation");     minima.append(minR);    maxima.append(maxR);    rootDofs.append(dof); dof+=1 
   #joints.append("hip_Xrotation");     minima.append(-45.0);   maxima.append(45.0);    rootDofs.append(dof); dof+=1 
   #-------------------------------------------------------------------------------
   #joints.append("abdomen_Zrotation"); minima.append(-174.88); maxima.append(174.88);  dof+=1 
   #joints.append("abdomen_Xrotation"); minima.append(-90.0);   maxima.append(90.0);    dof+=1 
   #joints.append("abdomen_Yrotation"); minima.append(minR);    maxima.append(maxR);    dof+=1 
   #joints.append("chest_Zrotation");   minima.append(-57.0);   maxima.append(57.0);    dof+=1 
   #joints.append("chest_Xrotation");   minima.append(-61.78);  maxima.append(40.94);   dof+=1 
   #joints.append("chest_Yrotation");   minima.append(-35.27);  maxima.append(27.58);   dof+=1 
   #joints.append("neck1_Zrotation");    minima.append(-76.28);  maxima.append(97.18);   dof+=1 
   #joints.append("neck1_Xrotation");    minima.append(-63.08);  maxima.append(81.78);   dof+=1 
   #joints.append("neck1_Yrotation");    minima.append(-63.31);  maxima.append(71.64);   dof+=1 
   #-------------------------------------------------------------------------------
   joints.append("rhip_Zrotation");    minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("rhip_Xrotation");    minima.append(-104.0);  maxima.append(94.0);   dof+=1 #Default -104.11 .. 73.67
   joints.append("rhip_Yrotation");    minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("rknee_Zrotation");   minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("rknee_Xrotation");   minima.append(-66.0);   maxima.append(94.0);   dof+=1 #Default -66.73 .. 94.07
   joints.append("rknee_Yrotation");   minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("rfoot_Zrotation");   minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("rfoot_Xrotation");   minima.append(-89.0);   maxima.append(89.0);   dof+=1 #Default -90.86 .. 89.26
   joints.append("rfoot_Yrotation");   minima.append(minR);    maxima.append(maxR);   dof+=1 
   #-------------------------------------------------------------------------------
   joints.append("lhip_Zrotation");    minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("lhip_Xrotation");    minima.append(-104.0);  maxima.append(94.0);   dof+=1  #Default -104.23 .. 93.92
   joints.append("lhip_Yrotation");    minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("lknee_Zrotation");   minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("lknee_Xrotation");   minima.append(-66.0);   maxima.append(94.0);   dof+=1  #Default -46.36 .. 94.34
   joints.append("lknee_Yrotation");   minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("lfoot_Zrotation");   minima.append(minR);    maxima.append(maxR);   dof+=1 
   joints.append("lfoot_Xrotation");   minima.append(-89.0);   maxima.append(89.0);   dof+=1  #Default -89.38 .. 91.81
   joints.append("lfoot_Yrotation");   minima.append(minR);    maxima.append(maxR);   dof+=1 
   #-----------------------------------
   return minima,maxima,joints,constants,dof,rootDofs




def getFullFaceList():
   #-----------------------------------
   minima    = list()
   maxima    = list()
   joints    = list()
   constants = dict()
   rootDofs  = list()
   dof       = 0
   #/home/ammar/Programs/blender-3.4.1-linux-x64/3.4/scripts/addons/io_anim_bvh/import_bvh.py
   #-----------------------------------
   joints.append("hip_Xposition");             minima.append(-0.24);  maxima.append(0.24);  rootDofs.append(dof); dof+=1 
   joints.append("hip_Yposition");             minima.append(-0.10);  maxima.append(0.10);  rootDofs.append(dof); dof+=1 
   joints.append("hip_Zposition");             minima.append(-2.4);   maxima.append(-1.0);  rootDofs.append(dof); dof+=1 
   joints.append("neck1_Zrotation");           minima.append(-20.0);  maxima.append(20.0);  rootDofs.append(dof); dof+=1 
   joints.append("neck1_Xrotation");           minima.append(-20.0);  maxima.append(20.0);  rootDofs.append(dof); dof+=1 
   joints.append("neck1_Yrotation");           minima.append(-30.0);  maxima.append(30.0);  rootDofs.append(dof); dof+=1 
   joints.append("eye.R_Zrotation");           minima.append(-45.36); maxima.append(45.36); dof+=1 
   joints.append("eye.R_Xrotation");           minima.append(-10.0);  maxima.append(16.0);  dof+=1 
   #Let's assume eyes move together..
   #joints.append("eye.L_Zrotation"); minima.append(-20.0); maxima.append(20.0)
   #joints.append("eye.L_Xrotation"); minima.append(-7.0); maxima.append(20.0)
   joints.append("oculi01.R_Zrotation");       minima.append(-20.0); maxima.append(20.0);  dof+=1 
   joints.append("oculi01.L_Zrotation");       minima.append(-20.0); maxima.append(20.0);  dof+=1 
   joints.append("orbicularis03.R_Xrotation"); minima.append(-15.0); maxima.append(15.0);  dof+=1 
   joints.append("orbicularis03.L_Xrotation"); minima.append(-15.0); maxima.append(15.0);  dof+=1 
   #joints.append("orbicularis04.R_Xrotation"); minima.append(-15.0);   maxima.append(15.0) #This is flipped orbicularis03.R_Xrotation
   #joints.append("orbicularis04.L_Xrotation"); minima.append(-15.0);   maxima.append(15.0) #This is flipped orbicularis03.L_Xrotation
   joints.append("levator06.L_Xrotation");     minima.append(-9.0);  maxima.append(9.0);   dof+=1 
   #joints.append("levator06.R_Xrotation");     minima.append(-9.0);  maxima.append(9.0) #This is levator06.L_Xrotation
   joints.append("levator03.L_Zrotation");     minima.append(-8.0);  maxima.append(9.0);   dof+=1 
   #joints.append("levator03.R_Zrotation");     minima.append(-9.0);  maxima.append(8.0);  dof+=1  #This is flipped levator03.L_Zrotation
   joints.append("oris03.L_Zrotation");        minima.append(-30.0); maxima.append(30.0);  dof+=1 
   joints.append("oris03.R_Zrotation");        minima.append(-30.0); maxima.append(30.0);  dof+=1 
   joints.append("oris07.L_Zrotation");        minima.append(-30.0); maxima.append(0.0);   dof+=1 
   joints.append("oris07.R_Zrotation");        minima.append(-30.0); maxima.append(0.0);   dof+=1 
   joints.append("jaw_Xrotation");             minima.append(-4.0);  maxima.append(20.0);  dof+=1 
   joints.append("jaw_Yrotation");             minima.append(-15.0); maxima.append(15.0);  dof+=1 
   joints.append("oris04.L_Zrotation");        minima.append(-30.0); maxima.append(0.0);   dof+=1 
   joints.append("oris04.R_Zrotation");        minima.append(0.0);   maxima.append(30.0);  dof+=1 
   joints.append("oris06.L_Zrotation");        minima.append(-30.0); maxima.append(30.0);  dof+=1 
   joints.append("oris06.R_Zrotation");        minima.append(-30.0); maxima.append(30.0);  dof+=1 
   #-----------------------------------
   constants["orbicularis03.R_Yrotation"] = 172.0
   constants["orbicularis04.R_Yrotation"] = 172.0
   constants["orbicularis03.L_Yrotation"] = -172.0
   constants["orbicularis04.L_Yrotation"] = 172.0
   constants["levator06.L_Yrotation"]     = -247.0
   constants["levator06.R_Yrotation"]     = 247.0
   constants["oris03.L_Xrotation"]        = -40.0
   constants["oris03.L_Yrotation"]        = 172.0
   constants["oris07.L_Yrotation"]        = 172.0
   constants["oris03.R_Xrotation"]        = -40.0
   constants["oris03.R_Yrotation"]        = 179.0
   constants["oris07.R_Yrotation"]        = 172.0
   constants["oris05_Xrotation"]          = -35.0
   constants["oris05_Yrotation"]          = -176.0
   #-----------------------------------
   return minima,maxima,joints,constants,dof,rootDofs


def getReyeList():
   #-----------------------------------
   minima    = list()
   maxima    = list()
   joints    = list()
   constants = dict()
   rootDofs  = list()
   dof       = 0
   #-----------------------------------
   joints.append("hip_Xposition");             minima.append(-0.24);  maxima.append(0.24);  rootDofs.append(dof); dof+=1 
   joints.append("hip_Yposition");             minima.append(-0.10);  maxima.append(0.10);  rootDofs.append(dof); dof+=1 
   joints.append("hip_Zposition");             minima.append(-2.3);   maxima.append(-1.0);  rootDofs.append(dof); dof+=1 
   joints.append("neck1_Zrotation");           minima.append(-20.0);  maxima.append(20.0);  rootDofs.append(dof); dof+=1 
   joints.append("neck1_Xrotation");           minima.append(-20.0);  maxima.append(20.0);  rootDofs.append(dof); dof+=1 
   joints.append("neck1_Yrotation");           minima.append(-30.0);  maxima.append(30.0);  rootDofs.append(dof); dof+=1 
   joints.append("eye.R_Zrotation");           minima.append(-45.36); maxima.append(45.36); dof+=1 
   joints.append("eye.R_Xrotation");           minima.append(-10.0);  maxima.append(16.0);  dof+=1 
   joints.append("oculi01.R_Zrotation");       minima.append(-20.0);  maxima.append(20.0);  dof+=1 
   joints.append("orbicularis03.R_Xrotation"); minima.append(-15.0);  maxima.append(15.0);  dof+=1 
   joints.append("jaw_Xrotation");             minima.append(-4.0);   maxima.append(20.0);  dof+=1  #Reason being adding some robustness
   joints.append("jaw_Yrotation");             minima.append(-15.0);  maxima.append(15.0);  dof+=1  #Reason being adding some chin robustness
   #-----------------------------------
   constants["orbicularis03.R_Yrotation"] = 172.0
   constants["orbicularis04.R_Yrotation"] = 172.0
   #-----------------------------------
   return minima,maxima,joints,constants,dof,rootDofs


def getMouthList():
   #-----------------------------------
   minima    = list()
   maxima    = list()
   joints    = list()
   constants = dict()
   rootDofs  = list()
   dof       = 0
   #-----------------------------------
   joints.append("hip_Xposition");             minima.append(-0.24); maxima.append(0.24);  rootDofs.append(dof); dof+=1 
   joints.append("hip_Yposition");             minima.append(-0.10); maxima.append(0.10);  rootDofs.append(dof); dof+=1
   joints.append("hip_Zposition");             minima.append(-2.4);  maxima.append(-1.0);  rootDofs.append(dof); dof+=1
   joints.append("neck1_Zrotation");           minima.append(-20.0); maxima.append(20.0);  rootDofs.append(dof); dof+=1
   joints.append("neck1_Xrotation");           minima.append(-20.0); maxima.append(20.0);  rootDofs.append(dof); dof+=1
   joints.append("neck1_Yrotation");           minima.append(-30.0); maxima.append(30.0);  rootDofs.append(dof); dof+=1
   joints.append("levator06.L_Xrotation");     minima.append(-9.0);  maxima.append(9.0);   dof+=1
   #joints.append("levator06.R_Xrotation");    minima.append(-9.0);  maxima.append(9.0);   dof+=1 #This is levator06.L_Xrotation
   joints.append("levator03.L_Zrotation");     minima.append(-8.0);  maxima.append(9.0);   dof+=1
   #joints.append("levator03.R_Zrotation");    minima.append(-9.0);  maxima.append(8.0);   dof+=1 #This is flipped levator03.L_Zrotation
   joints.append("oris03.L_Zrotation");        minima.append(-30.0); maxima.append(30.0);  dof+=1
   joints.append("oris03.R_Zrotation");        minima.append(-30.0); maxima.append(30.0);  dof+=1
   joints.append("oris07.L_Zrotation");        minima.append(-30.0); maxima.append(0.0);   dof+=1
   joints.append("oris07.R_Zrotation");        minima.append(-30.0); maxima.append(0.0);   dof+=1
   joints.append("jaw_Xrotation");             minima.append(-4.0);  maxima.append(20.0);  dof+=1
   joints.append("jaw_Yrotation");             minima.append(-15.0); maxima.append(15.0);  dof+=1
   joints.append("oris04.L_Zrotation");        minima.append(-30.0); maxima.append(0.0);   dof+=1
   joints.append("oris04.R_Zrotation");        minima.append(0.0);   maxima.append(30.0);  dof+=1
   joints.append("oris06.L_Zrotation");        minima.append(-30.0); maxima.append(30.0);  dof+=1
   joints.append("oris06.R_Zrotation");        minima.append(-30.0); maxima.append(30.0);  dof+=1
   #-----------------------------------
   constants["levator06.L_Yrotation"]     = -247.0
   constants["levator06.R_Yrotation"]     = 247.0
   constants["oris03.L_Xrotation"]        = -40.0
   constants["oris03.L_Yrotation"]        = 172.0
   constants["oris07.L_Yrotation"]        = 172.0
   constants["oris03.R_Xrotation"]        = -40.0
   constants["oris03.R_Yrotation"]        = 179.0
   constants["oris07.R_Yrotation"]        = 172.0
   constants["oris05_Xrotation"]          = -35.0
   constants["oris05_Yrotation"]          = -176.0
   #-----------------------------------
   return minima,maxima,joints,constants,dof,rootDofs



def getLHandList():
   #--------------------------------------
   minima    = list()
   maxima    = list()
   joints    = list()
   constants = dict()
   rootDofs  = list()
   dof       = 0
   #--------------------------------------
   joints.append("lhand_Xposition");       minima.append(-120.0); maxima.append(120.0); rootDofs.append(dof); dof+=1  #230
   joints.append("lhand_Yposition");       minima.append(-60.0);  maxima.append(60.0);  rootDofs.append(dof); dof+=1  #92
   joints.append("lhand_Zposition");       minima.append(-250.0); maxima.append(-75.0); rootDofs.append(dof); dof+=1
   #joints.append("padding_1");             minima.append(0.0);    maxima.append(0.0);   rootDofs.append(dof); dof+=1 #<- Align to 4 elements
   #--------------------------------------
   joints.append("lhand_Wrotation");       minima.append(-1.0);   maxima.append(1.0);   rootDofs.append(dof); dof+=1
   joints.append("lhand_Xrotation");       minima.append(-1.0);   maxima.append(1.0);   rootDofs.append(dof); dof+=1
   joints.append("lhand_Yrotation");       minima.append(-1.0);   maxima.append(1.0);   rootDofs.append(dof); dof+=1
   joints.append("lhand_Zrotation");       minima.append(-1.0);   maxima.append(1.0);   rootDofs.append(dof); dof+=1
   #--------------------------------------
   joints.append("finger2-1.l_Zrotation"); minima.append(-90.0);  maxima.append(10.0);  dof+=1
   joints.append("finger2-1.l_Yrotation"); minima.append(-20.0);  maxima.append(20.0);  dof+=1
   joints.append("finger2-2.l_Zrotation"); minima.append(-90.0);  maxima.append(0.0);   dof+=1
   joints.append("finger2-3.l_Zrotation"); minima.append(-45.0);  maxima.append(0.0);   dof+=1
   #--------------------------------------
   joints.append("finger3-1.l_Zrotation"); minima.append(-90.0);  maxima.append(10.0);  dof+=1
   joints.append("finger3-1.l_Yrotation"); minima.append(-10.0);  maxima.append(10.0);  dof+=1
   joints.append("finger3-2.l_Zrotation"); minima.append(-90.0);  maxima.append(0.0);   dof+=1
   joints.append("finger3-3.l_Zrotation"); minima.append(-45.0);  maxima.append(0.0);   dof+=1
   #--------------------------------------
   joints.append("finger4-1.l_Zrotation"); minima.append(-90.0);  maxima.append(10.0);  dof+=1
   joints.append("finger4-1.l_Yrotation"); minima.append(-10.0);  maxima.append(10.0);  dof+=1
   joints.append("finger4-2.l_Zrotation"); minima.append(-90.0);  maxima.append(0.0);   dof+=1
   joints.append("finger4-3.l_Zrotation"); minima.append(-45.0);  maxima.append(0.0);   dof+=1
   #--------------------------------------
   joints.append("finger5-1.l_Zrotation"); minima.append(-90.0);  maxima.append(10.0);  dof+=1
   joints.append("finger5-1.l_Yrotation"); minima.append(-8.0);   maxima.append(25.0);  dof+=1
   joints.append("finger5-2.l_Zrotation"); minima.append(-90.0);  maxima.append(0.0);   dof+=1
   joints.append("finger5-3.l_Zrotation"); minima.append(-45.0);  maxima.append(0.0);   dof+=1
   #--------------------------------------
   joints.append("lthumbBase_Zrotation");  minima.append(0.0);    maxima.append(60.0);  dof+=1
   joints.append("lthumbBase_Xrotation");  minima.append(-35.0);  maxima.append(0.0);   dof+=1
   joints.append("lthumbBase_Yrotation");  minima.append(0.0);    maxima.append(60.0);  dof+=1
   #joints.append("padding_2");             minima.append(0.0);    maxima.append(0.0);   #<- Align to 4 elements
   #--------------------------------------
   joints.append("lthumb_Zrotation");      minima.append(-85.0);  maxima.append(85.0);  dof+=1
   joints.append("lthumb_Xrotation");      minima.append(-30.0);  maxima.append(48.0);  dof+=1
   joints.append("lthumb_Yrotation");      minima.append(0.0);    maxima.append(85.0);  dof+=1
   #joints.append("padding_3");             minima.append(0.0);    maxima.append(0.0);   #<- Align to 4 elements
   #--------------------------------------
   joints.append("finger1-2.l_Zrotation"); minima.append(-35.0);  maxima.append(0.0);   dof+=1
   joints.append("finger1-2.l_Xrotation"); minima.append(-40.0);  maxima.append(45.0);  dof+=1
   joints.append("finger1-2.l_Yrotation"); minima.append(-70.0);  maxima.append(35.0);  dof+=1
   #joints.append("padding_4");             minima.append(0.0);    maxima.append(0.0);   #<- Align to 4 elements
   #--------------------------------------
   joints.append("finger1-3.l_Zrotation"); minima.append(-50.0);  maxima.append(0.0);   dof+=1
   joints.append("finger1-3.l_Xrotation"); minima.append(0.0);    maxima.append(50.0);  dof+=1
   #joints.append("padding_5");             minima.append(0.0);    maxima.append(0.0);   #<- Align to 4 elements
   #joints.append("padding_6");             minima.append(0.0);    maxima.append(0.0);   #<- Align to 4 elements
   #--------------------------------------
   return minima,maxima,joints,constants,dof,rootDofs



#------------------------------------------
#      Generate actual samples..
#------------------------------------------
minima    = list()
maxima    = list()
joints    = list()
constants = dict()
rootDofs  = list()
dof       = 0

datasetPart    = "Face"
exponent       = 17
doPlots        = False
doFinalCopy    = False
neutralSamples = 0

#----------------
#   Exponent
#----------------
# 14 = 16.384
# 15 = 32.768
# 16 = 65.536
# 17 = 131.072
# 18 = 262.144
# 19 = 524.288
# 20 = 1.048.576
# 21 = 2.097.152
# 22 = 4.194.304
# 23 = 8.388.608
# 24 = 16.777.216
#----------------

import sys
mem = 1.0 
if (len(sys.argv)>1):
       print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--scanreye"):
              scanReye("SCANREYE.csv")
           if (sys.argv[i]=="--copy"):
              doFinalCopy = True
           if (sys.argv[i]=="--plot"):
              doPlots = True
           if (sys.argv[i]=="--neutral"):
              neutralSamples = int(sys.argv[i+1])
           elif (sys.argv[i]=="--exponent"):
              exponent = int(sys.argv[i+1])
           elif (sys.argv[i]=="--upperbody"):
              datasetPart = "Upperbody"
              minima,maxima,joints,constants,dof,rootDofs = getUpperbodyList()
           elif (sys.argv[i]=="--lowerbody"):
              datasetPart = "Lowerbody"
              minima,maxima,joints,constants,dof,rootDofs = getLowerbodyList()
           elif (sys.argv[i]=="--lhand"):
              datasetPart = "LHand"
              minima,maxima,joints,constants,dof,rootDofs = getLHandList()
           elif (sys.argv[i]=="--reye"):
              datasetPart = "REye"
              minima,maxima,joints,constants,dof,rootDofs = getReyeList()
           elif (sys.argv[i]=="--mouth"):
              datasetPart = "Mouth"
              minima,maxima,joints,constants,dof,rootDofs = getMouthList()
           elif (sys.argv[i]=="--face"):
              datasetPart = "Face"
              minima,maxima,joints,constants,dof,rootDofs = getFullFaceList()



if (len(minima)==0) and (len(maxima)==0) and (len(joints)==0):
       print("Please rerun using --face OR --mouth OR --reye OR --lhand OR --upperbody OR --lowerbody to select a target")


numberOfDimensions = len(joints)
numberOfSamples = 2**exponent
filename = "sobol%s_%u.csv" % (datasetPart,numberOfSamples+neutralSamples)
#------------------------------------------


print("Will generate ",numberOfSamples," samples ")
print("Will generate random numbers for ",numberOfDimensions," dimensions")
print("We have ",dof," degrees of freedom")
 
samplesPerDimension = np.power(numberOfSamples,1/dof)
print("Samples per Dimension : %0.2f "%samplesPerDimension)



print("Involved joints are : ",joints)
sampler = qmc.Sobol(d=numberOfDimensions, scramble=False)
sample = sampler.random_base2(m=exponent)
print("Numbers generated now storing them to ",filename)
#print(sample)


#=========================================
#             Write label
#=========================================
f = open(filename, 'w')
recordsWritten=0 
#-----------------------------------------
for item in range(0,numberOfDimensions):
  if (recordsWritten>0):
               f.write(',')
  #f.write("dim%u"%(recordsWritten))
  f.write("%s" % (joints[item]))
  recordsWritten=recordsWritten+1
#-----------------------------------------
if (constants):
  for key in constants.keys():
    if (recordsWritten>0):
               f.write(',')
    #f.write("dim%u"%(recordsWritten))
    f.write("%s" % (joints[item]))
    recordsWritten=recordsWritten+1
#-----------------------------------------
f.write("\n")
#=========================================


#=========================================
#           Write Sobol Samples
#=========================================
for sampleNumber in range(0,numberOfSamples):
   recordsWritten=0 
   for item in range(0,numberOfDimensions):
       if (recordsWritten>0):
                    f.write(',')
       thisRange = maxima[item]-minima[item]
       f.write("%f"%(  (thisRange * sample[sampleNumber][item]) + minima[item])  )
       recordsWritten=recordsWritten+1
   if (constants):
     for key in constants.keys():
       if (recordsWritten>0):
                    f.write(',')
       f.write("%f"%(constants[key]))
       recordsWritten=recordsWritten+1
   f.write("\n")
#=========================================


#=========================================
#           Write Neutral Samples
#=========================================
if (neutralSamples>0):
 print("We will also append %u neutral samples " % neutralSamples)
#=========================================
 for sampleNumber in range(0,neutralSamples):
   recordsWritten=0 
   for item in range(0,numberOfDimensions):
       if (recordsWritten>0):
                    f.write(',')

       if (item in rootDofs):
          #Poor quality uniform random value
          value = random.uniform(minima[item],maxima[item]) 
       else:
          #True Neutral mean value
          value = float((maxima[item]+minima[item]) / 2)
       f.write("%f" % value)
       recordsWritten=recordsWritten+1
   if (constants):
     for key in constants.keys():
       if (recordsWritten>0):
                    f.write(',')
       f.write("%f"%(constants[key]))
       recordsWritten=recordsWritten+1
   f.write("\n")
#=========================================


f.close()

import os
#------------------------------
if (doPlots):
  os.system("python3 plotCSV.py --simple %s "%filename)
#------------------------------
command1 = ""
command2 = ""
command3 = ""
havePayload = False
minOrientation = -179.90
maxOrientation =  179.90
#------------------------------
if (datasetPart == "LHand"):
   command1 = "rm dataset/generated/2d_lhand_all.csv dataset/generated/bvh_lhand_all.csv"
   command2 = "./GroundTruthDumper --from dataset/lhand.qbvh --importCSVPoses %s --randomize2D 200 2500 -179.0 -179.0 -179.0 179.0 179.0 179.0 --selectJoints 1 17 lHand finger5-1.l finger5-2.l finger5-3.l finger4-1.l finger4-2.l finger4-3.l finger3-1.l finger3-2.l finger3-3.l finger2-1.l finger2-2.l finger2-3.l lthumbBase lthumb finger1-2.l finger1-3.l --hide2DLocationOfJoints 0 1 lthumbBase --csv dataset/generated/ lhand_all.csv 2d+bvh " % (filename)
   command3 = "python3 compressCSVFile.py dataset/generated/bvh_lhand_all.csv && mv dataset/generated/bvh_lhand_all.csv dataset/generated/bvh_lhand_all.csv.original && mv compressed.csv dataset/generated/bvh_lhand_all.csv "
   havePayload = True
elif (datasetPart == "Upperbody"):
   command1 = "rm dataset/generated/2d_upperbody_all.csv dataset/generated/bvh_upperbody_all.csv"
   command2 = "./GroundTruthDumper --from dataset/headerWithHeadAndOneMotion.bvh --importCSVPoses %s --randomize2D 1000 4500 -45 %0.2f -45 45 %0.2f 45 --selectJoints 1 13 hip eye.r eye.l abdomen chest neck head rshoulder relbow rhand lshoulder lelbow lhand --hide2DLocationOfJoints 0 4 abdomen chest eye.r eye.l --csv dataset/generated/ upperbody_all.csv 2d+bvh " % (filename,minOrientation,maxOrientation)
   havePayload = True
elif (datasetPart == "Lowerbody"):
   command1 = "rm dataset/generated/2d_lowerbody_all.csv dataset/generated/bvh_lowerbody_all.csv"
   command2 = "./GroundTruthDumper --from dataset/headerWithHeadAndOneMotion.bvh --importCSVPoses %s --randomize2D 1000 4500 -45 %0.2f -45 45 %0.2f 45 --selectJoints 1 14 hip abdomen chest neck rhip rknee rfoot lhip lknee lfoot toe1-2.r toe5-3.r toe1-2.l toe5-3.l --hide2DLocationOfJoints 0 6 abdomen chest toe1-2.r toe5-3.r toe1-2.l toe5-3.l --csv dataset/generated/ lowerbody_all.csv 2d+bvh " % (filename,minOrientation,maxOrientation)
   havePayload = True
#------------------------------
if (havePayload):
   if (doFinalCopy):
     os.system(command1)
     os.system(command2)
     if (command3!=""):
       os.system(command3)
   else:
     print("To copy data, please execute : ")
     print(command1)
     print(command2)
     if (command3!=""):
       print(command3)
#------------------------------



