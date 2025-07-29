import os
from datetime import datetime
import sys
import gradio as gr
import numpy as np
import cv2
from mediapipeHolisticWebcamMocapNET import MediaPipeHolistic



#----------------------------------------------------------------------
#                  Gradio UI Translation Layer
#---------------------------------------------------------------------- 
import os
def file_exists(path):
    return os.path.exists(path)

#If a file greek exists, trigger translation
GREEK_MENU=False
if (file_exists("greek")):
  GREEK_MENU=True

menuT = dict()
#----------------------------------------------------------------------
menuT["MocapNET Server"]="Διακομιστής MocapNET"
menuT["Files"]="Αρχεία"
menuT["Image"]="Εικόνα Εισόδου"
menuT["RGB and MocapNET"]="Εικόνα Εξόδου και έξοδος MocapNET"
menuT["Threshold"]="Όριο"
menuT["HCD Iterations"]="Αριθμός επαναλήψεων αλγόριθμου ΙΚΣ"
menuT["HCD Learning Rate"]="Ρυθμός αλγορίθμου ΙΚΣ"
menuT["Do HCD"]="Χρήση αλγορίθμου ΙΚΣ"
menuT["Output"]="Έξοδος"
menuT["Process"]="Υποβολή"
#---------------------------------------------------------------------- 

def t(inputString):
 global GREEK_MENU
 if (GREEK_MENU):
  global menuT
  return menuT[inputString]
 return inputString
#---------------------------------------------------------------------- 



from MocapNETVisualization import visualizeMocapNETEnsemble
from MocapNETVisualization import drawMocapNETOutput,drawDescriptor,drawNSRM,drawMAE2DError,drawMocapNETAllPlots

  #Select a MocapNET class from tensorflow/tensorrt/onnx/tf-lite engines
from MocapNET import easyMocapNETConstructor
from tools import img_resizeWithCrop,normalizedCoordinatesAdaptForVerticalImage,normalizedCoordinatesAdaptToResizedCrop


engine           = "onnx"
headless         = False
doProfiling      = False
multiThreaded    = False
videoFilePath    = "webcam"
videoWidth       = 1280
videoHeight      = 720
saveVideo        = False
doBody           = True
doFace           = False
doREye           = False
doMouth          = False
doHands          = True
aspectCorrection = 1.0
scale            = 1.0
addNoise         = 0.0
doNNEveryNFrames = 0
frameSkip        = 0
doHCDPostProcessing = 1 #Enabled/Disabled per frame
hcdLearningRate     = 0.001
hcdEpochs           = 40
hcdIterations       = 12 
smoothingSampling   = 30.0
smoothingCutoff     = 5.0

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
mnet.test()

mp = MediaPipeHolistic(doMediapipeVisualization = False)

port="8081"
server_name="0.0.0.0"

# Visual theme
visual_theme = gr.themes.Default()  # Default, Soft or Monochrome

def log_image(frame, directory="server_log"):
    # Ensure the directory exists
    if not os.path.exists(directory):
        os.makedirs(directory)
    
    # Generate a timestamp for the filename
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    # Create the filename
    filename = f"{timestamp}.png"  # You can change the extension as needed
    
    # Create the full path
    filepath = os.path.join(directory, filename)
    
    # Save the image
    cv2.imwrite(filepath, frame)
    print(f"Image saved at: {filepath}")
 

# Function to process the image and generate a description
def describe_image(image, threshold, hcd, hcd_it,  hcd_lr, history):
    log_image(image)

    mnet.smoothingSampling = 0 #Disable smoothing for gradio server
    mnet.smoothingCutoff   = 0 #Disable smoothing for gradio server
    mnet.hcdIterations   = hcd_it
    mnet.hcdLearningRate = hcd_lr
    sourceWidth  = image.shape[1]
    sourceHeight = image.shape[0] 
    currentAspectRatio = sourceWidth/sourceHeight
    trainedAspectRatio = 1920/1080
    #image = img_resizeWithCrop(image,1920,1080) 

    #if ( aspectCorrection!=1.0 ):
    #   width  = int(image.shape[1]*aspectCorrection)
    #   height = int(image.shape[0])
    #   image  = cv2.resize(image, (width,height))

    # Initialize cleaned_output variable
    cleaned_output = "" 
    # Preprocess the input image as per the model's requirements
    frame = image.copy()# cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

    mocapNETInput,annotated_image = mp.convertImageToMocapNETInput(frame) 

    #print("HCD : ",hcd)
    mocapNET3DOutput  = mnet.predict3DJoints(mocapNETInput,runNN=1,runHCD=hcd) 
    mocapNETBVHOutput = mnet.outputBVH
    mnet.bvh.eraseHistory()
    #--------------------------------------------------------------------------------------------------------------
    mocapnet_image,plotImage = visualizeMocapNETEnsemble(mnet,annotated_image,plotBVHChannels=0)
        
    # Append the new conversation to the history
    history=[]
    history.append(("2D",str(mocapNETInput)))
    history.append(("3D",str(mocapNET3DOutput)))
    history.append(("BVH",str(mnet.outputBVH))) 
    if (currentAspectRatio!=trainedAspectRatio):
      history.append(("Warning","For best results please use a 1920x1080 image with the person > 1m away from the camera")) 
       

    return history, mocapnet_image

# Function to clear the chat history
def clear_chat():
    return []

# Gradio Interface
def gradio_interface():
    with gr.Blocks(visual_theme) as demo:
        gr.HTML(
        """
    <h1 style='text-align: center'>
    %s
    </h1>
    """ % t("MocapNET Server") )
        with gr.Row():
            # Left column with image and parameter inputs
            with gr.Column(scale=1):
                image_input = gr.Image(
                    label=t("Image"), 
                    type="numpy", 
                    image_mode="RGB", 
                    height=720,  # Set the height
                    width=1280   # Set the width
                )

                # Parameter sliders
                threshold = gr.Slider(label=t("Threshold"), minimum=0.1, maximum=2.0, value=0.2, step=0.05, interactive=True)
                hcd_it = gr.Slider(label=t("HCD Iterations"), minimum=0, maximum=100, value=40, step=1, interactive=True)
                hcd_lr = gr.Slider(label=t("HCD Learning Rate"), minimum=0.001, maximum=0.1, value=0.01, step=0.01, interactive=True)
                hcd = gr.Checkbox(label=t("Do HCD"), value=True)
                generate_button = gr.Button(t("Process")) 

            # Right column with the interface
            with gr.Column(scale=2):
                #rgb_2d_output = gr.Image(label="RGB and 2D Pose", height=720, width=1280)  
                mnet_output   = gr.Image(label=t("RGB and MocapNET"), height=720, width=1280)  
                chat_history  = gr.Chatbot(label=t("Output"), height=400)
               
            # Define the action for the generate button
            generate_button.click(
                    fn=describe_image, 
                    inputs=[image_input, threshold, hcd, hcd_it, hcd_lr, chat_history],
                    outputs=[chat_history, mnet_output],
                    api_name="predict"
                )

    return demo

# Launch the interface
demo = gradio_interface()
demo.launch(favicon_path="favicon.ico",server_name=server_name, server_port=int(port))

