#!/usr/bin/python3

"""
Author : "Ammar Qammaz"
Copyright : "2023 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH"
"""

#Dependencies should be :
#python3 -m pip install tensorflow==2.15.0 numpy tensorboard opencv-python wget

import sys
import os
import numpy as np
import datetime
#----------------------------------------------
useGPU = True
if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--cpu"):
             useGPU = False
# Set CUDA_VISIBLE_DEVICES to an empty string to force TensorFlow to use the CPU
if (not useGPU):
     os.environ['CUDA_VISIBLE_DEVICES'] = '' #<- Force CPU
#----------------------------------------------
import cv2
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras.callbacks import TensorBoard
from tensorflow.keras import layers, models
from tensorflow.keras.losses import Loss
from tensorflow.keras.metrics import Metric
from tensorflow.keras.layers import Input, DepthwiseConv2D, Flatten, Dropout, Conv2D, Conv2DTranspose, AvgPool2D, BatchNormalization, ReLU, Reshape, Dense, Add, UpSampling2D, MaxPooling2D
from tensorflow.keras.models import Model
#----------------------------------------------
dataType   = np.uint8
dataTypeTF = tf.uint8

if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--float32"):
             dataType   = np.float32
             dataTypeTF = tf.float32
           if (sys.argv[i]=="--uint8"):
             dataType   = np.uint8
             dataTypeTF = tf.uint8
#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#-------------------------------------------------------------------------------
def retrieveModelOutputDimensions(model):
    output_layer = model.layers[-1]  # Assuming the output layer is the last layer
    output_shape = output_layer.output_shape
    output_size = (output_shape[1],output_shape[2])
    numberOfHeatmaps = output_shape[3]
    print("Number of Heatmaps is ", numberOfHeatmaps)
    print("Output Shape is ", output_size)
    return output_shape[1],output_shape[2],output_shape[3]
#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
def compose(*funcs):
    from functools import reduce
    if funcs:
        return reduce(lambda f, g: lambda *a, **kw: g(f(*a, **kw)), funcs)
    else:
        raise ValueError('Composition of empty sequence not supported.')

class Conv_Bn_Relu6(keras.layers.Layer):
    def __init__(self, filters, kernel_size, strides, padding, name):
        super(Conv_Bn_Relu6, self).__init__()
        self._name = name
        self.block = keras.Sequential()
        if name.find('depthwise') == -1:
            self.block.add(keras.layers.Conv2D(filters, kernel_size, strides, padding=padding))
        else:
            self.block.add(keras.layers.DepthwiseConv2D(kernel_size, strides, padding=padding))
        self.block.add(keras.layers.BatchNormalization())
        if name.find('relu') != -1:
            self.block.add(keras.layers.ReLU(6))
    def call(self, inputs, **kwargs):
        return self.block(inputs)

def block(x, filters, t, strides, name):
    shortcut = x

    x = compose(Conv_Bn_Relu6(t * filters, (1, 1), (1, 1), 'same', name='{}_conv_bn_relu6'.format(name)),
                Conv_Bn_Relu6(None, (3, 3), strides, 'same', name='{}_depthwiseconv_bn_relu6'.format(name)),
                Conv_Bn_Relu6(filters, (1, 1), (1, 1), 'same', name='{}_conv_bn'.format(name)))(x)

    if shortcut.shape[-1] == filters and strides == (1, 1):
        x = keras.layers.Add(name='{}_add'.format(name))([x, shortcut])

    return x

def add_block(x, filters, t, strides, n, name):
    x = block(x, filters, t, strides, name='{}_1'.format(name))
    for i in range(n - 1):
        x = block(x, filters, t, (1, 1), name='{}_{}'.format(name, i + 2))

    return x

#https://ustccoder.github.io/2020/03/22/feature_extraction%20MobileNet_V2/
def create_keypoints_modelNew(inputHeight, inputWidth, inputChannels, outputWidth, outputHeight, numKeypoints, midSectionRepetitions=5, activation='swish'):
    input_shape = (inputHeight, inputWidth, inputChannels)
    input_tensor = Input(shape=input_shape)
    #x = input_tensor

    # Create Rescaling layer
    rescale_layer = tf.keras.layers.experimental.preprocessing.Rescaling(scale=1./255)
    normalized_tensor = rescale_layer(input_tensor)
    x = normalized_tensor

    x = mobilenet_block(x, filters=32, strides=(2, 2), activation=activation, name='block1')
    x = add_block(x, filters=16,  t=1, strides=(1, 1), n=1, name='block2')
    x = add_block(x, filters=24,  t=6, strides=(2, 2), n=2, name='block3')
    x = add_block(x, filters=32,  t=6, strides=(2, 2), n=3, name='block4')
    x = add_block(x, filters=64,  t=6, strides=(2, 2), n=4, name='block5')
    x = add_block(x, filters=96,  t=6, strides=(1, 1), n=3, name='block6')
    x = add_block(x, filters=160, t=6, strides=(2, 2), n=3, name='block7')
    x = add_block(x, filters=320, t=6, strides=(1, 1), n=1, name='block8')

    x = compose(Conv_Bn_Relu6(1280, (1, 1), (1, 1), 'same', name='conv2'),AvgPool2D(pool_size=4, name='global_averagepool'))(x)

    x = Dense(units=int((outputHeight * outputWidth * numKeypoints) // 9), activation=activation, name='dense')(x)

    x = Reshape((numKeypoints, outputHeight, outputWidth), name='reshape')(x)

    model = Model(inputs=input_tensor, outputs=x, name='MobileNet-V2-KeyPoints')
    model.summary()

    return model
#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#-------------------------------------------------------------------------------
def mobilenet_block(x, filters, strides, activation, dropoutRate, name=None):
    x = DepthwiseConv2D(kernel_size = 3, strides = strides, padding = 'same', activation=activation)(x) #, activation=activation
    if (activation=='relu'): #Disabled after switching to swish/selu
      x = BatchNormalization()(x)
      x = ReLU()(x)

    x = Conv2D(filters = filters, kernel_size = 1, strides = 1, activation=activation)(x) # , activation=activation
    if (activation=='relu'): #Disabled after switching to swish/selu
      x = BatchNormalization()(x)
      x = ReLU()(x)

    if dropoutRate>0.0:
        x = Dropout(dropoutRate)(x)

    return x

# Define the CNN model for keypoints prediction
def create_keypoints_model(inputHeight, inputWidth, inputChannels, outputWidth, outputHeight, numKeypoints, midSectionRepetitions=5 ,activation='relu', dropoutRate=0.0, baseChannels = 64):
    input_shape  = (inputHeight, inputWidth, inputChannels)

    input_tensor = Input(shape=input_shape)

    # Create Rescaling layer
    rescale_layer = tf.keras.layers.experimental.preprocessing.Rescaling(scale=1./255)
    normalized_tensor = rescale_layer(input_tensor)

    
    x = mobilenet_block(normalized_tensor, filters=baseChannels, strides=1, dropoutRate=dropoutRate,  activation=activation)
    #x = Dropout(0.2)(x)
    x = mobilenet_block(x, filters=baseChannels*2, strides=2, dropoutRate=dropoutRate, activation=activation)
    #x = Dropout(0.2)(x)
    x = mobilenet_block(x, filters=baseChannels*3, strides=1, dropoutRate=dropoutRate, activation=activation)
    #x = Dropout(0.2)(x)
    x = mobilenet_block(x, filters=baseChannels*4, strides=2, dropoutRate=dropoutRate, activation=activation)
    x = mobilenet_block(x, filters=baseChannels*5, strides=1, dropoutRate=dropoutRate, activation=activation)
    x = mobilenet_block(x, filters=baseChannels*8, strides=2, dropoutRate=dropoutRate, activation=activation)

    for _ in range(midSectionRepetitions):
       x = mobilenet_block(x, filters=baseChannels*16, strides=1, dropoutRate=dropoutRate, activation=activation)

    x = mobilenet_block(x, filters=baseChannels*24, strides=2, dropoutRate=dropoutRate, activation=activation)
    x = mobilenet_block(x, filters=baseChannels*16, strides=1, dropoutRate=dropoutRate, activation=activation)

    x = AvgPool2D(pool_size=4)(x)  # Adjust pool_size based on your desired output size

    x = Conv2DTranspose(filters=baseChannels*12, kernel_size=4, strides=2, padding='same', activation=activation)(x)
    if (activation=='relu'): #Disabled after switching to swish/selu
      x = BatchNormalization()(x)
      x = ReLU()(x)

    x = Conv2DTranspose(filters=baseChannels*10, kernel_size=4, strides=2, padding='same', activation=activation)(x)
    if (activation=='relu'): #Disabled after switching to swish/selu
      x = BatchNormalization()(x)
      x = ReLU()(x)

    x = Conv2DTranspose(filters=baseChannels*8, kernel_size=4, strides=2, padding='same', activation=activation)(x)
    if (activation=='relu'): #Disabled after switching to swish/selu
      x = BatchNormalization()(x)
      x = ReLU()(x)

    x = Conv2DTranspose(filters=baseChannels*3, kernel_size=4, strides=2, padding='same', activation=activation)(x)
    if (activation=='relu'): #Disabled after switching to swish/selu
      x = BatchNormalization()(x)
      x = ReLU()(x)

    x = Conv2DTranspose(filters=baseChannels, kernel_size=4, strides=2, padding='same', activation=activation)(x)
    if (activation=='relu'): #Disabled after switching to swish/selu
      x = BatchNormalization()(x)
      x = ReLU()(x)

    x = Conv2D(filters=numKeypoints, kernel_size=1, strides=1, activation='linear')(x)

    # Adjust the Reshape layer based on the original output size
    x = Reshape((outputHeight, outputWidth,numKeypoints))(x)  # Reshape to the desired size
    model = Model(inputs=input_tensor, outputs=x)
    model.summary()

    return model
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
def Conv2D_BN_Leaky(*args, **kwargs):
    conv_kwargs = {
        'use_bias': True,
        'padding': 'same',
        'kernel_initializer': 'he_normal'
    }
    conv_kwargs.update(kwargs)
    return Conv2D(*args, **conv_kwargs)

def resblock_module(tensor, num_filters):
    skip = Conv2D_BN_Leaky(num_filters, (1, 1))(tensor)

    tensor = Conv2D_BN_Leaky(num_filters//2, (1, 1))(tensor)
    tensor = Conv2D_BN_Leaky(num_filters//2, (3, 3), padding='same')(tensor)
    tensor = Conv2D_BN_Leaky(num_filters, (1, 1))(tensor)
    tensor = Add()([skip, tensor])

    return tensor

def hourglass_module(input_tensor, stage):
    stage -= 1
    skip = resblock_module(input_tensor, 256)
    tensor = MaxPooling2D(pool_size=(2, 2), strides=(2, 2))(input_tensor)
    tensor = resblock_module(tensor, 256)
    if stage == 0:
        tensor = resblock_module(tensor, 256)
    else:
        tensor = hourglass_module(tensor, stage)
    tensor = resblock_module(tensor, 256)
    tensor = UpSampling2D(2)(tensor)
    tensor = Add()([skip, tensor])

    return tensor

def front_module(input_tensor, num_filters=256):
    tensor = Conv2D_BN_Leaky(num_filters//4, (7, 7), (2, 2), padding='same')(input_tensor)
    tensor = resblock_module(tensor, num_filters//2)
    tensor = MaxPooling2D(pool_size=(2, 2), strides=(2, 2))(tensor)
    tensor = resblock_module(tensor, num_filters//2)
    tensor = resblock_module(tensor, num_filters)

    return tensor

def stack_module(input_tensor, num_points, num_filters=256, stage=4, activation="sigmoid", is_head=False):
    tensor = hourglass_module(input_tensor, stage)
    tensor = resblock_module(tensor, num_filters)
    tensor = Conv2D_BN_Leaky(num_filters, (1, 1))(tensor)
    outputs = Conv2D(num_points, (1, 1))(tensor)
    if activation == "sigmoid":
        outputs = tf.keras.layers.Activation("sigmoid")(outputs)
    elif activation == "softmax":
        outputs = tf.keras.layers.Softmax(axis=-1)(outputs)

    if is_head:
        return outputs
    else:
        tensor = Conv2D(num_filters, (1, 1))(tensor)
        skip = Conv2D(num_filters, (1, 1))(outputs)
        tensor = Add()([skip, tensor])
        return outputs, tensor

def create_hourglass_keypoints_model(input_shape, num_stacks=8, num_points=15, num_filters=256, stage=4, activation="sigmoid", pretrained_weights=None):
    output_list = []
    inputs = Input(input_shape)
    tensor = front_module(inputs, num_filters=num_filters)

    for _ in range(num_stacks - 1):
        skip = tensor
        outputs, tensor = stack_module(
            tensor, num_points,
            num_filters=num_filters,
            stage=stage,
            activation=activation)
        tensor = Add()([skip, tensor])
        output_list.append(outputs)

    outputs = stack_module(
        tensor, num_points,
        num_filters=num_filters,
        stage=stage,
        activation=activation,
        is_head=True)
    output_list.append(outputs)

    model = tf.keras.models.Model(inputs, output_list)

    model.summary()

    x,y,num = retrieveModelOutputDimensions(model)

    return model
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
def logTrainingParameters(cfg, log_dir):
    try:
      # Create a summary writer
      param_log = tf.summary.create_file_writer(log_dir)

      with param_log.as_default():
        # Convert the dictionary to a formatted string
        params_str = "\n".join([f"{key}: {value}" for key, value in cfg.items()])
        # Log the parameters as text
        tf.summary.text("Training Parameters", params_str, step=0)
    except Exception as e:
        print(f"Error storing logging parameters in tensorboard: {e}")
#-------------------------------------------------------------------------------
def logSomeInputsAndOutputs(inputs, outputs, labels, log_dir, samples=100):
    try:
      # Create a summary writer
      image_log = tf.summary.create_file_writer(log_dir)

      with image_log.as_default():

        sample_indices = np.random.choice(len(inputs), size=min(samples,len(inputs)), replace=False)

        for logID in sample_indices:
            #Store the image as float32 [0..1] RGB to make sure tensorboard visualizes it correctly
            image_as_float = inputs[logID].astype(np.float32)
            image = image_as_float / 255.0

            # Convert input and output arrays to TensorFlow tensors
            bgr_image_tensor = tf.convert_to_tensor([image], dtype=tf.float32)

            # Swap BGR to RGB
            rgb_image_tensor = tf.reverse(bgr_image_tensor, axis=[-1])

            # Write input image summary
            tf.summary.image(f"Image {logID} Input", rgb_image_tensor , step=logID)

            # Write output image summary
            heatmapID = 0
            for heatmapID in range(0,18):
               heatmap = outputs[logID,:,:,heatmapID]
               #print(f"Heatmap {heatmapID} dimensions: {heatmap.shape}")
               # Add batch and channel dimensions
               heatmapS            = np.squeeze(heatmap)
               heatmapS            = np.expand_dims(heatmapS, axis=-1)
               heatmap_as_float    = heatmapS.astype(np.float32)
               output_image_tensor = tf.convert_to_tensor([heatmap_as_float], dtype=tf.float32)

               thisOutputlabel = "#%u" % heatmapID
               if (heatmapID < len(labels)):
                    thisOutputlabel = labels[heatmapID]

               tf.summary.image(f"Image {logID} Output / {thisOutputlabel}", output_image_tensor, step=logID)
               heatmapID = heatmapID + 1
    except Exception as e:
        print(f"Error storing image in tensorboard: {e}")
        sys.exit(1)
#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#-------------------------------------------------------------------------------
def checkIfFileExists(filename):
    return os.path.isfile(filename)
#-------------------------------------------------------------------------------
def convert_bytes(num):
    #This function will convert bytes to MB.... GB... strings
    step_unit = 1000.0 #1024 bad the size
    for x in ['bytes', 'KB', 'MB', 'GB', 'TB']:
        if num < step_unit:
            return "%3.1f %s" % (num, x)
        num /= step_unit
#-------------------------------------------------------------------------------
def printTFVersion():
       global useGPU
       print("")
       print("Tensorflow version : ",tf.__version__)
       #print("Keras version      : ",keras.__version__) <- no longer available in TF-2.13
       print("Numpy version      : ",np.__version__)
       #-----------------------------
       from tensorflow.python.platform import build_info as tf_build_info
       print("TF/CUDA version    : ",tf_build_info.build_info['cuda_version'])
       print("TF/CUDNN version   : ",tf_build_info.build_info['cudnn_version'])
       print("Use GPU            : ",useGPU)
       #-----------------------------
       if useGPU:
         physical_devices = tf.config.list_physical_devices('GPU')
         if physical_devices:
            gpuID = 0
            for gpu in physical_devices:
                print("GPU #",gpuID," Name:", gpu.name)
                try:
                    # Note: The following code may not be available in older versions of TensorFlow
                    memory_info = tf.config.experimental.get_memory_info('GPU:%u'%gpuID)
                    print("GPU #",gpuID," Memory Currently Used (in MB):", memory_info['current'] / (1024**2))
                    print("GPU #",gpuID," Memory Peak Used (in MB):", memory_info['peak'] / (1024**2))
                except Exception as e:
                    print(f"Error getting memory info for GPU #{gpuID}: {e}")
                gpuID += 1
         else:
            print("No GPU available.")
       print("")
       #-----------------------------
#-------------------------------------------------------------------------------
class RSquaredMetric(Metric):
    def __init__(self, name='r_squared', **kwargs):
        super(RSquaredMetric, self).__init__(name=name, **kwargs)
        self.ssr = self.add_weight(name='ssr', initializer='zeros')
        self.sst = self.add_weight(name='sst', initializer='zeros')

    def update_state(self, y_true, y_pred, sample_weight=None):
        # Explicitly cast y_true to float32
        y_true = tf.cast(y_true, tf.float32)
        y_pred = tf.cast(y_pred, tf.float32)

        # Reshape y_true and y_pred to ensure they're 1D tensors
        y_true = tf.reshape(y_true, [-1])
        y_pred = tf.reshape(y_pred, [-1])

        # Calculate the sum of squares of residuals
        ssr_update = tf.reduce_sum(tf.square(y_true - y_pred))
        self.ssr.assign_add(ssr_update)

        # Calculate the total sum of squares
        mean_y_true = tf.reduce_mean(y_true)
        sst_update = tf.reduce_sum(tf.square(y_true - mean_y_true))
        self.sst.assign_add(sst_update)

    def result(self):
        return 1 - (self.ssr / self.sst)

    def reset_state(self):
        self.ssr.assign(0.0)
        self.sst.assign(0.0)
#-------------------------------------------------------------------------------
class PCKMetric(Metric):
    def __init__(self, name='pck', threshold=0.01, **kwargs):
        super(PCKMetric, self).__init__(name=name, **kwargs)
        self.threshold = threshold
        self.total_correct_keypoints = self.add_weight(name='total_correct_keypoints', initializer='zeros')
        self.total_keypoints         = self.add_weight(name='total_keypoints', initializer='zeros')

    def update_state(self, y_true, y_pred, sample_weight=None):
        # Explicitly cast y_true to float32
        y_true = tf.cast(y_true, tf.float32)
        y_pred = tf.cast(y_pred, tf.float32)

        # Assuming y_true and y_pred have shape (batch_size, num_keypoints*2)
        batch_size = tf.shape(y_true)[0]

        # Reshape to (batch_size, num_keypoints, 2)
        y_true = tf.reshape(y_true, (batch_size, -1, 2))
        y_pred = tf.reshape(y_pred, (batch_size, -1, 2))

        # Calculate Euclidean distances between true and predicted keypoints
        distances = tf.norm(y_true - y_pred, axis=-1)

        # Count correct keypoints within the threshold
        correct_keypoints = tf.cast(tf.reduce_sum(tf.cast(distances <= self.threshold, tf.float32)), tf.float32)

        # Update total correct keypoints and total keypoints
        self.total_correct_keypoints.assign_add(correct_keypoints)
        self.total_keypoints.assign_add(tf.cast(tf.reduce_sum(tf.ones_like(distances)), tf.float32))

    def result(self):
        # Calculate the percentage of correct keypoints
        return self.total_correct_keypoints / self.total_keypoints if self.total_keypoints > 0 else 0.0

    def reset_state(self):
        # Reset counts at the start of each epoch or batch
        self.total_correct_keypoints.assign(0.0)
        self.total_keypoints.assign(0.0)
#-------------------------------------------------------------------------------
# Define Focal Loss
# categorical_focal_loss and binary_focal_loss
# https://github.com/aldi-dimara/keras-focal-loss/blob/master/focal_loss.py
class FocalLoss(Loss):
    def __init__(self, alpha=0.25, gamma=2.0, num_joints=17, **kwargs):
        super(FocalLoss, self).__init__(**kwargs)
        self.alpha = alpha
        self.gamma = gamma
        self.num_joints = num_joints

    def call(self, y_true, y_pred):
        #batch_size = tf.shape(y_true)[0]
        y_true = tf.cast(y_true, tf.float32)# / 255.0
        y_pred = tf.cast(y_pred, tf.float32)# / 255.0

        # Split the y_true into joint heatmaps and background heatmap
        y_true_joints       = y_true[:, :, :, :self.num_joints]
        y_true_background   = y_true[:, :, :, self.num_joints:]

        # Split the y_pred into joint predictions and background prediction
        y_pred_joints       = y_pred[:, :, :, :self.num_joints]
        y_pred_background   = y_pred[:, :, :, self.num_joints:]

        # Calculate focal loss for joint heatmaps
        joint_pos_mask      = tf.cast(y_true_joints > 0, dtype=tf.float32)
        joint_neg_mask      = tf.cast(y_true_joints == 0, dtype=tf.float32)

        alpha_factor_joint  = self.alpha * joint_pos_mask + (1 - self.alpha) * joint_neg_mask
        focal_weight_joint  = alpha_factor_joint * tf.pow(1 - y_pred_joints, self.gamma)

        joint_pos_loss      = focal_weight_joint * tf.square(y_pred_joints - y_true_joints)
        joint_neg_loss      = (1 - focal_weight_joint) * tf.square(y_pred_joints)

        num_joint_pos       = tf.reduce_sum(joint_pos_mask)
        joint_pos_loss      = tf.reduce_sum(joint_pos_loss)
        joint_neg_loss      = tf.reduce_sum(joint_neg_loss)

        joint_loss          = tf.cond(tf.greater(num_joint_pos, 0), lambda: (joint_pos_loss + joint_neg_loss) / num_joint_pos, lambda: joint_neg_loss)

        # Calculate focal loss for the background heatmap
        background_pos_mask = tf.cast(y_true_background > 0, dtype=tf.float32)
        background_neg_mask = tf.cast(y_true_background == 0, dtype=tf.float32)

        alpha_factor_background = self.alpha * background_pos_mask + (1 - self.alpha) * background_neg_mask
        focal_weight_background = alpha_factor_background * tf.pow(1 - y_pred_background, self.gamma)

        background_pos_loss = focal_weight_background * tf.square(y_pred_background - y_true_background)
        background_neg_loss = (1 - focal_weight_background) * tf.square(y_pred_background)

        num_background_pos  = tf.reduce_sum(background_pos_mask)
        background_pos_loss = tf.reduce_sum(background_pos_loss)
        background_neg_loss = tf.reduce_sum(background_neg_loss)

        background_loss = tf.cond(tf.greater(num_background_pos, 0), lambda: (background_pos_loss + background_neg_loss) / num_background_pos, lambda: background_neg_loss)

        # Combine joint and background losses
        total_loss = joint_loss + background_loss

        return total_loss
# Define your custom loss function
def focal_loss(y_true, y_pred):
    return FocalLoss()(y_true, y_pred)
#-------------------------------------------------------------------------------
#Define some more losses : https://github.com/stefanopini/simple-HRNet/blob/master/losses/loss.py#L58
#https://github.com/microsoft/human-pose-estimation.pytorch/blob/master/lib/core/loss.py
class JointsMSELoss(Loss):
    def __init__(self, num_joints=17, weight_factor=10.0, **kwargs):
        super(JointsMSELoss, self).__init__(**kwargs)
        self.num_joints = num_joints
        self.weight_factor = weight_factor
        self.criterion = tf.keras.losses.MeanSquaredError()

    def call(self, y_true, y_pred):
        y_true = tf.cast(y_true, tf.float32)# / 255.0
        y_pred = tf.cast(y_pred, tf.float32)# / 255.0

        # Split the y_true and y_pred into joint heatmaps and background heatmap
        y_true_joints     = y_true[:, :, :, :self.num_joints]
        y_true_background = y_true[:, :, :, self.num_joints:]

        y_pred_joints     = y_pred[:, :, :, :self.num_joints]
        y_pred_background = y_pred[:, :, :, self.num_joints:]

        # Compute loss for joint heatmaps
        joint_loss = 0.0
        for idx in range(self.num_joints):
            heatmap_pred = y_pred_joints[:, :, :, idx]
            heatmap_gt   = y_true_joints[:, :, :, idx]

            # Calculate the weight for this joint
            weight = tf.reduce_mean(heatmap_gt) * self.weight_factor + 1.0

            joint_loss += 0.5 * self.criterion(heatmap_gt, heatmap_pred) * weight

        joint_loss /= tf.cast(self.num_joints, dtype=tf.float32)

        # Compute loss for the background heatmap
        background_loss = 0.5 * self.criterion(y_true_background, y_pred_background)

        # Combine joint and background losses
        total_loss = joint_loss + background_loss

        return total_loss
# Define your custom loss function
def jointsMSE_loss(y_true, y_pred):
    return JointsMSELoss()(y_true, y_pred)
#-------------------------------------------------------------------------------
class VanillaMSELoss(tf.keras.losses.Loss):
    def __init__(self, **kwargs):
        super(VanillaMSELoss, self).__init__(**kwargs)

    def call(self, y_true, y_pred):
        # Ensure both y_true and y_pred are cast to float32
        y_true = tf.cast(y_true, tf.float32)
        y_pred = tf.cast(y_pred, tf.float32)

        # Compute the squared difference
        squared_difference = tf.square(y_true - y_pred)

        # Compute the mean over all elements
        mse_loss = tf.reduce_mean(squared_difference)

        return mse_loss

# Define your custom loss function
def vanilla_mse_loss(y_true, y_pred):
    return VanillaMSELoss()(y_true, y_pred)
#-------------------------------------------------------------------------------
class WeightedMSELoss(tf.keras.losses.Loss):
    def __init__(self, last_heatmap_weight=2.0, **kwargs):
        super(WeightedMSELoss, self).__init__(**kwargs)
        self.last_heatmap_weight = last_heatmap_weight

    def call(self, y_true, y_pred):
        # Ensure both y_true and y_pred are cast to float32
        y_true = tf.cast(y_true, tf.float32)
        y_pred = tf.cast(y_pred, tf.float32)

        # Compute the squared difference
        squared_difference = tf.square(y_true - y_pred)

        # Apply weighting to the last heatmap
        last_heatmap_weighted = squared_difference[..., -1] * self.last_heatmap_weight
        squared_difference = tf.concat([squared_difference[..., :-1], tf.expand_dims(last_heatmap_weighted, axis=-1)], axis=-1)

        # Compute the mean over all elements
        mse_loss = tf.reduce_mean(squared_difference)

        return mse_loss

# Define your custom loss function
def weighted_mse_loss(y_true, y_pred):
    return WeightedMSELoss(last_heatmap_weight=2.0)(y_true, y_pred)
#-------------------------------------------------------------------------------
def flip_data(inputs_tf, outputs_tf, flip_x=True, flip_y=True):
    print("Flipping data X:",flip_x," Y:",flip_y)
    # Flip along the X dimension if requested
    if flip_x:
        inputs_tf  = tf.image.flip_left_right(inputs_tf)
        outputs_tf = tf.image.flip_left_right(outputs_tf)

    # Flip along the Y dimension if requested
    if flip_y:
        inputs_tf  = tf.image.flip_up_down(inputs_tf)
        outputs_tf = tf.image.flip_up_down(outputs_tf)

    return inputs_tf, outputs_tf
#-------------------------------------------------------------------------------
def read_json_file(file_path):
    import json
    try:
        with open(file_path, 'r') as file:
            data = json.load(file)
            return data
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON format in file '{file_path}'.")
    except Exception as e:
        print(f"Error: {e}")
#-------------------------------------------------------------------------------
def download_image(url, save_path):
    import wget
    try:
        # Check if the file already exists at the save_path
        if os.path.exists(save_path):
            #print(f"File already exists at {save_path}. Skipping download.")
            return True
        else:
            # Download the image using wget
            wget.download(url, save_path)
            print(f"\nImage downloaded successfully to {save_path}")
    except Exception as e:
        print(f"Error downloading image: {e}")
    return False
#-------------------------------------------------------------------------------
def resize_image_with_borders(image, target_size=(300, 300)):
    try:
        # Get the original image size
        originalWidth  = image.shape[1]
        originalHeight = image.shape[0]
        #Notice that we use a different convention than OpenCV
        newWidth       = target_size[0]
        newHeight      = target_size[1]

        # Calculate the aspect ratios of the original and target sizes
        aspect_ratio_original = originalWidth  / originalHeight
        aspect_ratio_target   = newWidth / newHeight

        # Determine the resizing factor and size for maintaining the aspect ratio
        if aspect_ratio_original > aspect_ratio_target:
            newHeight = int(newWidth / aspect_ratio_original)
        else:
            newWidth  = int(newHeight * aspect_ratio_original)

        # Resize the image while maintaining the aspect ratio
        resized_image = cv2.resize(image, (newWidth, newHeight))

        # Create a new image with a black background
        new_image = np.zeros((target_size[1], target_size[0], 3), dtype=dataType)

        # Calculate the position to paste the resized image onto the new image
        x_offset = (target_size[0] - newWidth)  // 2
        y_offset = (target_size[1] - newHeight) // 2

        # Paste the resized image onto the new image
        new_image[y_offset:y_offset + newHeight, x_offset:x_offset + newWidth] = resized_image

        keypointXMultiplier =  newWidth  / originalWidth
        keypointYMultiplier =  newHeight / originalHeight
        keypointXOffset     =  x_offset
        keypointYOffset     =  y_offset

        return new_image, keypointXMultiplier, keypointYMultiplier, keypointXOffset, keypointYOffset

    except Exception as e:
        print(f"Error resizing image: {e}")
        return image, 0.0, 0.0, 0.0, 0.0
#-------------------------------------------------------------------------------
def emptyImage(labels=None, target_size=(300, 300), output_target_size=(64, 64), heatmapActive=0, heatmapDeactivated=255):
    # Create an empty black image
    image = np.zeros((target_size[1], target_size[0], 3), dtype=dataType)

    # Fill the image with randomized grayscale values
    image[:, :] = np.ones((target_size[1], target_size[0],3), dtype=dataType) * 255
    scale = np.random.rand()
    image = image * (scale)

    heatmap = np.full((output_target_size[1], output_target_size[0], len(labels)+1), heatmapDeactivated, dtype=dataType)
    heatmap[:,:,-1] = heatmapActive

    return image, heatmap
#-------------------------------------------------------------------------------
def syntheticImage(labels=None, target_size=(300, 300), output_target_size=(64, 64), heatmapActive=0, heatmapDeactivated=255):
    # Create an empty black image
    image = np.zeros((target_size[1], target_size[0], 3), dtype=dataType)

    # Fill the image with randomized grayscale values
    image[:, :] = np.random.rand(target_size[1], target_size[0], 3) * 255

    heatmap = np.full((output_target_size[1], output_target_size[0], len(labels)+1), heatmapDeactivated, dtype=dataType)
    heatmap[:,:,-1] = heatmapActive

    return image, heatmap
#-------------------------------------------------------------------------------
def add_gaussian_noise(image, maxValue=1.0, magnitude=0.01):
    # Generate Gaussian noise with the same shape as the input image
    noise = np.random.normal(scale=magnitude, size=image.shape)

    # Add noise to the image
    corrupted_image = image + noise

    # Clip values to be within [0, 1]
    corrupted_image = np.clip(corrupted_image, 0, maxValue)

    return corrupted_image
#-------------------------------------------------------------------------------
def generate_one_hot_images(keypointsList,labels, target_size=(64, 64), heatmapActive=0, heatmapDeactivated=255, simple=True):
    num_labels       = len(labels)
    numberOfHeatmaps = num_labels + 1 #Labels + Bkg label
    heatmaps         = np.full((target_size[1], target_size[0], numberOfHeatmaps), heatmapDeactivated, dtype=dataType)
    heatmaps[:,:,-1] = heatmapActive
 
    """
    # Define the kernel outside the loop
    kernel = np.array([[0.2, 0.4, 0.2],
                       [0.4, 1.0, 0.4],
                       [0.2, 0.4, 0.2]]) * heatmapActive
    # Normalize the kernel to ensure that the total intensity remains the same
    kernel /= np.sum(kernel) #Don;t normalize to boost gradient
    """

    for i in range(num_labels):
      #For each joint Label for each keypoint list
      for keypoints in keypointsList:
        x = int(keypoints[i*3+0]*target_size[0])
        y = int(keypoints[i*3+1]*target_size[1])
        v = keypoints[i*3+2]
        if (x!=0) and (y!=0) and (v!=0):
          #if (simple):
          heatmaps[y,x,i]  = heatmapActive #1 hot
          heatmaps[y,x,-1] = heatmapDeactivated
          """
          else:
            #Complex pattern for bigger training targets..
            if (y <= 1 or y >= target_size[1]-1 ) or (x <= 1 or x >= target_size[0]-1):
                # If at the border, set the center pixel to v without interpolation
                heatmap[y,x,i]        = heatmapMagnitude
                union_heatmap[y,x,-1] = 0
            else:
                heatmap[y-1:y+2, x-1:x+2,i]        += kernel #more spread..
                union_heatmap[y-1:y+2, x-1:x+2,-1] -= kernel
          """
      #-----------------------------------------------------------------------------

    #add union heatmap making sure it is in range
    #heatmaps = np.clip(heatmaps, 0, heatmapMagnitude)
    return heatmaps
#-------------------------------------------------------------------------------
def processImage(inputDataset,outputDataset,sampleNumber,image_path, keypointsList=list(), labels=list(), target_size=(300, 300),  output_target_size=(64,64), augment=False, heatmapActive=0, heatmapDeactivated=255):
        # Step 1: Read the image using OpenCV
        image_raw  = cv2.imread(image_path)
        # Step 2: Convert the image to float32
        image = image_raw.astype(dataType)

        #if (augment):
        #    image       = add_gaussian_noise(image, maxValue=1.0, magnitude=0.01) # <- This is not working well..

        originalWidth  = image.shape[1]
        originalHeight = image.shape[0]
        keypointXMultiplier = 1.0
        keypointYMultiplier = 1.0
        keypointXOffset     = 0.0
        keypointYOffset     = 0.0

        image, keypointXMultiplier, keypointYMultiplier, keypointXOffset, keypointYOffset = resize_image_with_borders(image,target_size)
        inputDataset[sampleNumber,:,:,:] = image
        resizedWidth  = image.shape[1]
        resizedHeight = image.shape[0]

        for keypoints in keypointsList:
            numberOfJoints = len(keypoints)
            for i in range(0,int(numberOfJoints/3)):
                    x = (keypointXMultiplier * keypoints[i*3+0]) + keypointXOffset
                    y = (keypointYMultiplier * keypoints[i*3+1]) + keypointYOffset
                    v = keypoints[i*3+2]
                    if v > 0:  # Only correct visible keypoints
                        #Correct keypoints
                        keypoints[i*3+0] = x / target_size[0]
                        keypoints[i*3+1] = y / target_size[1]

        # Calculate the heatmaps
        oneHotImages = generate_one_hot_images(keypointsList, labels, target_size=output_target_size, heatmapActive=heatmapActive, heatmapDeactivated=heatmapDeactivated)
        outputDataset[sampleNumber,:,:,:] = oneHotImages

        return inputDataset , outputDataset
#-------------------------------------------------------------------------------
def compileJSONAssociations(json_data):
 print("Compiling annotation->image associations")
 annotations={}
 for annotation in json_data['annotations']:
     image_id = annotation['image_id']
     if image_id not in annotations:
        annotations[image_id] = []
     annotations[image_id].append(annotation)
 return annotations
#-------------------------------------------------------------------------------
def createTrainingSetFromJSONFile(jsonPath,
                                  database,
                                  cache_directory       = 'cache/data/coco/val2017/',
                                  start_index           = 0,
                                  end_index             = None,
                                  target_size           = (200,200),
                                  output_target_size    = (120,120),
                                  RGBMagnitude          = 255,
                                  heatmapActive         = 0,
                                  heatmapDeactivated    = 255,
                                  mem                   = 1.0,
                                  visualize             = False,
                                  preloadedJSON         = None,
                                  preloadedAssociations = None
                                 ):
  # Call the function to read the JSON file
  if (not preloadedJSON):
     json_data = read_json_file(jsonPath)
     print("JSON has ",len(json_data['annotations'])," annotations")
     print("JSON has ",len(json_data['images'])," images")
  else:
     json_data = preloadedJSON

  augment = False
  if ("train" in jsonPath) and (not preloadedJSON):
       augment = True

  msgTick = 0
  labels  = json_data['categories'][0]['keypoints']

  # Check if the file was successfully read
  if json_data:
    if not os.path.exists(cache_directory):
        # Create the 'cache' directory if it doesn't exist to allow downloading to work
        os.makedirs(cache_directory)
        print(f"Directory '{cache_directory}' created.")

    # Create a dictionary to organize annotations by image ID and optimize reading if not already populated
    if (preloadedAssociations):
       annotations_of_image = preloadedAssociations
    else:
       annotations_of_image = compileJSONAssociations(json_data)

    #Decide on the number of entries to read
    end_index = end_index or len(json_data['images'])
    if ("train" in jsonPath) and (mem!=1.0):
        end_index = start_index + int(mem)

    #Allocate all of the I/O tensors in a contiguous memory block in a single step
    inputDataset  = np.zeros((end_index - start_index,*target_size,3), dtype=dataType)
    outputDataset = np.zeros((end_index - start_index,*output_target_size,len(labels)+1), dtype=dataType)

    for imageID in range(start_index,end_index):
        source      = "%s/%s" % (database,json_data['images'][imageID]["file_name"])
        cacheTarget = "%s/%s" % (cache_directory,json_data['images'][imageID]["file_name"])
        download_image(source,cacheTarget)

        width       =  int(json_data['images'][imageID]["width"])
        height      =  int(json_data['images'][imageID]["height"])
        ID          =  int(json_data['images'][imageID]["id"])

        keypointsList = list()
        numberOfSkeletons = 0

        if ID in annotations_of_image:
          annotations = annotations_of_image[ID]
          for annotation in annotations:
                    keypointsList.append(annotation["keypoints"])
                    numberOfSkeletons = numberOfSkeletons + 1
                    #break #<- Single person

        if (msgTick%31==0) and not preloadedJSON :
          print(f"\r Image ID: {imageID} /",len(json_data['images']) , end=" ")
          print(f"Width: {width}, Height: {height}, Skeletons: {numberOfSkeletons} \r",end=" ")
        msgTick = msgTick + 1

        inputDataset,outputDataset = processImage(
                                                  inputDataset,
                                                  outputDataset,
                                                  imageID,
                                                  cacheTarget,
                                                  keypointsList,
                                                  labels=labels,
                                                  target_size=target_size,
                                                  output_target_size = output_target_size,                                                   
                                                  heatmapActive      = heatmapActive,
                                                  heatmapDeactivated = heatmapDeactivated,
                                                  augment=augment
                                                 )
    """
    if (augment) and not preloadedJSON:
      #Add random noise Images..
      for syntheticID in range(100):
        thisInput,thisOutputs = syntheticImage(labels=json_data['categories'][0]['keypoints'],
                                               target_size=target_size,
                                               output_target_size=output_target_size,
                                               heatmapMagnitude=heatmapMagnitude
                                              )
        inputs.append(thisInput)
        outputs.append(thisOutputs)

      #Add random empty Images..
      for emptyID in range(100):
        thisInput,thisOutputs = emptyImage(labels=json_data['categories'][0]['keypoints'],
                                           target_size=target_size,
                                           output_target_size=output_target_size,
                                           heatmapMagnitude=heatmapMagnitude
                                          )
        inputs.append(thisInput)
        outputs.append(thisOutputs)
     """
    return inputDataset,outputDataset,labels
  return inputs,outputs,labels
#============================================================================================
#============================================================================================
#============================================================================================
#============================================================================================
#============================================================================================
class TrainingDataGenerator(tf.keras.utils.Sequence):
    def __init__(self, json_path, database, cache_directory, target_size, output_target_size, RGBMagnitude, heatmapActive, heatmapDeactivated, mem, visualize, batch_size):
        self.json_path          = json_path
        self.json_data          = read_json_file(json_path)
        self.labels             = self.json_data['categories'][0]['keypoints']
        self.associations       = compileJSONAssociations(self.json_data)
        self.database           = database
        self.cache_directory    = cache_directory
        self.target_size        = target_size
        self.output_target_size = output_target_size
        self.RGBMagnitude       = RGBMagnitude
        self.heatmapActive      = heatmapActive,
        self.heatmapDeactivated = heatmapDeactivated,
        self.mem                = mem
        self.visualize          = visualize
        self.batch_size         = batch_size

        # Load JSON data
        self.json_data = read_json_file(json_path)
        self.indices   = np.arange(len(self.json_data['images']))
        np.random.shuffle(self.indices)

    def __len__(self):
        return len(self.indices) // self.batch_size

    def __getitem__(self, index):
        start_index   = index * self.batch_size
        end_index     = min(start_index + self.batch_size, len(self.indices))
        batch_indices = self.indices[start_index:end_index]

        xs = np.zeros((len(batch_indices), self.target_size[1],  self.target_size[0], 3), dtype=dataType)
        ys = np.zeros((len(batch_indices), len(self.labels) + 1, self.output_target_size[1], self.output_target_size[0]), dtype=dataType)

        for i, imageID in enumerate(batch_indices):
            source      = f"{self.database}/{self.json_data['images'][imageID]['file_name']}"
            cacheTarget = f"{self.cache_directory}/{self.json_data['images'][imageID]['file_name']}"

            inputs, outputs, labels = createTrainingSetFromJSONFile(
                                                                    self.json_path,
                                                                    self.database,
                                                                    self.cache_directory,
                                                                    target_size           = self.target_size,
                                                                    output_target_size    = self.output_target_size,
                                                                    RGBMagnitude          = self.RGBMagnitude,
                                                                    heatmapActive         = self.heatmapActive,
                                                                    heatmapDeactivated    = self.heatmapDeactivated,
                                                                    mem                   = self.mem,
                                                                    visualize             = self.visualize,
                                                                    start_index           = imageID,
                                                                    end_index             = imageID + 1,
                                                                    preloadedJSON         = self.json_data,
                                                                    preloadedAssociations = self.associations
                                                                  )
            xs[i] = inputs[0]
            ys[i] = outputs[0]

        return xs, ys

def streamDataset(json_path,
                  database,
                  cache_directory     = 'cache/data/coco/val2017/',
                  target_size         = (200, 200),
                  output_target_size  = (64, 64),
                  RGBMagnitude        = 255,
                  heatmapActive       = 0,
                  heatmapDeactivated  = 255,
                  mem                 = 1.0,
                  visualize           = False,
                  batch_size          = 48,
                  shuffle_buffer_size = 100
                 ):
    json_data     = read_json_file(json_path)
    labels        = json_data['categories'][0]['keypoints']
    datasetLength = len(json_data['images'])

    dataset_generator = TrainingDataGenerator(json_path, database, cache_directory, target_size, output_target_size, RGBMagnitude, heatmapActive, heatmapDeactivated, mem, visualize, batch_size)

    dataset = tf.data.Dataset.from_generator(
                                             lambda: iter(dataset_generator),
                                             output_signature=(
                                                                tf.TensorSpec(shape=(batch_size, target_size[1], target_size[0], 3), dtype=dataTypeTF),
                                                                tf.TensorSpec(shape=(batch_size, len(labels) + 1, output_target_size[1], output_target_size[0]), dtype=dataTypeTF)
                                                              )
                                            )

    dataset = dataset.shuffle(buffer_size=shuffle_buffer_size)
    dataset = dataset.repeat()  # Add repeat to make the dataset repeat indefinitely
    dataset = dataset.prefetch(buffer_size=tf.data.AUTOTUNE)

    return dataset, datasetLength, labels
#============================================================================================
#============================================================================================
# Main Function
if __name__ == '__main__':
   cfg = {
           'COCOTrainingJSONPath':'cache/data/annotations/person_keypoints_train2017.json',
           'COCOTrainingURI':'http://ammar.gr/COCO_HumanPose/data/coco/train2017',
           'COCOTrainingLocalCache':'cache/data/coco/train2017/',

           'COCOValidationJSONPath':'cache/data/annotations/person_keypoints_val2017.json',
           'COCOValidationURI':'http://ammar.gr/COCO_HumanPose/data/coco/val2017',
           'COCOValidationLocalCache':'cache/data/coco/val2017/',

           'inputWidth'  :220, #140
           'inputHeight' :220, #140
           'outputWidth' :96,
           'outputHeight':96,

           'dropoutRate':0.1,
           'midSectionRepetitions':5,
           'activation':'relu',
           'baseChannels' : 78,

           'RGBMagnitude': 255,
           'heatmapActive': 0,
           'heatmapDeactivated': 255,
           'streamDataset': False,
           'streamBufferLength': 100,

           'earlyStoppingPatience':3,
           'earlyStoppingMinDelta':0.0001,
           'datasetUsage':1.0,
           'learningRate':0.0005,
           'batchSize':24,
           'epochs':54, #54
           'pCK_AP_Threshold':0.05,
           'loss':'mse'
         }

   if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--mem"):
              cfg['datasetUsage']=float(sys.argv[i+1])
           if (sys.argv[i]=="--stream"):
              cfg['streamDataset'] = True
           if (sys.argv[i]=="--clear"):
              os.system("rm -rf 2d_pose_estimation/tensorboard")
              os.system("rm 2d_pose_estimation.zip")
           if (sys.argv[i]=="--test"):
                 model = create_keypoints_model(
                                                cfg['inputHeight'],
                                                cfg['inputWidth'],
                                                3,
                                                cfg['outputWidth'],
                                                cfg['outputHeight'],
                                                17+1,
                                                midSectionRepetitions = cfg['midSectionRepetitions'],
                                                activation   = cfg['activation'],
                                                baseChannels = cfg['baseChannels'],
                                                dropoutRate  = cfg['dropoutRate']
                                               )
                 #hourglass_model = create_hourglass_keypoints_model( input_shape=(cfg['inputHeight'],cfg['inputWidth'],3), num_points=17+1)
                 sys.exit(0)

   # Set up TensorBoard logging
   log_dir = "2d_pose_estimation/tensorboard/" + datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
   tensorboard_callback = TensorBoard(log_dir=log_dir, histogram_freq=1)


   #First of all create the Neural Network model
   #Don't load other data in vain if this step fails due to bad configuration..
   #model = create_hourglass_keypoints_model( input_shape=(cfg['inputHeight'],cfg['inputWidth'],3), num_points=18)
   model = create_keypoints_model(
                                  cfg['inputHeight'],
                                  cfg['inputWidth'],
                                  3,
                                  cfg['outputWidth'],
                                  cfg['outputHeight'],
                                  18,
                                  midSectionRepetitions = cfg['midSectionRepetitions'],
                                  activation   = cfg['activation'],
                                  baseChannels = cfg['baseChannels'],
                                  dropoutRate  = cfg['dropoutRate']
                                 )
   cfg['outputWidth'],cfg['outputHeight'],numHeatmaps = retrieveModelOutputDimensions(model)

   if (cfg['streamDataset']):
           mem=1.0 #When streaming use everything..

   #Validation data is not so big and is loaded first in memory..
   onlyTrainingData = True 
   if (checkIfFileExists(cfg['COCOValidationJSONPath'])):
     onlyTrainingData = False
     rawValInputs,rawValOutputs,outValLabels = createTrainingSetFromJSONFile(
                                                                             cfg['COCOValidationJSONPath'],
                                                                             cfg['COCOValidationURI'],
                                                                             cfg['COCOValidationLocalCache'],
                                                                             target_size=(cfg['inputHeight'],cfg['inputWidth']),
                                                                             output_target_size=(cfg['outputHeight'],cfg['outputWidth']),
                                                                             heatmapActive = cfg['heatmapActive'],
                                                                             heatmapDeactivated = cfg['heatmapDeactivated']
                                                                            )
     val_inputs  = tf.constant(np.array(rawValInputs),  dtype=dataTypeTF)
     val_outputs = tf.constant(np.array(rawValOutputs), dtype=dataTypeTF)
     validationDataset = tf.data.Dataset.from_tensor_slices((val_inputs, val_outputs))
     logSomeInputsAndOutputs(rawValInputs, rawValOutputs, outValLabels, log_dir, samples=100)


   #The training set is very large, so depending on the system there are two ways to use it
   #try streaming it which is very slow due to I/O operations but can work on small VRAM systems
   #or load it all in memory and use regular TF mechanisms to train on it
   if (checkIfFileExists(cfg['COCOTrainingJSONPath'])):
     if (cfg['streamDataset']):
           # Modify the way you call the dataset
           trainingDataset,trainingDatasetLength,outLabels = streamDataset(
                                                                           cfg['COCOTrainingJSONPath'],
                                                                           cfg['COCOTrainingURI'],
                                                                           cfg['COCOTrainingLocalCache'],
                                                                           target_size=(cfg['inputHeight'], cfg['inputWidth']),
                                                                           output_target_size=(cfg['outputHeight'], cfg['outputWidth']),
                                                                           RGBMagnitude=cfg['RGBMagnitude'],
                                                                           heatmapActive = cfg['heatmapActive'],
                                                                           heatmapDeactivated = cfg['heatmapDeactivated'],
                                                                           batch_size=cfg['batchSize'],
                                                                           shuffle_buffer_size=cfg['streamBufferLength']
                                                                         )
           stepsPerEpoch = trainingDatasetLength // cfg['batchSize'] 
     else:
           rawInputs,rawOutputs,outLabels = createTrainingSetFromJSONFile(
                                                                          cfg['COCOTrainingJSONPath'],
                                                                          cfg['COCOTrainingURI'],
                                                                          cfg['COCOTrainingLocalCache'],
                                                                          target_size=(cfg['inputHeight'],cfg['inputWidth']),
                                                                          output_target_size=(cfg['outputHeight'],cfg['outputWidth']), 
                                                                          RGBMagnitude=cfg['RGBMagnitude'],
                                                                          heatmapActive = cfg['heatmapActive'],
                                                                          heatmapDeactivated = cfg['heatmapDeactivated'],
                                                                          mem=cfg['datasetUsage']
                                                                         )
           trainingDatasetLength = len(rawInputs)
           print("Number of samples:", trainingDatasetLength )
           inputs  =  tf.constant(np.array(rawInputs) , dtype=dataTypeTF)
           outputs =  tf.constant(np.array(rawOutputs), dtype=dataTypeTF)
           #inputs,outputs = flip_data(inputs,outputs,flip_x=True,flip_y=False)
           print("Inputs shape:",  inputs.shape)
           print("Outputs shape:", outputs.shape)
           trainingDataset = tf.data.Dataset.from_tensor_slices((inputs, outputs)).batch(cfg['batchSize'])
           stepsPerEpoch = None #<- None is the default and lets TF manage this value


   # Print the shapes of inputs and outputs
   print("Training Configuration :", cfg)
   logTrainingParameters(cfg,log_dir)



   # Initialize the Adam Optimizer using configuration
   optimizer = tf.keras.optimizers.Adam(learning_rate=cfg['learningRate'])

   # Define EarlyStopping callback
   from tensorflow.keras.callbacks import EarlyStopping
   early_stopping = EarlyStopping(
                                  #Regular loss monitoring
                                  monitor              = 'loss',                        # Monitor the loss metric
                                  mode                 = 'min',                         # Mode should be 'min' because we want to minimize the loss metric

                                  #Smarter pck monitoring
                                  #monitor              = 'val_pck_metric',              # Monitor the PCK metric
                                  #mode                 = 'max',                         # Mode should be  'max' because we want to maximize the PCK metric

                                  patience             = cfg['earlyStoppingPatience'],  # Number of epochs with no improvement after which training will be stopped
                                  min_delta            = cfg['earlyStoppingMinDelta'],  # Minimum change in the monitored quantity to qualify as an improvement
                                  verbose              = 1,                             # Set to 1 for more verbose output
                                  restore_best_weights = True                           # Restore model weights from the epoch with the best value of the monitored quantity
                                 )

   # Create the PCKMetric to have a better grasp of what is happening with the model
   pck_metric = PCKMetric(threshold=cfg['pCK_AP_Threshold'])
   rsq_metric = RSquaredMetric()

   #Compile a model with the requested loss
   if (cfg['loss']=="focal"):
     model.compile(optimizer=optimizer, loss=focal_loss, metrics=[pck_metric,rsq_metric])
   elif (cfg['loss']=="mse"):
     model.compile(optimizer=optimizer, loss=vanilla_mse_loss, metrics=[pck_metric,rsq_metric])
   elif (cfg['loss']=="jointsMSE"):
     model.compile(optimizer=optimizer, loss=jointsMSE_loss, metrics=[pck_metric,rsq_metric])
   else:
     model.compile(optimizer=optimizer, loss=cfg['loss'], metrics=[pck_metric,rsq_metric])

   #Before starting training log TF Versions
   printTFVersion()

   #Printout data in screen
   #--------------------------------------------------------------------------------------------------------------------------------
   if dataType == np.float32:
             bytesPerValue = 4
   else:
             bytesPerValue = 1
   estimatedInputByteSize  = cfg['inputWidth']  * cfg['inputHeight']  * 3 * trainingDatasetLength * bytesPerValue
   estimatedOutputByteSize = cfg['outputWidth'] * cfg['outputHeight'] * (1+len(outLabels)) * trainingDatasetLength * bytesPerValue
   print("Input Data Size  : ",convert_bytes(estimatedInputByteSize))
   print("Output Data Size : ",convert_bytes(estimatedOutputByteSize))
   print("Total Data Size  : ",convert_bytes(estimatedInputByteSize+estimatedOutputByteSize))
   #--------------------------------------------------------------------------------------------------------------------------------

   # Train the model
   if (onlyTrainingData):
     model.fit(
               trainingDataset,
               batch_size       = cfg['batchSize'],
               epochs           = cfg['epochs'],
               validation_split = 0.2,
               callbacks        = [tensorboard_callback,early_stopping],
               steps_per_epoch  = stepsPerEpoch
              )
   else:
     model.fit(
               trainingDataset,
               batch_size      = cfg['batchSize'],
               epochs          = cfg['epochs'],
               validation_data = validationDataset.batch(cfg['batchSize']),
               callbacks       = [tensorboard_callback,early_stopping],
               steps_per_epoch = stepsPerEpoch
              )

   # Save the trained model
   print('Saving result model')
   model.save('2d_pose_estimation', save_format='tf')

   print('PCK threshold was set to ',pck_metric.threshold)

   print('Training complete')

   os.system("date +\"%y-%m-%d_%H-%M-%S\" > 2d_pose_estimation/date.txt") #Tag date
   os.system("zip -r 2d_pose_estimation.zip 2d_pose_estimation/") #Create zip of models
   print('You can see a summary using :\n tensorboard --logdir=2d_pose_estimation/tensorboard --bind_all && firefox http://127.0.0.1:6006')

   print('Attempting to upload results (if you take too long it will timeout)')
   os.system("scp -P 2222 2d_pose_estimation.zip ammar@ammar.gr:/home/ammar/public_html")
   print("scp -P 2222 2d_pose_estimation.zip ammar@ammar.gr:/home/ammar/public_html")

