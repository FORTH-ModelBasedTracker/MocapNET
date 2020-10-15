#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"
cd ..

red=$(printf "\033[31m")
green=$(printf "\033[32m")
yellow=$(printf "\033[33m")
blue=$(printf "\033[34m")
magenta=$(printf "\033[35m")
cyan=$(printf "\033[36m")
white=$(printf "\033[37m")
normal=$(printf "\033[m")

SELECTBODY="--selectJoints 21 hip abdomen chest neck head rcollar rshoulder relbow rhand lcollar lshoulder lelbow lhand rhip rknee rfoot lhip lknee lfoot toe3-2.r toe3-2.l"
SELECTRHAND="--selectJoints 11 rhand rthumb1 rthumb2 rindex1 rindex2 rmid1 rmid2 rring1 rring2 rpinky1 rpinky2"
SELECTLHAND="--selectJoints 11 lhand lthumb1 lthumb2 lindex1 lindex2 lmid1 lmid2 lring1 lring2 lpinky1 lpinky2"

datasetDir="dataset/MotionCapture"
datasetSubDir="01 02 03 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19"
outputDir="dataset/"

rm $outputDir/2d_body_test.csv
rm $outputDir/3d_body_test.csv
rm $outputDir/bvh_body_test.csv

#create a small test file with one example  
#rm $outputDir/testSMALL.csv
# ./GroundTruthDumper --from MotionCapture/01/01_01.bvh  --offsetPositionRotation 0 750 2500 0 0 0 --maxFrames 1 --csv $outputDir testSMALL.csv 
 
#PRODUCESVG="--svg $outputDir/svg"
PRODUCESVG=" "


datasetsProcessed=0
totaldatasetsToGo=`echo "$datasetSubDir" | awk '{print NF}'`

rm $outputDir/test.csv
 for d in $datasetSubDir
           do  
             if [ -d $datasetDir/$d ]
              then
              echo "$green Found $d directory $normal" 
              datasetFile=`ls $datasetDir/$d | grep bvh`
              ((datasetsProcessed++)) 
              #-------------------------------------------------------
             
                for f in $datasetFile
                  do
                  echo "$green Generating Test - $datasetsProcessed/$totaldatasetsToGo - $datasetDir/$d/$f file $normal" 
                    #||||||||||||||||||||||||||||||||||||||||||||||||||||||||
                       BVHFILE="$datasetDir/$d/$f"                       # --svg $outputDir 
                       #./GroundTruthDumper --from $BVHFILE  --csv $outputDir test.csv# --bvh $outputDir/$f-random.bvh
                       ./GroundTruthDumper --from $BVHFILE  $SELECTBODY --offsetPositionRotation 0 750 2000 0 0 0 --occlusions --csv $outputDir body_test.csv 2d+bvh $PRODUCESVG #--to ~/Documents/Programming/RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/Scenes/testBVH.conf --bvh $outputDir/$f-random.bvh --svg $outputDir/svg
                       #exit 0
                       #./GroundTruthDumper --from $BVHFILE --setPositionRotation 0 400 0 0 0 0  --csv $outputDir test.csv# --bvh $outputDir/$f-random.bvh
                    #|||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
                done

              #-------------------------------------------------------
             else
              echo "$red Could not find $d directory $normal"
             fi
  done 
  
exit 0
