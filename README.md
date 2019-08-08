# MocapNET Project
## Ensemble of SNN Encoders for 3D Human Pose Estimation in RGB Images

We present MocapNET, an ensemble of SNN encoders that estimates the 3D human body pose based on 2D joint estimations extracted from monocular RGB images. MocapNET provides an efficient divide and conquer strategy for supervised learning. It outputs skeletal information directly into the BVH format which can be rendered in real-time or imported without any additional processing in most popular 3D animation software. The proposed architecture achieves 3D human pose estimations at state of the art rates of 400Hz using only CPU processing.
This is a placeholder for the creation of the repository of our BMVC 2019 paper 

![MocapNET](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/mocapnetBannerImages.png)

## Youtube Video
------------------------------------------------------------------ 

[![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/youtubevideolink.jpg)  ](https://www.youtube.com/watch?v=fH5e-KMBvM0)

## System Requirements and Dependencies
------------------------------------------------------------------ 
MocapNET is a high performance 2D to 3D pose estimator.
This code base targets recent Linux (Ubuntu) machines, and relies on Tensorflow and OpenCV. 

Tensorflow is the Neural Network framework used and OpenCV is used to enable acquire images from webcams or video files as well as provide a visualization.

Due to the fast processing speed and in order to enable an easy to use demo we use the VNect 2D joint estimator that is automatically downloaded using the initialize.sh script.  WebcamJointBIN tries to initialize 2D Joint estimation on GPU and 3D estimation on the system CPU. Since there are many GPU compatibility issues with Tensorflow since recent versions have dropped CUDA 9.0 as well as compute capabilities that might be required by your system, you can edit initialize.sh and change the variable TENSORFLOW_VERSION according to your needs. If you want CUDA 9.0 you should se it to 1.12.0. If you want CUDA 9.0 and have a card with older compute capabilities (5.2) then choose version 1.11.0.

If you are interested in acquiring the data


## Building
------------------------------------------------------------------ 

To compile the library issue :

```
./initialize.sh
mkdir build 
cd build 
cmake .. 
make 
```
 


## Testing
------------------------------------------------------------------ 

To test the library performance on your computer CPU issue :

```
./MocapNETBenchmark --cpu
```

To test the library using a webcam issue :

```
./WebcamJointBIN --from /dev/video0 --live
```

To test the library using a video file issue :

```
./WebcamJointBIN --from /path/to/yourfile.mp4
```


