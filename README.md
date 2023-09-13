# MocapNET Project


![MocapNET](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/mnet4/doc/method.png)


One click deployment in Google Collab : [![Open MocapNET v4 In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/FORTH-ModelBasedTracker/MocapNET/blob/mnet4/mocapnet4.ipynb)



## News
------------------------------------------------------------------ 


13-9-2023

MocapNET v4 has been accepted in the [AMFG 2023 ICCV workshop](https://web.northeastern.edu/smilelab/amfg2023/) with an Oral and Poster presentation. It can now also retrieve 3D gaze and BVH facial configurations.
The whole codebase has been written from scratch in Python to hopefully make it more useful for the community! This is a very big change including handling all of [3D rendering using Blender python scripts](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/mnet4/src/python/blender). In order to keep this repository manageable the new version lives in its own [mnet4 branch](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/mnet4/)

We have also implemented a one click Google Collab setup, which you can use to quickly and easily test the method : [![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/FORTH-ModelBasedTracker/MocapNET/blob/mnet4/mocapnet4.ipynb)

Hope that you find it useful and hopefully see you at ICCV 2023!
 

30-12-2022

MocapNET has a new [plugin/script](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/python/blender/blender_mocapnet.py) for the [Blender](https://www.blender.org/) 3D editor that when combined with [MPFB2](http://static.makehumancommunity.org/mpfb.html) (the MakeHuman addon for Blender) can make 3D animations using custom skinned humans with the output BVH files of MocapNET with a few clicks. The code targets recent Blender Versions 3.4+
Watch [this video](https://www.youtube.com/watch?v=9jmTdhVjAsI) to learn how to install MPFB2 and [this video](https://www.youtube.com/watch?v=ooLRUS5j4AI) to learn how to interface the provided plugin in this repository with the MocapNET output BVH file and the generated MakeHuman armature.

![MocapNET Blender Plugin](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/blenderscript.jpg)

[![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/blenderytb.png)](https://www.youtube.com/watch?v=ooLRUS5j4AI)


30-9-2022

MocapNET was demonstrated at the [Foundation of Research and technology of Greece](https://www.forth.gr/en/home/) as part of the [European Researcher's Night 2022 event](https://cordis.europa.eu/programme/id/HORIZON_HORIZON-MSCA-2022-CITIZENS-01-01).

![Researcher's Night 2022](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/ren2022.jpg)

1-6-2022

An added [python/mediapipe utility](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/master/src/python/mediapipe) can now help with generating 2D data for experiments! 
This can help you create datasets that include hands that can be processed using [MocapNETv3](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/mnet3)


7-4-2022

The open call of [BONSAPPS (https://bonsapps.eu/)](https://bonsapps.eu/) for AI talents received 126 proposals from 31 EU countries.
Out of these proposals, 30 were actually accepted. 
Out of the 30 running BONSAPPs projects, 10 were selected yesterday to continue into phase 2. 
I am very happy to report that our AUTO-MNET MocapNET based work made it to the top ten!

![BonsAPPs Hackathon/Stage 2 selection](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/bonsappshackathon.jpg)


9-3-2022

MocapNET was one of the selected projects in the [BonsAPPs Open Call for AI talents](https://bonsapps-1oc-ai-talents.fundingbox.com/)
We are now preparing a version of MocapNET called AUTO-MNET tailored for [3D Body Tracking for automotive uses](https://s3.amazonaws.com/fundingbox-sites/gear%2F1635238346063-BonsAPPs_Guide+for+Applicants+%5BOC1%5D_published.pdf)

Due to our limited resources this has currently pushed back merging of the [mnet3 branch](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/mnet3), however hopefully we will soon have a working MocapNET in the [Bonseyes platform](https://beta.bonseyes.com/).


8-11-2021

MocapNET3 with hand pose estimation support has landed in this repository! The latest version that has been accepted in BMVC2021 is now commited [in the mnet3 branch of this repository](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/mnet3). Since however there is considerable code-polish missing and currently the 2D joint estimator offered does not contain hands there needs to be a transition to a 2D joint estimator like [Mediapipe Holistic](https://google.github.io/mediapipe/solutions/holistic) for a better live webcam demo. MocapNET3 will appear in [the 32nd British Machine Vision Conference](http://www.bmvc2021.com/) that will be held virtually and is free to attend this year!!


An [upgraded 2020 version of MocapNET](https://github.com/FORTH-ModelBasedTracker/MocapNET/milestone/1) has landed! It contains a very big list of improvements that have been carried out during 2020 over the original work that allows higher accuracy, smoother BVH output and better occlusion robustness while maintaining realtime perfomance. MocapNET2 will appear in [the 25th International Conference on Pattern Recognition](https://www.icpr2020.it/)

If you are interested in the older MocapNET v1 release you can find it in the [mnet1 branch](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/mnet1), 

Visualization Example: 
With MocapNET2 an [RGB video feed like this](https://www.youtube.com/watch?v=Orb4pawcfFY#t=10m) can be converted to BVH motion frames in real-time. The result can be easily used in your favourite 3D engine or application. 

![Sample run](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/shuffle.gif)

Example Output:
| Youtube Video | MocapNET Output | Editing on Blender | 
| ------------- | ------------- | ------------- |
| [![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/youtube.png)](https://www.youtube.com/watch?v=GtJct8nKjcc) | [![BVH File](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/bvh.png)](http://ammar.gr/mocapnet/mnet2/sept2020version.bvh) | [![Blender Video](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/blender.png)](http://ammar.gr/mocapnet/mnet2/sept2020versionBlender.ogv) |


## Ensemble of SNN Encoders for 3D Human Pose Estimation in RGB Images
------------------------------------------------------------------ 

We present MocapNET v2, a real-time method that estimates the 3D human pose directly in the popular [Bio Vision Hierarchy (BVH)](https://en.wikipedia.org/wiki/Biovision_Hierarchy) format, given estimations of the 2D body joints originating from monocular color images. 

Our contributions include: 

 * A novel and compact 2D pose [NSRM representation](https://www.youtube.com/watch?v=Jgz1MRq-I-k#t=27s). 
 * A human body orientation classifier and an ensemble of orientation-tuned neural networks that regress the 3D human pose by also allowing for the decomposition of the body to an upper and lower kinematic hierarchy. This permits the recovery of the human pose even in the case of significant occlusions. 
 * An efficient Inverse Kinematics solver that refines the neural-network-based solution providing 3D human pose estimations that are consistent with the limb sizes of a target person (if known). 

All the above yield a 33\% accuracy improvement on the [Human 3.6 Million (H3.6M)](http://vision.imar.ro/human3.6m/description.php) dataset compared to the baseline method ([MocapNET v1](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/mnet1)) while maintaining real-time performance (70 fps in CPU-only execution).


![MocapNET](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/mnet2/doc/leedsDataset.jpg)


## Youtube Videos
------------------------------------------------------------------ 

| BMVC 2021 Supplementary Video | ICPR 2020 Poster Session |
| ------------- | ------------- |
| [![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/BMVC21YoutubeVideo.png)  ](https://www.youtube.com/watch?v=aaLOSY_p6Zc) |  [![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/ICPR2020_posterYoutubeVideoLink.png)  ](https://www.youtube.com/watch?v=mns2s4xUC7c) |

| ICPR 2020 Supplementary Video | BMVC 2019 Supplementary Video |
| ------------- | ------------- |
| [![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/youtubevideolink2.jpg)  ](https://www.youtube.com/watch?v=Jgz1MRq-I-k) | [![YouTube Link](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/youtubevideolink.jpg)  ](https://www.youtube.com/watch?v=fH5e-KMBvM0)  |

------------------------------------------------------------------ 




## Citation
------------------------------------------------------------------ 

Please cite the following papers [1](http://users.ics.forth.gr/~argyros/mypapers/2021_11_BMVC_Qammaz.pdf), [2](http://users.ics.forth.gr/~argyros/mypapers/2021_01_ICPR_Qammaz.pdf), [3](http://users.ics.forth.gr/~argyros/mypapers/2019_09_BMVC_mocapnet.pdf) according to the part of this work that helps your research :



```
@inproceedings{Qammaz2021,
  author = {Qammaz, Ammar and Argyros, Antonis A},
  title = {Towards Holistic Real-time Human 3D Pose Estimation using MocapNETs},
  booktitle = {British Machine Vision Conference (BMVC 2021)},
  publisher = {BMVA},
  year = {2021},
  month = {November},
  projects =  {I.C.HUMANS},
  videolink = {https://www.youtube.com/watch?v=aaLOSY_p6Zc}
}
```
For the BMVC21 version of MocapNET please [switch to the MNET3 branch](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/mnet3) 


```
@inproceedings{Qammaz2020,
  author = {Ammar Qammaz and Antonis A. Argyros},
  title = {Occlusion-tolerant and personalized 3D human pose estimation in RGB images},
  booktitle = {IEEE International Conference on Pattern Recognition (ICPR 2020), (to appear)},
  year = {2021},
  month = {January},
  url = {http://users.ics.forth.gr/argyros/res_mocapnet_II.html},
  projects =  {Co4Robots},
  pdflink = {http://users.ics.forth.gr/argyros/mypapers/2021_01_ICPR_Qammaz.pdf},
  videolink = {https://youtu.be/Jgz1MRq-I-k}
}
```


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
  pdflink = {http://users.ics.forth.gr/argyros/mypapers/2019_09_BMVC_mocapnet.pdf},
  videolink = {https://youtu.be/fH5e-KMBvM0}
}
```



## Overview, System Requirements and Dependencies
------------------------------------------------------------------ 
MocapNET is a high performance 2D to 3D single person pose estimator.
This code base targets recent Linux (Ubuntu  18.04 - 20.04 +) machines, and relies on the Tensorflow C-API and OpenCV. Windows 10 users can try the [linux subsystem](https://www.microsoft.com/en-us/p/ubuntu-1804-lts/9n9tngvndl3q?rtc=1&activetab=pivot:overviewtab) that has been also [reported](https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/10) to work.

Tensorflow is used as the Neural Network framework for our work and OpenCV is used to enable the acquisition of images from webcams or video files as well as to provide an easy visualization method.

We have provided an [initialization script](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/initialize.sh) that automatically handles most dependencies, as well as download all needed pretrained models. After running it the application should be ready for use. To examine the neural network .pb files provided you can [download](https://github.com/lutzroeder/netron/releases/) and use [Netron](https://github.com/lutzroeder/netron). 

Any issues not automatically resolved by the script can be reported on the [issues](https://github.com/FORTH-ModelBasedTracker/MocapNET/issues) section of this repository.

This repository contains 2D joint estimators for the [MocapNET2LiveWebcamDemo](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET2/MocapNET2LiveWebcamDemo/livedemo.cpp). By giving it the [correct parameters](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET2/MocapNETLib2/applicationLogic/parseCommandlineOptions.cpp#L117) you can switch between a cut-down version of OpenPose (--openpose), VNect (--vnect) or our own MobileNet (default) based 2D joint estimator. All of these are automatically downloaded using the [initialize.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/initialize.sh) script. However in order to achieve higher accuracy estimations you are advised to set up a full [OpenPose](https://github.com/CMU-Perceptual-Computing-Lab/openpose) instance and use it to acquire JSON files with 2D detections that can be subsequently converted to CSV using [convertOpenPoseJSONToCSV](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/master/src/MocapNET2/Converters/Openpose) and then to 3D BVH files using the [MocapNET2CSV](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/master/src/MocapNET2/MocapNETFromCSV) binary. They will provide superior accuracy compared to the bundled 2D joint detectors which are provided for faster performance in the live demo, since 2D estimation is the bottleneck of the application. Our live demo will try to run the 2D Joint estimation on your GPU and MocapNET 3D estimation on the system CPU to achieve a combined framerate of over 30 fps which in most systems matches or surpasses the acquisition rate of web cameras. Unfortunately there are many GPU compatibility issues with Tensorflow C-API builds since recent versions have dropped CUDA 9.0 support as well as compute capabilities that might be required by your system, you can edit the [initialize.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/initialize.sh) script and change the variable TENSORFLOW_VERSION according to your needs. If you want CUDA 9.0 you should se it to 1.12.0. If you want CUDA 9.0 and have a card with older compute capabilities (5.2) then choose version 1.11.0. If all else fails you can always [recompile the tensorflow C-API](https://github.com/tensorflow/tensorflow/blob/master/tensorflow/tools/lib_package/README.md) to match your specific hardware configuration. You can also [use this script that automates building tensorflow r1.15](https://github.com/AmmarkoV/MyScripts/blob/master/Tensorflow/tensorflowBuild.sh) that might help you, dealing with the Bazel build system and all of its weirdness. Release 1.15 is the final of the 1.x tensorflow tree and is compatible with MocapNET, Tensorflow 2.x is also supported, according to the [Tensorflow site, version 2.3](https://www.tensorflow.org/install/lang_c) is the first version of the 2.x tree to re-include C bindings. The [initialize.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/initialize.sh) script will ask you which version you want to use and try to download it and set it up locally for your MocapNET installation.


If you are interested in generating BVH training data for your research, we have also provided the code that handles randomization and pose perturbation from the CMU dataset. After a successful compilation, dataset generation is accessible using the scripts  [scripts/createRandomizedDataset.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/createRandomizedDataset.sh) and [scripts/createTestDataset.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/createTestDataset.sh). All BVH manipulation code is imported from a secondary [github project](https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader) that is automatically downloaded, included and built using the [initialize.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/initialize.sh) script. These [scripts/createRandomizedDataset.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/createRandomizedDataset.sh) and [scripts/createTestDataset.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/createTestDataset.sh) scripts will populate the dataset/ directory with CSV files that contain valid training samples based on the CMU dataset. It is [trivial](https://pythonspot.com/reading-csv-files-in-python/) to load these files using python. After loading them using them as training samples in conjunction with a deep learning framework like [Keras](https://keras.io/) you can facilitate learning of 2D to 3D BVH. 

## Building the library
------------------------------------------------------------------ 

To download and compile the library issue :

```
sudo apt-get install git build-essential cmake libopencv-dev libjpeg-dev libpng-dev libglew-dev libpthread-stubs0-dev

git clone https://github.com/FORTH-ModelBasedTracker/MocapNET

cd MocapNET

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

To test your OpenCV installation as well as support of your webcam issue :
```
./OpenCVTest --from /dev/video0 
```

To test OpenCV support of your video files issue :
```
./OpenCVTest --from /path/to/yourfile.mp4
```

These tests only use OpenCV (without Tensorflow or any other dependencies) and are intended as a quick method that can identify and debug configuration problems on your system.
In case of problems playing back video files or your webcam you might want to consider compiling OpenCV yourself. The [scripts/getOpenCV.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/getOpenCV.sh) script has been included to automatically fetch and make OpenCV for your convinience. The CMake file provided will automatically try to set the OpenCV_DIR variable to target the locally built version made using the script. If you are having trouble switching between the system version and the downloaded version consider using the cmake-gui utility or removing the build directory and making a fresh one, once again following the Building instructions. The new build directory should reset all paths and automatically see the local OpenCV version if you used the [scripts/getOpenCV.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/getOpenCV.sh) script and use this by default.

## Live Demo
------------------------------------------------------------------ 

Assuming that the OpenCVTest executable described previously is working correctly with your input source, to do a live test of the MocapNET library using a webcam issue :

```
./MocapNET2LiveWebcamDemo --from /dev/video0 --live
```

To dump 5000 frames from the webcam to out.bvh instead of the live directive issue :

```
./MocapNET2LiveWebcamDemo --from /dev/video0 --frames 5000
```

To control the resolution of your webcam you can use the --size width height parameter, make sure that the resolution you provide is supported by your webcam model. You can use the v4l2-ctl tool by executing it and examining your supported sensor sizes and rates. By issuing --forth you can use our FORTH developed 2D joint estimator that performs faster but offers lower accuracy

```
 v4l2-ctl --list-formats-ext
./MocapNET2LiveWebcamDemo --from /dev/video0 --live --forth --size 800 600
```


Testing the library using a pre-recorded video file (i.e. not live input) means you can use a slower but more precise 2D Joint estimation algorithm like the included OpenPose implementation. You should keep in mind that [this OpenPose implementation](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET1/MocapNETLiveWebcamDemo/utilities.cpp#L213) does not use PAFs and so it is still not as precise as  the official OpenPose implementation. To run the demo with a prerecorded file issue :

```
./MocapNET2LiveWebcamDemo --from /path/to/yourfile.mp4 --openpose
```

We have included a [video file](http://ammar.gr/mocapnet/shuffle.webm) that should be automatically downloaded by the initialize.sh script. Issuing the following command should run it and produce an out.bvh file even if you don't have any webcam or other video files available!  :

```
./MocapNET2LiveWebcamDemo --from shuffle.webm --openpose --frames 375
```


Since high-framerate output is hard to examine, if you need some more time to elaborate on the output you can use the delay flag to add programmable delays between frames. Issuing the following will add 1 second of delay after each processed frame  :

```
./MocapNET2LiveWebcamDemo --from shuffle.webm --openpose --frames 375 --delay 1000
```

If your target is a headless environment then you might consider deactivating the visualization by passing the runtime argument --novisualization. This will prevent any windows from opening and thus not cause issues even on a headless environment.

BVH output files are stored to the "out.bvh" file by default. If you want them to be stored in a different path use the -o option. They can be easily viewed using a variety of compatible applicatons. We suggest [Blender](https://www.blender.org/) which is a very powerful open-source 3D editing and animation suite or [BVHacker](https://www.bvhacker.com/) that is freeware and compatible with [Wine](https://wiki.winehq.org/)


![MocapNETLiveWebcamDemo default visualization](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/mnet2/doc/show0.jpg)

```
./MocapNET2LiveWebcamDemo --from shuffle.webm --openpose --show 0 --frames 375
```

![MocapNETLiveWebcamDemo all-in-one visualization](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/mnet2/doc/show3.jpg)

```
./MocapNET2LiveWebcamDemo --from shuffle.webm --openpose --show 3 --frames 375
```


![MocapNETLiveWebcamDemo rotation per joint visualization](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/mnet2/doc/show1.jpg)


```
./MocapNET2LiveWebcamDemo --from shuffle.webm --openpose --show 1 --frames 375
```


By using the --show variable you can alternate between different visualizations. A particularly useful visualization is the "--show 1" one that plots the joint rotations as seen above. 

![MocapNETLiveWebcamDemo OpenGL visualization](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/mnet2/doc/show0ogl.jpg)

```
./MocapNET2LiveWebcamDemo --from shuffle.webm --openpose --show 0 --opengl --frames 375
```
By executing "sudo apt-get install freeglut3-dev" to get the required libraries, then enabling the ENABLE_OPENGL CMake configuration flag during compilation and using the --opengl flag when running the MocapNET2LiveWebcamDemo you can also see the experimental OpenGL visualization illustrated above, rendering a skinned mesh that was generated using [makehuman](http://www.makehumancommunity.org/). The BVH file armature used corresponds to the [CMU+Face](http://www.makehumancommunity.org/content/cmu_plus_face.html) armature of makehuman.



```
./MocapNET2LiveWebcamDemo --from shuffle.webm --openpose --gestures --frames 375
```
By starting the live demo using the --gestures argument you can enable an experimental simple form of gesture detection as seen in the illustration above. Gestures are stored as [BVH files](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/master/dataset/gestures) and controlled through the [gestureRecognition.hpp](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET1/MocapNETLib/gestureRecognition.hpp#L18) file. A client application can register a callback as seen in the [demo](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET1/MocapNETLiveWebcamDemo/mocapNETLiveDemo.cpp#L50). The gesture detection code is experimental and has been included as a proof of concept, since due to our high-level output you can easily facilitate gesture detections by comparing subsequent BVH frames as [seen in the code](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET1/MocapNETLib/gestureRecognition.cpp#L148). That being said gestures where not a part of the original MocapNET papers.


## ROS (Robot Operating System) node 
------------------------------------------------------------------ 

[![mocapnet_rosnode screenshot with rviz](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/mocapnet_rosnode/main/doc/screenshot.jpg)](https://github.com/FORTH-ModelBasedTracker/mocapnet_rosnode)

If you are interested in [ROS](https://www.ros.org/) development and looking for a 3D pose estimator for your robot, you are in luck, MocapNET has a ROS node! You can [get it here](https://github.com/FORTH-ModelBasedTracker/mocapnet_rosnode)!



## Tuning Hierarchical Coordinate Descent for accuracy/performance gains
------------------------------------------------------------------ 

As described in the paper, the Hierarchical Coordinate Descent Inverse Kinematics algorithm has various hyper-parameters that have been set to default values after experiments. Depending on your deployment scenarios you might to sacrifice some performance for better accuracy. You can do this by altering the IK tuning parameters by using the --ik switch

A default run without the --ik switch is equivalent to a run using a learning rate of 0.01, 5 iterations, 30 epochs. The iterations variable has the biggest impact in performance.

A normal run without the --ik flag is equivalent to 

```
./MocapNET2LiveWebcamDemo --from shuffle.webm --ik 0.01 5 30
``` 

If you want a very high accuracy run and don't care about framerate as much consider 
```
./MocapNET2LiveWebcamDemo --from shuffle.webm --ik 0.01 15 40
``` 

The IK module supports tailoring the model used for posed estimation to your liking using the "--changeJointDimensions neckLength torsoLength chestWidth shoulderToElbowLength elbowToHandLength waistWidth hipToKneeLength kneeToFootLength shoeLength as well as the focal length of your specific camera using "--focalLength fx fy" The following example will try to track the shuffle.webm sample assuming a body with feet 150% the normal size and a focal length of 600 on x and y

```
./MocapNET2LiveWebcamDemo --from shuffle.webm --ik 0.01 25 40 --changeJointDimensions 1.0 1.0 1.0 1.0 1.0 1.5 1.5 1.5 1.0 --focalLength 600 600
``` 

If you don't care about fine results and just want a rough pose estimation extracted really fast you can completely switch the IK module off using  
```
./MocapNET2LiveWebcamDemo --from shuffle.webm --noik
``` 




## Headless deployment
------------------------------------------------------------------ 

When deploying the code on headless environments like [Google Colab](https://github.com/FORTH-ModelBasedTracker/MocapNET/issues/33) where there is no display available you might experience errors like
```
(3D Points Output:xxxx): Gtk-WARNING **:  cannot open display: 
```

To overcome these errors just use the --novisualization switch to disable visualization windows





## Higher accuracy with relatively little work using Mediapipe Holistic
------------------------------------------------------------------ 
To convert video files ready for use as input to MocapNET in a *relatively* easy way I have included a python converter that uses mediapipe/opencv to create the CSV files needed for MocapNET.

![MediaPipe Video 2 CSV utility](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/mediapipeConverter.jpg)

You can get mediapipe using this [src/python/mediapipe/setup.sh](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/python/mediapipe/setup.sh) script or by executing 

```
pip install --user mediapipe opencv-python
```

The converter utility receives an input video stream and creates an output directory with all image frames and the CSV file with 2D joint estimations.  

After going to the root directory of the project
```
python3 src/python/mediapipe/mediapipeHolistic2CSV.py --from shuffle.webm -o tester
```

After the conversion finishes you can process the generated "dataset" using MocapNET2CSV 

```
./MocapNET2CSV --from tester-mpdata/2dJoints_mediapipe.csv --show 3 
```
Due to the higher accuracy of [mediapipe holistic](https://google.github.io/mediapipe/solutions/holistic.html) (as well as inclusion of heads and hands which makes data forward compatible with the next versions of MocapNET) this might be a very useful tool to use in conjunction with MocapNET. In particular if you use this dumper be sure to checkout [MocapNET version 3](https://github.com/FORTH-ModelBasedTracker/MocapNET/tree/mnet3) that also supports hand pose estimation!







## Higher accuracy with more work deploying Caffe/OpenPose and using OpenPose JSON files
------------------------------------------------------------------ 

In order to get higher accuracy output compared to the live demo which is more performance oriented, you can use OpenPose and the 2D output JSON files produced by it. The convertOpenPoseJSONToCSV application can convert them to a BVH file. After downloading [OpenPose](https://github.com/CMU-Perceptual-Computing-Lab/openpose) and building it you can use it to acquire 2D JSON body pose data by running :

```
build/examples/openpose/openpose.bin -number_people_max 1 --hand --write_json /path/to/outputJSONDirectory/ -video /path/to/yourVideoFile.mp4
```

This will create files in the following fashion /path/to/outputJSONDirectory/yourVideoFile_XXXXXXXXXXXX_keypoints.json Notice that the filenames generated encode the serial number by padding it up to 12 characters (marked as X). You provide this information to our executable using the --seriallength commandline option.

The [dump_and_process_video.sh script has been included](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/dump_and_process_video.sh) that can be used to fully process a video file using openpose and then process it through MocapNET, or act as a guide for this procedure.

A utility has been included that can convert the JSON files to a single CSV file issuing :
```
 ./convertOpenPoseJSONToCSV --from /path/to/outputJSONDirectory/ --label yourVideoFile --seriallength 12 --size 1920 1080 -o .
```
For more information on how to use the conversion utility please [see the documentation inside the utility](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET2/Converters/Openpose/convertOpenPoseJSONToCSV.cpp)

A CSV file has been included that can be run by issuing : 
```
 ./MocapNET2CSV --from dataset/sample.csv --visualize --delay 30
```
The delay is added in every frame so that there is enough time for the user to see the results, of course the visualization only contains the armature since the CSV file does not have the input images.

Check out [this guide contributed by a project user](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/doc/OpenPose.md) for more info.

## Experimental utilities
------------------------------------------------------------------ 

The repository contains experimental utilities used for the development of the papers.


The CSV cluster plot utility if you choose to download the CMU-BVH dataset using the ./initialize.sh script will allow you to perform the clustering experiments described.

![CSV cluster plot utility](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/CSVClusterPlot.jpg)

```
./CSVClusterPlot
```


The BVHGUI2 is a very minimal utility you can use to become more familiar with the BVH armature used by the project. Using easy to use sliders you can animate the armature and it is [has a minimal source code](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET2/BVHGUI2/bvhGUI2.cpp).

![BVH GUI utility](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/master/doc/BVHGUI2.jpg)

```
./BVHGUI2 --opengl
```


## License
------------------------------------------------------------------ 
This library is provided under the [FORTH license](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/license.txt)

