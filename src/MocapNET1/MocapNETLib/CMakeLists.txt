project( MocapNETLib )  
cmake_minimum_required(VERSION 3.5) 

 
include_directories(${TENSORFLOW_INCLUDE_ROOT})
 

#add_executable(MocapNETLib mocapnet.cpp ../Tensorflow/tf_utils.cpp)   

add_library(MocapNETLib SHARED mocapnet.cpp ../../Tensorflow/tf_utils.cpp)   


target_link_libraries(MocapNETLib rt dl m pthread Tensorflow  TensorflowFramework )
set_target_properties(MocapNETLib PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(MocapNETLib PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )
 
