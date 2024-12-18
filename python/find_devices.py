import socket
import multiprocessing

def scan_devices(network_prefix, start_range, end_range, port):
    for i in range(start_range, end_range):
        ip = network_prefix + str(i)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(0.1)  # Set a timeout for the connection attempt
        result = sock.connect_ex((ip, port))
        if result == 0:
            print(f"SSH port {port} is open on {ip}")
        sock.close()


def scan_devices_multi(network_prefix, start_range, end_range, port):
    jobs = []
    num_proc = 100
    print(network_prefix)
    for i in range(num_proc):
        ip = network_prefix + str(i)
        start_range = int(i * 256/num_proc)
        end_range = min(int((i+1) * 256/num_proc), 255)
        process = multiprocessing.Process(target=scan_devices, args=(network_prefix,start_range, end_range, port))
        jobs.append(process)
        process.start()

    for process in jobs:
        process.join()

if __name__ == "__main__":
    net = "10.0.0."  # Adjust this according to your network
    start_range = 1
    end_range = 255
    port = 22  # SSH port for Raspberry Pi
    scan_devices_multi(net, start_range, end_range, port)

