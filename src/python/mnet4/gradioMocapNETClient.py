import json
import time
import sys
import os
from gradio_client import Client, handle_file
import cv2  # For saving image files

# Defaults
ip = "147.52.17.119"
port = "8081"

files = []
output_file = "output.json"
image_output_dir = "output_images"

# Ensure the output directory exists
if not os.path.exists(image_output_dir):
    os.makedirs(image_output_dir)

# Parameters
threshold = 0.2
startAt = 0
hcd = True
hcd_it = 40
hcd_lr = 0.01

# Parse command line arguments
argumentStart = 1
if len(sys.argv) > 1:
    for i in range(0, len(sys.argv)):
        if sys.argv[i] == "--ip":
            ip = sys.argv[i + 1]
            argumentStart += 2
        elif sys.argv[i] == "--port":
            port = sys.argv[i + 1]
            argumentStart += 2
        elif sys.argv[i] == "--directory":
            directory = sys.argv[i + 1]
            argumentStart += 2
            if os.path.isdir(directory):
                for file in sorted(os.listdir(directory)):
                    if file.lower().endswith(('.jpg', '.jpeg', '.png')):
                        files.append(os.path.join(directory, file))
            else:
                print(f"Directory {directory} not found")
                sys.exit(1)
        elif sys.argv[i] == "--start":
            startAt = int(sys.argv[i + 1])
            argumentStart += 2
        elif sys.argv[i] in ("--output", "-o"):
            output_file = sys.argv[i + 1]
            argumentStart += 2

# Add any individual files passed as positional arguments
for i in range(argumentStart, len(sys.argv)):
    files.append(sys.argv[i])

files.sort()

if len(files) == 0:
    print("No input files provided.")
    sys.exit(0)

# Init Gradio client
client = Client(f"http://{ip}:{port}")

results = {}

# Process each image
for i in range(startAt, len(files)):
    image_path = files[i]
    print(f"\nProcessing image {i+1}/{len(files)}: {image_path}")
    start = time.time()

    try:
        result = client.predict(
            image=handle_file(image_path),
            threshold=threshold,
            hcd=hcd,
            hcd_it=hcd_it,
            hcd_lr=hcd_lr,
            history=[],
            api_name="/predict"
        )
    except Exception as e:
        print(f"Error at image {i}: {e}")
        print(f"Use --start {i} to resume later")
        break

    elapsed = time.time() - start
    remaining = (len(files) - i - 1) * elapsed
    print(f"Elapsed: {elapsed:.2f}s | Est. remaining: {remaining/60:.1f} min")

    chat_history, image_output_path = result
    text = "\n".join([f"{label}: {desc}" for label, desc in chat_history])
    print("Chat Output:\n", text)

    # Save chat as text
    base_name = os.path.splitext(os.path.basename(image_path))[0]
    chat_path = os.path.join(image_output_dir, f"{base_name}_chat.txt")
    with open(chat_path, "w") as f:
        f.write(text)
    print(f"Saved chat output: {chat_path}")

    # Save image output
    try:
        output_image = cv2.imread(image_output_path)
        if output_image is not None:
            image_save_path = os.path.join(image_output_dir, f"{base_name}_output.png")
            cv2.imwrite(image_save_path, output_image)
            print(f"Saved image output: {image_save_path}")
        else:
            print(f"Failed to load image from {image_output_path}")
    except Exception as e:
        print(f"Failed to save output image: {e}")

    # Save full result in dictionary
    results[image_path] = {
        "chat": chat_history,
        "image": image_output_path
    }

# Save full result JSON
with open(output_file, "w") as f:
    json.dump(results, f, indent=4)
    print(f"\nSaved all results to {output_file}")

