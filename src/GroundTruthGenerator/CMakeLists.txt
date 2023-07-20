project( GroundTruthDumper ) 
cmake_minimum_required( VERSION 2.8.13 )
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_MODULE_PATH})
   

add_executable( 
GroundTruthDumper 
${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Applications/BVHTester/main.c 
${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/InputParser_C.c
${BVH_SOURCE}
              )
 
target_link_libraries(GroundTruthDumper rt m pthread ) 
#add_dependencies(GroundTruthDumper OGLRendererSandbox)  
       
 
set_target_properties(GroundTruthDumper PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )
