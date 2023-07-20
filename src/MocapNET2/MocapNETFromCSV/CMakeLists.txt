project( MocapNET2CSV ) 
cmake_minimum_required( VERSION 2.8.13 )   
#cmake_minimum_required(VERSION 3.5)

find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

add_executable(MocapNET2CSV  mocapnet2CSV.cpp  )   
target_link_libraries(MocapNET2CSV rt dl m ${OpenCV_LIBRARIES}  Tensorflow  TensorflowFramework MocapNETLib2 )
set_target_properties(MocapNET2CSV PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(MocapNET2CSV PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

