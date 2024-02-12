import os
import cv2

cam = cv2.VideoCapture(0)
cam.set(cv2.CAP_PROP_FRAME_WIDTH,  1920000) #2594)
cam.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080000) #1944)
#turn the light on
ret, image = cam.read()
cv2.imwrite('test.jpg', image)
