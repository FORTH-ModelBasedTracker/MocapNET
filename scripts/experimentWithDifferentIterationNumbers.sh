#!/bin/bash


#Simple dependency checker that will apt-get stuff if something is missing
# sudo apt-get install gnuplot
SYSTEM_DEPENDENCIES="gnuplot"

for REQUIRED_PKG in $SYSTEM_DEPENDENCIES
do
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
echo "Checking for $REQUIRED_PKG: $PKG_OK"
if [ "" = "$PKG_OK" ]; then

  echo "No $REQUIRED_PKG. Setting up $REQUIRED_PKG."

  #If this is uncommented then only packages that are missing will get prompted..
  #sudo apt-get --yes install $REQUIRED_PKG

  #if this is uncommented then if one package is missing then all missing packages are immediately installed..
  sudo apt-get install $SYSTEM_DEPENDENCIES  
  break
fi
done
#------------------------------------------------------------------------------



DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

cd ..

 
./GroundTruthDumper --from dataset/CMU_Sample_05_01.bvh --selectJoints 0 23 hip eye.r eye.l abdomen chest neck head rshoulder relbow rhand lshoulder lelbow lhand rhip rknee rfoot lhip lknee lfoot toe1-2.r toe5-3.r toe1-2.l toe5-3.l --tuneIterations 130 100 130 0.01 5 30 20 > tuneIterations.dat


#set view 45,45; set view map; set yrange [9:90]; set zrange [30:90];
# set dgrid3d 25,25 qnorm 4;
#splot \"tuneIterations.dat\" using 1:3:2 with points pointsize 3 pointtype 7; 
#HCD tuning iteration hyperparameter
GNUPLOT_CMD="set terminal png size 1000,800 font \"Helvetica,34\"; set output \"tuneIterations.png\";  set isosample 160; set pm3d at b; set palette defined (65 \"black\", 70 \"red\", 80 \"yellow\", 90 \"yellow\", 100 \"white\");  set view 30,45;  set hidden3d; set xrange [1:25]; set yrange [9:90]; set zrange [30:90]; set style fill solid; set xlabel \"HCD Iterations\" rotate parallel; set ylabel \"Frames per second\" rotate parallel; set zlabel \"Mean average error in mm\" rotate parallel; set ztics 30; set title \"Mean average error in mm\"; set ytics 30; set multiplot; splot \"tuneIterations.dat\" using 1:3:2 with lines palette lw 2 title \" \"; splot \"tuneIterations.dat\" using 1:3:2 with points palette pointsize 1 pointtype 7 title \" \";  " 

gnuplot -e "$GNUPLOT_CMD"

echo "Result of experiment is now ready @ tuneIterations.png"
timeout 10 gpicview tuneIterations.png

exit 0
