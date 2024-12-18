import serial

# Define the serial port and baud rate
ser = serial.Serial('/dev/cu.usbserial-A5XK3RJT', 115200)  # Update '/dev/ttyUSB0' with your serial port

try:
    while True:
        # Read a line from the serial port
        line = ser.readline()
        
        # Decode the line from bytes to string using the appropriate encoding
        decoded_line = line.decode('utf-8').strip()  # Adjust the encoding if necessary
        
        # Print the decoded line
        print(decoded_line)

except KeyboardInterrupt:
    print("Keyboard Interrupt. Exiting...")

finally:
    # Close the serial port
    ser.close()

