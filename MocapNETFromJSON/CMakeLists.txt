project( MocapNETJSON ) 
cmake_minimum_required( VERSION 2.8.7 )   
#cmake_minimum_required(VERSION 3.5)

find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})



add_executable(MocapNETJSON ${BVH_SOURCE} mocapnetJSON.cpp ../MocapNETLib/bvh.cpp ../MocapNETLib/nsdm.cpp ../MocapNETLib/visualization.cpp ../MocapNETLib/tools.cpp ../MocapNETLib/jsonCocoSkeleton.cpp ../MocapNETLib/jsonMocapNETHelpers.cpp ../MocapNETLib/InputParser_C.cpp  ../MocapNETLib/gestureRecognition.cpp ../Tensorflow/tensorflow.cpp ../Tensorflow/tf_utils.cpp)   
target_link_libraries(MocapNETJSON rt dl m ${OpenCV_LIBRARIES}  Tensorflow  TensorflowFramework MocapNETLib)
set_target_properties(MocapNETJSON PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(MocapNETJSON PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

