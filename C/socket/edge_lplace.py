import cv2

# Function to perform edge detection using Laplacian operator
def detect_edges_laplacian(frame, threshold):
    # Convert frame to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # Apply Gaussian blur to reduce noise
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    # Apply Laplacian operator
    laplacian = cv2.Laplacian(blurred, cv2.CV_64F)
    # Convert the result back to uint8
    laplacian = cv2.convertScaleAbs(laplacian)
    laplacian = cv2.threshold(abs(laplacian), threshold, 255, cv2.THRESH_BINARY)[1]
    return laplacian

# Main function
def main():
    # Initialize the webcam
    cap = cv2.VideoCapture(0)

    # Check if the webcam is opened successfully
    if not cap.isOpened():
        print("Error: Could not open webcam.")
        return

    threshold = 100
    while True:
        # Read frame from the webcam
        ret, frame = cap.read()
        if not ret:
            print("Error: Failed to capture frame.")
            break

        # Perform edge detection using Laplacian operator
        edges_laplacian = detect_edges_laplacian(frame, threshold)

        # Display the original and edge-detected frames
        cv2.imshow("Original", frame)
        cv2.imshow("Edges (Laplacian)", edges_laplacian)
        # Break the loop when 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        if cv2.waitKey(1) & 0xFF == ord('u'):
            threshold += 10
        if cv2.waitKey(1) & 0xFF == ord('j'):
            threshold -=10
    # Release the webcam and close all windows
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()

