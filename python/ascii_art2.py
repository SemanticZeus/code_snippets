import cv2
import numpy as np
from PIL import Image
import sys
# Define a Unicode character set for richer ASCII art
UNICODE_CHARS = [
    "█", "▓", "▒", "░", "＃", "田", "田", "由", "甲", "目", "日", "白", "Π", "π", "Σ", "Ω", 
    "ζ", "λ", "θ", "α", "β", "∑", "∂", "∫", "Ф", "Λ", "Ψ", "σ", "ω", "μ", "Χ", "ψ", "π", 
    "あ", "い", "う", "え", "お", "か", "き", "く", "け", "こ", "し", "す", "せ", "そ", 
    "ね", "の", "は", "ひ", "ふ", "へ", "ほ"
]

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

def pixels_to_unicode(image):
    # Map pixels to Unicode characters based on brightness
    pixels = np.array(image).flatten()
    unicode_str = ''.join([UNICODE_CHARS[pixel * len(UNICODE_CHARS) // 256] for pixel in pixels])
    return unicode_str

def format_unicode_with_color(unicode_str, width):
    # ANSI color codes for RGB
    def rgb_to_ansi(r, g, b):
        return f"\033[38;2;{r};{g};{b}m"

    colored_art = ""
    for i in range(0, len(unicode_str), width):
        line = unicode_str[i:i+width]
        for char in line:
            # Generate color based on character "brightness" for variety
            brightness = int(255 * UNICODE_CHARS.index(char) / (len(UNICODE_CHARS) - 1))
            color_code = rgb_to_ansi(brightness, brightness, brightness)
            colored_art += f"{color_code}{char}\033[0m"
        colored_art += "\n"
    return colored_art

def generate_unicode_art(image_path, new_width=100):
    # Apply the Laplacian filter with white background
    laplacian_image = apply_laplacian_filter(image_path, threshold=30)
    
    # Convert the filtered image to a Pillow image for resizing and ASCII conversion
    pil_image = Image.fromarray(laplacian_image)
    
    # Resize the image
    pil_image = resize_image(pil_image, new_width)
    
    # Convert pixels to Unicode
    unicode_str = pixels_to_unicode(pil_image)
    
    # Format Unicode output with color
    colored_art = format_unicode_with_color(unicode_str, new_width)
    return colored_art

# Specify the path to the image and generate Unicode art with color
unicode_art = generate_unicode_art(sys.argv[1], new_width=100)

# Output the colored Unicode art
if unicode_art:
    print(unicode_art)

