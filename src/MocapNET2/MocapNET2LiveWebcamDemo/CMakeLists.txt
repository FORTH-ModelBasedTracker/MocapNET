project( MocapNET2LiveWebcamDemo ) 
cmake_minimum_required( VERSION 2.8.13 )   
#cmake_minimum_required(VERSION 3.5)
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
include_directories(${TENSORFLOW_INCLUDE_ROOT})


add_executable(MocapNET2LiveWebcamDemo livedemo.cpp  )

target_link_libraries(MocapNET2LiveWebcamDemo rt dl m ${OpenCV_LIBRARIES} ${OPENGL_LIBS} JointEstimator2D Tensorflow TensorflowFramework MocapNETLib2 ${NETWORK_CLIENT_LIBRARIES} ${PNG_Libs} ${JPG_Libs} )
set_target_properties(MocapNET2LiveWebcamDemo PROPERTIES DEBUG_POSTFIX "D") 


set_target_properties(MocapNET2LiveWebcamDemo PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

