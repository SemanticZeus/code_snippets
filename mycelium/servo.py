from gpiozero import Servo
from time import sleep

servo = Servo(18)

try:
    while True:
        servo.min()
except KeyboardInterrupt:
    print("Program stopped")
