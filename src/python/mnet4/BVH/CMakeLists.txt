project( BVHTester ) 
cmake_minimum_required( VERSION 2.8.7 )
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_MODULE_PATH})
   


IF( ENABLE_JPG )
 MESSAGE("JPGs will be included in this codec build")
 set(JPG_Libs jpeg ) 
 set(JPG_Parts jpgInput.c jpgInput.h jpgExifexternal.c jpgExifexternal.h jpgExiforient_embed.c jpgExiforient_embed.h )
 set(JPG_Includes ${CMAKE_SOURCE_DIR}/3dparty/OpenNI2/Include/ )
 add_definitions(-DUSE_JPG_FILES)
 add_definitions(-DENABLE_JPG)
ENDIF( ENABLE_JPG )


IF( ENABLE_PNG )
 MESSAGE("PNGs will be included in this codec build")
 set(PNG_Libs png ) 
 set(PNG_Parts pngInput.c pngInput.h)
 set(PNG_Includes ${CMAKE_SOURCE_DIR}/3dparty/OpenNI2/Include/ )
 add_definitions(-DUSE_PNG_FILES)
 add_definitions(-DENABLE_PNG)
ENDIF( ENABLE_PNG )

IF( ENABLE_SHADERS )
 MESSAGE("Shaders will be included in this codec build")
 set(GLEW_Libs GLEW ) #sudo apt-get install libglew-dev 
 set(GLEW_Parts  )
 set(GLEW_Includes  ) 
 add_definitions(-DUSE_GLEW)
ENDIF( ENABLE_SHADERS )

add_executable( 
               BVHTester 
               main.c 
              ../../Library/MotionCaptureLoader/bvh_loader.c
              ../../Library/MotionCaptureLoader/calculate/bvh_transform.c 
              ../../Library/MotionCaptureLoader/calculate/bvh_project.c 
              ../../Library/MotionCaptureLoader/calculate/bvh_to_tri_pose.c
              ../../Library/MotionCaptureLoader/calculate/smoothing.h
              ../../Library/MotionCaptureLoader/calculate/smoothing.c
              ../../Library/MotionCaptureLoader/import/fromBVH.c
              ../../Library/MotionCaptureLoader/export/bvh_export.c
              ../../Library/MotionCaptureLoader/export/bvh_to_c.c
              ../../Library/MotionCaptureLoader/export/bvh_to_bvh.c
              ../../Library/MotionCaptureLoader/export/bvh_to_svg.c
              ../../Library/MotionCaptureLoader/export/bvh_to_csv.c
              ../../Library/MotionCaptureLoader/export/bvh_to_json.c
              ../../Library/MotionCaptureLoader/export/bvh_to_trajectoryParserTRI.c
              ../../Library/MotionCaptureLoader/export/bvh_to_trajectoryParserPrimitives.c
              ../../Library/MotionCaptureLoader/edit/bvh_randomize.c 
              ../../Library/MotionCaptureLoader/edit/bvh_rename.c 
              ../../Library/MotionCaptureLoader/edit/bvh_remapangles.c 
              ../../Library/MotionCaptureLoader/edit/bvh_interpolate.c 
              ../../Library/MotionCaptureLoader/edit/bvh_merge.c 
              ../../Library/MotionCaptureLoader/edit/bvh_filter.c 
              ../../Library/MotionCaptureLoader/edit/bvh_cut_paste.c 
              ../../Library/MotionCaptureLoader/ik/bvh_inverseKinematics.c 
              ../../Library/MotionCaptureLoader/ik/hardcodedProblems_inverseKinematics.c
              #../../Library/MotionCaptureLoader/ik/levmar.c
              ../../Library/MotionCaptureLoader/metrics/bvh_measure.c
              ../../Library/MotionCaptureLoader/tests/test.c
              ../../Library/TrajectoryParser/InputParser_C.c
              ../../../../../tools/AmMatrix/matrix4x4Tools.c
              ../../../../../tools/AmMatrix/matrixMultiplicationOptimization.c
              ../../../../../tools/AmMatrix/matrixOpenGL.c
              ../../../../../tools/AmMatrix/quaternions.c
              ../../../../../tools/AmMatrix/simpleRenderer.c
              )
 
target_link_libraries(BVHTester rt m pthread )   
       
 
set_target_properties(BVHTester PROPERTIES 
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
                      )
