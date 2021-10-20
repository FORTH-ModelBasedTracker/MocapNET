#!/bin/bash
 

# Ansi color code variables
red="\e[0;91m"
blue="\e[0;94m"
expand_bg="\e[K"
blue_bg="\e[0;104m${expand_bg}"
red_bg="\e[0;101m${expand_bg}"
green_bg="\e[0;102m${expand_bg}"
green="\e[0;92m"
white="\e[0;97m"
bold="\e[1m"
uline="\e[4m"
reset="\e[0m"


function checkForError
{ 
RESULT=$?
if [ $RESULT -eq 0 ]; then
  echo "Successfully ran the hand pose estimation"
else
  echo -e "${red_bg}TERMINATING SCRIPT${reset}"
  echo -e "${red}Failed running $1${reset}"
  exit 1
fi
}


#We move to the correct directory..
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"
cd ..


if [ $# -eq 0 ]
  then
    echo "No arguments supplied, please run with --quaternions or --euler"
    exit 0
fi


#Setting this to qbvh will activate 
#quaternion code..!
BVH_TYPE="bvh"

if [ "$1" == "--quaternions" ]; then
  BVH_TYPE="qbvh"
elif [ "$1" == "--euler" ]; then
  BVH_TYPE="bvh"
else
 echo "You gave arguments but where not what we expected.."
 exit 0
fi


#FULL SIZE EXPERIMENT
echo "Doing full size experiment"
FULL_RANDOM_POSES="1999999"
ZERO_POSES="199999"
BASE_POSES="599999"
FINGER_POSES="399999"
 
#SMALL SIZE (1/2) EXPERIMENT
#echo "Doing small (1/2) size experiment"
#FULL_RANDOM_POSES="1000000"
#ZERO_POSES="100000"
#BASE_POSES="300000"
#FINGER_POSES="200000"


#SMALL SIZE (1/10) EXPERIMENT
#echo "Doing small (1/10) size experiment"
#FULL_RANDOM_POSES="199999"
#ZERO_POSES="19999"
#BASE_POSES="59999"
#FINGER_POSES="39999"



#BASE_POSES="99999"
#FINGER_POSES="19999" 

USE_OCCLUSIONS="" # Set to "--occlusions" for occlusions #deactivated @ 2021-03-05

USE_END_JOINTS="1" # We need end joints since they are our fingertips..

#Where is our output located
outputDir="dataset/"

# Use --printparams in view commands to view the commands that are given to the utility at stderr
VIEW_COMMANDS=" --printparams "

HIDEBODY=" $DONT_USE_END_JOINTS 8 abdomen chest eye.r eye.l toe1-2.r toe5-3.r toe1-2.l toe5-3.l" 

SELECT_RHAND="--selectJoints $USE_END_JOINTS 17 rHand finger5-1.r finger5-2.r finger5-3.r finger4-1.r finger4-2.r finger4-3.r finger3-1.r finger3-2.r finger3-3.r finger2-1.r finger2-2.r finger2-3.r rthumbBase rthumb finger1-2.r finger1-3.r --hide2DLocationOfJoints 0 1 rthumbBase"
#Note on --hide2DLocationOfJoints: We want to contain these joints in the BVH file and solve them, but they do not correspond to the BODY25 format so we hide them as 2D points..

SELECT_LHAND="--selectJoints $USE_END_JOINTS 17 lHand finger5-1.l finger5-2.l finger5-3.l finger4-1.l finger4-2.l finger4-3.l finger3-1.l finger3-2.l finger3-3.l finger2-1.l finger2-2.l finger2-3.l lthumbBase lthumb finger1-2.l finger1-3.l --hide2DLocationOfJoints 0 1 lthumbBase"
#Note on --hide2DLocationOfJoints: We want to contain these joints in the BVH file and solve them, but they do not correspond to the BODY25 format so we hide them as 2D points..

#                                         LEFT      RIGHT
MID_THUMBBASE_Y="63" #Thumb   Left/Right -56->48   -48->56
MID_FINGER5_1_Y="54" #Pinky   Left/Right -10->22   -22->10
MID_FINGER4_1_Y="39" #Ring    Left/Right -12->12   -12->12
MID_FINGER3_1_Y="24" #Mid     Left/Right -12->12   -12->12
MID_FINGER2_1_Y="24" #Pointer Left/Right -12->12   -12->12

NATURAL_LHAND="--set 12 -5.0 --set 39 5.0 --set 54 10.5 --set 63 -18"

NATURAL_RHAND="--set 12 5.0 --set 39 -5.0 --set 54 -10.5 --set 63 18"

#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------


function generateRightHandDataset 
{
OUTBASE="$1"

#Make sure file is wiped before
#adding new poses..
#-------------------------
rm $outputDir/2d_$OUTBASE
rm $outputDir/bvh_$OUTBASE
#-------------------------

MIN_HAND_DEPTH="$2"
MAX_HAND_DEPTH="$3" #1500
MIN_RHAND_RZ="$4"
MAX_RHAND_RZ="$7"

MIN_RHAND_RY="$5"
MAX_RHAND_RY="$8"

MIN_RHAND_RX="$6"
MAX_RHAND_RX="$9"

FROM="--haltonerror $VIEW_COMMANDS --from dataset/rhand.$BVH_TYPE"

RANDOM_POSITION_ROTATION="--randomize2D $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_RHAND_RZ $MIN_RHAND_RY $MIN_RHAND_RX $MAX_RHAND_RZ $MAX_RHAND_RY $MAX_RHAND_RX $USE_OCCLUSIONS"

OUT="--csv $outputDir $OUTBASE 2d+bvh "

#RHAND
#./BVHGUI2 --from dataset/rhand.qbvh --set 2 -25 --set 3 0.5 --set 4 -0.5 --set 5 0.5 --set 6 -0.5  --set 12 5.0 --set 39 -5.0 --set 54 -10.5 --set 63 18

echo "TEST?"
#New automatic pose generation based on IK Problem..
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FULL_RANDOM_POSES $SELECT_RHAND --randomizeBasedOnIKConstrains rhand $RANDOM_POSITION_ROTATION $OUT
checkForError "rhand full random"

#Default BVH poses with everything at zero but hand is naturally open
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $BASE_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION  $OUT
checkForError "rhand naturally open"

#Default BVH poses with everything at zero but hand is not naturally open, it is has fingers touching 
./GroundTruthDumper $FROM --repeat $ZERO_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $OUT
checkForError "rhand to zero"

#Default BVH poses with hand naturally open but thumb in the middle
./GroundTruthDumper $FROM $NATURAL_RHAND --set 63 -6.5 --set 64 28.7 --set 65 -8.5 --set 66 -37 --set 67 13 --set 68 -7.6 --set 69 22 --set 70 5 --set 71 -43 --set 72 0 --repeat $ZERO_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $OUT
checkForError "rhand naturally open thumb in the middle"

#Right hand Thumb Movement on default pose
./GroundTruthDumper $FROM  $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION  --randomizeMID $MID_THUMBBASE_Y -48.0 56.0 $OUT
checkForError "Right hand Thumb Movement on default pose"

#All hand open naturally, only finger 5 (pinky) moving
RANDOMIZE_RIGHT_HAND="--/randomizeJointAngles 3 0 65 1 finger5-1.r finger5-2.r finger5-3.r"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "All hand open naturally, only finger 5 (pinky) moving"

#All hand open naturally, only finger 4 (ring) moving
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 3 0 65 1 finger4-1.r finger4-2.r finger4-3.r"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "All hand open naturally, only finger 4 (ring) moving"

#All hand open naturally, only finger 3 (middle) moving
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 3 0 65 1 finger3-1.r finger3-2.r finger3-3.r"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "All hand open naturally, only finger 3 (middle) moving"

#All hand open naturally, only finger 2 (pointer) moving
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 3 0 65 1 finger2-1.r finger2-2.r finger2-3.r"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "All hand open naturally, only finger 2 (pointer) moving"

#All hand open naturally, only finger 1 (thumb) moving
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 2 -75 75 2 rthumb finger1-2.r --randomizeJointAngles 1 0 45 3 rthumb"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "All hand open naturally, only finger 1 (thumb) moving"

#All fingers closed only thumb moving!
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.r finger5-2.r finger5-3.r finger4-1.r finger4-2.r finger4-3.r finger3-1.r finger3-2.r finger3-3.r finger2-1.r finger2-2.r finger2-3.r rthumb finger1-2.r finger1-3.r --randomizeJointAngles 5 90 90 1 finger5-1.r finger4-1.r finger3-1.r finger2-1.r rthumb --randomizeJointAngles 2 -75 75 2 rthumb finger1-2.r --randomizeJointAngles 1 0 45 3 rthumb"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "RHand - All fingers closed only thumb moving!"

#All fingers closed except Finger 5 ( pinky )
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.r finger5-2.r finger5-3.r finger4-1.r finger4-2.r finger4-3.r finger3-1.r finger3-2.r finger3-3.r finger2-1.r finger2-2.r finger2-3.r rthumb finger1-2.r finger1-3.r --randomizeJointAngles 5 90 90 1 finger5-1.r finger4-1.r finger3-1.r finger2-1.r rthumb --randomizeJointAngles 2 75 75 2 rthumb finger1-2.r --randomizeJointAngles 1 45 45 3 rthumb --randomizeJointAngles 3 0 65 1 finger5-1.r finger5-2.r finger5-3.r"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "RHand - All fingers closed except Finger 5 ( pinky )"

#All fingers closed except Finger 4 ( ring )
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.r finger5-2.r finger5-3.r finger4-1.r finger4-2.r finger4-3.r finger3-1.r finger3-2.r finger3-3.r finger2-1.r finger2-2.r finger2-3.r rthumb finger1-2.r finger1-3.r --randomizeJointAngles 5 90 90 1 finger5-1.r finger4-1.r finger3-1.r finger2-1.r rthumb --randomizeJointAngles 2 75 75 2 rthumb finger1-2.r --randomizeJointAngles 1 45 45 3 rthumb --randomizeJointAngles 3 0 65 1 finger4-1.r finger4-2.r finger4-3.r"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "RHand - All fingers closed except Finger 4 ( ring )"

#All fingers closed except Finger 3 ( middle )
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.r finger5-2.r finger5-3.r finger4-1.r finger4-2.r finger4-3.r finger3-1.r finger3-2.r finger3-3.r finger2-1.r finger2-2.r finger2-3.r rthumb finger1-2.r finger1-3.r --randomizeJointAngles 5 90 90 1 finger5-1.r finger4-1.r finger3-1.r finger2-1.r rthumb --randomizeJointAngles 2 75 75 2 rthumb finger1-2.r --randomizeJointAngles 1 45 45 3 rthumb --randomizeJointAngles 3 0 65 1 finger3-1.r finger3-2.r finger3-3.r"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "RHand - All fingers closed except Finger 3 ( middle )"

#All fingers closed except Finger 2 ( pointer )
RANDOMIZE_RIGHT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.r finger5-2.r finger5-3.r finger4-1.r finger4-2.r finger4-3.r finger3-1.r finger3-2.r finger3-3.r finger2-1.r finger2-2.r finger2-3.r rthumb finger1-2.r finger1-3.r --randomizeJointAngles 5 90 90 1 finger5-1.r finger4-1.r finger3-1.r finger2-1.r rthumb --randomizeJointAngles 2 75 75 2 rthumb finger1-2.r --randomizeJointAngles 1 45 45 3 rthumb --randomizeJointAngles 3 0 65 1 finger2-1.r finger2-2.r finger2-3.r"
./GroundTruthDumper $FROM $NATURAL_RHAND --repeat $FINGER_POSES $SELECT_RHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_RIGHT_HAND $OUT
checkForError "RHand - All fingers closed except Finger 2 ( pointer )"

#Done
}

#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------

function generateLeftHandDataset 
{
OUTBASE="$1"


#Make sure file is wiped before
#adding new poses..
#-------------------------
rm $outputDir/2d_$OUTBASE
rm $outputDir/bvh_$OUTBASE
#-------------------------

MIN_HAND_DEPTH="$2"
MAX_HAND_DEPTH="$3" #1500
MIN_LHAND_RZ="$4"
MAX_LHAND_RZ="$7"

MIN_LHAND_RY="$5"
MAX_LHAND_RY="$8"

MIN_LHAND_RX="$6"
MAX_LHAND_RX="$9"

FROM="--haltonerror $VIEW_COMMANDS --from dataset/lhand.$BVH_TYPE"

RANDOM_POSITION_ROTATION="--randomize2D $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_LHAND_RZ $MIN_LHAND_RY $MIN_LHAND_RX $MAX_LHAND_RZ $MAX_LHAND_RY $MAX_LHAND_RX $USE_OCCLUSIONS"

OUT="--csv $outputDir $OUTBASE 2d+bvh "

# ./BVHGUI2 --from dataset/lhand.qbvh --set 2 -25 --set 3 0.5 --set 4 -0.5 --set 5 -0.5 --set 6 0.5 --set 12 -5.0 --set 39 5.0 --set 54 10.5 --set 63 -18

#New automatic pose generation based on IK Problem..
./GroundTruthDumper $FROM --repeat $FULL_RANDOM_POSES $SELECT_LHAND --randomizeBasedOnIKConstrains lhand $RANDOM_POSITION_ROTATION $OUT 
checkForError "lhand full random"

#Default BVH poses with everything at zero but hand is naturally open
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $BASE_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $OUT
checkForError "lhand naturally open"

#Default BVH poses with everything at zero but hand is not naturally open, it is has fingers touching 
./GroundTruthDumper $FROM --repeat $ZERO_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $OUT
checkForError "lhand to zero"

#Default BVH poses with hand naturally open but thumb in the middle
./GroundTruthDumper $FROM $NATURAL_LHAND --set 63 6.5 --set 64 -28.7 --set 65 8.5 --set 66 37 --set 67 -13 --set 68 7.6 --set 69 -22 --set 70 -5 --set 71 43 --set 72 0 --repeat $ZERO_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $OUT
checkForError "lhand naturally open thumb in the middle"

#Left hand Thumb Movement on default pose
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION --randomizeMID $MID_THUMBBASE_Y -56.0 48.0 $OUT
checkForError "LHand - Left hand Thumb Movement on default pose"

#All hand open naturally, only finger 5 (pinky) moving
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 3 0 65 1 finger5-1.l finger5-2.l finger5-3.l"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All hand open naturally, only finger 5 (pinky) moving"

#All hand open naturally, only finger 4 (ring) moving
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 3 0 65 1 finger4-1.l finger4-2.l finger4-3.l"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All hand open naturally, only finger 4 (ring) moving"

#All hand open naturally, only finger 3 (middle) moving
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 3 0 65 1 finger3-1.l finger3-2.l finger3-3.l"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All hand open naturally, only finger 3 (middle) moving"

#All hand open naturally, only finger 2 (pointer) moving
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 3 0 65 1 finger2-1.l finger2-2.l finger2-3.l"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All hand open naturally, only finger 2 (pointer) moving"

#All hand open naturally, only finger 1 (thumb) moving
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 2 -75 75 2 lthumb finger1-2.l --randomizeJointAngles 1 0 45 3 lthumb"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All hand open naturally, only finger 1 (thumb) moving"

#All fingers closed only thumb Open!
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.l finger5-2.l finger5-3.l finger4-1.l finger4-2.l finger4-3.l finger3-1.l finger3-2.l finger3-3.l finger2-1.l finger2-2.l finger2-3.l lthumb finger1-2.l finger1-3.l --randomizeJointAngles 5 90 90 1 finger5-1.l finger4-1.l finger3-1.l finger2-1.l lthumb --randomizeJointAngles 2 -75 75 2 lthumb finger1-2.l --randomizeJointAngles 1 0 45 3 lthumb"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All fingers closed only thumb Open!"

#All fingers closed except Finger 5 ( pinky )
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.l finger5-2.l finger5-3.l finger4-1.l finger4-2.l finger4-3.l finger3-1.l finger3-2.l finger3-3.l finger2-1.l finger2-2.l finger2-3.l lthumb finger1-2.l finger1-3.l --randomizeJointAngles 5 90 90 1 finger5-1.l finger4-1.l finger3-1.l finger2-1.l lthumb --randomizeJointAngles 2 75 75 2 lthumb finger1-2.l --randomizeJointAngles 1 45 45 3 lthumb --randomizeJointAngles 3 0 65 1 finger5-1.l finger5-2.l finger5-3.l"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All fingers closed except Finger 5 ( pinky )"

#All fingers closed except Finger 4 ( ring )
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.l finger5-2.l finger5-3.l finger4-1.l finger4-2.l finger4-3.l finger3-1.l finger3-2.l finger3-3.l finger2-1.l finger2-2.l finger2-3.l lthumb finger1-2.l finger1-3.l --randomizeJointAngles 5 90 90 1 finger5-1.l finger4-1.l finger3-1.l finger2-1.l lthumb --randomizeJointAngles 2 75 75 2 lthumb finger1-2.l --randomizeJointAngles 1 45 45 3 lthumb --randomizeJointAngles 3 0 65 1 finger4-1.l finger4-2.l finger4-3.l"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All fingers closed except Finger 4 ( ring )"

#All fingers closed except Finger 3 ( middle )
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.l finger5-2.l finger5-3.l finger4-1.l finger4-2.l finger4-3.l finger3-1.l finger3-2.l finger3-3.l finger2-1.l finger2-2.l finger2-3.l lthumb finger1-2.l finger1-3.l --randomizeJointAngles 5 90 90 1 finger5-1.l finger4-1.l finger3-1.l finger2-1.l lthumb --randomizeJointAngles 2 75 75 2 lthumb finger1-2.l --randomizeJointAngles 1 45 45 3 lthumb --randomizeJointAngles 3 0 65 1 finger3-1.l finger3-2.l finger3-3.l"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All fingers closed except Finger 3 ( middle )"

#All fingers closed except Finger 2 ( pointer )
RANDOMIZE_LEFT_HAND="--randomizeJointAngles 15 65 65 1 finger5-1.l finger5-2.l finger5-3.l finger4-1.l finger4-2.l finger4-3.l finger3-1.l finger3-2.l finger3-3.l finger2-1.l finger2-2.l finger2-3.l lthumb finger1-2.l finger1-3.l --randomizeJointAngles 5 90 90 1 finger5-1.l finger4-1.l finger3-1.l finger2-1.l lthumb --randomizeJointAngles 2 75 75 2 lthumb finger1-2.l --randomizeJointAngles 1 45 45 3 lthumb --randomizeJointAngles 3 0 65 1 finger2-1.l finger2-2.l finger2-3.l"
./GroundTruthDumper $FROM $NATURAL_LHAND --repeat $FINGER_POSES $SELECT_LHAND $RANDOM_POSITION_ROTATION $RANDOMIZE_LEFT_HAND $OUT
checkForError "LHand - All fingers closed except Finger 2 ( pointer )"

#Done

#Let's count it and store the numbers so we don't have to spam disk reads on every training..
wc -l $outputDir/2d_$1 > $outputDir/count_$1
#wc -l $outputDir/3d_$1 > $outputDir/count_$1
wc -l $outputDir/bvh_$1 > $outputDir/count_$1
}


#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------


# ./BVHGUI2 --from dataset/lhand.$BVH_TYPE --set 3 -90 --set 4 0 --set 5 -135



MIN_HAND_DEPTH="200"
MAX_HAND_DEPTH="2000" #1500


MIN_HAND_RZ="-90"
MAX_HAND_RZ="-90"

MIN_HAND_RY="-50"
MAX_HAND_RY="50"

#If we have a quaternion we can handle all orientations!
if [ "$1" == "--quaternions" ]; then
 MIN_HAND_RZ="-179.9"
 MAX_HAND_RZ="179.9"

 MIN_HAND_RY="-179.9"
 MAX_HAND_RY="179.9"
fi


MIN_HAND_RX="-179.9"
MAX_HAND_RX="179.9"


generateLeftHandDataset lhand_all.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX
#./ReshapeCSV --from dataset/bvh_lhand_all.csv --faces 20 > dataset/category_lhand_all.csv

generateRightHandDataset rhand_all.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX
#./ReshapeCSV --from dataset/bvh_rhand_all.csv --faces 20 > dataset/category_rhand_all.csv

echo "Done Generating Hands.."

exit 0

MIN_HAND_RX="-135"
MAX_HAND_RX="-45"
generateRightHandDataset rhand_front.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX
generateLeftHandDataset lhand_front.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX

MIN_HAND_RX="45"
MAX_HAND_RX="135"
generateRightHandDataset rhand_back.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX
generateLeftHandDataset lhand_back.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX

MIN_HAND_RX="-45"
MAX_HAND_RX="45"
generateRightHandDataset rhand_left.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX
generateLeftHandDataset lhand_left.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX

MIN_HAND_RX="135"
MAX_HAND_RX="225"
generateRightHandDataset rhand_right.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX
generateLeftHandDataset lhand_right.csv $MIN_HAND_DEPTH $MAX_HAND_DEPTH $MIN_HAND_RZ $MIN_HAND_RY $MIN_HAND_RX $MAX_HAND_RZ $MAX_HAND_RY $MAX_HAND_RX
#--svg tmp/ --bvh randomRHand.$BVH_TYPE
#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------------------------------


exit 0
