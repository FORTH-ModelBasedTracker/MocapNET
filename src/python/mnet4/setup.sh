#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

#Simple dependency checker that will apt-get stuff if something is missing
# sudo apt-get install python3-venv python3-pip
SYSTEM_DEPENDENCIES="build-essential python3-venv python3-pip freeglut3-dev libglew-dev protobuf-compiler libprotobuf-dev libavdevice-dev"

for REQUIRED_PKG in $SYSTEM_DEPENDENCIES
do
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
echo "Checking for $REQUIRED_PKG: $PKG_OK"
if [ "" = "$PKG_OK" ]; then

  echo "No $REQUIRED_PKG. Setting up $REQUIRED_PKG."

  #If this is uncommented then only packages that are missing will get prompted..
  #sudo apt-get --yes install $REQUIRED_PKG

  #if this is uncommented then if one package is missing then all missing packages are immediately installed..
  sudo apt-get  --yes install $SYSTEM_DEPENDENCIES  
  break
fi
done
#------------------------------------------------------------------------------

HEADLESS=" --headless"
OPENCV=" "

if [[ $* == *--collab* ]]
then 
 echo "Collab mode!"
 echo "Performing python package installation without a virtual environment"
HEADLESS=" --headless"
OPENCV="opencv-python-headless"
rm -rf /content/sample_data/ #remove sample data
ln -s $DIR /content/mnet4
else 
 echo "Using a python pythonVirtualEnvironment environment"
 echo "Activate it using : source pythonVirtualEnvironment/bin/activate"
 python3 -m venv pythonVirtualEnvironment
 source pythonVirtualEnvironment/bin/activate
OPENCV="opencv-python"
HEADLESS=" "
fi



cd "$DIR"
if [ -f RGBDAcquisition/README.md ]; then
echo "RGBDAcquisition appears to already exist .."
else
 git clone https://github.com/AmmarkoV/RGBDAcquisition
 cd RGBDAcquisition
 #----------------------------------------------------------------------------------------------------------------------
 mkdir build
 cd build
 cmake ..
 cd ..
 #----------------------------------------------------------------------------------------------------------------------
 cd opengl_acquisition_shared_library/opengl_depth_and_color_renderer/
 mkdir build 
 cd build
 cmake ..
 make BVHTester
 #----------------------------------------------------------------------------------------------------------------------
 cd "$DIR"
 ln -s RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Applications/BVHTester BVH
 ln -s RGBDAcquisition/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/BVHTester GroundTruthDumper
fi


 
 python3 -m pip install --upgrade pip #2.8.0 nvidia-tensorrt
 python3 -m pip install tensorflow numpy numba tensorboard_plugin_profile tensorflow-model-optimization keras pillow tf2onnx onnxruntime onnx matplotlib pydot mediapipe $OPENCV scikit-learn

 
 #Two ways to get Models!
 #One through the database and compile them yourself (commented out to save Google Collab CPU time)..
 #python3 -m getModelFromDatabase --get 314
 #To generate the archive in the next lines do :
 #   zip mnet4_314.zip step1_*/*

 #Another just take the precompiled ensembles ready to run
 wget http://ammar.gr/mocapnet/mnet4/mnet4_314.zip -O mnet4_314.zip
 unzip -o mnet4_314.zip

 #Doing a test run!

# echo "To enable Tensorflow profiling for non-root users please add : "
# echo "options nvidia \"NVreg_RestrictProfilingToAdminUsers=0\""
# echo "to /etc/modprobe.d/nvidia-graphics-drivers.conf"
# echo " "
 echo "Ready for use"
 echo "From now on to work with MocapNET you can use :  "
 echo "source pythonVirtualEnvironment/bin/activate"
 echo "!python3 -m mediapipeHolisticWebcamMocapNET --from webcam  --save --plot"
 echo " OR "
 echo "!python3 -m mediapipeHolisticWebcamMocapNET --from shuffle.webm --save --plot"
 echo " OR "
 echo "!python3 -m csvNET --from ../../../dataset/sample.csv  --save --plot"
 echo "  "
 echo " if you are running on collab don't forget to append --headless to your command"
 echo " after the run is complete out.bvh will contain your results  "



