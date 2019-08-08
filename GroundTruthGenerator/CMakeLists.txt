project( GroundTruthDumper ) 
cmake_minimum_required( VERSION 2.8.7 )
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_MODULE_PATH})
   

add_executable( 
               GroundTruthDumper 
               #main.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Applications/BVHTester/main.c 
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_to_tri_pose.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_trajectoryParserTRI.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_trajectoryParserPrimitives.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_export.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_bvh.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_svg.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_csv.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_export.c
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_randomize.c 
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_rename.c 
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_cut_paste.c 
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_inverseKinematics.c 
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_transform.c 
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_project.c 
              ../RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/InputParser_C.c
              ../RGBDAcquisition/tools/AmMatrix/matrix4x4Tools.c
              ../RGBDAcquisition/tools/AmMatrix/matrixOpenGL.c
              ../RGBDAcquisition/tools/AmMatrix/quaternions.c
              ../RGBDAcquisition/tools/AmMatrix/simpleRenderer.c
              )
 
target_link_libraries(GroundTruthDumper rt m  ) 
#add_dependencies(GroundTruthDumper OGLRendererSandbox)  
       
 
set_target_properties(GroundTruthDumper PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )
