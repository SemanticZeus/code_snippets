#!/usr/bin/env python
import os

# Define the path to the USB port you want to control
usb_port_path = "/sys/bus/usb/devices/usb1/power/control"  # Replace usbX with your USB port's directory

# Write "auto" or "on" to enable power, "suspend" to disable power
with open(usb_port_path, "w") as f:
    f.write("suspend")  
