project( MocapNETLib2 )  
cmake_minimum_required(VERSION 3.5) 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Unfortunately needed for  tf_utils.cpp
set(CMAKE_CXX_STANDARD 11)  
 
include_directories(${TENSORFLOW_INCLUDE_ROOT})
  

#add_executable(MocapNETLib mocapnet.cpp ${CMAKE_SOURCE_DIR}/src/Tensorflow/tf_utils.cpp)   

add_library(MocapNETLib2 SHARED
${BVH_SOURCE}
${OPENGL_SOURCE}
../MocapNETLib2/config.h
../MocapNETLib2/core/core.cpp
../MocapNETLib2/core/core.hpp
../MocapNETLib2/core/singleThreaded.cpp
../MocapNETLib2/core/singleThreaded.hpp
../MocapNETLib2/core/multiThreaded.cpp
../MocapNETLib2/core/multiThreaded.hpp
../MocapNETLib2/mocapnet2.cpp 
../MocapNETLib2/tools.cpp
../MocapNETLib2/remoteExecution.cpp
../MocapNETLib2/applicationLogic/parseCommandlineOptions.cpp
../MocapNETLib2/applicationLogic/poseRecognition.cpp
../MocapNETLib2/applicationLogic/gestureRecognition.cpp
../MocapNETLib2/applicationLogic/artifactRecognition.cpp
../MocapNETLib2/qualityControl/qualityControl.cpp
../MocapNETLib2/postProcessing/outputFiltering.hpp
../MocapNETLib2/NSDM/generated_body.hpp
../MocapNETLib2/NSDM/generated_upperbody.hpp
../MocapNETLib2/NSDM/generated_lowerbody.hpp
../MocapNETLib2/IO/bvh.cpp
../MocapNETLib2/IO/commonSkeleton.hpp
../MocapNETLib2/IO/skeletonAbstraction.cpp
../MocapNETLib2/IO/skeletonSerializedToBVHTransform.hpp
../MocapNETLib2/IO/csvRead.cpp
../MocapNETLib2/IO/csvWrite.cpp
../MocapNETLib2/IO/jsonRead.cpp
../MocapNETLib2/IO/jsonMocapNETHelpers.cpp
../MocapNETLib2/IO/conversions.cpp
../MocapNETLib2/visualization/rgb.cpp
../MocapNETLib2/visualization/allInOne.cpp
../MocapNETLib2/visualization/widgets.cpp
../MocapNETLib2/visualization/visualization.cpp
../MocapNETLib2/visualization/drawSkeleton.cpp
../MocapNETLib2/visualization/opengl.cpp
../MocapNETLib2/visualization/camera_ready.cpp
../MocapNETLib2/visualization/map.cpp
../MocapNETLib2/visualization/template.cpp
../MocapNETLib2/solutionParts/body.cpp
../MocapNETLib2/solutionParts/upperBody.cpp
../MocapNETLib2/solutionParts/lowerBody.cpp
${CMAKE_SOURCE_DIR}/dependencies/InputParser/InputParser_C.cpp
#Tensorflow stuff..
${TENSORFLOW_SOURCE_FILES}
)   


target_link_libraries(MocapNETLib2 rt dl m pthread ${OpenCV_LIBRARIES} ${OPENGL_LIBS} Tensorflow TensorflowFramework ${NETWORK_CLIENT_LIBRARIES} ${PNG_Libs} ${JPG_Libs} )
set_target_properties(MocapNETLib2 PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(MocapNETLib2 PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )
 
