project( Tensorflow2 ) 
cmake_minimum_required( VERSION 2.8.7 )   
#cmake_minimum_required(VERSION 3.5)
 

add_executable(Tensorflow2  testtf2.cpp tensorflow2.h )   
target_link_libraries(Tensorflow2 rt dl m Tensorflow  TensorflowFramework MocapNETLib2 )
set_target_properties(Tensorflow2 PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(Tensorflow2 PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

