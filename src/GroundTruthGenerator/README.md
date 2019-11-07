To simplify package maintenance the GroundTruthGenerator uses the code from the BVHTester of RGBDAcquisition
If you want to browse the code, i.e. to find about different command line options this is the [main.c](https://github.com/AmmarkoV/RGBDAcquisition/blob/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Applications/BVHTester/main.c)

All the implementation files to parse BVH files are part of the MotionCaptureLoader module of the RGDAcquisition OGL Renderer Sandbox. You can [find the bvh code here](https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader)

Sorry for having different stuff  in different places but it really minimizes the effort needed to relase the code and it also reduces the number of possible bugs from keeping the same code in sync in different machines etc. 
