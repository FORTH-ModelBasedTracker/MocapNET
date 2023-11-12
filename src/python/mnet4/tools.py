#!/usr/bin/python3

import os
import sys

import numpy as np
from numba import njit #Test

""" 
Author : "Ammar Qammaz"
Copyright : "2022 Foundation of Research and Technology, Computer Science Department Greece, See license.txt"
License : "FORTH" 
"""

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'



def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

"""
Convert seconds to Hertz
"""
def secondsToHz(seconds):
    if (seconds==0.0):
             seconds=1.0
    return float(1 / seconds)



@njit
def optimizedStD(data,mean,N):
    #JiT compilation of this in place loop to speed it up
    M2 = np.float32(0.0)
    for row in data:
        for x in row:
            delta = x - mean
            M2   += delta**2
    return np.sqrt(M2 / N)


def calculateStandardDeviationInPlaceKnowingMean(data,mean):
    N = data.shape[0] * data.shape[1]
    if (N==0): 
      return np.NAN
    #----------------------------------
    return optimizedStD(data,np.float32(mean),N)
    #----------------------------------
    #M2 = 0.0
    #for row in data:
    #    for x in row:
    #        delta = x - mean
    #        M2 += delta**2
    #return np.sqrt(M2 / N)

def convertStandardDeviationToVariance(std):
    return std ** 2






"""
 This is a VERY commonly needed conversion, MocapNET is trained on a 1920x1080 frame to match popular cameras,
 many image sources/sensors produce different kinds of image resolutions. The aspect ratio is very important
 for good results, this function does the needed conversion..
"""
def transform_coordinate_wrt_target(x, y, width, height, targetWidth = 1920, targetHeight = 1080):
    aspect_ratio = width / height

    if aspect_ratio > targetWidth / targetHeight:
        # The image is wider than the frame
        scaled_width = targetWidth
        scaled_height = int(targetWidth / aspect_ratio)
        offset_x = 0
        offset_y = (targetHeight - scaled_height) // 2
    else:
        # The image is taller than the frame
        scaled_width = int(targetHeight * aspect_ratio)
        scaled_height = targetHeight
        offset_x = (targetWidth - scaled_width) // 2
        offset_y = 0
        
    new_x = (x / width) * scaled_width + offset_x
    new_y = (y / height) * scaled_height + offset_y
    return new_x, new_y




def img_resizeWithPadding(img,targetWidth,targetHeight):
    import cv2
    width  = targetWidth
    height = targetHeight 
    h, w = img.shape[:2]
    pad_bottom, pad_right = 0, 0
    ratio = w / h

    if h > height or w > width:
        # shrinking image algorithm
        interp = cv2.INTER_AREA
    else:
        # stretching image algorithm
        interp = cv2.INTER_CUBIC

    w = width
    h = round(w / ratio)
    if h > height:
        h = height
        w = round(h * ratio)
    pad_top    = int(abs(height - h)/2)
    pad_bottom = int(abs(height - h)/2)
    pad_left   = int(abs(width - w)/2)
    pad_right  = int(abs(width - w)/2)

    scaled_img = cv2.resize(img, (w, h), interpolation=interp)
    padded_img = cv2.copyMakeBorder(scaled_img,pad_top,pad_bottom,pad_left,pad_right,borderType=cv2.BORDER_CONSTANT,value=[0,0,0])
    return padded_img


def img_resizeWithCrop(img, targetWidth,targetHeight):
    import cv2
    import numpy as np
    interpolation=cv2.INTER_AREA
    h, w = img.shape[:2]
    min_size = np.amin([h,w])

    # Centralize and crop
    crop_img = img[int(h/2-min_size/2):int(h/2+min_size/2), int(w/2-min_size/2):int(w/2+min_size/2)]
    resized = cv2.resize(crop_img, (targetWidth,targetHeight), interpolation=interpolation)
    return resized



def normalizedCoordinatesAdaptForVerticalImage(sourceWidth,sourceHeight,targetWidth,targetHeight,nX,nY): 
    import numpy as np
    h = sourceHeight
    w = sourceWidth
    min_size = np.amin([h,w])
    
    nY = int(h/2-min_size/2) + int(nY*min_size)
    nX = int(w/2-min_size/2) + int(nX*min_size)

    return float(nX/sourceWidth),float(nY/sourceHeight)



def normalizedCoordinatesAdaptToResizedCrop(sourceWidth,sourceHeight,trainingWidth,trainingHeight,nX,nY):
    if (sourceHeight>sourceWidth):
        print("PoseNET.py normalizedCoordinatesAdaptToResizedCrop: FIX VERTICAL IMAGE")
        import numpy as np
    
    return nX,nY



"""
Split joint names based on configuration
"""
def splitTextBasedOnGroupNumber(groupNumber,sourceFile,targetFile):
    import sys
    #This function used to be this one liner.. 
    #However some joints have additional '-' characters e.g. 3DZ_toe1-2.r so this whole next codeblock
    #attempts to properly split them 
    #import os
    #os.system('cat %s | sed \'s/-/\\n/g\' > %s' % (sourceFile,targetFile))
    #return 
    #-------------------------------------------------------------------------------------------------------- 
    if (groupNumber==0):
            print(bcolors.FAIL," Cant split to ",groupNumber," groups ",bcolors.ENDC)
            sys.exit(1)
    #-------------------------------------------------------------------------------------------------------- 
    fileR = open(sourceFile, 'r')
    fileW = open(targetFile, 'w')
    #-------------------------------------------------------------------------------------------------------- 
    Lines = fileR.readlines()
    for line in Lines:
       lineNoNL = line.strip()
       split = lineNoNL.split('-')
       standaloneJoints = list()
       if (lineNoNL!=""):
          if (len(split)==groupNumber):
            for joint in split:
                standaloneJoints.append(joint)
          elif (len(split)<groupNumber):
            print(bcolors.FAIL," Could not split text `",lineNoNL,"` to ",groupNumber," groups (only got ",len(split),") ",bcolors.ENDC)
            sys.exit(1)
          else:
            #Weird case
            count = 0
            increments = int(len(split) / groupNumber)
            for jointNumber in range(0,groupNumber):
              collectedJointName = split[count]
              count = count + 1
              for jointIncrement in range(1,increments):
                  collectedJointName = collectedJointName + '-' + split[count]
                  count = count + 1
              standaloneJoints.append(collectedJointName) 

          for sJoint in standaloneJoints:
           fileW.write(sJoint)
           fileW.write("\n")
       else:
         print("Empty line skipped\n") 
    #-------------------------------------------------------------------------------------------------------- 
    fileR.close()
    fileW.close()
    #-------------------------------------------------------------------------------------------------------- 






"""
Capitalize 3D X,Y,Z coordinate tags e.g. 3dx_tag -> 3DX_tag
"""
def capitalizeCoordinateTags(inputs):
    out = dict()
    keysList = list(inputs.keys())
    for key in keysList:
        keyOriginal = key
        keyNew = ""
        convert = False 
        tagList = key.split('_')
        tag = tagList[0]
        if (tag=="3dx"):
            convert = True
            keyNew="3DX_"+key[4:]
        if (tag=="3dy"):
            convert = True
            keyNew="3DY_"+key[4:]
        if (tag=="3dz"):
            convert = True
            keyNew="3DZ_"+key[4:]
        
        if (convert):
           #print("Convert ",keyOriginal," to ",keyNew)
           out[keyNew]=inputs[keyOriginal]
        else:
           out[keyOriginal]=inputs[keyOriginal]
        
    return out



"""
Model names will cause errors if they contain invalid characters, so we make sure everything 
is named in accordance with tensorflow rules
"""
def tensorflowFriendlyModelName(rawName):
 #Avoiding "Tensorflow %s is not valid scope name error" when creating a model
 #'-', '\', '/', or '_'
 filteredName = rawName
 filteredName = filteredName.replace("-",  ".")
 filteredName = filteredName.replace("_",  ".")
 filteredName = filteredName.replace("/",  ".")
 filteredName = filteredName.replace("\\", ".")
 return filteredName

"""
Send a system notification that will travel to all connected devices
"""
def notification(title,message):
    #This needs a more proper sanitization!
    title = title.strip("\"'")
    message = message.strip("\"'")
    try:
      os.system('notify-send -a \"%s\" \"%s\"&' % (title,message))
    except:
      print("NOTIFICATION : ",title,message)

"""
Bytes to KB/MB/GB/TB converter
"""
def convert_bytes(num):
    """
    this function will convert bytes to MB.... GB... etc
    """
    step_unit = 1000.0 #1024 bad the size

    for x in ['bytes', 'KB', 'MB', 'GB', 'TB']:
        if num < step_unit:
            return "%3.1f %s" % (num, x)
        num /= step_unit

"""
Retreive the available system RAM
"""
def getRAMInformation():
    """
    Get node total memory and memory usage
    """
    with open('/proc/meminfo', 'r') as mem:
        ret = {}
        tmp = 0
        for i in mem:
            sline = i.split()
            if str(sline[0]) == 'MemTotal:':
                ret['total'] = int(sline[1])
            elif str(sline[0]) in ('MemFree:', 'Buffers:', 'Cached:'):
                tmp += int(sline[1])
        ret['free'] = int(tmp)
        ret['used'] = int(ret['total']) - int(ret['free'])
    return ret




    


"""
Count the number of lines by parsing the file inside python
"""
def getNumberOfLines(filename):
    #import socket 
    #print("Hostname is ",socket.gethostname())
    #if (socket.gethostname()=="ammar-kriti"):
    #  print(bcolors.FAIL,"RETURNING FIXED NUMBER OF LINES FOR SPEED IN MY SLOW PC",bcolors.ENDC)
    #  return 3858095
    print("Counting number of lines in file ",filename)
    with open(filename) as f:
        return sum(1 for line in f) 

"""
Count the number of lines by asking the shell/OS to do it using wc -l 
"""
def getNumberOfLinesOS(filename):
    #import socket 
    #print("Hostname is ",socket.gethostname())
    #if (socket.gethostname()=="ammar-kriti"):
    #  print(bcolors.FAIL,"RETURNING FIXED NUMBER OF LINES FOR SPEED IN MY SLOW PC",bcolors.ENDC)
    #  return 3858095
    print("Counting number of lines in file ",filename)
    import subprocess
    #Standard but more compatible 
    command = ('wc -l %s' % filename)
      
    p = subprocess.Popen(command, universal_newlines=True,shell=True, stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    text = p.stdout.read()
    retcode = p.wait()
    out = text.split( )
    print("It was ",out[0])
    return int(out[0])


"""
Check version against master server
"""
def checkVersion(MOCAPNET_VERSION):
    import socket 
    os.system("wget -qO- \"http://ammar.gr/mocapnet/version/index.php?h=%s&v=%s\"&"%(socket.gethostname(),MOCAPNET_VERSION))
    #http://ammar.gr/mocapnet/version/index.php?h=elina-kriti&v=4.0


"""
Check the number of times a specific keyword *pattern* appears inside the file with the given filename
"""
def getNumberOfOccurancesOS(filename,keyword):
    print("Counting number of occurances of ",keyword," in file ",filename)
    import subprocess
    #Standard but more compatible 
    command = ('cat %s grep -o %s | wc -l' % (filename,keyword))
      
    p = subprocess.Popen(command, universal_newlines=True,shell=True, stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    text = p.stdout.read()
    retcode = p.wait()
    out = text.split()
    print("It was ",out[0])
    return int(out[0])

"""
Get the size of a file in disk
"""
def getFileSizeInKB(filename):
  return int( os.path.getsize(filename)/1024 ) 

"""
Convert list to lowercase
"""
def convertListToLowerCase(theList):
  lowercaseList = [item.lower() for item in theList]
  return lowercaseList

"""
Dump a list to a file
"""
def dumpListToFile(filename,theList):
  file=open(filename,'w')
  for items in theList:
    file.writelines(items+'\n')
  file.close()

"""
Read a list from a file
"""
def readListFromFile(filename):
  file=open(filename,'r')
  Lines = file.readlines()
  res = []
  for x in Lines:
    res.append(x.replace("\n", ""))
  file.close()
  print("List : ",filename," has ",len(res)," elements")
  return res


"""
Dump a labeled mini CSV ( 2 rows ) to a file
"""
def dumpMiniCSVToFile(filename,theLabels,theValues,columns):
  #Write CSV outputOffsets.csv 
  f = open(filename, 'w')
  #------------------------------------------------------------------------  
  for column in range(columns):
   if (column>0):
      f.write(',')
   f.write("%s"%(theLabels[column]))
  f.write('\n')
  #------------------------------------------------------------------------  
  for column in range(columns):
   if (column>0):
     f.write(',')
   f.write("%f"%(theValues[column]))
  f.write('\n')
  #------------------------------------------------------------------------  
  f.close()



"""
Append a labeled CSV to a file
"""
def appendCSVToFile(filename,theValues,fID=0):
  if (fID==0):
     f = open(filename, 'w')
     #------------------------------------------------------------------------  
     for column in range(len(theValues)):
      if (column>0):
         f.write(',')
      f.write("label%u"%(column))
     f.write('\n')
     f.close()
  #------------------------------------------------------------------------  
  #------------------------------------------------------------------------  
  f = open(filename, 'a')
  #------------------------------------------------------------------------  
  for column in range(len(theValues)):
   if (column>0):
     f.write(',')
   f.write("%f"%(theValues[column]))
  f.write('\n')
  #------------------------------------------------------------------------  
  f.close()
  


def saveCSVFileFromListOfDicts(filename,inputDicts):
    labels = list()
    #--------------------------
    for frame in inputDicts:
      for label in frame.keys():
        if not label in labels: 
           labels.append(label)
    #--------------------------   
    f = open(filename, 'w')
    #Write header..
    #------------------------------------------------------------------------  
    for column in range(len(labels)):
      if (column>0):
        f.write(',')
      f.write("%s"%(labels[column]))
    f.write('\n')
    #------------------------------------------------------------------------  
    #Write body..
    #------------------------------------------------------------------------ 
    for frame in inputDicts: 
     for column in range(len(labels)):
      if (column>0):
        f.write(',')
      if (labels[column] in frame):
         f.write("%f"%(frame[labels[column]]))
      else:
         f.write("0")
     f.write('\n')
    #------------------------------------------------------------------------  
    f.close()


"""
Given a /path/to/a/specific/file.ext remove file.ext and return the base path
"""
def getDirectoryFromPath(path):
   return os.path.dirname(path)

"""
Given a /path/to/a/specific/file.ext remove /path/to/a/specific/ and return file.ext
"""
def getFileFromPath(path):
   return os.path.basename(path)




"""
Create a new directory if it does not exist
"""
def createDirectory(path):
    if not os.path.exists(path):
       os.makedirs(path)

"""
Check if a file exists
"""
def checkIfFileExists(filename):
    return os.path.isfile(filename) 

"""
Check if a path exists
"""
def checkIfPathExists(filename):
    return os.path.exists(filename) 


"""
Check if a path exists
"""
def checkIfPathIsDirectory(filename):
    return os.path.isdir(filename) 


"""
Check if an entry is part of a given list 
"""
def checkIfEntryIsInList(theList,theEntry):
    for listItem in theList:
        if(theEntry==listItem): return 1
    return 0


"""
Check if an entry is part of a given list 
"""
def checkIfListsAreTheSame(theListA,theListB):
    #----------------------------------------------
    if (len(theListA)!=len(theListB)):
     return 0
    #----------------------------------------------
    if (len(theListA)==0) and (len(theListB)==0):
     return 1
    #----------------------------------------------  
    for i in range(len(theListA)):
        if theListA[i]!=theListB[i]:
           return 0
    #----------------------------------------------
    return 1





def parseSerialNumberFromSummary(html_path):
    import re
    content = ""
    with open(html_path, 'r', encoding='utf-8') as html_file:
        content = html_file.read()

    # Regular expression pattern to match the serial number
    pattern = r'<td>Description<\/td>\s*<\/tr>\s*<tr>\s*<td>\s*([A-Za-z0-9]+)'

    # Search for the pattern in the HTML content
    match = re.search(pattern, content, re.DOTALL)

    if match:
        serial_number = match.group(1)
        return serial_number

    return "?"

def filterListOfStringsByRegex(string_list, regex_pattern):
    import re
    #Usage : 
    #input_list = ["apple", "banana", "cherry", "date", "elderberry"]
    #pattern = r"^[a-c].*"  # Matches strings starting with letters a, b, or c
    #result = filterListOfStringsByRegex(input_list, pattern)

    matched_strings = []
    
    for string in string_list:
        if re.match(regex_pattern, string):
            matched_strings.append(string)
    
    return matched_strings


def convertListOfRegexToListOfLists(master_string_list,regex_list):    
    string_list_output = []
    for regex_pattern in regex_list:
        string_list_output.append(filterListOfStringsByRegex(master_string_list,regex_pattern)) 
    return string_list_output


"""
Check if an entry is part of a given list 
"""
def getEntryIndexInList(theList,theEntry):
    i=0
    for listItem in theList:
        if(theEntry.lower()==listItem.lower()): 
         return i
        i=i+1
    return -1


"""
Check if an entry is in a sublist of our configuration
"""
def checkIfEntryIsInConfigurationKey(configuration,theKey,theEntry):
    for listItem in configuration[theKey]:
        if(theEntry==listItem): return 1
    return 0

"""
Check if a joint is declared in the configuration hierarchy
"""
def getConfigurationJointIsDeclaredInHierarchy(configuration,theEntry):
    #-------------------------------------------------------------------------------------------
    if (theEntry=="everything"):
      print(bcolors.WARNING,"EVERYTHING.. is declared always.. ",bcolors.ENDC)
      return 1
    #-------------------------------------------------------------------------------------------
    try:
      out = theEntry.split('_')
      theEntry=out[0]
    except:
      print("getConfigurationJointIsDeclaredInHierarchy could not split ",theEntry)
    #-------------------------------------------------------------------------------------------
    if 'banned' in configuration:
      for listItem in configuration['banned']:
        if(theEntry.lower()==listItem['output'].lower()): 
             print(bcolors.WARNING," Joint ",theEntry," is declared in banlist! ",bcolors.ENDC)
             return 1

    if 'hierarchy' in configuration:
      for listItem in configuration['hierarchy']:
        #print("Check ",theEntry," vs ",listItem['joint'])
        if(theEntry.lower()==listItem['joint'].lower()): 
             print("The Joint ",theEntry," is : declared in hierarchy")
             return 1

    print("Joint is not declared in hierarchy..")
    #-------------------------------------------------------------------------------------------
    return 0


"""
Retrieve the configuration joint priority of a joint is declared in the configuration hierarchy
"""
def getConfigurationJointPriority(configuration,theEntry):
    #-------------------------------------------------------------------------------------------
    if (theEntry=="everything"):
      print("EVERYTHING.. has a high priority always.. ")
      return 1
    #-------------------------------------------------------------------------------------------
    try:
      out = theEntry.split('_')
      theEntry=out[0]
    except:
      print("getConfigurationJointPriority could not split ",theEntry)
    #-------------------------------------------------------------------------------------------
    if 'outputMode' in configuration:
      if (configuration['outputMode'] == "3d"):
        print(bcolors.WARNING,"We treat all joints as terribly important in 3D point mode!",bcolors.ENDC)
        return 1

    if 'banned' in configuration:
      for listItem in configuration['banned']:
        if(theEntry==listItem['output']): 
             print(bcolors.WARNING," Joint ",theEntry," is in banlist! ",bcolors.ENDC)
             return 0

    if 'hierarchy' in configuration:
      for listItem in configuration['hierarchy']:
        if(theEntry==listItem['joint']): 
             print("The Importance of Joint ",theEntry," is : ",listItem['importance'])
             return listItem['importance']

    print("The Importance of Joint ",theEntry," is : 0 ")
    #-------------------------------------------------------------------------------------------
    return 0


"""
Get the parent network from our configuration joint hierarchy
"""
def getParentNetwork(configuration,theEntry):
    #-------------------------------------------------------------------------------------------
    if (theEntry=="everything"):
      print("EVERYTHING.. has no parent.. ")
      return "none"
    #-------------------------------------------------------------------------------------------
    try:
      out = theEntry.split('_')
      theJoint=out[0]
      theChannel=out[1]
    except:
      print("getParentNetwork could not split ",theEntry)
      theJoint=theEntry
      theChannel=0 
    #-------------------------------------------------------------------------------------------
    print("Checking the parent of Joint(",theJoint,")/Channel(",theChannel,")")
    for listItem in configuration['hierarchy']:
        if(theJoint==listItem['joint']):
             if (listItem['inheritNetwork']=="none"):
                  return "none"
             parentNetwork="%s_%s" % (listItem['inheritNetwork'],theChannel)  
             if (parentNetwork==theEntry):
                  return "none"
             else:
                  return parentNetwork
    #-------------------------------------------------------------------------------------------
    return "none"


if __name__ == '__main__':
    print("Tools.py is a library!")
    splitTextBasedOnGroupNumber(3,"tmp.tmp","tmpF.tmp")
    
