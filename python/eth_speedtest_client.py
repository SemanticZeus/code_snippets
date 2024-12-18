import socket
import time

# Server address and port
SERVER_IP = '169.254.212.170'  # Replace with your Raspberry Pi's IP address
SERVER_PORT = 5000
BUFFER_SIZE = 1024 * 1024  # 1 MB

# Create a socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((SERVER_IP, SERVER_PORT))

# Generate 1 MB of random data
data = bytearray(BUFFER_SIZE)

start_time = time.time()
client_socket.sendall(data)
response = b''
while len(response) < len(data):
    response += client_socket.recv(BUFFER_SIZE)
end_time = time.time()

elapsed_time = end_time - start_time
data_size_mb = len(response) / (1024 * 1024)
speed_mbps = data_size_mb / elapsed_time * 8

print(f"Data size: {data_size_mb:.2f} MB")
print(f"Elapsed time: {elapsed_time:.4f} seconds")
print(f"Speed: {speed_mbps:.2f} Mbps")

client_socket.close()

