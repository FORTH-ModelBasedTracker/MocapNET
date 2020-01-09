project( ConverterUtilities )  
cmake_minimum_required(VERSION 3.5) 


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
 
include_directories(${TENSORFLOW_INCLUDE_ROOT})
 


#-----------------------------------------------
#       This is the converter utilities..
#-----------------------------------------------
project( convertBody25JSONToCSV )  
add_executable(convertBody25JSONToCSV convertBody25JsonToCSV.cpp ../MocapNET1/MocapNETLib/tools.cpp ../MocapNET1/MocapNETLib/jsonCocoSkeleton.cpp ../MocapNET1/MocapNETLib/jsonMocapNETHelpers.cpp ../../dependencies/InputParser/InputParser_C.cpp ${BVH_SOURCE} ../MocapNET1/MocapNETLib/bvh.cpp ../MocapNET1/MocapNETLib/csv.cpp)   
target_link_libraries(convertBody25JSONToCSV rt dl m )
set_target_properties(convertBody25JSONToCSV PROPERTIES DEBUG_POSTFIX "D") 
set_target_properties(convertBody25JSONToCSV PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

project( convertOpenPoseJSONToCSV )  
add_executable(convertOpenPoseJSONToCSV convertOpenPoseJSONToCSV.cpp ../MocapNET1/MocapNETLib/tools.cpp ../MocapNET1/MocapNETLib/jsonCocoSkeleton.cpp ../MocapNET1/MocapNETLib/jsonMocapNETHelpers.cpp ../../dependencies/InputParser/InputParser_C.cpp ${BVH_SOURCE} ../MocapNET1/MocapNETLib/bvh.cpp ../MocapNET1/MocapNETLib/csv.cpp)   
target_link_libraries(convertOpenPoseJSONToCSV rt dl m )
set_target_properties(convertOpenPoseJSONToCSV PROPERTIES DEBUG_POSTFIX "D") 
set_target_properties(convertOpenPoseJSONToCSV PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )


project( convertBVHToCSV )  
add_executable(convertBVHToCSV convertBVHToCSV.cpp ../MocapNET1/MocapNETLib/tools.cpp ../MocapNET1/MocapNETLib/jsonCocoSkeleton.cpp ../MocapNET1/MocapNETLib/jsonMocapNETHelpers.cpp ../../dependencies/InputParser/InputParser_C.cpp ${BVH_SOURCE} ../MocapNET1/MocapNETLib/bvh.cpp ../MocapNET1/MocapNETLib/csv.cpp)   
target_link_libraries(convertBVHToCSV rt dl m )
set_target_properties(convertBVHToCSV PROPERTIES DEBUG_POSTFIX "D") 
set_target_properties(convertBVHToCSV PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )



