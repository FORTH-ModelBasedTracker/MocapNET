import os

def checkIfFileExists(filename):
    return os.path.isfile(filename) 

def createDirectory(path):
    if not os.path.exists(path):
       os.makedirs(path)
