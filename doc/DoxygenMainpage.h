/**
@mainpage  MocapNET
@author Ammar Qammaz a.k.a. AmmarkoV - http://ammar.gr

Ensemble of SNN Encoders for 3D Human Pose Estimation in RGB Images

Website: https://github.com/AmmarkoV/RGBDAcquisition  

@section libIntro Introduction

 We present MocapNET, an ensemble of SNN encoders that estimates the 3D human body pose based on 2D joint estimations extracted from monocular RGB images. MocapNET provides an efficient divide and conquer strategy for supervised learning. It outputs skeletal information directly into the BVH format which can be rendered in real-time or imported without any additional processing in most popular 3D animation software. The proposed architecture achieves 3D human pose estimations at state of the art rates of 400Hz in high-end systems using only CPU processing.
  
Please cite the following paper if this work helps your research :

@inproceedings{Qammaz2019,
  author = {Qammaz, Ammar and Argyros, Antonis A},
  title = {MocapNET: Ensemble of SNN Encoders for 3D Human Pose Estimation in RGB Images},
  booktitle = {British Machine Vision Conference (BMVC 2019)},
  publisher = {BMVA},
  year = {2019},
  month = {September},
  address = {Cardiff, UK},
  url = {http://users.ics.forth.gr/argyros/res_mocapnet.html},
  projects =  {CO4ROBOTS,MINGEI},
  pdflink = {http://users.ics.forth.gr/ argyros/mypapers/2019_09_BMVC_mocapnet.pdf},
  videolink = {https://youtu.be/fH5e-KMBvM0}
}



@section libDesignOverview Overview, System Requirements and Dependencies

MocapNET is a high performance 2D to 3D single person pose estimator. This code base targets recent Linux (Ubuntu 18.04 - 20.04 +) machines, and relies on the Tensorflow C-API and OpenCV. Windows 10 users can try the linux subsystem that has been also reported to work.

Tensorflow is used as the Neural Network framework for our work and OpenCV is used to enable the acquisition of images from webcams or video files as well as to provide an easy visualization method.

We have provided an initialization script that automatically handles most dependencies, as well as download all needed pretrained models. After running it the application should be ready for use.

Any issues not automatically resolved by the script can be reported on the issues section of this repository.

In order to enable a series of easy to use mini-demos with as few dependencies as possible. We have included a MocapNETBenchmark utility which has hardcoded input and output that can run even in a system without OpenCV to give you a performance estimation of our method. If you have OpenCV available you can use our live demo ( MocapNETLiveWebcamDemo binary ) that also contains the 2D joint detector. By giving it the correct parameters you can switch between a cut-down version of OpenPose (--openpose), VNect (--vnect) or our own MobileNet (default) based 2D joint estimator. All of these are automatically downloaded using the initialize.sh script. However in order to achieve higher accuracy estimations you are advised to set up a full OpenPose instance and use it to acquire JSON files with 2D detections that can be subsequently converted to 3D BVH files using the MocapNETJSON binary. They will provide superior accuracy compared to the bundled 2D joint detectors which are provided for faster performance in the live demo, since 2D estimation is the bottleneck of the application. The lowest end machine where this codebase is currently deployed is an Intel Core 2 6600 PC with 4GB of RAM and an NVIDIA GeForce GT740 GPU where MocapNET achieves 50fps and the bundled 2D Joint Detector 10fps for an aggregate 7fps of 3D pose estimation from RGB. Our live demo will try to run the 2D Joint estimation on your GPU and MocapNET 3D estimation on the system CPU to achieve a combined framerate of over 30 fps which in most cases matches or surpasses the acquisition rate of web cameras. Unfortunately there are many GPU compatibility issues with Tensorflow C-API builds since recent versions have dropped CUDA 9.0 support as well as compute capabilities that might be required by your system, you can edit the initialize.sh script and change the variable TENSORFLOW_VERSION according to your needs. If you want CUDA 9.0 you should se it to 1.12.0. If you want CUDA 9.0 and have a card with older compute capabilities (5.2) then choose version 1.11.0. If all else fails you can always recompile the tensorflow C-API to match your specific hardware configuration. You can also use this script that automates building tensorflow r1.15 that might help you, dealing with the Bazel build system and all of its weirdness. Release 1.15 is the final of the 1.x tensorflow tree and is compatible with MocapNET, Tensorflow 2.0 does not yet have C bindings and is thus not yet supported, according to the Tensorflow site, version 2.3 will be the first to re-include C bindings.

If you are interested in generating BVH training data for your research, we have also provided the code that handles randomization and pose perturbation from the CMU dataset. After a successful compilation, dataset generation is accessible using the scripts scripts/createRandomizedDataset.sh and scripts/createTestDataset.sh. All BVH manipulation code is imported from a secondary github project that is automatically downloaded, included and built using the initialize.sh script. These scripts/createRandomizedDataset.sh and scripts/createTestDataset.sh scripts will populate the dataset/ directory with CSV files that contain valid training samples based on the CMU dataset. It is trivial to load these files using python. After loading them using them as training samples in conjunction with a deep learning framework like Keras you can facilitate learning of 2D to 3D BVH.

@section libBuilding Building the library

To compile the library issue :

 sudo apt-get install build-essential cmake libopencv-dev libjpeg-dev libpng-dev libglew-dev libpthread-stubs0-dev

./initialize.sh

After performing changes to the source code, you do not need to rerun the initialization script. You can recompile the code by using :

cd build 
cmake .. 
make 
cd ..


@section libUpdating Updating the library

The MocapNET library is under active development, the same thing is true for its dependencies.

In order to update all the relevant parts of the code you can use the update.sh script provided.

./update.sh

If you made changes to the source code that you want to discard and want to revert to the master you can also use the revert.sh script provided

./revert.sh


*/
