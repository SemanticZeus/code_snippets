import cv2

webcam = cv2.VideoCapture(0)

c = 1

for i in range(100):
	ret, frame = webcam.read()
	cv2.imwrite('image{}.jpg'.format(i), frame)

cv2.destroyAllWindows()
