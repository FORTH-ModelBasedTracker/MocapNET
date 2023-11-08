#!/usr/bin/python3

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import sys
import os
import time

import tensorflow as tf
from tensorflow import keras
from tensorflow.keras.layers import concatenate, Add, Input, Dense, GlobalMaxPooling1D, GlobalAveragePooling1D, Flatten, Reshape, AlphaDropout, Dropout, Lambda, MaxPooling1D, MaxPooling2D, Conv2D, ZeroPadding1D
from tensorflow.keras.models import Model,Sequential, model_from_json
from tensorflow.keras.utils  import plot_model

from tensorflow.keras import layers
from tensorflow.keras import activations

from tools import bcolors,createDirectory,tensorflowFriendlyModelName

import math
import numpy as np

#Defaults, will get overridden by setupDNNModelsUsingJSONConfiguration
useLambdas       = 0
numberOfLayers   = 12
dropoutRate      = 0.15     #Global dropout rate
learningRate     = 0.00025 #0.00045 #0.00025=MocapNET2019
useModuloMetric  = 0
useQuadMetric    = 0
useSquaredMetric = 1

# ------------------------------------------------------------
#https://github.com/cpuimage/HardMish
def hard_mish(x):
    return tf.minimum(2., tf.nn.relu(x + 2.)) * 0.5 * x
# ------------------------------------------------------------

#https://stackoverflow.com/questions/46355068/keras-loss-function-for-360-degree-prediction

# y in radians
#def mean_squared_error_360(y_true, y_pred):
#  yTrueRads=tf.math.scalar_mul(0.017453292519943295,y_true)
#  yPredRads=tf.math.scalar_mul(0.017453292519943295,y_pred)

#  return tf.reduce_mean(tf.math.square(tf.math.scalar_mul(57.295779513,tf.atan2(tf.sin(yTrueRads - yPredRads), tf.cos(yTrueRads - yPredRads)))))
  #return tf.math.scalar_mul(57.295779513,tf.reduce_mean(tf.abs(tf.atan2(tf.sin(yTrueRads - yPredRads), tf.cos(yTrueRads - yPredRads)))))

#def rmse_360(y_true, y_pred):
#  return K.sqrt(mean_squared_error_360(y_true, y_pred))

def testMyObjective():
    X = tf.compat.v1.placeholder("float32", name="input")
    Y = tf.compat.v1.placeholder("float32", name="input")
    OUT = tf.abs(tf.subtract(tf.math.floormod(tf.add(X,180),360),tf.math.floormod(tf.add(Y,180),360)))
    with tf.compat.v1.Session() as sess:
        for x in range (-360,360):
          for y in range (-360,360):
            print("x=",x," y=",y," val=",sess.run(OUT, feed_dict={X:x,Y:y}))
    sys.exit(0)
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def mean_quad_error(yTrue,yPred): 
   #reduce_mean reduce_sum
  return tf.reduce_mean(input_tensor=tf.math.square(tf.math.square(tf.subtract(yTrue,yPred))))
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def mean_squared_error_modulo_360(yTrue,yPred): 
   #reduce_mean reduce_sum
  return tf.reduce_mean(input_tensor=tf.math.square(tf.abs(tf.subtract(tf.math.floormod(tf.add(yTrue,180),360),tf.math.floormod(tf.add(yPred,180),360)))))
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def average_error_modulo_360(yTrue,yPred):
  return tf.reduce_mean(input_tensor=tf.abs(tf.subtract(tf.math.floormod(tf.add(yTrue,180),360),tf.math.floormod(tf.add(yPred,180),360))))


#=================================================================================================================================================================================== 




#SWISH - https://arxiv.org/abs/1710.05941
#MISH  - https://arxiv.org/vc/arxiv/papers/1908/1908.08681v1.pdf / https://github.com/cpuimage/HardMish 
#https://krutikabapat.github.io/Swish-Vs-Mish-Latest-Activation-Functions/

#-------------------------------------------------------------
#-------------------------------------------------------------
theActivationMethod='selu' # hard_mish 'selu' 'swish' 
initializer = tf.keras.initializers.LecunNormal() #'lecun_normal'
#-------------------------------------------------------------
#-------------------------------------------------------------
def startProfiling():
       print(bcolors.WARNING,"Starting Tensorflow Profiling (this run will be slower than usual)..\n",bcolors.ENDC)
       os.system("rm -rf profiling")
       tf.profiler.experimental.start('profiling')
#-------------------------------------------------------------
def stopProfiling():
       print(bcolors.WARNING,"Stopping Tensorflow Profiling..\n",bcolors.ENDC)
       tf.profiler.experimental.stop()
#-------------------------------------------------------------
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#===================================================================================================================================================================================



def getActivationRandomization(configuration):
    global theActivationMethod
    theActivationMethod=configuration['activationFunction']
    global initializer
    theRandomizationMethod=configuration['weightRandomizationFunction']
    #----------------------------------------------------------------------
    print(bcolors.OKGREEN,"Activation/Randomization set to ",theActivationMethod," -> ",theRandomizationMethod,"  ",bcolors.ENDC)
    #----------------------------------------------------------------------
    thisSeed = 0
    if (configuration["setConstantSeedForReproducibleTraining"]==0):
       import random
       seed = random.randint(0,1024) 
       print("Setting random seed to ",seed,"! ")

    if (theRandomizationMethod=="auto"):
        if (theActivationMethod=='selu'):
           initializer = tf.keras.initializers.LecunNormal(seed=thisSeed)
           print("Automatic resolution SeLU -> LeCun Normal")
        elif (theActivationMethod=='swish'):
           #Draws samples from a uniform distribution within [-limit, limit], where limit = sqrt(6 / fan_in) (fan_in is the number of input units in the weight tensor).
           initializer=tf.keras.initializers.HeUniform(seed=thisSeed) #https://www.cv-foundation.org/openaccess/content_iccv_2015/html/He_Delving_Deep_into_ICCV_2015_paper.html
           print("Automatic resolution SWISH -> He Uniform")
    elif (theRandomizationMethod=="glorot_uniform"): #Xavier
        initializer=tf.keras.initializers.GlorotUniform(seed=thisSeed)
    elif (theRandomizationMethod=="lecun_normal"):
        initializer=tf.keras.initializers.LecunNormal(seed=thisSeed)
    elif (theRandomizationMethod=="he_uniform"): #Kaiming
        initializer=tf.keras.initializers.HeUniform(seed=thisSeed)
    else:
        print(bcolors.FAIL,"Please add ",theActivationMethod,"/",theRandomizationMethod," to getActivationRandomization",bcolors.ENDC)
        sys.exit(1)





def setupDNNModelsUsingJSONConfiguration(configuration):
    #Copy settings from configuration json file 
    #---------------------------------------------------------------
    global numberOfLayers
    numberOfLayers=configuration['neuralNetworkDepth'] 
    #---------------------------------------------------------------
    #New code that handles activation/randomization
    getActivationRandomization(configuration)
    #---------------------------------------------------------------
    global dropoutRate
    dropoutRate=configuration['dropoutRate']
    global learningRate
    learningRate=configuration['learningRate'] #0.00045 #0.00025=MocapNET2019 
    global useQuadMetric
    useQuadMetric=configuration['useQuadLoss']
    global useSquaredMetric
    useSquaredMetric=configuration['useSquaredLoss']
    #---------------------------------------------------------------
    print("Configuration setting numberOfLayers to ",numberOfLayers)
    print("Configuration setting activationMethod to ",theActivationMethod)
    print("Configuration setting dropoutRate to ",dropoutRate)
    print("Configuration setting learningRate to ",learningRate)
    print("Configuration setting useQuadMetric to ",useQuadMetric)
    #---------------------------------------------------------------
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def saveProgressEnded(message):
    file = open("progress.txt","w") 
    file.write(str(message))
    file.write("\n")
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def saveLastCompletedJob(lastCompletedJob,currentJob,lastJob,approxTime):
    file = open("progress.txt","w") 
    file.write(str(lastCompletedJob))
    file.write("\n")
    file.write(str(currentJob))
    file.write("/")
    file.write(str(lastJob))
    file.write("\n")
    file.write("TimeApprox:")
    file.write(str(approxTime))
    file.write(" minutes")
    file.write("\n")
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def saveConfiguration(path,model,modelName,modelParameters,numberOfEpochs,batchSize,trainInput,trainOutput,outputType,history,startAt,endAt,median,mean,std,var):
    finalPath = ("%s/%s") % (path,modelName) 
    createDirectory(path)
    createDirectory(finalPath)
    file = open("%s/configuration.txt" % finalPath,"w") 
    file.write("Number of Model Parameters:")
    file.write(str(modelParameters))
    file.write("\nNumber of Epochs:") 
    file.write(str(numberOfEpochs)) 
    file.write("\nBatch Size:") 
    file.write(str(batchSize))
    #--------------------------------
    file.write("\nTrain Input Number of elements:")
    file.write(str(len(trainInput))) 
    file.write("\nTrain Input: ")
    for item in trainInput:
            file.write("%s " % item)
    #--------------------------------


    #file.write("\nModel:")
    #file.write(model.summary()) #TypeError: write() argument must be str, not None
    #file.write("\n")


    #--------------------------------
    file.write("\nTest Output Number of elements:")
    file.write(str(len(trainOutput))) 
    file.write("\nTest Output: ")
    for item in trainOutput:
            file.write("%s " % item)
    #--------------------------------
    file.write("\n\nSpecific Output: ")
    file.write(trainOutput[outputType])
    file.write("\n")
    

    file.write("\n\nOutput Statistics for ")
    title_string=" %s : Median=%0.2f,Mean=%0.2f,Std=%0.2f,Var=%0.2f" % (modelName,median,mean,std,var)
    file.write(title_string)
    file.write("\n")

    try:
      file.write("\nTraining History Loss: ")
      file.write(str(history.history['loss']))
      file.write("\n")
    except:
      print("Tried to save loss but no such history element was found..") 

    try:
      file.write("\nTraining History MAE: ")
      file.write(str(history.history['mean_absolute_error']))
      file.write("\n")
    except:
      print("Tried to save mean absolute error but no such history element was found..") 

    #file.write("\nTraining History MAE: ")
    #file.write(str(history.history['mean_absolute_error']))
    #file.write("\n")

    #file.write("\nTraining History Accuracy: ")
    #file.write(str(history.history['acc']))
    #file.write("\n")

    #file.write("\nTesting History Loss: ")
    #file.write(str(history.history['val_loss']))
    #file.write("\n")

    #file.write("\nTesting History MAE: ")
    #file.write(str(history.history['val_mean_absolute_error']))
    #file.write("\n")

    #file.write("\nTesting History Accuracy: ")
    #file.write(str(history.history['val_acc']))
    #file.write("\n")

    file.write("\nDuration: ")
    file.write(str((endAt-startAt)/60))
    file.write(" mins\n")
    file.close() 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def loadNewModel(path):
    #loaded_model = tf.saved_model.load(path)
	#loaded_model.compile(loss='mse', optimizer='rmsprop', metrics=['mae', 'acc'])
    #return loaded_model
    return tf.keras.models.load_model(path)
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
"""
Load the model to the supplied path (first try TF, then JSON/H5)
"""
def loadModel(path,filename):
    if (os.path.isfile('%s/saved_model.pb' % (path))):
        print(bcolors.OKGREEN,"Loading TF Model %s/saved_model.pb from disk " % (path),bcolors.ENDC)
        loaded_model = tf.keras.models.load_model(path,custom_objects={'mean_quad_error':mean_quad_error})
        return loaded_model
    elif (os.path.isfile('%s/%s.json' % (path,filename))) and (os.path.isfile('%s/%s.h5' % (path,filename))):
        print(bcolors.FAIL,"File %s/%s.json does not exist\n",bcolors.ENDC)
        json_file = open('%s/%s.json' % (path,filename),'r')
        loaded_model_json = json_file.read()
        json_file.close()
        loaded_model = model_from_json(loaded_model_json)
        loaded_model.load_weights("%s/%s.h5" % (path,filename))
        print(bcolors.OKGREEN,"Success ",bcolors.ENDC)
        loaded_model.compile(loss='mse', optimizer='rmsprop', metrics=['mae', 'acc'])
        print("Loading Model %s/%s from disk" % (path,filename))
        return loaded_model
    else:
        print(bcolors.FAIL,"Could not find model %s \n" % path,bcolors.ENDC)
        sys.exit(1)
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
"""
Save the model to the supplied path
"""
def saveModel(path,model,name="model"):
    createDirectory(path)
    # serialize model to JSON
    model_json = model.to_json()
    with open("%s/%s.json" % (path,name) , "w") as json_file:
        json_file.write(model_json)
        json_file.close()
    # serialize weights to HDF5
    model.save_weights("%s/%s.h5" % (path,name))
    print("Saved model to disk at %s/%s.(h5/json)" % (path,name))
    #24/5/23:  TF 2.12.0 : emmits you must feed a value for placeholder tensor 'inputs' with dtype float and shape [?,76]
    model.save(path, save_format='tf') #save directory..
    print("Saved model to disk at %s/saved_model.pb (TF)" % (path))
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
"""
Counts the number of trainable parameters in a TensorFlow model
"""
def countModelParameters(model):
    total_parameters = 0
    for variable in model.trainable_variables:
        shape = variable.shape
        variable_parametes = 1
        for dim in shape:
            variable_parametes *= dim
        total_parameters += variable_parametes
    return total_parameters
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
# define autoencoder model
def newAutoencoderModel(modelName,inputDimension,shrinkRatio):
	# create model
	model = Sequential(name=modelName) #-----------------------------------------------------------------------------------------------------------------------------------
	model.add(Dense(inputDimension, input_dim=inputDimension, activation='relu' , name='encoder')) 
	model.add(Dense(int(inputDimension/shrinkRatio), kernel_initializer='normal', activation='relu'))
	model.add(Dense(inputDimension, activation='sigmoid' , name='decoder')) #-----------------------------------------------------------------------------------------------------------------------------------
	# Compile model
	model.compile(optimizer='adadelta',loss='binary_crossentropy', metrics=['accuracy'])
	#model.compile(optimizer='rmsprop', loss='mse', metrics=['mae'])
	model.summary() 
	return model


#----------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------
#                                                     LAMBDA LAYERS 
#----------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------
#Found in https://github.com/keras-team/keras/issues/890
#by https://github.com/marc-moreaux
def sliceL(dimension, start, end):
    # Crops (or slices) a Tensor on a given dimension from start to end
    # example : to crop tensor x[:, :, 5:10]
    # call slice(2, 5, 10) as you want to crop on the second dimension
    def func(x):
        if dimension == 0:
            return x[start: end]
        if dimension == 1:
            return x[:, start: end]
        if dimension == 2:
            return x[:, :, start: end]
        if dimension == 3:
            return x[:, :, :, start: end]
        if dimension == 4:
            return x[:, :, :, :, start: end]
    return Lambda(func, name='Slice_from_%u_to_%u_in_%u-D'%(start,end,dimension))
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def modL(): 
    def func(x): 
            return (360+x)%360 
    return Lambda(func, name='360_plus_x_modulo_360')
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def tag(kind="layer",name="mnet",index=0,number=0):
    return "%s_%u_%s_%u"%(name,index,kind,number)


#----------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------
#                                                      FIRST STAGE 
#----------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------
def newCategorizeOneHotModel(modelName,inputDimension,nonNSDMInputSize,numberOfChannelsPerNSDMElement,outputSize,networkCompression):
    print('newCategorizeOneHotModel has input ',inputDimension,' elements and output of ',outputSize,' elements e:',tf.keras.backend.epsilon())
    print('Learning Rate is 0.00001, Dropout Rate is ',dropoutRate,' ')
    modelName = tensorflowFriendlyModelName(modelName)
    print("Model renamed to ",modelName," to make sure it doesn't call tensorflow problems ")

    inputs = Input(shape=(inputDimension,))
     
    #positionalInput=nonNSDMInputSize
    #differentiatedInput=inputDimension-positionalInput
    #splitInput = sliceL(1,positionalInput,inputDimension)(inputs)    
    #inputDimension = inputDimension-positionalInput
    
    #initializer = tf.keras.initializers.lecun_normal(seed=0)
 
    outputArrayIndex=0
  
    doInputSplit = 0 
    selectedInput = inputs

    if (doInputSplit):
       positionalInput=nonNSDMInputSize
       differentiatedInput=inputDimension-positionalInput
       splitInput = sliceL(1,positionalInput,inputDimension)(inputs)    
       inputDimension = inputDimension-positionalInput
       selectedInput = splitInput
     
    # a layer instance is callable on a tensor, and returns a tensor
    #goldenRatio=1.61803398875  // was 2 3 4 5 6
    #-----------------------------------------------------------------------------------------------------------------------------------
    #Input    | 320     
    
    #Shorthand names that fit in screen
    act = theActivationMethod
    kinit = initializer
    indim = inputDimension
    l = networkCompression
 
    # Layer 1 
    thisLayerRatio=2
    layerNumber=1
    xA   =   Dense(int(indim/(thisLayerRatio*l)), input_shape=(inputDimension,) , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber) )(selectedInput)#(inputs)
    xA   =   Dropout(0.2, name='classifier_%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xA)

    # Layer 2 
    thisLayerRatio=2
    layerNumber=layerNumber+1
    xB   =   Dense(int(indim/(thisLayerRatio*l))                                , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber) )(xA) 
    xB   =   Dropout(0.3, name='classifier_%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xB)

    # Layer 3 
    thisLayerRatio=3
    layerNumber=layerNumber+1
    xC   =   Dense(int(indim/(thisLayerRatio*l))                                , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber) )(xB) 
    xC   =   Dropout(0.3, name='classifier_%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xC)

    # Layer 4 
    thisLayerRatio=4
    layerNumber=layerNumber+1
    xD   =   Dense(int(indim/(thisLayerRatio*l))                                , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber) )(xC) 
    xD   =   Dropout(0.4, name='classifier_%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xD)
    sBD  =   Dense(int(indim/(thisLayerRatio*networkCompression))               , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_residual_%u'%(modelName,outputArrayIndex,layerNumber))(xB)
    xD = Add()([xD,sBD]) # main + skip    

    # Layer 5
    thisLayerRatio=5
    layerNumber=layerNumber+1
    xE   =   Dense(int(indim/(thisLayerRatio*l))                                , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber) )(xD) 
    xE   =   Dropout(0.4, name='classifier_%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xE)

    # Layer 6 
    thisLayerRatio=6
    layerNumber=layerNumber+1
    xF   =   Dense(int(indim/(thisLayerRatio*l))                                , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber) )(xE) 
    xF   =   Dropout(0.4, name='classifier_%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xF)

    # Layer 7 
    thisLayerRatio=7
    layerNumber=layerNumber+1
    xG   =   Dense(int(indim/(thisLayerRatio*l))                                , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber) )(xF)
    xG   =   Dropout(0.4, name='classifier_%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xG)
    sDG  =   Dense(int(indim/(thisLayerRatio*networkCompression))               , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_residual_%u'%(modelName,outputArrayIndex,layerNumber))(xD)
    xG = Add()([xG,sDG]) # main + skip    

    # Layer 8 
    thisLayerRatio=8
    layerNumber=layerNumber+1
    xH   =   Dense(int(indim/(thisLayerRatio*l))                                , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber) )(xG)
    #xH   =   Dropout(0.4)(xH)

    # Layer 9 
    layerNumber=layerNumber+1
    xOut =   Dense(int(outputSize)                                              , kernel_initializer=kinit, activation=act, name='classifier_%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber) )(xH) 

    #-----------------------------------------------------------------------------------------------------------------------------------
    predictions = tf.keras.layers.Dense(int(outputSize),name='Category' , kernel_initializer='normal', activation='softmax')(xOut)
    #-----------------------------------------------------------------------------------------------------------------------------------

    # This creates a model that includes
    # the Input layer and three Dense layers
    model = Model(name=modelName, inputs=inputs, outputs=predictions) 

    #from tf.keras.metrics import categorical_accuracy
    #model.compile(optimizer='adam', loss='binary_crossentropy', metrics=[categorical_accuracy]) 
	 
    #special slower learning..
    #For 3.8M samples 0.00001 is a good value..
    if (optimizer=="adam"):
       activeOptimizer=tf.keras.optimizers.Adam(learning_rate=learningRate,epsilon=tf.keras.backend.epsilon())
    else:
       activeOptimizer=tf.keras.optimizers.RMSprop(learning_rate=learningRate, rho=0.9, epsilon=tf.keras.backend.epsilon()) # epsilon=1e-6, lr=0.00025 is the old value
    #-------------------------------------------------------------------------------------------------------
    model.compile(optimizer=activeOptimizer, loss='categorical_crossentropy', metrics=['accuracy']) 
    #-------------------------------------------------------------------------------------------------------	  
    model.summary() 
    #plot_model(model, to_file='modelXYZStage.png')
    return model
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
"""
  The regular MocapNETv3 Densely Connected Encoder Along With its Skip Connections 
"""
def newDeepRotModel(configuration,modelName,outputArrayIndex,inputDimension,nonNSDMInputSize,numberOfChannelsPerNSDMElement,outputSize,networkCompression,probabilisticMode=0,quantize=False,optimizer="rmsprop"):
    #return convDeepRotModel(modelName,outputArrayIndex,inputDimension,nonNSDMInputSize,numberOfChannelsPerNSDMElement,outputSize,networkCompression)
    
    dropoutRate = float(configuration["dropoutRate"])
    print('newXYZROTModel with skip connections has input ',inputDimension,' elements , compression λ=',networkCompression,' and output of ',outputSize,' elements')
    print('Learning Rate is ',learningRate,', Dropout Rate is ',dropoutRate,' ')
    print('Use Quad Loss is ',useQuadMetric,', Use Modulo Loss is ',useModuloMetric,' ')
    print('Use Squared Loss is ',useSquaredMetric)
    modelName = tensorflowFriendlyModelName(modelName)
    print("Model renamed to ",modelName," to make sure it doesn't call tensorflow problems ")

    inputs = Input(shape=(inputDimension,))
    
    doInputSplit = 0
    selectedInput = inputs

    if (doInputSplit):
       positionalInput=nonNSDMInputSize
       differentiatedInput=inputDimension-positionalInput
       splitInput = sliceL(1,positionalInput,inputDimension)(inputs)    
       inputDimension = inputDimension-positionalInput
       selectedInput = splitInput
     
    # a layer instance is callable on a tensor, and returns a tensor
    #goldenRatio=1.61803398875  // was 2 3 4 5 6
    #-----------------------------------------------------------------------------------------------------------------------------------
    #Input    | 320     | 463
    
    #Shorthand names that fit in screen
    act     = theActivationMethod
    kinit   = initializer
    inptdim = inputDimension

    if (numberOfLayers==0):
       print("Garbage configuration with 0 layers")
       sys.exit(1)


    # Layer 1 | 2 |160
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=1):
       layerNumber=1
       thisLayerRatio=2.2 #BMVC21 this was 2.2
       xA = Dense(int(inptdim/(thisLayerRatio*networkCompression)), input_shape=(inptdim,) , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(selectedInput)
       if (dropoutRate>0.0):
          xA = Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xA) #0.2 vs dropoutRate
    if (numberOfLayers==1):
       xOut = xA
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    # Layer 2 | 3 | 106
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=2):
       layerNumber=layerNumber+1
       thisLayerRatio=3.0 #BMVC21 this was 3.0
       xB   =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xA) 
       if (dropoutRate>0.0):
          xB   =   Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xB) #0.3 vs dropoutRate
    if (numberOfLayers==2):
       xOut = xB
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    
    # Layer 3 | 4 | 80
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=3):
       layerNumber=layerNumber+1
       thisLayerRatio=3.2 #BMVC21 this was 3.2
       sAC  =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_residual_%u'%(modelName,outputArrayIndex,layerNumber))(xA)
       if (dropoutRate>0.0):
          sAC  =   Dropout(dropoutRate, name='%s_%u_rdropout_%u'%(modelName,outputArrayIndex,layerNumber))(sAC) #0.4 vs dropoutRate 
       xC   =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xB) 
       if (dropoutRate>0.0):
          xC   =   Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xC) #0.4 vs dropoutRate
       xC = Add(name='%s_%u_add_%u'%(modelName,outputArrayIndex,layerNumber))([xC,sAC]) # main + skip    
    if (numberOfLayers==3):
       xOut = xC
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    # Layer 4 | 5 | 64 
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=4):
       layerNumber=layerNumber+1
       thisLayerRatio=3.4 #BMVC21 this was 3.4
       xD   =   Dense(int(inptdim/(thisLayerRatio*networkCompression)), kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xC)
       if (dropoutRate>0.0):
          xD   =   Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xD)
    if (numberOfLayers==4):
       xOut = xD
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    # Layer 5 | 6 | 53
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=5):
       layerNumber=layerNumber+1
       thisLayerRatio=3.5 #BMVC21 this was 3.5
       sCE  =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_residual_%u'%(modelName,outputArrayIndex,layerNumber))(xC)
       if (dropoutRate>0.0):
          sCE  =   Dropout(dropoutRate, name='%s_%u_rdropout_%u'%(modelName,outputArrayIndex,layerNumber))(sCE)
       xE   =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xD) 
       if (dropoutRate>0.0):
          xE   =   Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xE)
       xE = Add(name='%s_%u_add_%u'%(modelName,outputArrayIndex,layerNumber))([xE,sCE]) # main + skip    
    if (numberOfLayers==5):
       xOut = xE
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    # Layer 6 | 8 | 106
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=6):
       layerNumber=layerNumber+1
       thisLayerRatio=3.8 #BMVC21 this was 3.8
       xF   =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xE)
       if (dropoutRate>0.0):
          xF   =   Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xF)
    if (numberOfLayers==6):
       xOut = xF
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    # Layer 7
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=7):
       layerNumber=layerNumber+1
       thisLayerRatio=4.2 #BMVC21 this was 4.2
       sEG  =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_residual_%u'%(modelName,outputArrayIndex,layerNumber))(xE)
       if (dropoutRate>0.0):
          sEG  =   Dropout(dropoutRate, name='%s_%u_rdropout_%u'%(modelName,outputArrayIndex,layerNumber))(sEG)
       xG   =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xF)
       if (dropoutRate>0.0):
          xG   =   Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xG)
       xG = Add(name='%s_%u_add_%u'%(modelName,outputArrayIndex,layerNumber))([xG,sEG]) # main + skip    
    if (numberOfLayers==7):
       xOut = xG
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    # Layer 8
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=8):
       layerNumber=layerNumber+1
       thisLayerRatio=5 #BMVC21 this was 5
       xH   =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xG)
       if (dropoutRate>0.0):
          xH   =   Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xH)
    if (numberOfLayers==8):
       xOut = xH
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    # Layer 9
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=9):
       layerNumber=layerNumber+1
       thisLayerRatio=6 #BMVC21 this was 6
       sGI  =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_residual_%u'%(modelName,outputArrayIndex,layerNumber))(xG)
       #if (dropoutRate>0.0):
       #   sGI  =   Dropout(dropoutRate, name='%s_%u_rdropout_%u'%(modelName,outputArrayIndex,layerNumber))(sGI)
       xI   =   Dense(int(inptdim/(thisLayerRatio*networkCompression)) , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xH)
       #if (dropoutRate>0.0): #After experiment 269A, dropout this late seems like a bad idea
       #   xI   =   Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xI)
       xI = Add(name='%s_%u_add_%u'%(modelName,outputArrayIndex,layerNumber))([xI,sGI]) # main + skip    
    if (numberOfLayers==9):
       xOut = xI
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    # Layer 10
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=10):
       layerNumber=layerNumber+1
       preLastDimension = 60 #Pre probabilities was 32  
       #Last Dimension before max pooling
       xPreLast   =   Dense(preLastDimension , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xI)
       #if (dropoutRate>0.0): #After experiment 269A, dropout this late seems like a bad idea
       #   xPreLast   =   Dropout(dropoutRate, name='%s_%u_dropout_%u'%(modelName,outputArrayIndex,layerNumber))(xPreLast)
    if (numberOfLayers==10):
       xOut = xPreLast
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
     
    # Layer 11
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=11):
       layerNumber=layerNumber+1
       lastDimension = 60 #Pre probabilities was 16
       #Last Dimension before max pooling
       xLast   =   Dense(lastDimension , kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'%(modelName,outputArrayIndex,layerNumber))(xPreLast)
       
       #Lets pick the maximum response as our final output
       #reshaped = Reshape([lastDimension,1])(xLast)
       #flat = GlobalMaxPooling1D() (reshaped)
       #flat = GlobalAveragePooling1D() (reshaped)
       flat = xLast
    if (numberOfLayers==11):
       xOut = flat
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 
    # Layer 12
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (numberOfLayers>=12):
       layerNumber=layerNumber+1                            #last activation no longer linear 'linear'
       sEOut=   Dense(int(outputSize), kernel_initializer=kinit, activation=act, name='%s_%u_residual_%u'%(modelName,outputArrayIndex,layerNumber))(xE)
       xOut =   Dense(int(outputSize), kernel_initializer=kinit, activation=act, name='%s_%u_layer_%u'   %(modelName,outputArrayIndex,layerNumber))(flat)
       xOut = Add(name='%s_%u_add_%u'%(modelName,outputArrayIndex,layerNumber))([xOut,sEOut]) # main + skip
    #------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (probabilisticMode):
       #-----------------------------------------------------------------------------------------------------------------------------------
       predictions = tf.keras.layers.Dense(int(outputSize), name='output_rotation_%u_%s'%(outputArrayIndex,modelName), kernel_initializer='normal', activation='softmax')(xOut)
       #-----------------------------------------------------------------------------------------------------------------------------------
    else:
       #And also have a weight to scale it.
       predictions = tf.keras.layers.Dense(int(outputSize),kernel_initializer='normal', activation='linear', name='output_rotation_%u_%s'%(outputArrayIndex,modelName))(xOut)
       #-----------------------------------------------------------------------------------------------------------------------------------

 
    # the Input layer and three Dense layers
    model = Model(name=modelName, inputs=inputs, outputs=predictions) 
    #model.compile(optimizer='adam', loss='mse', metrics=['mae']) 
    if (optimizer=="adam"):
       activeOptimizer=tf.keras.optimizers.Adam(learning_rate=learningRate,epsilon=tf.keras.backend.epsilon())
    else:
       activeOptimizer=tf.keras.optimizers.RMSprop(learning_rate=learningRate, rho=0.9, epsilon=tf.keras.backend.epsilon()) # epsilon=1e-6, lr=0.00025 is the old value
    #-------------------------------------------------------------------------------------------------------
    
    if (quantize):
       #Careful not all activations are quantization aware
       print("Will attempt to quantize model..!")
       try:
         import tensorflow_model_optimization as tfmot
         quantize_model = tfmot.quantization.keras.quantize_model
         model = quantize_model(model) 
       except:
         print("Could not quantize model, you are using a non quantization aware activation function..!")
         print("https://github.com/tensorflow/model-optimization/blob/master/tensorflow_model_optimization/python/core/quantization/keras/quantize_aware_activation.py")
    
    if (probabilisticMode):
        model.compile(optimizer=activeOptimizer, loss='categorical_crossentropy', metrics=['accuracy'])
    elif (useQuadMetric): 
        model.compile(optimizer=activeOptimizer, loss=mean_quad_error, metrics=['mae'] ) #Penalize really bad output..
    elif (useSquaredMetric):
        model.compile(optimizer=activeOptimizer, loss='mse', metrics=['mae'])
    elif (useModuloMetric):
        model.compile(optimizer=activeOptimizer, loss=mean_squared_error_modulo_360, metrics=[average_error_modulo_360] ) #With 360  metrics=[rmse_360]
    else:
        model.compile(optimizer=activeOptimizer, loss='mae', metrics=['mae']) #Without 360  

    model.summary() 
    #plot_model(model, to_file='modelXYZStage.png')
    return model
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
"""
  A dummy model made to pad outputs and keep signature compatibility between different 
  NN ensembles
"""
def newTrivialModel(modelName,outputArrayIndex,inputDimension,outputSize):
    modelName = tensorflowFriendlyModelName(modelName)
    print("Model renamed to ",modelName," to make sure it doesn't call tensorflow problems ")
    inputs = Input(shape=(inputDimension,))
    trivialInitializer = tf.keras.initializers.Zeros()

    #The trivial model is basically dead and useless and we want to simplify it as much as possible
    #so that it occupies as little space in our network as possible
 
    #There are two ways to do this, first is by a tf split and the second by doing a Lambda function that ignores most inputs
    #-----------------------------------------------------------------------------------------------------------------------------------
    if (useLambdas==0): 
       ignoreInput = tf.split(inputs,inputDimension,num=inputDimension,axis=1,name='ignore_layer_for_%u_%s'%(outputArrayIndex,modelName)) #split inputs to single elements
       splitInput  = ignoreInput[0] #try to do same thing without lambdas 
    else:
       #partOfInputToKeep=int(inputDimension)   
       partOfInputToKeep=1 
       splitInput  = sliceL(1,0,partOfInputToKeep)(inputs)       
    #-----------------------------------------------------------------------------------------------------------------------------------

    #Now connect our single input with a mock layer with one set of weights so that it can learn to send zeros out :P  
    #-----------------------------------------------------------------------------------------------------------------------------------
    xOut =   Dense(int(outputSize), kernel_initializer=trivialInitializer , activation='linear', name='mock_layer_for_%u_%s'%(outputArrayIndex,modelName) )(splitInput) 
    predictions = tf.keras.layers.Dense(int(outputSize), name='output_trivial_%u_%s'%(outputArrayIndex,modelName) )(xOut)
    #----------------------------------------------------------------------------------------------------------------------------------- 

    # This creates a model that includes 
    model = Model(name=modelName, inputs=inputs, outputs=predictions) 
    model.compile(optimizer='rmsprop', loss='mse', metrics=['mae']) 
	 
    #Do not emmit a summary for trivial models
    #model.summary()
    print("Not emitting a summary for trivial model ",modelName," with input dim ",inputDimension," and output ",outputSize) 
    return model
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def combineModels(configuration,directory,outputFilename,modelInputSize,modelOutputSize,modelPaths,startModel,endModel,label,lpdnnPadding=0,doPNGPlot=0,optimizer="rmsprop",skipTrivialModels=False):
    inputLabel  = "input_%s"  % label
    outputLabel = "result_%s" % label
    
    if (lpdnnPadding!=0):
       print(bcolors.WARNING)
       print("Using LPDNN Padding.. ")
       print(bcolors.ENDC)
       #Not setting batch_size=1 will result in a [?,..]  shape which breaks things in the case of BonsApps/LPDNN
       rawInput = Input(shape=(1,1,modelInputSize,), batch_size=1, name=inputLabel) 
       rawInput.set_shape((1,1,1,modelInputSize))
       singleInput = Reshape((modelInputSize,), input_shape=(1,1,1,modelInputSize,) , name='reshaped_'+inputLabel)(rawInput)
    else:
       print(bcolors.OKGREEN)
       print("Using No Input Padding.. ")
       print(bcolors.ENDC)
       rawInput = Input(shape=(modelInputSize,), batch_size=1, name=inputLabel) 
       singleInput = rawInput

    print(bcolors.OKGREEN)
    print("Combining model Input:%s / Output:%s / modelInputSize:(1,%u) / modelOutputSize:(1,%u) / Models Combined:(%u->%u) "%(inputLabel,outputLabel,modelInputSize,modelOutputSize,startModel,endModel))
    print("Input Shape is : ")
    print(singleInput.get_shape())
    print(bcolors.ENDC)

    #Initialize our RMSProp optimizer
    #----------------------------------------------------------------------------------------------------------------------------------------
    if (optimizer=="adam"):
       activeOptimizer=tf.keras.optimizers.Adam(learning_rate=learningRate,epsilon=tf.keras.backend.epsilon())
    else:
       activeOptimizer=tf.keras.optimizers.RMSprop(learning_rate=learningRate, rho=0.9, epsilon=tf.keras.backend.epsilon()) # epsilon=1e-6, lr=0.00025 is the old value
    #-------------------------------------------------------------------------------------------------------

    #Start combining models
    #inModelList          = list() # empty list   
    #outModelList         = list() # empty list  
    allModelList         = list() # empty list    
    includedOutputs      = list() # empty list
    selectedColumns      = list() # empty list
    selectedColumnsCount = 0
    cumulativeTime       = 0.0 

    from tools import getConfigurationJointIsDeclaredInHierarchy,getConfigurationJointPriority
 
    #if ((startModel==0) and (endModel==0)):
    #       print("THIS LOOKS LIKE THE SPECIAL CASE OF MERGING ONE THING WITH ITSELF.. :S")
    #       endModel=1

    for modelNumber in range(startModel,endModel):
        jointIsFormallyDeclared = getConfigurationJointIsDeclaredInHierarchy(configuration,modelPaths[modelNumber])
        jointPriority           = getConfigurationJointPriority(configuration,modelPaths[modelNumber])
        if (skipTrivialModels) and (jointPriority==0):
           print(bcolors.FAIL,"Skipping Models IS BUGGY BE CAREFUL ",bcolors.ENDC)
           print(bcolors.WARNING,"Skipping Model %u/%u %s/%s!" % (modelNumber,endModel,directory,modelPaths[modelNumber]),bcolors.ENDC)
           selectedColumns.append(0)
        else:
           start = time.time()
           #--------------------------------------------------------------------------------------------------------
           print("Loading Model %u/%u %s/%s from disk" % (modelNumber,endModel,directory,modelPaths[modelNumber]))
           #--------------------------------------------------------------------------------------------------------
           loaded_model = loadModel("%s/%s/"% (directory,modelPaths[modelNumber]),"model")
           allModelList.append(loaded_model(singleInput)) 
           #--------------------------------------------------------------------------------------------------------
           #print("Inputs : ",allModelList[selectedColumnsCount].inputs)
           #print("Outputs : ",allModelList[selectedColumnsCount].outputs)
           #for layer in loaded_model.layers:
           #     if str(layer.name).find("input_")==0:
           #       print("Input layer is : ",layer.name)
           #       inModelList.append(layer)
           #     if str(layer.name).find("output_")==0:
           #       print("Output layer is : ",layer.name)
           #       outModelList.append(layer)
           #--------------------------------------------------------------------------------------------------------
           includedOutputs.append(modelPaths[modelNumber]) 
           selectedColumns.append(1)
           selectedColumnsCount = selectedColumnsCount + 1
           #--------------------------------------------------------------------------------------------------------
           end      = time.time()
           thisTime = (end-start)
           cumulativeTime+=thisTime
           #--------------------------------------------------------------------------------------------------------
           print(bcolors.OKGREEN,"loaded %u/%u @ %0.2f secs / Total %0.2f secs" % (modelNumber,endModel,thisTime,cumulativeTime),bcolors.ENDC)
    #-----------------------------------------------------------------------------------------------------------------------------------------
    print("Merging ",len(allModelList)," Models..")
    if (len(allModelList)>1):
        out = concatenate(allModelList[:],name=outputLabel)
    elif (len(allModelList)==1):
        #One model does not need concatenations
        out = allModelList[0]
    else:
        print("combineModels: No Model found!")
        raise ValueError('combineModels: No Model found.')

    mergedModel = Model(inputs=rawInput, outputs=out, name=configuration['OutputDirectory'])
    mergedModel.compile(optimizer=activeOptimizer,loss='mse',metrics=['mae', 'acc']) #,jit_compile=True #<- this may cause trouble on non-XLA builds?

    print("Merged Model summary : ")
    mergedModel.summary() 

    if (doPNGPlot):
     try:
      tf.keras.utils.plot_model(mergedModel, expand_nested=True)
     except:
      print("Please install pydot for network graph plot!")
      os.system('touch model.png') #<- just make a foo png  

    print("Done merging model and saved it to disk")
    return mergedModel,includedOutputs,selectedColumns
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def combineAsSingleModel(path,inputFilename,modelInputSize,modelOutputSize,outputFilename,label):
    inputLabel  = "input_%s" % label
    outputLabel = "result_%s" % label

    #We basically want to rename a single model the same way as if we had combined it with others..
    loaded_model = loadModel(path,"%s/model" % inputFilename)
    
    print("Using TensorFlow ",tf.__version__)
    print("Original Single Model was : ")
    loaded_model.summary() 

    doSimpleWay=0
    
    if (doSimpleWay):
       loaded_model.layers[0].name=inputLabel
       loaded_model.layers[-1].name=outputLabel
       loaded_model.compile(optimizer='rmsprop', loss='mse', metrics=['mae', 'acc']) 
       saveModel("%s/"%path,loaded_model) 
       print("Merged Single/Combined Model summary : ")
       loaded_model.summary() 
    else:
       dummyNetwork = newTrivialModel("DummyPadding",666,modelInputSize,modelOutputSize)

       print("Single model input will have ",modelInputSize," size")
       singleInput = Input(shape=(modelInputSize,), name=inputLabel) 
       outModelList = list() # empty list   
       outModelList.append(loaded_model(singleInput))
       outModelList.append(dummyNetwork(singleInput))
       out = concatenate(outModelList[:],name=outputLabel) 
       mergedModel = Model(singleInput,out)
       mergedModel.compile(optimizer='rmsprop', loss='mse', metrics=['mae', 'acc']) 
       saveModel("%s/"%path,mergedModel) 
       print("Merged Single/Combined Model summary : ")
       mergedModel.summary() 

    os.system('mv %s/model.h5 %s/%s.h5'%(path,path,outputFilename))
    os.system('mv %s/model.json %s/%s.json'%(path,path,outputFilename))
    return mergedModel
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def visualizeLayer(layerNumber,layerSize,hasSkip=False,dropout=0.3):
        print("%02u"%layerNumber,end="")
        if (hasSkip) :
           print(" HAS SKIP ",end="")
        else:
           print("          ",end="")
        for p in range(0,int(layerSize/10)):
            print("█",end="")
        print(layerSize," Dropout= %0.2f"%dropout) 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def autobuilderBB(inputSize,outputSize,modelName="mnet",outputArrayIndex=0,depth=12,lambdaF=1.0,skip=True,dropOutStart=0.3,dropoutStopNLayersBeforeEnd=4):
    print("Auto Builder for ",label," Input ",inputSize," and Output ",outputSize)
    print("Depth=",depth," λ=",lambdaF," skip=",skip)
    step = int((inputSize*lambdaF)/depth)
    dropout  = dropOutStart
    dropStep = dropOutStart/depth
    currentSize = inputSize
    for layer in range(0,depth):
        if (dropout<0.01):
            dropout=0
        elif (layer <= depth-dropoutStopNLayersBeforeEnd):
            dropout=0

        visualizeLayer(layer,currentSize,skip!=0,dropout=dropout)
        dropout = dropout - dropStep
        currentSize = currentSize - step
    
    currentSize = outputSize
    visualizeLayer(depth,currentSize,dropout=0) 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def autobuilder(inputSize,outputSize,modelName="mnet",outputArrayIndex=0,depth=12,lambdaF=1.0,skip=True,dropOutStart=0.3,dropoutStopNLayersBeforeEnd=4,probabilisticMode=0,quantize=False,optimizer="rmsprop"):
    #------------------------------------------------------------------------------------
    #------------------------------------------------------------------------------------
    print("Auto Builder for ",modelName," Input ",inputSize," and Output ",outputSize)
    print("Depth=",depth," λ=",lambdaF," skip=",skip)
    modelName = tensorflowFriendlyModelName(modelName)
    print("Model renamed to ",modelName," to make sure it doesn't call tensorflow problems ")
    #------------------------------------------------------------------------------------
    #------------------------------------------------------------------------------------
    inputs      = Input(shape=(inputSize,))
    act         = theActivationMethod
    kinit       = initializer
    layers      = list()
    layerSizes  = list()
    layerNumber = int()
    totalNumberOfLayers = 0
    #------------------------------------------------------------------------------------
    step           = int((inputSize*lambdaF)/depth)
    stepDecay      = True
    dropout        = dropOutStart
    dropStep       = dropOutStart/depth
    currentSize    = inputSize
    previousSize   = inputSize
    previousLayer  = inputs
    stop           = False
    #-----------------------------------------------------------------------------------------------------------------------------------
    addLayers      = list()
    skipLayers     = list()
    print("Will now try to add skip layers to ",totalNumberOfLayers," layers")
    #-----------------------------------------------------------------------------------------------------------------------------------
    for layerNumber in range(0,depth):
        if (currentSize<=outputSize):
            stop        = True
            currentSize = outputSize
            print("λ compression is too aggressive network max depth will be ",layerNumber)
        if (dropout<0.01):
            dropout=0
        visualizeLayer(layerNumber,currentSize,skip,dropout=dropout)
        #------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        layers.append(Dense(int(currentSize),input_shape=(previousSize,), kernel_initializer=kinit, activation=act, name=tag("layer",modelName,outputArrayIndex,layerNumber))(previousLayer))
        layerSizes.append(int(currentSize)) 
        previousLayer = layers[len(layers)-1]
        if (dropout!=0) and (layerNumber <= depth-dropoutStopNLayersBeforeEnd):
           Dropout(dropout,name=tag("dropout",modelName,outputArrayIndex,layerNumber))(layers[len(layers)-1])
        #------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        if (skip):
          for fromLayerMinusOne in range(0,layerNumber):
             toSize  = layerSizes[len(layerSizes)-1]
             toLayer = len(layerSizes)-1
             if (fromLayerMinusOne==0):
                fromSize = inputSize
                skipLayers.append(Dense(int(toSize),input_shape=(inputSize,),kernel_initializer=kinit, activation=act, name=tag("skip_in_to_%u"%(toLayer),modelName,outputArrayIndex,fromLayerMinusOne))(inputs))   
             elif (fromLayerMinusOne>0):
                fromLayer = fromLayerMinusOne-1
                fromSize = layerSizes[fromLayer]
                skipLayers.append(Dense(int(toSize),kernel_initializer=kinit, activation=act, name=tag("skip_%u_to_%u"%(fromLayer,toLayer),modelName,outputArrayIndex,fromLayer))(layers[fromLayer]))
             #-----------------------------------------------------------------------------------------------------   
             if (dropout!=0) and (layerNumber<= depth - dropoutStopNLayersBeforeEnd):
                Dropout(dropout,name=tag("skip_dropout_to_%u"%(toLayer),modelName,outputArrayIndex,layerNumber))(layers[len(layers)-1])
             #-----------------------------------------------------------------------------------------------------   
             #print("Add Skip ",fromLayer,"->",toLayer)
             layers[toLayer] = Add(name=tag("add_%u_to_%u"%(fromLayerMinusOne,toLayer),modelName,outputArrayIndex,fromLayerMinusOne))([skipLayers[len(skipLayers)-1],layers[toLayer]])
        #------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        dropout             = dropout - dropStep
        currentSize         = currentSize - step
        if (currentSize < 30):
              dropout = 0
        if (stepDecay): 
              step = max(depth , int(3/6 * step))
        totalNumberOfLayers = totalNumberOfLayers + 1 
        if (stop):
           break
    #-----------------------------------------------------------------------------------------------------------------------------------
    currentSize = outputSize
    visualizeLayer(depth,currentSize,dropout=0)
    #-----------------------------------------------------------------------------------------------------------------------------------
    predictions = tf.keras.layers.Dense(int(outputSize),kernel_initializer='normal', activation='linear', name='output_%u_%s'%(outputArrayIndex,modelName))(layers[len(layers)-1])
    #-----------------------------------------------------------------------------------------------------------------------------------

    # the Input layer and three Dense layers
    model = Model(name=modelName, inputs=inputs, outputs=predictions) 
    #-------------------------------------------------------------------------------------------------------
    if (optimizer=="adam"):
       activeOptimizer=tf.keras.optimizers.Adam(learning_rate=learningRate,epsilon=tf.keras.backend.epsilon())
    else:
       activeOptimizer=tf.keras.optimizers.RMSprop(learning_rate=learningRate, rho=0.9, epsilon=tf.keras.backend.epsilon()) # epsilon=1e-6, lr=0.00025 is the old value
    #-------------------------------------------------------------------------------------------------------
 

    #-----------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------
    if (quantize):
       #Careful not all activations are quantization aware
       print("Will attempt to quantize model..!")
       try:
         import tensorflow_model_optimization as tfmot
         quantize_model = tfmot.quantization.keras.quantize_model
         model = quantize_model(model) 
       except:
         print("Could not quantize model, you are using a non quantization aware activation function..!")
         print("https://github.com/tensorflow/model-optimization/blob/master/tensorflow_model_optimization/python/core/quantization/keras/quantize_aware_activation.py")
    #-----------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------
    if (probabilisticMode):
        model.compile(optimizer=activeOptimizer, loss='categorical_crossentropy', metrics=['accuracy'])
    elif (useQuadMetric): 
        model.compile(optimizer=activeOptimizer, loss=mean_quad_error, metrics=['mae'] ) #Penalize really bad output..
    elif (useSquaredMetric):
        model.compile(optimizer=activeOptimizer, loss='mse', metrics=['mae'])
    elif (useModuloMetric):
        model.compile(optimizer=activeOptimizer, loss=mean_squared_error_modulo_360, metrics=[average_error_modulo_360] ) #With 360  metrics=[rmse_360]
    else:
        model.compile(optimizer=activeOptimizer, loss='mae', metrics=['mae']) #Without 360  
    #-----------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------
    #-----------------------------------------------------------------------------------
    model.summary()
    return model
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
def newEncoderModelSelector(configuration,modelName,outputArrayIndex,inputDimension,nonNSDMInputSize,numberOfChannelsPerNSDMElement,outputSize,networkCompression,probabilisticMode=0,quantize=False,optimizer="rmsprop"):
    if ('autobuilder' in configuration) and (configuration['autobuilder']==1):
      depth   = int(configuration['neuralNetworkDepth'])
      lambdaF = float(configuration['lamda'])
      skip    = (int(configuration['skipConnections']) == 1)
      dropout = float(configuration['dropoutRate'])
      return autobuilder(
                         inputSize=inputDimension,
                         outputSize=outputSize,
                         modelName=modelName,
                         outputArrayIndex=outputArrayIndex,
                         depth=depth,
                         lambdaF=lambdaF,
                         skip=skip,
                         dropOutStart=dropout,
                         probabilisticMode=probabilisticMode,
                         quantize=quantize,
                         optimizer=optimizer
                        )
    else:
      return newDeepRotModel(
                             configuration,
                             modelName,
                             outputArrayIndex,
                             inputDimension,
                             nonNSDMInputSize,
                             numberOfChannelsPerNSDMElement,
                             outputSize,
                             networkCompression,
                             probabilisticMode=probabilisticMode,
                             quantize=quantize,
                             optimizer=optimizer
                            )
#    return convDeepRotModel(modelName,outputArrayIndex,inputDimension,nonNSDMInputSize,numberOfChannelsPerNSDMElement,outputSize,networkCompression,probabilisticMode=probabilisticMode,quantize=quantize)
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
#=================================================================================================================================================================================== 
if __name__ == '__main__':
    print("DNNModel.py is a library and cannot be run on it's own")
    #------------
    inputSize  = 160
    outputSize = 1
    label      = "mnet"
    depth      = 12
    lambdaF    = 1.5
    skip       = True

    if (len(sys.argv)<=1):
       print("Please supply arguments like : ")  #33 inputs are 2D 
       print(bcolors.OKGREEN,"python3 DNNModel.py --inputs 160 --lambda 1.0 --depth 12 --outputs 1 ",bcolors.ENDC)

    if (len(sys.argv)>1):
       print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)): 
           if (sys.argv[i]=="--inputs"):
              inputSize =int(sys.argv[i+1])
           if (sys.argv[i]=="--lambda"):
              lambdaF =float(sys.argv[i+1])  
           if (sys.argv[i]=="--depth"):
              depth =int(sys.argv[i+1])  
           if (sys.argv[i]=="--outputs"):
              outputSize =int(sys.argv[i+1])

    model = autobuilder(inputSize,outputSize,modelName=label,depth=depth,lambdaF=lambdaF,skip=skip)
    try:
      tf.keras.utils.plot_model(model,show_shapes=False,rankdir='LR',expand_nested=True)
    except:
      print("Please install pydot for network graph plot!")
      os.system('touch model.png') #<- just make a foo png  
    #------------
    
