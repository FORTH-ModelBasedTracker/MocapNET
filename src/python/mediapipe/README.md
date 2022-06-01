
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


This is a python holistic mediapipe wrapper that can dump datasets in CSV files
https://ai.googleblog.com/2020/12/mediapipe-holistic-simultaneous-face.html
https://google.github.io/mediapipe/solutions/holistic

