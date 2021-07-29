
def readCListFromFile(filename,listName):
    result=list()

    fi = open(filename, "r")
    Lines = fi.readlines() 
    
    print("readCListFromFile ",filename," ",listName)

    appendIncomingLines=0
    for line in Lines:
           #Switch appending on and off
           #----------------------------- 
           if (line.find(listName)!=-1):
                 appendIncomingLines=1 
           if (appendIncomingLines):
              if (line.find("};")!=-1):
                 appendIncomingLines=0
                 break;
           #----------------------------- 
           
           if (appendIncomingLines):
              #print(line) 
              startOfString = line.find("\"")
              endOfString = line.rfind("\"")
              #print(startOfString)
              #print(endOfString)
              if ( (startOfString!=-1) and (endOfString!=-1) ):
                 label = line[startOfString+1:endOfString]
                 #The C Lists have an End of X element we detect it and discard it
                 if (label.find("End of")==-1 ):
                 #print(label)
                    result.append(label)

    fi.close() 
    return result
