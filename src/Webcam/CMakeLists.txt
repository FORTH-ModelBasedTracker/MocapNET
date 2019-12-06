project( OpenCVTest ) 
#cmake_minimum_required( VERSION 2.8.7 )   
cmake_minimum_required(VERSION 3.5)
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

 

add_executable(OpenCVTest webcam.cpp)   
target_link_libraries(OpenCVTest rt dl m ${OpenCV_LIBRARIES}    )
set_target_properties(OpenCVTest PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(OpenCVTest PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

