#!/bin/bash

red=$(printf "\033[31m")
green=$(printf "\033[32m")
yellow=$(printf "\033[33m")
blue=$(printf "\033[34m")
magenta=$(printf "\033[35m")
cyan=$(printf "\033[36m")
white=$(printf "\033[37m")
normal=$(printf "\033[m")


datasetDir="MotionCapture"
datasetSubDir="01 02 03 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19"
#datasetSubDir="05"
outputDir="dataset/"
                          #Why is this 20 , shouldn't it be 12?
RANDOMIZEANGLES="--perturbJointAngles 20 30.0 rshoulder relbow rhand lshoulder lelbow lhand rhip rknee rfoot lhip lknee lfoot"

#FAR
MINIMUM_DEPTH="1000"
MAXIMUM_DEPTH="5500"

MINIMUM_POSITION="-1400 -300 $MINIMUM_DEPTH"
MINIMUM_ROTATION="-25 -178 -35"
MAXIMUM_POSITION="1400 300 $MAXIMUM_DEPTH"
MAXIMUM_ROTATION="25 178 35"
  
MIRROR_DATASET_HANDS=""
ITERATIONS="2" 

#We are also mirroring so bear in mind that this will effectively be doubled 
#MIRROR_DATASET_HANDS="--mirror lCollar rCollar"
#ITERATIONS="3" #*2

function generateDataset 
{
 rm $outputDir/$1
 rm $outputDir/2d_$1
 rm $outputDir/3d_$1
 rm $outputDir/bvh_$1
 for d in $datasetSubDir
           do
             if [ -d $datasetDir/$d ]
              then
              echo "Generating $1 - $green Found $d directory $normal" 
              datasetFile=`ls $datasetDir/$d | grep bvh`
              #-------------------------------------------------------
             
                for f in $datasetFile
                  do
                  echo "$green Found $datasetDir/$d/$f file $normal" 
                    #||||||||||||||||||||||||||||||||||||||||||||||||||||||||
                       BVHFILE="$datasetDir/$d/$f"                       # --svg $outputDir  
                       ./GroundTruthDumper --from $BVHFILE $MIRROR_DATASET_HANDS $RANDOMIZEANGLES --repeat $ITERATIONS $2 --occlusions --csv $outputDir $1 2d+bvh # --bvh $outputDir/$f-random.bvh
                    #|||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
                done

              #-------------------------------------------------------
             else
              echo "$red Could not find $d directory $normal"
             fi
  done
}


#./GroundTruthDumper --from MotionCapture/01/01_02.bvh --setPositionRotation 0 0 1000 0 -120 0 --occlusions --svg dataset/svg --csv dataset/ check.csv 2d+bvh# --bvh
#exit 0

#Old way to do that ( works if you don't go very far in depth )
#generateDataset dataBack.csv "--flipRandomizationOrientation --randomizeranges $MINIMUM_POSITION -35 -179.999999 -35 $MAXIMUM_POSITION 35 -90 35 $MINIMUM_POSITION -35 90 -35 $MAXIMUM_POSITION 35 180 35"
#generateDataset dataAll.csv "--randomize $MINIMUM_POSITION -35 -179.999999 -35 $MAXIMUM_POSITION 35 180 35"
#generateDataset dataFront.csv "--randomize $MINIMUM_POSITION -35 -90 -35 $MAXIMUM_POSITION 35 90 35"

#New way to do randomizations Position is picked in 2D and works in all depths
generateDataset dataBack.csv "--flipRandomizationOrientation --randomize2Dranges $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -179.999999 -35 35 -90 35 -35 90 -35 35 180 35"
generateDataset dataAll.csv "--randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -179.999999 -35 35 180 35"
generateDataset dataFront.csv "--randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -90 -35 35 90 35"




exit 0
