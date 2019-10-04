project( WebcamJointBIN ) 
cmake_minimum_required( VERSION 2.8.7 )   
#cmake_minimum_required(VERSION 3.5)
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
  
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
include_directories(${TENSORFLOW_INCLUDE_ROOT})
 

add_executable(WebcamJointBIN ${BVH_SOURCE} ${OPENGL_SOURCE} test.cpp cameraControl.cpp ../MocapNETLib/opengl.cpp ../MocapNETLib/bvh.cpp ../MocapNETLib/csv.cpp ../MocapNETLib/nsdm.cpp ../MocapNETLib/visualization.cpp ../MocapNETLib/tools.cpp ../MocapNETLib/jsonCocoSkeleton.cpp ../MocapNETLib/InputParser_C.cpp ../MocapNETLib/gestureRecognition.cpp utilities.cpp ../Tensorflow/tensorflow.cpp ../Tensorflow/tf_utils.cpp  )

target_link_libraries(WebcamJointBIN rt dl m ${OpenCV_LIBRARIES} ${OPENGL_LIBS} Tensorflow  TensorflowFramework MocapNETLib )
set_target_properties(WebcamJointBIN PROPERTIES DEBUG_POSTFIX "D") 


       

set_target_properties(WebcamJointBIN PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

