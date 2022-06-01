python3 -m venv mp_env
source mp_env/bin/activate

#For RPI4
#sudo apt install libxcb-shm0 libcdio-paranoia-dev libsdl2-2.0-0 libxv1  libtheora0 libva-drm2 libva-x11-2 libvdpau1 libharfbuzz0b libbluray2 libatlas-base-dev libhdf5-103 libgtk-3-0 libdc1394-22 libopenexr23
#pip install mediapipe-rpi4 opencv-python

#For Regular x86_64
python3 -m pip install mediapipe opencv-python
python3 holisticWebcam.py


