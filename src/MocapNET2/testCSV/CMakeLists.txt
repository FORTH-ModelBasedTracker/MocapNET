project( TestCSV ) 
cmake_minimum_required( VERSION 2.8.13 )   
#cmake_minimum_required(VERSION 3.5)

find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})



add_executable(TestCSV testCSV.cpp )   
target_link_libraries(TestCSV rt dl m Tensorflow  TensorflowFramework MocapNETLib2 )
#set_target_properties(TestCSV PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(TestCSV PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

