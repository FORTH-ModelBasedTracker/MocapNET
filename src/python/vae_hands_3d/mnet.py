from __future__ import print_function, unicode_literals
import tensorflow as tf
import numpy as np

from data.BinaryDbReader import *
from data.BinaryDbReaderSTB import *
from utils.general import EvalUtil, get_stb_ref_curves, calc_auc, np2pyt, pyt2np
import torch
import torch.nn as nn
from torch.autograd import Variable
import sys
import os
import cv2 as cv
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

device=torch.device("cpu")

def writeCSVFileHeader(filenameOutput,inputListLabels,inputStart,inputEnd,channels):
    inputNumber=0
    fileCSV = open(filenameOutput,"w") 
    for entry in inputListLabels[inputStart:inputEnd*channels]:
        #if (inputNumber>=inputStart) and (inputNumber<inputEnd): 
           fileCSV.write(str(entry))
           fileCSV.write(",")
        #++inputNumber
    fileCSV.write("\n")
    fileCSV.close()   
 

def writeCSVFileBody(filenameOutput,inputList,inputStart,inputEnd,channels):
    inputNumber=0
    fileCSV = open(filenameOutput,"a") 
    for entry in inputList[inputStart:inputEnd]:
      if (channels==1):
        fileCSV.write(str(entry))
        fileCSV.write(",")
      else:
        for ch in entry:
        #if (inputNumber>=inputStart) and (inputNumber<inputEnd): 
           fileCSV.write(str(ch))
           fileCSV.write(",")
        #++inputNumber 
    fileCSV.write("\n")
    fileCSV.close()   

labelsHandSide=list() 
labelsHandSide.append("left")
labelsHandSide.append("right")
writeCSVFileHeader("handSide.csv",labelsHandSide,0,2,1)

     
labels3D=list()
for i in range (0,22):
  labels3D.append("3DX_%u" % i)
  labels3D.append("3DY_%u" % i)
  labels3D.append("3DZ_%u" % i)
#writeCSVFileHeader("dumped3D.csv",labels3D,0,21,3)
writeCSVFileHeader("ground3D.csv",labels3D,0,21,3)


labels2D=list()
for i in range (0,22):
  labels2D.append("2DX_%u" % i)
  labels2D.append("2DY_%u" % i) 
writeCSVFileHeader("ground2D.csv",labels2D,0,21,2)



labels1D=list()
for i in range (0,22):
  labels1D.append("visible_%u" % i) 
writeCSVFileHeader("visibility.csv",labels1D,0,21,1)


os.system("rm images/*.png")
plt.ion()
# Experimental settings
hand_side_invariance = True
scale_invariance = True
dataset_name = 'RHD' #STB
# Only change this if you have moved the dataset to a different folder
path_to_db = 'data/'
# Load the correct model depending on the experiment
model_name = 'model_'
if scale_invariance and hand_side_invariance:
    if dataset_name == 'RHD':
        model_name += 'allInvar'
    elif dataset_name == 'STB':
        model_name += 'allInvar_RGB2RGB'
elif scale_invariance and not hand_side_invariance:
    model_name += 'noHandInvar_scaleInvar'
elif not scale_invariance and hand_side_invariance:
    model_name += 'handInvar_noScaleInvar'
else:
    model_name += 'noHandInvar_noScaleInvar'
model_path = os.path.join('models', model_name)
sys.path.append(os.path.join(model_path, 'source_files'))
from models import *
# Create the experiment folder
try:
    os.makedirs(model_path + '/eval_results')
except:
    pass
# Load the selected dataset
if dataset_name == 'RHD':
    path_to_db = os.path.join(path_to_db, 'bin/')
    dataset = BinaryDbReader(shuffle=False, use_wrist_coord=False, hand_crop=True,path_to_db=path_to_db)
elif dataset_name == 'STB':
    path_to_db = os.path.join(path_to_db, 'stb/')
    dataset = BinaryDbReaderSTB(shuffle=False, use_wrist_coord=False, hand_crop=True,path_to_db=path_to_db)
else:
    raise Exception("Unknown dataset")
# Build dataset
data = dataset.get()
# Build model
n_joints = 21
z_dim = 30
end_size = (256, 256)

# get keys as list
keys = list(data.keys())
#print keys
print(keys) 


# RGB
rgb_enc = resnet18(z_dim, hand_side_invariance)
rgb_dec = rgb_decoder(z_dim)
VAE_RGB = VAE(z_dim, rgb_enc, rgb_dec)
#VAE_RGB.cuda()
VAE_RGB.eval()
# 3D
joint_3D_enc = joints_encoder(z_dim, [n_joints, 3])
joint_3D_dec = joints_decoder(z_dim, [n_joints, 3])
VAE_3D = VAE(z_dim, joint_3D_enc, joint_3D_dec)
#VAE_3D.cuda()
VAE_3D.eval()
# Load params for own models
checkpoint = torch.load(model_path + '/model_weights.pth')
VAE_RGB.load_state_dict(checkpoint['state_dict_VAERGB'])
VAE_3D.load_state_dict(checkpoint['state_dict_VAE3D'])
# Start TF
gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.7)
sess = tf.Session(config=tf.ConfigProto(gpu_options=gpu_options))
tf.train.start_queue_runners(sess=sess)

colorlist_pred = ['#660000', '#b30000', '#ff0000', '#ff4d4d', '#ff9999']
colorlist_gt = ['#000066', '#0000b3', '#0000ff', '#4d4dff', '#9999ff']

# Plots all 5 fingers
def plot_fingers(points, plt_specs, c, ax):
    for i in range(5):
        start, end = i*4+1, (i+1)*4+1
        to_plot = np.concatenate((points[start:end], points[0:1]), axis=0)
        ax.plot(to_plot[:,0], to_plot[:,1], to_plot[:,2], plt_specs, color=c[i])

fig = None
#util_vae = EvalUtil()
# iterate dataset
for i in range(dataset.num_samples):
    # Get required data
    keypoint_xyz21, keypoint_vis21, keypoint_scale, img_crop, hand_side =  sess.run([data['keypoint_xyz21'], data['keypoint_vis21'], data['keypoint_scale'], data['image_crop'], data['hand_side']])

    keypoint_xyz21 = np.squeeze(keypoint_xyz21)
    keypoint_vis21 = np.squeeze(keypoint_vis21)
    keypoint_scale = np.squeeze(keypoint_scale)
    img_crop = np.squeeze(img_crop)
    
    #print(i," side ",hand_side)
    writeCSVFileBody("handSide.csv",hand_side[0],0,2,1)
    # Get VAE prediction
    #img_res = cv.resize(img_crop, end_size)
    #img_pyt = img_res.transpose(2, 0, 1).reshape((1, 3, end_size[0], end_size[1]))
    #img_crop_var = Variable(np2pyt(img_pyt), volatile=True).cuda()
    #if hand_side_invariance:
    #    hand_side_var = Variable(np2pyt(hand_side), volatile=True).cuda()
    #else:
    #    hand_side_var = None
    #coords_3D, _, _ = VAE_RGB(img_crop_var, VAE_3D, hand_side_var)
    #coords_3D_np = np.squeeze(pyt2np(coords_3D))

    # rescale to meters
    #if scale_invariance:
    #    coords_3D_np *= keypoint_scale

    # center gt
    keypoint_xyz21 -= keypoint_xyz21[0, :]
    
    #print(coords_3D_np) 
    #writeCSVFileBody("dumped3D.csv",coords_3D_np,0,22,3)
    
    #------------------------------------------------ 
    writeCSVFileBody("ground3D.csv",keypoint_xyz21,0,22,3)
    writeCSVFileBody("visibility.csv",keypoint_vis21,0,22,1)
    #Dump Images
    plt.imsave("images/im%u.png"%i, img_crop+ 0.5)

    #sys.exit(0)
    # Feed it to the evaluater
    #util_vae.feed(keypoint_xyz21, keypoint_vis21, coords_3D_np)

    if (i % 100) == 0:
        print('%d / %d images done: %.3f percent' % (i, dataset.num_samples, i*100.0/dataset.num_samples))
        if not fig:
            # visualize
            fig = plt.figure(figsize=(15,10))
            ax1 = fig.add_subplot(131)
            ax2 = fig.add_subplot(132, projection='3d')
            ax3 = fig.add_subplot(133, projection='3d')
        ax1.clear()
        ax2.clear()
        ax3.clear()
        # Plot the entire image plus the transformed 2D prediction
        ax1.imshow(img_crop + 0.5)
        ax1.set_title("Raw image")
        ax1.axis('off')

        # Plot the ground truth and the predictions
        plot_fingers(keypoint_xyz21, '.-', colorlist_gt, ax2)
        ax2.view_init(azim=-90.0, elev=-90.0)
        ax2.set_title("Ground truth")
        #plot_fingers(coords_3D_np, '.-', colorlist_pred, ax3)
        ax3.view_init(azim=-90.0, elev=-90.0)
        ax3.set_title(model_name)

        ax2.set_xlim([-0.1, 0.1])
        ax2.set_ylim([-0.1, 0.1])
        ax2.set_zlim([-0.1, 0.1])
        #ax2.set_aspect('equal')
        ax2.set_xticklabels([])
        ax2.set_yticklabels([])
        ax2.set_zticklabels([])

        ax3.set_xlim([-0.1, 0.1])
        ax3.set_ylim([-0.1, 0.1])
        ax3.set_zlim([-0.1, 0.1])
        #ax3.set_aspect('equal')
        ax3.set_xticklabels([])
        ax3.set_yticklabels([])
        ax3.set_zticklabels([])

        plt.tight_layout()
        plt.show()
        plt.pause(0.001)
        #plt.savefig(model_path + "/eval_results/prediction_RgbTo3D_" + dataset_name + "%05d" % i)



# Output results
mean, median, auc, pck_curve_all, threshs = util_vae.get_measures(0.0, 0.050, 20)
print('Evaluation results:')
print('Average mean EPE: %.3f mm' % (mean*1000))
print('Average median EPE: %.3f mm' % (median*1000))
print('Area under curve: %.3f' % auc)
pck_curve_all, threshs = pck_curve_all[8:], threshs[8:]*1000.0
auc_subset = calc_auc(threshs, pck_curve_all)
print('Area under curve between 20mm - 50mm: %.3f' % auc_subset)

curve_list = []
curve_list.append((threshs, pck_curve_all, model_name + ' (AUC=%.3f)' % auc_subset))
if type(dataset) == BinaryDbReaderSTB:
    curve_list.extend(get_stb_ref_curves())


os.system("sed -i 's/True/1/g' visibility.csv")
os.system("sed -i 's/False/0/g' visibility.csv")
os.system("./rename_files.sh")
os.system("tar cvfjh \"%s.tar.bz2\" plot.py ground3D.csv visibility.csv images/" % dataset_name)

plt.ioff()
fig = plt.figure()
ax = fig.add_subplot(111)
for t, v, name in curve_list:
    ax.plot(t, v*100, label=name)
ax.set_xlabel('threshold in mm')
ax.set_ylabel('% of correct keypoints')
plt.ylim([0.0, 100.0])
plt.grid()
plt.legend(loc='lower right')
plt.tight_layout(rect=(0.01, -0.05, 1.03, 1.03))

plt.show()
plt.pause(0.001)
plt.savefig(model_path + "/eval_results/pck_RgbTo3D_" + dataset_name)
