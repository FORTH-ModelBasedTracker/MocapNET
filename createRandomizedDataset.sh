#!/bin/bash


function getStatistics 
{ 
  #use R to generate statistics 
  #sudo apt-get install r-base
  R -q -e "x <- read.csv('$1', header = T); summary(x); sd(x[ , 1])" > $2
  cat $1 | wc -l >> $2 
}  

red=$(printf "\033[31m")
green=$(printf "\033[32m")
yellow=$(printf "\033[33m")
blue=$(printf "\033[34m")
magenta=$(printf "\033[35m")
cyan=$(printf "\033[36m")
white=$(printf "\033[37m")
normal=$(printf "\033[m")


datasetDir="dataset/MotionCapture"
datasetSubDir="01 02 03 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19"
#datasetSubDir="05"
outputDir="dataset/"
                          #Why was this 20 , shouldn't it be 12?
RANDOMIZEANGLES="--perturbJointAngles 12 30.0 rshoulder relbow rhand lshoulder lelbow lhand rhip rknee rfoot lhip lknee lfoot"
ERASEHANDS="--eraseJoints 20 rthumb1 rthumb2 rindex1 rindex2 rmid1 rmid2 rring1 rring2 rpinky1 rpinky2 lthumb1 lthumb2 lindex1 lindex2 lmid1 lmid2 lring1 lring2 lpinky1 lpinky2"

#FAR
MINIMUM_DEPTH="1000"
MAXIMUM_DEPTH="4500" #3000 is too small

MINIMUM_POSITION="-1400 -300 $MINIMUM_DEPTH"
MINIMUM_ROTATION="-25 -178 -35"
MAXIMUM_POSITION="1400 300 $MAXIMUM_DEPTH"
MAXIMUM_ROTATION="25 178 35"
  
MIRROR_DATASET_HANDS=""
ITERATIONS="1" #Smallest size..
#ITERATIONS="2" #float32 targeting 16GB RAM
#ITERATIONS="8" #float16 targeting 16GB RAM


if ! [ -x "$(command -v R)" ]; then
  echo 'Warning: R is not installed.' >&2
  sudo apt-get install r-base 
fi 

RAM=`free | grep Mem | tr -s ' '| cut -f2 -d ' '`


if [ "$RAM" -gt "11286464" ]; then
  echo "More than 12GB";
ITERATIONS="2"  
fi
if [ "$RAM" -gt "15297500" ]; then
  echo "More than 16GB";
ITERATIONS="3"  
fi
if [ "$RAM" -gt "31861780" ]; then
  echo "More than 32GB";
ITERATIONS="16"  
fi


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
                       ./GroundTruthDumper --from $BVHFILE $MIRROR_DATASET_HANDS $RANDOMIZEANGLES $ERASEHANDS --repeat $ITERATIONS $2 --occlusions --csv $outputDir $1 2d+bvh # --bvh $outputDir/$f-random.bvh
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
#------------------------------------------------------------------------------------------------------------
#generateDataset dataBack.csv "--flipRandomizationOrientation --randomizeranges $MINIMUM_POSITION -35 -179.999999 -35 $MAXIMUM_POSITION 35 -90 35 $MINIMUM_POSITION -35 90 -35 $MAXIMUM_POSITION 35 180 35"
#generateDataset dataAll.csv "--randomize $MINIMUM_POSITION -35 -179.999999 -35 $MAXIMUM_POSITION 35 180 35"
#generateDataset dataFront.csv "--randomize $MINIMUM_POSITION -35 -90 -35 $MAXIMUM_POSITION 35 90 35"


#New way to do randomizations Position is picked in 2D and works in all depths
#   \                / 
#     \    BACK    /
#       \        /
#         \    /  
#           \/
#-----------/\-------------
#         /    \ 
#       /        \
#     /    FRONT   \
#   /                \

#------------------------------------------------------------------------------------------------------------
#generateDataset dataBack.csv "--flipRandomizationOrientation --randomize2Dranges $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -179.999999 -35 35 -90 35 -35 90 -35 35 180 35"
#generateDataset dataFront.csv "--randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -90 -35 35 90 35"
#generateDataset dataAll.csv "--randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -179.999999 -35 35 180 35"

#exit 0

#New extra fine detail
#------------------------------------------------------------------------------------------------------------
#generateDataset dataFrontA.csv "--randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -90 -35 35 0 35"
#generateDataset dataFrontB.csv "--randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 0 -35 35 90 35"
#generateDataset dataBackA.csv "--flipRandomizationOrientation --randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -90 -35 35 0 35"
#generateDataset dataBackB.csv "--flipRandomizationOrientation --randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 0 -35 35 90 35"
#generateDataset dataAll.csv "--randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -179.999999 -35 35 180 35"



#New extra extra fine detail rotated by 45 degrees
#   \                / 
#     \    BACK    /
#       \        /
#         \    /  
#           \/
#   LEFT    /\    RIGHT
#         /    \ 
#       /        \
#     /    FRONT   \
#   /                \
#------------------------------------------------------------------------------------------------------------

#I have added 20 degrees on every limit more than the default
#to have some overlap and redundancy when switching between datasets
#---------------------------
FRONT_MIN_ORIENTATION="-55" #-45  default  
FRONT_MAX_ORIENTATION="55"  # 45  default
#---------------------------
BACK_MIN_ORIENTATION="125"  # 135 default 
BACK_MAX_ORIENTATION="235"  # 225 default
#---------------------------
LEFT_MIN_ORIENTATION="-145" #-135 default
LEFT_MAX_ORIENTATION="-35"  #-45  default
#---------------------------
RIGHT_MIN_ORIENTATION="35"  # 45  default 
RIGHT_MAX_ORIENTATION="145" # 135 default



generateDataset data_front.csv "--csvOrientation front --randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 $FRONT_MIN_ORIENTATION -35 35 $FRONT_MAX_ORIENTATION 35"
generateDataset data_back.csv "--csvOrientation back --randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 $BACK_MIN_ORIENTATION -35 35 $BACK_MAX_ORIENTATION 35"
generateDataset data_left.csv "--csvOrientation left --randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 $LEFT_MIN_ORIENTATION -35 35 $LEFT_MAX_ORIENTATION 35"
generateDataset data_right.csv "--csvOrientation right --randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 $RIGHT_MIN_ORIENTATION -35 35 $RIGHT_MAX_ORIENTATION 35"
generateDataset data_all.csv "--randomize2D $MINIMUM_DEPTH $MAXIMUM_DEPTH -35 -179.999999 -35 35 180 35"

echo "Getting statistics for front view.."
cat dataset/bvh_data_front.csv | cut -d',' -f 5 > frontOrientations
getStatistics frontOrientations dataset/bvh_data_front_statistics.txt
cat dataset/bvh_data_front_statistics.txt
rm frontOrientations

echo "Getting statistics for back view.."
cat dataset/bvh_data_back.csv | cut -d',' -f 5 > backOrientations
getStatistics backOrientations dataset/bvh_data_back_statistics.txt
cat dataset/bvh_data_back_statistics.txt
rm backOrientations

echo "Getting statistics for left view.."
cat dataset/bvh_data_left.csv | cut -d',' -f 5 > leftOrientations
getStatistics leftOrientations dataset/bvh_data_left_statistics.txt
cat dataset/bvh_data_left_statistics.txt
rm leftOrientations

echo "Getting statistics for right view.."
cat dataset/bvh_data_right.csv | cut -d',' -f 5 > rightOrientations
getStatistics rightOrientations dataset/bvh_data_right_statistics.txt
cat dataset/bvh_data_right_statistics.txt
rm rightOrientations

echo "Getting statistics for all view.."
cat dataset/bvh_data_all.csv | cut -d',' -f 5 > allOrientations
getStatistics allOrientations dataset/bvh_data_all_statistics.txt
cat dataset/bvh_data_all_statistics.txt
rm allOrientations




echo "Done.."
exit 0
