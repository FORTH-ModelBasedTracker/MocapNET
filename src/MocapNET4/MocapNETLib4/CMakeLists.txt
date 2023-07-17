project( MocapNETLib4 )  
cmake_minimum_required(VERSION 3.5) 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Unfortunately needed for  tf_utils.cpp
set(CMAKE_CXX_STANDARD 11)  
 
include_directories(${TENSORFLOW_INCLUDE_ROOT})
  

#add_executable(MocapNETLib mocapnet.cpp ${CMAKE_SOURCE_DIR}/src/Tensorflow/tf_utils.cpp)   

add_library(MocapNETLib4 SHARED 
${BVH_SOURCE}
${OPENGL_SOURCE}
../MocapNETLib4/config.h
../MocapNETLib4/tools.h
../MocapNETLib4/PCA/PCA.h
../MocapNETLib4/NSxM/NSxM.h
../MocapNETLib4/NSxM/NSDM.h
../MocapNETLib4/NSxM/NSRM.h
../MocapNETLib4/NSxM/EDM.h
../MocapNETLib4/NSxM/calculations.h
../MocapNETLib4/NSxM/calculations.c
../MocapNETLib4/IO/inputRouting.h
../MocapNETLib4/JSON/readModelConfiguration.h
../MocapNETLib4/JSON/readListFile.h
../MocapNETLib4/mocapnet4.cpp
../MocapNETLib4/mocapnet4.h
${CMAKE_SOURCE_DIR}/dependencies/nxjson/nxjson.h
${CMAKE_SOURCE_DIR}/dependencies/nxjson/nxjson.c
${CMAKE_SOURCE_DIR}/dependencies/InputParser/InputParser_C.cpp
#Tensorflow stuff..
${TENSORFLOW_SOURCE_FILES}
)   


target_link_libraries(MocapNETLib4 rt dl m pthread ${OpenCV_LIBRARIES} ${OPENGL_LIBS} Tensorflow TensorflowFramework ${NETWORK_CLIENT_LIBRARIES} ${PNG_Libs} ${JPG_Libs} )
set_target_properties(MocapNETLib4 PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(MocapNETLib4 PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )
 
