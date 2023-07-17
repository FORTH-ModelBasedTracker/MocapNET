project( MocapNET4Test ) 
cmake_minimum_required( VERSION 2.8.7 )   
#cmake_minimum_required(VERSION 3.5)
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
include_directories(${TENSORFLOW_INCLUDE_ROOT})
include_directories(../MocapNETLib4) #<- includes in test.cpp


add_executable(MocapNET4Test test.cpp)

target_link_libraries(MocapNET4Test rt dl m ${OpenCV_LIBRARIES} ${OPENGL_LIBS} JointEstimator2D Tensorflow  TensorflowFramework MocapNETLib4 ${NETWORK_CLIENT_LIBRARIES} ${PNG_Libs} ${JPG_Libs} )
set_target_properties(MocapNET4Test PROPERTIES DEBUG_POSTFIX "D") 


set_target_properties(MocapNET4Test PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

