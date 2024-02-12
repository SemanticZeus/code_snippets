import socket

def get_ip_address():
    try:
        # Create a socket object
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        # Connect to a remote server (doesn't matter which server)
        s.connect(("8.8.8.8", 80))
        
        # Get the local IP address
        ip_address = s.getsockname()[0]
        
        return ip_address
    except socket.error as e:
        print(f"Error: {e}")
    finally:
        s.close()

# Get and print the IP address of the current machine
print("Current IP Address:", get_ip_address())

