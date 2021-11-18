project( BVHTemplate ) 
cmake_minimum_required( VERSION 2.8.7 )
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_MODULE_PATH})
   

add_executable(BVHTemplate  main.c ${BVH_SOURCE} )
 
target_link_libraries(BVHTemplate rt m pthread ) 
#add_dependencies(BVHTemplate OGLRendererSandbox)  
       
 
set_target_properties(BVHTemplate PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )
