#!/bin/bash
#Written by Ammar Qammaz a.k.a AmmarkoV - 2020
#  This bash script uses gnuplot and R so make sure to : 
#          sudo apt-get install gnuplot r-base 

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

ORIG_DIR=`pwd`

function getStatistics 
{ 
  #use R to generate statistics 
  #sudo apt-get install r-base
  R -q -e "x <- read.csv('$1', header = F); summary(x); sd(x[ , 1])" > $2
  cat $1 | wc -l >> $2 
}  


IN="$1" 
OUTPUT_IMAGE="AllDistancesFrequency_$2.png" 
echo "plotAllJointsDistanceFrequency.sh $IN $OUTPUT_IMAGE" 
 
getStatistics $IN $IN-statsraw.txt
cat $IN-statsraw.txt | grep -E 'Min|Qu|Median|Mean|Max' > $IN-stats.txt
STATS=`cat $IN-stats.txt`
MEAN=`cat $IN-stats.txt | grep "Mean" | cut -d':' -f2 `
MEDIAN=`cat $IN-stats.txt | grep "Median" | cut -d':' -f2 `
MAXIMUM=`cat $IN-stats.txt | grep "Max" | cut -d':' -f2 `
PLACETEXT=`cat $IN-stats.txt | grep "3rd" | cut -d':' -f2 `

LOW_LIMIT="50";
LIMIT="150";
NUMBER_OF_RECORDS=`wc -l $1 | cut -d' ' -f1`
  
GNUPLOT_CMD="set terminal png; \
 set output \"$OUTPUT_IMAGE\"; set yrange [0:1];\
 set title \"Frequency precision diagram $IN \";\
 set xlabel \"Distance Of Joints(mm)\"; \
 set ylabel \"Frequency Of Value\"; \
 set arrow from $MEAN, graph 0 to $MEAN, graph 1 nohead; \
 set label \"Mean Value of $MEAN mm \" at $MEAN,0.40; \
 set arrow from $MEDIAN, graph 0 to $MEDIAN, graph 1 nohead; \
 set label \"Median Value of $MEDIAN mm \" at $MEDIAN,0.30; \
 set label \"$STATS\" at $PLACETEXT,0.85; \
 binwidth=3;\
 bin(x,width)=width*floor(x/width);\
 plot [0:] '$IN' using   (bin(\$1,binwidth)):(1.0) smooth cnorm t 'smooth cumulative'"
  

echo "WE WILL RUN "
echo "gnuplot -e \"$GNUPLOT_CMD\"" 
echo " "
echo " "


gnuplot -e "$GNUPLOT_CMD"

rm $IN-statsraw.txt
rm $IN-stats.txt
 
exit 0
