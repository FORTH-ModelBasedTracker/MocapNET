#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

HAND="r"
FILE="ground3D.csv"
#cp ground3D.csv $FILE

sed -i "s/_0,/_${HAND}hand,/g" $FILE
sed -i "s/_1,/_${HAND}thumb,/g" $FILE
sed -i "s/_2,/_finger1.2.${HAND},/g" $FILE
sed -i "s/_3,/_finger1.3.${HAND},/g" $FILE
sed -i "s/_4,/_${HAND}baseOfFourFingers,/g" $FILE
sed -i "s/_5,/_finger2.3.${HAND},/g" $FILE
sed -i "s/_6,/_finger2.2.${HAND},/g" $FILE
sed -i "s/_7,/_finger2.1.${HAND},/g" $FILE
sed -i "s/_8,/_metacarpal1.${HAND},/g" $FILE
sed -i "s/_9,/_finger3.3.${HAND},/g" $FILE
sed -i "s/_10,/_finger3.2.${HAND},/g" $FILE
sed -i "s/_11,/_finger3.1.${HAND},/g" $FILE
sed -i "s/_12,/_metacarpal2.${HAND},/g" $FILE
sed -i "s/_13,/_finger4.3.${HAND},/g" $FILE
sed -i "s/_14,/_finger4.2.${HAND},/g" $FILE
sed -i "s/_15,/_finger4.1.${HAND},/g" $FILE
sed -i "s/_16,/_metacarpal3.${HAND},/g" $FILE
sed -i "s/_17,/_finger5.3.${HAND},/g" $FILE
sed -i "s/_18,/_finger5.2.${HAND},/g" $FILE
sed -i "s/_19,/_finger5.1.${HAND},/g" $FILE
sed -i "s/_20,/_metacarpal4.${HAND},/g" $FILE


exit 0
