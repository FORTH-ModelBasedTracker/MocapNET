#!/usr/bin/python3
"""
Author : "Ammar Qammaz"
Modified : "2025 – YMAPNet front-end (no MediaPipe)"
Copyright : "FORTH"
License : "FORTH"
"""

import cv2
import numpy as np
import time
import sys
import os


from holisticPartNames import guessLandmarks,normalize2DPointWhileAlsoMatchingTrainingAspectRatio

from tools    import secondsToHz, eprint, checkIfPathIsDirectory
from MocapNET import MocapNET, easyMocapNETConstructor
from MocapNETVisualization import visualizeMocapNETEnsemble


sys.path.append('RGBToPoseDetect2D/')
from PoseEstimator2D import PoseEstimator2D   # YMAPNet 2D pose estimator
from imageProcessing import castNormalizedXYCoordinatesToOriginalImage


mnet_names = [
        "head",
        "endsite_eye.l",
        "endsite_eye.r",
        "lear",
        "rear",
        "lshoulder",
        "rshoulder",
        "lelbow",
        "relbow",
        "lhand",
        "rhand",
        "lhip",
        "rhip",
        "lknee",
        "rknee",
        "lfoot",
        "rfoot"
    ]

# -------------------------------------------------------------------------
# Helper: open webcam / file / folder
# -------------------------------------------------------------------------
def getCaptureDeviceFromPath(videoFilePath, videoWidth, videoHeight, videoFramerate=30):
    """
    OpenCV / custom capture source selection
    """
    # ESP32 / other special streams
    if videoFilePath == "esp":
        from espStream import ESP32CamStreamer
        cap = ESP32CamStreamer()
        return cap

    # Folder of images
    if checkIfPathIsDirectory(videoFilePath) and ("/dev/" not in videoFilePath) and (videoFilePath != "webcam"):
        from folderStream import FolderStreamer
        cap = FolderStreamer(path=videoFilePath, width=videoWidth, height=videoHeight)
        return cap

    # Native webcam / v4l
    if videoFilePath == "webcam":
        cap = cv2.VideoCapture(0)
    elif videoFilePath == "/dev/video0":
        cap = cv2.VideoCapture(0)
    elif videoFilePath == "/dev/video1":
        cap = cv2.VideoCapture(1)
    elif videoFilePath == "/dev/video2":
        cap = cv2.VideoCapture(2)
    else:
        # Generic file path (video file)
        cap = cv2.VideoCapture(videoFilePath)

    cap.set(cv2.CAP_PROP_FPS, videoFramerate)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH,  videoWidth)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, videoHeight)
    return cap


def extract_centered_rectangle(image):
    """
    Crop the image to a centered square region.
    """
    h, w = image.shape[:2]
    side = min(h, w)
    cx, cy = w // 2, h // 2

    x1 = max(0, cx - side // 2)
    y1 = max(0, cy - side // 2)
    x2 = x1 + side
    y2 = y1 + side

    return image[y1:y2, x1:x2]


def fakeFeet(mnetPose2D):
   if mnetPose2D is not None:
        #---------------------------------------------------
        if ("2dx_lfoot" in mnetPose2D) and ("2dy_lfoot" in mnetPose2D) and ("visible_lfoot" in mnetPose2D) :
           #print("Lfoot #1")
           if (not ("2dx_endsite_toe5-3.l" in mnetPose2D)) and (not ("2dy_endsite_toe5-3.l" in mnetPose2D)) and (not ("visible_endsite_toe5-3.l" in mnetPose2D)) :
                 mnetPose2D["2dx_endsite_toe5-3.l"]     = mnetPose2D["2dx_lfoot"]
                 mnetPose2D["2dy_endsite_toe5-3.l"]     = mnetPose2D["2dy_lfoot"]
                 mnetPose2D["visible_endsite_toe5-3.l"] = mnetPose2D["visible_lfoot"]
        #---------------------------------------------------
        if ("2dx_lfoot" in mnetPose2D) and ("2dy_lfoot" in mnetPose2D) and ("visible_lfoot" in mnetPose2D) :
           ##print("Lfoot #2")
           if (not ("2dx_endsite_toe1-2.l" in mnetPose2D)) and (not ("2dy_endsite_toe1-2.l" in mnetPose2D)) and (not ("visible_endsite_toe1-2.l" in mnetPose2D)) :
                 mnetPose2D["2dx_endsite_toe1-2.l"]     = mnetPose2D["2dx_lfoot"]
                 mnetPose2D["2dy_endsite_toe1-2.l"]     = mnetPose2D["2dy_lfoot"]
                 mnetPose2D["visible_endsite_toe1-2.l"] = mnetPose2D["visible_lfoot"]
        #---------------------------------------------------
        #---------------------------------------------------
        if ("2dx_rfoot" in mnetPose2D) and ("2dy_rfoot" in mnetPose2D) and ("visible_rfoot" in mnetPose2D) :
           #print("Rfoot #1")
           if (not ("2dx_endsite_toe5-3.r" in mnetPose2D)) and (not ("2dy_endsite_toe5-3.r" in mnetPose2D)) and (not ("visible_endsite_toe5-3.r" in mnetPose2D)) :
                 mnetPose2D["2dx_endsite_toe5-3.r"]     = mnetPose2D["2dx_rfoot"]
                 mnetPose2D["2dy_endsite_toe5-3.r"]     = mnetPose2D["2dy_rfoot"]
                 mnetPose2D["visible_endsite_toe5-3.r"] = mnetPose2D["visible_rfoot"]
        #---------------------------------------------------
        if ("2dx_rfoot" in mnetPose2D) and ("2dy_rfoot" in mnetPose2D) and ("visible_rfoot" in mnetPose2D) :
           #print("Rfoot #2")
           if (not ("2dx_endsite_toe1-2.r" in mnetPose2D)) and (not ("2dy_endsite_toe1-2.r" in mnetPose2D)) and (not ("visible_endsite_toe1-2.r" in mnetPose2D)) :
                 mnetPose2D["2dx_endsite_toe1-2.r"]     = mnetPose2D["2dx_rfoot"]
                 mnetPose2D["2dy_endsite_toe1-2.r"]     = mnetPose2D["2dy_rfoot"]
                 mnetPose2D["visible_endsite_toe1-2.r"] = mnetPose2D["visible_rfoot"]
        #---------------------------------------------------


   return mnetPose2D

# -------------------------------------------------------------------------
# Main: Stream 2D poses from YMAPNet into MocapNET
# -------------------------------------------------------------------------
def streamPosesFromCameraToMocapNET():
    # Default options (simplified vs original)
    engine              = "onnx"   # MocapNET backend engine
    headless            = False
    doProfiling         = False
    multiThreaded       = False
    videoFilePath       = "webcam"
    videoWidth          = 1280
    videoHeight         = 720
    saveVideo           = False
    aspectCorrection    = 1.0
    scale               = 1.0
    addNoise            = 0.0
    doNNEveryNFrames    = 0
    frameSkip           = 0
    doHCDPostProcessing = 1
    hcdLearningRate     = 0.001
    hcdEpochs           = 30
    hcdIterations       = 10
    smoothingSampling   = 30.0
    smoothingCutoff     = 5.0
    liveDemo            = False
    alterFocalLength    = False
    calibrationFile     = ""
    fX                  = None
    fY                  = None
    centerCrop          = False

    # What parts MocapNET will output
    doBody  = True
    doFace  = False
    doREye  = False
    doMouth = False
    doHands = False

    # ---------------------------------------------------------------------
    # Command line arguments (kept minimal but compatible)
    # ---------------------------------------------------------------------
    if len(sys.argv) > 1:
        for i in range(0, len(sys.argv)):
            if sys.argv[i] == "--headless":
                headless = True
            if sys.argv[i] == "--live":
                liveDemo = True
            if sys.argv[i] == "--mt":
                multiThreaded = True
            if sys.argv[i] == "--calib":
                calibrationFile = sys.argv[i+1]
            if sys.argv[i] == "--focalLength":
                fX = float(sys.argv[i+1])
                fY = float(sys.argv[i+2])
                alterFocalLength = True
            if sys.argv[i] == "--frameskip":
                frameSkip = int(sys.argv[i+1])
            if sys.argv[i] == "--nnsubsample":
                doNNEveryNFrames = int(sys.argv[i+1])
            if sys.argv[i] == "--ik":
                hcdLearningRate = float(sys.argv[i+1])
                hcdEpochs       = int(sys.argv[i+2])
                hcdIterations   = int(sys.argv[i+3])
            if sys.argv[i] == "--smooth":
                smoothingSampling = float(sys.argv[i+1])
                smoothingCutoff   = float(sys.argv[i+2])
            if sys.argv[i] == "--noik":
                doHCDPostProcessing = 0
            if sys.argv[i] == "--aspectCorrection":
                aspectCorrection = float(sys.argv[i+1])
            if sys.argv[i] == "--noise":
                addNoise = float(sys.argv[i+1])
            if sys.argv[i] == "--size":
                videoWidth  = int(sys.argv[i+1])
                videoHeight = int(sys.argv[i+2])
            if sys.argv[i] == "--scale":
                scale = float(sys.argv[i+1])
            if sys.argv[i] == "--all":
                doBody  = True
                doFace  = True
                doREye  = True
                doMouth = True
                doHands = True
            if sys.argv[i] == "--centercrop":
                centerCrop = True
            if sys.argv[i] == "--nobody":
                doBody = False
            if sys.argv[i] == "--face":
                doFace = True
            if sys.argv[i] == "--eyes" or sys.argv[i] == "--reye":
                doREye = True
            if sys.argv[i] == "--mouth":
                doMouth = True
            if sys.argv[i] == "--hands":
                doHands = True
            if sys.argv[i] == "--save":
                saveVideo = True
            if sys.argv[i] == "--engine":
                engine = sys.argv[i+1]
                print("Selecting MocapNET engine:", engine)
            if sys.argv[i] == "--from":
                videoFilePath = sys.argv[i+1]
            if sys.argv[i] == "--profile":
                doProfiling = True

    if "/dev/video" in videoFilePath:
        smoothingSampling = 20.0
        smoothingCutoff   = 8.0
        print("Special low smoothing setup for live camera")

    # ---------------------------------------------------------------------
    # Initialize MocapNET
    # ---------------------------------------------------------------------
    print("\nInitializing MocapNET...")
    mnet = easyMocapNETConstructor(
        engine,
        doProfiling         = doProfiling,
        multiThreaded       = multiThreaded,
        doHCDPostProcessing = doHCDPostProcessing,
        hcdLearningRate     = hcdLearningRate,
        hcdEpochs           = hcdEpochs,
        hcdIterations       = hcdIterations,
        smoothingSampling   = smoothingSampling,
        smoothingCutoff     = smoothingCutoff,
        bvhScale            = scale,
        doBody              = doBody,
        doFace              = doFace,
        doREye              = doREye,
        doMouth             = doMouth,
        doHands             = doHands,
        addNoise            = addNoise
    )

    if calibrationFile != "":
        print("Enforcing Calibration file :", calibrationFile)
        mnet.bvh.configureRendererFromFile(calibrationFile)

    if alterFocalLength and fX is not None and fY is not None:
        commands = dict()
        commands["fX"] = fX
        commands["fY"] = fY
        mnet.bvh.configureRenderer(commands)

    mnet.test()
    # Record BVH to out.bvh (if not liveDemo, will be closed at end)
    mnet.recordBVH(not liveDemo)

    # ---------------------------------------------------------------------
    # Initialize YMAPNet 2D Pose Estimator
    # ---------------------------------------------------------------------
    print("\nInitializing YMAPNet 2D pose estimator (PoseEstimator2D)...")
    # These defaults match runYMAPNet.py
    pose_threshold        = 84
    pose_keypoint_thresh  = 60.0
    pose_engine           = "tensorflow"  # YMAPNet backend
    profiling_2d          = False

    estimator = PoseEstimator2D(
        modelPath='2d_pose_estimation',
        threshold=pose_threshold,
        keypoint_threshold=pose_keypoint_thresh,
        engine=pose_engine,
        profiling=profiling_2d,
        illustrate=False,
        monitor=list(),
        window_arrangement=list()
    )

    # ---------------------------------------------------------------------
    # Capture device
    # ---------------------------------------------------------------------
    cap = getCaptureDeviceFromPath(videoFilePath, videoWidth, videoHeight)

    # ---------------------------------------------------------------------
    # Main loop
    # ---------------------------------------------------------------------
    print("\nStarting MocapNET using YMAPNet 2D Joint Estimator Input")
    print("Press ESC or q to quit.\n")

    frameNumber            = 0
    maxBrokenFrames        = 100
    brokenFrames           = 0
    totalProcessingTime    = 0.0
    totalProcessingSamples = 0

    # For optional performance history plotting inside mnet
    bvhAnglesForPlotting = list()

    while cap.isOpened():
        success, image = cap.read()
    
        #Crop input to square before YMAPNet
        if (centerCrop):
           image = extract_centered_rectangle(image) #<- aspect ratio is not correct ..

        # Frame skipping
        if frameSkip > 0:
            for _ in range(0, frameSkip):
                frameNumber += 1
                success, image = cap.read()

        if not success or image is None:
            brokenFrames += 1
            eprint("Ignoring empty camera frame:", brokenFrames, "/", maxBrokenFrames)
            if brokenFrames > maxBrokenFrames:
                break
            continue

        # Reset broken frame counter on success
        brokenFrames = 0

        # Aspect ratio correction (optional)
        if aspectCorrection != 1.0:
            width  = int(image.shape[1] * aspectCorrection)
            height = int(image.shape[0])
            image  = cv2.resize(image, (width, height))

        imageClean = image.copy()

        # -----------------------------------------------------------------
        # 2D Pose Estimation via YMAPNet (PoseEstimator2D)
        # -----------------------------------------------------------------
        start_2d = time.time()

        # Run YMAPNet on current frame
        estimator.process(image)

        # If no skeletons detected, skip frame
        if len(estimator.skeletons) == 0:
            frameNumber += 1
            continue

        # This returns the exact format MocapNET expects:
        #   {'2dx_nose': ..., '2dy_nose': ..., '2dv_nose': ..., ... }
        mocapNETInput = estimator.encodeSkeletonAsDict(0, denormalize=True, keypoint_names=mnet_names)
        #print("mocapNETInput Before",mocapNETInput)
        mocapNETInput = guessLandmarks(mocapNETInput)
        mocapNETInput = fakeFeet(mocapNETInput)
        #print("\n\nmocapNETInput ",mocapNETInput)

        # -----------------------------------------------------------------
        # IMPORTANT: match MocapNET's training aspect ratio, just like the
        # MediaPipe-based pipeline used to do.
        # -----------------------------------------------------------------
        height, width = image.shape[:2]
        currentAspectRatio = width / float(height)

        for key in list(mocapNETInput.keys()):
            if key.startswith("2dx_"):
                part_name = key[4:]              # strip "2dx_"
                y_key = "2dy_" + part_name
                if y_key in mocapNETInput:
                    x = mocapNETInput[key]
                    y = mocapNETInput[y_key]
                    # Apply the same normalization MediaPipe path used
                    x2, y2 = normalize2DPointWhileAlsoMatchingTrainingAspectRatio(
                        x, y, currentAspectRatio
                    )
                    mocapNETInput[key]   = x2
                    mocapNETInput[y_key] = y2
        # -----------------------------------------------------------------

        # For visualization, draw YMAPNet annotations on a copy
        annotated_image = image.copy()
        # Do not open extra windows from PoseEstimator2D; MocapNET will handle showing
        estimator.visualize(annotated_image, show=True, save=False)

        end_2d = time.time()
        mnet.hz_2DEst = secondsToHz(end_2d - start_2d)
        mnet.history_hz_2DEst.append(mnet.hz_2DEst)
        if len(mnet.history_hz_2DEst) > mnet.perfHistorySize:
            mnet.history_hz_2DEst.pop(0)

        # -----------------------------------------------------------------
        # MocapNET 3D prediction
        # -----------------------------------------------------------------
        doNN = 1
        if doNNEveryNFrames > 0:
            doNN = (frameNumber % doNNEveryNFrames) == 0

        start_total = time.time()
        mocapNET3DOutput  = mnet.predict3DJoints(mocapNETInput, runNN=doNN, runHCD=True)
        mocapNETBVHOutput = mnet.outputBVH
        bvhAnglesForPlotting.append(mocapNETBVHOutput)
        if len(bvhAnglesForPlotting) > 100:
            bvhAnglesForPlotting.pop(0)

        # Update timing stats
        totalProcessingTime += (time.time() - start_total)
        totalProcessingSamples += 1

        # -----------------------------------------------------------------
        # Visualization
        # -----------------------------------------------------------------
        # visualizeMocapNETEnsemble returns (image, plotImage)
        imageOut, plotImage = visualizeMocapNETEnsemble(
            mnet,
            annotated_image,
            plotBVHChannels=False,
            bvhAnglesForPlotting=bvhAnglesForPlotting
        )

        frameNumber += 1

        # Print status line in terminal
        mnet.printStatus()

        # -----------------------------------------------------------------
        # Optional saving of frames
        # -----------------------------------------------------------------
        if saveVideo:
            cv2.imwrite('colorFrame_0_%05u.jpg' % (frameNumber), imageOut)

        # -----------------------------------------------------------------
        # Show window (unless headless)
        # -----------------------------------------------------------------
        if not headless:
            cv2.imshow('MocapNET 4 using YMAPNet 2D Joints', imageOut)
            key = cv2.waitKey(1) & 0xFF
            if key == 27 or key == ord('q') or key == ord('Q'):
                print("Terminating after receiving keyboard request")
                break

    # ---------------------------------------------------------------------
    # Cleanup
    # ---------------------------------------------------------------------
    cap.release()
    if not headless:
        cv2.destroyAllWindows()

    if totalProcessingSamples > 0:
        avg_ms = 1000.0 * (totalProcessingTime / totalProcessingSamples)
        avg_hz = secondsToHz(totalProcessingTime / totalProcessingSamples)
        print("Average processing time over", totalProcessingSamples, "frames:")
        print("  %0.1f ms  (%0.1f Hz)" % (avg_ms, avg_hz))
    else:
        print("No processing time statistics (no valid frames processed).")

    # Save video if requested
    if saveVideo:
        os.system(
            "ffmpeg -framerate 30 -i colorFrame_0_%%05d.jpg "
            "-s %ux%u -y -r 30 -pix_fmt yuv420p -threads 8 "
            "livelastRun3DHiRes.mp4" % (videoWidth, videoHeight)
        )

    # Destroy MocapNET object so that out.bvh is finalized
    del mnet


# -------------------------------------------------------------------------
# Entry point
# -------------------------------------------------------------------------
if __name__ == '__main__':
    streamPosesFromCameraToMocapNET()

