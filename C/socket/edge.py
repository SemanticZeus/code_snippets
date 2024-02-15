import cv2

# Function to perform edge detection
def detect_edges(frame):
    # Convert frame to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # Apply Gaussian blur to reduce noise and improve edge detection
    #blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    # Perform Canny edge detection
    edges = cv2.Canny(blurred, 50, 150)
    return edges

# Main function
def main():
    # Initialize the webcam
    cap = cv2.VideoCapture(0)

    # Check if the webcam is opened successfully
    if not cap.isOpened():
        print("Error: Could not open webcam.")
        return

    while True:
        # Read frame from the webcam
        ret, frame = cap.read()
        if not ret:
            print("Error: Failed to capture frame.")
            break

        # Perform edge detection on the frame
        edges = detect_edges(frame)

        # Display the original and edge-detected frames
        cv2.imshow("Original", frame)
        cv2.imshow("Edges", edges)

        # Break the loop when 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Release the webcam and close all windows
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
