#!/usr/bin/python3
""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import sys
import os
import numpy as np

#python3 principleComponentAnalysisTester.py --config dataset/body_configuration.json --mem 1000 --highlight 4 --mode 3
#python3 principleComponentAnalysisTool.py --config dataset/body_configuration.json --all body --mem 1000 --show --type sparsepca
#python3 principleComponentAnalysisTool.py --config dataset/upperbody_configuration.json --all upperbody --mem 1000  --show --type factoranalysis

class EmptyDecomposition:
  def __init__(self,):
               self.decompositionType = ""
  def ok(self):
               return True
  def fit(self,data):
               pass
  def transform(self,data,selectedPCADimensions=0):
               return data
  def save(self,filename):
               return True
  def load(self,filename):
               return True


#------------------------------------------------------------------
#------------------------------------------------------------------
#------------------------------------------------------------------
def getSKPCA_Decomposer(numberOfDimensions=20):
     from sklearn.decomposition import PCA 
     engine = PCA(n_components=numberOfDimensions)
     return engine

def getTruncatedSVD_Decomposer(numberOfDimensions=20):
     from sklearn.decomposition import TruncatedSVD
     engine = TruncatedSVD(n_components=numberOfDimensions)
     return engine

def getLatentDirichletAllocation_Decomposer(numberOfDimensions=20):
     from sklearn.decomposition import LatentDirichletAllocation
     engine = LatentDirichletAllocation(n_components=numberOfDimensions)
     return engine

def getFactorAnalysis_Decomposer(numberOfDimensions=20):
     from sklearn.decomposition import FactorAnalysis
     engine = FactorAnalysis(n_components=numberOfDimensions)
     return engine

def getDictionaryLearning_Decomposer(numberOfDimensions=20):
     from sklearn.decomposition import DictionaryLearning
     engine = DictionaryLearning(n_components=numberOfDimensions)
     return engine

def getFastICA_Decomposer(numberOfDimensions=20):
     from sklearn.decomposition import FastICA
     engine = FastICA(n_components=numberOfDimensions)
     return engine

def getNMF_Decomposer(numberOfDimensions=20):
     from sklearn.decomposition import NMF
     engine = NMF(n_components=numberOfDimensions)
     return engine

def getIncrementalPCA_Decomposer(numberOfDimensions=20):
     from sklearn.decomposition import IncrementalPCA
     engine = IncrementalPCA(n_components=numberOfDimensions)
     return engine

def getSparsePCA_Decomposer(numberOfDimensions=20):
     from sklearn.decomposition import SparsePCA
     engine = SparsePCA(n_components=numberOfDimensions)
     return engine

def getTSNE_Decomposer(numberOfDimensions=20):
     from sklearn.manifold import TSNE
     engine = TSNE(n_components=numberOfDimensions,perplexity=100, learning_rate=100, verbose=2, n_iter=350)
     return engine
#------------------------------------------------------------------
#------------------------------------------------------------------
#------------------------------------------------------------------
class Decomposition():
  def __init__(self,
               inputData:np.array=np.array([]),
               savedFile:str="",
               decompositionType:str="pca",
               selectedPCADimensions:int=0
              ):
               self.disabledDecomposition   = False
               self.numberOfSamplesFittedOn = 0
               self.decompositionType       = decompositionType
               self.noTransformFunction     = 0 #Some methods dont have a transform function
               self.trackedFiles            = list() #<- all needed files should be tracked here
              
               if (selectedPCADimensions==0):
                 self.numberOfSamplesFittedOn = inputData.shape[0]
               else:
                 self.numberOfSamplesFittedOn = selectedPCADimensions


               import sklearn
               print("Decomposition code using Sk-Learn : ",sklearn.__version__)
               print("Will try to perform : ",self.decompositionType)

               #self.engine = getSKPCA_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               from sklearn.preprocessing import StandardScaler
               self.scaler = StandardScaler()

               if (self.decompositionType==""):
                          print("Not Performing any decomposition")
                          self.disabledDecomposition = True
                          return;
               elif (self.decompositionType=="skpca"):
                          self.engine = getSKPCA_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               elif (self.decompositionType=="sparsepca"):
                          self.engine = getSparsePCA_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               elif (self.decompositionType=="incrementalpca"):
                          self.engine = getIncrementalPCA_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               elif (self.decompositionType=="fastica"):
                          self.engine = getFastICA_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               elif (self.decompositionType=="nmf"):
                          self.engine = getNMF_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               elif (self.decompositionType=="dictionary"):
                          self.engine = getDictionaryLearning_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               elif (self.decompositionType=="factoranalysis"):
                          self.engine = getFactorAnalysis_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               elif (self.decompositionType=="dirichlet"):
                          self.engine = getLatentDirichletAllocation_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               elif (self.decompositionType=="svd"):
                          self.engine = getTruncatedSVD_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
               elif (self.decompositionType=="tsne"):
                          self.engine = getTSNE_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)
                          self.noTransformFunction = 1
               elif (self.decompositionType!="pca"):
                          print("Could not understand decomposition Type ",self.decompositionType)
                          sys.exit(1)
               else:
                          print("Using SK code for decomposition Type ",self.decompositionType)
                          self.engine = getSKPCA_Decomposer(numberOfDimensions=self.numberOfSamplesFittedOn)

               if (savedFile!=""):
                  print("Loading existing fitted decomposition from ",savedFile,"..!")
                  self.load(savedFile) 
               elif (inputData.size!=0):
                  print("Generating new fit..!")
                  self.fit(inputData)
               else:
                  print("No Decomposition input given..!")

  def ok(self):
    return self.numberOfSamplesFittedOn!=0

  def getNumberOfExpectedSamples(self):
    return self.numberOfSamplesFittedOn

  def fit(self,data):
    import time
    startAt = time.time()
    #----------------------------------------------------------------------------------------
    self.numberOfSamplesFittedOn = data.shape[0]
    self.scaler.fit(data)
    dataTransformed = self.scaler.transform(data)
    outputData = self.engine.fit_transform(dataTransformed)
    #----------------------------------------------------------------------------------------
    endAt = time.time()
    print("Time required to fit ",self.decompositionType," was ",(endAt-startAt)/60," mins")
    return outputData

  def transform(self,data,selectedPCADimensions=0):
    if (self.disabledDecomposition):
       return data
    #if (selectedPCADimensions!=self.numberOfSamplesFittedOn):
    #    print("INCONSISTENT TRANSFORM")

    if (selectedPCADimensions==0):
      if (self.noTransformFunction):
         dataTransformed = self.scaler.fit_transform(data)
         return self.engine.fit_transform(dataTransformed)
      else:
         dataTransformed = self.scaler.transform(data)
         return self.engine.transform(dataTransformed) 
    else:
      if (self.noTransformFunction):
         dataTransformed = self.scaler.fit_transform(data)
         return self.engine.fit_transform(dataTransformed)[:,:selectedPCADimensions] # ,n_components=selectedPCADimensions
      else:
         dataTransformed = self.scaler.transform(data)
         return self.engine.transform(dataTransformed)[:,:selectedPCADimensions] # ,n_components=selectedPCADimensions
      
  def save(self,filename):
       print("Saving Decomposition to ",filename) 
       OKGREEN = '\033[92m'
       FAIL    = '\033[91m'
       ENDC    = '\033[0m'
       import pickle
       self.trackedFiles = list()
       successfulSaves = 0
       #---------------------------------------------------------------
       pathToEngine = "%s.%s" % (filename,self.decompositionType) 
       with open(pathToEngine,'wb') as engineFile:
          self.trackedFiles.append(pathToEngine)
          print(OKGREEN,"Saving Decomposition Engine to ",pathToEngine,ENDC) 
          pickle.dump(self.engine,engineFile)
          successfulSaves = successfulSaves + 1
       #---------------------------------------------------------------
       pathToScaler = "%s.scaler.%s" % (filename,self.decompositionType)
       with open(pathToScaler,'wb') as scalerFile:
          self.trackedFiles.append(pathToScaler)
          print(OKGREEN,"Saving Decomposition Scaler to ",pathToScaler,ENDC) 
          pickle.dump(self.scaler,scalerFile)
          successfulSaves = successfulSaves + 1
       #---------------------------------------------------------------
       return (successfulSaves==2)

  def load(self,filename):
       print("Loading Decomposition from ",filename)
       OKGREEN = '\033[92m'
       FAIL    = '\033[91m'
       ENDC    = '\033[0m'
       import pickle
       self.trackedFiles = list()
       successfulLoads = 0
       #---------------------------------------------------------------
       pathToEngine = "%s.%s" % (filename,self.decompositionType) 
       if (os.path.isfile(pathToEngine)):
          self.trackedFiles.append(pathToEngine)
          unpickleEngine = open(pathToEngine, 'rb')
          print(OKGREEN,"Loaded Decomposition Engine from ",pathToEngine,ENDC) 
          self.engine = pickle.load(unpickleEngine) #, encoding='bytes'
          successfulLoads = successfulLoads + 1
       else:
          print(FAIL,"Failed Loading Decomposition Engine from ",pathToEngine,ENDC) 
       #---------------------------------------------------------------           
       pathToScaler = "%s.scaler.%s" % (filename,self.decompositionType)
       if (os.path.isfile(pathToEngine)):
          self.trackedFiles.append(pathToScaler)
          unpickleScaler = open(pathToScaler, 'rb')
          print(OKGREEN,"Loading Decomposition Scaler from ",pathToScaler,ENDC) 
          self.scaler = pickle.load(unpickleScaler) #, encoding='bytes'
          successfulLoads = successfulLoads + 1
       else:
          print(FAIL,"Failed Loading Decomposition Scaler from ",pathToScaler,ENDC)
       #---------------------------------------------------------------
       return (successfulLoads==2)

  def visualize(self,data,saveToFile="",onlyScreePlotNDimensions=0,label="PCA",colors=list(),colorLabel="Highlighting PC-4",viewAzimuth=45,viewElevation=45,showScree=0): 
    import matplotlib
    import matplotlib.pyplot as plt
    font = {'family' : 'normal',
            'weight' : 'bold',
            'size'   : 28 }
    plt.rc('font', **font)
    plt.rc('xtick', labelsize=15) 
    plt.rc('ytick', labelsize=15) 
    # === Plot =========================================================================
    fig = plt.figure()
    fig.set_size_inches(19.2, 10.8, forward=True)
    ax1 = fig.add_subplot(1, 1, 1,projection='3d')
    #=================================================================================== 

    #Number of PCA components to plot on first plot (our plot is 3D so max is 4 if we dont have a color ..! )
    keepNDimensions = 3
    if (len(colors)==0):
       keepNDimensions = 4

    #Do transform of our input using the PCA dimensions as new basis
    #=================================================================================== 
    transformedData = self.transform(data,selectedPCADimensions=keepNDimensions).real
    #print("TRANSFORMED DATA : ",transformedData)
    print("TRANSFORMED DATA")
    print("Number of Samples    : ",transformedData.shape[0])
    print("Number of Dimensions : ",transformedData.shape[1])
    #=================================================================================== 

    if (len(colors)==0):
       if (transformedData.shape[1]<4):
         print("Not enough dimensions to plot color")
         keepNDimensions = 3
       else:
         colors = transformedData[:,3]
         colorLabel = "highlighting PC-4"
    else:
       print("Using provided set of colorValues")
       keepNDimensions = 3

    ax1.view_init(viewAzimuth,viewElevation) 
    #===================================================================================
    if (len(colors)==0):
      ax1.scatter(transformedData[:,0],transformedData[:,1],transformedData[:,2],c='red')
    else:
      scatter = ax1.scatter(transformedData[:,0],transformedData[:,1],transformedData[:,2],c=colors,label=colors)
      ax1.legend(*scatter.legend_elements(num=5),loc="upper right", title="Values", fontsize="x-small", framealpha=0.4)
    #=================================================================================== 

    # Adding title, xlabel and ylabel
    ax1.set_title('%s %s %s '%(self.decompositionType,label,colorLabel)) # Title of the plot
    ax1.set_xlabel('PC-1',labelpad=30) # X-Label
    ax1.set_ylabel('PC-2',labelpad=30) # Y-Label
    ax1.set_zlabel('PC-3',labelpad=30) # Z-Label
    #ax1.tick_params(axis='x', pad=5) #fine tune numbers of plot
    plt.subplots_adjust(left=0.05, right=0.95, top=0.95, bottom=0.08)

    if (saveToFile!=""):
      print("Saving Figure to output file %s" % saveToFile)
      fig.savefig(saveToFile)
    else:
      plt.show() 


if __name__ == '__main__':
  print("principleComponentAnalysis.py is a library")
  decomp = Decomposition(savedFile='dataset/combinedModel/mocapnet4/mode1/1.0/step1_upperbody_all/upperbody_all.pca',decompositionType='pca')
  sample = list()
  sample.append(0.470052) #0
  sample.append(0.452315) #1
  sample.append(1.000000) #2
  sample.append(0.479688) #3
  sample.append(0.239352) #4
  sample.append(1.000000) #5
  sample.append(0.520312) #6
  sample.append(0.185185) #7
  sample.append(1.000000) #8
  sample.append(0.519271) #9
  sample.append(0.173148) #10
  sample.append(1.000000) #11
  sample.append(0.000000) #12
  sample.append(0.000000) #13
  sample.append(0.000000) #14
  sample.append(0.450000) #15
  sample.append(0.238889) #16
  sample.append(1.000000) #17
  sample.append(0.422917) #18
  sample.append(0.337037) #19
  sample.append(1.000000) #20
  sample.append(0.000000) #21
  sample.append(0.000000) #22
  sample.append(0.000000) #23
  sample.append(0.509375) #24
  sample.append(0.239815) #25
  sample.append(1.000000) #26
  sample.append(0.500000) #27
  sample.append(0.362963) #28
  sample.append(1.000000) #29
  sample.append(0.000000) #30
  sample.append(0.000000) #31
  sample.append(0.000000) #32
  sample.append(0.652332) #33
  sample.append(0.283472) #34
  sample.append(0.547378) #35
  sample.append(0.213181) #36
  sample.append(0.536087) #37
  sample.append(0.077135) #38
  sample.append(0.214366) #39
  sample.append(0.509478) #40
  sample.append(0.095500) #41
  sample.append(0.029691) #42
  sample.append(0.167750) #43
  sample.append(0.522894) #44
  sample.append(0.141564) #45
  sample.append(0.065052) #46
  sample.append(0.050908) #47
  sample.append(0.124542) #48
  sample.append(0.540789) #49
  sample.append(0.190115) #50
  sample.append(0.112984) #51
  sample.append(0.101816) #52
  sample.append(0.050908) #53
  sample.append(0.383942) #54
  sample.append(0.270394) #55
  sample.append(0.307847) #56
  sample.append(0.277424) #57
  sample.append(0.248705) #58
  sample.append(0.254739) #59
  sample.append(0.270394) #60
  sample.append(0.652332) #61
  sample.append(0.000000) #62
  sample.append(0.547378) #63
  sample.append(0.536087) #64
  sample.append(0.509478) #65
  sample.append(0.522894) #66
  sample.append(0.540789) #67
  sample.append(0.270394) #68
  sample.append(0.212132) #69
  sample.append(0.440391) #70
  sample.append(0.248585) #71
  sample.append(0.182055) #72
  sample.append(0.155387) #73
  sample.append(0.126341) #74
  sample.append(0.113851) #75
  sample.append(0.174184) #76
  sample.append(0.440391) #77
  sample.append(0.212132) #78
  sample.append(0.678899) #79
  sample.append(0.152507) #80
  sample.append(0.143071) #81
  sample.append(0.171002) #82
  sample.append(0.175392) #83
  sample.append(0.193561) #84
  sample.append(0.419609) #85
  sample.append(0.678899) #86
  sample.append(0.300000) #87
  sample.append(0.216108) #88
  sample.append(0.563004) #89
  sample.append(0.067397) #90
  sample.append(0.029691) #91
  sample.append(0.059382) #92
  sample.append(0.087379) #93
  sample.append(0.130105) #94
  sample.append(0.306329) #95
  sample.append(0.563004) #96
  sample.append(0.209537) #97
  sample.append(0.116016) #98
  sample.append(0.154849) #99
  sample.append(0.587831) #100
  sample.append(0.129067) #101
  sample.append(0.066885) #102
  sample.append(0.083048) #103
  sample.append(0.069538) #104
  sample.append(0.089203) #105
  sample.append(0.321928) #106
  sample.append(0.587831) #107
  sample.append(0.192741) #108
  sample.append(0.107269) #109
  sample.append(0.061752) #110
  sample.append(0.094237) #111
  sample.append(0.617853) #112
  sample.append(0.190791) #113
  sample.append(0.125269) #114
  sample.append(0.133770) #115
  sample.append(0.098298) #116
  sample.append(0.081326) #117
  sample.append(0.347944) #118
  sample.append(0.617853) #119
  sample.append(0.194984) #120
  sample.append(0.131074) #121
  sample.append(0.123504) #122
  sample.append(0.061752) #123
  sample.append(0.348961) #124
  sample.append(0.308926) #125
  sample.append(0.269400) #126
  sample.append(0.236866) #127
  sample.append(0.208076) #128
  sample.append(0.214721) #129
  sample.append(0.232589) #130
  sample.append(0.040663) #131
  sample.append(0.308926) #132
  sample.append(0.143338) #133
  sample.append(0.378955) #134
  sample.append(0.265854) #135
  sample.append(0.281502) #136
  sample.append(0.308926) #137
  sample.append(0.652332) #138
  sample.append(0.000000) #139
  sample.append(0.547378) #140
  sample.append(0.536087) #141
  sample.append(0.509478) #142
  sample.append(0.522894) #143
  sample.append(0.540789) #144
  sample.append(0.270394) #145
  sample.append(0.000000) #146
  sample.append(0.440391) #147
  sample.append(0.678899) #148
  sample.append(0.563004) #149
  sample.append(0.587831) #150
  sample.append(0.617853) #151
  sample.append(0.308926) #152
  sample.append(0.106590) #153
  sample.append(0.587454) #154
  sample.append(0.178302) #155
  sample.append(0.106590) #156
  sample.append(0.109798) #157
  sample.append(0.069458) #158
  sample.append(0.052693) #159
  sample.append(0.317531) #160
  sample.append(0.587454) #161
  sample.append(0.166465) #162
  sample.append(0.146195) #163
  sample.append(0.111492) #164
  sample.append(0.053520) #165
  sample.append(0.030413) #166
  sample.append(0.278528) #167
  sample.append(0.587454) #168
  sample.append(3.096379) #169
  sample.append(0.651328) #170
  sample.append(5.955375) #171
  sample.append(6.084674) #172
  sample.append(-0.059619) #173
  sample.append(0.048326) #174
  sample.append(0.234845) #175
  sample.append(0.585668) #176
  sample.append(0.651328) #177
  sample.append(0.686142) #178
  sample.append(5.398531) #179
  sample.append(5.946909) #180
  sample.append(5.904308) #181
  sample.append(5.806487) #182
  sample.append(0.529019) #183
  sample.append(0.651328) #184
  sample.append(6.084674) #185
  sample.append(3.792921) #186
  sample.append(3.694669) #187
  sample.append(4.237240) #188
  sample.append(4.096264) #189
  sample.append(4.071058) #190
  sample.append(3.975877) #191
  sample.append(3.886222) #192
  sample.append(3.886222) #193
  sample.append(0.000000) #194
  sample.append(3.776154) #195
  sample.append(4.110508) #196
  sample.append(4.119073) #197
  sample.append(4.020748) #198
  sample.append(3.931176) #199
  sample.append(3.931177) #200
  sample.append(0.000000) #201
  sample.append(3.929643) #202
  sample.append(2.813782) #203
  sample.append(1.095648) #204
  sample.append(3.325809) #205
  sample.append(2.449419) #206
  sample.append(2.176756) #207
  sample.append(2.363436) #208
  sample.append(2.456802) #209
  sample.append(1.402459) #210
  sample.append(1.095648) #211
  sample.append(2.003560) #212
  sample.append(3.639651) #213
  sample.append(2.840933) #214
  sample.append(2.875062) #215
  sample.append(2.887117) #216
  sample.append(1.448436) #217
  sample.append(1.095648) #218
  sample.append(2.736625) #219
  sample.append(2.943081) #220
  sample.append(0.954672) #221
  sample.append(5.591012) #222
  sample.append(3.255518) #223
  sample.append(1.401905) #224
  sample.append(2.261431) #225
  sample.append(2.461843) #226
  sample.append(1.159315) #227
  sample.append(0.954672) #228
  sample.append(1.819818) #229
  sample.append(4.163903) #230
  sample.append(4.543498) #231
  sample.append(3.371371) #232
  sample.append(3.151165) #233
  sample.append(1.170683) #234
  sample.append(0.954672) #235
  sample.append(2.943081) #236
  sample.append(3.081973) #237
  sample.append(0.929466) #238
  sample.append(5.318349) #239
  sample.append(4.543498) #240
  sample.append(3.256703) #241
  sample.append(2.719052) #242
  sample.append(2.719052) #243
  sample.append(1.130633) #244
  sample.append(0.929466) #245
  sample.append(1.897446) #246
  sample.append(4.228285) #247
  sample.append(4.543498) #248
  sample.append(3.707124) #249
  sample.append(3.371372) #250
  sample.append(1.137976) #251
  sample.append(0.929466) #252
  sample.append(3.216783) #253
  sample.append(3.189919) #254
  sample.append(0.834284) #255
  sample.append(5.505028) #256
  sample.append(5.403023) #257
  sample.append(5.860644) #258
  sample.append(3.256703) #259
  sample.append(2.719052) #260
  sample.append(0.929466) #261
  sample.append(0.834284) #262
  sample.append(1.597937) #263
  sample.append(4.522202) #264
  sample.append(5.142685) #265
  sample.append(4.364796) #266
  sample.append(3.691174) #267
  sample.append(0.898620) #268
  sample.append(0.834284) #269
  sample.append(3.574276) #270
  sample.append(3.376438) #271
  sample.append(0.744629) #272
  sample.append(5.598395) #273
  sample.append(5.603435) #274
  sample.append(5.860644) #275
  sample.append(5.860644) #276
  sample.append(3.166879) #277
  sample.append(0.744629) #278
  sample.append(0.744629) #279
  sample.append(1.183479) #280
  sample.append(4.781023) #281
  sample.append(5.403024) #282
  sample.append(4.970201) #283
  sample.append(4.234641) #284
  sample.append(0.684905) #285
  sample.append(0.744629) #286
  sample.append(4.391370) #287
  sample.append(3.727261) #288
  sample.append(0.744629) #289
  sample.append(4.544052) #290
  sample.append(4.300908) #291
  sample.append(4.272226) #292
  sample.append(4.071058) #293
  sample.append(3.886222) #294
  sample.append(3.166879) #295
  sample.append(0.744629) #296
  sample.append(3.604797) #297
  sample.append(4.254323) #298
  sample.append(4.324193) #299
  sample.append(4.133639) #300
  sample.append(3.966107) #301
  sample.append(4.234641) #302
  sample.append(0.744629) #303
  sample.append(3.966615) #304
  sample.append(3.792921) #305
  sample.append(0.000000) #306
  sample.append(4.237240) #307
  sample.append(4.096264) #308
  sample.append(4.071058) #309
  sample.append(3.975877) #310
  sample.append(3.886222) #311
  sample.append(3.886222) #312
  sample.append(3.694669) #313
  sample.append(3.776154) #314
  sample.append(4.110508) #315
  sample.append(4.119073) #316
  sample.append(4.020748) #317
  sample.append(3.931176) #318
  sample.append(3.931177) #319
  sample.append(0.000000) #320
  sample.append(3.929643) #321
  sample.append(3.827735) #322
  sample.append(0.634561) #323
  sample.append(5.145152) #324
  sample.append(4.961411) #325
  sample.append(5.039039) #326
  sample.append(4.739530) #327
  sample.append(4.325072) #328
  sample.append(0.463204) #329
  sample.append(0.634561) #330
  sample.append(0.000000) #331
  sample.append(4.613133) #332
  sample.append(4.903871) #333
  sample.append(4.607087) #334
  sample.append(4.287396) #335
  sample.append(0.295320) #336
  sample.append(0.634561) #337
  sample.append(4.346044) #338
  sample.append(2.256939) #339
  sample.append(0.968915) #340
  sample.append(0.498058) #341
  sample.append(1.022310) #342
  sample.append(1.086692) #343
  sample.append(1.380610) #344
  sample.append(1.639430) #345
  sample.append(1.112731) #346
  sample.append(0.968915) #347
  sample.append(1.471541) #348
  sample.append(0.000000) #349
  sample.append(0.927337) #350
  sample.append(1.482404) #351
  sample.append(1.967686) #352
  sample.append(1.114843) #353
  sample.append(0.968915) #354
  sample.append(1.776775) #355
  sample.append(2.805316) #356
  sample.append(0.977481) #357
  sample.append(5.982525) #358
  sample.append(1.401905) #359
  sample.append(1.401905) #360
  sample.append(2.001093) #361
  sample.append(2.261431) #362
  sample.append(1.182600) #363
  sample.append(0.977481) #364
  sample.append(1.762278) #365
  sample.append(4.068930) #366
  sample.append(3.258445) #367
  sample.append(2.912313) #368
  sample.append(2.912314) #369
  sample.append(1.196280) #370
  sample.append(0.977481) #371
  sample.append(2.673644) #372
  sample.append(2.762715) #373
  sample.append(0.879156) #374
  sample.append(6.016655) #375
  sample.append(0.229778) #376
  sample.append(0.565532) #377
  sample.append(1.223204) #378
  sample.append(1.828609) #379
  sample.append(0.992046) #380
  sample.append(0.879156) #381
  sample.append(1.465495) #382
  sample.append(4.623997) #383
  sample.append(6.053906) #384
  sample.append(3.258445) #385
  sample.append(2.912314) #386
  sample.append(0.977481) #387
  sample.append(0.879156) #388
  sample.append(2.397369) #389
  sample.append(2.664894) #390
  sample.append(0.789584) #391
  sample.append(6.028709) #392
  sample.append(0.009572) #393
  sample.append(0.229779) #394
  sample.append(0.549582) #395
  sample.append(1.093048) #396
  sample.append(0.824514) #397
  sample.append(0.789584) #398
  sample.append(1.145804) #399
  sample.append(5.109279) #400
  sample.append(6.053906) #401
  sample.append(6.053907) #402
  sample.append(3.136574) #403
  sample.append(0.789584) #404
  sample.append(0.789584) #405
  sample.append(0.819205) #406
  sample.append(3.670611) #407
  sample.append(0.789584) #408
  sample.append(4.590029) #409
  sample.append(4.312276) #410
  sample.append(4.279569) #411
  sample.append(4.040213) #412
  sample.append(3.826498) #413
  sample.append(1.093048) #414
  sample.append(0.789584) #415
  sample.append(3.436912) #416
  sample.append(4.256435) #417
  sample.append(4.337873) #418
  sample.append(4.119073) #419
  sample.append(3.931176) #420
  sample.append(3.136574) #421
  sample.append(0.789584) #422
  sample.append(3.927943) #423
  sample.append(3.792921) #424
  sample.append(0.000000) #425
  sample.append(4.237240) #426
  sample.append(4.096264) #427
  sample.append(4.071058) #428
  sample.append(3.975877) #429
  sample.append(3.886222) #430
  sample.append(3.886222) #431
  sample.append(0.000000) #432
  sample.append(3.776154) #433
  sample.append(4.110508) #434
  sample.append(4.119073) #435
  sample.append(4.020748) #436
  sample.append(3.931176) #437
  sample.append(3.931177) #438
  sample.append(3.694669) #439
  sample.append(3.929643) #440
  sample.append(2.943082) #441
  sample.append(0.788050) #442
  sample.append(5.878218) #443
  sample.append(6.084674) #444
  sample.append(0.075190) #445
  sample.append(0.432683) #446
  sample.append(1.249777) #447
  sample.append(0.825022) #448
  sample.append(0.788050) #449
  sample.append(1.204451) #450
  sample.append(4.918368) #451
  sample.append(5.815236) #452
  sample.append(5.538962) #453
  sample.append(3.960798) #454
  sample.append(0.786350) #455
  sample.append(0.788050) #456
  sample.append(3.255518) #457



  out = decomp.transform(np.asarray(sample),selectedPCADimensions=210)
  for i in range(0,len(out)): 
    print(i," = ",out[i])


