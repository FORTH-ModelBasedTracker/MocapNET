import ctypes
from ctypes import *
libCalc = CDLL("./libcalci.so")
 
#call C function to check connection
libCalc.connect() 
 
#calling randNum() C function
#it returns random number
varRand = libCalc.randNum()
print("Random Number:", varRand, type(varRand))
 
#calling addNum() C function
#it returns addition of two numbers
varAdd = libCalc.addNum(20,30)
print("Addition : ", varAdd)


pyarr = [0.1, 0.2, 0.3, 0.4]
arr = (ctypes.c_float * len(pyarr))(*pyarr)
libCalc.printFloatList(arr,len(pyarr))
