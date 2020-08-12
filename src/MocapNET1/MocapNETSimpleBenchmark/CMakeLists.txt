project( MocapNETBenchmark )
cmake_minimum_required(VERSION 3.5) 


add_executable(MocapNETBenchmark  ${BVH_SOURCE} benchmark.cpp ../MocapNETLib/tools.cpp ../MocapNETLib/jsonCocoSkeleton.cpp ../MocapNETLib/jsonMocapNETHelpers.cpp ../MocapNETLib/remoteExecution.cpp ../../../dependencies/InputParser/InputParser_C.cpp  ../MocapNETLib/gestureRecognition.cpp ../MocapNETLib/outputFiltering.cpp ../MocapNETLib/bvh.cpp ../MocapNETLib/csv.cpp ../../Tensorflow/tensorflow.cpp ../../Tensorflow/tf_utils.cpp)   
target_link_libraries(MocapNETBenchmark rt dl m ${NETWORK_CLIENT_LIBRARIES}  Tensorflow  TensorflowFramework MocapNETLib)
set_target_properties(MocapNETBenchmark PROPERTIES DEBUG_POSTFIX "D") 
       

set_target_properties(MocapNETBenchmark PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )

