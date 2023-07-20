project( MocapNETProject ) 
cmake_minimum_required( VERSION 2.8.13 )

#Make fast, lean and platform independent binaries..
#The -fuse-ld=gold flag solves issue https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/34
#However it causes https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/38
set(CMAKE_CXX_FLAGS "-s -O3 -fPIC -march=native -mtune=native") #-fuse-ld=gold
set(CMAKE_C_FLAGS "-s -O3 -fPIC -march=native -mtune=native") #-fuse-ld=gold

OPTION(ENABLE_OPENGL OFF)
OPTION(INTEL_OPTIMIZATIONS OFF)


SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

INCLUDE(FindSSE)
FindSSE ()
IF(SSE2_FOUND)
            SET(INTEL_OPTIMIZATIONS ON)
            MESSAGE("SSE2 detected and will be used..")
ENDIF(SSE2_FOUND)

if (INTEL_OPTIMIZATIONS)
add_definitions(-DINTEL_OPTIMIZATIONS)
endif(INTEL_OPTIMIZATIONS)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


#JPEG/PNG libraries..
set(JPG_Libs jpeg)
set(PNG_Libs png) 




#------------------------------------------------------------------------------------------------------------------------------------------------
#First search for Tensorflow 2.x
#------------------------------------------------------------------------------------------------------------------------------------------------
IF(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/libtensorflow/lib/libtensorflow.so.2")
 MESSAGE("Using locally found libtensorflow v2 C-API")
 set(TENSORFLOW_ROOT "${CMAKE_SOURCE_DIR}/dependencies/libtensorflow/lib/" CACHE PATH "tensorflow root") 
 set(TENSORFLOW_INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/dependencies/libtensorflow/include/" CACHE PATH "tensorflow include")  
 set(TENSORFLOW2_FOUND true CACHE BOOL "Tensorflow 2 available")
 set(TENSORFLOW_SOURCE_FILES
      ${CMAKE_SOURCE_DIR}/src/NeuralNetworkAbstractionLayer/neuralNetworkAbstraction.cpp
      ${CMAKE_SOURCE_DIR}/src/Tensorflow/tensorflow.cpp
      ${CMAKE_SOURCE_DIR}/src/Tensorflow/tf_utils.cpp
    )
ELSEIF(EXISTS "/usr/local/lib/libtensorflow.so.2")
 MESSAGE("Using system-wide found libtensorflow v2 C-API")
 set(TENSORFLOW_ROOT "/usr/local/lib/" CACHE PATH "tensorflow root") 
 set(TENSORFLOW_INCLUDE_ROOT "/usr/local/include/" CACHE PATH "tensorflow include")
 set(TENSORFLOW2_FOUND true CACHE BOOL "Tensorflow 2 available")
 set(TENSORFLOW_SOURCE_FILES
      ${CMAKE_SOURCE_DIR}/src/NeuralNetworkAbstractionLayer/neuralNetworkAbstraction.cpp
      ${CMAKE_SOURCE_DIR}/src/Tensorflow/tensorflow.cpp
      ${CMAKE_SOURCE_DIR}/src/Tensorflow/tf_utils.cpp
    )
#------------------------------------------------------------------------------------------------------------------------------------------------
#If we can't find it then search for Tensorflow 1.x  
#------------------------------------------------------------------------------------------------------------------------------------------------
ELSEIF(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/libtensorflow/lib/libtensorflow.so")
 MESSAGE("Using locally found libtensorflow v1 C-API")
 set(TENSORFLOW_ROOT "${CMAKE_SOURCE_DIR}/dependencies/libtensorflow/lib/" CACHE PATH "tensorflow root") 
 set(TENSORFLOW_INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/dependencies/libtensorflow/include/" CACHE PATH "tensorflow include")
 set(TENSORFLOW_SOURCE_FILES
      ${CMAKE_SOURCE_DIR}/src/NeuralNetworkAbstractionLayer/neuralNetworkAbstraction.cpp
      ${CMAKE_SOURCE_DIR}/src/Tensorflow/tensorflow.cpp
      ${CMAKE_SOURCE_DIR}/src/Tensorflow/tf_utils.cpp
    )
ELSEIF(EXISTS "/usr/local/lib/libtensorflow.so")
 MESSAGE("Using system-wide found libtensorflow v1 C-API")
 set(TENSORFLOW_ROOT "/usr/local/lib/" CACHE PATH "tensorflow root") 
 set(TENSORFLOW_INCLUDE_ROOT "/usr/local/include/" CACHE PATH "tensorflow include")
 set(TENSORFLOW_SOURCE_FILES
      ${CMAKE_SOURCE_DIR}/src/NeuralNetworkAbstractionLayer/neuralNetworkAbstraction.cpp
      ${CMAKE_SOURCE_DIR}/src/Tensorflow/tensorflow.cpp
      ${CMAKE_SOURCE_DIR}/src/Tensorflow/tf_utils.cpp
    )
#------------------------------------------------------------------------------------------------------------------------------------------------
#If we can't find any Tensorflow just point to the /usr/lib and hope for the best.. 
#------------------------------------------------------------------------------------------------------------------------------------------------
ELSE()
 MESSAGE("Did not find a tensorflow version, please consider running the initialize.sh script or reading the README file")
 #Set some default tensorflow paths to give some valid error afterwards..
 set(TENSORFLOW_ROOT "/usr/local/lib/" CACHE PATH "tensorflow root") 
 set(TENSORFLOW_INCLUDE_ROOT "/usr/local/include/" CACHE PATH "tensorflow include")  
ENDIF()
#------------------------------------------------------------------------------------------------------------------------------------------------


include_directories(${TENSORFLOW_INCLUDE_ROOT})
ADD_LIBRARY(TensorflowFramework SHARED IMPORTED) 

IF (TENSORFLOW2_FOUND)
  SET_TARGET_PROPERTIES(TensorflowFramework PROPERTIES IMPORTED_LOCATION ${TENSORFLOW_ROOT}/libtensorflow_framework.so.2) 
ELSE(TENSORFLOW2_FOUND)
  SET_TARGET_PROPERTIES(TensorflowFramework PROPERTIES IMPORTED_LOCATION ${TENSORFLOW_ROOT}/libtensorflow_framework.so) 
ENDIF (TENSORFLOW2_FOUND)

ADD_LIBRARY(Tensorflow SHARED IMPORTED)

IF (TENSORFLOW2_FOUND)
 SET_TARGET_PROPERTIES(Tensorflow PROPERTIES IMPORTED_LOCATION ${TENSORFLOW_ROOT}/libtensorflow.so.2)  
ELSE(TENSORFLOW2_FOUND)
 SET_TARGET_PROPERTIES(Tensorflow PROPERTIES IMPORTED_LOCATION ${TENSORFLOW_ROOT}/libtensorflow.so)  
ENDIF (TENSORFLOW2_FOUND)




#If our development environment has RGBDAcquisition then we can use BVH capabilities..
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
set(BVH_SOURCE "")
IF(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader")
add_definitions(-DUSE_BVH)
set(BVH_SOURCE 
              #--------------------------------------------------------------------------------
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/PThreadWorkerPool/pthreadWorkerPool.h
              #--------------------------------------------------------------------------------
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/bvh_loader.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/calculate/bvh_to_tri_pose.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/calculate/bvh_transform.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/calculate/bvh_project.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/import/fromBVH.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_trajectoryParserTRI.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_trajectoryParserPrimitives.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_export.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_c.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_bvh.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_svg.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_csv.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_to_json.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/export/bvh_export.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_randomize.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_filter.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_rename.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_merge.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_cut_paste.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_interpolate.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/edit/bvh_remapangles.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/ik/bvh_inverseKinematics.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/ik/hardcodedProblems_inverseKinematics.c
              #${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/ik/levmar.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/metrics/bvh_measure.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/tests/test.c
              #--------------------------------------------------------------------------------
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/Calibration/calibration.h 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/Calibration/calibration.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/Calibration/transform.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/Calibration/transform.h 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/Calibration/undistort.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/Calibration/undistort.h 
              #--------------------------------------------------------------------------------
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/AmMatrix/matrixTools.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/AmMatrix/matrix3x3Tools.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/AmMatrix/matrix4x4Tools.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/AmMatrix/matrixOpenGL.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/AmMatrix/matrixCalculations.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/AmMatrix/solveLinearSystemGJ.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/AmMatrix/solveHomography.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/AmMatrix/quaternions.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/AmMatrix/simpleRenderer.c 
      )
 MESSAGE("BVH Code found and will be used..")
 add_subdirectory(src/GroundTruthGenerator/) #Ground Truth generator can only be compiled with BVH code..
 add_subdirectory(src/MocapNET2/reshapeCSVFileToMakeClassification/) #Orientation classification needs this tool to be built before working..
 #MESSAGE("${BVH_SOURCE}")
ENDIF()
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------





#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
set(OPENGL_LIBS   "")
set(OPENGL_SOURCE "")
if (ENABLE_OPENGL)
IF(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library")

add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/Codecs)
add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/tools/Calibration)
#add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library) #OpenGLAcquisition
add_definitions(-DUSE_OPENGL)
#add_definitions(-DUSE_GLEW)
set(OPENGL_LIBS  rt m GL GLU GLEW X11 Codecs CalibrationLibrary)
set(OPENGL_SOURCE 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/OpenGLAcquisition.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/OpenGLAcquisition.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/System/glx.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/System/glx.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/System/glx2.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/System/glx2.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/System/glx3.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/System/glx3.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/main.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/OGLRendererSandbox.h
              #3D Models and how to load them  
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader_obj.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader_obj.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader_tri.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader_tri.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_processor.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_processor.h 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader_hardcoded.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader_hardcoded.h 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader_transform_joints.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_loader_transform_joints.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_converter.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_converter.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_editor.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader/model_editor.h
              #Textures and how to load them 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TextureLoader/texture_loader.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TextureLoader/texture_loader.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TextureLoader/image_proc.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TextureLoader/image_proc.h
               #OpenGL Rendering stuff  
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/FixedPipeline/ogl_fixed_pipeline_renderer.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/FixedPipeline/ogl_fixed_pipeline_renderer.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ShaderPipeline/render_buffer.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ShaderPipeline/render_buffer.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ShaderPipeline/uploadGeometry.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ShaderPipeline/uploadGeometry.c 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ShaderPipeline/ogl_shader_pipeline_renderer.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ShaderPipeline/ogl_shader_pipeline_renderer.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ShaderPipeline/shader_loader.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ShaderPipeline/shader_loader.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ogl_rendering.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/ogl_rendering.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/tiledRenderer.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/tiledRenderer.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/downloadFromRenderer.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Rendering/downloadFromRenderer.h
            
 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Tools/tools.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Tools/tools.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Tools/save_to_file.h 
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Tools/save_to_file.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Scene/scene.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Scene/scene.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Scene/control.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Scene/control.c
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Scene/photoShootingScene.h
              ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Scene/photoShootingScene.c
            #AmMatrix dependencies 
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/quaternions.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/quaternions.h
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/matrix3x3Tools.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/matrix3x3Tools.h
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/matrix4x4Tools.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/matrix4x4Tools.h
            #../tools/AmMatrix/matrixProject.c
            #../tools/AmMatrix/matrixProject.h
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/matrixCalculations.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/matrixCalculations.h
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/matrixOpenGL.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/matrixOpenGL.h
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/solveLinearSystemGJ.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/AmMatrix/solveLinearSystemGJ.h 
            #ImageOperations dependencies
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/ImageOperations/depthClassifier.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/ImageOperations/convolutionFilter.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/ImageOperations/imageFilters.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/ImageOperations/findSubImage.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/ImageOperations/imageOps.c   
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/ImageOperations/patchComparison.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/ImageOperations/patchComparison.h 
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition//tools/ImageOperations/resize.c 
            #Rest of the stuff  
            #${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/InputParser_C.c
            #${CMAKE_SOURCE_DIR}/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/InputParser_C.h
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/TrajectoryParser.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/TrajectoryParser.h 
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/TrajectoryParserDataStructures.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/TrajectoryParserDataStructures.h 
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/TrajectoryCalculator.c
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/TrajectoryCalculator.h  
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/TrajectoryPrimitives.c 
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/TrajectoryPrimitives.h
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/hashmap.c 
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser/hashmap.h
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Interfaces/webInterface.c 
            ${CMAKE_SOURCE_DIR}/dependencies/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/Interfaces/webInterface.h
      ) 
 MESSAGE("OpenGL code found and will be used..")
ELSE()
 MESSAGE("OpenGL support enabled, but OpenGL code not found..")
ENDIF()
ELSE (ENABLE_OPENGL) 
 MESSAGE("OpenGL support will not be compiled in")
ENDIF(ENABLE_OPENGL)

#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------



#Autosearch for the opencv maybe installed by the user..!
find_package(OpenCV PATHS dependencies/opencv-3.2.0/build NO_DEFAULT_PATH)
find_package (OpenCV)
if (OpenCV_FOUND)
  add_definitions(-DUSE_OPENCV)
  MESSAGE("OpenCV code found and will be used..")   
  MESSAGE(${OpenCV_DIR})   
ENDIF(OpenCV_FOUND)

 
 
#If ammarserver is present then use it..
set(NETWORK_CLIENT_LIBRARIES   "")
IF(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/AmmarServer/src/AmmServerlib")
 add_definitions(-DUSE_NETWORKING)
 MESSAGE("AmmarServer support will be compiled in")
 #add_subdirectory (dependencies/AmmarServer/) #You can force add everything but this adds too much to the build time
 set(NETWORK_CLIENT_LIBRARIES "AmmClient")
 add_subdirectory (dependencies/AmmarServer/src/AmmClient)
 add_subdirectory (dependencies/AmmarServer/src/AmmServerlib)
 add_subdirectory (dependencies/AmmarServer/src/Hashmap)
 add_subdirectory (dependencies/AmmarServer/src/InputParser) 
ENDIF()


#This needs Tensorflow C-API installed...
#https://www.tensorflow.org/install/lang_c
#wget https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-gpu-linux-x86_64-1.13.1.tar.gz
#pip3 show protobuf
#add_subdirectory (MocapNETStandalone/)


IF(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/AmmarServer/src/AmmServerlib")
 #add_subdirectory (src/MocapNET1/MocapNETServerHTTP/)
ENDIF()

if (OpenCV_FOUND)
add_subdirectory (src/Webcam/)
#Deactivated to keep build targets  sane
add_subdirectory(src/MocapNET2/BVHGUI2)
add_subdirectory(src/MocapNET2/MocapNET2LiveWebcamDemo)
ENDIF(OpenCV_FOUND)

#add_subdirectory(src/MocapNET2/BVHTemplate/)
add_subdirectory(src/MocapNET2/MocapNETLib2/)
add_subdirectory(src/MocapNET2/MocapNETFromCSV/)
add_subdirectory(src/MocapNET2/Converters/Openpose)
add_subdirectory(src/MocapNET2/Converters/H36M)
add_subdirectory(src/MocapNET2/Converters/convertCSV3D)
add_subdirectory(src/MocapNET2/testCSV/)
add_subdirectory(src/MocapNET2/drawCSV/)
add_subdirectory(src/MocapNET2/CSVClusterPlot)
#------------------------------------------------------------------
add_subdirectory(src/JointEstimator2D)


# TODO FIX INSTALLATION DIRECTORIES 

#    install(TARGETS RGBDAcquisitionProject
#                    LIBRARY DESTINATION lib
#                    ARCHIVE DESTINATION lib
#                    RUNTIME DESTINATION bin)
 

