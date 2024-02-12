import subprocess
import cv2
import numpy as np

filename = '../video.webm'

cmd = ['ffprobe', '-v', 'error', '-select_streams', 'v:0', '-show_entries', 'stream=width,height', '-of', 'csv=p=0', filename]
proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
out, _ = proc.communicate()
width, height = map(int, out.decode().strip().split(','))

cmd = ['ffmpeg', '-i', filename, '-f', 'image2pipe','-pix_fmt', 'rgb24',  '-s', f'{width}x{height}', '-vcodec', 'rawvideo', '-']
proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
frame_size = width*height*3
print(frame_size)

cv2.namedWindow('Video', cv2.WINDOW_NORMAL)
frame_num  = 1000
try:
    raw_frame = proc.stdout.read(frame_size*frame_num)
    if not raw_frame:
        exit(-1)
    frame_index = 0
    while True:
                # Decode raw frame into numpy array
        s = frame_size*frame_index
        e = frame_size*(frame_index+1)
        frame_index = (frame_index+1) % frame_num
        frame = np.frombuffer(raw_frame[s:e], dtype=np.uint8).reshape((height, width, 3))

        # Display frame using OpenCV
        cv2.imshow('Video', frame)

        # Check for ESC key press to exit
        if frame_index % 10 == 0 and cv2.waitKey(1) == 27:
            break

finally:
    # Close the FFmpeg process and OpenCV window
    proc.kill()
    cv2.destroyAllWindows()

