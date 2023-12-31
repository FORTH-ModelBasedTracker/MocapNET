import sys
import cv2
import numpy as np
import tensorflow as tf
from tensorflow.keras.models import load_model
from readCOCO import resize_image_with_borders


dataType =  np.uint8 #

useGPU = True
if (len(sys.argv)>1):
       #print('Argument List:', str(sys.argv))
       for i in range(0, len(sys.argv)):
           if (sys.argv[i]=="--float32"):
             dataType   = np.float32
           if (sys.argv[i]=="--uint8"):
             dataType = np.uint8
           if (sys.argv[i]=="--cpu"):
             useGPU = False

# Set CUDA_VISIBLE_DEVICES to an empty string to force TensorFlow to use the CPU
if (not useGPU):
     os.environ['CUDA_VISIBLE_DEVICES'] = '' #<- Force CPU

def load_keypoints_model(model_path):
    from readCOCO import PCKMetric,RSquaredMetric,FocalLoss,focal_loss,JointsMSELoss,jointsMSE_loss,VanillaMSELoss,vanilla_mse_loss
    model = load_model(model_path, custom_objects={'PCKMetric': PCKMetric, 'RSquaredMetric':RSquaredMetric, 'focal_loss': focal_loss, 'jointsMSE_loss': jointsMSE_loss, 'vanilla_mse_loss':vanilla_mse_loss})

    # Check the shape of the input layer
    input_layer = model.layers[0]  # Assuming the input layer is the first layer
    input_shape = input_layer.input_shape
    input_size = (input_shape[0][1],input_shape[0][2])
    print("Input shape is ",input_size)

    # Get the output layer of the model
    output_layer = model.layers[-1]  # Assuming the output layer is the last layer
    output_shape = output_layer.output_shape
    output_size = (output_shape[2],output_shape[3])
    numberOfHeatmaps = output_shape[1]
    print("Number of Heatmaps is ", numberOfHeatmaps)
    print("Output Shape is ", output_size)

    return model,input_size,output_size,numberOfHeatmaps

def preprocess_image(frame, target_size=(128, 128)):
    # Resize the frame to the target size and normalize pixel values
    # Step 1: Convert the image to float32
    image = frame.astype(dataType)
    
    #print("Input Image Range:", np.min(image), np.max(image))
    #print("Input Image Mean:", np.mean(image))
    #print("Input Image STD:", np.std(image))

    # Step 2: Normalize the pixel values to be in the range [0, 1]
    #image = image / 255
    image, keypointXMultiplier, keypointYMultiplier, keypointXOffset, keypointYOffset  = resize_image_with_borders(image,target_size)

    return image

def predict_keypoints(model, image): 
    # Use the model call for predictions
    image_batch = np.expand_dims(image, axis=0)
    predictions = model(image_batch, training=False)
    return predictions[0]


def visualize_heatmaps(frame, frameNumber, heatmaps, keypoint_names, threshold=0.0):
    i=0 
    wnd_x = 0
    wnd_y = 0

    #Scale back window
    #rgb_uint8_image = frame * 255
    rgb_uint8_image = frame

    # Convert to uint8 type for display
    rgb_uint8_image = np.uint8(rgb_uint8_image)

    # Display the result
    cv2.imshow('RGB Input', rgb_uint8_image)
    if (frameNumber==0):
       cv2.moveWindow("RGB Input", wnd_x, wnd_y)
       wnd_y+=231


    #print("Heatmaps ",heatmaps.shape)
    for i in range(0,heatmaps.shape[2]):
        heatmap         =  heatmaps[:,:,i]
        resized_heatmap =  np.array(heatmap,dtype=dataType)
  
        if (threshold>0.0):
            # Create a boolean mask for values above the threshold
            resized_heatmap[resized_heatmap <= threshold] = 0

        #heatmap_uint8   = np.uint8(resized_heatmap) 
        #print("Heatmap ",i," shape ",heatmap.shape)
        cv2.imshow('Heatmap  %s'% keypoint_names[i], resized_heatmap)
        if (frameNumber==0):
          cv2.moveWindow('Heatmap  %s'% keypoint_names[i], wnd_x, wnd_y) 
          wnd_y+=170
          if ( wnd_y > 900 ):
            wnd_x+=384
            wnd_y =0


        i=i+1
 
    cv2.waitKey(1)

def webcam_keypoints_detection(model_path, keypoint_names, threshold=0.0):
    # Load the 2D Pose Estimation model
    keypoints_model,input_size,output_size,numberOfHeatmaps = load_keypoints_model(model_path)

    # Open a connection to the webcam (0 indicates the default camera)
    cap = cv2.VideoCapture(0)

    frameNumber = 0

    while True:
        # Capture a single frame from the webcam
        ret, frame = cap.read()
        if not ret:
            print("Failed to capture frame")
            break
  
        # Preprocess the frame for the model
        input_image = preprocess_image(frame, target_size=input_size)

        # Make predictions using the model
        keypoints_predictions = predict_keypoints(keypoints_model, input_image)

        #i=0
        #for keypoint in keypoints_predictions:
        #  print("Prediction ",keypoint_names[i]," Range:", np.min(keypoint), np.max(keypoint))
        #  print("Prediction ",keypoint_names[i]," Mean:", np.mean(keypoint))
        #  print("Prediction ",keypoint_names[i]," STD:", np.std(keypoint))
        #  i=i+1

        # Visualize the heatmaps on the source image
        visualize_heatmaps(input_image,frameNumber, keypoints_predictions, keypoint_names, threshold=threshold)
        frameNumber = frameNumber + 1

    # Release the webcam and close all OpenCV windows
    cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    # Specify the path to the trained 2D Pose Estimation model
    model_path = '2d_pose_estimation'

    # Specify the names of keypoints (change accordingly based on your model's keypoint order)
    keypoint_names = [ 
                      "nose",
                      "left_eye",
                      "right_eye",
                      "left_ear",
                      "right_ear",
                      "left_shoulder",
                      "right_shoulder",
                      "left_elbow",
                      "right_elbow",
                      "left_wrist",
                      "right_wrist",
                      "left_hip",
                      "right_hip",
                      "left_knee",
                      "right_knee",
                      "left_ankle",
                      "right_ankle",
                      "bkg"
                     ]

    print("Reported Keypoints : ",keypoint_names)
    # Run the webcam keypoints detection
    webcam_keypoints_detection(model_path, keypoint_names, threshold = 0.0)

