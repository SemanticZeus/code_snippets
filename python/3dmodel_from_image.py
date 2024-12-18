import torch
import cv2
import numpy as np
from PIL import Image

# Load and preprocess the input image
input_image = cv2.imread('/Users/aliaminian/slideshow/jewel/44666/001.jpg')
transform = torch.hub.load("intel-isl/MiDaS", "transforms").dpt_transform
input_tensor = transform(input_image).unsqueeze(0)

# Load the MiDaS model
model_type = "DPT_Large"  # or "DPT_Hybrid"
midas = torch.hub.load("intel-isl/MiDaS", model_type, pretrained=True)
midas.eval()

# Convert the input image to a NumPy array and normalize it
input_image = np.array(input_image) / 255.0  # Normalize to [0, 1]
input_image = Image.fromarray((input_image * 255).astype(np.uint8))  # Convert back to uint8 for processing

# Apply the transform
transform = torch.hub.load("intel-isl/MiDaS", "transforms").dpt_transform
input_tensor = transform(input_image).unsqueeze(0)

# Estimate depth
with torch.no_grad():
    depth_map = midas(input_tensor)

# Convert depth map to NumPy array
depth_map = depth_map.squeeze().cpu().numpy()

# Normalize the depth map
depth_map = (depth_map - depth_map.min()) / (depth_map.max() - depth_map.min()) * 255
depth_map = depth_map.astype(np.uint8)

# Display depth map
cv2.imshow("Depth Map", depth_map)
cv2.waitKey(0)
cv2.destroyAllWindows()

