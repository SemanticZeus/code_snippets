from svglib.svglib import svg2rlg
from reportlab.graphics import renderPM
from PIL import Image
import matplotlib.pyplot as plt

def display_svg(svg_file):
    # Convert SVG to a ReportLab drawing object
    drawing = svg2rlg(svg_file)
    
    # Render the drawing to a PIL image
    png_image = renderPM.drawToPIL(drawing)
    
    # Display the image using matplotlib
    plt.imshow(png_image)
    plt.axis('off')  # Hide the axes
    plt.show()

# Path to your SVG file
svg_file_path = "camera.svg"

# Display the SVG
display_svg(svg_file_path)

