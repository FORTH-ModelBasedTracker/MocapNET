# MocapNET Project

## This is a snapshot of the deprecated MocapNET 1/BMVC 2019 release 
------------------------------------------------------------------ 

MocapNET has evolved, 
This branch contains the final release of the 1.x MocapNET tree, it has been superseded..
[Switch back to the master branch](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/master/) for the lastest stable version




## Ensemble of SNN Encoders for 3D Human Pose Estimation in RGB Images

We present MocapNET, an ensemble of SNN encoders that estimates the 3D human body pose based on 2D joint estimations extracted from monocular RGB images. MocapNET provides an efficient divide and conquer strategy for supervised learning. It outputs skeletal information directly into the BVH format which can be rendered in real-time or imported without any additional processing in most popular 3D animation software. The proposed architecture achieves 3D human pose estimations at state of the art rates of 400Hz in high-end systems using only CPU processing.

![MocapNET](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/mocapnetBannerImages.png)

## Youtube Videos
------------------------------------------------------------------ 

| BMVC 2019 Supplementary Video  | Demonstration of code in this repository |
| ------------- | ------------- |
| [![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/youtubevideolink.jpg)  ](https://www.youtube.com/watch?v=fH5e-KMBvM0)  | [![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/youtubevideolinkdemo.jpg)  ](https://www.youtube.com/watch?v=pKTA_odo9Xw) |

------------------------------------------------------------------ 



## Citation
------------------------------------------------------------------ 

Please cite the [following paper](http://users.ics.forth.gr/~argyros/mypapers/2019_09_BMVC_mocapnet.pdf) if this work helps your research :

```
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

```


## Overview, System Requirements and Dependencies
------------------------------------------------------------------ 
MocapNET is a high performance 2D to 3D single person pose estimator.
This code base targets recent Linux (Ubuntu  18.04 - 20.04 +) machines, and relies on the Tensorflow C-API and OpenCV. Windows 10 users can try the [linux subsystem](https://www.microsoft.com/en-us/p/ubuntu-1804-lts/9n9tngvndl3q?rtc=1&activetab=pivot:overviewtab) that has been also [reported](https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/10) to work.

Tensorflow is used as the Neural Network framework for our work and OpenCV is used to enable the acquisition of images from webcams or video files as well as to provide an easy visualization method.

We have provided an [initialization script](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/initialize.sh) that automatically handles most dependencies, as well as download all needed pretrained models. After running it the application should be ready for use.

Any issues not automatically resolved by the script can be reported on the [issues](https://github.com/FORTH-ModelBasedTracker/MocapNET/issues) section of this repository.

In order to enable a series of easy to use mini-demos with as few dependencies as possible.  We have included a MocapNETBenchmark utility which has hardcoded [input](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/MocapNETSimpleBenchmark/testCodeInput.hpp) and [output](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/MocapNETSimpleBenchmark/testCodeOutput.hpp) that can run even in a system without OpenCV to give you a performance estimation of our method. If you have OpenCV available you can use our live demo ( MocapNETLiveWebcamDemo binary ) that also contains the 2D joint detector.  By giving it the correct parameters you can switch between a cut-down version of OpenPose (--openpose), VNect (--vnect) or our own MobileNet (default) based 2D joint estimator. All of these are automatically downloaded using the [initialize.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/initialize.sh) script. However in order to achieve higher accuracy estimations you are advised to set up a full [OpenPose](https://github.com/CMU-Perceptual-Computing-Lab/openpose) instance and use it to acquire JSON files with 2D detections that can be subsequently converted to 3D BVH files using the MocapNETJSON binary. They will provide superior accuracy compared to the bundled 2D joint detectors which are provided for faster performance in the live demo, since 2D estimation is the bottleneck of the application. The lowest end machine where this codebase is currently deployed is an Intel Core 2 6600 PC with 4GB of RAM and an NVIDIA GeForce GT740 GPU where MocapNET achieves 50fps and the bundled 2D Joint Detector 10fps for an aggregate 7fps of 3D pose estimation from RGB. Our live demo will try to run the 2D Joint estimation on your GPU and MocapNET 3D estimation on the system CPU to achieve a combined framerate of over 30 fps which in most cases matches or surpasses the acquisition rate of web cameras. Unfortunately there are many GPU compatibility issues with Tensorflow C-API builds since recent versions have dropped CUDA 9.0 support as well as compute capabilities that might be required by your system, you can edit the [initialize.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/initialize.sh) script and change the variable TENSORFLOW_VERSION according to your needs. If you want CUDA 9.0 you should se it to 1.12.0. If you want CUDA 9.0 and have a card with older compute capabilities (5.2) then choose version 1.11.0. If all else fails you can always [recompile the tensorflow C-API](https://github.com/tensorflow/tensorflow/blob/master/tensorflow/tools/lib_package/README.md) to match your specific hardware configuration. You can also [use this script that automates building tensorflow r1.15](https://github.com/AmmarkoV/MyScripts/blob/master/Tensorflow/tensorflowBuild.sh) that might help you, dealing with the Bazel build system and all of its weirdness. Release 1.15 is the final of the 1.x tensorflow tree and is compatible with MocapNET, Tensorflow 2.0 does not yet have C bindings and is thus not yet supported, according to the [Tensorflow site, version 2.3](https://www.tensorflow.org/install/lang_c) will be the first to re-include C bindings. 



If you are interested in generating BVH training data for your research, we have also provided the code that handles randomization and pose perturbation from the CMU dataset. After a successful compilation, dataset generation is accessible using the scripts  [scripts/createRandomizedDataset.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/createRandomizedDataset.sh) and [scripts/createTestDataset.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/createTestDataset.sh). All BVH manipulation code is imported from a secondary [github project](https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader) that is automatically downloaded, included and built using the [initialize.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/initialize.sh) script. These [scripts/createRandomizedDataset.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/createRandomizedDataset.sh) and [scripts/createTestDataset.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/createTestDataset.sh) scripts will populate the dataset/ directory with CSV files that contain valid training samples based on the CMU dataset. It is [trivial](https://pythonspot.com/reading-csv-files-in-python/) to load these files using python. After loading them using them as training samples in conjunction with a deep learning framework like [Keras](https://keras.io/) you can facilitate learning of 2D to 3D BVH. 

## Building the library
------------------------------------------------------------------ 

To compile the library issue :

```
 sudo apt-get install build-essential cmake libopencv-dev libjpeg-dev libpng-dev libglew-dev libpthread-stubs0-dev

./initialize.sh

```
 
After performing changes to the source code, you do not need to rerun the initialization script. You can recompile the code by using : 

```
cd build 
cmake .. 
make 
cd ..
```



## Updating the library
------------------------------------------------------------------ 

The MocapNET library is under active development, the same thing is true for its dependencies.

In order to update all the relevant parts of the code you can use the [update.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/update.sh) script provided. 

```
./update.sh
```

If you made changes to the source code that you want to discard and want to revert to the master you can also use the [revert.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/revert.sh) script provided

```
./revert.sh
```




## Testing the library and performing benchmarks 
------------------------------------------------------------------ 

To test the library performance on the CPU of your computer issue :

```
./MocapNETBenchmark --cpu
```

The output should provide you with a model name of your CPU as well as the average framerate for 1000 samples evaluated, as seen in the following screenshots.

![MocapNETBenchmark High-Range](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/benchmarknew.png)

The high-range CPU achieves 385.08 fps when executing the proposed method to convert 2D points to a 3D pose.

![MocapNETBenchmark Medium-Range](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/benchmarkview.png)

The same benchmark when run on a medium-range CPU (considered medium-range due to being relatively old, 2014) achieves a lower 194.823 fps. 

![MocapNETBenchmark Low-Range](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/benchmarkold.png)

The very old CPU tested (2006) achieves 54.22 fps for the same task. However even this configuration can perform better using a different quality setting so by giving --quality 2.0 it performs @ 100hz although the mean average error is higher due to the less powerful ensemble.


------------------------------------------------------------------ 

After testing the tensorflow installation using MocapNETBenchmark, to test your OpenCV installation as well as support of your webcam issue :
```
./OpenCVTest --from /dev/video0 
```

To test OpenCV support of your video files issue :
```
./OpenCVTest --from /path/to/yourfile.mp4
```

These tests only use OpenCV (without Tensorflow or any other dependencies) and are intended as a quick method that can identify and debug configuration problems on your system.
In case of problems playing back video files or your webcam you might want to consider compiling OpenCV yourself. The [scripts/getOpenCV3.2.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/getOpenCV3.2.sh) script has been included to automatically fetch and make OpenCV 3.2 for your convinience. The CMake file provided will automatically try to set the OpenCV_DIR variable to target the locally built version made using the script. If you are having trouble switching between the system version and the downloaded version consider using the cmake-gui utility or removing the build directory and making a fresh one, once again following the Building instructions. The new build directory should reset all paths and automatically see the local OpenCV version if you used the [scripts/getOpenCV3.2.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/getOpenCV3.2.sh) script and use this by default.

## Live Demo
------------------------------------------------------------------ 

Assuming that the OpenCVTest executable described previously is working correctly with your input source, to do a live test of the MocapNET library using a webcam issue :

```
./MocapNETLiveWebcamDemo --from /dev/video0 --live
```

To dump 5000 frames from the webcam to out.bvh instead of the live directive issue :

```
./MocapNETLiveWebcamDemo --from /dev/video0 --frames 5000
```


Testing the library using a pre-recorded video file (i.e. not live input) means you can use a slower but more precise 2D Joint estimation algorithm like the included OpenPose implementation. You should keep in mind that [this OpenPose implementation](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET1/MocapNETLiveWebcamDemo/utilities.cpp#L213) does not use PAFs and so it is still not as precise as  the official OpenPose implementation. To run the demo with a prerecorded file issue :

```
./MocapNETLiveWebcamDemo --from /path/to/yourfile.mp4 --openpose
```

We have included a [video file](http://ammar.gr/mocapnet/shuffle.webm) that should be automatically downloaded by the initialize.sh script. Issuing the following command should run it and produce an out.bvh file even if you don't have any webcam or other video files available!  :

```
./MocapNETLiveWebcamDemo --from shuffle.webm --openpose --frames 375
```


Since high-framerate output is hard to examine, if you need some more time to elaborate on the output you can use the delay flag to add programmable delays between frames. Issuing the following will add 1 second of delay after each processed frame  :

```
./MocapNETLiveWebcamDemo --from shuffle.webm --openpose --frames 375 --delay 1000
```


Finally, as stated in the [paper](http://users.ics.forth.gr/~argyros/mypapers/2019_09_BMVC_mocapnet.pdf), MocapNET has a configurable quality/speed setting we call its λ variable. You can switch between different λ configurations using the --quality flag with possible values beeing 1.0(maximum quality), 1.5 and 2.0 (maximum framerate). By default a λ=1.0 is used. If you wish to override this issuing the following command will run the  maximum framerate ensemble :

```
./MocapNETLiveWebcamDemo --from shuffle.webm --frames 375 --quality 2.0
```

The output window of MocapNETLiveWebcamDemo contains a heatmap depicting the 2D Joint estimations, an RGB image cropped and centered on the observed person, a 2D overlay of the 2D Skeleton as well as a window that has the 3D output retrieved by our method as seen in the following image. It should be noted that this demo is performance oriented and to that end it uses the fast [VNect](http://gvv.mpi-inf.mpg.de/projects/VNect/) artificial neural network as its 2D joint estimator. On recent systems the framerate achieved by the application should match the input framerate of your camera which is typically 30 or 60 fps. That being said the visualization provided will provide detailed framerate information for every part of the demo and the bottleneck is the 2D joint estimator. 

If your target is a headless environment then you might consider deactivating the visualization by passing the runtime argument --novisualization. This will prevent any windows from opening and thus not cause issues even on a headless environment.

BVH output files are stored to the "out.bvh" file by default. If you want them to be stored in a different path use the -o option. They can be easily viewed using a variety of compatible applicatons. We suggest [Blender](https://www.blender.org/) which is a very powerful open-source 3D editing and animation suite or [BVHacker](https://www.bvhacker.com/) that is freeware and compatible with [Wine](https://wiki.winehq.org/)


![MocapNETLiveWebcamDemo default visualization](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/demoview.jpg)

![MocapNETLiveWebcamDemo rotation per joint visualization](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/demoperjoint.png)

By using the "Visualization Demo" slider bar you can alternate between different visualizations. A particularly useful visualization is the one that plots the joint rotations as seen above. 

![MocapNETLiveWebcamDemo OpenGL visualization](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/demoogl.png)

```
./MocapNETLiveWebcamDemo --from shuffle.webm --openpose --opengl --frames 375
```
By enabling the ENABLE_OPENGL CMake configuration flag during compilation and using the --opengl flag when running the MocapNETLiveWebcamDemo you can also see the experimental OpenGL visualization illustrated above, rendering a skinned mesh that was generated using [makehuman](http://www.makehumancommunity.org/). The BVH file armature used corresponds to the [CMU+Face](http://www.makehumancommunity.org/content/cmu_plus_face.html) armature of makehuman.

![MocapNETLiveWebcamDemo gesture experiments](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/demogesture.png)

```
./MocapNETLiveWebcamDemo --from shuffle.webm --openpose --gestures --frames 375
```
By starting the live demo using the --gestures argument or using the "Gesture Detection" slider bar you can enable a simple form of gesture detection as seen in the illustration above. Gestures are stored as [BVH files](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/master/dataset/gestures) and controlled through the [gestureRecognition.hpp](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET1/MocapNETLib/gestureRecognition.hpp#L18) file. A client application can register a callback as seen in the [demo](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET1/MocapNETLiveWebcamDemo/mocapNETLiveDemo.cpp#L50). The gesture detection code is experimental and has been included as a proof of concept, since due to our high-level output you can easily facilitate gesture detections by comparing subsequent BVH frames as [seen in the code](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET1/MocapNETLib/gestureRecognition.cpp#L148). That being said gestures where not a part of the original [MocapNET paper](http://users.ics.forth.gr/~argyros/mypapers/2019_09_BMVC_mocapnet.pdf).



## Higher accuracy using OpenPose JSON files
------------------------------------------------------------------ 

In order to get higher accuracy output compared to the live demo which is more performance oriented, you can use OpenPose and the 2D output JSON files produced by it. The MocapNETJSON application can convert them to a BVH file. After downloading [OpenPose](https://github.com/CMU-Perceptual-Computing-Lab/openpose) and building it you can use it to acquire 2D JSON body pose data by running :

```
build/examples/openpose/openpose.bin -number_people_max 1 --hand --write_json /path/to/outputJSONDirectory/ -video /path/to/yourVideoFile.mp4
```

This will create files in the following fashion /path/to/outputJSONDirectory/yourVideoFile_XXXXXXXXXXXX_keypoints.json Notice that the filenames generated encode the serial number by padding it up to 12 characters (marked as X). You provide this information to our executable using the --seriallength commandline option.

You can convert them to a BVH file by issuing :
```
./MocapNETJSON --from /path/to/outputJSONDirectory/ --label yourVideoFile --seriallength 12 --size 1920 1080
```

A utility has been included that can convert the JSON files to a single CSV file issuing :
```
 ./convertBody25JSONToCSV --from /path/to/outputJSONDirectory/ --label yourVideoFile --seriallength 12 --size 1920 1080 -o .
```
For more information on how to use the conversion utility please [see the documentation inside the utility](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/Converters/convertBody25JsonToCSV.cpp#L2)

A CSV file has been included that can be run by issuing : 
```
 ./MocapNETJSON --from sample.csv --visualize --delay 30
```
The delay is added in every frame so that there is enough time for the user to see the results, of course the visualization only contains the armature since the CSV file does not have the input images.

![MocapNETJSON](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/demojson.png)


## License
------------------------------------------------------------------ 
This library is provided under the [FORTH license](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/license.txt)

