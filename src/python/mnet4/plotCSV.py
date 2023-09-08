#!/usr/bin/python3
import sys
import numpy as np
import matplotlib.pyplot as plt 
from readCSV import readGroundTruthFile,splitNumpyArray
from readCSV import parseConfiguration
from tools   import getFileFromPath

def splitNumpyArray(model,column):
    numberOfSamples=len(model)
    npOutput = np.empty([numberOfSamples],dtype=float,order='C')
    for num in range(0,numberOfSamples):
                  npOutput[num]=float(model[num,column]);
    return npOutput;

def draw2D(data,width=1920,height=1080):
    #----------------------------------------------------
    from MocapNETVisualization import drawMocapNETInput
    #----------------------------------------------------
    import cv2
    import numpy as np
    #----------------------------------------------------
    for sampleID in range(0,len(data['body'])):
      image  = np.zeros([height,width,3],dtype=np.uint8) 
      joints2D = dict()
      for labelID in range(0,len(data['label'])):
          label = data['label'][labelID].lower()
          value = data['body'][sampleID][labelID]
          #----------------------------------------------------
          if "2d_x" in label:
             value = value * width
          if "2d_y" in label:
             value = value * height
          #----------------------------------------------------
          joints2D[label] = value        
      drawMocapNETInput(joints2D,image,flipX=False)


      font = cv2.FONT_HERSHEY_SIMPLEX 
      org = (30,60) 
      fontScale = 1.5 
      color = (255,255,255)
      thickness = 2
      message =  'Sample Number : %u / %u ' % (sampleID,len(data['body']))  
      image = cv2.putText(image, message , org, font, fontScale, color, thickness, cv2.LINE_AA)
     

      cv2.imshow('2D',image)
      #print("Sample ",sampleID," -> ",joints2D)
      if cv2.waitKey(1000) & 0xFF == 27:
        sys.exit(0)
    #----------------------------------------------------    

def plotDistribution(column,filename,values):
    print("Plotting ",filename)
    #print(values.shape) 
    
    plt.clf()
    plt.hist(values,bins=100)
    median  = np.median(values)
    mean    = np.mean(values)
    std     = np.std(values)
    var     = np.var(values)
    minimum = np.min(values)
    maximum = np.max(values)
    
    title_string="Histogram of Output distribution for %s \nMedian=%0.2f,Mean=%0.2f,Std=%0.2f,Var=%0.2f,Samples=%u,Min=%0.2f,Max=%0.2f" % (filename,median,mean,std,var,len(values),minimum,maximum)
    plt.title(title_string)

    print("Stats,%s,%0.2f,%0.2f,%0.2f,%0.2f,%u,%0.2f,%0.2f" % (filename,median,mean,std,var,len(values),minimum,maximum))

    plt.savefig("%03u_%s.png" % (column,filename))

def calculate_relative_magnitudes(edm, n):
    row_magnitudes = []
    column_magnitudes = []
    for i in range(n):
        row_magnitude = sum(edm[i*n:(i+1)*n])
        row_magnitudes.append(row_magnitude)
        column_magnitude = sum(edm[i::n])
        column_magnitudes.append(column_magnitude)
    central_element = edm[n*n // 2]
    row_magnitudes_sum = sum(row_magnitudes) - central_element
    column_magnitudes_sum = sum(column_magnitudes) - central_element
    max_magnitude = max(row_magnitudes_sum, column_magnitudes_sum)
    relative_row_magnitudes = [m / max_magnitude for m in row_magnitudes]
    relative_column_magnitudes = [m / max_magnitude for m in column_magnitudes]
    return relative_row_magnitudes, relative_column_magnitudes


def plotCovariance(X):
 covariance = np.cov(X.T)

  # Plot the covariance matrix
 plt.imshow(covariance, cmap='coolwarm', interpolation='nearest')
 plt.colorbar()
 plt.title('Covariance Matrix')
 plt.savefig("covariance.png")
 #plt.show()


def plotTestOutputDistribution(csvfiletoplotIn,csvfiletoplotOut,mem):
    useRadians=0
    useHalfFloats=0

    dataFile                        = "body"
    #This will be get populated using the --config argument
    configuration = []
    configurationPath=""



    if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
     for i in range(0, len(sys.argv)):
       if (sys.argv[i]=="--config"):
          configurationPath=sys.argv[i+1]
          configuration = parseConfiguration(configurationPath)
          #setupDNNModelsUsingJSONConfiguration(configuration)
       if (sys.argv[i]=="--mem"):
              print("\nMemory usage ",sys.argv[i+1]);
              mem=float(sys.argv[i+1])
           #-----------------------------------------------
           #                New 4way split
           #-----------------------------------------------
       if (sys.argv[i]=="--dataset"):
              print("\nOverriding dataset ",dataFile," and using ",sys.argv[i+1]);
              dataFile=sys.argv[i+1]
       if (sys.argv[i]=="--all"):
              #Don't mix this --all with the step2_OrientatonClassifier.py --all
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_all" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
       if (sys.argv[i]=="--back"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_back" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
       if (sys.argv[i]=="--front"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_front" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
       if (sys.argv[i]=="--left"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_left" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
       if (sys.argv[i]=="--right"):
              hierarchyPartName=sys.argv[i+1]
              dataFile="%s_right" % hierarchyPartName
              outputDirectoryForTrainedModels="step0_%s" % dataFile
           #-----------------------------------------------


    #Resolve PCA name.. :(
    x = configuration['doPCA'].split("$",1)
    if (len(x)>0):
          #If there is a $ character this is our place holder to autocomplete our dataFile 
          print("PCA filename is resolved from ",configuration['doPCA'])
          print(" to ")
          configuration['doPCA']=x[0]+dataFile+".pca"
          print(configuration['doPCA'])


    numberOfChannelsPerNSDMElement=2
    if (configuration['NSDMAlsoUseAlignmentAngles']==1):
       numberOfChannelsPerNSDMElement=1
    print("Number of Channels Per NSDM element ",numberOfChannelsPerNSDMElement)

    groundTruth  = readGroundTruthFile(
                                       configuration,
                                       csvfiletoplotIn,
                                       csvfiletoplotIn,
                                       csvfiletoplotOut,
                                       mem,    
                                       numberOfChannelsPerNSDMElement,
                                       useRadians,
                                       useHalfFloats
                                      )
    #----------------------------------------------------------
    data = np.empty(len(groundTruth['out']))
    for z in range(0,len(groundTruth['labelOut'])): 
        data=splitNumpyArray(groundTruth['out'],z)
        if (z<3):
            np.multiply(data,10) 
        plotDistribution(z,groundTruth['labelOut'][z],data)
    #----------------------------------------------------------
    data = np.empty(len(groundTruth['in']))
    for z in range(0,len(groundTruth['labelIn'])): 
        data=splitNumpyArray(groundTruth['in'],z)
        plotDistribution(z,groundTruth['labelIn'][z],data)
    #----------------------------------------------------------



#Test Plots
#./GroundTruthDumper --from dataset/yawTest.bvh --csv ./ yaw.csv 2d --svg .
# python3 csvNET.py --from 2d_yaw.csv --save --keep --hands --white
# convert colorFrame_0_00000.jpg -crop 234x500+0+90 nsrm_00000.jpg

#./GroundTruthDumper --from dataset/pitchTest.bvh --csv ./ pitch.csv 2d --svg .
# python3 csvNET.py --from 2d_pitch.csv --save --keep --hands --white

#./GroundTruthDumper --from dataset/rollTest.bvh --csv ./ roll.csv 2d --svg .
# python3 csvNET.py --from 2d_roll.csv --save --keep --hands --white


def doRun():
    label = "nolabel"
    if (len(sys.argv)==3) :
       #==========================================
       if (sys.argv[1]=="--2d"):
          print(sys.argv[0]," --2d your.csv")
          labelSplit = sys.argv[2].split('.',1)
          label      = labelSplit[0]
          from readCSV import readCSVFile
          data  = readCSVFile(sys.argv[2],1.0,0)
          draw2D(data)
          sys.exit(0)
       #==========================================
       elif (sys.argv[1]=="--simple"):
          print(sys.argv[0]," --simple your.csv")
          labelSplit = sys.argv[2].split('.',1)
          label      = labelSplit[0]

          from readCSV import readCSVFile
          data  = readCSVFile(sys.argv[2],1.0,0)
          plotCovariance(data['body']) 
          #USE : python3 plotCSV.py --simple loss.csv | grep Stats  > lossStats.csv to create stats csv file!
          print("Stats,Output,Median,Mean,Std,Var,Samples,Min,Max")

          for z in range(0,len(data['label'])): 
              print("Plotting Column ",data['label'][z])
              specificData=splitNumpyArray(data['body'],z)
              filename = getFileFromPath('%s-%s' % (label,data['label'][z]))
              plotDistribution(z,filename,specificData)

          sys.exit(0)
       #==========================================


    if (len(sys.argv)<5) :
       print("plotCSV.py: Please rerun using a parameter of the CSV file you want to run..")
       print("e.g.")
       print(sys.argv[0]," --config dataset/lhand_configuration.json dataset/generated/2d_lhand_all.csv dataset/generated/bvh_lhand_all.csv --all lhand")
       print(" or ")
       print(sys.argv[0]," --config dataset/upperbody_configuration.json dataset/generated/2d_upperbody_all.csv dataset/generated/bvh_upperbody_all.csv --all upperbody ")
       print(" or ")
       print(sys.argv[0]," --config dataset/face_configuration.json dataset/generated/2d_face_all.csv dataset/generated/bvh_face_all.csv --all face")
       print(" or ")
       print(sys.argv[0]," --simple your.csv")
       sys.exit(0)
    plotTestOutputDistribution(sys.argv[3],sys.argv[4],1.0)


if __name__== "__main__":
   doRun()
