project( BVHGUI ) 
cmake_minimum_required( VERSION 2.8.7 )   
#cmake_minimum_required(VERSION 3.5)
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
include_directories(${TENSORFLOW_INCLUDE_ROOT})


add_executable(BVHGUI ${BVH_SOURCE} ${OPENGL_SOURCE} bvhgui.cpp ../MocapNETLib/opengl.cpp ../MocapNETLib/bvh.cpp ../MocapNETLib/csv.cpp ../MocapNETLib/nsdm.cpp ../MocapNETLib/visualization.cpp ../MocapNETLib/remoteExecution.cpp ../MocapNETLib/tools.cpp ../MocapNETLib/jsonCocoSkeleton.cpp  ../MocapNETLib/jsonMocapNETHelpers.cpp ../MocapNETLib/InputParser_C.cpp ../MocapNETLib/gestureRecognition.cpp ../MocapNETLib/outputFiltering.cpp ../Tensorflow/tensorflow.cpp ../Tensorflow/tf_utils.cpp  )

target_link_libraries(BVHGUI rt dl m ${OpenCV_LIBRARIES} ${OPENGL_LIBS} Tensorflow  TensorflowFramework MocapNETLib ${NETWORK_CLIENT_LIBRARIES} ${PNG_Libs} ${JPG_Libs} )
set_target_properties(BVHGUI PROPERTIES DEBUG_POSTFIX "D") 


set_target_properties(BVHGUI PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

