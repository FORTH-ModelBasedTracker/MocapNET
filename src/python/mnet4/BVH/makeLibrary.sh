#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

echo "JIT Python/C Compilation *made by AmmarTM* handled by : "
gcc --version

#in case of a build after ./batherFiles.sh
BVHTESTER_DIR="opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Applications/BVHTester"
AMMATRIX_DIRECTORY="tools/AmMatrix"
MODELLOADER_DIRECTORY="opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/ModelLoader"
BVH_DIRECTORY="opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader"
INPUTPARSER_DIRECTORY="opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/TrajectoryParser"

BVHTESTER_DIR="."
AMMATRIX_DIRECTORY="../../../../../tools/AmMatrix"
MODELLOADER_DIRECTORY="../../Library/ModelLoader"
BVH_DIRECTORY="../../Library/MotionCaptureLoader"
INPUTPARSER_DIRECTORY="../../Library/TrajectoryParser"

SOURCE="
$AMMATRIX_DIRECTORY/matrix3x3Tools.c
$AMMATRIX_DIRECTORY/matrix3x3Tools.h
$AMMATRIX_DIRECTORY/matrix4x4Tools.c
$AMMATRIX_DIRECTORY/matrix4x4Tools.h
$AMMATRIX_DIRECTORY/matrixCalculations.c
$AMMATRIX_DIRECTORY/matrixCalculations.h
$AMMATRIX_DIRECTORY/matrixOpenGL.c
$AMMATRIX_DIRECTORY/matrixOpenGL.h
$AMMATRIX_DIRECTORY/quaternions.c
$AMMATRIX_DIRECTORY/quaternions.h
$AMMATRIX_DIRECTORY/simpleRenderer.c
$AMMATRIX_DIRECTORY/simpleRenderer.h
$AMMATRIX_DIRECTORY/solveLinearSystemGJ.c
$AMMATRIX_DIRECTORY/solveLinearSystemGJ.h
$MODELLOADER_DIRECTORY/hardcoded_shapes.h
$MODELLOADER_DIRECTORY/model_converter.c
$MODELLOADER_DIRECTORY/model_converter.h
$MODELLOADER_DIRECTORY/model_editor.c
$MODELLOADER_DIRECTORY/model_editor.h
$MODELLOADER_DIRECTORY/model_loader.h
$MODELLOADER_DIRECTORY/model_loader_assimp.h
$MODELLOADER_DIRECTORY/model_loader_hardcoded.h
$MODELLOADER_DIRECTORY/model_loader_obj.h
$MODELLOADER_DIRECTORY/model_loader_setup.h
$MODELLOADER_DIRECTORY/model_loader_transform_joints.c
$MODELLOADER_DIRECTORY/model_loader_transform_joints.h
$MODELLOADER_DIRECTORY/model_loader_tri.c
$MODELLOADER_DIRECTORY/model_loader_tri.h
$MODELLOADER_DIRECTORY/model_processor.c
$MODELLOADER_DIRECTORY/model_processor.h
$MODELLOADER_DIRECTORY/tri_bvh_controller.h
$BVH_DIRECTORY/bvh_loader.c
$BVH_DIRECTORY/bvh_loader.h
$BVH_DIRECTORY/calculate/bvh_project.c
$BVH_DIRECTORY/calculate/bvh_project.h
$BVH_DIRECTORY/calculate/bvh_to_tri_pose.c
$BVH_DIRECTORY/calculate/bvh_to_tri_pose.h
$BVH_DIRECTORY/calculate/smoothing.c
$BVH_DIRECTORY/calculate/smoothing.h
$BVH_DIRECTORY/calculate/bvh_transform.c
$BVH_DIRECTORY/calculate/bvh_transform.h
$BVH_DIRECTORY/edit/bvh_cut_paste.c
$BVH_DIRECTORY/edit/bvh_cut_paste.h
$BVH_DIRECTORY/edit/bvh_filter.c
$BVH_DIRECTORY/edit/bvh_filter.h
$BVH_DIRECTORY/edit/bvh_interpolate.c
$BVH_DIRECTORY/edit/bvh_interpolate.h
$BVH_DIRECTORY/edit/bvh_merge.c
$BVH_DIRECTORY/edit/bvh_merge.h
$BVH_DIRECTORY/edit/bvh_randomize.c
$BVH_DIRECTORY/edit/bvh_randomize.h
$BVH_DIRECTORY/edit/bvh_remapangles.c
$BVH_DIRECTORY/edit/bvh_remapangles.h
$BVH_DIRECTORY/edit/bvh_rename.c
$BVH_DIRECTORY/edit/bvh_rename.h
$BVH_DIRECTORY/edit/cTextFileToMemory.h
$BVH_DIRECTORY/export/bvh_export.c
$BVH_DIRECTORY/export/bvh_export.h
$BVH_DIRECTORY/export/bvh_to_bvh.c
$BVH_DIRECTORY/export/bvh_to_bvh.h
$BVH_DIRECTORY/export/bvh_to_c.c
$BVH_DIRECTORY/export/bvh_to_c.h
$BVH_DIRECTORY/export/bvh_to_csv.c
$BVH_DIRECTORY/export/bvh_to_csv.h
$BVH_DIRECTORY/export/bvh_to_svg.c
$BVH_DIRECTORY/export/bvh_to_svg.h
$BVH_DIRECTORY/export/bvh_to_json.c
$BVH_DIRECTORY/export/bvh_to_json.h
$BVH_DIRECTORY/export/bvh_to_trajectoryParserPrimitives.c
$BVH_DIRECTORY/export/bvh_to_trajectoryParserPrimitives.h
$BVH_DIRECTORY/export/bvh_to_trajectoryParserTRI.c
$BVH_DIRECTORY/export/bvh_to_trajectoryParserTRI.h
$BVH_DIRECTORY/ik/bvh_inverseKinematics.c
$BVH_DIRECTORY/ik/bvh_inverseKinematics.h
$BVH_DIRECTORY/ik/hardcodedProblems_inverseKinematics.c
$BVH_DIRECTORY/ik/hardcodedProblems_inverseKinematics.h
$BVH_DIRECTORY/ik/levmar.c
$BVH_DIRECTORY/ik/levmar.h
$BVH_DIRECTORY/import/fromBVH.c
$BVH_DIRECTORY/import/fromBVH.h
$BVH_DIRECTORY/metrics/bvh_measure.c
$BVH_DIRECTORY/metrics/bvh_measure.h
$BVH_DIRECTORY/tests/test.c
$BVH_DIRECTORY/tests/test.h
$INPUTPARSER_DIRECTORY/InputParser_C.c
$INPUTPARSER_DIRECTORY/InputParser_C.h
$BVHTESTER_DIR/bvhLibrary.h
$BVHTESTER_DIR/bvhConverter.c
"

#$BVHTESTER_DIR/main.c <- This used to be in the same binary with the BVHTester utility, now its split..

INTEL_OPTIMIZATIONS=`cat /proc/cpuinfo | grep sse3`

if [ -z "$var" ] ; then
 echo "No intel optimizations available"
 EXTRA_FLAGS=" "
else
 echo "Intel Optimizations available and will be used"
 EXTRA_FLAGS="-DINTEL_OPTIMIZATIONS"
fi
 

gcc -shared -o libBVHConverter.so -O3 -fPIC $EXTRA_FLAGS -march=native -mtune=native -lm -DBVH_USE_AS_A_LIBRARY $SOURCE



exit 0
