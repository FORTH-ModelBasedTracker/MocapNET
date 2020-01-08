project( MocapNETServer ) 
cmake_minimum_required( VERSION 2.8.7 )
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_MODULE_PATH})
   
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
  
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
include_directories(${TENSORFLOW_INCLUDE_ROOT})

add_executable(MocapNETServer ${BVH_SOURCE} webserver.cpp ../MocapNETLib/bvh.cpp ../MocapNETLib/visualization.cpp ../MocapNETLib/remoteExecution.cpp ../MocapNETLib/tools.cpp ../MocapNETLib/jsonCocoSkeleton.cpp ../MocapNETLib/jsonMocapNETHelpers.cpp ../MocapNETLib/InputParser_C.cpp ../MocapNETLib/gestureRecognition.cpp ../MocapNETLib/outputFiltering.cpp ../Tensorflow/tensorflow.cpp ../Tensorflow/tf_utils.cpp  ) 
target_link_libraries(MocapNETServer pthread rt  dl m AmmarServer ${OpenCV_LIBRARIES} ${NETWORK_CLIENT_LIBRARIES} Tensorflow  TensorflowFramework MocapNETLib )
set_target_properties(MocapNETServer PROPERTIES DEBUG_POSTFIX "D")
add_dependencies(MocapNETServer AmmarServer)  
 
       
 
set_target_properties(MocapNETServer PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )
