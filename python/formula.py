import numpy as np
import cv2
from matplotlib.figure import Figure
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas

# Video properties
fps = 30
duration = 5  # seconds
frame_count = fps * duration
width, height = 800, 600

# Bounce properties
amplitude = 100  # amplitude of the bounce
mid_y = height // 2

# Create a video writer
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
video = cv2.VideoWriter('bounce_formula.mp4', fourcc, fps, (width, height))

# Generate frames
for frame in range(frame_count):
    fig = Figure(figsize=(width / 100, height / 100), dpi=100)
    canvas = FigureCanvas(fig)
    ax = fig.add_subplot(111)

    # Calculate vertical position
    y = mid_y + amplitude * np.sin(2 * np.pi * (frame / fps))
    
    # Add LaTeX-rendered text to the plot
    ax.text(400, y, '$E=mc^2$', fontsize=16, ha='center', va='center')
    ax.set_xlim(0, width)
    ax.set_ylim(0, height)
    ax.axis('off')

    # Convert Matplotlib figure to an OpenCV image
    canvas.draw()
    img = np.frombuffer(canvas.tostring_rgb(), dtype='uint8').reshape(height, width, 3)
    img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)

    # Write frame to video
    video.write(img)

    # Clear the figure to reuse it for the next frame
    fig.clear()

# Release the video writer
video.release()
cv2.destroyAllWindows()
