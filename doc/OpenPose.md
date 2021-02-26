Workflow for 2-D Video Conversion to a .bvh File on Windows and Linux

	For highest speed and accuracy install OpenPose, in a Linux, Windows or Mac system, using the faster GPU version if a capable GPU is available.  Probably a graphics card with 8GB of VRAM is the minimum needed.  As described later, a Linux system will be needed to run the MocapNET utilities for making conversions to a .bvh file.

	After installing OpenPose, navigate in a terminal to wherever the openpose directory is located.  For example in Windows PowerShell:

cd D:\openpose-1.7.0-binaries-win64-gpu-python3.7-flir-3d_recommended\openpose

	Run OpenPose as shown here for Windows PowerShell:

bin\OpenPoseDemo.exe --video D:\Test4.mp4 --hand --scale_number 8 --scale_gap 0.125  --net_resolution "320x176" --write_json output_jsons/ --model_pose BODY_25B  -number_people_max 1

	The above command specifies slower, higher accuracy scale flags with body bones and additional hand bones.  Add --face to generate face bones if there is enough graphics memory.  A rather low net resolution is needed to prevent Cuda out of video memory errors.   320x176 resolution works on an 8GB VRAM RTX 2070.  If you have more video memory you might be able to set the resolution with --net_resolution -1x368 or higher.  OpenPose can process a crowd but the programs to convert its output to a .bvh file handle one person only.  The --model_pose BODY_25B flag specifies a high resolution body model that can be added as follows:

	Download the Caffe model (100 MB) from:

http://posefs1.perception.cs.cmu.edu/OpenPose/models/pose/1_25BBkg/body_25b/pose_iter_XXXXXX.caffemodel

	Then go to:

https://github.com/CMU-Perceptual-Computing-Lab/openpose_train/blob/master/experimental_models/1_25BBkg/body_25b

	Right-click on the pose_deploy.prototxt file and Save link as..., which will name it as pose_deploy.prototxt.  Save it into an openpose/models/pose/body_25b folder you create.  Also put the Caffe model you downloaded in this folder.  You can now run OpenPose with this higher accuracy model.

The output of the OpenPose run will be in the folder openpose/output_jsons containing .json files that specify the joints’ x and y positions for each frame.  For the next operation change all the .json file names using, for example, Bulk Rename Utility in Windows and rename the resulting folder with a convenient name such as BRUout.  Change sections Name (2) Fixed, and Numbering (10) Mode, Insert at 13, Start 0 and Pad 5.  The exact new names of the .json files should be:

colorFrame_0_00000_keypoints.json, colorFrame_0_00001_keypoints.json, etc.

	
Install MocapNET

	Run the MocapNET installer in an Ubuntu-like Linux system after installing the dependencies.  MocapNET has utilities to convert the OpenPose .json files into a comma-separated-values (CSV) file, which in turn will be converted into a .bvh file.  If your computer has Windows only, you can try the command-line Windows Subsystem for Linux (WSL) in newer versions of Win 10, though it won’t provide a graphical output.  Otherwise install Ubuntu as a virtual machine in VirtualBox on Windows.  Another solution is to add Ubuntu as a dual-boot option on a Windows computer.

	After installation of MocapNET copy the BRUout folder created above into the MocapNET folder.  Then in a terminal navigate to the MocapNET folder and run:

./convertOpenPoseJSONToCSV --from BRUout  --size 640 480

	The program will run, showing the progress of processing the data and will output a file stored at BRUout/2dJoints_v1.4.csv.  This CSV file can then be further processed with the following command:

./MocapNET2CSV --from BRUout/2dJoints_v1.4.csv  --size 640 480

	After completion the output will be an out.bvh file in the MocapNET folder.  This file can be loaded into Blender, etc. using an import mocap bvh option, and re-targeted to any desired armature for animation.  One good option is to use the MakeHuman program to generate a model with a matching CMU body, hands and face rig.  The MakehHuman MHX2 option provides another way to retarget the .bvh to an armature.

Note that presently the public version of MocapNET supports conversion of body bones only, but hand and face bone support is well underway.

	- end -



