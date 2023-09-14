
import numpy as np
import sys
import os

from readCSV import readCSVFile,writeCSVFile,splitNumpyArray

def functionThatFiltersOutColumnsWithStandardDeviation0(dataIn):
    body = dataIn['body']
    std_deviations = np.std(body, axis=0)
    columns_to_keep = np.where(std_deviations != 0)[0]

    filtered_body = body[:, columns_to_keep]
    filtered_labels = [dataIn['label'][i] for i in columns_to_keep]
    filtered_label_lookup = {label: i for i, label in enumerate(filtered_labels)}

    return {
        'label': filtered_labels,
        'labelLookup': filtered_label_lookup,
        'body': filtered_body
    }


def functionThatAppendsZeroColumnsByLabels(dataIn, labelsToAppendZeroColumns, preFixPadding=0, postFixPadding=0):
    body = dataIn['body']
    num_samples, num_columns = body.shape
    new_num_columns = num_columns + len(labelsToAppendZeroColumns) + preFixPadding + postFixPadding

    new_labels = []
  
    paddingNumber = 0
    # Create the new zero-filled data array
    new_body = np.zeros((num_samples, new_num_columns), dtype=body.dtype)

    #==========================================================================================
    for prefixedColumns in range(preFixPadding):
            print("Add prefixed padding ",paddingNumber)
            new_labels.append("padding_%u"%paddingNumber)
            new_body[:, targetColumnIndex] = 0.0  # Fill with zeros
            targetColumnIndex += 1  # Move to the next column
            paddingNumber += 1
    #==========================================================================================
    # Fill the new array with data from the original array
    targetColumnIndex = 0
    for originalColumnIndex in range(num_columns):
        if  dataIn['label'][originalColumnIndex] in labelsToAppendZeroColumns:
            print("Add padding ",paddingNumber)
            new_labels.append("padding_%u"%paddingNumber)
            new_body[:, targetColumnIndex] = 0.0  # Fill with zeros
            targetColumnIndex += 1  # Move to the next column
            paddingNumber += 1

        print("Processing #",originalColumnIndex," -> ",dataIn['label'][originalColumnIndex])
        new_labels.append(dataIn['label'][originalColumnIndex])
        new_body[:,targetColumnIndex] = body[:,originalColumnIndex]
        targetColumnIndex += 1
    #==========================================================================================
    for postfixedColumns in range(postFixPadding):
            print("Add postfixed padding ",paddingNumber)
            new_labels.append("padding_%u"%paddingNumber)
            new_body[:, targetColumnIndex] = 0.0  # Fill with zeros
            targetColumnIndex += 1  # Move to the next column
            paddingNumber += 1
    #==========================================================================================

    labelDict = dict()
    for i in range(0,len(new_labels)):
       labelDict[new_labels[i]]=i


    return {
        'label':       new_labels,
        'labelLookup': labelDict,
        'body':        new_body
    }




if __name__== "__main__":
  fileInput = sys.argv[1]
  data  = readCSVFile(fileInput,1.0,0)
  data  = functionThatFiltersOutColumnsWithStandardDeviation0(data)

  # Append zero columns at specific labels
  print("Disabled Paddings, to test all outputs trained together!")
  #if ("lhand" in fileInput):
  #  labels_to_append_zero_columns = ['lhand_Wrotation', 'lthumb_Zrotation', 'finger1-2.l_Zrotation', 'finger1-3.l_Zrotation', 'finger1-3.l_Xrotation']
  #  data = functionThatAppendsZeroColumnsByLabels(data, labels_to_append_zero_columns, postFixPadding = 1)
  
  column = 0
  for label in data['label']:
     thisColumnData = splitNumpyArray(data['body'],column,1,0) 
     minimum = np.min(thisColumnData)
     maximum = np.max(thisColumnData)
     median  = np.median(thisColumnData)
     mean    = np.mean(thisColumnData)
     std     = np.std(thisColumnData)
     var     = np.var(thisColumnData)
     title_string="%s : Min/Max=%0.2f/%0.2f,Median=%0.2f,Mean=%0.2f,Std=%0.2f,Var=%0.2f" % (label,minimum,maximum,median,mean,std,var)
     print(title_string)
     column = column + 1

  print("Writing New CSV file to : compressed.csv")
  writeCSVFile("compressed.csv",data)
  #-----------------------------------------------------------
  os.system("mv %s %s.uncompressed" % (fileInput,fileInput) )
  os.system("mv compressed.csv %s" % (fileInput) )
  print("Moved Old CSV file to : %s.uncompressed "%fileInput)
  print("Moved New CSV file to : %s "%fileInput)
  #-----------------------------------------------------------

