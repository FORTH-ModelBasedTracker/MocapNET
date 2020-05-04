project( helloWorld )  
cmake_minimum_required(VERSION 3.5) 

 

#-----------------------------------------------
#       This is the converter utilities..
#-----------------------------------------------
project( helloWorld )  
add_executable(helloWorld main.cpp)   
target_link_libraries(helloWorld rt dl m )
set_target_properties(helloWorld PROPERTIES DEBUG_POSTFIX "D") 
set_target_properties(helloWorld PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )
 

