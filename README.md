# MocapNET Project
## Ensemble of SNN Encoders for 3D Human Pose Estimation in RGB Images

We present MocapNET, an ensemble of SNN encoders that estimates the 3D human body pose based on 2D joint estimations extracted from monocular RGB images. MocapNET provides an efficient divide and conquer strategy for supervised learning. It outputs skeletal information directly into the BVH format which can be rendered in real-time or imported without any additional processing in most popular 3D animation software. The proposed architecture achieves 3D human pose estimations at state of the art rates of 400Hz using only CPU processing.
This is a placeholder for the creation of the repository of our BMVC 2019 paper 

![MocapNET](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/mocapnetBannerImages.png)

## Youtube Video
------------------------------------------------------------------ 

[![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/youtubevideolink.jpg)  ](https://www.youtube.com/watch?v=fH5e-KMBvM0)


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


