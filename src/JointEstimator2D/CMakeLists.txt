project( JointEstimator2D )  
cmake_minimum_required(VERSION 3.5) 


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
 
include_directories(${TENSORFLOW_INCLUDE_ROOT})


add_library(JointEstimator2D SHARED jointEstimator2D.cpp ../Tensorflow/tf_utils.cpp)   


target_link_libraries(JointEstimator2D rt dl m Tensorflow  TensorflowFramework )
set_target_properties(JointEstimator2D PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(JointEstimator2D PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

