project( MocapNETProject ) 
cmake_minimum_required( VERSION 2.8.7 )

set(CMAKE_CXX_FLAGS "-fPIC ")
set(CMAKE_C_FLAGS "-fPIC") 


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)  
set(TENSORFLOW_ROOT "/usr/local/lib/" CACHE PATH "tensorflow root") 
set(TENSORFLOW_INCLUDE_ROOT "/usr/local/include/" CACHE PATH "tensorflow include")  
 

#If our development environment has RGBDAcquisition then we can use BVH capabilities..
set(BVH_SOURCE " ")
IF(EXISTS "${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader")
add_definitions(-DUSE_BVH)
set(BVH_SOURCE 
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_to_tri_pose.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_trajectoryParserTRI.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_trajectoryParserPrimitives.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_export.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_bvh.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_svg.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_csv.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_export.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_randomize.c 
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_rename.c 
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_cut_paste.c 
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_inverseKinematics.c 
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_transform.c 
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_project.c 
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/tools/AmMatrix/matrix4x4Tools.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/tools/AmMatrix/matrixOpenGL.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/tools/AmMatrix/quaternions.c
              ${CMAKE_SOURCE_DIR}/RGBDAcquisition/tools/AmMatrix/simpleRenderer.c 
      ) 
 MESSAGE("BVH Code found and will be used..")
 #MESSAGE("${BVH_SOURCE}")
ENDIF()





find_package(OpenCV)
if (OpenCV_FOUND)
  add_definitions(-DUSE_OPENCV)
  MESSAGE("OpenCV code found and will be used..")   
ENDIF(OpenCV_FOUND)



include_directories(${TENSORFLOW_INCLUDE_ROOT})

ADD_LIBRARY(TensorflowFramework SHARED IMPORTED) 
SET_TARGET_PROPERTIES(TensorflowFramework PROPERTIES IMPORTED_LOCATION ${TENSORFLOW_ROOT}/libtensorflow_framework.so) 


ADD_LIBRARY(Tensorflow SHARED IMPORTED)
SET_TARGET_PROPERTIES(Tensorflow PROPERTIES IMPORTED_LOCATION ${TENSORFLOW_ROOT}/libtensorflow.so)  

#add_subdirectory (RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer) 
add_subdirectory (GroundTruthGenerator/)
 



#This needs Tensorflow C-API installed...
#https://www.tensorflow.org/install/lang_c
#wget https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-gpu-linux-x86_64-1.13.1.tar.gz
#pip3 show protobuf
#add_subdirectory (MocapNETStandalone/)

add_subdirectory (MocapNETLib/)
add_subdirectory (MocapNETFromJSON/)
add_subdirectory (MocapNETSimpleBenchmark/)


if (OpenCV_FOUND)
add_subdirectory (Webcam/)
add_subdirectory (WebcamAndDeepJoint/)
ENDIF(OpenCV_FOUND)


#If ammarserver is present then use it..
#IF(EXISTS "${CMAKE_SOURCE_DIR}/AmmarServer/src/AmmServerlib")
# add_subdirectory (AmmarServer/)
# add_subdirectory (MocapNETHTTP/)
#ENDIF()


# TODO FIX INSTALLATION DIRECTORIES 

#    install(TARGETS RGBDAcquisitionProject
#                    LIBRARY DESTINATION lib
#                    ARCHIVE DESTINATION lib
#                    RUNTIME DESTINATION bin)
 

