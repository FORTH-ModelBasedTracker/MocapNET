project( MocapNETLib )  
cmake_minimum_required(VERSION 3.5) 


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
 
include_directories(${TENSORFLOW_INCLUDE_ROOT})
 

#add_executable(MocapNETLib mocapnet.cpp ../Tensorflow/tf_utils.cpp)   

add_library(MocapNETLib SHARED mocapnet.cpp ../Tensorflow/tf_utils.cpp)   


target_link_libraries(MocapNETLib rt dl m Tensorflow  TensorflowFramework )
set_target_properties(MocapNETLib PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(MocapNETLib PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )











#-----------------------------------------------
#       This is the converter utility..
#-----------------------------------------------
project( convertBody25JSONToCSV )  
add_executable(convertBody25JSONToCSV convertBody25JsonToCSV.cpp tools.cpp jsonCocoSkeleton.cpp jsonMocapNETHelpers.cpp InputParser_C.cpp nsdm.cpp ${BVH_SOURCE} bvh.cpp csv.cpp)   
target_link_libraries(convertBody25JSONToCSV rt dl m )
set_target_properties(convertBody25JSONToCSV PROPERTIES DEBUG_POSTFIX "D") 
set_target_properties(convertBody25JSONToCSV PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

project( convertBVHToCSV )  
add_executable(convertBVHToCSV convertBVHToCSV.cpp tools.cpp jsonCocoSkeleton.cpp jsonMocapNETHelpers.cpp InputParser_C.cpp nsdm.cpp ${BVH_SOURCE} bvh.cpp csv.cpp)   
target_link_libraries(convertBVHToCSV rt dl m )
set_target_properties(convertBVHToCSV PROPERTIES DEBUG_POSTFIX "D") 
set_target_properties(convertBVHToCSV PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )



