import cv2
from PIL import Image
import numpy as np
import sys
from matplotlib import pyplot as plt

# Define a refined ASCII character set with 50 characters for finer detail
ASCII_CHARS = [
    "@", "#", "8", "&", "W", "M", "B", "Q", "H", "R", "D", "N", "K", "A", "S", 
    "E", "Z", "O", "X", "0", "U", "V", "Y", "C", "L", "J", "T", "I", "i", "l", 
    "!", ";", ":", "~", "-", "_", "`", "'", ".", " ", " ", " ", " ", " ", " ", 
    " ", " "
]
chars = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,                      "

ASCII_CHARS = [c for c in chars]
def apply_laplacian_filter(image_path, threshold=30):
    # Load image with OpenCV in grayscale mode
    image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    
    # Apply the Laplacian filter to enhance edges
    laplacian = cv2.Laplacian(image, cv2.CV_64F)
    abs_laplacian = np.absolute(laplacian)
    
    # Convert back to 8-bit format (0-255 range)
    enhanced_image = np.uint8(abs_laplacian)
    
    # Set pixels below the threshold to white (255)
    enhanced_image[enhanced_image < threshold] = 255
    
    return enhanced_image


def resize_image(image, new_width=100):
    width, height = image.size
    aspect_ratio = height / width
    new_height = int(aspect_ratio * new_width * 0.55)
    resized_image = image.resize((new_width, new_height))
    return resized_image

def pixels_to_ascii(image):
    # Map pixels to characters in the ASCII_CHARS based on brightness
    pixels = np.array(image).flatten()
    ascii_str = ''.join([ASCII_CHARS[pixel * len(ASCII_CHARS) // 256] for pixel in pixels])
    return ascii_str

def format_ascii(ascii_str, width):
    ascii_art = ""
    for i in range(0, len(ascii_str), width):
        ascii_art += ascii_str[i:i+width] + "\n"
    return ascii_art

def generate_ascii_art(image_path, new_width=100):
    # Apply the Laplacian filter to enhance edges
    laplacian_image = apply_laplacian_filter(image_path)
    #plt.imshow(laplacian_image)
    #plt.show()
    
    # Convert the filtered image to a Pillow image for resizing and ASCII conversion
    pil_image = Image.fromarray(laplacian_image)
    
    # Resize the image
    pil_image = resize_image(pil_image, new_width)
    
    # Convert pixels to ASCII
    ascii_str = pixels_to_ascii(pil_image)
    
    # Format ASCII output to match image width
    ascii_art = format_ascii(ascii_str, new_width)
    return ascii_art

# Specify the path to the image and generate ASCII art
ascii_art = generate_ascii_art(sys.argv[1], new_width=100)

# Output the ASCII art
if ascii_art:
    print(ascii_art)



