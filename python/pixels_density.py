from PIL import Image, ImageDraw, ImageFont
import string

def get_pixel_count(char, font_path="arial.ttf", font_size=100):
    # Create an image with transparency to place the character on
    image = Image.new("L", (font_size, font_size), 0)  # "L" mode for grayscale
    draw = ImageDraw.Draw(image)
    
    # Load the font
    try:
        font = ImageFont.truetype(font_path, font_size)
    except IOError:
        font = ImageFont.load_default()
    
    # Get the bounding box of the character and position it in the center
    bbox = draw.textbbox((0, 0), char, font=font)
    text_position = ((font_size - (bbox[2] - bbox[0])) // 2, (font_size - (bbox[3] - bbox[1])) // 2)
    
    # Draw the character on the image
    draw.text(text_position, char, fill=255, font=font)
    
    # Count non-background pixels
    pixel_count = sum(image.getdata()) // 255  # Normalize pixel values to 0 or 1
    return pixel_count

def order_characters_by_pixels(characters, font_path="arial.ttf", font_size=100):
    # Calculate pixel count for each character
    pixel_counts = [(char, get_pixel_count(char, font_path, font_size)) for char in characters]
    
    # Sort characters by pixel count in ascending order
    pixel_counts.sort(key=lambda x: x[1])
    return pixel_counts

# Define a set of characters to evaluate
characters = string.ascii_letters + string.digits  # or any other characters

# Order characters by pixel density
sorted_characters = order_characters_by_pixels(characters)

# Print sorted characters and their pixel counts
for char, pixel_count in sorted_characters:
    print(f"Character: '{char}' - Pixels: {pixel_count}")

