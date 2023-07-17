#!/usr/bin/python3
""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

import sys
import numpy as np

#python3 principleComponentAnalysisTester.py --config dataset/body_configuration.json --mem 1000 --highlight 4 --mode 3 

"""
Debug sanity check function to do PCA using SK Learn to make sure there are no errors on my numpy implementation
"""
def doPCAUsingSKLearn(data,label):
     from sklearn.decomposition import PCA
     import pandas as pd

     numberOfDimensions = 20
     pca = PCA(n_components=numberOfDimensions)
    
     # Standardizing the features
     from sklearn.preprocessing import StandardScaler
     data = StandardScaler().fit_transform(data)

     principalComponents = pca.fit_transform(data)
     #print(principalComponents) 

     import matplotlib.pyplot as plt
     fig = plt.figure(figsize = (8,8))
     fig.set_size_inches(19.2, 10.8, forward=True) 
  
     ax1 = fig.add_subplot(2, 1, 1,projection='3d') 
     ax2 = fig.add_subplot(2, 1, 2) 
     #ax = fig.add_subplot(1,1,1)  

     ax1.set_xlabel('Principal Component 1', fontsize = 15)
     ax1.set_ylabel('Principal Component 2', fontsize = 15)
     ax1.set_zlabel('Principal Component 3', fontsize = 15)
     ax1.set_title('%u component PCA %s '%(numberOfDimensions,label), fontsize = 20)

     ax1.scatter(principalComponents[:,0], 
                principalComponents[:,1], 
                principalComponents[:,2], 
                c = principalComponents[:,3] 
                #,s = principalComponents[:,3]
               )

   
     print("PCA variange ration : ",pca.explained_variance_ratio_)

     PC_values = np.arange(pca.n_components_) + 1
     ax2.plot(PC_values, pca.explained_variance_ratio_, 'o-', linewidth=2, color='blue')
     ax2.set_title('Scree Plot %s'%label)
     ax2.set_xlabel('Principal Component')
     ax2.set_ylabel('Variance Explained') 
     plt.show()
     sys.exit(0)

"""
Debug function to check the stats per column for our input
"""
def getStatsPerColumn(NSxM):
    print("NSxM is ",NSxM.shape[0]," x ",NSxM.shape[1])
    #NSxM.tofile('dump.csv', sep=',')
    
    for column in range(0,NSxM.shape[1]):
       singleNSxMElement = splitNumpyArray(NSxM,column,1,0)
       median  =  np.median(singleNSxMElement)
       mean    =  np.mean(singleNSxMElement)
       std     =  np.std(singleNSxMElement)
       var     =  np.var(singleNSxMElement)
       print("Element ",column,"/",NSxM.shape[1]," => Median is ",median,"Mean is ",mean," Std is ",std," Var is ",var)

    sys.exit(0)

class PCA():
  def __init__(self,
               inputData:np.array=np.array([]),
               savedFile:str=""
              ):
               self.mean                    = 0.0
               self.std                     = 1.0
               self.eigenvalues             = np.array([])
               self.eigenvectors            = np.array([])
               self.proportional            = list()
               self.cumulative              = list()
               self.numberOfSamplesFittedOn = 0
               self.expectedInputs          = 0
  
               if (savedFile!=""):
                  self.load(savedFile) 
               elif inputData.size != 0:
                  self.fit(inputData)
               else:
                  print("No PCA input given..!")

  def ok(self):
    return self.numberOfSamplesFittedOn!=0

  def getNumberOfExpectedSamples(self):
    #return len(self.eigenvalues)
    return np.size(self.eigenvalues, axis = 0)

  def fit(self,data):
    #doPCAUsingSKLearn(data,"Test") 
    #getStatsPerColumn(data)
    #print(data)
    
    self.numberOfSamplesFittedOn = data.shape[0]

    print("Doing PCA fit on ",self.numberOfSamplesFittedOn)
    print(" please wait .. ")

    #Standardize data
    #-------------------------------------------------------------------------------------------------
    self.mean = data.mean()
    data = data - self.mean
    # Normalize
    self.std  = data.std()
    if (self.std!=0.0):
       data = data / self.std
    #print('Data Mean : ',self.mean,'STD: ',self.std) 
    #-------------------------------------------------------------------------------------------------

    #Take the matrix, transpose it, and multiply the transposed matrix. This is the covariance matrix.
    covarianceMatrix = np.dot(data.T,data)

    #Get an array of computed eigenvalues and a matrix whose columns are the normalized eigenvectors corresponding to the eigenvalues in that order.
    #In this step it is important to make sure that the eigenvalues and its eigenvectors are sorted in descending order (from largest to smallest). Sort the eigenvalues and then the eigenvectors, accordingly.
    self.eigenvalues, self.eigenvectors = np.linalg.eig(covarianceMatrix)

    #Sort eigenvectors according to eigenvalues
    idx = self.eigenvalues.argsort()[::-1]
    self.eigenvalues  = self.eigenvalues[idx]
    self.eigenvectors = self.eigenvectors[:,idx]

    #Assign P to the matrix of eigenvectors and D to the diagonal matrix with eigenvalues on the diagonal and values of zero everywhere else. 
    #The eigenvalues on the diagonal of D will be associated with the corresponding column in P.
    D = np.diag(self.eigenvalues)
    P = self.eigenvectors

    self.expectedInputs = len(self.eigenvalues)

    #1. Calculate the proportion of variance explained by each feature
    sumOfEigenvalues = np.sum(self.eigenvalues)
    self.proportional = [i/sumOfEigenvalues for i in self.eigenvalues] 
    #2. Calculate the cumulative variance
    self.cumulative = [np.sum(self.proportional[:i+1]) for i in range(len(self.proportional))]

  def transform(self,data,selectedPCADimensions=0): 
    if (self.numberOfSamplesFittedOn==0):
        print("Can't transform input with no PCA loaded ..")
        return data
    #Normalize input data
    data = data - self.mean
    if (self.std!=0.0):
       data = data / self.std
    #Do transform..
    if (selectedPCADimensions==0):
       #Transform using all PCA components 
       return np.dot(data,self.eigenvectors)
    else:
       #print("eigenvectors is ",eigenvectors.shape[0]," x ",eigenvectors.shape[1])
       return data.dot(self.eigenvectors[:,:selectedPCADimensions])

  def save(self,filename):
       print("Saving PCA to ",filename)
       outputDict = dict()
       #------------------------------------------
       outputDict["numberOfSamplesFittedOn"]= str(self.numberOfSamplesFittedOn)
       outputDict["expectedInputs"]         = str(self.expectedInputs) 
       outputDict["mean"]                   = str(self.mean)
       outputDict["std"]                    = str(self.std)
       outputDict["eigenvalues"]            = list()
       outputDict["eigenvectors"]           = list()
       outputDict["scree_proportion"]       = list() 
       outputDict["scree_cumulative"]       = list() 
       #------------------------------------------ 
       for v in range(0,len(self.proportional)):
           outputDict["scree_proportion"].append(str(self.proportional[v]))
           outputDict["scree_cumulative"].append(str(self.cumulative[v]))
       #------------------------------------------
       print("eigenvalues ",self.eigenvalues.shape[0])
       for v in range(0,self.eigenvalues.shape[0]):
           outputDict["eigenvalues"].append(str(self.eigenvalues[v]))
       #------------------------------------------
       print("eigenvectors ",self.eigenvectors.shape[0]," x ",self.eigenvectors.shape[1])
       for r in range(0,self.eigenvectors.shape[0]):
         thisRow = list()
         for c in range(0,self.eigenvectors.shape[1]):
           thisRow.append(str(self.eigenvectors[r,c]))
         outputDict["eigenvectors"].append(thisRow)
       #------------------------------------------
       import json
       json_obj = json.dumps(outputDict)
       file = open(filename,'w',encoding="utf-8")
       file.write(json_obj) 
       file.close()

  def load(self,filename):
       print("Loading PCA from ",filename)
       import json
       file = open(filename,'r',encoding="utf-8")
       data = json.load(file)
       #-----------------------------------------------------
       self.numberOfSamplesFittedOn = int(data["numberOfSamplesFittedOn"])
       self.expectedInputs          = int(data["expectedInputs"]) 
       self.mean                    = float(data["mean"])
       self.std                     = float(data["std"])
       #-----------------------------------------------------
       numberOfEigenValues = len(data["eigenvalues"])
       print("Eigen values = ",numberOfEigenValues)
       self.eigenvalues = np.full([numberOfEigenValues],fill_value=0,dtype=np.complex_,order='C')
       for i in range(0,numberOfEigenValues):
           self.eigenvalues[i] = complex(data["eigenvalues"][i]) 
       #-----------------------------------------------------
       numberOfEigenVectors = len(data["eigenvectors"])
       print("Eigen vectors = ",numberOfEigenVectors)
       self.eigenvectors = np.full([numberOfEigenVectors,numberOfEigenVectors],fill_value=0,dtype=np.complex_,order='C')
       for r in range(0,numberOfEigenVectors):
         for c in range(0,numberOfEigenVectors):
           self.eigenvectors[r,c] = complex(data["eigenvectors"][r][c]) 
       #-----------------------------------------------------
       file.close()
       return self.mean,self.std,self.eigenvalues,self.eigenvectors


  def visualize(self,data,saveToFile="",onlyScreePlotNDimensions=0,label="PCA",colors=list(),colorLabel="Highlighting PC-4",viewAzimuth=45,viewElevation=45,showScree=1): 
    import matplotlib.pyplot as plt

    font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 28}

    plt.rc('font', **font)
    plt.rc('xtick', labelsize=15) 
    plt.rc('ytick', labelsize=15) 
    # === Plot =========================================================================
    fig = plt.figure()
    fig.set_size_inches(19.2, 10.8, forward=True)
    
    if (showScree==1):
      ax2 = fig.add_subplot(1, 2, 1) 
      ax1 = fig.add_subplot(1, 2, 2,projection='3d')
    else: 
      ax1 = fig.add_subplot(1, 1, 1,projection='3d')
    #=================================================================================== 

    #Number of PCA components to plot on first plot (our plot is 3D so max is 4 if we dont have a color ..! )
    keepNDimensions = 3
    if (len(colors)==0):
       keepNDimensions = 4

    #Do transform of our input using the PCA dimensions as new basis
    #=================================================================================== 
    transformedData = self.transform(data,selectedPCADimensions=keepNDimensions).real
    #=================================================================================== 

    if (len(colors)==0):
       colors = transformedData[:,3]
       colorLabel = "highlighting PC-4"
    else:
       print("Using provided set of colorValues")
       keepNDimensions = 3

    #If there is no limit on Scree plot dimensions then plot all
    if (onlyScreePlotNDimensions==0): 
       onlyScreePlotNDimensions = len(eigenvalues)
    #===================================================================================
    plottedEigenValues = self.eigenvalues
    plottedEigenValues=list()
    for i in range(0,onlyScreePlotNDimensions):
        plottedEigenValues.append(self.eigenvalues[i])
    #===================================================================================
    #1. Calculate the proportion of variance explained by each feature
    sum_eigenvalues = np.sum(plottedEigenValues)
    prop_var = [i/sum_eigenvalues for i in plottedEigenValues]
    #2. Calculate the cumulative variance
    cum_var = [np.sum(prop_var[:i+1]) for i in range(len(prop_var))]
    #===================================================================================

    ax1.view_init(viewAzimuth,viewElevation) 
    #=================================================================================== 
    ax1.scatter(transformedData[:,0],transformedData[:,1],transformedData[:,2],c=colors)
    #=================================================================================== 

    # Adding title, xlabel and ylabel
    ax1.set_title('PCA %s %s '%(label,colorLabel)) # Title of the plot
    ax1.set_xlabel('PC-1 (%0.2f %%) '% (100.0*float(prop_var[0])),labelpad=30) # X-Label
    ax1.set_ylabel('PC-2 (%0.2f %%) '% (100.0*float(prop_var[1])),labelpad=30) # Y-Label
    ax1.set_zlabel('PC-3 (%0.2f %%) '% (100.0*float(prop_var[2])),labelpad=30) # Z-Label
    #ax1.tick_params(axis='x', pad=5) #fine tune numbers of plot
    #===================================================================================
    #===================================================================================
    #=================================================================================== 
    if (showScree==1):
       # Plot scree plot from PCA
       x_labels = ['PC{}'.format(i+1) for i in range(len(prop_var))]
       ax2.plot(x_labels, prop_var, marker='o', markersize=6, color='skyblue', linewidth=2, label='Proportion of variance')
       ax2.plot(x_labels, cum_var, marker='o', color='orange', linewidth=2, label="Cumulative variance")
       ax2.legend()
       ax2.set_title('Scree plot %s '%label)
       ax2.set_xlabel('Principal components')
       ax2.set_ylabel('Proportion of variance')
    #===================================================================================
    #===================================================================================
    #===================================================================================
    
    plt.subplots_adjust(left=0.05, right=0.95, top=0.95, bottom=0.08)

    if (saveToFile!=""):
      fig.savefig(saveToFile)
    else:
      plt.show() 


if __name__ == '__main__':
  pca = PCA(savedFile="../../../../dataset/combinedModel/mocapnet4/mode1/1.0/step1_upperbody_all/upperbody_all.pca")
  inptR = [1.0] * 458
  inpt =np.asarray(inptR,dtype=np.float32)
  outLength = 210
  out = pca.transform(inpt,selectedPCADimensions=outLength)
  for i in range(0,outLength):
    print("%u = %0.6f" % (i,out[i]))

