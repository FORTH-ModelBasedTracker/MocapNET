#!/bin/bash

#Colors for easier output
red=$(printf "\033[31m")
green=$(printf "\033[32m")
yellow=$(printf "\033[33m")
blue=$(printf "\033[34m")
magenta=$(printf "\033[35m")
cyan=$(printf "\033[36m")
white=$(printf "\033[37m")
normal=$(printf "\033[m")

#We can use R to gather statistics about the generated ground truth..
if ! [ -x "$(command -v R)" ]; then
  echo 'Warning: R is not installed.' >&2
  sudo apt-get install r-base 
fi 

function getStatistics 
{ 
  #use R to generate statistics 
  #sudo apt-get install r-base
  R -q -e "x <- read.csv('$1', header = T); summary(x); sd(x[ , 1])" > $2
  cat $1 | wc -l >> $2 
}  


#We move to the correct directory..
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"
cd ..



#Where is our datasets located, compared to the current directory..
datasetDir="dataset/MotionCapture"
#MocapNET1 -> datasetSubDir="01 02 03 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19"
datasetSubDir="h36 01 02 03 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 45 46 47 49 54 55 56 60 61 62 63 64 69 70 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 93 94 102 103 104 105 106 107 108 111 113 114 115 117 118 120 121 122 123 124 125 126 127 128 131 132 133 134 135 136 137 138 139 140 141 142 143 144"
#datasetSubDir="h36"

#Where is our output located
outputDir="dataset/"


# Use --printparams in view commands to view the commands that are given to the utility at stderr
VIEW_COMMANDS=" "

RAND_UPPER_BODY="--perturbJointAngles 2 30.0 rshoulder lshoulder --perturbJointAngles 2 16.0 relbow lelbow --perturbJointAngles 2 10.0 abdomen chest"
RAND_LOWER_BODY="--perturbJointAngles 2 30.0 rhip lhip --perturbJointAngles 4 10.0 lknee rknee lfoot rfoot --perturbJointAngles 2 10.0 abdomen chest"
 
                       
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 15 -65 0 1 finger5-1.l finger5-2.l finger5-3.l finger4-1.l finger4-2.l finger4-3.l finger3-1.l finger3-2.l finger3-3.l finger2-1.l finger2-2.l finger2-3.l lthumb finger1-2.l finger1-3.l --randomizeJointAngles 5 -90 0 1 finger5-1.l finger4-1.l finger3-1.l finger2-1.l lthumb --randomizeJointAngles 2 -75 75 2 lthumb finger1-2.l --randomizeJointAngles 1 -45 0 3 lthumb"
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 15 0 65 1 finger5-1.r finger5-2.r finger5-3.r finger4-1.r finger4-2.r finger4-3.r finger3-1.r finger3-2.r finger3-3.r finger2-1.r finger2-2.r finger2-3.r rthumb finger1-2.r finger1-3.r --randomizeJointAngles 5 0 90 1 finger5-1.r finger4-1.r finger3-1.r finger2-1.r rthumb --randomizeJointAngles 2 -75 75 2 rthumb finger1-2.r --randomizeJointAngles 1 0 45 3 rthumb"

FILTER_OUT_POSES_WITH_HANDS_IN_SIDE="--filterout 0 0 -130.0 0 90 0 1920 1080 570.7 570.3 6 rhand lhip 0 120 rhand rhip 0 120 rhand lhand 0 150 lhand rhip 0 120 lhand lhip 0 120 lhand rhand 0 150"
#FILTER_OUT_POSES_WITH_HANDS_IN_SIDE=" "

DONT_USE_END_JOINTS="0"
USE_END_JOINTS="1"

HIDEBODY="--hide2DLocationOfJoints $DONT_USE_END_JOINTS 8 abdomen chest eye.r eye.l toe1-2.r toe5-3.r toe1-2.l toe5-3.l" #We want to contain these joints in the BVH file and solve them, but they do not correspond to the BODY25 format so we hide them as 2D points..
SELECTBODY="--selectJoints $USE_END_JOINTS 23 hip eye.r eye.l abdomen chest neck head rshoulder relbow rhand lshoulder lelbow lhand rhip rknee rfoot lhip lknee lfoot toe1-2.r toe5-3.r toe1-2.l toe5-3.l $HIDEBODY"


HIDEUPPERBODY="--hide2DLocationOfJoints $DONT_USE_END_JOINTS 4 abdomen chest eye.r eye.l" #We want to contain these joints in the BVH file and solve them, but they do not correspond to the BODY25 format so we hide them as 2D points..
SELECTUPPERBODY="--selectJoints $USE_END_JOINTS 13 hip eye.r eye.l abdomen chest neck head rshoulder relbow rhand lshoulder lelbow lhand $HIDEUPPERBODY"

HIDELOWERBODY="--hide2DLocationOfJoints $DONT_USE_END_JOINTS 6 abdomen chest toe1-2.r toe5-3.r toe1-2.l toe5-3.l" 
SELECTLOWERBODY="--selectJoints $USE_END_JOINTS 14 hip abdomen chest neck rhip rknee rfoot lhip lknee lfoot toe1-2.r toe5-3.r toe1-2.l toe5-3.l $HIDELOWERBODY"



SELECT_RHAND="--selectJoints $USE_END_JOINTS 16 rhand finger5-1.r finger5-2.r finger5-3.r finger4-1.r finger4-2.r finger4-3.r finger3-1.r finger3-2.r finger3-3.r finger2-1.r finger2-2.r finger2-3.r rthumb finger1-2.r finger1-3.r"

SELECT_LHAND="--selectJoints $USE_END_JOINTS 16 lhand finger5-1.l finger5-2.l finger5-3.l finger4-1.l finger4-2.l finger4-3.l finger3-1.l finger3-2.l finger3-3.l finger2-1.l finger2-2.l finger2-3.l lthumb finger1-2.l finger1-3.l"

#FAR
MIN_DEPTH="900" #1000 original
MAX_DEPTH="4500" #3000 is too small


ITERATIONS="1" #Smallest size..
#ITERATIONS="2" #float32 targeting 16GB RAM
#ITERATIONS="8" #float16 targeting 16GB RAM



#Get number of bytes in RAM
RAM=`free | grep Mem | tr -s ' '| cut -f2 -d ' '`

if [ "$RAM" -gt "11286464" ]; then
  echo "More than 12GB";
ITERATIONS="0"  
fi
if [ "$RAM" -gt "15297500" ]; then
  echo "More than 16GB";
ITERATIONS="1"  
fi
if [ "$RAM" -gt "31861780" ]; then
  echo "More than 32GB";
ITERATIONS="2"  
fi

#Uncomment next line to override settings and make the 
#minimum training set for development
#ITERATIONS="0"


totaldatasetsToGo=`echo "$datasetSubDir" | awk '{print NF}'`


function generateDataset 
{
 datasetsProcessed=0
 rm $outputDir/$1
 rm $outputDir/count_$1
 rm $outputDir/2d_$1
 #rm $outputDir/3d_$1
 rm $outputDir/bvh_$1
 for d in $datasetSubDir
           do
             if [ -d $datasetDir/$d ]
              then
              echo "Generating $1 - $green Found $d directory $normal" 
              datasetFile=`ls $datasetDir/$d | grep bvh`
              ((datasetsProcessed++))
              #-------------------------------------------------------
             
                for f in $datasetFile
                  do
                  echo "$green Generating $1 - $datasetsProcessed/$totaldatasetsToGo - $datasetDir/$d/$f file $normal" 
                    #||||||||||||||||||||||||||||||||||||||||||||||||||||||||
                       BVHFILE="$datasetDir/$d/$f"                       # --svg $outputDir  
                       ./GroundTruthDumper $VIEW_COMMANDS --haltonerror --from $BVHFILE --filtergimballocks 4 $3 --repeat $ITERATIONS $2 --occlusions --csv $outputDir $1 2d+bvh # --bvh $outputDir/$f-random.bvh
                    #|||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
                done

              #-------------------------------------------------------
             else
              echo "$red Could not find $d directory $normal"
             fi 
  done

  #Let's count it and store the numbers so we don't have to spam disk reads on every training..
  wc -l $outputDir/2d_$1 > $outputDir/count_$1
  #wc -l $outputDir/3d_$1 >> $outputDir/count_$1
  wc -l $outputDir/bvh_$1 >> $outputDir/count_$1

}



 
#------------------------------------------------------------------------------------------------------------
#       MocapNET 1
#   \                / 
#     \    BACK    /
#       \        /
# BACK    \    /    BACK
#           \/
#-----------/\-------------
#         /    \ 
# FRONT /        \  FRONT
#     /   FRONT    \
#   /                \
 
#Old way to do that ( works if you don't go very far in depth )
#------------------------------------------------------------------------------------------------------------
#MINIMUM_POSITION="-1400 -300 $MIN_DEPTH"
#MINIMUM_ROTATION="-25 -178 -35"
#MAXIMUM_POSITION="1400 300 $MAX_DEPTH"
#MAXIMUM_ROTATION="25 178 35" 
#generateDataset dataBack.csv "--flipRandomizationOrientation --randomizeranges $MINIMUM_POSITION -35 -179.999999 -35 $MAXIMUM_POSITION 35 -90 35 $MINIMUM_POSITION -35 90 -35 $MAXIMUM_POSITION 35 180 35"
#generateDataset dataAll.csv "--randomize $MINIMUM_POSITION -35 -179.999999 -35 $MAXIMUM_POSITION 35 180 35"
#generateDataset dataFront.csv "--randomize $MINIMUM_POSITION -35 -90 -35 $MAXIMUM_POSITION 35 90 35"







#------------------------------------------------------------------------------------------------------------
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


MIN_LIM="-45"
MAX_LIM="45"


#I have added 20 degrees on every limit more than the default
#to have some overlap and redundancy when switching between datasets
#---------------------------
FRONT_MIN_ORIENTATION="-55" #-45  default , -55 with 10 deg safety  
FRONT_MAX_ORIENTATION="55"  # 45  default , 55 with 10 deg safety
#---------------------------
BACK_MIN_ORIENTATION="125"  # 135 default , 125 with 10 deg safety
BACK_MAX_ORIENTATION="235"  # 225 default , 235 with 10 deg safety
#---------------------------
LEFT_MIN_ORIENTATION="-145" #-135 default
LEFT_MAX_ORIENTATION="-35"  #-45  default
#---------------------------
RIGHT_MIN_ORIENTATION="35"  # 45  default 
RIGHT_MAX_ORIENTATION="145" # 135 default

# $FILTER_OUT_POSES_WITH_HANDS_IN_SIDE 
#rm $outputDir/2d_filteredPosesRHand.csv
#rm $outputDir/bvh_filteredPosesRHand.csv
#./GroundTruthDumper --haltonerror --filtergimballocks --from filteredPosesRHand.bvh $SELECTUPPERBODY $RAND_UPPER_BODY --csvOrientation front --randomize2D $MIN_DEPTH $MAX_DEPTH -10 -45 -0 10 45 10 $SELECTUPPERBODY --occlusions --csv $outputDir #filteredPosesRHand.csv 2d+bvh
#exit 0

FILTER_OUT="$FILTER_OUT_POSES_WITH_HANDS_IN_SIDE"

#UPPER BODY
generateDataset upperbody_all.csv "$FILTER_OUT --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM -179.999999 $MIN_LIM $MAX_LIM 180 $MAX_LIM" "$SELECTUPPERBODY" 
./ReshapeCSV dataset/bvh_upperbody_all.csv > dataset/category_upperbody_all.csv
echo "Category size = "#cat dataset/category_upperbody_all.csv | wc -c
#-----------------------------------------------
generateDataset upperbody_front.csv "$FILTER_OUT --csvOrientation front --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $FRONT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $FRONT_MAX_ORIENTATION $MAX_LIM" "$SELECTUPPERBODY $RAND_UPPER_BODY"
generateDataset upperbody_back.csv "$FILTER_OUT --csvOrientation back --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $BACK_MIN_ORIENTATION $MIN_LIM $MAX_LIM $BACK_MAX_ORIENTATION $MAX_LIM" "$SELECTUPPERBODY $RAND_UPPER_BODY"
generateDataset upperbody_left.csv "$FILTER_OUT --csvOrientation left --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $LEFT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $LEFT_MAX_ORIENTATION $MAX_LIM" "$SELECTUPPERBODY $RAND_UPPER_BODY"
generateDataset upperbody_right.csv "$FILTER_OUT --csvOrientation right --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $RIGHT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $RIGHT_MAX_ORIENTATION $MAX_LIM" "$SELECTUPPERBODY $RAND_UPPER_BODY"


#LOWER BODY
generateDataset lowerbody_all.csv "--randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM -179.999999 $MIN_LIM $MAX_LIM 180 $MAX_LIM" "$SELECTLOWERBODY" 
./ReshapeCSV dataset/bvh_lowerbody_all.csv > dataset/category_lowerbody_all.csv
echo "Category size = "
cat dataset/category_lowerbody_all.csv | wc -c
#-----------------------------------------------
generateDataset lowerbody_front.csv "--csvOrientation front --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $FRONT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $FRONT_MAX_ORIENTATION $MAX_LIM" "$SELECTLOWERBODY $RAND_LOWER_BODY"
generateDataset lowerbody_back.csv "--csvOrientation back --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $BACK_MIN_ORIENTATION $MIN_LIM $MAX_LIM $BACK_MAX_ORIENTATION $MAX_LIM" "$SELECTLOWERBODY $RAND_LOWER_BODY"
generateDataset lowerbody_left.csv "--csvOrientation left --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $LEFT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $LEFT_MAX_ORIENTATION $MAX_LIM" "$SELECTLOWERBODY $RAND_LOWER_BODY"
generateDataset lowerbody_right.csv "--csvOrientation right --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $RIGHT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $RIGHT_MAX_ORIENTATION $MAX_LIM" "$SELECTLOWERBODY $RAND_LOWER_BODY"


exit 0 # <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <>

#This is a better hand dataset generator that ignores poses
./createRandomHandDataset.sh

exit 0 # <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <>

#This hand is just based on CMU poses 
#LEFT HAND
generateDataset lhand_front.csv "--csvOrientation front --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $FRONT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $FRONT_MAX_ORIENTATION $MAX_LIM" "$SELECT_LHAND $RANDOMIZE_LEFT_HAND"
generateDataset lhand_back.csv "--csvOrientation back --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $BACK_MIN_ORIENTATION $MIN_LIM $MAX_LIM $BACK_MAX_ORIENTATION $MAX_LIM" "$SELECT_LHAND $RANDOMIZE_LEFT_HAND"
generateDataset lhand_left.csv "--csvOrientation left --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $LEFT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $LEFT_MAX_ORIENTATION $MAX_LIM" "$SELECT_LHAND $RANDOMIZE_LEFT_HAND"
generateDataset lhand_right.csv "--csvOrientation right --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $RIGHT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $RIGHT_MAX_ORIENTATION $MAX_LIM" "$SELECT_LHAND $RANDOMIZE_LEFT_HAND"

#RIGHT HAND
generateDataset rhand_front.csv "--csvOrientation front --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $FRONT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $FRONT_MAX_ORIENTATION $MAX_LIM" "$SELECT_RHAND $RANDOMIZE_RIGHT_HAND"
generateDataset rhand_back.csv "--csvOrientation back --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $BACK_MIN_ORIENTATION $MIN_LIM $MAX_LIM $BACK_MAX_ORIENTATION $MAX_LIM" "$SELECT_RHAND $RANDOMIZE_RIGHT_HAND"
generateDataset rhand_left.csv "--csvOrientation left --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $LEFT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $LEFT_MAX_ORIENTATION $MAX_LIM" "$SELECT_RHAND $RANDOMIZE_RIGHT_HAND"
generateDataset rhand_right.csv "--csvOrientation right --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $RIGHT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $RIGHT_MAX_ORIENTATION $MAX_LIM" "$SELECT_RHAND $RANDOMIZE_RIGHT_HAND"


exit 0 # <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <>


#WHOLE BODY AT ONCE
generateDataset body_all.csv "$FILTER_OUT --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM -179.999999 $MIN_LIM $MAX_LIM 180 $MAX_LIM" "$SELECTBODY" 
./ReshapeCSV dataset/bvh_body_all.csv > dataset/category_body_all.csv
echo "Category size = "
cat dataset/category_body_all.csv | wc -c
#-----------------------------------------------
generateDataset body_front.csv "$FILTER_OUT --csvOrientation front --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $FRONT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $FRONT_MAX_ORIENTATION $MAX_LIM" "$SELECTBODY"
generateDataset body_back.csv "$FILTER_OUT --csvOrientation back --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $BACK_MIN_ORIENTATION $MIN_LIM $MAX_LIM $BACK_MAX_ORIENTATION $MAX_LIM" "$SELECTBODY"
generateDataset body_left.csv "$FILTER_OUT --csvOrientation left --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $LEFT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $LEFT_MAX_ORIENTATION $MAX_LIM" "$SELECTBODY"
generateDataset body_right.csv "$FILTER_OUT --csvOrientation right --randomize2D $MIN_DEPTH $MAX_DEPTH $MIN_LIM $RIGHT_MIN_ORIENTATION $MIN_LIM $MAX_LIM $RIGHT_MAX_ORIENTATION $MAX_LIM" "$SELECTBODY"

exit 0 # <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <> <>


echo "Getting statistics for front view.."
cat dataset/bvh_body_front.csv | cut -d',' -f 5 > frontOrientations
getStatistics frontOrientations dataset/bvh_body_front_statistics.txt
cat dataset/bvh_body_front_statistics.txt
rm frontOrientations

echo "Getting statistics for back view.."
cat dataset/bvh_body_back.csv | cut -d',' -f 5 > backOrientations
getStatistics backOrientations dataset/bvh_body_back_statistics.txt
cat dataset/bvh_body_back_statistics.txt
rm backOrientations

echo "Getting statistics for left view.."
cat dataset/bvh_body_left.csv | cut -d',' -f 5 > leftOrientations
getStatistics leftOrientations dataset/bvh_body_left_statistics.txt
cat dataset/bvh_body_left_statistics.txt
rm leftOrientations

echo "Getting statistics for right view.."
cat dataset/bvh_body_right.csv | cut -d',' -f 5 > rightOrientations
getStatistics rightOrientations dataset/bvh_body_right_statistics.txt
cat dataset/bvh_body_right_statistics.txt
rm rightOrientations

echo "Getting statistics for all view.."
cat dataset/bvh_body_all.csv | cut -d',' -f 5 > allOrientations
getStatistics allOrientations dataset/bvh_body_all_statistics.txt
cat dataset/bvh_body_all_statistics.txt
rm allOrientations




echo "Done.."
exit 0
