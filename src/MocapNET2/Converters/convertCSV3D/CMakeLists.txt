project( convertCSV3DToMocapNETInput )  
cmake_minimum_required(VERSION 3.5) 


set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
 
include_directories(${TENSORFLOW_INCLUDE_ROOT})

#-----------------------------------------------
#       This is the converter utilities..
#-----------------------------------------------
add_executable(
               convertCSV3DToMocapNETInput 
               convertCSV3DToMocapNETInput.cpp
               ../../MocapNETLib2/tools.cpp 
               ../../MocapNETLib2/IO/jsonRead.cpp 
               ../../MocapNETLib2/IO/jsonMocapNETHelpers.cpp 
               ../../../../dependencies/InputParser/InputParser_C.cpp 
               ${BVH_SOURCE} 
               ${OPENGL_SOURCE} 
               ../../MocapNETLib2/IO/bvh.cpp 
               ../../MocapNETLib2/IO/conversions.cpp 
               ../../MocapNETLib2/IO/csvRead.cpp 
               ../../MocapNETLib2/IO/csvWrite.cpp
              )   

#-----------------------------------------------

target_link_libraries(convertCSV3DToMocapNETInput rt dl m pthread MocapNETLib2)
set_target_properties(convertCSV3DToMocapNETInput PROPERTIES DEBUG_POSTFIX "D") 
set_target_properties(convertCSV3DToMocapNETInput PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

