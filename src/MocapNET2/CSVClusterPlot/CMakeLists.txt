project( CSVClusterPlot ) 
cmake_minimum_required( VERSION 2.8.13 )   
#cmake_minimum_required(VERSION 3.5)
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
include_directories(${TENSORFLOW_INCLUDE_ROOT})


add_executable(
CSVClusterPlot 
csvClusterPlot.cpp  
perform2DClustering.cpp 
perform3DClustering.cpp 
${BVH_SOURCE}
)

target_link_libraries(CSVClusterPlot rt dl m ${OpenCV_LIBRARIES} ${OPENGL_LIBS} Tensorflow  TensorflowFramework MocapNETLib2 )
#set_target_properties(CSVClusterPlot PROPERTIES DEBUG_POSTFIX "D") 


set_target_properties(CSVClusterPlot PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

