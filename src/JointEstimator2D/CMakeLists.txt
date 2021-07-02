project( JointEstimator2D )  
cmake_minimum_required(VERSION 3.5) 


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#set(CMAKE_CXX_STANDARD 11)  
 
include_directories(${TENSORFLOW_INCLUDE_ROOT})

add_library(
            JointEstimator2D SHARED 
            jointEstimator2D.cpp 
            cameraControl.cpp 
            visualization.cpp 
            ${CMAKE_SOURCE_DIR}/src/Tensorflow/tensorflow.cpp 
            ${CMAKE_SOURCE_DIR}/src/Tensorflow/tf_utils.cpp
           )

target_link_libraries(JointEstimator2D rt dl m Tensorflow TensorflowFramework )
set_target_properties(JointEstimator2D PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(JointEstimator2D PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )




find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

add_executable(WebcamAnd2DJoints webcamAnd2DJoints.cpp)   
target_link_libraries(WebcamAnd2DJoints rt dl m ${OpenCV_LIBRARIES} JointEstimator2D  Tensorflow  TensorflowFramework)
set_target_properties(WebcamAnd2DJoints PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(WebcamAnd2DJoints PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

