# MocapNET Project


![MocapNET](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/mnet4/doc/method.png)

Finishing my PhD this will probably be the *final* version of MocapNET!
MocapNET 4 will deal with upperbody / lowerbody / hands / eye tracking and / facial capture
It has a written from scratch python interface, but maintain the same compatible BVH output format.
It will also be compatible with Raspberry Pi 4 and use Tensorflow /Tf-Lite / ONNX backends
  
This branch is still under construction, and has been ported to Python to boost usability
so if you want the older C/C++ version of MocapNET you ignore it for now..!




![MocapNET](https://raw.githubusercontent.com/FORTH-ModelBasedTracker/MocapNET/mnet4/doc/ICCV2023_Presentation_Slide18.png)



## Deploy it now on Google Colab with a single click!
------------------------------------------------------------------ 

Click here for one click setup : [![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/FORTH-ModelBasedTracker/MocapNET/blob/mnet4/mocapnet4.ipynb)




## Relevant publications!
------------------------------------------------------------------ 


| Download Paper | Year            |    Conference |     Title     | 
| -------------  | -------------   | ------------- | ------------- |
| [![A Unified Approach for Occlusion Tolerant 3D Facial Pose Capture and Gaze Estimation using MocapNETs](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/mnet4/doc/pdf.png?raw=true)](http://users.ics.forth.gr/~argyros/mypapers/2023_10_AMFG_Qammaz.pdf) | 2023 | AMFG@ICCV | A Unified Approach for Occlusion Tolerant 3D Facial Pose Capture and Gaze Estimation using MocapNETs | 
| [![Compacting MocapNET-based 3D Human Pose Estimation via Dimensionality Reduction](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/mnet4/doc/pdf.png?raw=true)](http://users.ics.forth.gr/~argyros/mypapers/2023_07_PETRA_Qammaz.pdf) | 2023 | PeTRA | Compacting MocapNET-based 3D Human Pose Estimation via Dimensionality Reduction | 
| [![Towards Holistic Real-time Human 3D Pose Estimation using MocapNETs](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/mnet4/doc/pdf.png?raw=true)](http://users.ics.forth.gr/~argyros/mypapers/2021_11_BMVC_Qammaz.pdf) | 2021 | BMVC | Towards Holistic Real-time Human 3D Pose Estimation using MocapNETs | 
| [![Occlusion-tolerant and personalized 3D human pose estimation in RGB images](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/mnet4/doc/pdf.png?raw=true)](http://users.ics.forth.gr/~argyros/mypapers/2021_01_ICPR_Qammaz.pdf) | 2021 | ICPR | Occlusion-tolerant and personalized 3D human pose estimation in RGB images | 
| [![MocapNET: Ensemble of SNN Encoders for 3D Human Pose Estimation in RGB Images](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/mnet4/doc/pdf.png?raw=true)](http://users.ics.forth.gr/~argyros/mypapers/2019_09_BMVC_mocapnet.pdf) | 2019 | BMVC | MocapNET: Ensemble of SNN Encoders for 3D Human Pose Estimation in RGB Images | 








## AMFG@ICCV 2023 Poster
------------------------------------------------------------------ 

![Our Poster in the Analysis and Modeling of Faces and Gestures Workshop @ ICCV 2023 ](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/mnet4/doc/ICCV2023_MocapNET4_poster.png?raw=true)







## Citation
------------------------------------------------------------------ 

Please cite the following papers if this work helps your research : 
``` 
@inproceedings{Qammaz2023b,
  author = {Qammaz, Ammar and Argyros, Antonis},
  title = {A Unified Approach for Occlusion Tolerant 3D Facial Pose Capture and Gaze Estimation using MocapNETs},
  booktitle = {International Conference on Computer Vision Workshops (AMFG 2023 - ICCVW 2023), (to appear)},
  publisher = {IEEE},
  year = {2023},
  month = {October},
  address = {Paris, France},
  projects =  {VMWARE,I.C.HUMANS},
  pdflink = {http://users.ics.forth.gr/ argyros/mypapers/2023_10_AMFG_Qammaz.pdf}
}

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




## License
------------------------------------------------------------------ 
This library is provided under the [FORTH license](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/license.txt)

