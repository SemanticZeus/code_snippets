import cv2
import numpy as np

# Read images
d = "/Users/aliaminian/slideshow/jewel/44666"
img1 = cv2.imread(d+'/001.jpg', cv2.IMREAD_GRAYSCALE)
img2 = cv2.imread(d+'/002.jpg', cv2.IMREAD_GRAYSCALE)


while(True):
    cv2.imshow('frame',img1)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
# Detect features and compute descriptors
sift = cv2.SIFT_create()
keypoints1, descriptors1 = sift.detectAndCompute(img1, None)
keypoints2, descriptors2 = sift.detectAndCompute(img2, None)

# Match descriptors
bf = cv2.BFMatcher(cv2.NORM_L2, crossCheck=True)
matches = bf.match(descriptors1, descriptors2)

# Draw matches
matched_img = cv2.drawMatches(img1, keypoints1, img2, keypoints2, matches, None)
cv2.imshow('Matches', matched_img)
cv2.waitKey(0)
cv2.destroyAllWindows()
