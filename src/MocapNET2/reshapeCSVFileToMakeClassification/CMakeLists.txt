project( ReshapeCSV ) 
cmake_minimum_required( VERSION 2.8.13 )   
#cmake_minimum_required(VERSION 3.5)

#find_package(OpenCV REQUIRED)
#INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})



add_executable(ReshapeCSV reshapeCSV.cpp )   
target_link_libraries(ReshapeCSV rt dl m Tensorflow  TensorflowFramework MocapNETLib2 )
#set_target_properties(TestCSV PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(ReshapeCSV PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

