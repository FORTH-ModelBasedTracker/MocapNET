#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

#Simple dependency checker that will apt-get stuff if something is missing
# sudo apt-get install python3-venv python3-pip
SYSTEM_DEPENDENCIES="python3-venv python3-pip"

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

 #ONNX needs protobuf AI need avdevice ?
 sudo apt-get install protobuf-compiler libprotobuf-dev libavdevice-dev
 
 python3 -m venv pythonVirtualEnvironment
 source pythonVirtualEnvironment/bin/activate
 
 python3 -m pip install --upgrade pip #2.8.0 nvidia-tensorrt
 python3 -m pip install tensorflow numpy numba tensorboard_plugin_profile tensorflow-model-optimization keras pillow tf2onnx onnxruntime onnx matplotlib pydot mediapipe opencv-python scikit-learn

 echo "To enable Tensorflow profiling for non-root users please add : "
 echo "options nvidia \"NVreg_RestrictProfilingToAdminUsers=0\""
 echo "to /etc/modprobe.d/nvidia-graphics-drivers.conf"
 echo " "
 echo "Ready for use"
 echo "From now on to work with MocapNET you can use :  "
 echo "source pythonVirtualEnvironment/bin/activate"
 echo "python3 mediapipeHolisticWebcamMocapNET.py --from webcam"
 
